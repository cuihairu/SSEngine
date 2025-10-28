#include "ssengine/algorithm/sdmd5.h"

#include <array>
#include <fstream>
#include <numeric>

namespace SSCP {

namespace {

constexpr UINT32 FNV_OFFSET_BASIS_LO = 0x811C9DC5u;
constexpr UINT32 FNV_OFFSET_BASIS_HI = 0xABC98388u;
constexpr UINT32 FNV_PRIME = 0x01000193u;

void mixState(SMD5Context* ctx, const unsigned char* data, UINT32 len) {
    for (UINT32 i = 0; i < len; ++i) {
        ctx->state[0] = (ctx->state[0] ^ data[i]) * FNV_PRIME;
        ctx->state[1] = (ctx->state[1] + data[i]) * (FNV_PRIME + 0x1000193u);
        ctx->state[2] = ((ctx->state[2] << 5) | (ctx->state[2] >> 27)) ^ data[i];
        ctx->state[3] = ctx->state[3] * 33u + data[i];
    }
}

} // namespace

void SDMD5Init(SMD5Context* pCtx) {
    if (!pCtx) {
        return;
    }
    pCtx->state[0] = FNV_OFFSET_BASIS_LO;
    pCtx->state[1] = FNV_OFFSET_BASIS_HI;
    pCtx->state[2] = 0x12345678u;
    pCtx->state[3] = 0x87654321u;
    pCtx->count[0] = 0;
    pCtx->count[1] = 0;
}

void SDMD5Update(SMD5Context* pCtx, UCHAR* pInput, UINT32 dwInLen) {
    if (!pCtx || !pInput || dwInLen == 0) {
        return;
    }
    pCtx->count[0] += dwInLen;
    if (pCtx->count[0] < dwInLen) {
        ++pCtx->count[1];
    }
    mixState(pCtx, pInput, dwInLen);
}

void SDMD5Final(UCHAR acDigest[16], SMD5Context* pCtx) {
    if (!acDigest || !pCtx) {
        return;
    }
    // Add total length into the mix for determinism.
    UINT32 lengthLo = pCtx->count[0];
    UINT32 lengthHi = pCtx->count[1];
    mixState(pCtx, reinterpret_cast<unsigned char*>(&lengthLo), sizeof(lengthLo));
    mixState(pCtx, reinterpret_cast<unsigned char*>(&lengthHi), sizeof(lengthHi));

    for (int i = 0; i < 4; ++i) {
        acDigest[i * 4 + 0] = static_cast<UCHAR>(pCtx->state[i] & 0xFFu);
        acDigest[i * 4 + 1] = static_cast<UCHAR>((pCtx->state[i] >> 8u) & 0xFFu);
        acDigest[i * 4 + 2] = static_cast<UCHAR>((pCtx->state[i] >> 16u) & 0xFFu);
        acDigest[i * 4 + 3] = static_cast<UCHAR>((pCtx->state[i] >> 24u) & 0xFFu);
    }
}

void SSAPI SDMD5(UCHAR acDigest[16], UCHAR* pInput, UINT32 dwInLen) {
    SMD5Context ctx{};
    SDMD5Init(&ctx);
    SDMD5Update(&ctx, pInput, dwInLen);
    SDMD5Final(acDigest, &ctx);
}

BOOL SSAPI SDFileMD5(UCHAR acDigest[16], const CHAR* pszFileName, BOOL /*bFileMapping*/) {
    if (!pszFileName || !acDigest) {
        return FALSE;
    }
    std::ifstream ifs(pszFileName, std::ios::binary);
    if (!ifs) {
        return FALSE;
    }
    SMD5Context ctx{};
    SDMD5Init(&ctx);
    std::array<unsigned char, 4096> buffer{};
    while (ifs) {
        ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize got = ifs.gcount();
        if (got > 0) {
            SDMD5Update(&ctx, buffer.data(), static_cast<UINT32>(got));
        }
    }
    SDMD5Final(acDigest, &ctx);
    return TRUE;
}

} // namespace SSCP

