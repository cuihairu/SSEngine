/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdprocess.h - ���̴���

******************************************************************************/

#ifndef SDPROCESS_H
#define SDPROCESS_H
/**
* @file sdprocess.h
* @author lw
* @brief ���̴���
*
**/
#include "sdtype.h"
#include <string>
#ifdef WINDOWS
#include "Aclapi.h"
#endif // 


namespace SSCP
{
    /**
    * @defgroup groupthread ����̶��߳�(�����̳߳�)
    * @ingroup  SSCP
    * @{
    */
#ifdef WINDOWS
	typedef struct tagProcessSecInfo 
	{
		BOOL bSet; // �Ƿ����ô˰�ȫ��Ϣ
		SE_OBJECT_TYPE objectType;
		SECURITY_INFORMATION securityInfo;
		PSID psidOwner;
		PSID psidGroup;
		PACL pDacl;
		PACL pSacl;
		PSECURITY_DESCRIPTOR pSecurityDescriptor; // ��ȡʱʹ��
	} SProcessSecInfo; 
#endif 

    struct SProcessAttr
    {
        std::string  workDir;  //����ִ�еĹ���Ŀ¼
        std::string  environment;
        UINT32       createFlags;  // ���̴�����ʶ fow windows.
        BOOL         inherithandle;// for Windows
#ifdef WINDOWS
		SProcessAttr()
		{
			secInfo.bSet = FALSE; 
		}
		SProcessSecInfo secInfo; 
#endif // 
    };


#ifdef WINDOWS
typedef UINT32 SDPROCESSID; 
#else 
typedef pid_t  SDPROCESSID; 
#endif // 

    /**
    * @brief
    * ����һ������
    * @param progName : �ý��̵�ִ���ļ�
    * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
    */
    SDHANDLE SSAPI SDCreateProcess(
        const CHAR *progName,
        const CHAR *pCmdLine = NULL,
        const CHAR *pWorkDir = NULL,
        SProcessAttr *pAttr = NULL);

    /**
    * @brief
    * ��ֹһ������
    * @param handle : ���̾��
    * @param err  :ָ�������˳�ʱ�Ĵ�������Ϣ
    * @return void
    */
    void SSAPI SDTerminateProcess(SDHANDLE handle, INT32 err = 0);

    /**
    * @brief
    * �رս��̵ľ��,���ڽ��̲�Ҫʹ��ͨ�õ�SDCloseHandle���رվ��,����ᵼ����Դδ�ͷ�
    * @param handle : ���̾��
    * @return void
    */
    void SSAPI SDCloseProcess(SDHANDLE handle);

	/**
	* @brief
	* ��ȡ����ID�����̱�����������
	* @param handle : ���̾��
	* @return void
	*/
	SDPROCESSID SSAPI SDGetProcessId(SDHANDLE handle); 

	/**
	* @brief
	* ͨ������ID��ȡ����handle��
	* @param pid: ����ʵ��ID
	* @return void
	*/
	SDHANDLE SSAPI SDOpenProcess(SDPROCESSID pid); 

	/**
	* @brief
	* ��ȡ��ǰ�Ľ���
	* @param handle : ����,�߳̾��
	* @return void
	*/
	SDHANDLE SSAPI SDGetCurrentProcess(); 

	/**
	* @brief
	* ��ȡ��ǰ�Ľ���id
	* @param handle : ����,�߳̾��
	* @return void
	*/
	SDPROCESSID SSAPI SDGetCurrentProcessId(); 

    /**
    * @brief
    * �ȴ�������ֹ
    * @param handle : ���̾��
    * @return void
    */
    void SSAPI SDWaitProcess(SDHANDLE handle);

	/**
	* @brief
	* ���ý�������
	* @param handle : ���̾��
	* @return ���ý��
	*/
    BOOL SSAPI SDSetProcessAttr(SDHANDLE handle, const SProcessAttr &pAttr);

	/**
	* @brief
	* ��ȡ��������
	* @param handle : ���̾��
	* @param procAttr : ��������
	* @return ��ȡ�Ƿ�ɹ�
	*/
	BOOL SSAPI SDGetProcessAttr(SDHANDLE handle, SProcessAttr & procAttr); 

    /**
    * @brief ���̲�����
    */
    class CSDProcess
    {
    public:

        CSDProcess();
        ~CSDProcess();

        /**
        * @brief
        * ��������
        * @param path : �ý��̵�ִ���ļ�
        * @return ����ɹ�����TRUE, ʧ�ܷ���FALSE
        */
        BOOL SSAPI Create(
            const CHAR *pProgName,
            const CHAR *pArgs = NULL,
            const CHAR *pEnvs = NULL,
            SProcessAttr *pAttr = NULL);

        /**
        * @brief
        * ��ֹ����
        * @return void
        */
        void SSAPI Terminate();

        /**
        * @brief
        * �ȴ�������ֹ����ִ�н���
        * @return void
        */
        void SSAPI Wait();

		/**
		* @brief
		* ��ȡ����ID
		* @return SDHANDLE
		*/
		UINT32 SSAPI GetProcessID(); 

        /**
        * @brief
        * ��ȡ���̾��
        * @return SDHANDLE
        */
        SDHANDLE SSAPI GetHandle();

        /**
        * @brief
        * ���ý�������
        * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
        */
        BOOL SSAPI SetAttribute(const SProcessAttr &pAttr);

        /**
        * @brief
        * ��ȡ��������
        * @return ���صĽ������Խṹ��
        */
        BOOL SSAPI GetAttribute(SDHANDLE handle, SProcessAttr & procAttr);

    private:
        CSDProcess (const CSDProcess&);
        CSDProcess& operator = (const CSDProcess&);
        SDHANDLE m_handle;
    };

    /** @} */
}

#endif

