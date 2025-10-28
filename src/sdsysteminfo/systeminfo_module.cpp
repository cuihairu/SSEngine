#include "ssengine/sdsysteminfo.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <iomanip>

#ifdef _WIN32
#  include <windows.h>
#  include <intrin.h>
#else
#  include <sys/utsname.h>
#  include <unistd.h>
#endif

namespace SSCP {

namespace {

std::string cpuVendor() {
#ifdef _WIN32
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0);
    char vendor[13] = {0};
    std::memcpy(vendor + 0, &cpuInfo[1], 4);
    std::memcpy(vendor + 4, &cpuInfo[3], 4);
    std::memcpy(vendor + 8, &cpuInfo[2], 4);
    return vendor;
#else
    FILE* f = std::fopen("/proc/cpuinfo", "r");
    if (!f) return "Unknown";
    char line[256];
    std::string vendor = "Unknown";
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strstr(line, "vendor_id")) {
            char* colon = std::strchr(line, ':');
            if (colon) {
                while (*++colon == ' ' || *colon == '\t') {}
                vendor = colon;
                vendor.erase(vendor.find_last_not_of(" \t\r\n") + 1);
                break;
            }
        }
    }
    std::fclose(f);
    return vendor;
#endif
}

std::string cpuModel() {
#ifdef _WIN32
    int cpuInfo[4] = {0};
    char brand[49] = {0};
    __cpuid(cpuInfo, 0x80000000);
    unsigned int maxExtendedId = cpuInfo[0];
    if (maxExtendedId >= 0x80000004) {
        __cpuid(reinterpret_cast<int*>(brand), 0x80000002);
        __cpuid(reinterpret_cast<int*>(brand) + 4, 0x80000003);
        __cpuid(reinterpret_cast<int*>(brand) + 8, 0x80000004);
        return brand;
    }
    return "Generic CPU";
#else
    FILE* f = std::fopen("/proc/cpuinfo", "r");
    if (!f) return "Generic CPU";
    char line[256];
    std::string model = "Generic CPU";
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strstr(line, "model name")) {
            char* colon = std::strchr(line, ':');
            if (colon) {
                while (*++colon == ' ' || *colon == '\t') {}
                model = colon;
                model.erase(model.find_last_not_of(" \t\r\n") + 1);
                break;
            }
        }
    }
    std::fclose(f);
    return model;
#endif
}

UINT32 vendorIdEnum(const std::string& vendor) {
    if (vendor == "GenuineIntel" || vendor.find("Intel") != std::string::npos) {
        return SDSYSTEMINFO_CPU_INTEL;
    }
    if (vendor == "AuthenticAMD" || vendor.find("AMD") != std::string::npos) {
        return SDSYSTEMINFO_CPU_AMD;
    }
    return SDSYSTEMINFO_CPU_OTHER;
}

UINT32 systemMemoryMB(UINT32* totalOut) {
#ifdef _WIN32
    MEMORYSTATUSEX status{};
    status.dwLength = sizeof(status);
    if (::GlobalMemoryStatusEx(&status)) {
        if (totalOut) {
            *totalOut = static_cast<UINT32>(status.ullTotalPhys / (1024 * 1024));
        }
        return static_cast<UINT32>(status.ullAvailPhys / (1024 * 1024));
    }
    if (totalOut) *totalOut = 0;
    return 0;
#else
    long pages = ::sysconf(_SC_AVPHYS_PAGES);
    long pageSize = ::sysconf(_SC_PAGE_SIZE);
    long totalPages = ::sysconf(_SC_PHYS_PAGES);
    if (pages <= 0 || pageSize <= 0 || totalPages <= 0) {
        if (totalOut) *totalOut = 0;
        return 0;
    }
    if (totalOut) *totalOut = static_cast<UINT32>((static_cast<long long>(totalPages) * pageSize) / (1024 * 1024));
    return static_cast<UINT32>((static_cast<long long>(pages) * pageSize) / (1024 * 1024));
#endif
}

std::string kernelVersion() {
#ifdef _WIN32
    OSVERSIONINFOEX info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (::GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&info))) {
        std::ostringstream oss;
        oss << info.dwMajorVersion << '.' << info.dwMinorVersion << " (build " << info.dwBuildNumber << ")";
        return oss.str();
    }
    return "Windows";
#else
    struct utsname uts{};
    if (::uname(&uts) == 0) {
        return std::string(uts.sysname) + " " + uts.release;
    }
    return "Unix";
#endif
}

