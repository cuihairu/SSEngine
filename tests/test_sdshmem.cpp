#include <gtest/gtest.h>
#include "ssengine/sdshmem.h"
#include <cstring>
#include <filesystem>

using namespace SSCP;

TEST(sdshmem, create_and_reopen) {
    auto path = (std::filesystem::temp_directory_path() / "sdu_shmem.bin").string();
    constexpr UINT32 kSize = 64;

    auto shm = SDCreateShmem(path.c_str(), kSize);
    ASSERT_NE(shm.mem, nullptr);
    ASSERT_TRUE(shm.handle != SDINVALID_HANDLE);

    const char* message = "shared-memory";
    std::memcpy(shm.mem, message, std::strlen(message) + 1);
    SDCloseShmem(&shm);

    auto reopened = SDOpenShmem(path.c_str(), FALSE, 0);
    ASSERT_NE(reopened.mem, nullptr);
    EXPECT_STREQ(static_cast<const char*>(reopened.mem), message);
    SDCloseShmem(&reopened);

    std::filesystem::remove(path);
}

TEST(sdshmem, existence_check) {
    auto path = (std::filesystem::temp_directory_path() / "sdu_shmem_exists.bin").string();
    std::filesystem::remove(path);
    EXPECT_FALSE(SDShmemExist(path.c_str()));
    auto shm = SDCreateShmem(path.c_str(), 32);
    ASSERT_NE(shm.mem, nullptr);
    EXPECT_TRUE(SDShmemExist(path.c_str()));
    SDCloseShmem(&shm);
    std::filesystem::remove(path);
}
