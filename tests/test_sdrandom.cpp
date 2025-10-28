#include <gtest/gtest.h>

#include "ssengine/sdrandom.h"

using namespace SSCP;

TEST(sdrandom, c_api_range) {
    SDSetSeed(1);
    UINT32 value = SDRandom(10);
    EXPECT_LT(value, 10u);
}

TEST(sdrandom, class_api_deterministic) {
    CSDRandom rng;
    rng.SetSeed(123u);
    UINT32 first = rng.Random();
    CSDRandom rng2;
    rng2.SetSeed(123u);
    EXPECT_EQ(first, rng2.Random());
    EXPECT_LT(rng.Random(5u), 5u);
    EXPECT_GE(rng.Random(5u, 10u), 5u);
    EXPECT_LT(rng.Random(5u, 10u), 10u);
}
