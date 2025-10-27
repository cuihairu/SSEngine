#include <gtest/gtest.h>
#include "sdlogger.h"

using namespace SSCP;

TEST(sdlogger, file_logger_factory_not_null) {
    auto* p = SDCreateFileLogger(&SDLOGGER_VERSION);
    ASSERT_NE(p, nullptr);
    p->Release();
}

TEST(sdlogger, file_logger_basic_init_and_log) {
    auto* p = SDCreateFileLogger(&SDLOGGER_VERSION);
    ASSERT_TRUE(p->Init(".", "sse_log", LOG_MODE_SINGLE_FILE));
    EXPECT_TRUE(p->LogText("hello"));
    UINT8 buf[4] = {1,2,3,4};
    EXPECT_TRUE(p->LogBinary(buf, 4));
    p->Release();
}
