/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/

#ifndef SDLOOPBUFFER_H
#define SDLOOPBUFFER_H
/**
* @file sdloopbuffer.h
* @author lw
* @brief ѭ����������
*
**/
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup grouploopbuffer ѭ��������
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief ѭ����������֧�ֵ��̶߳�д
    */
    class CSDLoopBuffer
    {
    public:
        CSDLoopBuffer(void);
        ~CSDLoopBuffer(void);

        /**
        * @brief
        * ��ʼ��ѭ��������
        * @param nSize : ��ʼ����û������Ĵ�С��ʵ�ʴ�СΪnSize+1
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI Init(INT32 nSize);

        /**
        * @brief
        * ����Ҫ�洢��Buffer������ѭ���������Ľ�β
        * @param pData : [�������]ָ����Ҫ����ѭ����������Buffer��ʼλ��
        * @param nLen : ָ����Ҫ�����Buffer�ĳ���
        * @return ���ѭ��������ӵ�еĴ�С���ڵ���nLen������TRUE�����򷵻�FALSE
        * @remark �˺��������̰߳�ȫ��
        */
        BOOL SSAPI PushBack(const CHAR *pData, INT32 nLen);

        /**
        * @brief
        * ��ѭ������������ʼλ��ȡnLen���ȵ�Buffer����������Buffer��
        * @param pBuf : [�����������]��ȡ���ݵ�Buffer����ʼָ��
        * @param nLen : ��Ҫ������Buffer����
        * @return ������㹻������������ݣ�����TRUE�����򷵻�FALSE
        * @remark �˺��������̰߳�ȫ��
        */
        BOOL SSAPI PopFront(CHAR *pBuf, INT32 nLen);

        /**
        * @brief
        * ����nLen���ȵ�����
        * @param nLen : ��Ҫ�����ĳ���
        * @return void
        * @remark �˺��������̰߳�ȫ��
        */
        void SSAPI DiscardFront(INT32 nLen);

	


    private:
        CHAR    *m_pBuffer;
        CHAR    *m_pHead;
        CHAR    *m_pTail;
        INT32	m_nSize;
    };

    /** @} */
}

#endif
