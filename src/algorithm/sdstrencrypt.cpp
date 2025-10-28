#include "ssengine/algorithm/sdstrencrypt.h"

#include <algorithm>

namespace SSCP {

namespace {

// Rotate nibbles and xor with key for encode; inverse for decode.
inline BYTE encode_byte(BYTE v) {
    BYTE swapped = static_cast<BYTE>(((v & 0x0F) << 4) | ((v & 0xF0) >> 4));
    return static_cast<BYTE>(swapped ^ CSDStrEncrypt::CODE_KEY);
}
inline BYTE decode_byte(BYTE v) {
    BYTE unx = static_cast<BYTE>(v ^ CSDStrEncrypt::CODE_KEY);
    return static_cast<BYTE>(((unx & 0x0F) << 4) | ((unx & 0xF0) >> 4));
}

template <typename It>
void process_encode(It begin, It end) {
    for (auto it = begin; it != end; ++it) {
        BYTE val = static_cast<BYTE>(*it);
        *it = static_cast<typename std::iterator_traits<It>::value_type>(encode_byte(val));
    }
}

template <typename It>
void process_decode(It begin, It end) {
    for (auto it = begin; it != end; ++it) {
        BYTE val = static_cast<BYTE>(*it);
        *it = static_cast<typename std::iterator_traits<It>::value_type>(decode_byte(val));
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
    process_encode(pBuf, pBuf + nLen);
}

void SSAPI CSDStrEncrypt::Decode(std::string& sContent) {
    Decode(reinterpret_cast<BYTE*>(sContent.data()), static_cast<INT32>(sContent.size()));
}

void SSAPI CSDStrEncrypt::Decode(BYTE* pBuf, INT32 nLen) {
    if (!pBuf || nLen <= 0) {
        return;
    }
    process_decode(pBuf, pBuf + nLen);
}

} // namespace SSCP

