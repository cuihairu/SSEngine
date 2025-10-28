#include <gtest/gtest.h>

#include "ssengine/sdloopbuffer.h"

using namespace SSCP;

TEST(sdloopbuffer, push_pop_roundtrip) {
    CSDLoopBuffer buf;
    ASSERT_TRUE(buf.Init(16));
    const char data[] = "abcd";
    EXPECT_TRUE(buf.PushBack(data, 4));
    char out[5] = {};
    EXPECT_TRUE(buf.PopFront(out, 4));
    out[4] = '\0';
    EXPECT_STREQ(out, "abcd");
}

TEST(sdloopbuffer, discard) {
    CSDLoopBuffer buf;
    ASSERT_TRUE(buf.Init(8));
    const char data[] = "abcdefgh";
    EXPECT_TRUE(buf.PushBack(data, 8));
    buf.DiscardFront(3);
    char out[6] = {};
    EXPECT_TRUE(buf.PopFront(out, 5));
    out[5] = '\0';
    EXPECT_STREQ(out, "defgh");
}
