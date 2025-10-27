// Minimal Linux implementation using POSIX sockets + background threads
#include "sdnet.h"
#include "sdnetopt.h"
#include "sdnet_ver.h"
#include "sdnetutils.h"

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

namespace SSCP {

enum class NetEventType { Established, Terminated, Error, Recv };
struct NetEvent { NetEventType type; class Connection* conn; std::string data; int modErr{0}; int sysErr{0}; };

class Connection : public ISSConnection {
public:
    Connection():_sock(-1),_connected(false),_parser(nullptr),_session(nullptr) { _remoteIpStr[0]=0; _localIpStr[0]=0; }
    ~Connection() override { Disconnect(); }
    bool SSAPI IsConnected(void) override { return _connected.load(); }
    void SSAPI Send(const char* pBuf,UINT32 dwLen) override {
        if (!_connected.load() || !pBuf || dwLen==0) return; std::lock_guard<std::mutex> lk(_sendMtx);
        ::send(_sock, pBuf, dwLen, 0);
    }
    void SSAPI DelaySend(const char* pBuf,UINT32 dwLen) override { Send(pBuf, dwLen); }
    void SSAPI SetOpt(UINT32, void*) override {}
    void SSAPI Disconnect(void) override { _connected.store(false); if (_sock!=-1) { ::shutdown(_sock, SHUT_RDWR); ::close(_sock); _sock=-1; } }
    const UINT32 SSAPI GetRemoteIP(void) override { return _remoteIp; }
    const char* SSAPI GetRemoteIPStr(void) override { return _remoteIpStr; }
    UINT16 SSAPI GetRemotePort(void) override { return _remotePort; }
    const UINT32 SSAPI GetLocalIP(void) override { return _localIp; }
    const char* SSAPI GetLocalIPStr(void) override { return _localIpStr; }
    UINT16 SSAPI GetLocalPort(void) override { return _localPort; }
    UINT32 SSAPI GetSendBufFree(void) override { return 0xFFFFFFFF; }

    void attach(int s, sockaddr_in local, sockaddr_in remote) {
        _sock = s; _connected.store(true);
        _localIp = local.sin_addr.s_addr; _localPort = ntohs(local.sin_port);
        _remoteIp = remote.sin_addr.s_addr; _remotePort = ntohs(remote.sin_port);
        std::strncpy(_localIpStr, SDInetNtoa(_localIp), sizeof(_localIpStr)-1);
        std::strncpy(_remoteIpStr, SDInetNtoa(_remoteIp), sizeof(_remoteIpStr)-1);
        if (_session) _session->SetConnection(this);
        startRecvThread();
    }
    void setParser(ISSPacketParser* p) { _parser = p; }
    void setSession(ISSSession* s) { _session = s; }
    void setQueue(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv) { _eq=q; _eqmtx=m; _eqcv=cv; }

