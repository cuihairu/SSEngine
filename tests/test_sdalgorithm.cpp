#include <gtest/gtest.h>

#include "ssengine/algorithm/sdcrc.h"
#include "ssengine/algorithm/sddes.h"
#include "ssengine/algorithm/sdmd5.h"
#include "ssengine/algorithm/sdstrencrypt.h"

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

using namespace SSCP;

TEST(sdcrc, buffer_crc32_matches_known_value) {
    const std::string data = "hello";
    UINT32 crc = SDCRC(data.data(), static_cast<UINT32>(data.size()));
    // Reference CRC32 for the ASCII string "hello"
    EXPECT_EQ(0x3610A686u, crc);
}

TEST(sdcrc, file_crc_matches_buffer_crc) {
    const std::string data = "crc-on-file";
    auto tempPath = std::filesystem::temp_directory_path() / "sse_crc_test.bin";
    {
        std::ofstream ofs(tempPath, std::ios::binary);
        ASSERT_TRUE(ofs.good());
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    }
    UINT32 fileCrc = SDFileCRC(tempPath.string().c_str());
    UINT32 bufferCrc = SDCRC(data.data(), static_cast<UINT32>(data.size()));
    EXPECT_EQ(bufferCrc, fileCrc);
    std::filesystem::remove(tempPath);
}

TEST(sdmd5, chunked_vs_single_digest) {
    const std::string data = "The quick brown fox jumps over the lazy dog.";
    UCHAR digestSingle[16]{};
    SDMD5(digestSingle,
          reinterpret_cast<UCHAR*>(const_cast<char*>(data.data())),
          static_cast<UINT32>(data.size()));

    SMD5Context ctx{};
    SDMD5Init(&ctx);
    auto half = data.size() / 2;
    SDMD5Update(&ctx,
                reinterpret_cast<UCHAR*>(const_cast<char*>(data.data())),
                static_cast<UINT32>(half));
    SDMD5Update(&ctx,
                reinterpret_cast<UCHAR*>(const_cast<char*>(data.data() + half)),
                static_cast<UINT32>(data.size() - half));
    UCHAR digestChunked[16]{};
    SDMD5Final(digestChunked, &ctx);

    EXPECT_EQ(0, std::memcmp(digestSingle, digestChunked, sizeof(digestSingle)));
}

TEST(sdmd5, file_digest_consistency) {
    const std::string data = "file-md5-check";
    auto tempPath = std::filesystem::temp_directory_path() / "sse_md5_test.bin";
    {
        std::ofstream ofs(tempPath, std::ios::binary);
        ASSERT_TRUE(ofs.good());
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    }

    UCHAR digestFile[16]{};
    ASSERT_TRUE(SDFileMD5(digestFile, tempPath.string().c_str()));

    UCHAR digestBuffer[16]{};
    SDMD5(digestBuffer,
          reinterpret_cast<UCHAR*>(const_cast<char*>(data.data())),
          static_cast<UINT32>(data.size()));

    EXPECT_EQ(0, std::memcmp(digestFile, digestBuffer, sizeof(digestFile)));
    std::filesystem::remove(tempPath);
}

TEST(sddes, encrypt_decrypt_roundtrip) {
    CSDDes des;
    SDESBlock key{};
    des.DESGenKey(&key);

    SDESContext ctxEnc{};
    des.DESGenEncKeySche(&ctxEnc, key);
    SDESContext ctxDec{};
    des.DESGenDecKeySche(&ctxDec, key);

    std::array<UCHAR, 16> plain{};
    for (size_t i = 0; i < plain.size(); ++i) {
        plain[i] = static_cast<UCHAR>(i);
    }
    std::array<UCHAR, 16> encrypted{};
    des.DESEncryptNCBC(&ctxEnc, &key, static_cast<INT32>(plain.size()),
                       plain.data(), encrypted.data());
    std::array<UCHAR, 16> decrypted{};
    des.DESDecryptNCBC(&ctxDec, &key, static_cast<INT32>(encrypted.size()),
                       encrypted.data(), decrypted.data());

    EXPECT_EQ(plain, decrypted);
}

TEST(sdstrencrypt, encode_decode_roundtrip) {
    std::string text = "string-encryption";
    std::string encoded = text;
    CSDStrEncrypt::Encode(encoded);
    EXPECT_NE(text, encoded);
    CSDStrEncrypt::Decode(encoded);
    EXPECT_EQ(text, encoded);
}

