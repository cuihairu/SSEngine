#include <gtest/gtest.h>
#include "sdmutex.h"
#include "sdcondition.h"

using namespace SSCP;

TEST(sdmutex, basic_lock_unlock) {
#if defined(_WIN32)
    CSDMutex m;
    m.Lock();
    m.Unlock();
    // recursive lock not guaranteed here
#else
    GTEST_SKIP() << "non-Windows implementation pending";
#endif
}

#if defined(_WIN32)
TEST(sdmutex, rw_mutex_basic) {
    CSDRWMutex rw;
    // Readers can enter when no writers
    rw.LockRead();
    EXPECT_EQ(rw.ReaderCount(), 1u);
    rw.Unlock(); // reader unlock
}

TEST(sdmutex, condition_signal_basic) {
    CSDMutex m;
    CSDCondition cv;
    // Can't reliably test cross-thread timing here; just call APIs
    m.Lock();
    cv.Signal();
    cv.Broadcast();
    m.Unlock();
}
#endif
