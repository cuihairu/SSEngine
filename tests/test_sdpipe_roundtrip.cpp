#include <gtest/gtest.h>
#include "sdpipe.h"
#include "sdnet.h"
#include "sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct PipeReporter : public ISSPipeReporter {
    void SSAPI OnReport(INT32 nErrCode, UINT32 dwID) override {
        (void)nErrCode; (void)dwID;
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
#if defined(_WIN32)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);

    PipeReporter reporter; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &reporter, net));

    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45678));

    UINT32 id = 0x01020304;
    ASSERT_TRUE(pipe->AddConn(id, "127.0.0.1", 45678));

    // Pump until established
    for (int i=0;i<100;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

    auto* p = pipe->GetPipe(id);
    ASSERT_NE(p, nullptr);

    std::atomic<int> recvc{0}; std::string last;
    PipeSink sink(recvc, last);
    ASSERT_TRUE(p->SetSink(7, &sink));

    const char* msg = "hello-pipe";
    ASSERT_TRUE(p->Send(7, msg, (UINT32)strlen(msg)));

    for (int i=0;i<100 && recvc.load()==0;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_EQ(last, std::string("hello-pipe"));

    pipe->Run(10);
    net->Release();
    pipe->Release();
#else
    GTEST_SKIP() << "Windows sdpipe roundtrip only";
#endif
}
