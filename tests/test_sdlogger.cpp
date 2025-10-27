#include <gtest/gtest.h>
#include "sdlogger.h"
#include <string>

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

namespace {
class TestLogger : public ISSLogger {
public:
    bool SSAPI LogText(const char* pszLog) override {
        last = pszLog ? pszLog : ""; return true;
    }
    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        (void)pLog; (void)dwLen; return true;
    }
    std::string last;
};
}

TEST(sdlogger, csdlogger_delegates_to_logger) {
    TestLogger tl;
    CSDLogger clogger;
    clogger.SetLogger(&tl);
    clogger.Info("hello %d", 42);
    EXPECT_NE(tl.last.find("hello 42"), std::string::npos);
    clogger.Warn("warn");
    EXPECT_NE(tl.last.find("warn"), std::string::npos);
}
