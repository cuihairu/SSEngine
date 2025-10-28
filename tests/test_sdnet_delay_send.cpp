#include <gtest/gtest.h>
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct AnyParser : public ISSPacketParser { INT32 SSAPI ParsePacket(const char* p, UINT32 n) override { return (INT32)n; } };

struct EchoSession : public ISSSession {
    ISSConnection* conn{nullptr};
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char* p, UINT32 n) override { if (conn) conn->Send(p, n); }
    void SSAPI Release(void) override { delete this; }
};

struct CountSession : public ISSSession {
    std::atomic<int>& cnt; std::string& last; ISSConnection* conn{nullptr};
    CountSession(std::atomic<int>& c, std::string& s):cnt(c),last(s){}
    void SSAPI SetConnection(ISSConnection* c) override { conn=c; }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char* p, UINT32 n) override { last.assign(p, p+n); ++cnt; }
    void SSAPI Release(void) override { delete this; }
};

TEST(sdnet, delay_send_roundtrip) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    AnyParser parser;

    struct Fac : public ISSSessionFactory { ISSSession* SSAPI CreateSession(ISSConnection* c) override { auto* s=new EchoSession(); s->SetConnection(c); return s; } } fac;
    auto* lis = net->CreateListener(NETIO_ASYNCSELECT);
    lis->SetSessionFactory(&fac); lis->SetPacketParser(&parser);
    ASSERT_TRUE(lis->Start("127.0.0.1", 34584));

    std::atomic<int> recvc{0}; std::string last;
    CountSession cs(recvc, last);
    auto* con = net->CreateConnector(NETIO_ASYNCSELECT);
    con->SetSession(&cs); con->SetPacketParser(&parser);
    ASSERT_EQ(con->Connect("127.0.0.1", 34584), NET_SUCCESS);

    const char* msg = "delayed";
    for (int i=0;i<50 && cs.conn==nullptr;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    ASSERT_NE(cs.conn, nullptr);
    cs.conn->DelaySend(msg, 7);

    for (int i=0;i<100 && recvc.load()==0;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_EQ(last, std::string("delayed",7));

    lis->Stop(); lis->Release(); con->Release(); net->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}

