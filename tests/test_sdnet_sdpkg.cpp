#include <gtest/gtest.h>
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include "ssengine/sdpkg.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct PkgParser : public ISSPacketParser {
    INT32 SSAPI ParsePacket(const char* pBuf, UINT32 dwLen) override {
        return CheckSDPkgHead(pBuf, dwLen);
    }
};

struct PkgSession : public ISSSession {
    std::atomic<int>& cnt;
    bool autoDelete;
    PkgSession(std::atomic<int>& c, bool selfDelete)
        : cnt(c), autoDelete(selfDelete), conn(nullptr) {}
    void SSAPI SetConnection(ISSConnection* c) override { conn = c; }
    void SSAPI OnEstablish(void) override {}
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) override {
        // validate head and count
        EXPECT_GT(dwLen, sizeof(SSDPkgHead16));
        EXPECT_NE(CheckSDPkgHead(pBuf, dwLen), -1);
        ++cnt;
    }
    void SSAPI Release(void) override { if (autoDelete) delete this; }
    ISSConnection* conn;
};

static std::string make_pkg(const char* payload, UINT16 len) {
    std::string buf; buf.resize(sizeof(SSDPkgHead16) + len);
    auto* head = reinterpret_cast<SSDPkgHead16*>(&buf[0]);
    BuildSDPkgHead16(head, len);
    std::memcpy(&buf[sizeof(SSDPkgHead16)], payload, len);
    return buf;
}

TEST(sdnet, sdpkg_sticky_and_split) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);

    PkgParser parser;
    std::atomic<int> svr_recv{0};

    // Start listener
    auto* listener = net->CreateListener(NETIO_ASYNCSELECT);
    struct Factory : public ISSSessionFactory {
        std::atomic<int>& r; PkgParser& p; Factory(std::atomic<int>& rr, PkgParser& pp):r(rr),p(pp){}
        ISSSession* SSAPI CreateSession(ISSConnection* c) override { auto* s = new PkgSession(r, true); s->SetConnection(c); return s; }
    } fac(svr_recv, parser);
    listener->SetSessionFactory(&fac);
    listener->SetPacketParser(&parser);
    ASSERT_TRUE(listener->Start("127.0.0.1", 34568));

    // Connect client
    std::atomic<int> cli_est{0};
    PkgSession client(cli_est, false); // reuse PkgSession as client session to get connection pointer
    auto* connector = net->CreateConnector(NETIO_ASYNCSELECT);
    connector->SetSession(&client);
    connector->SetPacketParser(&parser);
    ASSERT_EQ(connector->Connect("127.0.0.1", 34568), NET_SUCCESS);

    for (int i=0;i<50 && client.conn==nullptr;++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    ASSERT_NE(client.conn, nullptr);

    // sticky: send two pkgs together
    auto p1 = make_pkg("A", 1);
    auto p2 = make_pkg("BC", 2);
    std::string both = p1 + p2;
    client.conn->Send(both.data(), (UINT32)both.size());

    // split: send one pkg in two parts
    auto p3 = make_pkg("DEFG", 4);
    client.conn->Send(p3.data(), (UINT32)(p3.size()/2));
    client.conn->Send(p3.data() + p3.size()/2, (UINT32)(p3.size() - p3.size()/2));

    for (int i=0;i<100 && svr_recv.load()<3; ++i) { net->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(svr_recv.load(), 3);

    listener->Stop(); listener->Release(); connector->Release(); net->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
