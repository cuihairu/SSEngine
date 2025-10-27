// Windows-first minimal implementation using Winsock + background threads.
#include "ssengine/sdnet.h"
#include "ssengine/sdnetopt.h"
#include "ssengine/sdnet_ver.h"
#include "ssengine/sdnetutils.h"

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

namespace SSCP {

// Simple event queue to deliver callbacks in Run() thread
enum class NetEventType { Established, Terminated, Error, Recv };
struct NetEvent {
    NetEventType type;
    class Connection* conn;
    std::string data; // for Recv
    int modErr{0};
    int sysErr{0};
};

static ISSLogger* g_logger = nullptr;
static UINT32 g_log_level = 0;

class Connection : public ISSConnection {
public:
    Connection():_sock(INVALID_SOCKET),_connected(false),_parser(nullptr),_session(nullptr),_recvThreadRunning(false){
        _remoteIpStr[0] = 0; _localIpStr[0] = 0;
    }
    ~Connection() override { Disconnect(); }

    bool SSAPI IsConnected(void) override { return _connected.load(); }

    void SSAPI Send(const char* pBuf,UINT32 dwLen) override {
        if (!_connected.load() || !pBuf || dwLen==0) return;
        std::lock_guard<std::mutex> lk(_sendMtx);
#ifdef _WIN32
        ::send(_sock, pBuf, static_cast<int>(dwLen), 0);
#endif
    }
    void SSAPI DelaySend(const char* pBuf,UINT32 dwLen) override {
        if (!_connected.load() || !pBuf || dwLen==0) return;
        // fire-and-forget async send
        std::string buf(pBuf, pBuf + dwLen);
        SOCKET s = _sock;
        std::thread([s, buf=std::move(buf)]() {
            ::send(s, buf.data(), static_cast<int>(buf.size()), 0);
        }).detach();
    }

    void SSAPI SetOpt(UINT32 dwType, void* pOpt) override {
        if (dwType == CONNECTION_OPT_SOCKOPT && pOpt) {
            SConnectionOptSockopt* opt = reinterpret_cast<SConnectionOptSockopt*>(pOpt);
#ifdef _WIN32
            ::setsockopt(_sock, opt->nLevel, opt->nOptName, opt->pOptVal, opt->nOptLen);
#endif
        }
    }

    void SSAPI Disconnect(void) override {
        _connected.store(false);
#ifdef _WIN32
        if (_sock != INVALID_SOCKET) {
            ::shutdown(_sock, SD_BOTH);
            ::closesocket(_sock);
            _sock = INVALID_SOCKET;
        }
#endif
    }

    const UINT32 SSAPI GetRemoteIP(void) override { return _remoteIp; }
    const char* SSAPI GetRemoteIPStr(void) override { return _remoteIpStr; }
    UINT16 SSAPI GetRemotePort(void) override { return _remotePort; }
    const UINT32 SSAPI GetLocalIP(void) override { return _localIp; }
    const char* SSAPI GetLocalIPStr(void) override { return _localIpStr; }
    UINT16 SSAPI GetLocalPort(void) override { return _localPort; }
    UINT32 SSAPI GetSendBufFree(void) override {
#ifdef _WIN32
        int sz = 0; int len = sizeof(sz);
        if (getsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&sz), &len) == 0) {
            return static_cast<UINT32>(sz);
        }
#endif
        return 0xFFFFFFFF;
    }

#ifdef _WIN32
    void attach(SOCKET s, sockaddr_in local, sockaddr_in remote) {
        _sock = s;
        _connected.store(true);
        _localIp = local.sin_addr.S_un.S_addr;
        _localPort = ntohs(local.sin_port);
        _remoteIp = remote.sin_addr.S_un.S_addr;
        _remotePort = ntohs(remote.sin_port);
        std::strncpy(_localIpStr, SDInetNtoa(_localIp), sizeof(_localIpStr)-1);
        std::strncpy(_remoteIpStr, SDInetNtoa(_remoteIp), sizeof(_remoteIpStr)-1);
        if (_session) _session->SetConnection(this);
        startRecvThread();
    }
