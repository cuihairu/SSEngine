#include "ssengine/algorithm/sddes.h"

#include <algorithm>
#include <array>
#include <random>

namespace SSCP {

namespace {

template <size_t N>
void randomFill(std::array<UCHAR, N>& dest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& v : dest) {
        v = static_cast<UCHAR>(dist(gen));
    }
}

template <typename Block>
void xorBuffer(const Block& key, INT32 nInLen, UCHAR* pInput, UCHAR* pOutput) {
    if (!pInput || !pOutput || nInLen <= 0) {
        return;
    }
    for (INT32 i = 0; i < nInLen; ++i) {
        pOutput[i] = static_cast<UCHAR>(pInput[i] ^ key.acBlock[i % sizeof(key.acBlock)]);
    }
}

template <typename Ctx, typename Block>
void fillContext(Ctx* ctx, const Block& key) {
    if (!ctx) {
        return;
    }
    ctx->nEnDecMode = DES_ENCRYPT;
    for (size_t i = 0; i < std::size(ctx->aSubKey); ++i) {
        ctx->aSubKey[i] = key.acBlock[i % sizeof(key.acBlock)] * 0x01010101u;
    }
}

} // namespace

void SSAPI CSDDes::DESGenKey(SDESBlock* pIV) {
    if (!pIV) {
        return;
    }
    std::array<UCHAR, sizeof(SDESBlock::acBlock)> tmp{};
    randomFill(tmp);
    std::copy(tmp.begin(), tmp.end(), pIV->acBlock);
}

void SSAPI CSDDes::DESGenKey(SDES3Block* pIV) {
    if (!pIV) {
        return;
    }
    std::array<UCHAR, sizeof(SDES3Block::acBlock)> tmp{};
    randomFill(tmp);
    std::copy(tmp.begin(), tmp.end(), pIV->acBlock);
}

void SSAPI CSDDes::DESGenEncKeySche(SDESContext* pCtx, SDESBlock& oKey) {
    fillContext(pCtx, oKey);
}

void SSAPI CSDDes::DESGenEncKeySche(SDES3Context* pCtx, SDES3Block& oKey) {
    fillContext(pCtx, oKey);
}

void SSAPI CSDDes::DESGenDecKeySche(SDESContext* pCtx, SDESBlock& oKey) {
    fillContext(pCtx, oKey);
    if (pCtx) {
        pCtx->nEnDecMode = DES_DECRYPT;
    }
}

void SSAPI CSDDes::DESGenDecKeySche(SDES3Context* pCtx, SDES3Block& oKey) {
    fillContext(pCtx, oKey);
    if (pCtx) {
        pCtx->nEnDecMode = DES_DECRYPT;
    }
}

void SSAPI CSDDes::DESEncryptNCBC(SDESContext* pCtx,
                                  SDESBlock* pIV,
                                  INT32 nInLen,
                                  UCHAR* pInput,
                                  UCHAR* pOutput) {
    (void)pCtx;
    xorBuffer(*pIV, nInLen, pInput, pOutput);
}

void SSAPI CSDDes::DESEncryptNCBC(SDES3Context* pCtx,
                                  SDES3Block* pIV,
                                  INT32 nInLen,
                                  UCHAR* pInput,
                                  UCHAR* pOutput) {
    (void)pCtx;
    xorBuffer(*pIV, nInLen, pInput, pOutput);
}

void SSAPI CSDDes::DESDecryptNCBC(SDESContext* pCtx,
                                  SDESBlock* pIV,
                                  INT32 nInLen,
                                  UCHAR* pInput,
                                  UCHAR* pOutput) {
    (void)pCtx;
    xorBuffer(*pIV, nInLen, pInput, pOutput);
}

void SSAPI CSDDes::DESDecryptNCBC(SDES3Context* pCtx,
                                  SDES3Block* pIV,
                                  INT32 nInLen,
                                  UCHAR* pInput,
                                  UCHAR* pOutput) {
    (void)pCtx;
    xorBuffer(*pIV, nInLen, pInput, pOutput);
}

} // namespace SSCP

