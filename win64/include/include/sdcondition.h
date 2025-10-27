/******************************************************************************
Copyright (C) YoFei Corporation. All rights reserved.

sdcondition.h - ��������,�����߳�ͬ��

******************************************************************************/


#ifndef SDCONDITION_H
#define SDCONDITION_H

#include "sdtype.h"

/**
* @file sdcondition.h
* @author lw
* @brief ����������
*
**/
#ifndef WINDOWS
#  include <pthread.h>
#  include <errno.h>
#  include <sys/time.h>
#endif
#include "sdtype.h"

namespace  SSCP
{
    /**
    * @defgroup groupsync �߳�ͬ��
    * @ingroup  SSCP
    * @{
    */

// Attempt to resolve POSIX behaviour conformance for win32 build.
#define CONDITION_WIN32_CONFORMANCE_TO_POSIX

    class CSDMutex;

    /**
    *@brief ���������������߳�ͬ��
    */
    class CSDCondition
    {
    public:
        CSDCondition();
        ~CSDCondition();


        /**
        * @brief
        * �ȴ����������������������������һΪ�����ı䣬��Ϊ���жϣ���Ϊ��ʱ��
        * ��������������ԭ�ӵĽ��뻥�����Ļ��ⷶΧ�ڡ�
        * @param mutex : ������������صĻ�����������
        * @param ms : �ȴ���ʱʱ�䣬��λΪ����
        * @return TRUE ����������; FALSE ��ʱ���߱��ж�
        **/
        BOOL SSAPI Wait (CSDMutex &mutex, UINT32 dwMs = SDINFINITE);



        /**
        * @brief
        * �ȴ����������������������������һΪ�����ı䣬��Ϊ���жϣ���Ϊ��ʱ.
        * ��������������ԭ�ӵĽ��뻥�����Ļ��ⷶΧ�ڡ�
        * @param mutex : ������������صĻ�������ָ��
        * @param ms : �ȴ���ʱʱ�䣬��λΪ����
        * @return TRUE ����������; FALSE ��ʱ���߱��ж�
        **/
        BOOL SSAPI Wait (CSDMutex *mutex, UINT32 dwMs = SDINFINITE);

        /**
        * @brief
        * �������еȴ��е����һ���ȴ���Wait��
        * @return void
        **/
        void SSAPI Signal();

        /**
        * @brief
        * �������еĵȴ���Wait��
        * @return void
        **/
        void SSAPI Broadcast();

    private:

        //  no value sematics, therefore private and not implemented.
        CSDCondition (const CSDCondition&);
        CSDCondition& operator = (const CSDCondition&);

    private:
#ifdef WINDOWS
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX

        // boost clone with modification
        // licesnse text below
        void EnterWait ();
        void* m_gate;
        void* m_queue;
        void* m_mutex;
        UINT32 m_gone;  // # threads that timed out and never made it to m_queue
        ULONG m_blocked; // # threads blocked on the condition
        UINT32 m_waiting; // # threads no longer waiting for the condition but
        // still waiting to be removed from m_queue
#  else
        HANDLE mId;
#  endif

#else
        mutable  pthread_cond_t mId;
#endif
    };

    /** @} */
}

#endif

