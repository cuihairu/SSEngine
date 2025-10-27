#include <gtest/gtest.h>
#include "ssengine/sdpipe.h"
#include "ssengine/sdnet.h"
#include "ssengine/sdnet_ver.h"
#include <fstream>

using namespace SSCP;

TEST(sdpipe, whitelist_addconn_blocks_disallowed) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    auto* pipe = SSPipeGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(pipe, nullptr);
    ASSERT_TRUE(pipe->Init(nullptr, nullptr, nullptr, net));

    // Write whitelist with a bogus IP
    const char* f = "iplist.tmp";
    {
        std::ofstream ofs(f); ofs << "192.0.2.1\n"; // TEST-NET-1 address
    }
    ASSERT_TRUE(pipe->ReloadIPList(f));
    // Attempt to connect to 127.0.0.1 should fail under whitelist
    EXPECT_FALSE(pipe->AddConn(0x55, "127.0.0.1", 45685));

    // Now allow 127.0.0.1
    {
        std::ofstream ofs(f); ofs << "127.0.0.1\n";
    }
    ASSERT_TRUE(pipe->ReloadIPList(f));
    // Should succeed AddConn to loopback (listener not required to exist for this test)
    // We will not Run(), just verify AddConn returns true
    EXPECT_TRUE(pipe->AddConn(0x56, "127.0.0.1", 45685));

    net->Release(); pipe->Release();
#else
    GTEST_SKIP() << "Windows/Linux/macOS only";
#endif
}