#endif

    void setParser(ISSPacketParser* p) { _parser = p; }
    void setSession(ISSSession* s) { _session = s; }

    void enqueueRecv(const char* data, size_t len);

    // pump called by NetImpl
    void onEstablished();
    void onTerminated();
    void onRecv(const std::string& buf);
    void onError(int modErr, int sysErr);
    void releaseSession() { if (_session) { _session->Release(); _session = nullptr; } }

    void setQueue(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv) {
        _eq = q; _eqmtx = m; _eqcv = cv;
    }

private:
    void startRecvThread() {
        if (_recvThreadRunning.load()) return;
        _recvThreadRunning.store(true);
        _recvThread = std::thread([this](){
#ifdef _WIN32
            char buf[8192];
            while (_connected.load()) {
                int n = ::recv(_sock, buf, sizeof(buf), 0);
                if (n > 0) {
                    enqueueRecv(buf, static_cast<size_t>(n));
                } else if (n == 0) {
                    // closed
                    postEvent(NetEvent{NetEventType::Terminated, this});
                    _connected.store(false);
                    break;
                } else {
                    postEvent(NetEvent{NetEventType::Error, this, std::string(), NET_RECV_ERROR, WSAGetLastError()});
                    _connected.store(false);
                    break;
                }
            }
#endif
        });
        _recvThread.detach();
    }

    void postEvent(const NetEvent& ev) {
        if (!_eq || !_eqmtx) return;
        {
            std::lock_guard<std::mutex> lk(*_eqmtx);
            _eq->push(ev);
        }
        if (_eqcv) _eqcv->notify_one();
    }

private:
#ifdef _WIN32
    SOCKET _sock;
#else
    int _sock{ -1 };
#endif
    std::atomic<bool> _connected;
    ISSPacketParser* _parser;
    ISSSession* _session;
    std::mutex _sendMtx;
    std::thread _recvThread;
    std::atomic<bool> _recvThreadRunning;

    UINT32 _remoteIp{0}; UINT16 _remotePort{0}; char _remoteIpStr[32];
    UINT32 _localIp{0};  UINT16 _localPort{0};  char _localIpStr[32];

    // event queue backrefs
    std::queue<NetEvent>* _eq{nullptr};
    std::mutex* _eqmtx{nullptr};
    std::condition_variable* _eqcv{nullptr};
};

void Connection::enqueueRecv(const char* data, size_t len) {
    if (!_parser) { postEvent(NetEvent{NetEventType::Recv, this, std::string(data, len)}); return; }
    // Accumulate and parse frames by parser
    _accum.append(data, data + len);
    for (;;) {
        int want = _parser->ParsePacket(_accum.data(), static_cast<UINT32>(_accum.size()));
        if (want < 0) { postEvent(NetEvent{NetEventType::Error, this, std::string(), NET_PACKET_ERROR, 0}); _accum.clear(); break; }
        if (want == 0) break; // need more
        if (static_cast<size_t>(want) <= _accum.size()) {
            postEvent(NetEvent{NetEventType::Recv, this, std::string(_accum.data(), _accum.data() + want)});
            _accum.erase(0, static_cast<size_t>(want));
        } else {
            break;
        }
    }
}

void Connection::onEstablished() { if (_session) _session->OnEstablish(); }
void Connection::onTerminated() { if (_session) _session->OnTerminate(); }
void Connection::onRecv(const std::string& buf) { if (_session) _session->OnRecv(buf.data(), static_cast<UINT32>(buf.size())); }
void Connection::onError(int modErr, int sysErr) { if (_session) _session->OnError(modErr, sysErr); }
// Ensure session release when connection terminated
// NetImpl will invoke onTerminated via event; after that, release here for safety
// (idempotent if called once)
// Note: error path does not force release

class ListenerImpl : public ISSListener {
public:
    ListenerImpl(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv)
        : _parser(nullptr), _factory(nullptr), _running(false), _eq(q), _eqmtx(m), _eqcv(cv), _recvBuf(0), _sendBuf(0) {
#ifdef _WIN32
        _sock = INVALID_SOCKET;
#endif
    }
    ~ListenerImpl() override { Stop(); }

