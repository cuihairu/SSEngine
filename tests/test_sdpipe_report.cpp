#include <gtest/gtest.h>
#include "ssengine/sdpipe.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

using namespace SSCP;

struct CapturingReporter : public ISSPipeReporter {
    std::atomic<int> success{0};
    std::atomic<int> disconnect{0};
    std::atomic<int> repeat{0};
    std::atomic<int> remote{0};
    std::vector<UINT32> ids;
    std::mutex guard;
    void SSAPI OnReport(INT32 nErrCode, UINT32 dwID) override {
        if (nErrCode == PIPE_SUCCESS) {
            ++success;
            std::lock_guard<std::mutex> lk(guard);
            ids.push_back(dwID);
        }
        if (nErrCode == PIPE_DISCONNECT) { ++disconnect; }
        if (nErrCode == PIPE_REPEAT_CONN) { ++repeat; }
        if (nErrCode == PIPE_REMOTEID_ERR) { ++remote; }
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
    for (int i=0;i<50; ++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    for (int i=0;i<100 && rep.disconnect.load() < 2; ++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_GE(rep.disconnect.load(), 1); // at least client disconnect; server side may also report on close
    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}

TEST(sdpipe, reporter_repeat_connection_code) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);
    CapturingReporter rep; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &rep, net));
    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45683));
    UINT32 id = 0x55667788;
    ASSERT_TRUE(pipe->AddConn(id, "127.0.0.1", 45683));
    EXPECT_FALSE(pipe->AddConn(id, "127.0.0.1", 45683));
    EXPECT_EQ(rep.repeat.load(), 1);
    for (int i=0;i<50; ++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    EXPECT_TRUE(pipe->RemoveConn(id));
    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}

TEST(sdpipe, reporter_remoteid_error_code) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);
    CapturingReporter rep; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &rep, net));
    EXPECT_FALSE(pipe->ReplaceConn(0xAABBCCDD, "127.0.0.1", 45684));
    EXPECT_EQ(rep.remote.load(), 1);
    EXPECT_FALSE(pipe->RemoveConn(0xAABBCCDD));
    EXPECT_EQ(rep.remote.load(), 2);
    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}
