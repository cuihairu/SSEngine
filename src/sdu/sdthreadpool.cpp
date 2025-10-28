#include "ssengine/sdthreadpool.h"

#include "ssengine/sdcondition.h"
#include "ssengine/sdmutex.h"
#include "ssengine/sdthread.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace SSCP {

namespace {

class MutexGuard {
public:
    explicit MutexGuard(CSDMutex& m) : _mutex(m) { _mutex.Lock(); }
    ~MutexGuard() { _mutex.Unlock(); }

private:
    CSDMutex& _mutex;
};

} // namespace

CSDThreadPool::CSDThreadPool()
    : m_minThreads(0)
    , m_maxThreads(0)
    , m_maxPendingJobs(0)
    , m_waitTerminate(FALSE)
    , m_quickTerminate(FALSE) {}

CSDThreadPool::~CSDThreadPool() {
    TerminateQuick();
}

BOOL CSDThreadPool::Init(UINT32 dwMinThrds, UINT32 dwMaxThrds, UINT32 dwMaxPendJobs) {
    TerminateQuick();

    if (dwMinThrds == 0) {
        dwMinThrds = 1;
    }
    if (dwMaxThrds == 0) {
        dwMaxThrds = dwMinThrds;
    }
    dwMaxThrds = std::max(dwMinThrds, dwMaxThrds);

    m_minThreads = dwMinThrds;
    m_maxThreads = dwMaxThrds;
    m_maxPendingJobs = dwMaxPendJobs;
    m_waitTerminate = FALSE;
    m_quickTerminate = FALSE;

    for (UINT32 i = 0; i < m_minThreads; ++i) {
        ThreadArg* arg = new ThreadArg();
        arg->pThreadPool = this;
        arg->pThread = new CSDThread();
        arg->keepWorking = TRUE;

        if (!arg->pThread->Start(&CSDThreadPool::WorkThreadFunc, arg)) {
            delete arg->pThread;
            delete arg;
            TerminateQuick();
            return FALSE;
        }

        MutexGuard guard(m_threadMutex);
        m_threadArgContainer.push_back(arg);
        m_threadArgMap.insert(ThreadArgPair(arg, arg));
    }
    return TRUE;
}

BOOL CSDThreadPool::ScheduleJob(ISSRunable* pJob) {
    if (!pJob) {
        return FALSE;
    }
    if (m_quickTerminate || m_waitTerminate) {
        return FALSE;
    }

    size_t pendingAfterPush = 0;
    {
        MutexGuard jobGuard(m_jobMutex);
        if (m_quickTerminate || m_waitTerminate) {
            return FALSE;
        }
        if (m_maxPendingJobs > 0 && m_jobContainer.size() >= m_maxPendingJobs) {
            return FALSE;
        }
        m_jobContainer.push_back(pJob);
        pendingAfterPush = m_jobContainer.size();
        m_jobCondition.Signal();
    }

    // If there are more pending jobs than threads and capacity allows, add a worker.
    while (true) {
        size_t threadCount = 0;
        {
            MutexGuard threadGuard(m_threadMutex);
            threadCount = m_threadArgContainer.size();
        }
        if (threadCount >= m_maxThreads || pendingAfterPush <= threadCount) {
            break;
        }

        ThreadArg* arg = new ThreadArg();
        arg->pThreadPool = this;
        arg->pThread = new CSDThread();
        arg->keepWorking = TRUE;
        if (!arg->pThread->Start(&CSDThreadPool::WorkThreadFunc, arg)) {
            delete arg->pThread;
            delete arg;
            break;
        }

        {
            MutexGuard threadGuard(m_threadMutex);
            m_threadArgContainer.push_back(arg);
            m_threadArgMap.insert(ThreadArgPair(arg, arg));
            threadCount = m_threadArgContainer.size();
        }

        if (threadCount >= m_maxThreads || pendingAfterPush <= threadCount) {
            break;
        }
    }

    return TRUE;
}

