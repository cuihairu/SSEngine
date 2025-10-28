#include <gtest/gtest.h>

#include <array>

#include "ssengine/sddatastream.h"

using namespace SSCP;

TEST(sddatastream, write_read_scalars) {
    std::array<char, 128> buffer{};
    CSDDataStream stream;
    stream.Prepare(buffer.data(), static_cast<INT32>(buffer.size()));

    EXPECT_GE(stream.AddInt(42), 0);
    EXPECT_GE(stream.AddFloat(3.14f), 0);
    EXPECT_GE(stream.AddString("hi", 2), 0);

    INT32 i = 0;
    FLOAT f = 0.0f;
    char str[16] = {};

    EXPECT_GE(stream.DelInt(i), 0);
    EXPECT_GE(stream.DelFloat(f), 0);
    EXPECT_GE(stream.DelString(str, sizeof(str)), 0);

    EXPECT_EQ(i, 42);
    EXPECT_FLOAT_EQ(f, 3.14f);
    EXPECT_STREQ(str, "hi");
}
