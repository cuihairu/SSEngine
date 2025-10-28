#include "ssengine/sdshmem.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <mutex>
#include <unordered_map>

#ifdef WINDOWS
#include <Windows.h>
#include <io.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace SSCP {
namespace {

void ResetShmem(SShmem& shm) {
    shm.handle = SDINVALID_HANDLE;
    shm.mem = nullptr;
    shm.file.clear();
}

std::mutex& ShmemMutex() {
    static std::mutex mtx;
    return mtx;
}

std::unordered_map<void*, size_t>& ShmemRegistry() {
    static std::unordered_map<void*, size_t> registry;
    return registry;
}

void TrackShmem(void* base, size_t length) {
    if (!base || length == 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(ShmemMutex());
    ShmemRegistry()[base] = length;
}

size_t UntrackShmem(void* base) {
    std::lock_guard<std::mutex> lock(ShmemMutex());
    auto& registry = ShmemRegistry();
    auto it = registry.find(base);
    if (it == registry.end()) {
        return 0;
    }
    size_t length = it->second;
    registry.erase(it);
    return length;
}

#ifdef WINDOWS
bool EnsureFileSize(HANDLE file, UINT32 size) {
    if (size == 0) {
        return false;
    }
    LARGE_INTEGER liSize{};
    liSize.QuadPart = size;
    if (!SetFilePointerEx(file, liSize, nullptr, FILE_BEGIN)) {
        return false;
    }
    if (!SetEndOfFile(file)) {
        return false;
    }
    return true;
}
#else
bool EnsureFileSize(int fd, UINT32 size) {
    if (size == 0) {
        return false;
    }
    if (ftruncate(fd, static_cast<off_t>(size)) != 0) {
        return false;
    }
    return true;
}
#endif

} // namespace

SShmem SSAPI SDOpenShmem(const CHAR* pszFileName, BOOL bCreate, UINT32 size) {
    SShmem shm{};
    ResetShmem(shm);
    if (!pszFileName) {
        return shm;
    }
    shm.file = pszFileName;

#ifdef WINDOWS
    DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD creationDisposition = bCreate ? OPEN_ALWAYS : OPEN_EXISTING;
    HANDLE file = CreateFileA(
        pszFileName,
        desiredAccess,
        shareMode,
        nullptr,
        creationDisposition,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return shm;
    }

    LARGE_INTEGER currentSize{};
    if (!GetFileSizeEx(file, &currentSize)) {
        CloseHandle(file);
        return shm;
    }

    if (currentSize.QuadPart == 0 && bCreate && size > 0) {
        if (!EnsureFileSize(file, size)) {
            CloseHandle(file);
            return shm;
        }
        currentSize.QuadPart = size;
    }

    if (currentSize.QuadPart == 0) {
        CloseHandle(file);
        return shm;
    }

    DWORD maxSizeHigh = static_cast<DWORD>((static_cast<UINT64>(currentSize.QuadPart) >> 32) & 0xFFFFFFFF);
    DWORD maxSizeLow = static_cast<DWORD>(static_cast<UINT64>(currentSize.QuadPart) & 0xFFFFFFFF);

    HANDLE mapping = CreateFileMappingA(
        file,
        nullptr,
        PAGE_READWRITE,
        maxSizeHigh,
        maxSizeLow,
        nullptr);
    CloseHandle(file);

    if (!mapping) {
        return shm;
    }

    void* view = MapViewOfFile(
        mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0);
    if (!view) {
        CloseHandle(mapping);
        return shm;
    }

    shm.handle = mapping;
    shm.mem = view;
    size_t length = static_cast<size_t>(std::min<INT64>(currentSize.QuadPart, static_cast<INT64>(std::numeric_limits<size_t>::max())));
    TrackShmem(view, length);
    return shm;
#else
    int flags = O_RDWR;
    if (bCreate) {
        flags |= O_CREAT;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int fd = open(pszFileName, flags, mode);
    if (fd < 0) {
        return shm;
    }

    struct stat st{};
    if (fstat(fd, &st) != 0) {
        close(fd);
        return shm;
    }

    if (st.st_size == 0) {
        if (!bCreate || size == 0) {
            close(fd);
            return shm;
        }
        if (!EnsureFileSize(fd, size)) {
            close(fd);
            return shm;
        }
        st.st_size = size;
    }

    size_t length = static_cast<size_t>(st.st_size);
    void* view = mmap(
        nullptr,
        length,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0);
    if (view == MAP_FAILED) {
        close(fd);
        return shm;
    }

    shm.handle = fd;
    shm.mem = view;
    TrackShmem(view, length);
    return shm;
#endif
}

SShmem SSAPI SDCreateShmem(const CHAR* pszFileName, UINT32 size) {
    return SDOpenShmem(pszFileName, TRUE, size);
}

BOOL SSAPI SDShmemExist(const CHAR* pszFileName) {
    if (!pszFileName) {
        return false;
    }
#ifdef WINDOWS
    return _access(pszFileName, 0) == 0;
#else
    return access(pszFileName, F_OK) == 0;
#endif
}

void SSAPI SDCloseShmem(SShmem* shm) {
    if (!shm || !shm->mem) {
        return;
    }

    size_t length = UntrackShmem(shm->mem);

#ifdef WINDOWS
    UnmapViewOfFile(shm->mem);
    if (shm->handle && shm->handle != SDINVALID_HANDLE) {
        CloseHandle(shm->handle);
    }
#else
    if (length == 0 && shm->handle != SDINVALID_HANDLE) {
        struct stat st{};
        if (fstat(shm->handle, &st) == 0) {
            length = static_cast<size_t>(st.st_size);
        }
    }
    if (length > 0) {
        munmap(shm->mem, length);
    }
    if (shm->handle != SDINVALID_HANDLE) {
        close(shm->handle);
    }
#endif
    ResetShmem(*shm);
}

CSDShmem::CSDShmem() {
    ResetShmem(m_shm);
}

void* SSAPI CSDShmem::Open(const CHAR* pszFileName) {
    SDCloseShmem(&m_shm);
    m_shm = SDOpenShmem(pszFileName, FALSE, 0);
    return m_shm.mem;
}

void* SSAPI CSDShmem::Create(const CHAR* pszFileName, UINT32 size) {
    SDCloseShmem(&m_shm);
    m_shm = SDCreateShmem(pszFileName, size);
    return m_shm.mem;
}

void SSAPI CSDShmem::Close() {
    SDCloseShmem(&m_shm);
}

SDHANDLE CSDShmem::GetHandle() {
    return m_shm.handle;
}

} // namespace SSCP

