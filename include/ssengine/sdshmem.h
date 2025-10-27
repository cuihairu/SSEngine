/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/


#ifndef SDSHMEM_H
#define SDSHMEM_H
/**
* @file sdshmem.h
* @author lw
* @brief �����ڴ湤��
*
**/
#include <string>
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupsharememory �����ڴ�
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief �����ڴ�Ľṹ��
    *
    */
    struct SShmem
    {
        SDHANDLE    handle;		/**<�ļ���Ӧ��handle*/
        void *      mem;		/**<�����ڴ����ʼ��ַ*/
        std::string file;		/**<�����ڴ��Ӧ���ļ�·��*/
    };

    /**
    * @brief
    * ��һ�������ڴ�
    * @param pszFileName : �����ڴ�ӳ���ļ�
    * @param bCreate : ���û�иù����ڴ�ʱ���Ƿ񴴽������ڴ�
    * @param size : �����ڴ��С
    * @return �����ڴ�Ľṹ��
    */
    SShmem SSAPI SDOpenShmem( const CHAR * pszFileName, BOOL bCreate = FALSE, UINT32 size = 0 );

    /**
    * @brief
    * ����һ�������ڴ�
    * @param pszFileName : �����ڴ�ӳ���ļ�
    * @param size : �����ڴ��С
    * @return �����ڴ�Ľṹ��
    */
    SShmem SSAPI SDCreateShmem( const CHAR * pszFileName, UINT32 size );

    /**
    * @brief
    * ����һ�������ڴ�
    * @param file : �����ڴ�ӳ���ļ�
    * @param size : �����ڴ��С
    * @return �����ڴ�Ľṹ��
    */
    BOOL SSAPI SDShmemExist( const CHAR * pszFileName );

    /**
    * @brief
    * �ر�һ�������ڴ�
    * ��windows����ϵͳ�У�����򿪴������ڹرմ������ڴ潫������
    * ��linux����ϵͳ�У� ��ֱ�ӱ�����
    * @param shm : �����ڴ�Ľṹ��
    * @return void
    */
    void SSAPI SDCloseShmem( SShmem * shm );

    /**
    * @brief
    * ����һ�������ڴ���
    */
    class CSDShmem
    {
    public:
        CSDShmem();

        /**
        * @brief
        * ��һ�������ڴ�
        * @param pszFileName : �ļ�·��
        * @return ָ�����ڴ��ָ��
        */
        void * SSAPI Open( const CHAR * pszFileName );

        /**
        * @brief
        * ����һ�������ڴ�
        * @param pszFileName : �ļ�·��
        * @param size : �ļ���С
        * @return ָ�����ڴ��ָ��
        */
        void * SSAPI Create( const CHAR * pszFileName, UINT32 size );

        /**
        * @brief
        * �رչ����ڴ�
        * @return void
        */
        void SSAPI Close();

        /**
        * @brief
        * ��ȡ�����ڴ���
        * @return �����ڴ���
        */
        SDHANDLE GetHandle();

    private:
        SShmem m_shm;
    };

    /** @} */
}//

#endif

