/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

 sdbase.h - The base class and base log class define and some structure define
            of the whole namespace SSCP.

******************************************************************************/

#ifndef SDBASE_H
#define SDBASE_H
/**
* @file sdbase.h
* @author wangkui
* @brief SSCP»ù´¡Àà
*  
**/
#include "sdtype.h"

namespace SSCP {

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
struct SSSVersion
{
	UINT16 wMajorVersion;
	UINT16 wMinorVersion;
	UINT16 wCompatibleVersion;
	UINT16 wBuildNumber;
};


const SSSVersion SSCP_VERSION = { 3, 0, 1, 2 };

const SSSVersion SDCOMMON_MODULE_VERSION = { 5, 0, 0, 1 }; 
//
// Name     : ISSBase
// Function : The base class of modules in namespace SSCP.
//            Every module that exports from DLL was inherited from this class.
//
class ISSBase
{
public:
    virtual SSAPI ~ISSBase(void){}

    //
    // Name     : AddRef
    // Function : Add the reference of the module instance.
    //            This method should be called every time the CreateInstance
    //            method of the module was called.
    //
    virtual void SSAPI AddRef(void) = 0;

    //
    // Name         : QueryRef
    // Function     : Query the reference time of the module instance.
    // Return Values: QueryRef returns the reference number of the instance.
    //
    virtual UINT32  SSAPI QueryRef(void) = 0;

    //
    // Name     : Release
    // Function : Decrease the reference number of the module by 1. When 
    //            decrease to zero, the instance of the module will be destroyed
    //
    virtual void SSAPI Release(void) = 0;

    //
    // Name         : GetVersion
    // Function     : Get the version information of the module.
    // Return Values: GetVersion returns the version structure of the module.
    //
    virtual SSSVersion SSAPI GetVersion(void) = 0;

    //
    // Name         : GetModuleName
    // Function     : Get the name of the module.
    // Return Values: GetModuleName returns the name string of the module.
    //
    virtual const char * SSAPI GetModuleName(void) = 0;
};



}

#endif