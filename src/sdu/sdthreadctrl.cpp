#include "ssengine/sdthreadctrl.h"
#include "ssengine/sdthread.h"

#include <memory>
#include <mutex>
#include <condition_variable>

namespace SSCP {

namespace {

class ThreadCtrl : public ISSThreadCtrl {
public:
    explicit ThreadCtrl(ISSThread* thread)
        : _thread(thread)
        , _handle(SDINVALID_HANDLE)
        , _threadId(0)
        , _terminated(false) {
        if (_thread) {
            // Launch thread immediately.
            SDTHREADID id = 0;
            _handle = SDCreateThread(nullptr, &ThreadCtrl::Entry, this, &id, FALSE);
            _threadId = static_cast<UINT32>(id);
        }
    }

    ~ThreadCtrl() override {
        if (_handle != SDINVALID_HANDLE) {
            SDThreadWait(_handle);
            SDThreadCloseHandle(_handle);
        }
        delete _thread;
    }

    void SSAPI Resume(void) override {
        // No-op for portable implementation.
    }

    void SSAPI Suspend() override {
        // No-op: suspension not supported on POSIX.
    }

    void SSAPI Terminate(void) override {
        if (_thread) {
            _thread->Terminate();
        }
    }

    bool SSAPI Kill(UINT32 /*dwExitCode*/) override {
        Terminate();
        return true;
    }

    bool SSAPI WaitFor(UINT32 dwWaitTime = SD_INFINITE) override {
        if (_handle == SDINVALID_HANDLE) {
            return true;
        }
        if (dwWaitTime == SD_INFINITE) {
            return SDThreadWait(_handle) == 0;
        }
        std::unique_lock<std::mutex> lk(_mutex);
        return _cv.wait_for(lk, std::chrono::milliseconds(dwWaitTime), [this]() { return _terminated; });
    }

    UINT32 SSAPI GetThreadId(void) override {
        return _threadId;
    }

    ISSThread* SSAPI GetThread(void) override {
        return _thread;
    }

    void SSAPI Release(void) override {
        delete this;
    }

private:
    static SDTHREAD_DECLARE(Entry)(void* arg) {
        auto* self = static_cast<ThreadCtrl*>(arg);
        if (self && self->_thread) {
            self->_thread->ThrdProc();
        }
        if (self) {
            std::lock_guard<std::mutex> lk(self->_mutex);
            self->_terminated = true;
            self->_cv.notify_all();
        }
        SDTHREAD_RETURN(0);
    }

    ISSThread* _thread;
    SDHANDLE _handle;
    UINT32 _threadId;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _terminated;
};

} // namespace

ISSThreadCtrl* CreateThreadCtrl(ISSThread* thread) {
    return new ThreadCtrl(thread);
}

} // namespace SSCP
