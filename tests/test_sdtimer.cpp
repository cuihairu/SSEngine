#include <gtest/gtest.h>

#include "ssengine/sdtimer.h"
#include <chrono>
#include <thread>

using namespace SSCP;

namespace {
class TestTimer : public ISSTimer {
public:
    void SSAPI OnTimer(UINT32 dwTimerID) override {
        ++count;
        lastId = dwTimerID;
    }
    UINT32 count{0};
    UINT32 lastId{0};
};
}

TEST(sdtimer, basic_timeout) {
    CSDTimer timer;
    TestTimer handler;
    ASSERT_TRUE(timer.SetTimer(&handler, 42, 1, 1));
    // Busy wait until fired
    for (int i = 0; i < 100 && handler.count == 0; ++i) {
        timer.Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    EXPECT_EQ(handler.count, 1u);
    EXPECT_EQ(handler.lastId, 42u);
}
