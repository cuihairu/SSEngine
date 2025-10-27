#include <gtest/gtest.h>
#include "sdlogger.h"
#include "sdfile.h"
#include <chrono>
#include <ctime>
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

#if defined(_WIN32)
static std::string today_suffix() {
    using namespace std::chrono;
    auto now = system_clock::now(); std::time_t tt = system_clock::to_time_t(now);
    std::tm tm{}; localtime_s(&tm, &tt);
    char buf[32]; std::snprintf(buf, sizeof(buf), "_%04d%02d%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
    return std::string(buf);
}

TEST(sdlogger, file_logger_day_rolling_creates_expected_file) {
    auto* p = SDCreateFileLogger(&SDLOGGER_VERSION);
    ASSERT_TRUE(p->Init(".", "sse_day_test", LOG_MODE_DAY_DIVIDE));
    EXPECT_TRUE(p->LogText("day rolling"));
    p->Release();
    std::string expect = std::string("./sse_day_test") + today_suffix() + ".log";
    EXPECT_TRUE(SDFileExists(expect.c_str()));
    SDFileRemove(expect.c_str());
}

TEST(sdlogger, udp_logger_basic_send) {
    auto* p = SDCreateUdpLogger(&SDLOGGER_VERSION);
    ASSERT_TRUE(p->Init("127.0.0.1", 9, 0)); // discard port or closed, UDP send still ok
    EXPECT_TRUE(p->LogText("hello udp"));
    p->Release();
}

TEST(sdlogger, tcp_logger_connect_fail_writes_errorfile) {
    const char* errfile = "tcp_error.tmp";
    SDFileRemove(errfile);
    auto* p = SDCreateTcpLogger(&SDLOGGER_VERSION);
    // Port 1 usually not listening; Init returns false but should write errorfile
    EXPECT_FALSE(p->Init("127.0.0.1", 1, 0, errfile));
    p->Release();
    EXPECT_TRUE(SDFileExists(errfile));
    EXPECT_GT(SDFileGetSize(errfile), 0);
    SDFileRemove(errfile);
}
#endif
