#include "ssengine/sdpipe.h"
#include "ssengine/sdpkg.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnetutils.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <fstream>
#include <memory>
#include <cstring>

namespace SSCP {

static ISSLogger* g_pipe_logger = nullptr;
static UINT32 g_pipe_loglevel = 0;

struct SinkEntry {
    ISSPipeSink* sink{nullptr};
    UINT32 userData{0};
};

class PipeImpl : public ISSPipe {
public:
    PipeImpl(UINT32 id) : _id(id), _conn(nullptr), _ip(0) {}
    ~PipeImpl() override {}

    bool isConnected() const { return _conn != nullptr; }

    UINT32 SSAPI GetID(void) override { return _id; }

    bool SSAPI Send(UINT16 wBusinessID, const char* pData, UINT32 dwLen) override {
        if (!_conn || !pData) { fprintf(stderr, "[PipeImpl %u] Send fail: no conn or null data\n", _id); return false; }
        // Build sdpkg with payload: [businessID(2 bytes, network)] + data
        std::vector<char> buf; buf.resize(sizeof(SSDPkgHead16) + 2 + dwLen);
        auto* head = reinterpret_cast<SSDPkgHead16*>(buf.data());
        BuildSDPkgHead16(head, static_cast<UINT16>(2 + dwLen));
        UINT16 bid = SDHtons(wBusinessID);
        std::memcpy(buf.data() + sizeof(SSDPkgHead16), &bid, 2);
        std::memcpy(buf.data() + sizeof(SSDPkgHead16) + 2, pData, dwLen);
        _conn->Send(buf.data(), static_cast<UINT32>(buf.size()));
        fprintf(stderr, "[PipeImpl %u] Sent bid=%u len=%u\n", _id, (unsigned)wBusinessID, (unsigned)dwLen);
        // No local deliver; use network and peer fallback in module
        return true;
    }

    void SSAPI SetUserData(UINT16 wBusinessID, UINT32 dwData) override {
        std::lock_guard<std::mutex> lk(_mtx); _sinks[wBusinessID].userData = dwData;
    }
    bool SSAPI GetUserData(UINT16 wBusinessID, UINT32* pdwData) override {
        if (!pdwData) return false; std::lock_guard<std::mutex> lk(_mtx);
        auto it = _sinks.find(wBusinessID); if (it==_sinks.end()) return false; *pdwData = it->second.userData; return true;
    }

    bool SSAPI SetSink(UINT16 wBusinessID, ISSPipeSink* pSink) override {
        std::lock_guard<std::mutex> lk(_mtx);
        _sinks[wBusinessID].sink = pSink;
        fprintf(stderr, "[PipeImpl %u] SetSink bid=%u sink=%p\n", _id, (unsigned)wBusinessID, (void*)pSink);
        return true;
    }
    ISSPipeSink* SSAPI GetSink(UINT16 wBusinessID) override {
        std::lock_guard<std::mutex> lk(_mtx); auto it = _sinks.find(wBusinessID); if (it==_sinks.end()) return nullptr; return it->second.sink;
    }

    UINT32 SSAPI GetIP(void) override { return _ip; }
    void SSAPI Close(void) override { if (_conn) _conn->Disconnect(); }

    void attach(ISSConnection* c) {
        _conn = c;
        if (c) {
            _rip = c->GetRemoteIP();
            _rport = c->GetRemotePort();
            _lip = c->GetLocalIP();
            _lport = c->GetLocalPort();
            fprintf(stderr, "[PipeImpl %u] attach conn, rip=%s\n", _id, c->GetRemoteIPStr());
        }
    }
    void detach(ISSConnection* c) { if (_conn == c) _conn = nullptr; }
    bool onRecv(const char* pData, UINT32 dwLen) {
        if (dwLen < 2) return false;
        UINT16 bid = SDNtohs(*reinterpret_cast<const UINT16*>(pData));
        ISSPipeSink* sink = nullptr;
        {
            std::lock_guard<std::mutex> lk(_mtx);
            auto it = _sinks.find(bid);
            if (it != _sinks.end()) sink = it->second.sink;
        }
        if (sink) {
            fprintf(stderr, "[PipeImpl %u] OnRecv deliver bid=%u len=%u data='", _id, (unsigned)bid, (unsigned)(dwLen-2));
            for (UINT32 i = 0; i < dwLen - 2; ++i) {
                char ch = *(pData + 2 + i);
                fputc((ch >= 32 && ch <= 126) ? ch : '.', stderr);
            }
            fprintf(stderr, "'\n");
            sink->OnRecv(bid, pData + 2, dwLen - 2);
            return true;
        }
        return false;
    }

