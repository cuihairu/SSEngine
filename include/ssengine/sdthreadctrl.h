/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

 This file provides a set of interfaces used to control the thread.

******************************************************************************/

#ifndef SSCP_SDTHREAD_H_20070614
#define SSCP_SDTHREAD_H_20070614

#include "sdbase.h"

namespace SSCP {

#ifndef SD_INFINITE
#define SD_INFINITE		0xffffffff
#endif


///
/// @class ISSThread
/// @brief A thread "main function" callback interface.
///
class ISSThread
{
public:
	virtual SSAPI ~ISSThread(){}

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThread::ThrdProc() 
    /// @brief  The "main function" of the thread method.
    ///         The method will be called after the thread create.
    ///         When the method return, the thread terminates.
    /// @return 
    ///
    virtual void SSAPI ThrdProc() = 0;

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThread::Terminate() 
    /// @brief  The terminate notify method to the thread.
    ///         When the thread is asked to terminate itself, this method will
    ///         called. User should notify the thread "main" method to quit
    ///         when this method is called.
    /// @return 
    ///
    virtual void SSAPI Terminate() = 0;
};

///
/// @class ISSThreadCtrl
/// @brief A interface used to control the thread.
///        The ISSThreadCtrl is always bound to a ISSThread. The ISSThreadCtrl
///        provides a set of operation to ISSThread.
///
class ISSThreadCtrl
{
public:
	virtual SSAPI ~ISSThreadCtrl(){}

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThreadCtrl::Resume(void) 
    /// @brief  resume the suspended thread.
    /// @return 
    ///
    virtual void SSAPI Resume(void) = 0;

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThreadCtrl::Suspend() 
    /// @brief  suspend the thread
    /// @return 
    ///
    virtual void SSAPI Suspend() = 0;

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThreadCtrl::Terminate(void) 
    /// @brief  terminate the thread
    ///         When Terminate is called, the ISSThread::Terminate method of
    ///         the instance of ISSThread bound to the ISSThreadCtrl will be
    ///         called to ask the thread to terminate itself.
    /// @return 
    ///
    virtual void SSAPI Terminate(void) = 0;

    ///
    /// @fn     virtual bool SSAPI SSCP::ISSThreadCtrl::Kill(UINT32 dwExitCode) 
    /// @brief  force to kill a thread
    /// @param[in] dwExitCode the code which the thread should return.
    /// @return return true if thread is killed successfully.
    ///         return false if failed.
    ///
    virtual bool SSAPI Kill(UINT32 dwExitCode) = 0;

    ///
    /// @fn     virtual bool SSAPI SSCP::ISSThreadCtrl::WaitFor(UINT32 dwWaitTime /*= INFINITE*/) 
    /// @brief  wait for a thread to terminate
    /// @param[in] dwWaitTime timeout interval, in milliseconds. If dwWaitTime
    ///                       is INFINITE, the method's timeout interval never
    ///                       elapses.
    /// @return return true if thread is terminated successfully, or return false
    ///
    ///   The method wait for a thread, until it is terminated or the timeout
    ///   interval elapses. 
    ///
    virtual bool SSAPI WaitFor(UINT32 dwWaitTime = SD_INFINITE) = 0;

    ///
    /// @fn     virtual UINT32 SSAPI SSCP::ISSThreadCtrl::GetThreadId(void) 
    /// @brief  get the thread id of the thread bound to ISSThreadCtrl
    /// @return the thread id of the thread bound by the ISSThreadCtrl.
    ///
    virtual UINT32 SSAPI GetThreadId(void) = 0;
    
    ///
    /// @fn     virtual ISSThread* SSAPI SSCP::ISSThreadCtrl::GetThread(void) 
    /// @brief  get the ISSThread bound to the ISSThreadCtrl
    /// @return the point to the ISSThread object bound to the ISSThreadCtrl
    ///
    virtual ISSThread* SSAPI GetThread(void) = 0;

    ///
    /// @fn     virtual void SSAPI SSCP::ISSThreadCtrl::Release(void) 
    /// @brief  release the ISSThreadCtrl object
    ///
    virtual void SSAPI Release(void) = 0;
};

}

#endif
