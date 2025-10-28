#include "ssengine/sdfilemapping.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace SSCP {
namespace {

void ResetMapping(SFileMapping& mapping) {
    mapping.mem = nullptr;
    mapping.pos = nullptr;
    mapping.size = 0;
    mapping.maphandle = SDINVALID_HANDLE;
}

#ifndef WINDOWS
size_t SystemPageSize() {
    static size_t page = 0;
    if (page == 0) {
        long val = sysconf(_SC_PAGESIZE);
        page = val > 0 ? static_cast<size_t>(val) : 4096;
    }
    return page;
}
#endif

} // namespace

BOOL SSAPI SDFileMapping(SFileMapping& mapping, const CHAR* pszFileName, UINT32 dwBeg, UINT32 dwSize) {
    ResetMapping(mapping);
    if (!pszFileName) {
        return false;
    }

#ifdef WINDOWS
    HANDLE file = CreateFileA(
        pszFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER fileSize{};
    if (!GetFileSizeEx(file, &fileSize)) {
        CloseHandle(file);
        return false;
    }
    if (dwBeg > static_cast<UINT64>(fileSize.QuadPart)) {
        CloseHandle(file);
        return false;
    }
    UINT64 available = static_cast<UINT64>(fileSize.QuadPart) - dwBeg;
    if (available == 0) {
        CloseHandle(file);
        return false;
    }
    UINT32 viewSize = dwSize == 0 ? static_cast<UINT32>(std::min<UINT64>(available, UINT32_MAX)) : std::min(dwSize, static_cast<UINT32>(available));
    if (viewSize == 0) {
        CloseHandle(file);
        return false;
    }

    SYSTEM_INFO sysInfo{};
    GetSystemInfo(&sysInfo);
    DWORD granularity = sysInfo.dwAllocationGranularity;
    UINT32 alignedOffset = (dwBeg / granularity) * granularity;
    UINT32 delta = dwBeg - alignedOffset;
    SIZE_T mapViewSize = static_cast<SIZE_T>(viewSize) + delta;

    HANDLE mappingHandle = CreateFileMappingA(
        file,
        nullptr,
        PAGE_READWRITE,
        0,
        0,
        nullptr);
    CloseHandle(file);

    if (!mappingHandle) {
        return false;
    }

    void* base = MapViewOfFile(
        mappingHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        static_cast<DWORD>(alignedOffset),
        mapViewSize);
    if (!base) {
        CloseHandle(mappingHandle);
        return false;
    }

    mapping.mem = base;
    mapping.pos = static_cast<char*>(base) + delta;
    mapping.size = viewSize;
    mapping.maphandle = mappingHandle;
    return true;
#else
    int flags = O_RDWR;
    int fd = open(pszFileName, flags);
    if (fd < 0) {
        return false;
    }

    struct stat st{};
    if (fstat(fd, &st) != 0) {
        close(fd);
        return false;
    }

    if (dwBeg > static_cast<UINT64>(st.st_size)) {
        close(fd);
        return false;
    }

    UINT64 available = static_cast<UINT64>(st.st_size) - dwBeg;
    if (available == 0) {
        close(fd);
        return false;
    }

    UINT32 viewSize = dwSize == 0 ? static_cast<UINT32>(std::min<UINT64>(available, UINT32_MAX)) : std::min(dwSize, static_cast<UINT32>(available));
    if (viewSize == 0) {
        close(fd);
        return false;
    }

    size_t pageSize = SystemPageSize();
    UINT64 alignedOffset = (static_cast<UINT64>(dwBeg) / pageSize) * pageSize;
    size_t delta = static_cast<size_t>(dwBeg - alignedOffset);
    size_t mapViewSize = static_cast<size_t>(viewSize) + delta;

    void* base = mmap(
        nullptr,
        mapViewSize,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        static_cast<off_t>(alignedOffset));
    if (base == MAP_FAILED) {
        close(fd);
        return false;
    }

    mapping.mem = base;
    mapping.pos = static_cast<char*>(base) + delta;
    mapping.size = viewSize;
    mapping.maphandle = fd;
    return true;
#endif
}

void SSAPI SDFileUnMapping(SFileMapping& mapping) {
    if (!mapping.mem) {
        return;
    }

    size_t delta = static_cast<char*>(mapping.pos) - static_cast<char*>(mapping.mem);
    size_t totalSize = static_cast<size_t>(mapping.size) + delta;

#ifdef WINDOWS
    UnmapViewOfFile(mapping.mem);
    if (mapping.maphandle && mapping.maphandle != SDINVALID_HANDLE) {
        CloseHandle(mapping.maphandle);
    }
#else
    munmap(mapping.mem, totalSize);
    if (mapping.maphandle != SDINVALID_HANDLE) {
        close(mapping.maphandle);
    }
#endif
    ResetMapping(mapping);
}

void SSAPI SDFileFlushMapping(void* pMem, size_t len) {
    if (!pMem || len == 0) {
        return;
    }
#ifdef WINDOWS
    FlushViewOfFile(pMem, len);
#else
    msync(pMem, len, MS_SYNC);
#endif
}

} // namespace SSCP
