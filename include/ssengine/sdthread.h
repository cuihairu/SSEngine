/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/


#ifndef SD_THREAD_H
#define SD_THREAD_H

/**
* @file sdthread.h
* @author lw
* @brief �̹߳�����
**/

#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupthread ����̶��߳�(�����̳߳�)
    * @ingroup  SSCP
    * @{
    */

#ifdef WINDOWS
    typedef UINT32 SDTHREADID;
#else
    typedef INT32 SDTHREADID;
#endif // WIN32 

    /**
    * @brief
    * �õ��߳�id��
    * @return �߳�id��
    */
    SDTHREADID SSAPI SDGetThreadId();

    typedef struct tagSThreadAttr
    {

    } SThreadAttr;

#if defined(WINDOWS)
    typedef unsigned int (WINAPI *PFThrdProc)(void *);
#define SDTHREAD_DECLARE(x)  unsigned int WINAPI  x
#define SDTHREAD_RETURN(x)   return x;
#else
    typedef void *  (*PFThrdProc)(void *);
#define SDTHREAD_DECLARE(x)  void *  x
#define SDTHREAD_RETURN(x)   return x;
#endif // 


    SDHANDLE SSAPI SDCreateThread(
        SThreadAttr * pAttr,
        PFThrdProc pThrdProc,
        void * pArg,
        SDTHREADID *pThreadId = NULL,
        BOOL bSuspend = FALSE);

    INT32 SSAPI SDThreadWait(SDHANDLE handle);

    void SSAPI SDThreadCloseHandle(SDHANDLE handle);

    void SSAPI SDThreadTerminate(SDHANDLE handle);

    void SSAPI SDThreadSuspend(SDHANDLE handle);

    void SSAPI SDThreadResume(SDHANDLE handle);

    void SSAPI SDSetThreadAttr(SDHANDLE,SThreadAttr * pAttr);

    SThreadAttr* SSAPI SDGetThreadAttr(SDHANDLE);

    /**
    * @brief �̲߳�����
    */
    class CSDThread
    {

    public:
        CSDThread();
        virtual ~CSDThread();

        /**
        * @brief
        * ����һ���߳�
        * @param pThredProc: �߳�ִ�д�������ע�⣺ʹ�ôνӿں󣬲��������ĳ�Ա����ThrdProc
        * @param pArg : �߳�ִ�в���
        * @param bSuspend : �����߳�״̬Ϊ����
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI Start(PFThrdProc pfThrdProc, void *pArg, BOOL bSuspend = FALSE);

        /**
        * @brief
        * ����һ���߳� ע�⣺ʹ�ô˽ӿ������̻߳Ὣ���Ա����ThrdProc��Ϊ�߳�ִ�к������û���̳д�
        * ����ִ���߳�
        * @param bSuspend : �����߳�״̬Ϊ����
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI Start(BOOL bSuspend = FALSE);


        /**
        * @brief
        * ���߳���ֹʱ�Ļص����������Լ̳д˺���ʵ���û��Լ��Ĺ���
        * @return void
        */
        virtual void SSAPI OnTerminated()
        {
        }

        /**
        * @brief
        * �ȴ��߳�ֹͣ
        * @return void
        */
        void SSAPI Wait();

        /**
        * @brief
        * ǿ���߳���ֹ
        * @return void
        */
        void SSAPI Terminate();

        /**
        * @brief
        * �����̣߳��˷���ֻ��Windows����ϵͳ�п���
        * @return void
        */
        void SSAPI Suspend();

        /**
        * @brief
        * �ظ��̣߳��˷���ֻ��Windows����ϵͳ�п���
        * @return void
        */
        void SSAPI Resume();

        /**
        * @brief
        * �����߳�����
        * @param pAttr : ���õ����Խṹ��
        * @return void
        */
        void SSAPI SetAttribute(SThreadAttr *pAttr);

        /**
        * @brief
        * ��ȡ�߳�����
        * @return ���ػ�ȡ���߳�����
        */
        SThreadAttr* SSAPI GetAttribute();

	protected:
        /**
        * @brief
        * �߳�����߳�ִ�к�����. �û���Ҫ���ش˺��������Լ����̺߳���
        * ����û�����Start(bSuspend)�������̣߳�CSDThread�ཫ�����ThrdProcִ���߳�
        * @return void
        */
        virtual void SSAPI ThrdProc()
        {
        }
    private:

		static SDTHREAD_DECLARE(SDThreadFunc) (void * arg); 
        CSDThread (const CSDThread&);
        CSDThread& operator = (const CSDThread&);

        BOOL m_bstart;
        SDTHREADID m_tid;
        SDHANDLE m_handle;
        void* m_arg;			/**<�̺߳�������*/
    };


    /** @} */
}//

#endif


