#include "ssengine/algorithm/sdcrc.h"

#include <array>
#include <fstream>

namespace SSCP {

namespace {

constexpr UINT32 CRC32_POLY = 0xEDB88320u;

std::array<UINT32, 256> buildTable() {
    std::array<UINT32, 256> table{};
    for (UINT32 i = 0; i < table.size(); ++i) {
        UINT32 c = i;
        for (int j = 0; j < 8; ++j) {
            if (c & 1U) {
                c = CRC32_POLY ^ (c >> 1U);
            } else {
                c >>= 1U;
            }
        }
        table[i] = c;
    }
    return table;
}

const std::array<UINT32, 256>& crcTable() {
    static const std::array<UINT32, 256> table = buildTable();
    return table;
}

} // namespace

UINT32 SSAPI SDCRC(const void* pBuf, UINT32 dwLen, UINT32 dwCrcVal) {
    if (!pBuf || dwLen == 0) {
        return dwCrcVal;
    }
    const auto* data = static_cast<const unsigned char*>(pBuf);
    UINT32 crc = dwCrcVal ^ 0xFFFFFFFFu;
    const auto& table = crcTable();
    for (UINT32 i = 0; i < dwLen; ++i) {
        crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8U);
    }
    return crc ^ 0xFFFFFFFFu;
}

UINT32 SSAPI SDFileCRC(const CHAR* pszFileName, BOOL /*bFileMapping*/) {
    if (!pszFileName) {
        return 0;
    }
    std::ifstream ifs(pszFileName, std::ios::binary);
    if (!ifs) {
        return 0;
    }
    UINT32 crc = 0;
    std::array<unsigned char, 4096> buffer{};
    while (ifs) {
        ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize got = ifs.gcount();
        if (got > 0) {
            crc = SDCRC(buffer.data(), static_cast<UINT32>(got), crc);
        }
    }
    return crc;
}

} // namespace SSCP

