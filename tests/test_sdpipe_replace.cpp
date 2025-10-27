#include <gtest/gtest.h>
#include "ssengine/sdpipe.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <thread>

using namespace SSCP;

struct DummyReporter : public ISSPipeReporter { void SSAPI OnReport(INT32, UINT32) override {} };

struct CntSink : public ISSPipeSink {
    std::atomic<int>& c; std::string& last;
    CntSink(std::atomic<int>& cc, std::string& s):c(cc),last(s){}
    void SSAPI OnRecv(UINT16, const char* d, UINT32 n) override { last.assign(d, d+n); ++c; }
    void SSAPI OnReport(UINT16, INT32) override {}
};

TEST(sdpipe, replace_conn_keeps_sinks) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);

    DummyReporter rep; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &rep, net));
    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45679));

    UINT32 id = 0x0A0B0C0D;
    ASSERT_TRUE(pipe->AddConn(id, "127.0.0.1", 45679));

    for (int i=0;i<50;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

    auto* p = pipe->GetPipe(id);
    ASSERT_NE(p, nullptr);
    std::atomic<int> cnt{0}; std::string last;
    CntSink sink(cnt, last); ASSERT_TRUE(p->SetSink(9, &sink));

    ASSERT_TRUE(pipe->ReplaceConn(id, "127.0.0.1", 45679));
    for (int i=0;i<50;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

    // Send after replace; sink should still be active
    ASSERT_TRUE(p->Send(9, "R", 1));
    for (int i=0;i<50 && cnt.load()==0;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_EQ(last, std::string("R",1));

    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
