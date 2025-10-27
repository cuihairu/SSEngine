#include <gtest/gtest.h>
#include "sdpipe.h"
#include "sdnet.h"
#include "sdnet_ver.h"
#include <thread>

using namespace SSCP;

struct DummyReporter2 : public ISSPipeReporter { void SSAPI OnReport(INT32, UINT32) override {} };

TEST(sdpipe, remove_conn_erases_pipe) {
#if defined(_WIN32)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);
    DummyReporter2 rep; ASSERT_TRUE(pipe->Init(nullptr, nullptr, &rep, net));
    ASSERT_TRUE(pipe->AddListen("127.0.0.1", 45680));
    UINT32 id = 0x0A0B0C0E;
    ASSERT_TRUE(pipe->AddConn(id, "127.0.0.1", 45680));
    for (int i=0;i<50;++i) { net->Run(10); pipe->Run(10); std::this_thread::sleep_for(std::chrono::milliseconds(5)); }
    ASSERT_NE(pipe->GetPipe(id), nullptr);
    EXPECT_TRUE(pipe->RemoveConn(id));
    EXPECT_EQ(pipe->GetPipe(id), nullptr);
    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows only";
#endif
}

