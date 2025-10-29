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
    
    // 对齐到8字节边界
    dwLen = (dwLen + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    
    // 如果请求的内存大于最大单元大小，直接分配
    if (dwLen > MAX_UNIT_SIZE)
    {
        void* p = malloc(dwLen + sizeof(UINT32));
        if (p)
        {
            *(UINT32*)p = dwLen;
            return (BYTE*)p + sizeof(UINT32);
        }
        return nullptr;
    }
    
    // 计算单元类型索引
    INT32 dwIndex = (dwLen - 1) / ALIGNMENT;
    if (dwIndex >= UNIT_TYPE_COUNT)
        dwIndex = UNIT_TYPE_COUNT - 1;
    
    // 从空闲列表获取内存
    if (m_pFreeHead[dwIndex])
    {
        void* pResult = m_pFreeHead[dwIndex];
        m_pFreeHead[dwIndex] = *(BYTE**)m_pFreeHead[dwIndex];
        m_nFreeCount[dwIndex]--;
        return pResult;
    }
    
    // 空闲列表为空，添加新的空闲内存
    if (!AddFreeMemory(dwIndex))
        return nullptr;
    
    // 再次尝试分配
    if (m_pFreeHead[dwIndex])
    {
        void* pResult = m_pFreeHead[dwIndex];
        m_pFreeHead[dwIndex] = *(BYTE**)m_pFreeHead[dwIndex];
        m_nFreeCount[dwIndex]--;
        return pResult;
    }
    
    return nullptr;
}

void CSDVarMemoryPool::Free(void* p)
{
    if (!p)
        return;
    
    // 检查是否是大内存块
    BYTE* pByte = (BYTE*)p - sizeof(UINT32);
    UINT32 dwLen = *(UINT32*)pByte;
    
    if (dwLen > MAX_UNIT_SIZE)
    {
        free(pByte);
        return;
    }
    
    // 计算单元类型索引
    INT32 dwIndex = (dwLen - 1) / ALIGNMENT;
    if (dwIndex >= UNIT_TYPE_COUNT)
        dwIndex = UNIT_TYPE_COUNT - 1;
    
    // 添加到空闲列表头部
    *(BYTE**)p = m_pFreeHead[dwIndex];
    m_pFreeHead[dwIndex] = (BYTE*)p;
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
    UINT32 dwUnitSize = (dwIndex + 1) * ALIGNMENT;
    
    // 检查当前页是否有足够空间
    if (!m_pWorkPage || m_pPageBuf + dwUnitSize * ALLOC_COUNT > GetPageBufEnd(m_pWorkPage))
    {
        if (!SetMemoryPage())
            return FALSE;
    }
    
    // 分配ALLOC_COUNT个单元
    for (INT32 i = 0; i < ALLOC_COUNT; i++)
    {
        *(BYTE**)m_pPageBuf = m_pFreeHead[dwIndex];
        m_pFreeHead[dwIndex] = m_pPageBuf;
        m_pPageBuf += dwUnitSize;
        m_nFreeCount[dwIndex]++;
    }
    
    return TRUE;
}

BOOL CSDVarMemoryPool::SetMemoryPage()
{
    MemoryPage* pNewPage = (MemoryPage*)malloc(sizeof(MemoryPage) + m_nPageSize);
    if (!pNewPage)
        return FALSE;
    
    pNewPage->Next = m_pHeadPage;
    m_pHeadPage = pNewPage;
    m_pWorkPage = pNewPage;
    m_pPageBuf = GetPageBufGegin(pNewPage);
    
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