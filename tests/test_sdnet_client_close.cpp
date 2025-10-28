#include <gtest/gtest.h>
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct PassthroughParser : public ISSPacketParser { INT32 SSAPI ParsePacket(const char* p, UINT32 n) override { return (INT32)n; } };

struct ServerTermSession : public ISSSession {
    std::atomic<int>& term;
    ServerTermSession(std::atomic<int>& t):term(t),conn(nullptr){}
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override { ++term; }
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char*, UINT32) override {}
    void SSAPI Release(void) override { delete this; }
    ISSConnection* conn;
};

struct ClientCloseSession : public ISSSession {
    ISSConnection* conn{nullptr};
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override { if (conn) conn->Disconnect(); }
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char*, UINT32) override {}
    void SSAPI Release(void) override { delete this; }
};

TEST(sdnet, client_close_triggers_server_terminate) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    PassthroughParser parser;

    std::atomic<int> server_term{0};
    struct Fac : public ISSSessionFactory { std::atomic<int>& t; Fac(std::atomic<int>& tt):t(tt){} ISSSession* SSAPI CreateSession(ISSConnection* c) override { auto* s=new ServerTermSession(t); s->SetConnection(c); return s; } } fac(server_term);
    auto* lis = net->CreateListener(NETIO_ASYNCSELECT);
    lis->SetSessionFactory(&fac); lis->SetPacketParser(&parser);
    ASSERT_TRUE(lis->Start("127.0.0.1", 34583));

    ClientCloseSession cs;
    auto* con = net->CreateConnector(NETIO_ASYNCSELECT);
    con->SetSession(&cs); con->SetPacketParser(&parser);
    ASSERT_EQ(con->Connect("127.0.0.1", 34583), NET_SUCCESS);

    for (int i=0;i<100 && server_term.load()==0;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(server_term.load(), 1);

    lis->Stop(); lis->Release(); con->Release(); net->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}