void writeString(char* dest, const std::string& src, size_t maxLen) {
    if (!dest || maxLen == 0) {
        return;
    }
    std::strncpy(dest, src.c_str(), maxLen - 1);
    dest[maxLen - 1] = '\0';
}

std::string machineId() {
    static std::string id;
    static std::once_flag once;
    std::call_once(once, []() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist;
        std::ostringstream oss;
        for (int i = 0; i < 2; ++i) {
            uint64_t value = dist(gen);
            oss << std::hex << std::setw(16) << std::setfill('0') << value;
        }
        id = oss.str();
    });
    return id;
}

} // namespace

class SystemInfoModule : public ISSSystemInfo {
public:
    SystemInfoModule() : m_ref(1) {}

    void SSAPI AddRef(void) override {
        m_ref.fetch_add(1);
    }

    UINT32 SSAPI QueryRef(void) override {
        return m_ref.load();
    }

    void SSAPI Release(void) override {
        if (m_ref.fetch_sub(1) == 1) {
            delete this;
        }
    }

    SSSVersion SSAPI GetVersion(void) override {
        return SDSYSTEMINFO_VERSION;
    }

    const char* SSAPI GetModuleName(void) override {
        return SDSYSTEMINFO_MODULENAME;
    }

    void SSAPI GetCpuName(char* strResult) override {
        writeString(strResult, cpuModel(), 128);
    }

    UINT32 SSAPI GetCpuVendor(char* str) override {
        std::string vendor = cpuVendor();
        writeString(str, vendor, 32);
        return vendorIdEnum(vendor);
    }

    void SSAPI GetCpuModel(char* str) override {
        writeString(str, cpuModel(), 64);
    }

    bool SSAPI IsSupportMMX() override { return false; }
    bool SSAPI IsSupportSSE() override { return false; }
    bool SSAPI IsSupportSSE2() override { return false; }
    bool SSAPI IsSupport3DNow() override { return false; }
    bool SSAPI IsSupportHT() override { return std::thread::hardware_concurrency() > 1; }

    UINT32 SSAPI GetCpuSpeedReg() override { return 0; }
    UINT32 SSAPI GetCpuSpeed() override { return 0; }

    UINT32 SSAPI GetVideoCardName(char* str) override {
        writeString(str, "Integrated Graphics", 256);
        return 0xFFFF;
    }

    UINT32 SSAPI GetVideoMemory() override {
        return 0;
    }

    UINT32 SSAPI GetVideoCardModel(char* str, INT32* pVideoCardID) override {
        if (pVideoCardID) {
            *pVideoCardID = SDSYSTEMINFO_GPU_UNKONW;
        }
        writeString(str, "Generic GPU", 128);
        return 0;
    }

    UINT32 SSAPI GetWindowsVersion() override {
#ifdef _WIN32
        OSVERSIONINFOEX info{};
        info.dwOSVersionInfoSize = sizeof(info);
        if (::GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&info))) {
            return (info.dwMajorVersion * 1000) + info.dwMinorVersion;
        }
        return 0;
#else
        return 0;
#endif
    }

    UINT32 SSAPI GetWindowsMemory(UINT32* pTotal) override {
        return systemMemoryMB(pTotal);
    }

    UINT32 SSAPI GetDXVersion() override {
        return 0;
    }

    void SSAPI GetMachineID(char* str) override {
        writeString(str, machineId(), 16);
    }

    bool SSAPI GetSystemInfo(char* strOutput) override {
        if (!strOutput) {
            return false;
        }
        UINT32 totalMem = 0;
        UINT32 freeMem = GetWindowsMemory(&totalMem);
        std::ostringstream oss;
        oss << "CPU: " << cpuModel()
            << "\nVendor: " << cpuVendor()
            << "\nCores: " << std::thread::hardware_concurrency()
            << "\nMemory Total(MB): " << totalMem
            << "\nMemory Free(MB): " << freeMem
            << "\nKernel: " << kernelVersion()
            << "\nMachineID: " << machineId();
        std::string info = oss.str();
        constexpr size_t kBufferSize = 4096;
        std::strncpy(strOutput, info.c_str(), kBufferSize - 1);
        strOutput[kBufferSize - 1] = '\0';
        return true;
    }

private:
    std::atomic<UINT32> m_ref;
};

ISSSystemInfo* SSAPI SDSystemInfoGetModule(const SSSCPVersion* /*pstVersion*/) {
    return new SystemInfoModule();
}

} // namespace SSCP
