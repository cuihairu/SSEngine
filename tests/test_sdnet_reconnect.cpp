#include <gtest/gtest.h>
#include "sdnet.h"
#include "sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct SimpleParser : public ISSPacketParser {
    INT32 SSAPI ParsePacket(const char* p, UINT32 n) override { return (INT32)n; }
};

struct ReConnSession : public ISSSession {
    std::atomic<int>& estab;
    ReConnSession(std::atomic<int>& e):estab(e),conn(nullptr){}
    void SSAPI SetConnection(ISSConnection* c) override { conn = c; }
    void SSAPI OnEstablish(void) override { ++estab; }
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char*, UINT32) override {}
    void SSAPI Release(void) override { delete this; }
    ISSConnection* conn;
};

TEST(sdnet, reconnect_flow) {
#if defined(_WIN32)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);

    // listener
    SimpleParser parser;
    auto* lis = net->CreateListener(NETIO_ASYNCSELECT);
    struct Fac : public ISSSessionFactory { ISSession* SSAPI CreateSession(ISSConnection* c) override { auto* s = new ReConnSession(dummy); s->SetConnection(c); return s; } std::atomic<int> dummy{0}; } fac;
    lis->SetSessionFactory(&fac);
    lis->SetPacketParser(&parser);
    ASSERT_TRUE(lis->Start("127.0.0.1", 34569));

    std::atomic<int> est{0};
    ReConnSession client(est);
    auto* con = net->CreateConnector(NETIO_ASYNCSELECT);
    con->SetSession(&client);
    con->SetPacketParser(&parser);
    ASSERT_EQ(con->Connect("127.0.0.1", 34569), NET_SUCCESS);

    for (int i=0;i<50 && est.load()==0;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GT(est.load(), 0);

    ASSERT_NE(client.conn, nullptr);
    client.conn->Disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_EQ(con->ReConnect(), NET_SUCCESS);
    for (int i=0;i<50 && est.load()<2;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(est.load(), 2);

    lis->Stop();
    lis->Release();
    con->Release();
    net->Release();
#else
    GTEST_SKIP() << "Windows only";
#endif
}

