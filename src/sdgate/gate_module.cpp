#include "ssengine/sdgate.h"

#include "ssengine/sdlogger.h"
#include "ssengine/sdnet.h"

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace SSCP {

namespace {

ISSLogger* g_gateLogger = nullptr;
UINT32 g_gateLogLevel = LOGLV_INFO | LOGLV_WARN | LOGLV_CRITICAL;
std::mutex g_loggerMutex;

void logInfo(const std::string& text) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    if (g_gateLogger && (g_gateLogLevel & LOGLV_INFO)) {
        g_gateLogger->LogText(text.c_str());
    }
}

class MockGate;

class MockServerConnection : public ISSServerConnection {
public:
    MockServerConnection(MockGate& owner, ISSServerSession* session, ISSPacketParser* /*parser*/)
        : m_owner(owner)
        , m_session(session)
        , m_connected(false)
        , m_remoteIP(0)
        , m_remotePort(0)
        , m_localIP(0)
        , m_localPort(0)
        , m_recvBuf(0)
        , m_sendBuf(0) {
        if (m_session) {
            m_session->SetConnection(this);
        }
    }

    ~MockServerConnection() override = default;

    ISSServerSession* SSAPI GetSession(void) override { return m_session; }
    void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) override {
        m_recvBuf = dwRecvBufSize;
        m_sendBuf = dwSendBufSize;
    }

    int SSAPI Connect(const char* pszIP, UINT16 wPort, bool /*bAutoReconnect*/) override {
        m_connected = true;
        m_remoteIpStr = pszIP ? pszIP : "";
        m_remotePort = wPort;
        if (m_session) {
            m_session->OnConnect(true);
        }
        logInfo("MockServerConnection connected to " + m_remoteIpStr);
        return NET_SUCCESS;
    }

    void SSAPI Close(void) override {
        if (!m_connected) return;
        m_connected = false;
        if (m_session) {
            m_session->OnClose();
        }
    }

    bool SSAPI SendServerData(const char* pData, UINT32 nLen) override {
        if (!m_connected || !m_session) return false;
        m_session->OnRecvServerData(pData, nLen);
        return true;
    }

    bool SSAPI SendClientData(ISSClientConnection* poClient, const char* pData, UINT32 nLen) override {
        if (!m_connected || !m_session) return false;
        m_session->OnRecvClientData(poClient, pData, nLen);
        return true;
    }

    bool SSAPI Enter(ISSClientConnection* poClient, UINT64 dwTransID) override {
        if (!m_session) return false;
        m_session->OnEnter(poClient, SDGATEERR_SUCCESS, dwTransID);
        return true;
    }

    bool SSAPI Leave(ISSClientConnection* poClient, UINT64 dwTransID) override {
        if (!m_session) return false;
        m_session->OnLeave(poClient, SDGATEERR_SUCCESS, dwTransID);
        return true;
    }

    UINT32 SSAPI GetRemoteIP(void) override { return m_remoteIP; }
    UINT16 SSAPI GetRemotePort(void) override { return m_remotePort; }
    UINT32 SSAPI GetLocalIP(void) override { return m_localIP; }
    UINT16 SSAPI GetLocalPort(void) override { return m_localPort; }
    bool SSAPI SetOpt(INT32, void*) override { return true; }

    void SSAPI Release(void) override;

private:
    MockGate& m_owner;
    ISSServerSession* m_session;
    bool m_connected;
    UINT32 m_remoteIP;
    UINT16 m_remotePort;
    UINT32 m_localIP;
    UINT16 m_localPort;
    UINT32 m_recvBuf;
    UINT32 m_sendBuf;
    std::string m_remoteIpStr;
};

struct ListenerInfo {
    std::string ip;
    UINT16 port{0};
};

class MockGate : public ISSGate {
public:
    explicit MockGate(ISSNet* net)
        : m_ref(1)
        , m_net(net)
        , m_clientParser(nullptr)
        , m_sessionFactory(nullptr)
        , m_clientRecvBuf(0)
        , m_clientSendBuf(0)
        , m_nextListenerId(1) {}

    ~MockGate() override {
        std::vector<MockServerConnection*> toRelease;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            for (auto* conn : m_serverConnections) {
                toRelease.push_back(conn);
            }
            m_serverConnections.clear();
            m_listeners.clear();
        }
        for (auto* conn : toRelease) {
            if (conn) {
                conn->Release();
            }
        }
    }

    void SSAPI AddRef(void) override { m_ref.fetch_add(1); }
    UINT32 SSAPI QueryRef(void) override { return m_ref.load(); }
    void SSAPI Release(void) override {
        if (m_ref.fetch_sub(1) == 1) {
            delete this;
        }
    }

    SSSVersion SSAPI GetVersion(void) override { return SDGATE_VERSION; }
    const char* SSAPI GetModuleName(void) override { return SDGATE_MODULENAME; }

    ISSServerConnection* SSAPI CreateServerConnection(ISSServerSession* poSession, ISSPacketParser* poSvrPacketParser = nullptr) override {
        auto* conn = new MockServerConnection(*this, poSession, poSvrPacketParser);
        std::lock_guard<std::mutex> lk(m_mutex);
        m_serverConnections.insert(conn);
        return conn;
    }

    void SSAPI SetClientPacketParser(ISSPacketParser* poPacketParser) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_clientParser = poPacketParser;
    }

    void SSAPI SetClientSessionFactory(ISSClientSessionFactory* poFactory) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_sessionFactory = poFactory;
    }

    bool SSAPI SetClientBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_clientRecvBuf = dwRecvBufSize;
        m_clientSendBuf = dwSendBufSize;
        return true;
    }

    INT32 SSAPI AddListen(const char* pszIP, UINT16 wPort) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        INT32 id = m_nextListenerId++;
        m_listeners.emplace(id, ListenerInfo{pszIP ? pszIP : "", wPort});
        logInfo("MockGate listening on id " + std::to_string(id));
        return id;
    }

    bool SSAPI DelListen(INT32 nID) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_listeners.erase(nID) > 0;
    }

    bool SSAPI Run(INT32 /*nCount*/) override {
        // Nothing asynchronous to drive in mock implementation.
        return true;
    }

    bool SSAPI SetOpt(INT32, void*) override { return true; }

    void removeServerConnection(MockServerConnection* conn) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_serverConnections.erase(conn);
    }

private:
    std::atomic<UINT32> m_ref;
    ISSNet* m_net;
    std::mutex m_mutex;
    ISSPacketParser* m_clientParser;
    ISSClientSessionFactory* m_sessionFactory;
    UINT32 m_clientRecvBuf;
    UINT32 m_clientSendBuf;
    INT32 m_nextListenerId;
    std::unordered_map<INT32, ListenerInfo> m_listeners;
    std::unordered_set<MockServerConnection*> m_serverConnections;
};

void MockServerConnection::Release(void) {
    m_owner.removeServerConnection(this);
    delete this;
}

} // namespace

ISSGate* SSAPI SSCreateGate(const SSSVersion* /*pstVersion*/, ISSNet* poNet) {
    return new MockGate(poNet);
}

bool SSAPI SSGateSetLogger(ISSLogger* poLogger, UINT32 dwLevel) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    g_gateLogger = poLogger;
    g_gateLogLevel = dwLevel;
    return true;
}

} // namespace SSCP
