/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdfile.h - �ļ�����API

******************************************************************************/


#ifndef SDFILE_H
#define SDFILE_H

#include "sdtype.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>

/**
* @file sdfile.h
* @author lw
* @brief �ļ�������
*
**/

namespace SSCP
{
    /**
    * @defgroup groupfile �ļ�����
    * @ingroup  SSCP
    * @{
    */

    /* An arbitrary size that is digestable. True max is a bit less than 32000 */
#ifdef WINDOWS
#define SD_MAX_PATH  MAX_PATH
#define SDPATH_DELIMITER '\\'
#else /* !SD_HAS_UNICODE_FS */
#define SD_MAX_PATH 260
#define SDPATH_DELIMITER '/'
#endif

    /* Win32 .h ommissions we really need */
#define SDFILENO_STDIN   0
#define SDFILENO_STDOUT  1
#define SDFILENO_STDERR  2
#define SDPATH_IS_DELIMITER(x)  (x == '\\' || x == '/')
    /**
    *@brief �ļ�״̬�ṹ��
    */
    struct SFileStatus
    {
        UINT32 mode;		/**<�ļ���Ӧ��ģʽ���ļ���Ŀ¼��*/
        UINT32 inode;		/**<i-node�ڵ��, linux-specific*/
        UINT32 dev;			/**<�豸����*/
        UINT32 rdev;		/**<�����豸����*/
        UINT32 nlink;		/**<�ļ���������*/
        UINT32 uid;			/**<�ļ�������, linux-specific*/
        UINT32 gid;			/**<�ļ������߶�Ӧ����, linux-specific*/
        UINT32 size;		/**<��ͨ�ļ�����Ӧ���ļ��ֽ���*/
        time_t atime;		/**<�ļ���󱻷��ʵ�ʱ��*/
        time_t mtime;		/**<�ļ���������޸ĵ�ʱ��*/
        time_t ctime;		/**<�ļ�״̬�����ԣ��ı�ʱ��*/
    };


    /**
    *@brief �ļ�Ȩ��ö����
    */
    enum AccessMode
    {
#ifdef WINDOWS
        EXIST_OK = 0,
        RD_OK = 2,
        WD_OK = 4,
#else
        EXIST_OK = F_OK,
        RD_OK = R_OK,
        WD_OK = W_OK,
        EXECUTE_OK = X_OK,
#endif
    };



    /**
    *@brief �ļ������࣬�����������ļ�����,��ʹ��FileMapping��ʽ��ȡ�ļ�(���幦�ܲ��սӿ�)
    */
    class CSDFile
    {

    public:
        CSDFile();
        ~CSDFile();

        /**
        *@brief seekλ��ö����
        */
        enum SeekOffset
        {
            SK_SET = SEEK_SET,		/**<�ļ���ʼλ��*/
            SK_END = SEEK_END,		/**<�ļ���βλ��*/
            SK_CUR = SEEK_CUR,		/**<�ļ���ǰλ��*/
        };

        /**
        * @brief
        * Open a file, type����ͬfopen�ӿ�
        * @param pszFileName : �ļ�����
        * @param type : �ļ�����Ȩ��
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI Open(const CHAR* pszFileName, const CHAR* type, BOOL bFileMapping = FALSE);

        /**
        * @brief
        * �ر�һ���ļ�
        * @return void
        */
        void SSAPI Close();

        /**
        * @brief
        * Flushˢ���ļ�
        * @return void
        */
        void SSAPI Flush();

        /**
        * @brief
        * ���ļ��ж�ȡ����
        * @param pBuf : ��ȡ���ݵĴ��λ��
        * @param nLen : �ܹ��淢���ݵ���󳤶�
        * @return ʵ�ʶ�ȡ�����ݳ���
        */
		UINT32 SSAPI Read(void* pBuf, UINT32 nLen);

        /**
        * @brief
        * ���ļ���д������
        * @param pBuf : ָ��д�����ݵ�ָ��
        * @param nLen : ����д�����ݳ���
        * @return ʵ��д������ݳ���
        */
		UINT32 SSAPI Write(const void* pBuf, UINT32 nLen);

