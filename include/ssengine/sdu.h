/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/

#ifndef SDU_H
#define SDU_H
/**
* @file sdu.h
* @author wk
* @brief sdu ∞Ê±æ–≈œ¢
*
**/
#include "sdtype.h"
#include "sdnetutils.h"
#include "sdatomic.h"
#include "sdcondition.h"
#include "sdconfigure.h"
#include "sddataqueue.h"
#include "sddatastream.h"
#include "sddebug.h"
#include "sddir.h"
#include "sderrno.h"
#include "sdfifo.h"
#include "sdfile.h"
#include "sdfilemapping.h"
#include "sdhashmap.h"
#include "sdindexer.h"
#include "sdlimit.h"
#include "sdlock.h"
#include "sdloopbuffer.h"
#include "sdmath.h"
#include "sdmemorypool.h"
#include "sdmutex.h"
#include "sdobjectpool.h"
#include "sdprocess.h"
#include "sdrandom.h"
#include "sdregex.h"
#include "sdserverid.h"
#include "sdshmem.h"
#include "sdsingleton.h"
#include "sdstring.h"
#include "sdthread.h"
#include "sdthreadpool.h"
#include "sdtime.h"
#include "sdtimer.h"
#include "sdutil.h"
//
// Version structure define of namespace SSCP.
// Every module of SSCP has its own version information.Make sure to get the
// right version DLL and Head file.Otherwise there may be some exceptions
// while using the modules.
// SSCP version information has the following part:
// Major Version     :This field will increase by 1 when the module was changed
//                    from beta version to final version.
// Minor Version     :This field will increase by 1 every time the module's
//                    interface changed and conflicted with the last version.
// Compatible Version:This field will increase by 1 every time the module's
//                    interface changed but compatible with the last version.
// Build Number      :This field will increase by 1 every official build.And
//                    never back to zero.
//
struct SSDUVersion
{
    UINT16 wMajorVersion;
    UINT16 wMinorVersion;
    UINT16 wCompatibleVersion;
    UINT16 wBuildNumber;
};

namespace SSCP
{
    const SSDUVersion SDU_VERSION = { 1, 0, 0, 2};

    inline SSDUVersion SDUGetVersion()
    {
        return SDU_VERSION;
    }

	inline std::string SDUGetVersionStr()
	{
		static CHAR szVerStr[128] = {0}; 
		SDSprintf(szVerStr,"SDU v%d-%d-%d-%d",SDU_VERSION.wMajorVersion,
			SDU_VERSION.wMinorVersion,
			SDU_VERSION.wCompatibleVersion,
			SDU_VERSION.wBuildNumber); 
		return szVerStr; 
	}

}//end namespace

#endif
