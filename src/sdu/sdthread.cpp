#include "sdthread.h"
#include "sdtype.h"

#ifdef WINDOWS
#  include <windows.h>
#  include <process.h>
#endif

namespace SSCP {

SDTHREADID SSAPI SDGetThreadId() {
#ifdef WINDOWS
    return static_cast<SDTHREADID>(::GetCurrentThreadId());
#else
    // Stub for non-Windows (to be implemented in Linux/macOS phases)
    return 0;
#endif
}

SDHANDLE SSAPI SDCreateThread(
    SThreadAttr* /*pAttr*/,
    PFThrdProc pThrdProc,
    void* pArg,
    SDTHREADID* pThreadId,
    BOOL bSuspend)
{
#ifdef WINDOWS
    unsigned threadId = 0;
    unsigned initFlag = bSuspend ? CREATE_SUSPENDED : 0;
    // Prefer _beginthreadex for C runtime initialization correctness
    uintptr_t h = _beginthreadex(
        nullptr,
        0,
        pThrdProc,
        pArg,
        initFlag,
        &threadId);
    if (h == 0) {
        return SDINVALID_HANDLE;
    }
    if (pThreadId) *pThreadId = static_cast<SDTHREADID>(threadId);
    return reinterpret_cast<SDHANDLE>(h);
#else
    (void)pThrdProc; (void)pArg; (void)pThreadId; (void)bSuspend;
    return SDINVALID_HANDLE;
#endif
}

INT32 SSAPI SDThreadWait(SDHANDLE handle) {
#ifdef WINDOWS
    if (!handle) return -1;
    DWORD rc = ::WaitForSingleObject(reinterpret_cast<HANDLE>(handle), INFINITE);
    return (rc == WAIT_OBJECT_0) ? 0 : -1;
#else
    (void)handle; return -1;
#endif
}

void SSAPI SDThreadCloseHandle(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::CloseHandle(reinterpret_cast<HANDLE>(handle));
#else
    (void)handle;
#endif
}

void SSAPI SDThreadTerminate(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::TerminateThread(reinterpret_cast<HANDLE>(handle), 0);
#else
    (void)handle;
#endif
}

void SSAPI SDThreadSuspend(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::SuspendThread(reinterpret_cast<HANDLE>(handle));
#else
    (void)handle;
#endif
}

void SSAPI SDThreadResume(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::ResumeThread(reinterpret_cast<HANDLE>(handle));
#else
    (void)handle;
#endif
}

void SSAPI SDSetThreadAttr(SDHANDLE /*h*/, SThreadAttr* /*pAttr*/) {}

SThreadAttr* SSAPI SDGetThreadAttr(SDHANDLE /*h*/) { return nullptr; }

// ---------------- CSDThread ----------------

CSDThread::CSDThread()
  : m_bstart(FALSE), m_tid(0), m_handle(SDINVALID_HANDLE), m_arg(nullptr) {}

CSDThread::~CSDThread() {
#ifdef WINDOWS
    if (m_handle) {
        // Avoid leaking handle; do not force terminate here
        ::CloseHandle(reinterpret_cast<HANDLE>(m_handle));
        m_handle = SDINVALID_HANDLE;
    }
#endif
}

BOOL SSAPI CSDThread::Start(PFThrdProc pfThrdProc, void* pArg, BOOL bSuspend) {
    if (m_bstart) return FALSE;
    m_handle = SDCreateThread(nullptr, pfThrdProc, pArg, &m_tid, bSuspend);
    m_bstart = (m_handle != SDINVALID_HANDLE);
    return m_bstart;
}

// Trampoline to call virtual ThrdProc
SDTHREAD_DECLARE(CSDThread::SDThreadFunc) (void* arg) {
    CSDThread* self = static_cast<CSDThread*>(arg);
    if (self) self->ThrdProc();
    SDTHREAD_RETURN(0);
}

BOOL SSAPI CSDThread::Start(BOOL bSuspend) {
    if (m_bstart) return FALSE;
    m_arg = this;
    m_handle = SDCreateThread(nullptr, &CSDThread::SDThreadFunc, this, &m_tid, bSuspend);
    m_bstart = (m_handle != SDINVALID_HANDLE);
    return m_bstart;
}

void SSAPI CSDThread::Wait() {
    (void)SDThreadWait(m_handle);
}

void SSAPI CSDThread::Terminate() {
    SDThreadTerminate(m_handle);
}

void SSAPI CSDThread::Suspend() {
    SDThreadSuspend(m_handle);
}

void SSAPI CSDThread::Resume() {
    SDThreadResume(m_handle);
}

void SSAPI CSDThread::SetAttribute(SThreadAttr* /*pAttr*/) {}

SThreadAttr* SSAPI CSDThread::GetAttribute() { return nullptr; }

} // namespace SSCP

