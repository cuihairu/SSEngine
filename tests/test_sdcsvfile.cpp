#include <gtest/gtest.h>

#include "ssengine/sdcsvfile.h"

using namespace SSCP;

TEST(sdcsvfile, insert_and_read) {
    CSDCsvBase csv;
    EXPECT_EQ(csv.AddLine(), 0);
    EXPECT_EQ(csv.InsertItem(0, 0, "42"), 0);
    EXPECT_EQ(csv.InsertItem(0, 1, "hello"), 0);

    INT32 intValue = 0;
    EXPECT_EQ(csv.ReadData<int>(0, 0, intValue), 0);
    EXPECT_EQ(intValue, 42);

    std::string strValue;
    EXPECT_EQ(csv.ReadData<std::string>(0, 1, strValue), 0);
    EXPECT_EQ(strValue, "hello");
}

