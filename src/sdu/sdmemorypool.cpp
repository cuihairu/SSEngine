/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdmemorypool.cpp - 内存池实现
******************************************************************************/

#include "ssengine/sdmemorypool.h"
#include <cstring>
#include <algorithm>

namespace SSCP
{

///////////////////////////////////////////////////////////////////////////////
// CSDVarMemoryPool - 可变内存池
///////////////////////////////////////////////////////////////////////////////

CSDVarMemoryPool::CSDVarMemoryPool()
    : m_pHeadPage(nullptr)
    , m_pWorkPage(nullptr)
    , m_pPageBuf(nullptr)
    , m_nPageSize(0)
{
    memset(m_pFreeHead, 0, sizeof(m_pFreeHead));
    memset(m_nFreeCount, 0, sizeof(m_nFreeCount));
}

CSDVarMemoryPool::~CSDVarMemoryPool()
{
    Clear();
}

BOOL CSDVarMemoryPool::Create(UINT32 dwPageSize)
{
    if (dwPageSize < MIN_PAGESIZE)
        dwPageSize = MIN_PAGESIZE;
    
    // 对齐到8字节边界
    m_nPageSize = (dwPageSize + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    
    // 初始化空闲列表
    memset(m_pFreeHead, 0, sizeof(m_pFreeHead));
    memset(m_nFreeCount, 0, sizeof(m_nFreeCount));
    
    m_pHeadPage = nullptr;
    m_pWorkPage = nullptr;
    m_pPageBuf = nullptr;
    
    return SetMemoryPage();
}

void* CSDVarMemoryPool::Malloc(UINT32 dwLen)
{
    if (dwLen == 0)
        return nullptr;

    // align to 8-byte boundary for payload size
    dwLen = (dwLen + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

    // Large allocations bypass pool; prepend size header for Free()
    if (dwLen > MAX_UNIT_SIZE)
    {
        BYTE* p = (BYTE*)malloc(dwLen + sizeof(UINT32));
        if (!p) return nullptr;
        *(UINT32*)p = dwLen;
        return p + sizeof(UINT32);
    }

    // Which free-list bucket
    INT32 dwIndex = (dwLen - 1) / ALIGNMENT;
    if (dwIndex >= UNIT_TYPE_COUNT) dwIndex = UNIT_TYPE_COUNT - 1;

    // Try fast path from free list
    if (m_pFreeHead[dwIndex])
    {
        BYTE* pBlock = m_pFreeHead[dwIndex]; // block base (header/next area)
        m_pFreeHead[dwIndex] = *(BYTE**)pBlock;
        m_nFreeCount[dwIndex]--;
        // Write size header and return payload pointer
        *(UINT32*)pBlock = dwLen;
        return pBlock + sizeof(UINT32);
    }

    // No free block; replenish and retry
    if (!AddFreeMemory(dwIndex))
        return nullptr;

    if (m_pFreeHead[dwIndex])
    {
        BYTE* pBlock = m_pFreeHead[dwIndex];
        m_pFreeHead[dwIndex] = *(BYTE**)pBlock;
        m_nFreeCount[dwIndex]--;
        *(UINT32*)pBlock = dwLen;
        return pBlock + sizeof(UINT32);
    }

    return nullptr;
}


void CSDVarMemoryPool::Free(void* p)
{
    if (!p) return;

    // Recover size header stored just before payload
    BYTE* pBase = (BYTE*)p - sizeof(UINT32);
    UINT32 dwLen = *(UINT32*)pBase;

    if (dwLen > MAX_UNIT_SIZE)
    {
        // Large blocks were allocated via malloc with header
        free(pBase);
        return;
    }

    // Map size back to bucket index
    INT32 dwIndex = (dwLen - 1) / ALIGNMENT;
    if (dwIndex >= UNIT_TYPE_COUNT) dwIndex = UNIT_TYPE_COUNT - 1;

    // Push block base back to free list head
    *(BYTE**)pBase = m_pFreeHead[dwIndex];
    m_pFreeHead[dwIndex] = pBase;
    m_nFreeCount[dwIndex]++;
}


void CSDVarMemoryPool::Clear()
{
    MemoryPage* pPage = m_pHeadPage;
    while (pPage)
    {
        MemoryPage* pNext = pPage->Next;
        free(pPage);
        pPage = pNext;
    }
    
    m_pHeadPage = nullptr;
    m_pWorkPage = nullptr;
    m_pPageBuf = nullptr;
    
    memset(m_pFreeHead, 0, sizeof(m_pFreeHead));
    memset(m_nFreeCount, 0, sizeof(m_nFreeCount));
}

INT32 CSDVarMemoryPool::GetMemUsed()
{
    INT32 nUsed = 0;
    MemoryPage* pPage = m_pHeadPage;
    
    while (pPage)
    {
        nUsed += m_nPageSize + sizeof(MemoryPage);
        pPage = pPage->Next;
    }
    
    return nUsed;
}

BOOL CSDVarMemoryPool::AddFreeMemory(INT32 dwIndex)
{
    // Payload size for this bucket
    UINT32 unitSize = (dwIndex + 1) * ALIGNMENT;
    // Each block reserves a 4-byte header (size) in front of payload
    UINT32 stride = unitSize + sizeof(UINT32);

    // Ensure current work page has enough contiguous space; otherwise allocate a new page
    if (!m_pWorkPage || m_pPageBuf + (size_t)stride * ALLOC_COUNT > GetPageBufEnd(m_pWorkPage))
    {
        if (!SetMemoryPage())
            return FALSE;
    }

    // Carve ALLOC_COUNT blocks from current page
    for (INT32 i = 0; i < ALLOC_COUNT; ++i)
    {
        // Link into free list; store next pointer in block base
        *(BYTE**)m_pPageBuf = m_pFreeHead[dwIndex];
        m_pFreeHead[dwIndex] = m_pPageBuf;
        m_pPageBuf += stride;
        m_nFreeCount[dwIndex]++;
    }

    return TRUE;
}


BOOL CSDVarMemoryPool::SetMemoryPage()
{
    MemoryPage* pNewPage = (MemoryPage*)malloc(sizeof(MemoryPage) + m_nPageSize);
    if (!pNewPage) return FALSE;

    pNewPage->Next = m_pHeadPage;
    m_pHeadPage = pNewPage;
    m_pWorkPage = pNewPage;
    m_pPageBuf = GetPageBufGegin(pNewPage);

    // Adjust start so that payload pointer (base + sizeof(UINT32)) is ALIGNMENT-aligned
    size_t mis = ((size_t)m_pPageBuf + sizeof(UINT32)) % ALIGNMENT;
    if (mis != 0)
        m_pPageBuf += (ALIGNMENT - mis);

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CSDFixMemoryPool - 固定内存池
///////////////////////////////////////////////////////////////////////////////

CSDFixMemoryPool::CSDFixMemoryPool()
    : m_pHeadPage(nullptr)
    , m_nUnitSize(0)
    , m_nPageSize(0)
{
}

CSDFixMemoryPool::~CSDFixMemoryPool()
{
    Clear();
}

BOOL CSDFixMemoryPool::Create(UINT32 dwUnitSize, UINT32 dwPageSize)
{
    if (dwUnitSize == 0)
        return FALSE;
    
    if (dwPageSize < MIN_PAGESIZE)
        dwPageSize = MIN_PAGESIZE;
    
    // 对齐单元大小
    m_nUnitSize = (dwUnitSize + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    m_nPageSize = dwPageSize;
    
    m_pHeadPage = nullptr;
    
    return AddMemoryPage();
}

void* CSDFixMemoryPool::Malloc()
{
    MemoryPage* pPage = m_pHeadPage;
    
    // 查找有空闲单元的页
    while (pPage && pPage->nFreecount == 0)
    {
        pPage = pPage->Next;
    }
    
    // 如果没有空闲页，添加新页
    if (!pPage)
    {
        if (!AddMemoryPage())
            return nullptr;
        pPage = m_pHeadPage;
    }
    
    // 从页中分配单元
    if (pPage->nFreecount > 0)
    {
        char* pBuf = GetPageBuf(pPage);
        INT32* pFreeIndex = (INT32*)(pBuf + pPage->nFreeHead * m_nUnitSize);
        
        void* pResult = pFreeIndex;
        pPage->nFreeHead = *pFreeIndex;
        pPage->nFreecount--;
        
        return pResult;
    }
    
    return nullptr;
}

void CSDFixMemoryPool::Free(void* p)
{
    if (!p)
        return;
    
    // 查找指针所属的页
    MemoryPage* pPage = m_pHeadPage;
    while (pPage)
    {
        char* pPageBuf = GetPageBuf(pPage);
        if (p >= pPageBuf && p < pPageBuf + m_nPageSize)
        {
            // 计算单元索引
            INT32 nIndex = ((char*)p - pPageBuf) / m_nUnitSize;
            
            // 添加到空闲链表
            *(INT32*)p = pPage->nFreeHead;
            pPage->nFreeHead = nIndex;
            pPage->nFreecount++;
            
            return;
        }
        pPage = pPage->Next;
    }
}

void CSDFixMemoryPool::Clear()
{
    MemoryPage* pPage = m_pHeadPage;
    while (pPage)
    {
        MemoryPage* pNext = pPage->Next;
        free(pPage);
        pPage = pNext;
    }
    
    m_pHeadPage = nullptr;
}

INT32 CSDFixMemoryPool::GetMemUsed()
{
    INT32 nUsed = 0;
    MemoryPage* pPage = m_pHeadPage;
    
    while (pPage)
    {
        nUsed += m_nPageSize + sizeof(MemoryPage);
        pPage = pPage->Next;
    }
    
    return nUsed;
}

BOOL CSDFixMemoryPool::AddMemoryPage()
{
    MemoryPage* pNewPage = (MemoryPage*)malloc(sizeof(MemoryPage) + m_nPageSize);
    if (!pNewPage)
        return FALSE;
    
    pNewPage->Next = m_pHeadPage;
    m_pHeadPage = pNewPage;
    
    InitPage(pNewPage);
    
    return TRUE;
}

void CSDFixMemoryPool::InitPage(MemoryPage* pPage)
{
    char* pBuf = GetPageBuf(pPage);
    INT32 nMaxUnits = m_nPageSize / m_nUnitSize;
    
    pPage->nFreeHead = 0;
    pPage->nFreecount = nMaxUnits;
    
    // 初始化空闲链表
    for (INT32 i = 0; i < nMaxUnits - 1; i++)
    {
        *(INT32*)(pBuf + i * m_nUnitSize) = i + 1;
    }
    
    // 最后一个单元指向-1表示结束
    *(INT32*)(pBuf + (nMaxUnits - 1) * m_nUnitSize) = -1;
}

} // namespace SSCP