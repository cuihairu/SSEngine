/******************************************************************************
					Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/
#ifndef SDMEMORYPOOL_H
#define SDMEMORYPOOL_H

/**
* @file sdmemorypool.h
* @author lw
* @brief �ɱ�ͷǿɱ��ڴ��
*
**/
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupmemorypool �ڴ��(���������)
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief �ɱ��ڴ���С�ڴ�أ�
    * �ڴ��������ڴ治���Զ��ͷţ�ֻ���ڴ������ʱ�ͷš�
    * ע�⣺���ࡰ�ǡ��̰߳�ȫ
    * ʹ��ʾ����
    * CSDVarMemoryPool pool;
    * pool.Create();
    * ĳһ�̣߳�
    * char * p1 = pool.Malloc(512);//�ǵü���
    * char * p2 = pool.Malloc(31);//�ǵü���
    * char * p3 = pool.Malloc(128);//�ǵü���
    *
    * ����һ���̣߳�
    * pool.Free(p3);//�ǵü���
    * pool.Free(p2);//�ǵü���
    * pool.Free(p1);//�ǵü���
    */
    class CSDVarMemoryPool
    {
        struct MemoryPage
        {
            MemoryPage* Next;	// next memory page
        };

    public:
        CSDVarMemoryPool();
        ~CSDVarMemoryPool();

        /**
        * @brief
        * �����ɱ��ڴ��
        * @param dwPageSize : �ڲ�������ڴ�ҳ��С���ڴ治��ʱ���ڴ�ػ�����һ���µ��ڴ�ҳ
        * @return �����ɹ�����TRUE�����򷵻�FALSE
        **/
        BOOL SSAPI Create(UINT32 dwPageSize = 0x80000);

        /**
        * @brief
        * ����Len���ȵ�Buffer
        * @param dwLen : ��õ��ڴ���С����
        * @return  ���ص��ڴ棬�������ΪNULL����������ʧ��
        **/
        void* SSAPI Malloc(UINT32 dwLen);

        /**
        * @brief
        * �����ڴ�
        * @param p : ָ����Ҫ���յ��ڴ�
        * @return void
        **/
        void SSAPI Free(void* p);

        /**
        * @brief
        * ����ڴ�أ�ʹ���е��ڴ涼����ʹ�ã��˷������Ὣ�ڴ淵�ظ�����ϵͳ
        * @return void
        **/
        void SSAPI Clear();

        /**
        * @brief
        * ��ȡ��ǰ�ڴ�ʹ����
        * @return ��ǰ�ڴ�ʹ����
        **/
        INT32 SSAPI GetMemUsed();

    private:
        void* GetPoolMemory(UINT32 dwLen);
        void FreePoolMemory(void* pMemBlock, UCHAR dwType);

        BOOL AddFreeMemory(INT32 dwIndex);
        BOOL SetMemoryPage();

        inline BYTE* GetPageBufGegin(MemoryPage *pPage)
        {
            return (BYTE*)(pPage + 1);
        }

        inline BYTE* GetPageBufEnd(MemoryPage *pPage)
        {
            return (BYTE*)(pPage + 1) + m_nPageSize;
        }

    private:
        static const UINT32 ALIGNMENT = 8;
        static const UINT32 ALLOC_COUNT = 16;
        static const UINT32 MIN_PAGESIZE = 0x40000;	// min pPage size
        static const UINT32 MAX_UNIT_SIZE = 128;
        static const UINT32 UNIT_TYPE_COUNT = 16;

        BYTE* m_pFreeHead[UNIT_TYPE_COUNT];
        INT32 m_nFreeCount[UNIT_TYPE_COUNT];

        MemoryPage* m_pHeadPage;
        MemoryPage* m_pWorkPage;
        BYTE* m_pPageBuf;
        UINT32 m_nPageSize;
    };




    /**
    *@brief �̶��ڴ���С�ڴ�أ����ڷ���̶���С���ڴ��
    * �ڴ��������ڴ治���Զ��ͷţ�ֻ���ڴ������ʱ�ͷ�
    * ע�⣺���ࡰ�ǡ��̰߳�ȫ
    * ʹ��ʾ����
    * CSDFixMemoryPool pool;
    * pool.Create(128);
    * ĳһ�̣߳�
    * char * p1 = pool.Malloc();//�ǵü���
    * char * p2 = pool.Malloc();//�ǵü���
    * char * p3 = pool.Malloc();//�ǵü���
    *
    * ����һ���̣߳�
    * pool.Free(p3);//�ǵü���
    * pool.Free(p2);//�ǵü���
    * pool.Free(p1);//�ǵü���
    */
    class CSDFixMemoryPool
    {
        struct MemoryPage
        {
            MemoryPage* Next;	// next memory page
            INT32 nFreeHead;		// the first free unit in page
            INT32 nFreecount;		// free unit in page
        };

    public:
        CSDFixMemoryPool();
        ~CSDFixMemoryPool();

        /**
        * @brief
        * ��ʼ���ڴ��
        * @param dwUnitSize : ÿһ��������ڴ���С
        * @param dwPageSize : �ڲ�������ڴ�ҳ��С���ڴ治��ʱ���ڴ�ػ�����һ���µ��ڴ�ҳ
        * @return  �����ɹ�����true��ʧ�ܷ���false
        **/
        BOOL SSAPI Create(UINT32 dwUnitSize, UINT32 dwPageSize = 0x40000);

        /**
        * @brief
        * �õ�һ���µ��ڴ�
        * @return  void*
        **/
        void* SSAPI Malloc();

        /**
        * @brief
        * �黹һ�������ڴ�
        * @param p : �ڴ�ĵ�ַ
        * @return  void
        **/
        void SSAPI Free(void* p);

        /**
        * @brief
        * ����ڴ�أ�ʹ���е��ڴ涼����ʹ�ã��˷������Ὣ�ڴ淵�ظ�����ϵͳ
        * @return void
        **/
        void SSAPI Clear();

        /**
        * @brief
        * ��ȡ��ǰ�ڴ�ʹ����
        * @return ��ǰ�ڴ�ʹ����
        **/
        INT32 SSAPI GetMemUsed();

    private:
        BOOL AddMemoryPage();
        void InitPage(MemoryPage *pPage);

        inline char* GetPageBuf(MemoryPage *pPage)
        {
            return (char*)(pPage + 1);
        }

    private:
        static const INT32 ALIGNMENT = 4;
        static const UINT32 MIN_PAGESIZE = 0x40000;	// min page size

        MemoryPage* m_pHeadPage;					// first page
        UINT32 m_nUnitSize;					// unit memory size
        UINT32 m_nPageSize;					// total memory in page
    };


}//namespace SSCP

#endif