    void SSAPI SetPacketParser(ISSPacketParser* poPacketParser) override { _parser = poPacketParser; }
    void SSAPI SetSessionFactory(ISSSessionFactory* poSessionFactory) override { _factory = poSessionFactory; }
    void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) override {
        _recvBuf = dwRecvBufSize; _sendBuf = dwSendBufSize;
    }
    void SSAPI SetOpt(UINT32, void*) override {}

    bool SSAPI Start(const char* pszIP, UINT16 wPort, bool bReUseAddr = true) override {
        if (_running.load()) return false;
#ifdef _WIN32
        SOCKET ls = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ls == INVALID_SOCKET) return false;
        if (bReUseAddr) {
            BOOL opt = TRUE; setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        }
        sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(wPort);
        in_addr ip{}; if (!SDNetInetPton(AF_INET, pszIP, &ip)) { ::closesocket(ls); return false; }
        addr.sin_addr = ip;
        if (::bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) { ::closesocket(ls); return false; }
        if (::listen(ls, SOMAXCONN) == SOCKET_ERROR) { ::closesocket(ls); return false; }
        _sock = ls; _running.store(true);
        _th = std::thread([this](){ this->acceptLoop(); }); _th.detach();
        return true;
#else
        (void)pszIP; (void)wPort; (void)bReUseAddr; return false;
#endif
    }
    bool SSAPI Stop(void) override {
        _running.store(false);
#ifdef _WIN32
        if (_sock != INVALID_SOCKET) { ::closesocket(_sock); _sock = INVALID_SOCKET; }
#endif
        return true;
    }
    void SSAPI Release(void) override { delete this; }

private:
#ifdef _WIN32
    void acceptLoop() {
        while (_running.load()) {
            sockaddr_in cli{}; int clen = sizeof(cli);
            SOCKET cs = ::accept(_sock, reinterpret_cast<sockaddr*>(&cli), &clen);
            if (cs == INVALID_SOCKET) {
                if (!_running.load()) break;
                continue;
            }
            sockaddr_in local{}; int llen = sizeof(local);
            getsockname(cs, reinterpret_cast<sockaddr*>(&local), &llen);
            auto* conn = new Connection();
            conn->setQueue(_eq, _eqmtx, _eqcv);
            auto* sess = _factory ? _factory->CreateSession(conn) : nullptr;
            conn->setSession(sess);
            conn->setParser(_parser);
            if (_recvBuf) { setsockopt(cs, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&_recvBuf), sizeof(_recvBuf)); }
            if (_sendBuf) { setsockopt(cs, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&_sendBuf), sizeof(_sendBuf)); }
            conn->attach(cs, local, cli);
            // post established event
            NetEvent ev{NetEventType::Established, conn};
            {
                std::lock_guard<std::mutex> lk(*_eqmtx);
                _eq->push(ev);
            }
            _eqcv->notify_one();
        }
    }
#endif

private:
#ifdef _WIN32
    SOCKET _sock;
#endif
    ISSPacketParser* _parser;
    ISSSessionFactory* _factory;
    std::atomic<bool> _running;
    std::thread _th;
    std::queue<NetEvent>* _eq; std::mutex* _eqmtx; std::condition_variable* _eqcv;
    UINT32 _recvBuf; UINT32 _sendBuf;
};

class ConnectorImpl : public ISSConnector {
public:
    ConnectorImpl(std::queue<NetEvent>* q, std::mutex* m, std::condition_variable* cv)
        : _parser(nullptr), _session(nullptr), _eq(q), _eqmtx(m), _eqcv(cv) {}
    ~ConnectorImpl() override { }

    void SSAPI SetPacketParser(ISSPacketParser* poPakcetParser) override { _parser = poPakcetParser; }
    void SSAPI SetSession(ISSSession* poSession) override { _session = poSession; }
    ISSSession* SSAPI GetSession() override { return _session; }
    void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) override {
        _recvBuf = dwRecvBufSize; _sendBuf = dwSendBufSize;
    }

    int SSAPI Connect(const char* pszIP, UINT16 wPort) override {
#ifdef _WIN32
        SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) return NET_CONNECT_FAIL;
        if (_recvBuf) { setsockopt(s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&_recvBuf), sizeof(_recvBuf)); }
        if (_sendBuf) { setsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&_sendBuf), sizeof(_sendBuf)); }
        sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(wPort);
        in_addr ip{}; if (!SDNetInetPton(AF_INET, pszIP, &ip)) { ::closesocket(s); return NET_CONNECT_FAIL; }
        addr.sin_addr = ip;
        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            int se = WSAGetLastError(); ::closesocket(s); (void)se; return NET_CONNECT_FAIL;
        }
        sockaddr_in local{}; int llen = sizeof(local);
        getsockname(s, reinterpret_cast<sockaddr*>(&local), &llen);
        _conn = std::make_unique<Connection>();
        _conn->setQueue(_eq, _eqmtx, _eqcv);
        _conn->setSession(_session);
        _conn->setParser(_parser);
        _conn->attach(s, local, addr);
        // post established
        NetEvent ev{NetEventType::Established, _conn.get()};
        {
            std::lock_guard<std::mutex> lk(*_eqmtx);
            _eq->push(ev);
        }
        _eqcv->notify_one();
        _lastIp = addr.sin_addr.S_un.S_addr; _lastPort = wPort;
        return NET_SUCCESS;
