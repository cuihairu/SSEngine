/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/


#ifndef SD_THREAD_H
#define SD_THREAD_H

/**
* @file sdthread.h
* @author lw
* @brief 线程工具类
**/

#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupthread 多进程多线程(包括线程池)
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
    * 得到线程id号
    * @return 线程id号
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
    * @brief 线程操作类
    */
    class CSDThread
    {

    public:
        CSDThread();
        virtual ~CSDThread();

        /**
        * @brief
        * 启动一个线程
        * @param pThredProc: 线程执行处理函数，注意：使用次接口后，不会调用类的成员函数ThrdProc
        * @param pArg : 线程执行参数
        * @param bSuspend : 设置线程状态为挂起
        * @return 成功返回TRUE，失败返回FALSE
        */
        BOOL SSAPI Start(PFThrdProc pfThrdProc, void *pArg, BOOL bSuspend = FALSE);

        /**
        * @brief
        * 启动一个线程 注意：使用此接口启动线程会将类成员函数ThrdProc作为线程执行函数，用户需继承此
        * 类来执行线程
        * @param bSuspend : 设置线程状态为挂起
        * @return 成功返回TRUE，失败返回FALSE
        */
        BOOL SSAPI Start(BOOL bSuspend = FALSE);


        /**
        * @brief
        * 当线程终止时的回调函数，可以继承此函数实现用户自己的功能
        * @return void
        */
        virtual void SSAPI OnTerminated()
        {
        }

        /**
        * @brief
        * 等待线程停止
        * @return void
        */
        void SSAPI Wait();

        /**
        * @brief
        * 强制线程终止
        * @return void
        */
        void SSAPI Terminate();

        /**
        * @brief
        * 挂起线程，此方法只在Windows操作系统中可用
        * @return void
        */
        void SSAPI Suspend();

        /**
        * @brief
        * 回复线程，此方法只在Windows操作系统中可用
        * @return void
        */
        void SSAPI Resume();

        /**
        * @brief
        * 设置线程属性
        * @param pAttr : 设置的属性结构体
        * @return void
        */
        void SSAPI SetAttribute(SThreadAttr *pAttr);

        /**
        * @brief
        * 获取线程属性
        * @return 返回获取的线程属性
        */
        SThreadAttr* SSAPI GetAttribute();

	protected:
        /**
        * @brief
        * 线程类的线程执行函数体. 用户需要重载此函数运行自己的线程函数
        * 如果用户调用Start(bSuspend)来启动线程，CSDThread类将会调用ThrdProc执行线程
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
        void* m_arg;			/**<线程函数参数*/
    };


    /** @} */
}//

#endif


