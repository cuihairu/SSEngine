/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sddir.h - 目录操作

******************************************************************************/
#ifndef SDDIR_H
#define SDDIR_H

#include "sdtype.h"

/**
* @file sddir.h
* @author lw
* @brief dir utility
*
**/
#ifndef WINDOWS
#include <sys/types.h>
#include <dirent.h>
#endif

#include <string>
#include "sdtype.h"
#include "sdfile.h"
namespace SSCP
{
    /**
    * @defgroup groupdir 目录操作
    * @ingroup  SSCP
    * @{
    */

    struct SFileAttr
    {
        BOOL isDir; //是否为目录
    };
    /**
    *@brief 目录操作类，可用于获取该目录下所有的文件以文件夹的名称
    */
    class CSDDirectory
    {
    public:
        /**
        * @brief
        * 打开一个目录，参数和fopen一致
        * @param pszPath : 目录的路径
        * @return FALSE 代表打开失败，TRUE代表打开成功
        */
        BOOL SSAPI Open(const CHAR* pszPath);

        /**
        * @brief
        * 关闭此目录
        * @return void
        */
        void SSAPI Close();

        /**
        * @brief
        * 读取此目录中下一个文件的名称，并可知道该文件是否为目录
        * @param pszName : [输出参数]从该目录中获取的文件名
        * @param nBufLen : [输入参数]能够赋值的最大长度
        * @param pAttr : [输出参数]返回文件属性, isDir如果为1，则为目录；否则为0；若输入是为NULL，则无返回值
        * @return 成功返回TRUE；读取失败，或者到达目录中文件的结尾返回FALSE
        */
        BOOL SSAPI Read(CHAR *pszName, INT32 nBufLen, SFileAttr *pAttr = NULL);

    private:
#ifdef WINDOWS
        std::string m_szPath;
        HANDLE m_fHandle;
#else
        DIR * m_dir;
		CHAR m_curDir[SD_MAX_PATH]; 
#endif
    };


    /**
    * @brief
    * 创建目录
    * @param pszDirName : 被创建的目录路径及名称
    * @param bforce : 当没有上级目录时，是否强制创建目录
    * @return 成功返回TRUE，失败返回FALSE
    */
    BOOL SSAPI SDCreateDirectory(const CHAR *pszDirName, BOOL bForce= FALSE);

    /**
    * @brief
    * 删除一个目录
    * @param pszDirName : 被删除的目录路径及名称
    * @return 成功返回TRUE，失败返回FALSE(文件夹中有内容或者没有删除权限)
    */
    BOOL SSAPI SDDeleteDirectory(const CHAR *pszDirName, BOOL bForce = FALSE);

    /**
    * @brief
    * 删除一个目录
    * @param pszDirName : 被删除的目录路径及名称
    * @return 成功返回TRUE，失败返回FALSE
    */
    BOOL SSAPI SDDirectoryExists(const CHAR *pszDirName);

	/**
	* @brief
	* 判断一个文件是否为文件夹
	* @param pszFileName : 文件名
	* @return 成功返回TRUE，失败返回FALSE
	*/
    BOOL SSAPI SDIsDirectory(const CHAR * pszFileName);

	/**
	* @brief
	* 判断一个文件夹是否为空
	* @param pszFileName :  文件名
	* @return 成功返回TRUE，失败返回FALSE
	*/
    BOOL SSAPI SDIsEmptyDirectory(const CHAR* pszFileName);

	//TODO 目录拷贝

    /** @} */
}

#endif

