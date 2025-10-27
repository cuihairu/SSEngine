/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sddir.h - Ŀ¼����

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
    * @defgroup groupdir Ŀ¼����
    * @ingroup  SSCP
    * @{
    */

    struct SFileAttr
    {
        BOOL isDir; //�Ƿ�ΪĿ¼
    };
    /**
    *@brief Ŀ¼�����࣬�����ڻ�ȡ��Ŀ¼�����е��ļ����ļ��е�����
    */
    class CSDDirectory
    {
    public:
        /**
        * @brief
        * ��һ��Ŀ¼��������fopenһ��
        * @param pszPath : Ŀ¼��·��
        * @return FALSE �����ʧ�ܣ�TRUE����򿪳ɹ�
        */
        BOOL SSAPI Open(const CHAR* pszPath);

        /**
        * @brief
        * �رմ�Ŀ¼
        * @return void
        */
        void SSAPI Close();

        /**
        * @brief
        * ��ȡ��Ŀ¼����һ���ļ������ƣ�����֪�����ļ��Ƿ�ΪĿ¼
        * @param pszName : [�������]�Ӹ�Ŀ¼�л�ȡ���ļ���
        * @param nBufLen : [�������]�ܹ���ֵ����󳤶�
        * @param pAttr : [�������]�����ļ�����, isDir���Ϊ1����ΪĿ¼������Ϊ0����������ΪNULL�����޷���ֵ
        * @return �ɹ�����TRUE����ȡʧ�ܣ����ߵ���Ŀ¼���ļ��Ľ�β����FALSE
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
    * ����Ŀ¼
    * @param pszDirName : ��������Ŀ¼·��������
    * @param bforce : ��û���ϼ�Ŀ¼ʱ���Ƿ�ǿ�ƴ���Ŀ¼
    * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
    */
    BOOL SSAPI SDCreateDirectory(const CHAR *pszDirName, BOOL bForce= FALSE);

    /**
    * @brief
    * ɾ��һ��Ŀ¼
    * @param pszDirName : ��ɾ����Ŀ¼·��������
    * @return �ɹ�����TRUE��ʧ�ܷ���FALSE(�ļ����������ݻ���û��ɾ��Ȩ��)
    */
    BOOL SSAPI SDDeleteDirectory(const CHAR *pszDirName, BOOL bForce = FALSE);

    /**
    * @brief
    * ɾ��һ��Ŀ¼
    * @param pszDirName : ��ɾ����Ŀ¼·��������
    * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
    */
    BOOL SSAPI SDDirectoryExists(const CHAR *pszDirName);

	/**
	* @brief
	* �ж�һ���ļ��Ƿ�Ϊ�ļ���
	* @param pszFileName : �ļ���
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
    BOOL SSAPI SDIsDirectory(const CHAR * pszFileName);

	/**
	* @brief
	* �ж�һ���ļ����Ƿ�Ϊ��
	* @param pszFileName :  �ļ���
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
    BOOL SSAPI SDIsEmptyDirectory(const CHAR* pszFileName);

	//TODO Ŀ¼����

    /** @} */
}

#endif

