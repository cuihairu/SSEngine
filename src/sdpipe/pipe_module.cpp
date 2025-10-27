#include "ssengine/sdpipe.h"
#include "ssengine/sdpkg.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnetutils.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>

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

    UINT32 SSAPI GetID(void) override { return _id; }

    bool SSAPI Send(UINT16 wBusinessID, const char* pData, UINT32 dwLen) override {
        if (!_conn || !pData) return false;
        // Build sdpkg with payload: [businessID(2 bytes, network)] + data
        std::vector<char> buf; buf.resize(sizeof(SSDPkgHead16) + 2 + dwLen);
        auto* head = reinterpret_cast<SSDPkgHead16*>(buf.data());
        BuildSDPkgHead16(head, static_cast<UINT16>(2 + dwLen));
        UINT16 bid = SDHtons(wBusinessID);
        std::memcpy(buf.data() + sizeof(SSDPkgHead16), &bid, 2);
        std::memcpy(buf.data() + sizeof(SSDPkgHead16) + 2, pData, dwLen);
        _conn->Send(buf.data(), static_cast<UINT32>(buf.size()));
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
        std::lock_guard<std::mutex> lk(_mtx); _sinks[wBusinessID].sink = pSink; return true;
    }
    ISSPipeSink* SSAPI GetSink(UINT16 wBusinessID) override {
        std::lock_guard<std::mutex> lk(_mtx); auto it = _sinks.find(wBusinessID); if (it==_sinks.end()) return nullptr; return it->second.sink;
    }

    UINT32 SSAPI GetIP(void) override { return _ip; }
    void SSAPI Close(void) override { if (_conn) _conn->Disconnect(); }

    void attach(ISSConnection* c) { _conn = c; if (c) _ip = c->GetRemoteIP(); }
    void onRecv(const char* pData, UINT32 dwLen) {
        if (dwLen < 2) return; UINT16 bid = SDNtohs(*reinterpret_cast<const UINT16*>(pData));
        std::lock_guard<std::mutex> lk(_mtx); auto it = _sinks.find(bid); if (it && it != _sinks.end() && it->second.sink) {
            it->second.sink->OnRecv(bid, pData + 2, dwLen - 2);
        }
    }

private:
    UINT32 _id; ISSConnection* _conn; UINT32 _ip; std::mutex _mtx; std::unordered_map<UINT16, SinkEntry> _sinks;
};

class PipeSession : public ISSSession {
public:
    PipeSession(class PipeModule* mod, UINT32 pid) : _mod(mod), _id(pid), _conn(nullptr) {}
    void SSAPI SetConnection(ISSConnection* c) override { _conn = c; if (_mod) _mod->attachConnection(_id, c); }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override { if (_mod) _mod->report(PIPE_DISCONNECT, _id); }
    bool SSAPI OnError(INT32 nModuleErr, INT32 nSysErr) override { if (_mod) _mod->report(nModuleErr, _id); return true; }
    void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) override { if (_mod) _mod->onPipeRecv(_id, pBuf, dwLen); }
    void SSAPI Release(void) override { delete this; }
private:
    class PipeModule* _mod; UINT32 _id; ISSConnection* _conn;
};