    void enqueueRecv(const char* data, size_t len) {
        if (!_parser) { postEvent(NetEvent{NetEventType::Recv, this, std::string(data, len)}); return; }
        _accum.append(data, data+len);
        for (;;) {
            int want = _parser->ParsePacket(_accum.data(), static_cast<UINT32>(_accum.size()));
            if (want < 0) { postEvent(NetEvent{NetEventType::Error, this, std::string(), NET_PACKET_ERROR, 0}); _accum.clear(); break; }
            if (want == 0) break;
            if (static_cast<size_t>(want) <= _accum.size()) {
                postEvent(NetEvent{NetEventType::Recv, this, std::string(_accum.data(), _accum.data()+want)});
                _accum.erase(0, static_cast<size_t>(want));
            } else break;
        }
    }
    void onEstablished() { if (_session) _session->OnEstablish(); }
    void onTerminated() { if (_session) _session->OnTerminate(); releaseSession(); }
    void onRecv(const std::string& buf) { if (_session) _session->OnRecv(buf.data(), static_cast<UINT32>(buf.size())); }
    void onError(int modErr, int sysErr) { if (_session) _session->OnError(modErr, sysErr); }
    void releaseSession() { if (_session) { _session->Release(); _session=nullptr; } }

private:
    void startRecvThread() {
        _recvThread = std::thread([this](){
            char buf[8192];
            while (_connected.load()) {
                ssize_t n = ::recv(_sock, buf, sizeof(buf), 0);
                if (n > 0) { enqueueRecv(buf, static_cast<size_t>(n)); }
                else if (n == 0) { postEvent(NetEvent{NetEventType::Terminated, this}); _connected.store(false); break; }
                else { postEvent(NetEvent{NetEventType::Error, this, std::string(), NET_RECV_ERROR, errno}); _connected.store(false); break; }
            }
        });
        _recvThread.detach();
    }
    void postEvent(const NetEvent& ev) { if (!_eq || !_eqmtx) return; { std::lock_guard<std::mutex> lk(*_eqmtx); _eq->push(ev);} if (_eqcv) _eqcv->notify_one(); }

private:
    int _sock; std::atomic<bool> _connected; ISSPacketParser* _parser; ISSSession* _session; std::mutex _sendMtx; std::thread _recvThread; std::string _accum;
    UINT32 _remoteIp{0}; UINT16 _remotePort{0}; char _remoteIpStr[32]; UINT32 _localIp{0}; UINT16 _localPort{0}; char _localIpStr[32];
    std::queue<NetEvent>* _eq{nullptr}; std::mutex* _eqmtx{nullptr}; std::condition_variable* _eqcv{nullptr};
};

class ListenerImpl : public ISSListener {
public:
    ListenerImpl(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv)
        : _sock(-1), _parser(nullptr), _factory(nullptr), _running(false), _eq(q), _eqmtx(m), _eqcv(cv), _recvBuf(0), _sendBuf(0) {}
    ~ListenerImpl() override { Stop(); }
    void SSAPI SetPacketParser(ISSPacketParser* p) override { _parser=p; }
    void SSAPI SetSessionFactory(ISSSessionFactory* f) override { _factory=f; }
    void SSAPI SetBufferSize(UINT32 r, UINT32 s) override { _recvBuf=r; _sendBuf=s; }
    void SSAPI SetOpt(UINT32, void*) override {}
    bool SSAPI Start(const char* pszIP, UINT16 wPort, bool) override {
        int ls = ::socket(AF_INET, SOCK_STREAM, 0); if (ls<0) return false;
        int on = 1; setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        sockaddr_in addr{}; addr.sin_family=AF_INET; addr.sin_port=htons(wPort); inet_pton(AF_INET, pszIP, &addr.sin_addr);
        if (::bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))<0) { ::close(ls); return false; }
        if (::listen(ls, SOMAXCONN)<0) { ::close(ls); return false; }
        _sock=ls; _running.store(true);
        _th=std::thread([this](){ acceptLoop(); }); _th.detach();
        return true;
    }
    bool SSAPI Stop(void) override { _running.store(false); if (_sock!=-1){::close(_sock); _sock=-1;} return true; }
    void SSAPI Release(void) override { delete this; }
private:
    void acceptLoop() {
        while (_running.load()) {
            sockaddr_in cli{}; socklen_t clen=sizeof(cli);
            int cs = ::accept(_sock, reinterpret_cast<sockaddr*>(&cli), &clen);
            if (cs<0) { if(!_running.load()) break; continue; }
            sockaddr_in local{}; socklen_t llen=sizeof(local); getsockname(cs, reinterpret_cast<sockaddr*>(&local), &llen);
            if (_recvBuf) setsockopt(cs, SOL_SOCKET, SO_RCVBUF, &_recvBuf, sizeof(_recvBuf));
            if (_sendBuf) setsockopt(cs, SOL_SOCKET, SO_SNDBUF, &_sendBuf, sizeof(_sendBuf));
            auto* conn = new Connection(); conn->setQueue(_eq,_eqmtx,_eqcv); auto* sess = _factory? _factory->CreateSession(conn):nullptr; conn->setSession(sess); conn->setParser(_parser); conn->attach(cs, local, cli);
            NetEvent ev{NetEventType::Established, conn}; { std::lock_guard<std::mutex> lk(*_eqmtx); _eq->push(ev);} _eqcv->notify_one();
        }
    }
private:
    int _sock; ISSPacketParser* _parser; ISSSessionFactory* _factory; std::atomic<bool> _running; std::thread _th; std::queue<NetEvent>* _eq; std::mutex* _eqmtx; std::condition_variable* _eqcv; UINT32 _recvBuf; UINT32 _sendBuf;
};