#else
        (void)pszIP; (void)wPort; return NET_CONNECT_FAIL;
#endif
    }

    int SSAPI ReConnect(void) override {
#ifdef _WIN32
        if (_lastIp == 0 || _lastPort == 0) return NET_CONNECT_FAIL;
        CHAR ipbuf[32] = {0};
        std::strncpy(ipbuf, SDInetNtoa(_lastIp), sizeof(ipbuf)-1);
        return Connect(ipbuf, _lastPort);
#else
        return NET_CONNECT_FAIL;
#endif
    }
    void SSAPI Release(void) override { delete this; }
    void SSAPI SetOpt(UINT32, void*) override {}

private:
    ISSPacketParser* _parser;
    ISSSession* _session;
    std::unique_ptr<Connection> _conn;
    std::queue<NetEvent>* _eq; std::mutex* _eqmtx; std::condition_variable* _eqcv;
    UINT32 _lastIp{0}; UINT16 _lastPort{0};
    UINT32 _recvBuf{0}; UINT32 _sendBuf{0};
};

class NetImpl : public ISSNet {
public:
    NetImpl() : _ref(1) {
#ifdef _WIN32
        WSADATA wsa{}; WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    }
    ~NetImpl() override {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void SSAPI AddRef(void) override { _ref.fetch_add(1); }
    UINT32 SSAPI QueryRef(void) override { return _ref.load(); }
    void SSAPI Release(void) override { if (_ref.fetch_sub(1)==1) delete this; }
    SSSVersion SSAPI GetVersion(void) override { return SDNET_MODULE_VERSION; }
    const char * SSAPI GetModuleName(void) override { return SDNET_MODULENAME; }

    ISSConnector* SSAPI CreateConnector(UINT32 /*dwNetIOType*/) override {
        return new ConnectorImpl(&_eq, &_eqmtx, &_eqcv);
    }
    ISSListener* SSAPI CreateListener(UINT32 /*dwNetIOType*/) override {
        return new ListenerImpl(&_eq, &_eqmtx, &_eqcv);
    }

    bool SSAPI Run(INT32 nCount = -1) override {
        int processed = 0;
        while (nCount < 0 || processed < nCount) {
            NetEvent ev;
            {
                std::unique_lock<std::mutex> lk(_eqmtx);
                if (_eq.empty()) {
                    if (nCount < 0) {
                        _eqcv.wait_for(lk, std::chrono::milliseconds(10));
                    } else {
                        break;
                    }
                }
                if (_eq.empty()) break;
                ev = std::move(_eq.front()); _eq.pop();
            }
            dispatch(ev);
            ++processed;
        }
        return true;
    }

private:
    void dispatch(const NetEvent& ev) {
        switch (ev.type) {
            case NetEventType::Established: ev.conn->onEstablished(); break;
            case NetEventType::Terminated: ev.conn->onTerminated(); ev.conn->releaseSession(); break;
            case NetEventType::Error: ev.conn->onError(ev.modErr, ev.sysErr); break;
            case NetEventType::Recv: ev.conn->onRecv(ev.data); break;
        }
    }

private:
    std::atomic<UINT32> _ref;
    std::queue<NetEvent> _eq; std::mutex _eqmtx; std::condition_variable _eqcv;
};

// Factories / options
ISSNet* SSAPI SSNetGetModule(const SSSVersion* /*pstVersion*/) {
    return new NetImpl();
}

bool SSAPI SSNetSetLogger(ISSLogger* poLogger, UINT32 dwLevel) {
    g_logger = poLogger; g_log_level = dwLevel; (void)g_log_level; return true;
}

void SSAPI SSNetSetOpt(UINT32 /*dwType*/, void* /*pOpt*/) {
    // Store options as needed in future
}

} // namespace SSCP
