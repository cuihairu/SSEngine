#include "ssengine/sdlogger.h"
#include <cstdarg>
#include <cstdio>
#include <string>

namespace SSCP {

CSDLogger::CSDLogger()
  : m_pLogger(nullptr)
{
    m_aszLogPrefix[0] = ""; // unused
    m_aszLogPrefix[1] = "[D] ";
    m_aszLogPrefix[2] = "[I] ";
    m_aszLogPrefix[3] = "[W] ";
    m_aszLogPrefix[4] = "[C] ";
}

CSDLogger::~CSDLogger() {}

void CSDLogger::SetLogger(ISSLogger* pLogger) {
    m_pLogger = pLogger;
}

void CSDLogger::Critical(const char *format, ...) {
    va_list ap; va_start(ap, format); _Log(LOGLV_CRITICAL, format, ap); va_end(ap);
}
void CSDLogger::Info(const char *format, ...) {
    va_list ap; va_start(ap, format); _Log(LOGLV_INFO, format, ap); va_end(ap);
}
void CSDLogger::Warn(const char *format, ...) {
    va_list ap; va_start(ap, format); _Log(LOGLV_WARN, format, ap); va_end(ap);
}
void CSDLogger::Debug(const char *format, ...) {
    va_list ap; va_start(ap, format); _Log(LOGLV_DEBUG, format, ap); va_end(ap);
}

void CSDLogger::_Log(UINT32 dwLevel, const char *pszFormat, va_list argptr)
{
    if (!m_pLogger || !pszFormat) return;

    char buf[2048];
#ifdef _MSC_VER
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, pszFormat, argptr);
#else
    vsnprintf(buf, sizeof(buf), pszFormat, argptr);
#endif

    const char* prefix = "";
    if (dwLevel & LOGLV_CRITICAL) prefix = m_aszLogPrefix[4];
    else if (dwLevel & LOGLV_WARN) prefix = m_aszLogPrefix[3];
    else if (dwLevel & LOGLV_INFO) prefix = m_aszLogPrefix[2];
    else if (dwLevel & LOGLV_DEBUG) prefix = m_aszLogPrefix[1];

    std::string line(prefix);
    line += buf;
    m_pLogger->LogText(line.c_str());
}

} // namespace SSCP
