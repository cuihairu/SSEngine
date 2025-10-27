/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/
#ifndef SDMUTEX_H
#define SDMUTEX_H
/**
* @file sdmutex.h
* @author lw
* @brief 互斥类，包括无锁，有锁，读写锁等
*
**/
#include "sdtype.h"
#include "sdcondition.h"

#ifndef WINDOWS
#include <pthread.h>
#endif

namespace SSCP
{
    /**
    * @defgroup groupsync 线程同步
    * @ingroup  SSCP
    * @{
    */
#ifdef WINDOWS
    typedef CRITICAL_SECTION SDMutexId;
#else
    typedef pthread_mutex_t SDMutexId;
#endif

    BOOL SDMutexInit(SDMutexId & id);

    void SDMutexLock(SDMutexId & id);

    void SDMutexUnlock(SDMutexId & id);

    BOOL SDMutexUninit(SDMutexId &id);


    /**
    *@brief 空锁，什么也不做
    */
    class CSDNonMutex
    {
    public:

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SSAPI Lock() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SSAPI Unlock() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SSAPI LockRead() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SSAPI LockWrite() {}
    };

    /**
    *@brief 线程同步普通锁操作类
    */
    class CSDMutex
    {
        friend class CSDCondition;

    public:
        CSDMutex();
        ~CSDMutex();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SSAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SSAPI LockWrite()
        {
            Lock();
        }

    private:
        //  no value sematics, therefore private and not implemented.
        CSDMutex (const CSDMutex&);
        CSDMutex& operator= (const CSDMutex&);

    private:
#ifdef WINDOWS
        SDMutexId m_mutexId;
#else
        mutable SDMutexId m_mutexId;
#endif
    };

    /**
    *@brief 线程同步读写锁操作类
    */
    class CSDRWMutex
    {
    public:
        CSDRWMutex();
        ~CSDRWMutex();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SSAPI LockRead();

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SSAPI LockWrite();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * 当前有多少个读取线程同时读取
        * @return 读取者的数量
        */
        unsigned int SSAPI ReaderCount() const;

        /**
        * @brief
        * 当前有多少个写操作线程在等待
        * @return 等待写入者的数量
        */
        unsigned int SSAPI PendingWriterCount() const;

    private:
        CSDMutex m_mutex;
        CSDCondition m_readCond;
        CSDCondition m_pendingWriteCond;
        unsigned int m_readerCount;
        bool m_hasWriterLock;
        unsigned int m_pendingWriteCount;
    };

    /**
    *@brief 线程同步递归锁操作类
    */
    class CSDRecursiveMutex
    {
    public:
        CSDRecursiveMutex();
        ~CSDRecursiveMutex();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SSAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SSAPI LockWrite()
        {
            Lock();
        }
    private:
        SDMutexId m_mutexId;
#ifndef WINDOWS
        pthread_mutexattr_t m_mutexAttr;
#endif
    };

    /** @} */
} //namespace SSCP



#endif
