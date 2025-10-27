/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/


#ifndef SDSHMEM_H
#define SDSHMEM_H
/**
* @file sdshmem.h
* @author lw
* @brief 共享内存工具
*
**/
#include <string>
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupsharememory 共享内存
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief 共享内存的结构体
    *
    */
    struct SShmem
    {
        SDHANDLE    handle;		/**<文件对应的handle*/
        void *      mem;		/**<共享内存的起始地址*/
        std::string file;		/**<共享内存对应的文件路径*/
    };

    /**
    * @brief
    * 打开一个共享内存
    * @param pszFileName : 共享内存映射文件
    * @param bCreate : 如果没有该共享内存时，是否创建共享内存
    * @param size : 共享内存大小
    * @return 共享内存的结构体
    */
    SShmem SSAPI SDOpenShmem( const CHAR * pszFileName, BOOL bCreate = FALSE, UINT32 size = 0 );

    /**
    * @brief
    * 创建一个共享内存
    * @param pszFileName : 共享内存映射文件
    * @param size : 共享内存大小
    * @return 共享内存的结构体
    */
    SShmem SSAPI SDCreateShmem( const CHAR * pszFileName, UINT32 size );

    /**
    * @brief
    * 创建一个共享内存
    * @param file : 共享内存映射文件
    * @param size : 共享内存大小
    * @return 共享内存的结构体
    */
    BOOL SSAPI SDShmemExist( const CHAR * pszFileName );

    /**
    * @brief
    * 关闭一个共享内存
    * 在windows操作系统中，如果打开次数等于关闭次数，内存将被销毁
    * 在linux操作系统中， 则直接被销毁
    * @param shm : 共享内存的结构体
    * @return void
    */
    void SSAPI SDCloseShmem( SShmem * shm );

    /**
    * @brief
    * 这是一个共享内存类
    */
    class CSDShmem
    {
    public:
        CSDShmem();

        /**
        * @brief
        * 打开一个共享内存
        * @param pszFileName : 文件路径
        * @return 指向共享内存的指针
        */
        void * SSAPI Open( const CHAR * pszFileName );

        /**
        * @brief
        * 创建一个共享内存
        * @param pszFileName : 文件路径
        * @param size : 文件大小
        * @return 指向共享内存的指针
        */
        void * SSAPI Create( const CHAR * pszFileName, UINT32 size );

        /**
        * @brief
        * 关闭共享内存
        * @return void
        */
        void SSAPI Close();

        /**
        * @brief
        * 获取共享内存句柄
        * @return 共享内存句柄
        */
        SDHANDLE GetHandle();

    private:
        SShmem m_shm;
    };

    /** @} */
}//

#endif

