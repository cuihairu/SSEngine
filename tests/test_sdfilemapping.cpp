#include <gtest/gtest.h>
#include "ssengine/sdfile.h"
#include "ssengine/sdfilemapping.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

using namespace SSCP;

namespace {
std::string WriteTempFile(const std::string& name, const std::string& content) {
    auto dir = std::filesystem::temp_directory_path();
    auto path = dir / name;
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.close();
    return path.string();
}
}

TEST(sdfilemapping, map_and_modify) {
    const std::string original = "hello-mapping";
    auto path = WriteTempFile("sdu_mapping.dat", original);

    SFileMapping mapping{};
    ASSERT_TRUE(SDFileMapping(mapping, path.c_str()));
    ASSERT_NE(mapping.pos, nullptr);
    ASSERT_EQ(mapping.size, original.size());

    std::string mapped(static_cast<const char*>(mapping.pos), mapping.size);
    EXPECT_EQ(mapped, original);

    std::string replacement = "HELLO";
    std::memcpy(mapping.pos, replacement.data(), replacement.size());
    SDFileFlushMapping(mapping.pos, replacement.size());
    SDFileUnMapping(mapping);

    {
        std::ifstream ifs(path, std::ios::binary);
        std::string final((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        EXPECT_EQ(final.substr(0, replacement.size()), replacement);
    }
    std::filesystem::remove(path);
}
