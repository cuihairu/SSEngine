#include <gtest/gtest.h>
#include "ssengine/sdprocess.h"

using namespace SSCP;

TEST(sdprocess, create_and_wait) {
    CSDProcess proc;
#if defined(_WIN32)
    const char* app = "cmd.exe";
    const char* args = "/C exit 0";
#else
    const char* app = "/bin/sh";
    const char* args = "-c exit 0";
#endif
    ASSERT_TRUE(proc.Create(app, args));
    proc.Wait();
    EXPECT_NE(proc.GetProcessID(), 0u);
}

TEST(sdprocess, current_process_id) {
    auto pid = SDGetCurrentProcessId();
    EXPECT_NE(pid, 0u);
}
