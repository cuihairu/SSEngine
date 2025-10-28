#include <gtest/gtest.h>
#include "ssengine/sdregex.h"

using namespace SSCP;

TEST(sdregex, match_groups) {
    SMatchResult matches;
    ASSERT_TRUE(SDRegexMatch("hello world!", matches, "(\\w+) (\\w+)!"));
    ASSERT_EQ(matches.size(), 3u);
    EXPECT_EQ(matches[0], "hello world!");
    EXPECT_EQ(matches[1], "hello");
    EXPECT_EQ(matches[2], "world");
}

TEST(sdregex, replace_text) {
    std::string result = SDRegexReplace("This is his face", R"(\bhis\b)", "her");
    EXPECT_EQ(result, "This is her face");
}
