/******************************************************************************
		Copyright (C)  YoFei Corporation. All rights reserved.


******************************************************************************/

#ifndef SD_FIFOQUEUE_H
#define SD_FIFOQUEUE_H
#include "sdmutex.h"
#include "sdlock.h"
#include "sdcondition.h"
#include <deque>
#define MULTI_THREAD_READ

namespace SSCP
{
    /**
    * @defgroup groupmessagequeue ��Ϣ����
    * @ingroup  SSCP
    * @{
    */

    /**
    *@brief  ʹ�õ����е���Ϣ���У��Ӷ���ͷ�����ݣ��Ӷ���βд������
    */
    template< typename T, typename MT = CSDMutex >
    class CSDFifoQueue
    {
    public:

        typedef std::deque< T* > TypedDeque;

        /**
        * @brief
        * ����Ϣ���л�ȡ��Ϣ
        * @param ms : ��ȡ��Ϣ�ĳ�ʱʱ�䣬��λ����
        * ��msС����ʱ�����޵ȴ�
        * ��ms������ʱ�����һ�Σ������Ƿ��ܻ�ȡ�������������
        * ��ms������ʱ���ȴ���Ӧ��ʱ��
        * @return :  ��ȡ����Ϣָ�룬�����ʱ������NULL
        */
        T * Pop(int ms = 0)
        {
            for (;;)
            {
                {
                    CSDLock<MT> lock (m_queueMutex);
                    // if there are only one thread to read the queue
                    // we needn't to lock it .
                    if (!m_queue.empty())
                    {
                        T * pItem = m_queue.front();
                        m_queue.pop_front();
                        return pItem ;
                    }
                }

                if (ms == 0)
                {
                    return NULL;
                }
                if (ms < 0)
                {
                    CSDLock<MT> lock (m_queueMutex);
                    m_fifoNotEmpty.Wait(m_queueMutex);
                }
                else
                {
                    CSDLock<MT> lock (m_queueMutex);
                    bool ret = m_fifoNotEmpty.Wait(m_queueMutex,ms);
                    if (!ret)
                    {
                        return NULL;
                    }
                }
            }
            return NULL;
        }

        /**
        * @brief
        * ����Ϣ����д����Ϣ
        * @param pItem : д�����Ϣָ��
        * @return void
        */
        void Push(T * pItem)
        {
            CSDLock<MT> lock (m_queueMutex);
            m_queue.push_back(pItem);
            m_fifoNotEmpty.Broadcast();
        }

        /**
        * @brief
        * ��ǰ�ڶ������еĿɶ�����Ϣ��
        * @return : ��Ϣ�����е���Ϣ����
        */
        size_t ReadSize()
        {
            return m_queue.size();
        }

        size_t WriteSize()
        {
            return m_queue.size();
        }

    private:

        TypedDeque  m_queue;
        MT m_queueMutex;
        CSDCondition  m_fifoNotEmpty;
    };

}
#endif // 
