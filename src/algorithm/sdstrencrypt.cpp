#include "ssengine/algorithm/sdstrencrypt.h"

#include <algorithm>

namespace SSCP {

namespace {

template <typename It>
void process(It begin, It end, bool encode) {
    for (auto it = begin; it != end; ++it) {
        BYTE val = static_cast<BYTE>(*it);
        BYTE high = static_cast<BYTE>((val & 0xF0u) >> 4u);
        BYTE low = static_cast<BYTE>(val & 0x0Fu);
        BYTE mixed = encode ? static_cast<BYTE>((low << 4u) | high) : static_cast<BYTE>((low << 4u) | high);
        *it = static_cast<typename std::iterator_traits<It>::value_type>(mixed ^ CSDStrEncrypt::CODE_KEY);
    }
}

} // namespace

void SSAPI CSDStrEncrypt::Encode(std::string& sContent) {
    Encode(reinterpret_cast<BYTE*>(sContent.data()), static_cast<INT32>(sContent.size()));
}

void SSAPI CSDStrEncrypt::Encode(BYTE* pBuf, INT32 nLen) {
    if (!pBuf || nLen <= 0) {
        return;
    }
    process(pBuf, pBuf + nLen, true);
}

void SSAPI CSDStrEncrypt::Decode(std::string& sContent) {
    Decode(reinterpret_cast<BYTE*>(sContent.data()), static_cast<INT32>(sContent.size()));
}

void SSAPI CSDStrEncrypt::Decode(BYTE* pBuf, INT32 nLen) {
    if (!pBuf || nLen <= 0) {
        return;
    }
    process(pBuf, pBuf + nLen, false);
}

} // namespace SSCP

