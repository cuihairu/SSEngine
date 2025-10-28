// Windows-first implementation with cross-platform stubs.
#include "ssengine/sdlogger.h"
#include <string>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <atomic>
#include <memory>
#include <cstring>

#if defined(_WIN32)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

namespace fs = std::filesystem;

namespace SSCP {

// ---------------- File Logger ----------------
class FileLoggerImpl : public ISSFileLogger {
public:
    FileLoggerImpl() : _inited(false), _mode(0) {}
    ~FileLoggerImpl() override { closeUnlocked(); }

    bool SSAPI Init(const char* pszDir, const char* pszFileName, INT32 nMode) override {
        if (!pszDir || !pszFileName) return false;
        std::lock_guard<std::mutex> lk(_mtx);
        try {
            _dir = pszDir;
            _base = pszFileName;
            _mode = nMode;
            if (!_dir.empty()) fs::create_directories(_dir);
            _currentPath = buildPathForNow();
            _ofs.open(_currentPath, std::ios::out | std::ios::app | std::ios::binary);
            _inited = _ofs.is_open();
        } catch (...) {
            _inited = false;
        }
        return _inited;
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
        std::lock_guard<std::mutex> lk(_mtx);
        rollIfNeededUnlocked();
        _ofs << pszLog << "\n";
        _ofs.flush();
        return static_cast<bool>(_ofs);
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
        std::lock_guard<std::mutex> lk(_mtx);
        rollIfNeededUnlocked();
        _ofs.write(reinterpret_cast<const char*>(pLog), static_cast<std::streamsize>(dwLen));
        _ofs.flush();
        return static_cast<bool>(_ofs);
    }

private:
    std::string suffixForNow() const {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t tt = system_clock::to_time_t(now);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        char buf[32] = {0};
        if (_mode == LOG_MODE_DAY_DIVIDE) {
            std::snprintf(buf, sizeof(buf), "_%04d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        } else if (_mode == LOG_MODE_MONTH_DIVIDE) {
            std::snprintf(buf, sizeof(buf), "_%04d%02d", tm.tm_year + 1900, tm.tm_mon + 1);
        } else {
            buf[0] = '\0';
        }
        return std::string(buf);
    }

    std::string buildPathForNow() const {
        std::string filename = _base;
        filename += suffixForNow();
        filename += ".log";
        if (_dir.empty()) return filename;
        return (fs::path(_dir) / filename).string();
    }

    void closeUnlocked() {
        if (_ofs.is_open()) {
            _ofs.close();
        }
    }

    void rollIfNeededUnlocked() {
        if (_mode == LOG_MODE_SINGLE_FILE) return;
        std::string pathNow = buildPathForNow();
        if (pathNow != _currentPath) {
            closeUnlocked();
            _currentPath = std::move(pathNow);
            _ofs.open(_currentPath, std::ios::out | std::ios::app | std::ios::binary);
        }
    }

private:
    bool _inited;
    INT32 _mode;
    std::string _dir;
    std::string _base;
    std::string _currentPath;
    std::ofstream _ofs;
    std::mutex _mtx;
};

// --------------- UDP/TCP Logger ----------------

#if defined(_WIN32)
// Windows sockets helpers
struct WsaRef {
    static std::atomic<int>& refcnt() { static std::atomic<int> c{0}; return c; }
    WsaRef() { ensure(); }
    ~WsaRef() { release(); }
    static void ensure() {
        if (refcnt().fetch_add(1, std::memory_order_acq_rel) == 0) {
            WSADATA wsa{}; WSAStartup(MAKEWORD(2,2), &wsa);
        }
    }
    static void release() {
        if (refcnt().fetch_sub(1, std::memory_order_acq_rel) == 1) {
            WSACleanup();
        }
    }
};

static bool inet_pton_wrap(const char* ip, in_addr* out) {
    if (!ip || !out) return false;
    return ::InetPtonA(AF_INET, ip, out) == 1;
}
#endif

class UdpLoggerImpl : public ISSUdpLogger {
public:
    UdpLoggerImpl()
      : _inited(false)
#if defined(_WIN32)
      , _sock(INVALID_SOCKET)
#endif
    {}

    ~UdpLoggerImpl() override { close(); }

    bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 /*byServerType*/, const char* pszErrorfile = NULL) override {
        if (!pszIp || wPort == 0) return false;
        _errorfile = pszErrorfile ? pszErrorfile : std::string();
#if defined(_WIN32)
        _wsa = std::make_unique<WsaRef>();
        _sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (_sock == INVALID_SOCKET) return false;
        std::memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(wPort);
        in_addr ip{};
        if (!inet_pton_wrap(pszIp, &ip)) return false;
        _addr.sin_addr = ip;
        _inited = true;
        return true;
#else
        (void)pszIp; (void)wPort;
        _inited = true; // non-Windows stub
        return true;
#endif
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
#if defined(_WIN32)
        int sent = ::sendto(_sock, pszLog, static_cast<int>(std::strlen(pszLog)), 0, reinterpret_cast<sockaddr*>(&_addr), sizeof(_addr));
        if (sent == SOCKET_ERROR) return fallbackFile(pszLog, std::strlen(pszLog));
        return true;
#else
        (void)pszLog; return true;
#endif
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
#if defined(_WIN32)
        int sent = ::sendto(_sock, reinterpret_cast<const char*>(pLog), static_cast<int>(dwLen), 0, reinterpret_cast<sockaddr*>(&_addr), sizeof(_addr));
        if (sent == SOCKET_ERROR) return fallbackFile(reinterpret_cast<const char*>(pLog), dwLen);
        return true;
#else
        (void)pLog; (void)dwLen; return true;
#endif
    }

private:
    void close() {
#if defined(_WIN32)
        if (_sock != INVALID_SOCKET) {
            ::closesocket(_sock); _sock = INVALID_SOCKET;
        }
        _wsa.reset();
#endif
    }

    bool fallbackFile(const char* data, size_t len) {
        if (_errorfile.empty()) return false;
        std::ofstream ofs(_errorfile, std::ios::out | std::ios::app | std::ios::binary);
        if (!ofs) return false;
        ofs.write(data, static_cast<std::streamsize>(len));
        ofs << '\n';
        return true;
    }

private:
    bool _inited;
    std::string _errorfile;
#if defined(_WIN32)
    std::unique_ptr<WsaRef> _wsa;
    SOCKET _sock;
    sockaddr_in _addr{};
#endif
};

class TcpLoggerImpl : public ISSTcpLogger {
public:
    TcpLoggerImpl()
      : _inited(false)
#if defined(_WIN32)
      , _sock(INVALID_SOCKET)
#endif
    {}
    ~TcpLoggerImpl() override { close(); }

    bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 /*byServerType*/, const char* pszErrorfile = NULL) override {
        if (!pszIp || wPort == 0) return false;
        _errorfile = pszErrorfile ? pszErrorfile : std::string();
#if defined(_WIN32)
        _wsa = std::make_unique<WsaRef>();
        _sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_sock == INVALID_SOCKET) return false;
        sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(wPort);
        in_addr ip{}; if (!inet_pton_wrap(pszIp, &ip)) return false; addr.sin_addr = ip;
        if (::connect(_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            fallbackFile("[tcplog] connect failed", sizeof("[tcplog] connect failed")-1);
            return false;
        }
        _inited = true; return true;
#else
        (void)pszIp; (void)wPort; _inited = true; return true;
#endif
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
#if defined(_WIN32)
        int sent = ::send(_sock, pszLog, static_cast<int>(std::strlen(pszLog)), 0);
        if (sent == SOCKET_ERROR) return fallbackFile(pszLog, std::strlen(pszLog));
        return true;
#else
        (void)pszLog; return true;
#endif
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
#if defined(_WIN32)
        int sent = ::send(_sock, reinterpret_cast<const char*>(pLog), static_cast<int>(dwLen), 0);
        if (sent == SOCKET_ERROR) return fallbackFile(reinterpret_cast<const char*>(pLog), dwLen);
        return true;
#else
        (void)pLog; (void)dwLen; return true;
#endif
    }

private:
    void close() {
#if defined(_WIN32)
        if (_sock != INVALID_SOCKET) { ::closesocket(_sock); _sock = INVALID_SOCKET; }
        _wsa.reset();
#endif
    }

    bool fallbackFile(const char* data, size_t len) {
        if (_errorfile.empty()) return false;
        std::ofstream ofs(_errorfile, std::ios::out | std::ios::app | std::ios::binary);
        if (!ofs) return false;
        ofs.write(data, static_cast<std::streamsize>(len));
        ofs << '\n';
        return true;
    }

private:
    bool _inited;
    std::string _errorfile;
#if defined(_WIN32)
    std::unique_ptr<WsaRef> _wsa;
    SOCKET _sock;
#endif
};

// Factories
ISSFileLogger* SSAPI SDCreateFileLogger(const SSSVersion* /*pstVersion*/) {
    return new FileLoggerImpl();
}

ISSUdpLogger* SSAPI SDCreateUdpLogger(const SSSVersion* /*pstVersion*/) {
    return new UdpLoggerImpl();
}

ISSTcpLogger* SSAPI SDCreateTcpLogger(const SSSVersion* /*pstVersion*/) {
    return new TcpLoggerImpl();
}

} // namespace SSCP
