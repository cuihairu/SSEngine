#include "ssengine/sdconsole.h"

#include "ssengine/sdlogger.h"

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace SSCP {

namespace {

ISSLogger* g_consoleLogger = nullptr;
UINT32 g_consoleLogLevel = LOGLV_DEBUG | LOGLV_INFO | LOGLV_WARN | LOGLV_CRITICAL;
std::mutex g_loggerMutex;
std::mutex g_stdoutMutex;

std::string timestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
#ifdef WINDOWS
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tm.tm_hour << ':'
        << std::setw(2) << tm.tm_min << ':'
        << std::setw(2) << tm.tm_sec;
    return oss.str();
}

std::string vformat(const char* fmt, va_list ap) {
    if (!fmt) {
        return {};
    }
    va_list copy;
    va_copy(copy, ap);
    int needed = std::vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);
    if (needed <= 0) {
        return {};
    }
    std::string result;
    result.resize(static_cast<size_t>(needed));
    std::vsnprintf(result.data(), result.size() + 1, fmt, ap);
    return result;
}

void logToLogger(const std::string& line) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    if (!g_consoleLogger) {
        return;
    }
    // Console output is informational by default.
    if (g_consoleLogLevel & LOGLV_INFO) {
        g_consoleLogger->LogText(line.c_str());
    }
}

} // namespace

class ConsoleModule : public ISSConsole {
public:
    ConsoleModule()
        : m_lineCtrl(true)
        , m_initialized(false)
        , m_shutdown(false)
        , m_handler(nullptr) {}

    ~ConsoleModule() override {
        CloseMsgQueue();
    }

    bool SSAPI Init(INT32 nMaxFixedAreaLines, bool bLineCtrlFlag) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_fixedLines.assign(nMaxFixedAreaLines > 0 ? static_cast<size_t>(nMaxFixedAreaLines) : 0, std::string());
        m_lineCtrl = bLineCtrlFlag;
        m_initialized = true;
        m_shutdown.store(false);
        return true;
    }

    bool SSAPI Release(void) override {
        CloseMsgQueue();
        delete this;
        return true;
    }

    void SSAPI CloseMsgQueue(void) override {
        bool expected = false;
        if (!m_shutdown.compare_exchange_strong(expected, true)) {
            return;
        }
        ISSConsoleEventHandler* handler = nullptr;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            handler = m_handler;
            m_handler = nullptr;
        }
        if (handler) {
            handler->OnCloseEvent();
        }
    }

    INT32 SSAPI WriteString(const char* pszFormat, ...) override {
        va_list ap;
        va_start(ap, pszFormat);
        INT32 ret = doWrite(-1, pszFormat, ap);
        va_end(ap);
        return ret;
    }

    INT32 SSAPI WriteStringC(INT32 Color, const char* pszFormat, ...) override {
        va_list ap;
        va_start(ap, pszFormat);
        INT32 ret = doWrite(Color, pszFormat, ap);
        va_end(ap);
        return ret;
    }

    INT32 SSAPI WriteStringFixed(INT32 nIndex, const char* pszFormat, ...) override {
        va_list ap;
        va_start(ap, pszFormat);
        INT32 ret = doWriteFixed(-1, nIndex, pszFormat, ap);
        va_end(ap);
        return ret;
    }

    INT32 SSAPI WriteStringFixedC(INT32 Color, INT32 nIndex, const char* pszFormat, ...) override {
        va_list ap;
        va_start(ap, pszFormat);
        INT32 ret = doWriteFixed(Color, nIndex, pszFormat, ap);
        va_end(ap);
        return ret;
    }

    void SSAPI SetCtrlCHookHandler(ISSConsoleEventHandler* poHandler) override {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_handler = poHandler;
    }

    void SSAPI Run(void) override {
        // Keep the run-loop alive until CloseMsgQueue is called.
        while (!m_shutdown.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

private:
    INT32 doWrite(INT32 color, const char* fmt, va_list ap) {
        std::string message = vformat(fmt, ap);
        if (!m_initialized || message.empty()) {
            return message.empty() ? 0 : -1;
        }
        if (!message.empty() && message.back() != '\n') {
            message.push_back('\n');
        }

        std::ostringstream oss;
        oss << '[' << timestamp() << ']';
        if (color >= 0) {
            oss << "[0x" << std::hex << std::setw(4) << std::setfill('0') << (color & 0xFFFF) << std::dec << ']';
        }
        oss << ' ' << message;

        {
            std::lock_guard<std::mutex> lk(g_stdoutMutex);
            std::cout << oss.str();
            std::cout.flush();
        }
        logToLogger(oss.str());
        return static_cast<INT32>(message.size());
    }

    INT32 doWriteFixed(INT32 color, INT32 index, const char* fmt, va_list ap) {
        if (index < 0) {
            return -1;
        }
        std::string message = vformat(fmt, ap);
        if (!m_initialized) {
            return -1;
        }

        if (m_lineCtrl) {
            auto pos = message.find('\n');
            if (pos != std::string::npos) {
                message.erase(pos);
            }
        }

        {
            std::lock_guard<std::mutex> lk(m_mutex);
            if (static_cast<size_t>(index) >= m_fixedLines.size()) {
                m_fixedLines.resize(static_cast<size_t>(index) + 1);
            }
            m_fixedLines[static_cast<size_t>(index)] = message;
        }

        std::ostringstream oss;
        oss << '[' << timestamp() << "] [fixed:" << index << ']';
        if (color >= 0) {
            oss << "[0x" << std::hex << std::setw(4) << std::setfill('0') << (color & 0xFFFF) << std::dec << ']';
        }
        oss << ' ' << message << '\n';

        {
            std::lock_guard<std::mutex> lk(g_stdoutMutex);
            std::cout << oss.str();
            std::cout.flush();
        }
        logToLogger(oss.str());

        return static_cast<INT32>(message.size());
    }

private:
    std::mutex m_mutex;
    std::vector<std::string> m_fixedLines;
    bool m_lineCtrl;
    bool m_initialized;
    std::atomic<bool> m_shutdown;
    ISSConsoleEventHandler* m_handler;
};

ISSConsole* SSAPI SSConsoleGetModule(const SSSVersion* /*pstVersion*/) {
    return new ConsoleModule();
}

bool SSAPI SSConsoleSetLogger(ISSLogger* poLogger, UINT32 dwLevel) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    g_consoleLogger = poLogger;
    g_consoleLogLevel = dwLevel;
    return true;
}

} // namespace SSCP

