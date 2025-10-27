#include "ssengine/sdthread.h"
#include "ssengine/sdtype.h"

#ifdef WINDOWS
#  include <windows.h>
#  include <process.h>
#else
#  include <pthread.h>
#  include <map>
#  include <mutex>
#  include <atomic>
namespace {
struct ThReg { std::mutex mtx; std::map<int, pthread_t> tbl; std::atomic<int> next{1}; } g_threg;
}
#endif

namespace SSCP {

SDTHREADID SSAPI SDGetThreadId() {
#ifdef WINDOWS
    return static_cast<SDTHREADID>(::GetCurrentThreadId());
#else
    // Derive an integer id from pthread_self
    auto self = pthread_self();
    return static_cast<SDTHREADID>(reinterpret_cast<intptr_t>(self));
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
    // Maintain a registry from small int handle to pthread_t
    pthread_t tid;
    pthread_attr_t attr; pthread_attr_init(&attr);
    int rc = pthread_create(&tid, &attr, pThrdProc, pArg);
    pthread_attr_destroy(&attr);
    if (rc != 0) return SDINVALID_HANDLE;
    int h;
    { std::lock_guard<std::mutex> lk(g_threg.mtx); h = g_threg.next++; g_threg.tbl[h] = tid; }
    if (pThreadId) *pThreadId = static_cast<SDTHREADID>(reinterpret_cast<intptr_t>(tid));
    (void)bSuspend; // not supported on POSIX
    return h;
#endif
}

INT32 SSAPI SDThreadWait(SDHANDLE handle) {
#ifdef WINDOWS
    if (!handle) return -1;
    DWORD rc = ::WaitForSingleObject(reinterpret_cast<HANDLE>(handle), INFINITE);
    return (rc == WAIT_OBJECT_0) ? 0 : -1;
#else
    // join and erase from registry
    pthread_t tid;
    {
        std::lock_guard<std::mutex> lk(g_threg.mtx);
        auto it = g_threg.tbl.find(handle);
        if (it == g_threg.tbl.end()) return -1;
        tid = it->second;
        g_threg.tbl.erase(it);
    }
    void* ret=nullptr; return pthread_join(tid, &ret)==0 ? 0 : -1;
#endif
}

void SSAPI SDThreadCloseHandle(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::CloseHandle(reinterpret_cast<HANDLE>(handle));
#else
    // nothing to close for pthreads
    (void)handle;
#endif
}

void SSAPI SDThreadTerminate(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::TerminateThread(reinterpret_cast<HANDLE>(handle), 0);
#else
    // pthread_cancel is unsafe generally; avoid
    (void)handle;
#endif
}

void SSAPI SDThreadSuspend(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::SuspendThread(reinterpret_cast<HANDLE>(handle));
#else
    (void)handle; // not supported
#endif
}

void SSAPI SDThreadResume(SDHANDLE handle) {
#ifdef WINDOWS
    if (handle) ::ResumeThread(reinterpret_cast<HANDLE>(handle));
#else
    (void)handle; // not supported
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
