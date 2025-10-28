// Cross-platform directory utilities implementation.
// Implements CSDDirectory and helpers declared in include/ssengine/sddir.h

#include "ssengine/sddir.h"

#include <cstring>
#include <string>
#include <filesystem>

#ifdef WINDOWS
#  include <windows.h>
#else
#  include <sys/stat.h>
#  include <unistd.h>
#endif

namespace fs = std::filesystem;
namespace SSCP {

// Open the directory for iteration.
BOOL SSAPI CSDDirectory::Open(const CHAR* pszPath)
{
    if (!pszPath || !*pszPath) return FALSE;
#ifdef WINDOWS
    // Verify the path exists and is a directory; defer FindFirstFileA until first Read.
    DWORD attr = GetFileAttributesA(pszPath);
    if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        m_fHandle = NULL;
        return FALSE;
    }
    m_szPath = pszPath;
    m_fHandle = NULL; // will be created on first Read()
    return TRUE;
#else
    // Close any previous stream if we can safely detect it. We cannot assume
    // default initialization from header in all build modes, so avoid calling
    // Close() here.
    m_dir = ::opendir(pszPath);
    if (!m_dir) return FALSE;
    std::strncpy(m_curDir, pszPath, sizeof(m_curDir) - 1);
    m_curDir[sizeof(m_curDir) - 1] = '\0';
    return TRUE;
#endif
}

// Close the directory handle/stream.
void SSAPI CSDDirectory::Close()
{
#ifdef WINDOWS
    if (m_fHandle) {
        ::FindClose(m_fHandle);
        m_fHandle = NULL;
    }
    m_szPath.clear();
#else
    if (m_dir) {
        ::closedir(m_dir);
        m_dir = nullptr;
    }
    m_curDir[0] = '\0';
#endif
}

// Read next entry name and attribute; returns FALSE on failure or end.
BOOL SSAPI CSDDirectory::Read(CHAR* pszName, INT32 nBufLen, SFileAttr* pAttr)
{
    if (!pszName || nBufLen <= 0) return FALSE;
#ifdef WINDOWS
    WIN32_FIND_DATAA ffd{};
    // Iterate until we find a name other than "." and ".."
    for (;;) {
        if (!m_fHandle) {
            std::string pattern = m_szPath;
            if (!pattern.empty() && pattern.back() != '\\' && pattern.back() != '/') pattern += '\\';
            pattern += "*";
            HANDLE h = ::FindFirstFileA(pattern.c_str(), &ffd);
            if (h == INVALID_HANDLE_VALUE) {
                m_fHandle = NULL;
                return FALSE; // invalid path or no entries
            }
            m_fHandle = h;
        } else {
            if (!::FindNextFileA(m_fHandle, &ffd)) {
                // End reached or error; close handle and return FALSE
                ::FindClose(m_fHandle);
                m_fHandle = NULL;
                return FALSE;
            }
        }
        // Skip . and ..
        if (std::strcmp(ffd.cFileName, ".") == 0 || std::strcmp(ffd.cFileName, "..") == 0) {
            // Continue loop to get next
            continue;
        }
        // Fill outputs
        std::strncpy(pszName, ffd.cFileName, static_cast<size_t>(nBufLen) - 1);
        pszName[nBufLen - 1] = '\0';
        if (pAttr) {
            pAttr->isDir = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
        }
        return TRUE;
    }
#else
    // POSIX readdir loop
    for (;;) {
        if (!m_dir) return FALSE;
        struct dirent* ent = ::readdir(m_dir);
        if (!ent) return FALSE; // end
        if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0) continue;

        std::strncpy(pszName, ent->d_name, static_cast<size_t>(nBufLen) - 1);
        pszName[nBufLen - 1] = '\0';
        if (pAttr) {
            BOOL isd = FALSE;
#  ifdef DT_DIR
            if (ent->d_type == DT_DIR) {
                isd = TRUE;
            } else if (ent->d_type == DT_UNKNOWN || ent->d_type == DT_LNK) {
                // Fallback to stat
                std::string full = std::string(m_curDir) + "/" + ent->d_name;
                struct stat st{};
                if (::stat(full.c_str(), &st) == 0) {
                    isd = S_ISDIR(st.st_mode) ? TRUE : FALSE;
                }
            }
#  else
            // No d_type provided. Fallback to stat
            std::string full = std::string(m_curDir) + "/" + ent->d_name;
            struct stat st{};
            if (::stat(full.c_str(), &st) == 0) {
                isd = S_ISDIR(st.st_mode) ? TRUE : FALSE;
            }
#  endif
            pAttr->isDir = isd;
        }
        return TRUE;
    }
#endif
}

// Create a directory
BOOL SSAPI SDCreateDirectory(const CHAR* pszDirName, BOOL bForce)
{
    if (!pszDirName) return FALSE;
    std::error_code ec;
    bool ok = bForce ? fs::create_directories(fs::path(pszDirName), ec)
                     : fs::create_directory(fs::path(pszDirName), ec);
    return ok ? TRUE : FALSE;
}

// Delete a directory; when bForce is TRUE, remove recursively
BOOL SSAPI SDDeleteDirectory(const CHAR* pszDirName, BOOL bForce)
{
    if (!pszDirName) return FALSE;
    std::error_code ec;
    if (bForce) {
        return fs::remove_all(fs::path(pszDirName), ec) > 0 ? TRUE : FALSE;
    }
    return fs::remove(fs::path(pszDirName), ec) ? TRUE : FALSE;
}

// Check if path exists and is a directory
BOOL SSAPI SDDirectoryExists(const CHAR* pszDirName)
{
    if (!pszDirName) return FALSE;
    std::error_code ec;
    fs::path p(pszDirName);
    return (fs::exists(p, ec) && fs::is_directory(p, ec)) ? TRUE : FALSE;
}

// Check if a path is a directory
BOOL SSAPI SDIsDirectory(const CHAR* pszFileName)
{
    if (!pszFileName) return FALSE;
    std::error_code ec;
    return fs::is_directory(fs::path(pszFileName), ec) ? TRUE : FALSE;
}

// Check if a directory is empty
BOOL SSAPI SDIsEmptyDirectory(const CHAR* pszFileName)
{
    if (!pszFileName) return FALSE;
    std::error_code ec;
    fs::path p(pszFileName);
    if (!fs::exists(p, ec) || !fs::is_directory(p, ec)) return FALSE;
    bool empty = fs::is_empty(p, ec);
    return (!ec && empty) ? TRUE : FALSE;
}

} // namespace SSCP
