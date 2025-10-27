#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include "sdbase.h"
#include "sdconfig.h"
#include <stdarg.h>


namespace SSCP 
{
	// FileLogger module name
	const char SDLOGGER_MODULENAME[]        = "SSLOGGER";           
	// FileLogger version number
	const SSSVersion	SDLOGGER_VERSION    =SDCOMMON_MODULE_VERSION;  
	

#define LOGLV_NONE      0x0000
#define LOGLV_DEBUG     0x0001
#define LOGLV_INFO      (0x0001<< 1)
#define LOGLV_WARN      (0x0001<< 2)
#define LOGLV_CRITICAL  (0x0001<< 3)


	enum ELogMode
	{
		LOG_MODE_SINGLE_FILE = 0,	// Record the log in the single file
		LOG_MODE_DAY_DIVIDE,		// Record the log in different files partition by days
		LOG_MODE_MONTH_DIVIDE,		// Record the log in different files partition by month
	};

	//
	// Name     : ISSLogger
	// Function : The based log interfaces.
	//
	class ISSLogger
	{
		public:
			virtual SSAPI ~ISSLogger(){}

			//
			// Name         : LogText
			// Function     : Log information in text format.
			// Parameters   : [IN] pszLog: Pointer to the logs to be logged.
			// Return Values: LogText returns true if log success.
			//                If the pszLog is NULL, LogText returns false.
			//
			virtual bool SSAPI LogText(const char* pszLog) = 0;

			//
			// Name         : LogBinary
			// Function     : Log information in Binary format.
			// Parameters   : [IN] pLog: Pointer to the logs to be logged.
			//                [IN] dwLen: Length of the logs to be logged.
			// Return Values: LogBinary returns true if log success.
			//                Otherwise returns false.
			//
			virtual bool SSAPI LogBinary(const UINT8* pLog, UINT32 dwLen) = 0;
	};

	//File Logger class*/
	class ISSFileLogger : public ISSLogger
	{
		public:
			/*!func
			//Initialize
			!param [in] pszDir logger file directory
			!param [in] pszFileName logger file name
			!param [in] nMode logger file partition mode
			!return operation result
			*/
			virtual bool SSAPI Init(const char* pszDir, const char* pszFileName, INT32 nMode) = 0;

			virtual void SSAPI Release() = 0;
	};

	//UDP Logger class*/
	class ISSUdpLogger :  public ISSLogger
	{
		public:
			/*!func
			//Initialize
			!param [in] pszIp the IP of logger server
			!param [in] wPort the Port of logger server
			!param [in] byServerType what type server to use the NetLogger module
			!return operation result
			*/
			virtual bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 byServerType, const char* pszErrorfile = NULL) = 0;

			virtual void SSAPI Release() = 0;
	};

	// 
	// Name     : ISSTcpLogger
	// Function : This pure virtual base class is the interface of tcp logger used tcp
	// --------------------------------------------------------------------------------
	// Remark   : You need to check the return value of init function. Don't use if failed.
	//

	class ISSTcpLogger : public ISSLogger
	{
		public:
			~ISSTcpLogger()
			{

			}

			virtual bool SSAPI Init(const char* pszIp, const UINT16 wPort, UINT8 byServerType, const char* pszErrorfile = NULL) = 0;

			virtual void SSAPI Release() = 0;

	};

#ifdef SSCP_HAS_MSMQ_LOGGER

	// 
	// Name     : ISSMsmqLogger
	// Function : This pure virtual base class is the interface of msmq logger used shared memory
	// --------------------------------------------------------------------------------
	// Remark   : You need to check the return value of init function. Don't use if failed.
	//
	class ISSMsmqLogger : public ISSLogger
	{
		public:
			~ISSMsmqLogger()
			{
			}

			virtual bool SSAPI Init(const char* szQueuePath, int sendtype) = 0;

			virtual void SSAPI Release() = 0;

	};

#endif 

	/*!typedef
	  SDCreateNetLogger function type
	  !inc sdlogger.h
	  */
	typedef ISSUdpLogger* (SSAPI *PFN_SDCreateUdpLogger)(const SSSVersion* pstVersion);

	/*!func
	  Module function getting method which return the pointer of ISSUdpLogger 
	  !inc sdlogger.h
	  !param [in] the version number of target module, please passed in SDLOGGER_MODULENAME
	  !return	return the pointer of ISSUdpLogger if succeeded£¬NULL for failed
	  */
	ISSUdpLogger* SSAPI SDCreateUdpLogger(const SSSVersion* pstVersion);

	/*!typedef
	  SDCreateTcpLogger function type
	  !inc sdlogger.h
	  */
	typedef ISSTcpLogger* (SSAPI *PFN_SDCreateTcpLogger)(const SSSVersion* pstVersion);

	/*!func
	  Module function getting method which return the pointer of ISSTcpLogger 
	  !inc sdlogger.h
	  !param [in] the version number of target module, please passed in SDLOGGER_MODULENAME
	  !return	return the pointer of ISSTcpLogger if succeeded£¬NULL for failed
	  */
	ISSTcpLogger* SSAPI SDCreateTcpLogger(const SSSVersion* pstVersion);

	/*!typedef
	  SDCreateFileLogger function type
	  !inc sdlogger.h
	  */
	typedef ISSFileLogger* (SSAPI *PFN_SDCreateFileLogger)(const SSSVersion* pstVersion);

	/*!func
	  Module function getting method which return the pointer of ISSFileLogger
	  !inc sdlogger.h
	  !param [in] the version number of target module, please passed in SDLOGGER_MODULENAME
	  !return	return the pointer of ISSFileLogger if succeeded£¬NULL for failed
	  */
	ISSFileLogger* SSAPI SDCreateFileLogger(const SSSVersion* pstVersion);

#ifdef SSCP_HAS_MSMQ_LOGGER
	/*!func
	  Module function getting method which return the pointer of ISSMsmqLogger
	  !inc sdlogger.h
	  !param [in] the version number of target module, please passed in SDLOGGER_MODULENAME
	  !return	return the pointer of ISSMsmqLogger if succeeded£¬NULL for failed
	  */
	ISSMsmqLogger* SSAPI SDCreateMsmqLogger(const SSSVersion* pstVersion);

	/*!typedef
	  SDCreateMsmqLogger function type
	  !inc sdlogger.h
	  */
	typedef ISSMsmqLogger* (SSAPI *PFN_SDCreateMsmqLogger)(const SSSVersion* pstVersion);
#else 
	// for sdcommon.def 
	void * SSAPI SDCreateMsmqLogger(const SSSVersion* pstVersion);

#endif //

	class CSDLogger
	{
		public:
			CSDLogger();
			virtual ~CSDLogger();
			void SSAPI SetLogger(ISSLogger* pLogger);

			void SSAPI Critical(const char *format, ...);
			void SSAPI Info(const char *format, ...);
			void SSAPI Warn(const char *format, ...);
			void SSAPI Debug(const char *format, ...);
			void _Log(UINT32 dwLevel, const char *pszFormat, va_list argptr);

		protected:    
			const char* m_aszLogPrefix[9];
			ISSLogger* m_pLogger;
	};


}
#endif
 

