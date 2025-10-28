#include "ssengine/sdtime.h"
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

namespace SSCP {

using Clock = std::chrono::system_clock;

static std::tm tm_from_time_t(time_t t) {
    std::tm out{};
#ifdef _WIN32
    localtime_s(&out, &t);
#else
    localtime_r(&t, &out);
#endif
    return out;
}

static time_t time_t_from_tm(const std::tm& tmv) {
    std::tm copy = tmv;
    return std::mktime(&copy);
}

// ----- CSDDateTime -----

CSDDateTime SDNow() {
    return CSDDateTime(Clock::to_time_t(Clock::now()));
}

CSDDateTime::CSDDateTime(time_t t) {
    if (t == 0) {
        m_time = {};
        m_time.tm_year = 70; // 1970
        m_time.tm_mon = 0;
        m_time.tm_mday = 1;
    } else {
        m_time = tm_from_time_t(t);
    }
}

time_t CSDDateTime::GetTimeValue() {
    return time_t_from_tm(m_time);
}

void CSDDateTime::SetTimeValue(time_t t) {
    m_time = tm_from_time_t(t);
}

INT64 CSDDateTime::operator - (const CSDDateTime &datetime) {
    return DiffSecond(datetime);
}

INT64 CSDDateTime::DiffSecond(const CSDDateTime &datetime) {
    time_t t1 = GetTimeValue();
    time_t t0 = const_cast<CSDDateTime&>(datetime).GetTimeValue();
    return static_cast<INT64>(std::difftime(t1, t0));
}

INT64 CSDDateTime::DiffMinute(const CSDDateTime &datetime) {
    return DiffSecond(datetime) / 60;
}

INT64 CSDDateTime::DiffHour(const CSDDateTime &datetime) {
    return DiffSecond(datetime) / 3600;
}

INT64 CSDDateTime::DiffDay(const CSDDateTime &datetime) {
    return DiffSecond(datetime) / (24 * 3600);
}

INT64 CSDDateTime::DiffWeek(const CSDDateTime &datetime) {
    return DiffSecond(datetime) / (7 * 24 * 3600);
}

INT32 CSDDateTime::DiffMonth(const CSDDateTime &datetime) {
    // Rough: difference in months based on year and month fields
    int y1 = m_time.tm_year + 1900;
    int m1 = m_time.tm_mon + 1;
    std::tm t2 = datetime.m_time;
    int y2 = t2.tm_year + 1900;
    int m2 = t2.tm_mon + 1;
    return (y1 - y2) * 12 + (m1 - m2);
}

INT32 CSDDateTime::DiffYear(const CSDDateTime &datetime) {
    return (m_time.tm_year + 1900) - (datetime.m_time.tm_year + 1900);
}

static void normalize_tm(std::tm& tmv) {
    time_t t = std::mktime(&tmv);
    if (t != static_cast<time_t>(-1)) {
        tmv = tm_from_time_t(t);
    }
}

CSDDateTime & CSDDateTime::IncYear(UINT32 year) {
    m_time.tm_year += static_cast<int>(year);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecYear(UINT32 year) {
    m_time.tm_year -= static_cast<int>(year);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncMonth(UINT32 month) {
    m_time.tm_mon += static_cast<int>(month);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecMonth(UINT32 month) {
    m_time.tm_mon -= static_cast<int>(month);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncDay(UINT32 day) {
    m_time.tm_mday += static_cast<int>(day);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecDay(UINT32 day) {
    m_time.tm_mday -= static_cast<int>(day);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncHour(UINT32 hour) {
    m_time.tm_hour += static_cast<int>(hour);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecHour(UINT32 hour) {
    m_time.tm_hour -= static_cast<int>(hour);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncMinute(UINT32 minute) {
    m_time.tm_min += static_cast<int>(minute);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecMinute(UINT32 minute) {
    m_time.tm_min -= static_cast<int>(minute);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncSecond(UINT32 second) {
    m_time.tm_sec += static_cast<int>(second);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::DecSecond(UINT32 second) {
    m_time.tm_sec -= static_cast<int>(second);
    normalize_tm(m_time);
    return *this;
}

CSDDateTime & CSDDateTime::IncWeek(UINT32 week) {
    return IncDay(week * 7);
}

CSDDateTime & CSDDateTime::DecWeek(UINT32 week) {
    return DecDay(week * 7);
}

BOOL CSDDateTime::SetDateTime(UINT32 year, UINT32 month, UINT32 day, UINT32 hours, UINT32 minutes, UINT32 seconds) {
    m_time = {};
    m_time.tm_year = static_cast<int>(year) - 1900;
    m_time.tm_mon = static_cast<int>(month) - 1;
    m_time.tm_mday = static_cast<int>(day);
    m_time.tm_hour = static_cast<int>(hours);
    m_time.tm_min = static_cast<int>(minutes);
    m_time.tm_sec = static_cast<int>(seconds);
    time_t t = std::mktime(&m_time);
    return (t != static_cast<time_t>(-1));
}

BOOL CSDDateTime::SetDate(UINT32 year, UINT32 month, UINT32 day) {
    return SetDateTime(year, month, day, m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
}

BOOL CSDDateTime::SetTime(UINT32 hours, UINT32 minutes, UINT32 seconds) {
    return SetDateTime(m_time.tm_year + 1900, m_time.tm_mon + 1, m_time.tm_mday, hours, minutes, seconds);
}

UINT32 CSDDateTime::GetYear() { return static_cast<UINT32>(m_time.tm_year + 1900); }
UINT32 CSDDateTime::GetMonth() { return static_cast<UINT32>(m_time.tm_mon + 1); }
UINT32 CSDDateTime::GetDay() { return static_cast<UINT32>(m_time.tm_mday); }
UINT32 CSDDateTime::GetHour() { return static_cast<UINT32>(m_time.tm_hour); }
UINT32 CSDDateTime::GetMinute() { return static_cast<UINT32>(m_time.tm_min); }
UINT32 CSDDateTime::GetSecond() { return static_cast<UINT32>(m_time.tm_sec); }
UINT32 CSDDateTime::GetWeek() { return static_cast<UINT32>(m_time.tm_wday); }

std::string CSDDateTime::ToString(CHAR * format) {
    char buf[128] = {0};
    std::tm copy = m_time;
    size_t n = std::strftime(buf, sizeof(buf), format, &copy);
    return std::string(buf, n);
}

BOOL CSDDateTime::FromString(CHAR *pDateTime, CHAR *pFormat) {
    if (!pDateTime || !pFormat) return FALSE;
    std::tm out{};
    const char* fmt = reinterpret_cast<const char*>(pFormat);
    std::string text(reinterpret_cast<const char*>(pDateTime));
    std::istringstream iss(text);
    iss >> std::get_time(&out, fmt);
    if (iss.fail()) return FALSE;
    // normalize
    time_t t = std::mktime(&out);
    if (t == static_cast<time_t>(-1)) return FALSE;
    m_time = tm_from_time_t(t);
    return TRUE;
}

BOOL SDIsLeapYear(UINT32 year) {
    if (year % 400 == 0) return TRUE;
    if (year % 100 == 0) return FALSE;
    return (year % 4 == 0) ? TRUE : FALSE;
}

UINT32 SSAPI SDGetTickCount() {
#ifdef _WIN32
    return ::GetTickCount();
#else
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    return static_cast<UINT32>(ms & 0xFFFFFFFFu);
#endif
}

void SSAPI SDSleep(UINT32 milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

UINT64 SSAPI SDTimeMicroSec() {
    using namespace std::chrono;
    return static_cast<UINT64>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
}

UINT64 SSAPI SDTimeMilliSec() {
    using namespace std::chrono;
    return static_cast<UINT64>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

UINT64 SSAPI SDTimeSecs() {
    using namespace std::chrono;
    return static_cast<UINT64>(duration_cast<seconds>(steady_clock::now().time_since_epoch()).count());
}

} // namespace SSCP
