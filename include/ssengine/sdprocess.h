/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdprocess.h - 进程处理

******************************************************************************/

#ifndef SDPROCESS_H
#define SDPROCESS_H
/**
* @file sdprocess.h
* @author lw
* @brief 进程处理
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
    * @defgroup groupthread 多进程多线程(包括线程池)
    * @ingroup  SSCP
    * @{
    */
#ifdef WINDOWS
	typedef struct tagProcessSecInfo 
	{
		BOOL bSet; // 是否设置此安全信息
		SE_OBJECT_TYPE objectType;
		SECURITY_INFORMATION securityInfo;
		PSID psidOwner;
		PSID psidGroup;
		PACL pDacl;
		PACL pSacl;
		PSECURITY_DESCRIPTOR pSecurityDescriptor; // 获取时使用
	} SProcessSecInfo; 
#endif 

    struct SProcessAttr
    {
        std::string  workDir;  //进程执行的工作目录
        std::string  environment;
        UINT32       createFlags;  // 进程创建标识 fow windows.
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
    * 创建一个进程
    * @param progName : 该进程的执行文件
    * @return 成功返回TRUE，失败返回FALSE
    */
    SDHANDLE SSAPI SDCreateProcess(
        const CHAR *progName,
        const CHAR *pCmdLine = NULL,
        const CHAR *pWorkDir = NULL,
        SProcessAttr *pAttr = NULL);

    /**
    * @brief
    * 终止一个进程
    * @param handle : 进程句柄
    * @param err  :指定进程退出时的错误码信息
    * @return void
    */
    void SSAPI SDTerminateProcess(SDHANDLE handle, INT32 err = 0);

    /**
    * @brief
    * 关闭进程的句柄,对于进程不要使用通用的SDCloseHandle来关闭句柄,否则会导致资源未释放
    * @param handle : 进程句柄
    * @return void
    */
    void SSAPI SDCloseProcess(SDHANDLE handle);

	/**
	* @brief
	* 获取进程ID，进程必须设置属性
	* @param handle : 进程句柄
	* @return void
	*/
	SDPROCESSID SSAPI SDGetProcessId(SDHANDLE handle); 

	/**
	* @brief
	* 通过进程ID获取进程handle，
	* @param pid: 进程实际ID
	* @return void
	*/
	SDHANDLE SSAPI SDOpenProcess(SDPROCESSID pid); 

	/**
	* @brief
	* 获取当前的进程
	* @param handle : 进程,线程句柄
	* @return void
	*/
	SDHANDLE SSAPI SDGetCurrentProcess(); 

	/**
	* @brief
	* 获取当前的进程id
	* @param handle : 进程,线程句柄
	* @return void
	*/
	SDPROCESSID SSAPI SDGetCurrentProcessId(); 

    /**
    * @brief
    * 等待进程终止
    * @param handle : 进程句柄
    * @return void
    */
    void SSAPI SDWaitProcess(SDHANDLE handle);

	/**
	* @brief
	* 设置进程属性
	* @param handle : 进程句柄
	* @return 设置结果
	*/
    BOOL SSAPI SDSetProcessAttr(SDHANDLE handle, const SProcessAttr &pAttr);

	/**
	* @brief
	* 获取进程属性
	* @param handle : 进程句柄
	* @param procAttr : 进程属性
	* @return 获取是否成功
	*/
	BOOL SSAPI SDGetProcessAttr(SDHANDLE handle, SProcessAttr & procAttr); 

    /**
    * @brief 进程操作类
    */
    class CSDProcess
    {
    public:

        CSDProcess();
        ~CSDProcess();

        /**
        * @brief
        * 创建进程
        * @param path : 该进程的执行文件
        * @return 如果成功返回TRUE, 失败返回FALSE
        */
        BOOL SSAPI Create(
            const CHAR *pProgName,
            const CHAR *pArgs = NULL,
            const CHAR *pEnvs = NULL,
            SProcessAttr *pAttr = NULL);

        /**
        * @brief
        * 终止进程
        * @return void
        */
        void SSAPI Terminate();

        /**
        * @brief
        * 等待进程终止或者执行结束
        * @return void
        */
        void SSAPI Wait();

		/**
		* @brief
		* 获取进程ID
		* @return SDHANDLE
		*/
		UINT32 SSAPI GetProcessID(); 

        /**
        * @brief
        * 获取进程句柄
        * @return SDHANDLE
        */
        SDHANDLE SSAPI GetHandle();

        /**
        * @brief
        * 设置进程属性
        * @return 成功返回TRUE，失败返回FALSE
        */
        BOOL SSAPI SetAttribute(const SProcessAttr &pAttr);

        /**
        * @brief
        * 获取进程属性
        * @return 返回的进程属性结构体
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