class ConnectorImpl : public ISSConnector {
public:
    ConnectorImpl(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv)
        : _parser(nullptr), _session(nullptr), _eq(q), _eqmtx(m), _eqcv(cv), _recvBuf(0), _sendBuf(0), _lastIp(0), _lastPort(0) {}
    void SSAPI SetPacketParser(ISSPacketParser* p) override { _parser=p; }
    void SSAPI SetSession(ISSSession* s) override { _session=s; }
    ISSSession* SSAPI GetSession() override { return _session; }
    void SSAPI SetBufferSize(UINT32 r, UINT32 s) override { _recvBuf=r; _sendBuf=s; }
    int SSAPI Connect(const char* pszIP, UINT16 wPort) override {
        int s = ::socket(AF_INET, SOCK_STREAM, 0); if (s<0) return NET_CONNECT_FAIL;
        if (_recvBuf) setsockopt(s, SOL_SOCKET, SO_RCVBUF, &_recvBuf, sizeof(_recvBuf));
        if (_sendBuf) setsockopt(s, SOL_SOCKET, SO_SNDBUF, &_sendBuf, sizeof(_sendBuf));
        sockaddr_in addr{}; addr.sin_family=AF_INET; addr.sin_port=htons(wPort); inet_pton(AF_INET, pszIP, &addr.sin_addr);
        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))<0) { ::close(s); return NET_CONNECT_FAIL; }
        sockaddr_in local{}; socklen_t llen=sizeof(local); getsockname(s, reinterpret_cast<sockaddr*>(&local), &llen);
        _conn = std::make_unique<Connection>(); _conn->setQueue(_eq,_eqmtx,_eqcv); _conn->setSession(_session); _conn->setParser(_parser); _conn->attach(s, local, addr);
        NetEvent ev{NetEventType::Established, _conn.get()}; { std::lock_guard<std::mutex> lk(*_eqmtx); _eq->push(ev);} _eqcv->notify_one();
        _lastIp = addr.sin_addr.s_addr; _lastPort=wPort; return NET_SUCCESS;
    }
    int SSAPI ReConnect(void) override { if (_lastIp==0 || _lastPort==0) return NET_CONNECT_FAIL; char buf[32]; std::strncpy(buf, SDInetNtoa(_lastIp), sizeof(buf)-1); return Connect(buf, _lastPort); }
    void SSAPI Release(void) override { delete this; }
    void SSAPI SetOpt(UINT32, void*) override {}
private:
    ISSPacketParser* _parser; ISSSession* _session; std::unique_ptr<Connection> _conn; std::queue<NetEvent>* _eq; std::mutex* _eqmtx; std::condition_variable* _eqcv; UINT32 _recvBuf; UINT32 _sendBuf; UINT32 _lastIp; UINT16 _lastPort;
};

class NetImpl : public ISSNet {
public:
    NetImpl():_ref(1){}
    void SSAPI AddRef(void) override { _ref.fetch_add(1); }
    UINT32 SSAPI QueryRef(void) override { return _ref.load(); }
    void SSAPI Release(void) override { if (_ref.fetch_sub(1)==1) delete this; }
    SSSVersion SSAPI GetVersion(void) override { return SDNET_MODULE_VERSION; }
    const char * SSAPI GetModuleName(void) override { return SDNET_MODULENAME; }
    ISSConnector* SSAPI CreateConnector(UINT32) override { return new ConnectorImpl(&_eq,&_eqmtx,&_eqcv); }
    ISSListener* SSAPI CreateListener(UINT32) override { return new ListenerImpl(&_eq,&_eqmtx,&_eqcv); }
    bool SSAPI Run(INT32 nCount = -1) override {
        int processed=0; while (nCount<0 || processed<nCount) { NetEvent ev; { std::unique_lock<std::mutex> lk(_eqmtx); if (_eq.empty()) { if (nCount<0) _eqcv.wait_for(lk, std::chrono::milliseconds(10)); else break; } if (_eq.empty()) break; ev=std::move(_eq.front()); _eq.pop(); }
            switch (ev.type) { case NetEventType::Established: ev.conn->onEstablished(); break; case NetEventType::Terminated: ev.conn->onTerminated(); break; case NetEventType::Error: ev.conn->onError(ev.modErr, ev.sysErr); break; case NetEventType::Recv: ev.conn->onRecv(ev.data); break; }
            ++processed; }
        return true;
    }
private:
    std::atomic<UINT32> _ref; std::queue<NetEvent> _eq; std::mutex _eqmtx; std::condition_variable _eqcv;
};

ISSNet* SSAPI SSNetGetModule(const SSSVersion*) { return new NetImpl(); }
bool SSAPI SSNetSetLogger(ISSLogger*, UINT32) { return true; }
void SSAPI SSNetSetOpt(UINT32, void*) {}

} // namespace SSCP