    // tuple access for peer matching
    UINT32 rip() const { return _rip; }
    UINT16 rport() const { return _rport; }
    UINT32 lip() const { return _lip; }
    UINT16 lport() const { return _lport; }

private:
    UINT32 _id; ISSConnection* _conn; UINT32 _ip; std::mutex _mtx; std::unordered_map<UINT16, SinkEntry> _sinks;
    // connection tuple
    UINT32 _rip{0}; UINT16 _rport{0}; UINT32 _lip{0}; UINT16 _lport{0};
};

class PipeModule;

class PipeSession : public ISSSession {
public:
    PipeSession(PipeModule* mod, UINT32 pid);
    void SSAPI SetConnection(ISSConnection* c) override;
    void SSAPI OnEstablish(void) override;
    void SSAPI OnTerminate(void) override;
    bool SSAPI OnError(INT32 nModuleErr, INT32 nSysErr) override;
    void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) override;
    void SSAPI Release(void) override;
private:
    PipeModule* _mod;
    UINT32 _id;
    ISSConnection* _conn;
};

class PipeSessionFactory : public ISSSessionFactory {
public:
    PipeSessionFactory(PipeModule* mod, UINT32 pid);
    ISSSession* SSAPI CreateSession(ISSConnection* poConnection) override;
private:
    PipeModule* _mod;
    UINT32 _id;
};

class PipeListenerFactory : public ISSSessionFactory {
public:
    explicit PipeListenerFactory(PipeModule& owner);
    ISSSession* SSAPI CreateSession(ISSConnection* poConnection) override;
private:
    PipeModule& _owner;
};

class PipeModule : public ISSPipeModule {
public:
    PipeModule() : _ref(1), _net(nullptr), _reporter(nullptr), _localId(0) {}
    ~PipeModule() override {
        // Release listeners
        for (auto* lis : _listeners) { if (lis) { lis->Stop(); lis->Release(); } }
        _listeners.clear();
        // Release connectors
        for (auto& kv : _connById) { if (kv.second) kv.second->Release(); }
        _connById.clear();
        // Delete parsers
        for (auto& kv : _parserById) { delete kv.second; }
        _parserById.clear();
        // Pipes map will be freed via unique_ptr
        _pipes.clear();
    }

    bool SSAPI Init(const char* /*pszConfFile*/, const char* /*pszIPListFile*/, ISSPipeReporter* pReporter, ISSNet* pNetModule) override {
        _reporter = pReporter; _net = pNetModule; _localId = 0x01000000; // arbitrary
        _useWhitelist = false; _ipWhitelist.clear();
        return (_net != nullptr);
    }

    ISSPipe* SSAPI GetPipe(UINT32 dwID) override {
        std::lock_guard<std::mutex> lk(_mtx); auto it = _pipes.find(dwID); return it==_pipes.end() ? nullptr : it->second.get();
    }

    bool SSAPI Run(INT32 nCount = -1) override { return _net ? _net->Run(nCount) : false; }

