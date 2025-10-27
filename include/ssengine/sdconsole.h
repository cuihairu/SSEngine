/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdconsole.h  -  Console class interface definitions. This library is designed to 
				support printting debug message to console which is devided into 
				two areas, fixed area and scrolling area. The former is used to 
				display constancy information such as online number,connection 
				number and so on. The latter is used to display runtime debug message.

Class   -  ISSConsole

Remarks -  ISSConsole can only run in exclusive mode of one process, so it's inner
		   implementation has been designed in singleton mode and it's member function 
		   supported multi-threading.
******************************************************************************/

#ifndef SSCP_SDCONSOLE_H_20080125
#define SSCP_SDCONSOLE_H_20080125

#include "sdbase.h"
#include "sdlogger.h"

namespace SSCP
{
	
	// Default fixed area lines of the console screen
	const INT32 DEFAULT_MAX_FIXED_AREA_LINES = 8;
	
	// Console Module Name
	const char SDCONSOLE_MODULENAME[]        = "SSCONSOLE";

	// Version of this Module
	const SSSVersion	SDCONSOLE_VERSION    = SDCOMMON_MODULE_VERSION;


	// The idea of the SDConsole is that we divided the screen into two areas, one is the
	// Fixed Area which is used to print the fixed debug message such as on-line number, 
	// connection number etc. which you'd like to look at every time.The other is the Scroll
	// Area which is used to print the normal debug message and scrolled automatically.
	// You can press 'p' to PAUSE and the four direction keys to scroll up or down the screen.
	// |******************************************************************************|
	// |																			  |
	// |					The Fixed Area											  |
	// |   In line control mode, you should set the line index when writing string.   |
	// |																			  |
	// |   In block control mode, you should set the entry index when writing string. |
	// |   and you can press 'w' and 's' key to scroll up or down the fixed screen.   |
	// |																			  |
	// *******************************************************************************|
	// |																			  |
	// |					The Scrolling Area										  |
	// |												  ^					          |
	// |			This area will auto scroll message	  |							  |
	// |			You can press 'p' key to pause and <--|-->                        |
	// |			to scroll up or down. 			      |							  |
	// |												  v							  |
	// |																			  |
	// --------------------------------------------------------------------------------
	// [view/input] You can input commands defined yourself here. 
	// --------------------------------------------------------------------------------
	//
	// typedef Ctrl+C or Close event hook handler function
	//
	struct  ISSConsoleEventHandler
	{
		virtual ~ISSConsoleEventHandler() {}
		virtual void OnCloseEvent() = 0;
		virtual void OnUserCmdEvent(const char* szCmd) = 0;
	};

	//
	// Name		: ISSConsole
	// Function	: Display debug message on the system's console screen.	  
	// Remarks	: The inner implementation of the ISSConsole in the singleton mode,
	//			  so the user should use it as a singleton.
	//
	class ISSConsole 
	{
	public:
	    virtual ~ISSConsole()  {}

		//
		// Name		  :	Init
		// Function	  : Initialize console and start it's two inner threads: one is 
		//				used to popping debug message from the MsgQueue, the other
		//				is used to listening the keyboard.
		// @param [IN]: nMaxFixedAreaLines:Fixed Area lines number, which ranged from 0 to
		//				25, 8 for default.
		// @param [IN]: bLineControFlag: 
		//				[true] means you can only print a single line in each calling
		//				WriteStringFixed, the SDConsole will truncate the string extended
		//				the border.
		//				[false] means you have no need to take care the length limit. The 
		//				SDConsole will print the whole block you passed. In this mode the 
		//				content of fixed area maybe dynamic changed according to your passed 
		//				string contents.
		// @return	  : true: success, false: failed
		//
		virtual bool SSAPI Init(INT32 nMaxFixedAreaLines = DEFAULT_MAX_FIXED_AREA_LINES,
								bool  bLineCtrlFlag      = true) = 0;

		//
		// Name		  : Release
		// Function	  : Finalize console and stop it's two inner threads.
		// @return	  : true: success, false: failed
		//
		virtual bool SSAPI Release(void) = 0;

		//
		// Name		  : Close
		// Function	  : Close it's internal msg queue
		//
		virtual	void SSAPI CloseMsgQueue(void) = 0;		 

		//
		// Name		  : WritesString
		// Function	  : Write string on the Scroll Area of the console screen. 
		// @param[IN] : uncertain parameters
		// @return	  : writing number for success, -1 for failed 
		// Remarks	  :	The following function can be called in multi-threading context. and
		//				it's thread-safe and supported uncertain parameters.
		//
		virtual INT32 SSAPI WriteString(const char* pszFormat, ...) = 0;
		//White 0x000F
		//FOREGROUND_INTENSITY(8) | FOREGROUND_RED(4) | FOREGROUND_GREEN(2) | FOREGROUND_BLUE(1)
		virtual INT32 SSAPI WriteStringC(INT32 Color, const char* pszFormat, ...) = 0;

		//
		// Name		  : WritesStringFixed
		// Function	  : Write string on the Fixed Area of the console screen. 
		// @param[IN] : nIndex, line or block index
		// @param[IN] : uncertain parameters
		// @return	  : writing number for success, -1 for failed 
		// Remarks	  : Write string on the Fixed Area. You should set which line you want
		//				to display it. it's also a thread-safe function and	supported 
		//				uncertain parameters
		//
		virtual INT32 SSAPI WriteStringFixed(INT32 nIndex, const char* pszFormat, ...) = 0;
		virtual INT32 SSAPI WriteStringFixedC(INT32 Color, INT32 nIndex, const char* pszFormat, ...) = 0;

		//
		// Name		 : SetCtrlCEventHandleHook
		// Function  : Install a Ctrl+C event hook handler when the SDConsole received Ctrl+C
		//			   event, so you can do exit operation such as saving data, release resource 
		//			   and etc if any needed. If you didn't set this hook, when the user pressed
		//			   Ctrl+C the SDConsole won't call poHandler->OnCloseEvent();
		// @param[IN]: poHandler, Ctrl+C or CLOSE event hook handler
		//
		virtual	void SSAPI SetCtrlCHookHandler(ISSConsoleEventHandler* poHandler) = 0;

		//
		// Name		: Run
		// Function : External thread can drive this method to handle their specific commands.
		//			  In this method, the sdconsole will dequeue a user input commonds each time,
		//			  to let the external thread parsing and do their logic operation.
		//			  You should implement ISSConsoleEventHandler before calling the Run method
		//
		virtual void SSAPI Run(void) = 0;

	};

	//
	// Name		  : SSConsoleGetModule
	// Function	  : Get the pointer of ISSConsole instance
	// @param[IN] : SSCP version string
	// @return	  : ISSConsole Pointer : success, NULL: failed
	//
	ISSConsole* SSAPI SSConsoleGetModule(const SSSVersion* pstVersion);

	typedef ISSConsole* (SSAPI *PFN_SSConsoleGetModule)(const SSSVersion* pstVersion);

	//
	// Name		  : SSConsoleSetLogger
	// Function   : Set Logger for SDConsole
	// @param[IN] : poLogger, SDLogger instance pointer
	// @param[IN] : dwLevel, SDLogger level
	// @return    : true : success, false : failed
	//				
	bool SSAPI SSConsoleSetLogger(ISSLogger* poLogger, UINT32 dwLevel);

	typedef bool (SSAPI *PFN_SSConsoleSetLogger)(ISSLogger*  poLogger, UINT32 dwLevel);
};

#endif //SSCP_SDCONSOLE_H_20080125
