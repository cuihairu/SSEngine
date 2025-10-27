/******************************************************************************
		Copyright (C) YoFei Corporation. All rights reserved.

	sdthreadpool.h - �̳߳ز���
******************************************************************************/


#ifndef SDTHREADPOOL_H
#define SDTHREADPOOL_H
/**
* @file sdthreadpool.h
* @author lw
* @brief �̳߳ص�ʵ��
**/
#include <deque>
#include <vector>
#include <map>
#include "sdthread.h"
#include "sdmutex.h"
#include "sdcondition.h"

namespace SSCP
{
    /**
    * @defgroup groupthread ����̶��߳�(�����̳߳�)
    * @ingroup  SSCP
    * @{
    */

    /**
    *@brief �̳߳�����ӿڣ�ûһ�����񶼱���̳в�ʵ�ָýӿ�
    */
    class ISSRunable
    {
    public:
		virtual ~ISSRunable(){}
        /**
        * @brief
        * ����ӿڣ�ÿһ������Ҫʵ�ָýӿ�
        * @return void
        **/
        virtual void Run() = 0;
    };

    /**
    *@brief �̳߳ز�����
    */
    class CSDThreadPool
    {
        typedef std::deque<ISSRunable*> JobContainer;

        typedef std::vector<CSDThread*> ThreadContainer;

        typedef struct _tagThreadArg
        {
            CSDThreadPool* pThreadPool;
            CSDThread* pThread;
            volatile BOOL keepWorking;
        } ThreadArg;

        typedef std::vector<ThreadArg*> ThreadArgContainer;

        typedef std::map<ThreadArg*, ThreadArg*> ThreadArgMap;

        typedef std::pair<ThreadArg*, ThreadArg*> ThreadArgPair;

    public:
        CSDThreadPool();
        ~CSDThreadPool();

        /**
        * @brief
        * ��ʼ���̳߳�
        * @param dwMinThrds : �̳߳���С�߳���
        * @param dwMaxThrds : �̳߳�����߳���
        * @param dwMaxPendJobs : �����Ϊִ�����������
        * @return  �����ɹ�����true��ʧ�ܷ���false
        **/
        BOOL Init(UINT32 dwMinThrds, UINT32 dwMaxThrds, UINT32 dwMaxPendJobs);

        /**
        * @brief
        * ��������̳߳�
        * @param pJob : ��������
        * @return  �ɹ�����true��ʧ�ܷ���false
        **/
        BOOL ScheduleJob(ISSRunable *pJob);

        /**
        * @brief
        * �����̳߳�����ִ���̣߳����ȴ�δִ�е�����ִ����ɣ��������ٵ�ʱ��
        * @return void
        **/
        void TerminateQuick();

        /**
        * @brief
        * �����̳߳�����ִ���̣߳��ȴ���������ִ������ٽ����̣߳��������ٵ�ʱ��
        * @return void
        **/
        void TerminateWaitJobs();

        /**
        * @brief
        * �õ��̳߳��й����̵߳���Ŀ
        * @return �̳߳��й����̵߳���Ŀ
        **/
        UINT32 GetThreadNum();

        /**
        * @brief
        * �õ��̳߳��еȴ�ִ�е�������Ŀ
        * @return �̳߳��еȴ�ִ�е�������Ŀ
        **/
        UINT32 GetJobPending();


    private:
        static SDTHREAD_DECLARE( WorkThreadFunc)(void *pArg);

        CSDThreadPool(const CSDThreadPool &other);              // no implementation
        void operator = (const CSDThreadPool &other);       // no implementation

    private:
        static const UINT32 WAITTIME = 32;
        static const UINT32 WAITCOUNT = 16384;

        JobContainer	   m_jobContainer;
        ThreadArgContainer m_threadArgContainer;

        ThreadArgMap  m_threadArgMap;
        CSDMutex	  m_jobMutex;
        CSDMutex	  m_threadMutex;
        CSDCondition  m_jobCondition;

        UINT32 m_minThreads;
        UINT32 m_maxThreads;
        UINT32 m_maxPendingJobs;

        volatile BOOL m_waitTerminate;
        volatile BOOL m_quickTerminate;
    };


    /** @} */
}//

#endif