        /**
        * @brief
        * �ƶ��ļ��α굽�ض���λ��
        * @param offset : ��Գ�ʼλ���ƶ��ľ���
        * @param whence : ��ʼλ�ã�������enum SeekOffset�е�ֵ
        * @return ����ɹ�������0
        */
        INT32 SSAPI Seek(INT32 offset, CSDFile::SeekOffset whence);

        /**
        * @brief
        * �ƶ��ļ��α굽��ǰ�ļ����ļ�ͷ
        * @return ���أ�1����ʧ��
        */
        INT32 SSAPI SeekToBegin();

        /**
        * @brief
        * �ƶ��ļ��α굽��ǰ�ļ����ļ�β
        * @return ���أ�1����ʧ��
        */
        INT32 SSAPI SeekToEnd();

        /**
        * @brief
        * ��ȡ��ǰ�ļ��α�����ļ�ͷ�ľ��룬��λbyte
        * @return ��ǰ�ļ��α�����ļ�ͷ�ľ���
        */
		UINT32 SSAPI GetPosition();

        /**
        * @brief
        * �����ļ��ĳ��ȣ���λΪbyte
        * @param newLen : ���µ��ļ����ȣ���λΪbyte
        * @return void
        */
        void SSAPI SetLength(UINT32 newLen);

        /**
        * @brief
        * �����Ƿ�Ϊ�ļ�β��
        * @return ������ļ�β��������TRUE�����򷵻�FALSE
        */
        BOOL SSAPI Eof();

        /**
        * @brief
        * ��ȡ��ǰ�ļ�������
        * @param pStatus : ��ȡ���ļ����Խṹ��
        * @return ����ɹ�������TRUE�����򷵻�FALSE
        */
        BOOL SSAPI GetFileStatus(SFileStatus &pStatus);

    private:
        CSDFile(const CSDFile& origFile);              // no implementation
        void operator=(const CSDFile& origFile);       // no implementation

        void * m_pFileHandle;
        std::string m_filepath;
        BOOL m_bFileMapping; // �Ƿ�ʹ��FileMapping��ʽ��ȡ�ļ�
    };


    /**
    * @brief
    * ��ȡ�ļ�������
    * @param pszFileName : ��Ҫ��ȡ�ļ����Ե��ļ�·��
    * @param pStatus : ��ȡ���ļ����Խṹ��
    * @return ����ɹ�������TRUE�����򷵻�FALSE
    */
    BOOL SSAPI SDGetFileStatus(const CHAR* pszFileName, SFileStatus& stStatus);

    /**
    * @brief
    * ɾ��һ���ļ�
    * @param pszFileName : ��Ҫɾ�����ļ����ļ�·��
    * @return �Ƿ�ɹ�������TRUE�����򷵻�FALSE
    */
    BOOL SSAPI SDFileRemove(const CHAR* pszFileName);

    /**
    * @brief
    * ����һ���ļ����ļ������������ļ�·����
    * @param pszOldName : ԭ�ȵ��ļ�·��
    * @param pszNewName : �µ��ļ�·��
    * @return �ɹ�����0
    */
    INT32 SSAPI SDFileRename(const CHAR *pszOldName, const CHAR *pszNewName, BOOL bForce = FALSE);

    /**
    * @brief
    * ����һ���ļ����ļ�·�����������ļ�����
    * @param pszOldName : ԭ�ȵ��ļ�·��
    * @param pszNewName : �µ��ļ�·��
    * @return �ɹ�����0
    */
    INT32 SSAPI SDFileMove(const CHAR *pszOldName, const CHAR *pszNewName, BOOL bForce = FALSE);

    /**
    * @brief
    * ȷ���ļ��Ķ�дȨ��
    * @param pszPath : ��Ҫȷ����дȨ�޵��ļ�·��
    * @param mode : ��Ҫȷ�ϵĿɷ���ģʽ
    * �����¼���ģʽ��EXIST_OK(�Ƿ����), RD_OK(�Ƿ�ɶ�), WD_OK(�Ƿ��д)
    * @return ���ӵ�и�ģʽ������0
    */
    INT32 SSAPI SDFileAccess(const CHAR *pszPath, AccessMode mode);

    /**
    * @brief
    * ��ȡ��ǰӦ�õ�����·��,����ִ���ļ����ļ���
    * ��Ҫע����ǣ��˺����������������Ҳ�޷�ȷ���̰߳�ȫ
    * @return ��ȡ�ĵ�ǰӦ�õ�·��
    */
    const CHAR* SSAPI SDGetModuleName(void);

