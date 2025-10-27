/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

	sdthrdqueue.h - ���������̶߳���,���ڶ��̻߳�����ʹ��
******************************************************************************/


#ifndef SDDATAQUEUE_H
#define SDDATAQUEUE_H
/**
* @file sdthrdqueue.h
* @author lw
* @brief ѭ�����ݶ���
*
**/
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup grouploopbuffer ������ѭ�����ݶ���
    * @ingroup  SSCP
    * @{
    */

    /**
    *@brief ѭ��ָ����У���ȡΪ������ָ�룬֧�ֵ��̶߳�д
    */
    class CSDDataQueue
    {
    public:
        CSDDataQueue(void);
        ~CSDDataQueue(void);

        /**
        * @brief
        * ��ʼ��ѭ��������
        * @param nSize ������ĳ��ȣ�ʵ�ʳ���Ϊ����ĳ��ȼ�1
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI Init(INT32 nSize);

        /**
        * @brief
        * ��ʹ����ϵ�Buffer����ѭ���������Ľ�β
        * @param ptr : ָ��Buffer�ĵ�ַ
        * @return ���ѭ��������ӵ�еĴ�С����nLen������TRUE�����򷵻�FALSE
        * @remark �˺��������̰߳�ȫ��
        */
        BOOL SSAPI PushBack(void *ptr);

        /**
        * @brief
        * ��ȡ�µ�Buffer
        * @return ָ��Buffer�ĵ�ַ�����ʧ�ܣ�����NULL
        * @remark �˺��������̰߳�ȫ��
        */
        void* SSAPI PopFront();

    protected:
        void            **m_pArray;	/**<��ȡ��ַ������*/
        INT32			m_nHead;	/**<������ͷ��*/
        INT32			m_nTail;	/**<������β��*/
        INT32			m_nSize;	/**<��������С*/
    };

    /** @} */
}

#endif