class PipeSessionFactory : public ISSSessionFactory {
public:
    PipeSessionFactory(class PipeModule* mod, UINT32 pid) : _mod(mod), _id(pid) {}
    ISSSession* SSAPI CreateSession(ISSConnection* poConnection) override {
        auto* s = new PipeSession(_mod, _id); s->SetConnection(poConnection); return s;
    }
private:
    class PipeModule* _mod; UINT32 _id;
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
        return (_net != nullptr);
    }

    ISSPipe* SSAPI GetPipe(UINT32 dwID) override {
        std::lock_guard<std::mutex> lk(_mtx); auto it = _pipes.find(dwID); return it==_pipes.end() ? nullptr : it->second.get();
    }

    bool SSAPI Run(INT32 nCount = -1) override { return _net ? _net->Run(nCount) : false; }

    bool SSAPI ReplaceConn(UINT32 dwID, const char* pszRemoteIP, UINT16 wRemotePort, UINT32, UINT32) override {
        // Keep existing PipeImpl (sinks/userdata) and reconnect session
        if (!_net) return false;
        auto* conn = _net->CreateConnector(NETIO_ASYNCSELECT);
        auto* parser = new CSDPacketParser();
        auto* session = new PipeSession(this, dwID);
        conn->SetPacketParser(parser);
        conn->SetSession(session);
        int rc = conn->Connect(pszRemoteIP, wRemotePort);
        if (rc != NET_SUCCESS) { conn->Release(); delete parser; return false; }
        std::lock_guard<std::mutex> lk(_mtx);
        if (_pipes.find(dwID) == _pipes.end()) _pipes.emplace(dwID, std::unique_ptr<PipeImpl>(new PipeImpl(dwID)));
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
        auto* conn = _net->CreateConnector(NETIO_ASYNCSELECT);
        auto* parser = new CSDPacketParser();
        auto* session = new PipeSession(this, dwID);
        conn->SetPacketParser(parser);
        conn->SetSession(session);
        int rc = conn->Connect(pszRemoteIP, wRemotePort);
        if (rc != NET_SUCCESS) { conn->Release(); delete parser; return false; }
        std::lock_guard<std::mutex> lk(_mtx);
        if (_pipes.find(dwID) == _pipes.end()) _pipes.emplace(dwID, std::unique_ptr<PipeImpl>(new PipeImpl(dwID)));
        // store mapping
        _connById[dwID] = conn;
        _parserById[dwID] = parser;
        return true;
    }
    bool SSAPI RemoveConn(UINT32 dwID) override {
        std::lock_guard<std::mutex> lk(_mtx);
        auto it = _pipes.find(dwID); if (it == _pipes.end()) return false; it->second->Close(); _pipes.erase(it);
        auto itc = _connById.find(dwID); if (itc != _connById.end()) { itc->second->Release(); _connById.erase(itc); }
        auto itp = _parserById.find(dwID); if (itp != _parserById.end()) { delete itp->second; _parserById.erase(itp); }
        report(PIPE_DISCONNECT, dwID);
        return true;
    }

    bool SSAPI AddListen(const char* pszLocalIP, UINT16 wLocalPort, UINT32, UINT32) override {
        if (!_net) return false;
        auto* lis = _net->CreateListener(NETIO_ASYNCSELECT);
        auto* parser = new CSDPacketParser();
        lis->SetPacketParser(parser);
        // For passive connections, create a new random id per accept; we pass 0 here, replaced on accept
        // We use factory per accept by capturing 'this' and constructing session with allocated id.
        class AcceptFactory : public ISSSessionFactory {
        public:
            AcceptFactory(PipeModule* m):_m(m){}
            ISSession* SSAPI CreateSession(ISSConnection* poConnection) override {
                UINT32 id = _m->allocId();
                auto* s = new PipeSession(_m, id); s->SetConnection(poConnection); return s;
            }
        private: PipeModule* _m;
        };
        _acceptFactories.emplace_back(new AcceptFactory(this));
        lis->SetSessionFactory(_acceptFactories.back().get());
        bool ok = lis->Start(pszLocalIP, wLocalPort, true);
        if (!ok) { delete parser; _acceptFactories.pop_back(); lis->Release(); return false; }
        _listeners.emplace_back(lis); _ownedParsers.emplace_back(parser);
        return true;
    }

    UINT32 SSAPI GetLocalID(void) override { return _localId; }

    bool SSAPI ReloadIPList(const char* ) override { return true; }
    bool SSAPI ReloadPipeConfig(const char* , const UINT32 ) override { return true; }
    bool SSAPI CheckIpValid(const char* ) override { return true; }

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
        std::lock_guard<std::mutex> lk(_mtx);
        auto it = _pipes.find(id); if (it!=_pipes.end()) it->second->onRecv(p + GetSDPkgDataOffset(p, len), len - GetSDPkgDataOffset(p, len));
    }
    void report(INT32 code, UINT32 id) {
        if (_reporter) _reporter->OnReport(code, id);
    }

    UINT32 allocId() { return ++_localId; }

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
    UINT32 _localId;
};

// factory and logger
ISSPipeModule* SSAPI SSPipeGetModule(const SSSVersion* /*pstVersion*/) { return new PipeModule(); }
bool SSAPI SSPipeSetLogger(ISSLogger* poLogger, UINT32 dwLevel) { g_pipe_logger = poLogger; g_pipe_loglevel = dwLevel; return true; }

} // namespace SSCP
