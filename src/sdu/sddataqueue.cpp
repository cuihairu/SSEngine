#include "ssengine/sddataqueue.h"

#include <cstring>

namespace SSCP {

CSDDataQueue::CSDDataQueue(void)
    : m_pArray(nullptr)
    , m_nHead(0)
    , m_nTail(0)
    , m_nSize(0) {}

CSDDataQueue::~CSDDataQueue(void) {
    delete[] m_pArray;
    m_pArray = nullptr;
    m_nHead = m_nTail = 0;
    m_nSize = 0;
}

BOOL SSAPI CSDDataQueue::Init(INT32 nSize) {
    delete[] m_pArray;
    if (nSize <= 0) {
        m_pArray = nullptr;
        m_nHead = m_nTail = 0;
        m_nSize = 0;
        return FALSE;
    }
    m_nSize = nSize + 1;
    m_pArray = new void*[m_nSize];
    std::memset(m_pArray, 0, sizeof(void*) * m_nSize);
    m_nHead = m_nTail = 0;
    return TRUE;
}

BOOL SSAPI CSDDataQueue::PushBack(void* ptr) {
    if (!m_pArray || !ptr) {
        return FALSE;
    }
    INT32 nextTail = (m_nTail + 1) % m_nSize;
    if (nextTail == m_nHead) {
        return FALSE;
    }
    m_pArray[m_nTail] = ptr;
    m_nTail = nextTail;
    return TRUE;
}

void* SSAPI CSDDataQueue::PopFront() {
    if (!m_pArray || m_nHead == m_nTail) {
        return nullptr;
    }
    void* ptr = m_pArray[m_nHead];
    m_pArray[m_nHead] = nullptr;
    m_nHead = (m_nHead + 1) % m_nSize;
    return ptr;
}

} // namespace SSCP