    bool SSAPI ReplaceConn(UINT32 dwID, const char* pszRemoteIP, UINT16 wRemotePort, UINT32, UINT32) override {
        // Keep existing PipeImpl (sinks/userdata) and reconnect session
        if (!_net) return false;
        {
            std::lock_guard<std::mutex> lk(_mtx);
            if (_pipes.find(dwID) == _pipes.end()) {
                report(PIPE_REMOTEID_ERR, dwID);
                return false;
            }
        }
        auto* conn = _net->CreateConnector(NETIO_ASYNCSELECT);
        auto* parser = new CSDPacketParser();
        auto* session = new PipeSession(this, dwID);
        conn->SetPacketParser(parser);
        conn->SetSession(session);
        int rc = conn->Connect(pszRemoteIP, wRemotePort);
        if (rc != NET_SUCCESS) {
            conn->Release();
            delete parser;
            return false;
        }
        std::lock_guard<std::mutex> lk(_mtx);
        auto itPipe = _pipes.find(dwID);
        if (itPipe == _pipes.end()) {
            conn->Release();
            delete parser;
            report(PIPE_REMOTEID_ERR, dwID);
            return false;
        }
        // release old
        auto itc = _connById.find(dwID);
        if (itc != _connById.end()) { itc->second->Release(); _connById.erase(itc); }
        auto itp = _parserById.find(dwID);
        if (itp != _parserById.end()) { delete itp->second; _parserById.erase(itp); }
        _connById[dwID] = conn;
        _parserById[dwID] = parser;
        return true;
    }
    bool SSAPI AddConn(UINT32 dwID, const char* pszRemoteIP, UINT16 wRemotePort, UINT32, UINT32) override {
        if (!_net) return false;
        if (!_checkIp(pszRemoteIP)) return false;
        {
            std::lock_guard<std::mutex> lk(_mtx);
            if (_hasActiveConnectionLocked(dwID) || _pendingConnects.find(dwID) != _pendingConnects.end()) {
                report(PIPE_REPEAT_CONN, dwID);
                return false;
            }
            if (_pipes.find(dwID) == _pipes.end()) {
                _pipes.emplace(dwID, std::unique_ptr<PipeImpl>(new PipeImpl(dwID)));
            }
            _pendingConnects.insert(dwID);
        }
        auto* conn = _net->CreateConnector(NETIO_ASYNCSELECT);
        auto* parser = new CSDPacketParser();
        auto* session = new PipeSession(this, dwID);
        conn->SetPacketParser(parser);
        conn->SetSession(session);
        int rc = conn->Connect(pszRemoteIP, wRemotePort);
        if (rc != NET_SUCCESS) {
            {
                std::lock_guard<std::mutex> lk(_mtx);
                _pendingConnects.erase(dwID);
            }
            conn->Release();
            delete parser;
            return false;
        }
        std::lock_guard<std::mutex> lk(_mtx);
        auto itPipe = _pipes.find(dwID);
        if (itPipe == _pipes.end()) {
            _pendingConnects.erase(dwID);
            conn->Release();
            delete parser;
            report(PIPE_REMOTEID_ERR, dwID);
            return false;
        }
        _pendingConnects.erase(dwID);
        // store mapping
        _connById[dwID] = conn;
        _parserById[dwID] = parser;
        return true;
    }
    bool SSAPI RemoveConn(UINT32 dwID) override {
        std::lock_guard<std::mutex> lk(_mtx);
        auto it = _pipes.find(dwID);
        if (it == _pipes.end()) {
            report(PIPE_REMOTEID_ERR, dwID);
            return false;
        }
        it->second->Close();
        _pendingConnects.erase(dwID);
        _pipes.erase(it);
        auto itc = _connById.find(dwID); if (itc != _connById.end()) { itc->second->Release(); _connById.erase(itc); }
        auto itp = _parserById.find(dwID); if (itp != _parserById.end()) { delete itp->second; _parserById.erase(itp); }
        report(PIPE_DISCONNECT, dwID);
        return true;
    }

    bool SSAPI AddListen(const char* pszLocalIP, UINT16 wLocalPort, UINT32, UINT32) override {
        if (!_net) return false;
        auto* lis = _net->CreateListener(NETIO_ASYNCSELECT);
        auto parser = std::make_unique<CSDPacketParser>();
        lis->SetPacketParser(parser.get());
        _acceptFactories.emplace_back(std::make_unique<PipeListenerFactory>(*this));
        lis->SetSessionFactory(_acceptFactories.back().get());
        bool ok = lis->Start(pszLocalIP, wLocalPort, true);
        if (!ok) {
            _acceptFactories.pop_back();
            lis->Release();
            return false;
        }
        _listenerParsers.emplace_back(std::move(parser));
        _listeners.emplace_back(lis);
        return true;
    }

