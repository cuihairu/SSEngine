#include "ssengine/sdloopbuffer.h"

#include <algorithm>
#include <cstring>

namespace SSCP {

namespace {
inline INT32 used(const CHAR* head, const CHAR* tail, INT32 size) {
    if (tail >= head) {
        return static_cast<INT32>(tail - head);
    }
    return size - static_cast<INT32>(head - tail);
}

inline INT32 freeSpace(const CHAR* head, const CHAR* tail, INT32 size) {
    return size - used(head, tail, size) - 1;
}
}

CSDLoopBuffer::CSDLoopBuffer(void)
    : m_pBuffer(nullptr)
    , m_pHead(nullptr)
    , m_pTail(nullptr)
    , m_nSize(0) {}

CSDLoopBuffer::~CSDLoopBuffer(void) {
    delete[] m_pBuffer;
    m_pBuffer = nullptr;
    m_pHead = m_pTail = nullptr;
    m_nSize = 0;
}

BOOL SSAPI CSDLoopBuffer::Init(INT32 nSize) {
    delete[] m_pBuffer;
    if (nSize <= 0) {
        m_pBuffer = nullptr;
        m_pHead = m_pTail = nullptr;
        m_nSize = 0;
        return FALSE;
    }
    m_nSize = nSize + 1; // reserve one slot to distinguish full/empty
    m_pBuffer = new CHAR[m_nSize];
    m_pHead = m_pBuffer;
    m_pTail = m_pBuffer;
    return TRUE;
}

BOOL SSAPI CSDLoopBuffer::PushBack(const CHAR* pData, INT32 nLen) {
    if (!m_pBuffer || !pData || nLen <= 0) {
        return FALSE;
    }
    if (freeSpace(m_pHead, m_pTail, m_nSize) < nLen) {
        return FALSE;
    }
    INT32 firstChunk = std::min(nLen, static_cast<INT32>(m_pBuffer + m_nSize - m_pTail));
    std::memcpy(m_pTail, pData, firstChunk);
    m_pTail += firstChunk;
    if (m_pTail == m_pBuffer + m_nSize) {
        m_pTail = m_pBuffer;
    }
    INT32 remaining = nLen - firstChunk;
    if (remaining > 0) {
        std::memcpy(m_pTail, pData + firstChunk, remaining);
        m_pTail += remaining;
        if (m_pTail >= m_pBuffer + m_nSize) {
            m_pTail = m_pBuffer + (m_pTail - (m_pBuffer + m_nSize));
        }
    }
    return TRUE;
}

BOOL SSAPI CSDLoopBuffer::PopFront(CHAR* pBuf, INT32 nLen) {
    if (!m_pBuffer || !pBuf || nLen <= 0) {
        return FALSE;
    }
    if (used(m_pHead, m_pTail, m_nSize) < nLen) {
        return FALSE;
    }
    INT32 firstChunk = std::min(nLen, static_cast<INT32>(m_pBuffer + m_nSize - m_pHead));
    std::memcpy(pBuf, m_pHead, firstChunk);
    m_pHead += firstChunk;
    if (m_pHead == m_pBuffer + m_nSize) {
        m_pHead = m_pBuffer;
    }
    INT32 remaining = nLen - firstChunk;
    if (remaining > 0) {
        std::memcpy(pBuf + firstChunk, m_pHead, remaining);
        m_pHead += remaining;
        if (m_pHead >= m_pBuffer + m_nSize) {
            m_pHead = m_pBuffer + (m_pHead - (m_pBuffer + m_nSize));
        }
    }
    return TRUE;
}

void SSAPI CSDLoopBuffer::DiscardFront(INT32 nLen) {
    if (!m_pBuffer || nLen <= 0) {
        return;
    }
    INT32 available = used(m_pHead, m_pTail, m_nSize);
    nLen = std::min(nLen, available);
    INT32 firstChunk = std::min(nLen, static_cast<INT32>(m_pBuffer + m_nSize - m_pHead));
    m_pHead += firstChunk;
    if (m_pHead == m_pBuffer + m_nSize) {
        m_pHead = m_pBuffer;
    }
    INT32 remaining = nLen - firstChunk;
    if (remaining > 0) {
        m_pHead += remaining;
        if (m_pHead >= m_pBuffer + m_nSize) {
            m_pHead = m_pBuffer + (m_pHead - (m_pBuffer + m_nSize));
        }
    }
    if (m_pHead == m_pTail) {
        m_pHead = m_pTail = m_pBuffer;
    }
}

} // namespace SSCP
