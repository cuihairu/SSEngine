#include "ssengine/sdcondition.h"
#include "ssengine/sdmutex.h"

#ifdef WINDOWS
#  include <windows.h>
#else
#  include <pthread.h>
#  include <time.h>
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
    pthread_cond_init(&mId, nullptr);
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
    pthread_cond_destroy(&mId);
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
    int rc = 0;
    if (dwMs == SDINFINITE) {
        rc = pthread_cond_wait(&mId, &mutex.m_mutexId);
    } else {
        struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += dwMs / 1000;
        ts.tv_nsec += (dwMs % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L) { ts.tv_sec += 1; ts.tv_nsec -= 1000000000L; }
        rc = pthread_cond_timedwait(&mId, &mutex.m_mutexId, &ts);
    }
    return rc == 0 ? TRUE : FALSE;
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
    pthread_cond_signal(&mId);
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
    pthread_cond_broadcast(&mId);
#endif
}

} // namespace SSCP