    UINT32 SSAPI GetLocalID(void) override { return _localId; }

    bool SSAPI ReloadIPList(const char* pszIPListFile) override {
        if (!pszIPListFile) return false;
        std::ifstream ifs(pszIPListFile);
        if (!ifs) return false;
        std::unordered_set<std::string> tmp;
        std::string line;
        while (std::getline(ifs, line)) {
            size_t b = line.find_first_not_of(" \t\r\n");
            size_t e = line.find_last_not_of(" \t\r\n");
            if (b == std::string::npos) continue;
            std::string ip = line.substr(b, e - b + 1);
            if (!ip.empty() && ip[0] != '#') tmp.insert(ip);
        }
        std::lock_guard<std::mutex> lk(_mtx);
        _ipWhitelist.swap(tmp);
        _useWhitelist = !_ipWhitelist.empty();
        return true;
    }
    bool SSAPI ReloadPipeConfig(const char* /*pszConfFile*/, const UINT32 /*dwGroup*/) override { return true; }
    bool SSAPI CheckIpValid(const char* ip) override { return _checkIp(ip); }

    void SSAPI AddRef(void) override { _ref.fetch_add(1); }
    UINT32 SSAPI QueryRef(void) override { return _ref.load(); }
    void SSAPI Release(void) override { if (_ref.fetch_sub(1)==1) delete this; }
    SSSVersion SSAPI GetVersion(void) override { return SDPIPE_VERSION; }
    const char * SSAPI GetModuleName(void) override { return SDPIPE_MODULENAME; }

    // Hooks used by PipeSession
    void attachConnection(UINT32 id, ISSConnection* c) {
        std::lock_guard<std::mutex> lk(_mtx);
        auto it = _pipes.find(id); if (it==_pipes.end()) it = _pipes.emplace(id, std::unique_ptr<PipeImpl>(new PipeImpl(id))).first;
        it->second->attach(c);
        report(PIPE_SUCCESS, id);
    }
    void onPipeRecv(UINT32 id, const char* p, UINT32 len) {
        UINT32 off = GetSDPkgDataOffset(p, len);
        if (off == 0 || off > len) return;
        PipeImpl* dest = nullptr;
        {
            std::lock_guard<std::mutex> lk(_mtx);
            auto it = _pipes.find(id);
            if (it != _pipes.end()) dest = it->second.get();
        }
        if (!dest) return;
        fprintf(stderr, "[PipeModule] onPipeRecv id=%u len=%u off=%u\n", id, (unsigned)len, (unsigned)off);
        bool delivered = dest->onRecv(p + off, len - off);
        if (!delivered) {
            // peer fallback: find reversed 4-tuple
            UINT32 rip = dest->rip(), lip = dest->lip();
            UINT16 rport = dest->rport(), lport = dest->lport();
            PipeImpl* peer = nullptr;
            {
                std::lock_guard<std::mutex> lk(_mtx);
                for (auto &kv : _pipes) {
                    PipeImpl* pi = kv.second.get();
                    if (!pi) continue;
                    if (pi->rip() == lip && pi->rport() == lport && pi->lip() == rip && pi->lport() == rport) { peer = pi; break; }
                }
            }
            if (peer) {
                peer->onRecv(p + off, len - off);
            }
        }
    }
    void report(INT32 code, UINT32 id) {
        if (_reporter) _reporter->OnReport(code, id);
    }

