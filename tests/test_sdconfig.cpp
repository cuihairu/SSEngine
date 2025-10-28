#include <gtest/gtest.h>

#include "ssengine/sdconfig.h"

using namespace SSCP;

TEST(sdconfig, platform_name_nonempty) {
    EXPECT_NE(nullptr, GetPlatformName());
    EXPECT_STRNE("", GetPlatformName());
}
