#include "ssengine/sdprocess.h"

#include <cstring>
#include <string>
#include <vector>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace SSCP {
namespace {

struct EnvBuffer {
    std::vector<char> data;

    explicit EnvBuffer(const std::string& source) {
        if (source.empty()) {
            return;
        }
        data.reserve(source.size() + 2);
        bool hasNull = false;
        for (char ch : source) {
            if (ch == '\n') {
                data.push_back('\0');
            } else {
                data.push_back(ch);
            }
            hasNull = (ch == '\0');
        }
        if (!hasNull || (!data.empty() && data.back() != '\0')) {
            data.push_back('\0');
        }
        data.push_back('\0');
    }
};

#ifndef WINDOWS
void ApplyEnvironment(const std::string& envSpec) {
    if (envSpec.empty()) {
        return;
    }
    size_t start = 0;
    while (start < envSpec.size()) {
        size_t end = envSpec.find_first_of("\n\0", start);
        if (end == std::string::npos) {
            end = envSpec.size();
        }
        if (end > start) {
            std::string kv = envSpec.substr(start, end - start);
            putenv(strdup(kv.c_str()));
        }
        start = end + 1;
    }
}
#endif

} // namespace

SDHANDLE SSAPI SDCreateProcess(
    const CHAR* progName,
    const CHAR* pCmdLine,
    const CHAR* pWorkDir,
    SProcessAttr* pAttr) {
    if (!progName) {
        return SDINVALID_HANDLE;
    }

#ifdef WINDOWS
    std::string workDir;
    if (pAttr && !pAttr->workDir.empty()) {
        workDir = pAttr->workDir;
    } else if (pWorkDir) {
        workDir = pWorkDir;
    }

    std::string cmdLineStr = pCmdLine ? pCmdLine : "";
    std::vector<char> cmdBuffer;
    if (!cmdLineStr.empty()) {
        cmdBuffer.assign(cmdLineStr.begin(), cmdLineStr.end());
        cmdBuffer.push_back('\0');
    }

    EnvBuffer envBuffer(pAttr ? pAttr->environment : std::string());

    STARTUPINFOA startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION procInfo{};

    SECURITY_ATTRIBUTES* pProcSec = nullptr;
    SECURITY_ATTRIBUTES procSec{};
    SECURITY_ATTRIBUTES* pThreadSec = nullptr;
    SECURITY_ATTRIBUTES threadSec{};

    if (pAttr && pAttr->secInfo.bSet && pAttr->secInfo.pSecurityDescriptor) {
        procSec.nLength = sizeof(procSec);
        procSec.lpSecurityDescriptor = pAttr->secInfo.pSecurityDescriptor;
        procSec.bInheritHandle = pAttr->inherithandle ? TRUE : FALSE;
        pProcSec = &procSec;
        threadSec = procSec;
        pThreadSec = &threadSec;
    }

    BOOL inheritHandles = pAttr ? (pAttr->inherithandle ? TRUE : FALSE) : FALSE;
    DWORD creationFlags = pAttr ? pAttr->createFlags : 0;

    BOOL result = CreateProcessA(
        progName,
        cmdBuffer.empty() ? nullptr : cmdBuffer.data(),
        pProcSec,
        pThreadSec,
        inheritHandles,
        creationFlags,
        envBuffer.data.empty() ? nullptr : envBuffer.data.data(),
        workDir.empty() ? (pWorkDir ? pWorkDir : nullptr) : workDir.c_str(),
        &startupInfo,
        &procInfo);

    if (!result && progName) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            std::string fullCmd = progName;
            if (!cmdLineStr.empty()) {
                fullCmd.push_back(' ');
                fullCmd.append(cmdLineStr);
            }
            std::vector<char> fallback(fullCmd.begin(), fullCmd.end());
            fallback.push_back('\0');
            result = CreateProcessA(
                nullptr,
                fallback.data(),
                pProcSec,
                pThreadSec,
                inheritHandles,
                creationFlags,
                envBuffer.data.empty() ? nullptr : envBuffer.data.data(),
                workDir.empty() ? (pWorkDir ? pWorkDir : nullptr) : workDir.c_str(),
                &startupInfo,
                &procInfo);
        }
        if (!result) {
            return SDINVALID_HANDLE;
        }
    }

    if (!result) {
        return SDINVALID_HANDLE;
    }

    CloseHandle(procInfo.hThread);
    return static_cast<SDHANDLE>(procInfo.hProcess);