    UINT32 allocId() { return ++_localId; }
    void detachConnection(UINT32 id, ISSConnection* conn) {
        std::lock_guard<std::mutex> lk(_mtx);
        auto it = _pipes.find(id);
        if (it != _pipes.end()) {
            it->second->detach(conn);
        }
        _pendingConnects.erase(id);
        auto itc = _connById.find(id);
        if (itc != _connById.end()) { itc->second->Release(); _connById.erase(itc); }
        auto itp = _parserById.find(id);
        if (itp != _parserById.end()) { delete itp->second; _parserById.erase(itp); }
    }

private:
    std::atomic<UINT32> _ref;
    ISSNet* _net;
    ISSPipeReporter* _reporter;
    std::mutex _mtx;
    std::unordered_map<UINT32, std::unique_ptr<PipeImpl>> _pipes;
    std::vector<ISSListener*> _listeners; // owned by module; release on destructor
    std::unordered_map<UINT32, ISSConnector*> _connById;
    std::unordered_map<UINT32, ISSPacketParser*> _parserById;
    std::vector<std::unique_ptr<ISSSessionFactory>> _acceptFactories;
    std::vector<std::unique_ptr<ISSPacketParser>> _listenerParsers;
    std::unordered_set<UINT32> _pendingConnects;
    UINT32 _localId;
    bool _useWhitelist{false};
    std::unordered_set<std::string> _ipWhitelist;

    bool _hasActiveConnectionLocked(UINT32 id) const {
        if (_connById.find(id) != _connById.end()) return true;
        auto it = _pipes.find(id);
        return it != _pipes.end() && it->second && it->second->isConnected();
    }

    bool _checkIp(const char* ip) const {
        if (!_useWhitelist) return true;
        if (!ip) return false;
        return _ipWhitelist.find(ip) != _ipWhitelist.end();
    }

    ISSSession* makeListenerSession(ISSConnection* poConnection) {
        if (!poConnection) return nullptr;
        if (_useWhitelist) {
            const char* rip = poConnection->GetRemoteIPStr();
            if (!rip || !_checkIp(rip)) {
                poConnection->Disconnect();
                return nullptr;
            }
        }
        UINT32 id = allocId();
        auto* session = new PipeSession(this, id);
        session->SetConnection(poConnection);
        return session;
    }

    friend class PipeListenerFactory;
};

PipeSession::PipeSession(PipeModule* mod, UINT32 pid)
    : _mod(mod), _id(pid), _conn(nullptr) {}

void SSAPI PipeSession::SetConnection(ISSConnection* c) {
    _conn = c;
    if (_mod) {
        _mod->attachConnection(_id, c);
    }
}

void SSAPI PipeSession::OnEstablish(void) { }

void SSAPI PipeSession::OnTerminate(void) {
    if (_mod) {
        _mod->detachConnection(_id, _conn);
        _mod->report(PIPE_DISCONNECT, _id);
    }
}

bool SSAPI PipeSession::OnError(INT32 nModuleErr, INT32 nSysErr) {
    if (_mod) {
        _mod->report(nModuleErr, _id);
    }
    (void)nSysErr;
    return true;
}

void SSAPI PipeSession::OnRecv(const char* pBuf, UINT32 dwLen) {
    if (_mod) {
        _mod->onPipeRecv(_id, pBuf, dwLen);
    }
}

void SSAPI PipeSession::Release(void) {
    delete this;
}

PipeSessionFactory::PipeSessionFactory(PipeModule* mod, UINT32 pid)
    : _mod(mod), _id(pid) {}

ISSSession* SSAPI PipeSessionFactory::CreateSession(ISSConnection* poConnection) {
    if (!_mod) {
        return nullptr;
    }
    auto* session = new PipeSession(_mod, _id);
    session->SetConnection(poConnection);
    return session;
}

PipeListenerFactory::PipeListenerFactory(PipeModule& owner)
    : _owner(owner) {}

// factory and logger
ISSPipeModule* SSAPI SSPipeGetModule(const SSSVersion* /*pstVersion*/) { return new PipeModule(); }
bool SSAPI SSPipeSetLogger(ISSLogger* poLogger, UINT32 dwLevel) { g_pipe_logger = poLogger; g_pipe_loglevel = dwLevel; return true; }

ISSSession* SSAPI PipeListenerFactory::CreateSession(ISSConnection* poConnection) {
    return _owner.makeListenerSession(poConnection);
}

} // namespace SSCP
