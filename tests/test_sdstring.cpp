#include <gtest/gtest.h>

#include "ssengine/sdstring.h"

using namespace SSCP;

TEST(sdstring, to_lower_upper) {
    EXPECT_EQ(ToLower("AbC"), "abc");
    EXPECT_EQ(ToUpper("AbC"), "ABC");
}

TEST(sdstring, split_basic) {
    auto parts = Split("a,b,c", ',');
    ASSERT_EQ(parts.size(), 3u);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
}