void CSDThreadPool::TerminateQuick() {
    {
        MutexGuard jobGuard(m_jobMutex);
        if (m_jobContainer.empty() && m_threadArgContainer.empty()) {
            m_quickTerminate = FALSE;
            m_waitTerminate = FALSE;
            return;
        }
        m_quickTerminate = TRUE;
        m_jobContainer.clear();
        m_jobCondition.Broadcast();
    }

    std::vector<ThreadArg*> threads;
    {
        MutexGuard threadGuard(m_threadMutex);
        for (ThreadArg* arg : m_threadArgContainer) {
            arg->keepWorking = FALSE;
            threads.push_back(arg);
        }
    }

    for (ThreadArg* arg : threads) {
        if (arg && arg->pThread) {
            arg->pThread->Wait();
        }
    }

    {
        MutexGuard threadGuard(m_threadMutex);
        for (ThreadArg* arg : m_threadArgContainer) {
            delete arg->pThread;
            delete arg;
        }
        m_threadArgContainer.clear();
        m_threadArgMap.clear();
    }

    m_quickTerminate = FALSE;
    m_waitTerminate = FALSE;
}

void CSDThreadPool::TerminateWaitJobs() {
    m_waitTerminate = TRUE;

    // Wait until all queued jobs are consumed.
    while (true) {
        {
            MutexGuard jobGuard(m_jobMutex);
            if (m_jobContainer.empty()) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAITTIME));
    }

    {
        MutexGuard jobGuard(m_jobMutex);
        m_jobCondition.Broadcast();
    }

    std::vector<ThreadArg*> threads;
    {
        MutexGuard threadGuard(m_threadMutex);
        for (ThreadArg* arg : m_threadArgContainer) {
            arg->keepWorking = FALSE;
            threads.push_back(arg);
        }
    }

    for (ThreadArg* arg : threads) {
        if (arg && arg->pThread) {
            arg->pThread->Wait();
        }
    }

    {
        MutexGuard threadGuard(m_threadMutex);
        for (ThreadArg* arg : m_threadArgContainer) {
            delete arg->pThread;
            delete arg;
        }
        m_threadArgContainer.clear();
        m_threadArgMap.clear();
    }

    m_waitTerminate = FALSE;
    m_quickTerminate = FALSE;
}

UINT32 CSDThreadPool::GetThreadNum() {
    MutexGuard guard(m_threadMutex);
    return static_cast<UINT32>(m_threadArgContainer.size());
}

UINT32 CSDThreadPool::GetJobPending() {
    MutexGuard guard(m_jobMutex);
    return static_cast<UINT32>(m_jobContainer.size());
}

SDTHREAD_DECLARE(CSDThreadPool::WorkThreadFunc)(void* pArg) {
    ThreadArg* arg = static_cast<ThreadArg*>(pArg);
    if (!arg || !arg->pThreadPool) {
        SDTHREAD_RETURN(0);
    }
    CSDThreadPool* pool = arg->pThreadPool;

    while (arg->keepWorking && !pool->m_quickTerminate) {
        ISSRunable* job = nullptr;
        {
            MutexGuard jobGuard(pool->m_jobMutex);
            UINT32 waitCount = 0;
            while (arg->keepWorking && !pool->m_quickTerminate && pool->m_jobContainer.empty()) {
                if (pool->m_waitTerminate) {
                    arg->keepWorking = FALSE;
                    break;
                }
                if (waitCount++ > WAITCOUNT) {
                    waitCount = 0;
                }
                pool->m_jobCondition.Wait(pool->m_jobMutex, WAITTIME);
            }
            if (!arg->keepWorking || pool->m_quickTerminate) {
                break;
            }
            if (!pool->m_jobContainer.empty()) {
                job = pool->m_jobContainer.front();
                pool->m_jobContainer.pop_front();
            } else if (pool->m_waitTerminate) {
                arg->keepWorking = FALSE;
            }
        }

        if (!job) {
            continue;
        }

        job->Run();

        {
            MutexGuard jobGuard(pool->m_jobMutex);
            if (pool->m_jobContainer.empty()) {
                pool->m_jobCondition.Broadcast();
            }
        }
    }

    arg->keepWorking = FALSE;
    SDTHREAD_RETURN(0);
}

} // namespace SSCP

