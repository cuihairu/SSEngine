#include <gtest/gtest.h>
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct AllParser : public ISSPacketParser { INT32 SSAPI ParsePacket(const char* p, UINT32 n) override { return (INT32)n; } };

struct CloseOnEstablish : public ISSSession {
    ISSConnection* conn{nullptr}; std::atomic<int>& closed;
    CloseOnEstablish(std::atomic<int>& c):closed(c){}
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override { if (conn) conn->Disconnect(); }
    void SSAPI OnTerminate(void) override { ++closed; }
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char*, UINT32) override {}
    void SSAPI Release(void) override { delete this; }
};

struct ClientTermSession : public ISSSession {
    std::atomic<int>& term;
    bool autoDelete;
    ClientTermSession(std::atomic<int>& t, bool selfDelete = false)
        : term(t), autoDelete(selfDelete), conn(nullptr) {}
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override { ++term; }
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char*, UINT32) override {}
    void SSAPI Release(void) override { if (autoDelete) delete this; }
    ISSConnection* conn;
};

TEST(sdnet, server_close_triggers_client_terminate) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    AllParser parser;

    // server listener closes immediately upon establish
    std::atomic<int> server_closed{0};
    struct Fac : public ISSSessionFactory { std::atomic<int>& c; Fac(std::atomic<int>& cc):c(cc){} ISSSession* SSAPI CreateSession(ISSConnection* po) override { auto* s=new CloseOnEstablish(c); s->SetConnection(po); return s; } } fac(server_closed);
    auto* lis = net->CreateListener(NETIO_ASYNCSELECT);
    lis->SetSessionFactory(&fac); lis->SetPacketParser(&parser);
    ASSERT_TRUE(lis->Start("127.0.0.1", 34581));

    // client waits for terminate
    std::atomic<int> client_term{0}; ClientTermSession cs(client_term);
    auto* con = net->CreateConnector(NETIO_ASYNCSELECT);
    con->SetSession(&cs); con->SetPacketParser(&parser);
    ASSERT_EQ(con->Connect("127.0.0.1", 34581), NET_SUCCESS);

    for (int i=0;i<100 && client_term.load()==0;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(client_term.load(), 1);

    lis->Stop(); lis->Release(); con->Release(); net->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
