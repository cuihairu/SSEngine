#include "ssengine/sdtimer.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace SSCP {

namespace {
struct TimerEntry {
    ISSTimer* handler{nullptr};
    UINT32 id{0};
    UINT32 elapse{0};
    UINT32 loop{0};
    std::chrono::steady_clock::time_point next;
};
}

class CSDTimerImpl {
public:
    BOOL SetTimer(ISSTimer* handler, UINT32 timerId, UINT32 elapse, UINT32 loop) {
        if (!handler || elapse == 0) {
            return FALSE;
        }
        std::lock_guard<std::mutex> lk(_mutex);
        TimerEntry entry;
        entry.handler = handler;
        entry.id = timerId;
        entry.elapse = elapse;
        entry.loop = loop;
        entry.next = std::chrono::steady_clock::now() + std::chrono::milliseconds(elapse);
        _entries.push_back(entry);
        return TRUE;
    }

    BOOL KillTimer(UINT32 timerId) {
        std::lock_guard<std::mutex> lk(_mutex);
        auto it = std::remove_if(_entries.begin(), _entries.end(), [timerId](const TimerEntry& e) {
            return e.id == timerId;
        });
        bool removed = it != _entries.end();
        _entries.erase(it, _entries.end());
        return removed ? TRUE : FALSE;
    }

    BOOL Run() {
        std::vector<TimerEntry> callbacks;
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lk(_mutex);
            for (auto& entry : _entries) {
                if (now >= entry.next) {
                    callbacks.push_back(entry);
                    if (entry.loop == 1) {
                        entry.loop = 0;
                    } else if (entry.loop > 1 && entry.loop != 0xFFFFFFFFu) {
                        --entry.loop;
                    }
                    entry.next = now + std::chrono::milliseconds(entry.elapse);
                }
            }
            _entries.erase(std::remove_if(_entries.begin(), _entries.end(), [](const TimerEntry& e) {
                return e.loop == 0;
            }), _entries.end());
        }
        for (auto& cb : callbacks) {
            if (cb.handler) {
                cb.handler->OnTimer(cb.id);
            }
        }
        return !callbacks.empty() ? TRUE : FALSE;
    }

private:
    std::mutex _mutex;
    std::vector<TimerEntry> _entries;
};

CSDTimer::CSDTimer()
    : m_pTimerImpl(new CSDTimerImpl()) {}

CSDTimer::~CSDTimer() {
    delete m_pTimerImpl;
    m_pTimerImpl = nullptr;
}

BOOL SSAPI CSDTimer::SetTimer(ISSTimer* pTimer, UINT32 dwTimerId, UINT32 dwElapse, UINT32 dwLoop) {
    return m_pTimerImpl->SetTimer(pTimer, dwTimerId, dwElapse, dwLoop);
}

BOOL SSAPI CSDTimer::KillTimer(UINT32 dwTimerId) {
    return m_pTimerImpl->KillTimer(dwTimerId);
}

BOOL SSAPI CSDTimer::Run() {
    return m_pTimerImpl->Run();
}

std::string SSAPI CSDTimer::DumpTimerInfo() {
    return "timer-count:"; // placeholder summary
}

} // namespace SSCP
