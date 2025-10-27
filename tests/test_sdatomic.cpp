#include <gtest/gtest.h>
#include "sdatomic.h"

using namespace SSCP;

TEST(sdatomic, add_inc_dec_cas) {
    volatile INT32 v32 = 0;
    EXPECT_EQ(SDAtomicAdd32(&v32, 5), 0);
    EXPECT_EQ(SDAtomicRead32(&v32), 5);
    EXPECT_EQ(SDAtomicInc32(&v32), 5); // returns prior value
    EXPECT_EQ(SDAtomicRead32(&v32), 6);
    EXPECT_EQ(SDAtomicDec32(&v32), 6);
    EXPECT_EQ(SDAtomicRead32(&v32), 5);
    EXPECT_EQ(SDAtomicCas32(&v32, 10, 5), 5);
    EXPECT_EQ(SDAtomicRead32(&v32), 10);

    volatile INT64 v64 = 0;
    EXPECT_EQ(SDAtomicAdd64(&v64, 7), 0);
    EXPECT_EQ(SDAtomicRead64(&v64), 7);
    EXPECT_EQ(SDAtomicInc64(&v64), 7);
    EXPECT_EQ(SDAtomicRead64(&v64), 8);
    EXPECT_EQ(SDAtomicDec64(&v64), 8);
    EXPECT_EQ(SDAtomicRead64(&v64), 7);
    EXPECT_EQ(SDAtomicCas64(&v64, 11, 7), 7);
    EXPECT_EQ(SDAtomicRead64(&v64), 11);
}

TEST(sdatomic, xchg_ptr) {
    int a=1, b=2; volatile void* p = &a;
    void* old = SDAtomicXchgptr(&p, &b);
    EXPECT_EQ(old, &a);
    EXPECT_EQ(p, &b);
    old = SDAtomicCasptr(&p, &a, &b);
    EXPECT_EQ(old, &b);
    EXPECT_EQ(p, &a);
}