#else
    pid_t pid = fork();
    if (pid < 0) {
        return SDINVALID_HANDLE;
    }

    if (pid == 0) {
        if (pAttr && !pAttr->workDir.empty()) {
            chdir(pAttr->workDir.c_str());
        } else if (pWorkDir) {
            chdir(pWorkDir);
        }

        if (pAttr) {
            ApplyEnvironment(pAttr->environment);
        }

        std::string command = progName;
        if (pCmdLine && std::strlen(pCmdLine) > 0) {
            command.append(" ");
            command.append(pCmdLine);
        }
        const char* shell = "/bin/sh";
        execl(shell, shell, "-c", command.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    return static_cast<SDHANDLE>(pid);
#endif
}

void SSAPI SDTerminateProcess(SDHANDLE handle, INT32 err) {
    if (handle == SDINVALID_HANDLE) {
        return;
    }
#ifdef WINDOWS
    TerminateProcess(handle, static_cast<UINT>(err));
#else
    int signal = err == 0 ? SIGTERM : SIGKILL;
    kill(static_cast<pid_t>(handle), signal);
#endif
}

void SSAPI SDCloseProcess(SDHANDLE handle) {
    if (handle == SDINVALID_HANDLE) {
        return;
    }
#ifdef WINDOWS
    CloseHandle(handle);
#else
    (void)handle;
#endif
}

SDPROCESSID SSAPI SDGetProcessId(SDHANDLE handle) {
#ifdef WINDOWS
    return handle == SDINVALID_HANDLE ? 0 : GetProcessId(handle);
#else
    return static_cast<SDPROCESSID>(handle);
#endif
}

SDHANDLE SSAPI SDOpenProcess(SDPROCESSID pid) {
#ifdef WINDOWS
    return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
#else
    if (pid <= 0) {
        return SDINVALID_HANDLE;
    }
    if (kill(pid, 0) != 0) {
        return SDINVALID_HANDLE;
    }
    return static_cast<SDHANDLE>(pid);
#endif
}

SDHANDLE SSAPI SDGetCurrentProcess() {
#ifdef WINDOWS
    DWORD pid = GetCurrentProcessId();
    return SDOpenProcess(pid);
#else
    return static_cast<SDHANDLE>(getpid());
#endif
}

SDPROCESSID SSAPI SDGetCurrentProcessId() {
#ifdef WINDOWS
    return GetCurrentProcessId();
#else
    return static_cast<SDPROCESSID>(getpid());
#endif
}

void SSAPI SDWaitProcess(SDHANDLE handle) {
    if (handle == SDINVALID_HANDLE) {
        return;
    }
#ifdef WINDOWS
    WaitForSingleObject(handle, INFINITE);
#else
    int status = 0;
    waitpid(static_cast<pid_t>(handle), &status, 0);
#endif
}

BOOL SSAPI SDSetProcessAttr(SDHANDLE, const SProcessAttr&) {
    return false;
}

BOOL SSAPI SDGetProcessAttr(SDHANDLE, SProcessAttr&) {
    return false;
}

CSDProcess::CSDProcess() : m_handle(SDINVALID_HANDLE) {}

CSDProcess::~CSDProcess() {
    if (m_handle != SDINVALID_HANDLE) {
        SDCloseProcess(m_handle);
        m_handle = SDINVALID_HANDLE;
    }
}

BOOL SSAPI CSDProcess::Create(
    const CHAR* pProgName,
    const CHAR* pArgs,
    const CHAR* pEnvs,
    SProcessAttr* pAttr) {
    SProcessAttr attrCopy;
    SProcessAttr* attrPtr = nullptr;
    if (pAttr) {
        attrCopy = *pAttr;
        attrPtr = &attrCopy;
    }
    if (pEnvs) {
        if (!attrPtr) {
            attrPtr = &attrCopy;
        }
        attrCopy.environment = pEnvs;
    }
    SDHANDLE handle = SDCreateProcess(pProgName, pArgs, nullptr, attrPtr);
    if (handle == SDINVALID_HANDLE) {
        return false;
    }
    m_handle = handle;
    return true;
}

void SSAPI CSDProcess::Terminate() {
    if (m_handle != SDINVALID_HANDLE) {
        SDTerminateProcess(m_handle, 0);
    }
}

void SSAPI CSDProcess::Wait() {
    if (m_handle != SDINVALID_HANDLE) {
        SDWaitProcess(m_handle);
    }
}

UINT32 SSAPI CSDProcess::GetProcessID() {
    if (m_handle == SDINVALID_HANDLE) {
        return 0;
    }
    return static_cast<UINT32>(SDGetProcessId(m_handle));
}

SDHANDLE SSAPI CSDProcess::GetHandle() {
    return m_handle;
}

BOOL SSAPI CSDProcess::SetAttribute(const SProcessAttr& pAttr) {
    if (m_handle == SDINVALID_HANDLE) {
        return false;
    }
    return SDSetProcessAttr(m_handle, pAttr);
}

BOOL SSAPI CSDProcess::GetAttribute(SDHANDLE handle, SProcessAttr& procAttr) {
    return SDGetProcessAttr(handle, procAttr);
}

} // namespace SSCP
