#include <gtest/gtest.h>
#include "sdpipe.h"
#include "sdnet.h"
#include "sdnet_ver.h"
#include <atomic>
#include <thread>
#include <vector>

using namespace SSCP;

struct CapturingReporter : public ISSPipeReporter {
    std::atomic<int> success{0};
    std::atomic<int> disconnect{0};
    std::vector<UINT32> ids;
    void SSAPI OnReport(INT32 nErrCode, UINT32 dwID) override {
        if (nErrCode == PIPE_SUCCESS) { ++success; ids.push_back(dwID); }
        if (nErrCode == PIPE_DISCONNECT) { ++disconnect; }
    }
};

TEST(sdpipe, reporter_success_and_disconnect) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);
    CapturingReporter rep; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &rep, net));
    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45682));
    UINT32 id = 0x11223344; ASSERT_TRUE(pipe->AddConn(id, "127.0.0.1", 45682));
    for (int i=0;i<100 && rep.success.load() < 2; ++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(rep.success.load(), 2); // server+client attach
    EXPECT_TRUE(pipe->RemoveConn(id));
    for (int i=0;i<100 && rep.disconnect.load() < 2; ++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(rep.disconnect.load(), 1); // at least client disconnect; server side may also report on close
    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
