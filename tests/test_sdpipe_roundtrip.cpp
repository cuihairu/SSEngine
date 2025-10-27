#include <gtest/gtest.h>
#include "ssengine/sdpipe.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct PipeReporter : public ISSPipeReporter {
    std::vector<UINT32> ids;
    void SSAPI OnReport(INT32 nErrCode, UINT32 dwID) override {
        if (nErrCode == PIPE_SUCCESS) ids.push_back(dwID);
    }
};

struct PipeSink : public ISSPipeSink {
    std::atomic<int>& count; std::string& last;
    PipeSink(std::atomic<int>& c, std::string& s):count(c),last(s){}
    void SSAPI OnRecv(UINT16 wBusinessID, const char* pData, UINT32 dwLen) override {
        EXPECT_EQ(wBusinessID, 7);
        last.assign(pData, pData + dwLen);
        ++count;
    }
    void SSAPI OnReport(UINT16, INT32) override {}
};

TEST(sdpipe, roundtrip) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);

    PipeReporter reporter; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &reporter, net));

    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45678));

    UINT32 clientId = 0x01020304;
    ASSERT_TRUE(pipe->AddConn(clientId, "127.0.0.1", 45678));

    // Pump until established
    for (int i=0;i<100;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

    auto* p = pipe->GetPipe(clientId);
    ASSERT_NE(p, nullptr);

    std::atomic<int> recvc{0}; std::string last;
    PipeSink sink(recvc, last);
    ASSERT_TRUE(p->SetSink(7, &sink));

    // Attach echo sink on server side to bounce back
    UINT32 serverId = 0;
    for (int i=0;i<50 && serverId==0;++i) {
        if (!reporter.ids.empty()) {
            // the reporter captures both client and server ids; pick the one not equal to clientId
            for (auto id : reporter.ids) if (id != clientId) { serverId = id; break; }
        }
        net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_NE(serverId, 0u);
    auto* sp = pipe->GetPipe(serverId);
    ASSERT_NE(sp, nullptr);
    struct EchoSink : public ISSPipeSink {
        ISSPipe* pipe;
        EchoSink(ISSPipe* p):pipe(p){}
        void SSAPI OnRecv(UINT16 bid, const char* d, UINT32 n) override { pipe->Send(bid, d, n); }
        void SSAPI OnReport(UINT16, INT32) override {}
    } echo(sp);
    ASSERT_TRUE(sp->SetSink(7, &echo));

    const char* msg = "hello-pipe";
    ASSERT_TRUE(p->Send(7, msg, (UINT32)strlen(msg)));

    for (int i=0;i<100 && recvc.load()==0;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_EQ(last, std::string("hello-pipe"));

    pipe->Run(10);
    net->Release();
    pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
