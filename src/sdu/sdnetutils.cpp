#include "ssengine/sdnetutils.h"
#include <string>
#include <vector>
#include <cstring>

#ifdef WINDOWS
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <unistd.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <ifaddrs.h>
#endif

namespace SSCP {

const CHAR* SSAPI SDNetInetNtop(INT32 af, const void *pSrc, CHAR *pDst, size_t size)
{
    if (!pSrc || !pDst || size == 0) return nullptr;
#ifdef WINDOWS
    // Windows ships inet_ntop from Vista+, provided in Ws2_32
    return ::InetNtopA(af, const_cast<void*>(pSrc), pDst, static_cast<DWORD>(size));
#else
    return ::inet_ntop(af, pSrc, pDst, size);
#endif
}

BOOL SSAPI SDNetInetPton(INT32 af, const CHAR *pSrc, void *pDst)
{
    if (!pSrc || !pDst) return FALSE;
#ifdef WINDOWS
    return (::InetPtonA(af, pSrc, pDst) == 1) ? TRUE : FALSE;
#else
    return (::inet_pton(af, pSrc, pDst) == 1) ? TRUE : FALSE;
#endif
}

LONG SSAPI SDGetIpFromName(const CHAR * pszName)
{
    if (!pszName) return 0;
#ifdef WINDOWS
    addrinfoA hints{}; hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    addrinfoA* res = nullptr;
    if (getaddrinfo(pszName, nullptr, &hints, &res) != 0) return 0;
    ULONG addr = 0;
    for (auto p = res; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = sin->sin_addr.S_un.S_addr; break;
        }
    }
    freeaddrinfo(res);
    return static_cast<LONG>(addr);
#else
    addrinfo hints{}; hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    if (getaddrinfo(pszName, nullptr, &hints, &res) != 0) return 0;
    ULONG addr = 0;
    for (auto p = res; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = sin->sin_addr.s_addr; break;
        }
    }
    freeaddrinfo(res);
    return static_cast<LONG>(addr);
#endif
}

std::string SSAPI SDGetHostName()
{
    char buf[256] = {0};
#ifdef WINDOWS
    if (gethostname(buf, sizeof(buf)) != 0) return std::string();
#else
    if (::gethostname(buf, sizeof(buf)) != 0) return std::string();
#endif
    buf[sizeof(buf)-1] = '\0';
    return std::string(buf);
}

SDHostent* SSAPI SDGetHostByName(const CHAR * pszName)
{
    if (!pszName) return nullptr;
#ifdef WINDOWS
    // Note: gethostbyname is deprecated; kept for compatibility with header contract
    return ::gethostbyname(pszName);
#else
    return ::gethostbyname(pszName);
#endif
}

INT32 SSAPI SDGetLocalIp(ULONG dwIp[], UINT32 dwCount)
{
    if (!dwIp || dwCount == 0) return 0;
    INT32 filled = 0;
#ifdef WINDOWS
    char hostname[256] = {0};
    if (gethostname(hostname, sizeof(hostname)) != 0) return 0;
    hostent* he = gethostbyname(hostname);
    if (!he) return 0;
    for (char** addr = he->h_addr_list; *addr && filled < static_cast<INT32>(dwCount); ++addr) {
        in_addr* ina = reinterpret_cast<in_addr*>(*addr);
        dwIp[filled++] = ina->S_un.S_addr;
    }
#else
    // Prefer getifaddrs to enumerate IPv4 addresses
    ifaddrs* ifa_list = nullptr;
    if (getifaddrs(&ifa_list) != 0) return 0;
    for (auto p = ifa_list; p && filled < static_cast<INT32>(dwCount); p = p->ifa_next) {
        if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
        sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(p->ifa_addr);
        dwIp[filled++] = sin->sin_addr.s_addr;
    }
    freeifaddrs(ifa_list);
    if (filled == 0) {
        // fallback: hostent
        char hostname[256] = {0};
        if (gethostname(hostname, sizeof(hostname)) != 0) return 0;
        hostent* he = gethostbyname(hostname);
        if (!he) return 0;
        for (char** addr = he->h_addr_list; *addr && filled < static_cast<INT32>(dwCount); ++addr) {
            in_addr* ina = reinterpret_cast<in_addr*>(*addr);
            dwIp[filled++] = ina->s_addr;
        }
    }
#endif
    return filled;
}

} // namespace SSCP
