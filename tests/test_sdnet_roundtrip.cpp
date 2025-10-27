#include <gtest/gtest.h>
#include "sdnet.h"
#include "sdnet_ver.h"
#include "sdpkg.h" // use CSDPacketParser for framing
#include <atomic>
#include <thread>

using namespace SSCP;

struct LoopParser : public ISSPacketParser {
    INT32 SSAPI ParsePacket(const char* pBuf, UINT32 dwLen) override {
        // No framing: deliver whole buffer
        return (INT32)dwLen;
    }
};

struct ServerSession : public ISSSession {
    explicit ServerSession(std::atomic<int>& est, std::atomic<int>& recv, std::string& last)
        : established(est), received(recv), lastMsg(last), conn(nullptr) {}
    void SSAPI SetConnection(ISSConnection* poConnection) override { conn = poConnection; }
    void SSAPI OnEstablish(void) override { ++established; }
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) override {
        lastMsg.assign(pBuf, pBuf + dwLen);
        ++received;
        // echo back
        if (conn) conn->Send(pBuf, dwLen);
    }
    void SSAPI Release(void) override { delete this; }
    std::atomic<int>& established; std::atomic<int>& received; std::string& lastMsg; ISSConnection* conn;
};

struct ServerFactory : public ISSSessionFactory {
    ServerFactory(std::atomic<int>& est, std::atomic<int>& recv, std::string& last)
        : established(est), received(recv), lastMsg(last) {}
    ISSSession* SSAPI CreateSession(ISSConnection* poConnection) override {
        auto* s = new ServerSession(established, received, lastMsg); s->SetConnection(poConnection); return s;
    }
    std::atomic<int>& established; std::atomic<int>& received; std::string& lastMsg;
};

struct ClientSession : public ISSSession {
    explicit ClientSession(std::atomic<int>& est, std::atomic<int>& recv, std::string& last)
        : established(est), received(recv), lastMsg(last), conn(nullptr) {}
    void SSAPI SetConnection(ISSConnection* poConnection) override { conn = poConnection; }
    void SSAPI OnEstablish(void) override { ++established; }
    void SSAPI OnTerminate(void) override {}
    bool SSAPI OnError(INT32, INT32) override { return true; }
    void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) override { lastMsg.assign(pBuf, pBuf + dwLen); ++received; }
    void SSAPI Release(void) override { delete this; }
    ISSConnection* conn; std::atomic<int>& established; std::atomic<int>& received; std::string& lastMsg;
};

TEST(sdnet, roundtrip_client_server) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);

    std::atomic<int> svr_est{0}, svr_recv{0}; std::string svr_last;
    std::atomic<int> cli_est{0}, cli_recv{0}; std::string cli_last;

    ServerFactory factory(svr_est, svr_recv, svr_last);
    LoopParser parser;

    auto* listener = net->CreateListener(NETIO_ASYNCSELECT);
    listener->SetSessionFactory(&factory);
    listener->SetPacketParser(&parser);
    ASSERT_TRUE(listener->Start("127.0.0.1", 34567));

    ClientSession clientSession(cli_est, cli_recv, cli_last);

    auto* connector = net->CreateConnector(NETIO_ASYNCSELECT);
    connector->SetSession(&clientSession);
    connector->SetPacketParser(&parser);
    ASSERT_EQ(connector->Connect("127.0.0.1", 34567), NET_SUCCESS);

    // pump events until both connected
    for (int i=0; i<100 && (svr_est.load()==0 || cli_est.load()==0); ++i) {
        net->Run(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_GT(svr_est.load(), 0);
    EXPECT_GT(cli_est.load(), 0);

    // Send data from client to server
    const char* msg = "ping";
    if (connector->GetSession()) {
        auto* c = clientSession.conn; // not set via connector; use ISSession SetConnection called by impl
        // If connection pointer not available here, we can't directly send; but server echoes back upon receiving
    }
    // We can use client's session's connection if assigned
    ASSERT_NE(clientSession.conn, nullptr);
    clientSession.conn->Send(msg, 4);

    for (int i=0; i<100 && svr_recv.load()==0; ++i) {
        net->Run(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(svr_last, std::string("ping",4));

    for (int i=0; i<100 && cli_recv.load()==0; ++i) {
        net->Run(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(cli_last, std::string("ping",4));

    listener->Stop();
    listener->Release();
    connector->Release();
    net->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