    /**
    * @brief
    * ��ȡ��ǰӦ�õ�·��
    * ��Ҫע����ǣ��˺����������������Ҳ�޷�ȷ���̰߳�ȫ
    * @return ��ǰӦ�õ�·��
    */
    const CHAR* SSAPI SDGetModulePath();

	/**
	* @brief
	* ��ȡ��ǰӦ�õĹ���·��(current work directory) 
	* ��Ҫע����ǣ��˺����������������Ҳ�޷�ȷ���̰߳�ȫ
	* @return ��ǰӦ�õĹ���·��
	*/
    const CHAR* SSAPI SDGetWorkPath();

    /**
    * @brief
    * ��ȡ�ļ����ļ�·��
    * @param pszFileName : �ļ�·�����ļ�����
    * @return �ļ�·��
    */
    std::string SSAPI SDFileExtractPath(const CHAR *pszFileName);

    /**
    * @brief
    * ��ȡ�ļ����ļ���
    * @param pszFileName : �ļ�·�����ļ�����
    * @return �ļ���
    */
    std::string SSAPI SDFileExtractName(const CHAR *pszFileName);

    /**
    * @brief
    * ��ȡ�ļ����ļ���չ��
    * @param pszFileName : �ļ�·�����ļ�����
    * @return �ļ���չ��
    */
    std::string SSAPI SDFileExtractExt(const CHAR *pszFileName);

    /**
    * @brief
    * ȥ���ļ�·������"\"
    * @param pszPath : �ļ�·��
    * @return ȥ������"\"����ļ�·��
    */
    std::string SSAPI SDFileExcludeTrailingDelimiter(const CHAR *pszPath);

    /**
    * @brief
    * ���"\"���ļ�·��������ļ�·������"\"�������
    * @param pszPath : �ļ�·��
    * @return ���"\"����ļ�·��
    */
    std::string SSAPI SDFileIncludeTrailingDelimiter(const CHAR *pszPath);

    /**
    * @brief
    * ���"\"���ļ�·��ǰ�����ļ�·��ǰ����"\"�������
    * @param pszPath : �ļ�·��
    * @return ���"\"����ļ�·��
    */
    std::string SSAPI SDFileIncludeLeadingDelimiter(const CHAR *pszPath);

    /**
    * @brief
    * ȥ���ļ�·��ǰ��"\"
    * @param pszPath : �ļ�·��
    * @return ȥ��"\"����ļ�·��
    */
    std::string SSAPI SDFileExcludeLeadingDelimiter(const CHAR *pszPath);

    /**
    * @brief
    * ���ļ�·���е�"\"��"//"�滻Ϊ�ض����ַ�delimiter
    * @param pszPath : �ļ�·��
    * @param delimiter : �滻�ַ�
    * @return �滻����ļ�·��
    */
    std::string SSAPI SDFileReplaceDelimiter(const CHAR *pszPath, CHAR delimiter);

    /**
    * @brief
    * ����ļ��Ƿ����
    * @param pszFileName : �ļ�·��
    * @return �����򷵻�TRUE���������򷵻�FALSE
    */
    BOOL SSAPI SDFileExists(const CHAR *pszFileName);

    /**
    * @brief
    * ��ȡ�ļ���С
    * @param pszFileName : �ļ�·��
    * @return �ļ���С
    */
    INT32 SSAPI SDFileGetSize(const CHAR *pszFileName);

    /**
    * @brief
    * �����ļ�
    * @param pszFileName : �ļ�·��
    * @return �����ɹ�����TRUE�����򷵻�FALSE
    */
    BOOL SSAPI SDFileCreate(const CHAR *pszFileName);


	
	/**
	* @brief
	* �����ļ�
	* @param pszExistFile :Դ�ļ�·�� 
	* @param pNewFile :   Ŀ���ļ�·��
	* @param bFailIfExists : ���Ŀ������Ƿ񷵻�ʧ��
	* @return �����ɹ�����TRUE�����򷵻�FALSE
	*/
	BOOL SSAPI SDFileCopy(const CHAR *pszExistFile, const CHAR *pNewFile, BOOL bFailIfExists); 


    /** @} */
}

#endif




