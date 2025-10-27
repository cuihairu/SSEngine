/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/
#ifndef SDMUTEX_H
#define SDMUTEX_H
/**
* @file sdmutex.h
* @author lw
* @brief �����࣬������������������д����
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
    * @defgroup groupsync �߳�ͬ��
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
    *@brief ������ʲôҲ����
    */
    class CSDNonMutex
    {
    public:

        /**
        * @brief
        * ʲôҲ����
        * @return void
        */
        inline void SSAPI Lock() {}

        /**
        * @brief
        * ʲôҲ����
        * @return void
        */
        inline void SSAPI Unlock() {}

        /**
        * @brief
        * ʲôҲ����
        * @return void
        */
        inline void SSAPI LockRead() {}

        /**
        * @brief
        * ʲôҲ����
        * @return void
        */
        inline void SSAPI LockWrite() {}
    };

    /**
    *@brief �߳�ͬ����ͨ��������
    */
    class CSDMutex
    {
        friend class CSDCondition;

    public:
        CSDMutex();
        ~CSDMutex();

        /**
        * @brief
        * ������
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * �뿪��
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * �����������ǰ���ж�����߳̽���
        * @return void
        */
        void SSAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * ����д������ǰֻ����һ���߳̽���
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
    *@brief �߳�ͬ����д��������
    */
    class CSDRWMutex
    {
    public:
        CSDRWMutex();
        ~CSDRWMutex();

        /**
        * @brief
        * �����������ǰ���ж�����߳̽���
        * @return void
        */
        void SSAPI LockRead();

        /**
        * @brief
        * ����д������ǰֻ����һ���߳̽���
        * @return void
        */
        void SSAPI LockWrite();

        /**
        * @brief
        * ������
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * �뿪��
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * ��ǰ�ж��ٸ���ȡ�߳�ͬʱ��ȡ
        * @return ��ȡ�ߵ�����
        */
        unsigned int SSAPI ReaderCount() const;

        /**
        * @brief
        * ��ǰ�ж��ٸ�д�����߳��ڵȴ�
        * @return �ȴ�д���ߵ�����
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
    *@brief �߳�ͬ���ݹ���������
    */
    class CSDRecursiveMutex
    {
    public:
        CSDRecursiveMutex();
        ~CSDRecursiveMutex();

        /**
        * @brief
        * ������
        * @return void
        */
        void SSAPI Lock();

        /**
        * @brief
        * �뿪��
        * @return void
        */
        void SSAPI Unlock();

        /**
        * @brief
        * �����������ǰ���ж�����߳̽���
        * @return void
        */
        void SSAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * ����д������ǰֻ����һ���߳̽���
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
