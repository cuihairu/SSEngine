/******************************************************************************
		Copyright (C)  YoFei Corporation. All rights reserved.

sdfifodqueue.h - ʹ��˫���е��Ƚ��ȳ�����


******************************************************************************/


#ifndef SD_FIFODQUEUE_H
#define SD_FIFODQUEUE_H
#include "sdmutex.h"
#include "sdlock.h"
#include "sdcondition.h"
#include <deque>

namespace SSCP
{
    /**
    * @defgroup groupmessagequeue ��Ϣ����
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief ʹ��˫���е���Ϣ���У�һ����������Ϣ��
    * һ������д����Ϣ�����ڲ���˫���У�����д��Ϣ�����໥Ӱ�죨������
    */
    template< typename T, typename ReadMT = CSDMutex, typename WriteMT = CSDMutex >
    class CSDFifoDqueue
    {
    public:

        typedef std::deque< T* > TypedDeque;

        /**
        * @brief
        * ����Ϣ���л�ȡ��Ϣ
        * @param ms : ��ȡ��Ϣ�ĳ�ʱʱ��
        * ��ms������ʱ�����һ�Σ������Ƿ��ܻ�ȡ�������������
        * ��ms������ʱ���ȴ���Ӧ��ʱ��
        * @return ��ȡ����Ϣָ�룬�����ʱ������NULL
        */
        T * Pop(UINT32 ms = 0)
        {
            for (;;)
            {

                {
                    // if there are only one thread to read the queue
                    // we needn't to lock it .
                    CSDLock<ReadMT> rlock(m_readMutex);
                    if (!m_readQueue.empty())
                    {
                        T * pItem = m_readQueue.front();
                        m_readQueue.pop_front();
                        return pItem;
                    }
                }

                {
                    CSDLock<WriteMT> wlock(m_writeMutex);
                    if (m_writeQueue.empty())
                    {
                        if (ms == 0)
                        {
                            return NULL;
                        }

                        bool ret = m_fifoNotEmpty.Wait(m_writeMutex, ms);
                        if (!ret)
                        {
                            return NULL;
                        }
                    }

                    {
                        CSDLock<ReadMT> rlock(m_readMutex);
                        if (m_readQueue.empty() && !m_writeQueue.empty())
                        {
                            m_writeQueue.swap(m_readQueue);
                        }
                    }
                }
            }
        }

        /**
        * @brief
        * ����Ϣ����д����Ϣ
        * @param pItem : д�����Ϣָ��
        * @return void
        */
        void Push(T * pItem)
        {
            CSDLock<WriteMT> lock (m_writeMutex);
            m_writeQueue.push_back(pItem);
            m_fifoNotEmpty.Broadcast();
        }

        /**
        * @brief
        * ��ǰ�ڶ������еĿɶ�����Ϣ��
        * @return void
        */
        size_t ReadSize()
        {
            return m_readQueue.size();
        }

        /**
        * @brief
        * ��ǰд�����е���Ϣ����
        * @return �Ѿ�д����Ϣ����
        */
        size_t WriteSize()
        {
            return m_writeQueue.size();
        }

    private:
        TypedDeque    m_readQueue;
        TypedDeque    m_writeQueue;
        WriteMT       m_writeMutex;
        ReadMT        m_readMutex;
        CSDCondition  m_fifoNotEmpty;
    };

};
#endif // 
