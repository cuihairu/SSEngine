#include <gtest/gtest.h>
#include "ssengine/sdfile.h"
#include <string>

using namespace SSCP;

TEST(sdfile, create_write_read_remove) {
    const char* path = "test_sse_file.tmp";
    // ensure remove
    SDFileRemove(path);

    ASSERT_TRUE(SDFileCreate(path));
    ASSERT_TRUE(SDFileExists(path));

    CSDFile f;
    ASSERT_TRUE(f.Open(path, "wb+", FALSE));
    const char* msg = "hello";
    EXPECT_EQ(f.Write(msg, 5), 5u);
    f.Flush();
    EXPECT_EQ(f.SeekToBegin(), 0);
    char buf[8] = {0};
    EXPECT_EQ(f.Read(buf, 8), 5u);
    EXPECT_STREQ(buf, msg);
    f.Close();

    EXPECT_TRUE(SDFileRemove(path));
}

TEST(sdfile, path_helpers) {
    std::string p = SDFileIncludeTrailingDelimiter("/a/b");
#if defined(_WIN32)
    EXPECT_EQ(p.back(), '\\');
#else
    EXPECT_EQ(p.back(), '/');
#endif
    auto name = SDFileExtractName("/a/b/c.txt");
    EXPECT_EQ(name, "c.txt");
    auto ext = SDFileExtractExt("/a/b/c.txt");
    EXPECT_EQ(ext, ".txt");
}
