#include "ssengine/sdmutex.h"
#include "ssengine/sdcondition.h"
#ifndef WINDOWS
#  include <pthread.h>
#endif

namespace SSCP {

BOOL SDMutexInit(SDMutexId & id) {
#ifdef WINDOWS
    ::InitializeCriticalSection(&id);
    return TRUE;
#else
    return pthread_mutex_init(&id, nullptr) == 0 ? TRUE : FALSE;
#endif
}

void SDMutexLock(SDMutexId & id) {
#ifdef WINDOWS
    ::EnterCriticalSection(&id);
#else
    pthread_mutex_lock(&id);
#endif
}

void SDMutexUnlock(SDMutexId & id) {
#ifdef WINDOWS
    ::LeaveCriticalSection(&id);
#else
    pthread_mutex_unlock(&id);
#endif
}

BOOL SDMutexUninit(SDMutexId &id) {
#ifdef WINDOWS
    ::DeleteCriticalSection(&id);
    return TRUE;
#else
    return pthread_mutex_destroy(&id) == 0 ? TRUE : FALSE;
#endif
}

// --------------- CSDMutex ---------------

CSDMutex::CSDMutex() {
#ifdef WINDOWS
    ::InitializeCriticalSection(&m_mutexId);
#else
    pthread_mutex_init(&m_mutexId, nullptr);
#endif
}

CSDMutex::~CSDMutex() {
#ifdef WINDOWS
    ::DeleteCriticalSection(&m_mutexId);
#else
    pthread_mutex_destroy(&m_mutexId);
#endif
}

void CSDMutex::Lock() {
#ifdef WINDOWS
    ::EnterCriticalSection(&m_mutexId);
#else
    pthread_mutex_lock(&m_mutexId);
#endif
}

void CSDMutex::Unlock() {
#ifdef WINDOWS
    ::LeaveCriticalSection(&m_mutexId);
#else
    pthread_mutex_unlock(&m_mutexId);
#endif
}

// --------------- CSDRWMutex ---------------

CSDRWMutex::CSDRWMutex()
  : m_readerCount(0)
  , m_hasWriterLock(false)
  , m_pendingWriteCount(0) {}

CSDRWMutex::~CSDRWMutex() {}

void CSDRWMutex::LockRead() {
    m_mutex.Lock();
    // Prefer writer priority if any pending writers exist
    while (m_hasWriterLock || m_pendingWriteCount > 0) {
        m_readCond.Wait(m_mutex, SDINFINITE);
    }
    ++m_readerCount;
    m_mutex.Unlock();
}

void CSDRWMutex::LockWrite() {
    m_mutex.Lock();
    ++m_pendingWriteCount;
    while (m_hasWriterLock || m_readerCount > 0) {
        m_pendingWriteCond.Wait(m_mutex, SDINFINITE);
    }
    // acquire write
    --m_pendingWriteCount;
    m_hasWriterLock = true;
    m_mutex.Unlock();
}

void CSDRWMutex::Lock() {
    LockWrite();
}

void CSDRWMutex::Unlock() {
    m_mutex.Lock();
    if (m_hasWriterLock) {
        // release writer
        m_hasWriterLock = false;
        if (m_pendingWriteCount > 0) {
            // wake one pending writer
            m_pendingWriteCond.Signal();
        } else {
            // wake all readers
            m_readCond.Broadcast();
        }
    } else {
        // release reader
        if (m_readerCount > 0) {
            --m_readerCount;
            if (m_readerCount == 0 && m_pendingWriteCount > 0) {
                // last reader leaves, wake one writer
                m_pendingWriteCond.Signal();
            }
        }
    }
    m_mutex.Unlock();
}

unsigned int CSDRWMutex::ReaderCount() const {
    return m_readerCount;
}

unsigned int CSDRWMutex::PendingWriterCount() const {
    return m_pendingWriteCount;
}

// --------------- CSDRecursiveMutex ---------------

CSDRecursiveMutex::CSDRecursiveMutex() {
#ifdef WINDOWS
    ::InitializeCriticalSection(&m_mutexId); // CRITICAL_SECTION is recursive
#else
    pthread_mutexattr_t attr; pthread_mutexattr_init(&m_mutexAttr);
    pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutexId, &m_mutexAttr);
#endif
}

CSDRecursiveMutex::~CSDRecursiveMutex() {
#ifdef WINDOWS
    ::DeleteCriticalSection(&m_mutexId);
#else
    pthread_mutex_destroy(&m_mutexId);
    pthread_mutexattr_destroy(&m_mutexAttr);
#endif
}

void CSDRecursiveMutex::Lock() {
#ifdef WINDOWS
    ::EnterCriticalSection(&m_mutexId);
#else
    pthread_mutex_lock(&m_mutexId);
#endif
}

void CSDRecursiveMutex::Unlock() {
#ifdef WINDOWS
    ::LeaveCriticalSection(&m_mutexId);
#else
    pthread_mutex_unlock(&m_mutexId);
#endif
}

} // namespace SSCP
