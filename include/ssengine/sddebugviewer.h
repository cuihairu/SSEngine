#ifndef __SSCP_SDDEBUGVIEWER_IDEBUGVIEWER_H__
#define __SSCP_SDDEBUGVIEWER_IDEBUGVIEWER_H__

#include "sdbase.h"


namespace SSCP
{

	using SSSCPVersion = SSSVersion;

#ifndef WINDOWS
using WORD = UINT16;
using DWORD = unsigned long;
using UINT = unsigned int;
#endif

	const char SDDEBUGVIEWER_MODULENAME[] = "SSDebugViewer";

	const SSSCPVersion	SDDEBUGVIEWER_VERSION = {0,3,0,1};


	class ISSDebugViewer : public ISSBase
	{
	public:

		virtual	void			Register(const char* pName, INT32* pAddress) = 0;
		virtual	void			Register(const char* pName, char* pAddress) = 0;
		virtual	void			Register(const char* pName, double* pAddress) = 0;
		virtual	void			Register(const char* pName, float* pAddress) = 0;
		virtual	void			Register(const char* pName, WORD* pAddress) = 0;
		virtual	void			Register(const char* pName, DWORD* pAddress) = 0;
		virtual	void			Register(const char* pName, UINT* pAddress) = 0;

		virtual	void			UnRegister( const char* pName ) = 0;
	};


	ISSDebugViewer* SSAPI		SDDebugViewerGetModule(const SSSCPVersion* pstVersion );

}

#endif



