#include "ssengine/sddatastream.h"

#include <algorithm>
#include <cstring>

namespace SSCP {

namespace {

template <typename T>
INT32 writeScalar(CHAR* buf, INT32 bufLen, INT32& pos, T value) {
    if (!buf || bufLen <= 0) {
        return -1;
    }
    if (pos + static_cast<INT32>(sizeof(T)) > bufLen) {
        return -1;
    }
    INT32 offset = pos;
    std::memcpy(buf + pos, &value, sizeof(T));
    pos += static_cast<INT32>(sizeof(T));
    return offset;
}

template <typename T>
INT32 readScalar(const CHAR* buf, INT32 dataLen, INT32& readPos, T& out) {
    if (!buf) {
        return -1;
    }
    if (readPos + static_cast<INT32>(sizeof(T)) > dataLen) {
        return -1;
    }
    std::memcpy(&out, buf + readPos, sizeof(T));
    INT32 offset = readPos;
    readPos += static_cast<INT32>(sizeof(T));
    return offset;
}

} // namespace

CSDDataStream::CSDDataStream()
    : m_pBuf(nullptr)
    , m_bufLen(0)
    , m_pos(0)
    , m_readPos(0) {}

CSDDataStream::~CSDDataStream() = default;

void SSAPI CSDDataStream::Prepare(CHAR* pszBuf, INT32 dwBufLen) {
    m_pBuf = pszBuf;
    m_bufLen = dwBufLen;
    m_pos = 0;
    m_readPos = 0;
}

void SSAPI CSDDataStream::Reset() {
    m_pos = 0;
    m_readPos = 0;
}

INT32 SSAPI CSDDataStream::AddByte(UCHAR byValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, byValue);
}

INT32 SSAPI CSDDataStream::DelByte(UCHAR& byValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, byValue);
}

INT32 SSAPI CSDDataStream::AddChar(CHAR chChar) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, chChar);
}

INT32 SSAPI CSDDataStream::DelChar(CHAR& chChar) {
    return readScalar(m_pBuf, m_pos, m_readPos, chChar);
}

INT32 SSAPI CSDDataStream::AddWord(UINT16 wValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, wValue);
}

INT32 SSAPI CSDDataStream::DelWord(UINT16& wValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, wValue);
}

INT32 SSAPI CSDDataStream::AddShort(INT16 sValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, sValue);
}

INT32 SSAPI CSDDataStream::DelShort(INT16& sValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, sValue);
}

INT32 SSAPI CSDDataStream::AddDword(UINT32 dwValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, dwValue);
}

INT32 SSAPI CSDDataStream::DelDword(UINT32& dwValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, dwValue);
}

INT32 SSAPI CSDDataStream::AddInt(INT32 iValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, iValue);
}

INT32 SSAPI CSDDataStream::DelInt(INT32& iValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, iValue);
}

INT32 SSAPI CSDDataStream::AddUint64(UINT64 uullValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, uullValue);
}

INT32 SSAPI CSDDataStream::DelUint64(UINT64& uullValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, uullValue);
}

INT32 SSAPI CSDDataStream::AddInt64(INT64 llValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, llValue);
}

INT32 SSAPI CSDDataStream::DelInt64(INT64& llValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, llValue);
}

INT32 SSAPI CSDDataStream::AddFloat(FLOAT fValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, fValue);
}

INT32 SSAPI CSDDataStream::DelFloat(FLOAT& fValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, fValue);
}

INT32 SSAPI CSDDataStream::AddDouble(DOUBLE dbValue) {
    return writeScalar(m_pBuf, m_bufLen, m_pos, dbValue);
}

INT32 SSAPI CSDDataStream::DelDouble(DOUBLE& dbValue) {
    return readScalar(m_pBuf, m_pos, m_readPos, dbValue);
}

INT32 SSAPI CSDDataStream::AddString(const CHAR* pszStr, INT32 dwBufLen) {
    if (!pszStr || dwBufLen <= 0) {
        return -1;
    }
    INT32 actualLen = std::min(dwBufLen, static_cast<INT32>(std::strlen(pszStr)));
    INT32 offset = AddDword(static_cast<UINT32>(actualLen));
    if (offset < 0) {
        return -1;
    }
    if (m_pos + actualLen > m_bufLen) {
        m_pos = offset;
        return -1;
    }
    std::memcpy(m_pBuf + m_pos, pszStr, actualLen);
    m_pos += actualLen;
    return offset;
}

INT32 SSAPI CSDDataStream::DelString(CHAR* pszOut, INT32 dwBufLen) {
    if (!pszOut || dwBufLen <= 0) {
        return -1;
    }
    UINT32 length = 0;
    INT32 offset = DelDword(length);
    if (offset < 0) {
        return -1;
    }
    if (m_readPos + static_cast<INT32>(length) > m_pos) {
        return -1;
    }
    INT32 copyLen = std::min(dwBufLen - 1, static_cast<INT32>(length));
    std::memcpy(pszOut, m_pBuf + m_readPos, copyLen);
    pszOut[copyLen] = '\0';
    m_readPos += static_cast<INT32>(length);
    return offset;
}

INT32 SSAPI CSDDataStream::AddBuf(const UCHAR* pszBuf, INT32 dwBufLen) {
    if (!pszBuf || dwBufLen <= 0) {
        return -1;
    }
    INT32 offset = AddDword(static_cast<UINT32>(dwBufLen));
    if (offset < 0) {
        return -1;
    }
    if (m_pos + dwBufLen > m_bufLen) {
        m_pos = offset;
        return -1;
    }
    std::memcpy(m_pBuf + m_pos, pszBuf, dwBufLen);
    m_pos += dwBufLen;
    return offset;
}

INT32 SSAPI CSDDataStream::DelBuf(UCHAR* pszBuf, INT32 dwBufLen) {
    if (!pszBuf || dwBufLen <= 0) {
        return -1;
    }
    UINT32 storedLen = 0;
    INT32 offset = DelDword(storedLen);
    if (offset < 0) {
        return -1;
    }
    if (m_readPos + static_cast<INT32>(storedLen) > m_pos) {
        return -1;
    }
    INT32 copyLen = std::min(dwBufLen, static_cast<INT32>(storedLen));
    std::memcpy(pszBuf, m_pBuf + m_readPos, copyLen);
    m_readPos += static_cast<INT32>(storedLen);
    return offset;
}

} // namespace SSCP

