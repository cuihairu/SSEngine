// Windows-first skeleton for sdnet, with cross-platform no-op fallback.
#include "sdnet.h"
#include "sdnetopt.h"
#include "sdnet_ver.h"
#include <string>
#include <memory>

namespace SSCP {

static ISSLogger* g_logger = nullptr;
static UINT32 g_log_level = 0;

class ConnectionImpl : public ISSConnection {
public:
    bool SSAPI IsConnected(void) override { return false; }
    void SSAPI Send(const char* /*pBuf*/,UINT32 /*dwLen*/) override {}
    void SSAPI DelaySend(const char* /*pBuf*/,UINT32 /*dwLen*/) override {}
    void SSAPI SetOpt(UINT32 /*dwType*/, void* /*pOpt*/) override {}
    void SSAPI Disconnect(void) override {}
    const UINT32 SSAPI GetRemoteIP(void) override { return 0; }
    const char* SSAPI GetRemoteIPStr(void) override { return "0.0.0.0"; }
    UINT16 SSAPI GetRemotePort(void) override { return 0; }
    const UINT32 SSAPI GetLocalIP(void) override { return 0; }
    const char* SSAPI GetLocalIPStr(void) override { return "0.0.0.0"; }
    UINT16 SSAPI GetLocalPort(void) override { return 0; }
    UINT32 SSAPI GetSendBufFree(void) override { return 0; }
};

class ListenerImpl : public ISSListener {
public:
    void SSAPI SetPacketParser(ISSPacketParser* /*poPacketParser*/) override {}
    void SSAPI SetSessionFactory(ISSSessionFactory* /*poSessionFactory*/) override {}
    void SSAPI SetBufferSize(UINT32 /*dwRecvBufSize*/, UINT32 /*dwSendBufSize*/) override {}
    void SSAPI SetOpt(UINT32 /*dwType*/, void* /*pOpt*/) override {}
    bool SSAPI Start(const char* /*pszIP*/, UINT16 /*wPort*/, bool /*bReUseAddr*/ = true) override { return false; }
    bool SSAPI Stop(void) override { return true; }
    void SSAPI Release(void) override { delete this; }
};

class ConnectorImpl : public ISSConnector {
public:
    ConnectorImpl() : _session(nullptr), _parser(nullptr) {}
    void SSAPI SetPacketParser(ISSPacketParser* poPakcetParser) override { _parser = poPakcetParser; }
    void SSAPI SetSession(ISSSession* poSession) override { _session = poSession; }
    ISSSession* SSAPI GetSession() override { return _session; }
    void SSAPI SetBufferSize(UINT32 /*dwRecvBufSize*/, UINT32 /*dwSendBufSize*/) override {}
    int SSAPI Connect(const char* /*pszIP*/, UINT16 /*wPort*/) override { return NET_CONNECT_FAIL; }
    int SSAPI ReConnect(void) override { return NET_CONNECT_FAIL; }
    void SSAPI Release(void) override { delete this; }
    void SSAPI SetOpt(UINT32 /*dwType*/, void* /*pOpt*/) override {}
private:
    ISSSession* _session;
    ISSPacketParser* _parser;
};

class NetImpl : public ISSNet {
public:
    void SSAPI AddRef(void) override {}
    UINT32 SSAPI QueryRef(void) override { return 1; }
    void SSAPI Release(void) override { delete this; }
    SSSVersion SSAPI GetVersion(void) override { return SDNET_MODULE_VERSION; }
    const char * SSAPI GetModuleName(void) override { return SDNET_MODULENAME; }

    ISSConnector* SSAPI CreateConnector(UINT32 /*dwNetIOType*/) override {
        return new ConnectorImpl();
    }
    ISSListener* SSAPI CreateListener(UINT32 /*dwNetIOType*/) override {
        return new ListenerImpl();
    }
    bool SSAPI Run(INT32 /*nCount*/ = -1) override { return true; }
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

