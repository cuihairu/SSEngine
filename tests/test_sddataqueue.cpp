#include <gtest/gtest.h>

#include "ssengine/sddataqueue.h"

using namespace SSCP;

TEST(sddataqueue, push_and_pop) {
    CSDDataQueue queue;
    ASSERT_TRUE(queue.Init(4));
    int a = 1;
    EXPECT_TRUE(queue.PushBack(&a));
    EXPECT_EQ(queue.PopFront(), &a);
}

TEST(sddataqueue, overflow_protection) {
    CSDDataQueue queue;
    ASSERT_TRUE(queue.Init(2));
    int a = 1, b = 2, c = 3;
    EXPECT_TRUE(queue.PushBack(&a));
    EXPECT_TRUE(queue.PushBack(&b));
    EXPECT_FALSE(queue.PushBack(&c));
    EXPECT_EQ(queue.PopFront(), &a);
    EXPECT_EQ(queue.PopFront(), &b);
    EXPECT_EQ(queue.PopFront(), nullptr);
}
