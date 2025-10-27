#include "sdlogger.h"
#include <string>
#include <cstdio>

namespace SSCP {

class FileLoggerImpl : public ISSFileLogger {
public:
    FileLoggerImpl() : _inited(false) {}
    virtual ~FileLoggerImpl() {}

    bool SSAPI Init(const char* pszDir, const char* pszFileName, INT32 nMode) override {
        (void)pszDir; (void)pszFileName; (void)nMode;
        _inited = true; // stub; real impl will create/roll files
        return true;
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
        // stub: write to stdout for now
        std::fputs(pszLog, stdout);
        std::fputc('\n', stdout);
        return true;
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
        // stub: count only
        (void)dwLen;
        return true;
    }
private:
    bool _inited;
};

class UdpLoggerImpl : public ISSUdpLogger {
public:
    UdpLoggerImpl() : _inited(false) {}
    virtual ~UdpLoggerImpl() {}

    bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 byServerType, const char* pszErrorfile = NULL) override {
        (void)pszIp; (void)wPort; (void)byServerType; (void)pszErrorfile;
        _inited = true; // stub: no socket yet
        return true;
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
        return true;
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
        (void)dwLen;
        return true;
    }
private:
    bool _inited;
};

class TcpLoggerImpl : public ISSTcpLogger {
public:
    TcpLoggerImpl() : _inited(false) {}
    virtual ~TcpLoggerImpl() {}

    bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 byServerType, const char* pszErrorfile = NULL) override {
        (void)pszIp; (void)wPort; (void)byServerType; (void)pszErrorfile;
        _inited = true; // stub: no socket yet
        return true;
    }

    void SSAPI Release() override { delete this; }

    bool SSAPI LogText(const char* pszLog) override {
        if (!_inited || !pszLog) return false;
        return true;
    }

    bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) override {
        if (!_inited || !pLog) return false;
        (void)dwLen;
        return true;
    }
private:
    bool _inited;
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
