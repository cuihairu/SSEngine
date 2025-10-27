#include "sdfile.h"
#include "sdtype.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <filesystem>
#include <system_error>

#ifdef _WIN32
#  include <windows.h>
#  include <Shlwapi.h>
#  include <io.h>
#  include <direct.h>
#  pragma comment(lib, "Shlwapi.lib")
#else
#  include <unistd.h>
#  include <sys/stat.h>
#  include <libgen.h>
#endif

namespace fs = std::filesystem;
namespace SSCP {

// ---------------- CSDFile ----------------

CSDFile::CSDFile() : m_pFileHandle(nullptr), m_bFileMapping(FALSE) {}
CSDFile::~CSDFile() { Close(); }

BOOL CSDFile::Open(const CHAR* pszFileName, const CHAR* type, BOOL bFileMapping) {
    Close();
    if (!pszFileName || !type) return FALSE;
    m_filepath = pszFileName;
    m_bFileMapping = bFileMapping;
    // Ignore file mapping for now; fallback to stdio
    std::FILE* f = std::fopen(pszFileName, type);
    if (!f) return FALSE;
    m_pFileHandle = f;
    return TRUE;
}

void CSDFile::Close() {
    if (m_pFileHandle) {
        std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
        std::fclose(f);
        m_pFileHandle = nullptr;
    }
}

void CSDFile::Flush() {
    if (!m_pFileHandle) return;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    std::fflush(f);
}

UINT32 CSDFile::Read(void* pBuf, UINT32 nLen) {
    if (!m_pFileHandle || !pBuf || nLen == 0) return 0;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    size_t n = std::fread(pBuf, 1, nLen, f);
    return static_cast<UINT32>(n);
}

UINT32 CSDFile::Write(const void* pBuf, UINT32 nLen) {
    if (!m_pFileHandle || !pBuf || nLen == 0) return 0;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    size_t n = std::fwrite(pBuf, 1, nLen, f);
    return static_cast<UINT32>(n);
}

INT32 CSDFile::Seek(INT32 offset, CSDFile::SeekOffset whence) {
    if (!m_pFileHandle) return -1;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    return std::fseek(f, offset, static_cast<int>(whence));
}

INT32 CSDFile::SeekToBegin() {
    return Seek(0, SK_SET);
}

INT32 CSDFile::SeekToEnd() {
    return Seek(0, SK_END);
}

UINT32 CSDFile::GetPosition() {
    if (!m_pFileHandle) return 0;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    long p = std::ftell(f);
    return (p < 0) ? 0 : static_cast<UINT32>(p);
}

void CSDFile::SetLength(UINT32 newLen) {
    if (!m_pFileHandle) return;
#ifdef _WIN32
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    int fd = _fileno(f);
    _chsize(fd, static_cast<long>(newLen));
#else
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    int fd = fileno(f);
    ftruncate(fd, static_cast<off_t>(newLen));
#endif
}

BOOL CSDFile::Eof() {
    if (!m_pFileHandle) return TRUE;
    std::FILE* f = static_cast<std::FILE*>(m_pFileHandle);
    return std::feof(f) ? TRUE : FALSE;
}

BOOL CSDFile::GetFileStatus(SFileStatus &st) {
    if (m_filepath.empty()) return FALSE;
#ifdef _WIN32
    struct _stat s;
    if (_stat(m_filepath.c_str(), &s) != 0) return FALSE;
    st.mode = static_cast<UINT32>(s.st_mode);
    st.inode = static_cast<UINT32>(s.st_ino);
    st.dev = static_cast<UINT32>(s.st_dev);
    st.rdev = static_cast<UINT32>(s.st_rdev);
    st.nlink = static_cast<UINT32>(s.st_nlink);
    st.uid = 0; st.gid = 0;
    st.size = static_cast<UINT32>(s.st_size);
    st.atime = s.st_atime; st.mtime = s.st_mtime; st.ctime = s.st_ctime;
    return TRUE;
#else
    struct stat s{};
    if (stat(m_filepath.c_str(), &s) != 0) return FALSE;
    st.mode = static_cast<UINT32>(s.st_mode);
    st.inode = static_cast<UINT32>(s.st_ino);
    st.dev = static_cast<UINT32>(s.st_dev);
    st.rdev = static_cast<UINT32>(s.st_rdev);
    st.nlink = static_cast<UINT32>(s.st_nlink);
    st.uid = static_cast<UINT32>(s.st_uid);
    st.gid = static_cast<UINT32>(s.st_gid);
    st.size = static_cast<UINT32>(s.st_size);
    st.atime = s.st_atime; st.mtime = s.st_mtime; st.ctime = s.st_ctime;
    return TRUE;
#endif
}

// ---------------- Global helpers ----------------

BOOL SSAPI SDGetFileStatus(const CHAR* pszFileName, SFileStatus& stStatus) {
    if (!pszFileName) return FALSE;
#ifdef _WIN32
    struct _stat s;
    if (_stat(pszFileName, &s) != 0) return FALSE;
    stStatus.mode = static_cast<UINT32>(s.st_mode);
    stStatus.inode = static_cast<UINT32>(s.st_ino);
    stStatus.dev = static_cast<UINT32>(s.st_dev);
    stStatus.rdev = static_cast<UINT32>(s.st_rdev);
    stStatus.nlink = static_cast<UINT32>(s.st_nlink);
    stStatus.uid = 0; stStatus.gid = 0;
    stStatus.size = static_cast<UINT32>(s.st_size);
    stStatus.atime = s.st_atime; stStatus.mtime = s.st_mtime; stStatus.ctime = s.st_ctime;
    return TRUE;
#else
    struct stat s{};
    if (stat(pszFileName, &s) != 0) return FALSE;
    stStatus.mode = static_cast<UINT32>(s.st_mode);
    stStatus.inode = static_cast<UINT32>(s.st_ino);
    stStatus.dev = static_cast<UINT32>(s.st_dev);
    stStatus.rdev = static_cast<UINT32>(s.st_rdev);
    stStatus.nlink = static_cast<UINT32>(s.st_nlink);
    stStatus.uid = static_cast<UINT32>(s.st_uid);
    stStatus.gid = static_cast<UINT32>(s.st_gid);
    stStatus.size = static_cast<UINT32>(s.st_size);
    stStatus.atime = s.st_atime; stStatus.mtime = s.st_mtime; stStatus.ctime = s.st_ctime;
    return TRUE;
#endif
}

BOOL SSAPI SDFileRemove(const CHAR* pszFileName) {
    if (!pszFileName) return FALSE;
    std::error_code ec;
    return fs::remove(fs::path(pszFileName), ec) ? TRUE : FALSE;
}

INT32 SSAPI SDFileRename(const CHAR *pszOldName, const CHAR *pszNewName, BOOL bForce) {
    if (!pszOldName || !pszNewName) return -1;
    std::error_code ec;
    if (bForce) {
        fs::remove(fs::path(pszNewName), ec);
    }
    fs::rename(fs::path(pszOldName), fs::path(pszNewName), ec);
    return ec ? -1 : 0;
}

INT32 SSAPI SDFileMove(const CHAR *pszOldName, const CHAR *pszNewName, BOOL bForce) {
    return SDFileRename(pszOldName, pszNewName, bForce);
}

INT32 SSAPI SDAccess(const CHAR *pszPath, INT32 mode) {
    if (!pszPath) return -1;
#ifdef _WIN32
    return _access(pszPath, mode);
#else
    return access(pszPath, mode);
#endif
}

BOOL SSAPI SDDirectoryCreate(const CHAR *pszPath) {
    if (!pszPath) return FALSE;
    std::error_code ec;
    return fs::create_directories(fs::path(pszPath), ec) ? TRUE : FALSE;
}

BOOL SSAPI SDDirectoryRemove(const CHAR *pszPath) {
    if (!pszPath) return FALSE;
    std::error_code ec;
    return fs::remove_all(fs::path(pszPath), ec) > 0 ? TRUE : FALSE;
}

INT32 SSAPI SDGetFileList(const CHAR *pszPath, const CHAR *pszExt, std::vector<std::string>& rFiles) {
    if (!pszPath) return -1;
    rFiles.clear();
    std::error_code ec;
    fs::path base(pszPath);
    if (!fs::exists(base, ec)) return -1;
    std::string ext = pszExt ? pszExt : "";
    for (auto &p : fs::directory_iterator(base, ec)) {
        if (ec) break;
        if (!p.is_regular_file()) continue;
        if (!ext.empty()) {
            if (p.path().extension() != ext) continue;
        }
        rFiles.emplace_back(p.path().string());
    }
    return static_cast<INT32>(rFiles.size());
}

const CHAR* SSAPI SDGetModuleName(void) {
    static thread_local std::string name;
#ifdef _WIN32
    char buf[SD_MAX_PATH] = {0};
    if (GetModuleFileNameA(NULL, buf, sizeof(buf)) > 0) {
        name = fs::path(buf).filename().string();
    } else {
        name.clear();
    }
#else
    name = ""; // TBD: /proc/self/exe on Linux, _NSGetExecutablePath on macOS
#endif
    return name.c_str();
}

const CHAR* SSAPI SDGetModulePath() {
    static thread_local std::string path;
#ifdef _WIN32
    char buf[SD_MAX_PATH] = {0};
    if (GetModuleFileNameA(NULL, buf, sizeof(buf)) > 0) {
        path = fs::path(buf).parent_path().string();
    } else {
        path.clear();
    }
#else
    path = fs::current_path().string();
#endif
    return path.c_str();
}

const CHAR* SSAPI SDGetWorkPath() {
    static thread_local std::string path;
    std::error_code ec;
    path = fs::current_path(ec).string();
    return path.c_str();
}

std::string SSAPI SDFileExtractPath(const CHAR *pszFileName) {
    if (!pszFileName) return std::string();
    return fs::path(pszFileName).parent_path().string();
}

std::string SSAPI SDFileExtractName(const CHAR *pszFileName) {
    if (!pszFileName) return std::string();
    return fs::path(pszFileName).filename().string();
}

std::string SSAPI SDFileExtractExt(const CHAR *pszFileName) {
    if (!pszFileName) return std::string();
    return fs::path(pszFileName).extension().string();
}

std::string SSAPI SDFileExcludeTrailingDelimiter(const CHAR *pszPath) {
    if (!pszPath) return std::string();
    std::string s(pszPath);
    while (!s.empty() && SDPATH_IS_DELIMITER(s.back())) s.pop_back();
    return s;
}

std::string SSAPI SDFileIncludeTrailingDelimiter(const CHAR *pszPath) {
    if (!pszPath) return std::string();
    std::string s(pszPath);
    if (s.empty() || SDPATH_IS_DELIMITER(s.back())) return s;
    s.push_back(SDPATH_DELIMITER);
    return s;
}

std::string SSAPI SDFileIncludeLeadingDelimiter(const CHAR *pszPath) {
    if (!pszPath) return std::string();
    std::string s(pszPath);
    if (!s.empty() && SDPATH_IS_DELIMITER(s.front())) return s;
    std::string t; t.push_back(SDPATH_DELIMITER); t += s;
    return t;
}

std::string SSAPI SDFileExcludeLeadingDelimiter(const CHAR *pszPath) {
    if (!pszPath) return std::string();
    std::string s(pszPath);
    while (!s.empty() && SDPATH_IS_DELIMITER(s.front())) s.erase(s.begin());
    return s;
}

std::string SSAPI SDFileReplaceDelimiter(const CHAR *pszPath, CHAR delimiter) {
    if (!pszPath) return std::string();
    std::string s(pszPath);
    for (auto &c : s) {
        if (SDPATH_IS_DELIMITER(c)) c = delimiter;
    }
    return s;
}

BOOL SSAPI SDFileExists(const CHAR *pszFileName) {
    if (!pszFileName) return FALSE;
    std::error_code ec;
    return fs::exists(fs::path(pszFileName), ec) ? TRUE : FALSE;
}

INT32 SSAPI SDFileGetSize(const CHAR *pszFileName) {
    if (!pszFileName) return -1;
    std::error_code ec;
    auto sz = fs::file_size(fs::path(pszFileName), ec);
    if (ec) return -1;
    return static_cast<INT32>(sz);
}

BOOL SSAPI SDFileCreate(const CHAR *pszFileName) {
    if (!pszFileName) return FALSE;
    std::FILE* f = std::fopen(pszFileName, "wb");
    if (!f) return FALSE;
    std::fclose(f);
    return TRUE;
}

BOOL SSAPI SDFileCopy(const CHAR *pszExistFile, const CHAR *pNewFile, BOOL bFailIfExists) {
    if (!pszExistFile || !pNewFile) return FALSE;
    std::error_code ec;
    fs::copy_options opts = bFailIfExists ? fs::copy_options::none
                                          : fs::copy_options::overwrite_existing;
    fs::copy_file(fs::path(pszExistFile), fs::path(pNewFile), opts, ec);
    return ec ? FALSE : TRUE;
}

} // namespace SSCP

