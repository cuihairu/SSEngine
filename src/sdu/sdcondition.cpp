#include "sdcondition.h"
#include "sdmutex.h"

#ifdef WINDOWS
#  include <windows.h>
#endif

namespace SSCP {

CSDCondition::CSDCondition() {
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
    // Use native CONDITION_VARIABLE while keeping pointers opaque
    auto* cv = new CONDITION_VARIABLE{};
    ::InitializeConditionVariable(cv);
    m_gate = cv; // reuse gate as cv store
    m_queue = nullptr;
    m_mutex = nullptr;
    m_gone = 0; m_blocked = 0; m_waiting = 0;
#  else
    mId = ::CreateEvent(NULL, TRUE, FALSE, NULL);
#  endif
#else
    // TODO: pthread_cond_init for non-Windows platform in later phases
#endif
}

CSDCondition::~CSDCondition() {
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
    auto* cv = reinterpret_cast<CONDITION_VARIABLE*>(m_gate);
    delete cv;
    m_gate = nullptr;
#  else
    if (mId) ::CloseHandle(mId);
#  endif
#else
    // TODO: pthread_cond_destroy later
#endif
}

BOOL CSDCondition::Wait (CSDMutex &mutex, UINT32 dwMs) {
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
    auto* cv = reinterpret_cast<CONDITION_VARIABLE*>(m_gate);
    DWORD timeout = (dwMs == SDINFINITE) ? INFINITE : dwMs;
    BOOL ok = ::SleepConditionVariableCS(cv, &mutex.m_mutexId, timeout);
    return ok ? TRUE : FALSE;
#  else
    DWORD timeout = (dwMs == SDINFINITE) ? INFINITE : dwMs;
    // Auto-reset event emulation would need a queue; fallback simple
    DWORD rc = ::WaitForSingleObject(mId, timeout);
    return (rc == WAIT_OBJECT_0) ? TRUE : FALSE;
#  endif
#else
    (void)mutex; (void)dwMs; return FALSE;
#endif
}

BOOL CSDCondition::Wait (CSDMutex *mutex, UINT32 dwMs) {
    if (!mutex) return FALSE;
    return Wait(*mutex, dwMs);
}

void CSDCondition::Signal() {
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
    auto* cv = reinterpret_cast<CONDITION_VARIABLE*>(m_gate);
    ::WakeConditionVariable(cv);
#  else
    ::SetEvent(mId);
#  endif
#else
    // TODO: pthread_cond_signal later
#endif
}

void CSDCondition::Broadcast() {
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX
    auto* cv = reinterpret_cast<CONDITION_VARIABLE*>(m_gate);
    ::WakeAllConditionVariable(cv);
#  else
    ::SetEvent(mId);
#  endif
#else
    // TODO: pthread_cond_broadcast later
#endif
}

} // namespace SSCP

