#include <gtest/gtest.h>
#include "sdtime.h"

using namespace SSCP;

TEST(sdtime, tick_and_sleep) {
    auto t0 = SDGetTickCount();
    SDSleep(50);
    auto t1 = SDGetTickCount();
    EXPECT_GE(t1 - t0, 40u);
}

TEST(sdtime, now_and_diff) {
    auto now = SDNow();
    auto later = now;
    later.IncSecond(2);
    EXPECT_GE(later.DiffSecond(now), 2);
}
