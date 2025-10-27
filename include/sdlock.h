/******************************************************************************
		Copyright (C) YoFei Corporation. All rights reserved.

sdlock.h - �߳�ͬ��

******************************************************************************/

#ifndef SDLOCK_H
#define SDLOCK_H
/**
* @file sdlock.h
* @author lw
* @brief ����
*
**/
#include "sdmutex.h"

namespace  SSCP
{
    /**
    * @defgroup groupsync �߳�ͬ��
    * @ingroup  SSCP
    * @{
    */
    /**
    *@brief �߳�ͬ��lock
    */
    template <class MT = CSDMutex>
    class CSDLock
    {
    public:
        CSDLock(MT & mt):m_mutex(mt)
        {
            m_mutex.Lock();
        }
        ~CSDLock()
        {
            m_mutex.Unlock();
        }
    private:
        MT & m_mutex;
    };

    typedef CSDLock<CSDMutex>  CSDMutexLock;

    typedef CSDLock<CSDRecursiveMutex>  CSDRecursiveLock;

    /**
    *@brief �̶߳�ͬ��lock
    */
    class CSDReadLock
    {
    public:
        CSDReadLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockRead();
        }
        ~CSDReadLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex ;
    };

    /**
    *@brief �߳�дͬ��lock
    */
    class CSDWriteLock
    {
    public:
        CSDWriteLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockWrite();
        }
        ~CSDWriteLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex ;
    };



    /**
    * @brief �߳�ͬ��lock����Ҫ���ڴӽ���һ���������˳��ú������������̼�����
    * ��ʹ�øо����ԣ��˼���Ϊ����Χ����һ�����뷶Χ��ֻ����һ���߳�ͬʱ���룩
    * �ļ�������һ�������������󣨶���ĳ����(CriticalSection, Mutex��)�Ķ���ֻ����һ���߳�ռ�д���Դ��
    * ������
    * ����һ�����������ж���˳��㣬�Դ˺���ʹ�����ּ�������������������ԣ����ù��������´���ʱҪ������
    * �������������д�����˳����룬�Ӷ��������
    */
    template<class Mutex>
    class CSDLockGuard
    {
    private:
        Mutex& m_lock;
    public:
        CSDLockGuard(Mutex& oLock) : m_lock(oLock)
        {
            m_lock.Lock();
        }
        ~CSDLockGuard()
        {
            m_lock.Unlock();
        }

        class GuardOff
        {
            CSDLockGuard& m_guard;
        public:
            GuardOff(CSDLockGuard<Mutex>& g):m_guard(g)
            {
                m_guard.m_lock.Unlock();
            }
            ~GuardOff()
            {
                m_guard.m_lock.Lock();
            }
        };
    };


    /** @} */
}
#endif

