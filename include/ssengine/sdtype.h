
/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

 This file defines the basic data types used in SSCP, which including 1,2,4,8 bytes
 signed and unsigned data types. In order to avoid the size of the basic type diff-
 renent in various system such as int, the SSCP used all the data types defined here.

******************************************************************************/

#ifndef SDTYPE_H
#define SDTYPE_H

/**
* @file sdtype.h
* @author wangkui
* @brief SSCP数据类型集合
*
**/

#if defined(WIN32) || defined(WIN64)
#define WINDOWS
#elif defined(__linux__)
#if defined(__x86_64__)
#define LINUX64
#else
#define LINUX32
#endif
#define LINUX
#endif //__linux__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef WINDOWS
#include <Windows.h>
#include <BaseTsd.h>
typedef double DOUBLE;

#pragma warning(disable:4996) // suppress VS 2005 deprecated function warnings
#pragma warning(disable:4786) // for string

typedef signed long long    INT64;
typedef unsigned long long  UINT64;

#else

typedef  unsigned char     BYTE;
typedef  bool              BOOL;

#ifndef FALSE
#define FALSE			   false
#endif

#ifndef TRUE
#define TRUE               true
#endif


//
// One byte signed type
//
typedef char         CHAR;

//
// One byte unsigned type
//
typedef unsigned char       UCHAR;

//
// One byte signed type
//
typedef signed char         INT8;

//
// One byte unsigned type
//
typedef unsigned char       UINT8;

//
// Two bytes signed type
//
typedef signed short        INT16;

//
// Two bytes unsigned type
//
typedef unsigned short      UINT16;

//
// Four bytes signed type
//
typedef signed int          INT32;

//
// Four bytes unsigned type
//
typedef unsigned int        UINT32;

//
// Eight bytes signed type
//
typedef signed long long    INT64;

//
// Eight bytes unsigned type
//
typedef unsigned long long  UINT64;

//
//double type
//
typedef double  DOUBLE;

//
//float type
//
typedef float  FLOAT;


//
// Four bytes signed long
//
typedef long   LONG ;

//
// Four bytes unsigned type
//
typedef unsigned long  ULONG;


//
//void type
//
#ifndef VOID
#define VOID void
#endif

#endif //endif WIN32

#ifndef NULL
#include <cstddef>
#endif


#ifdef WINDOWS
#define SDFMT_I64         "%I64d"
#define SDFMT_U64         "%I64u"
#else
#define SDFMT_I64         "%lld"
#define SDFMT_U64         "%llu"
#endif


#ifdef WINDOWS
#define SSAPI
//#define SSAPI __stdcall
typedef HANDLE SDHANDLE ;
#define SDINVALID_HANDLE  NULL
#else
#define SSAPI
#define SDINVALID_HANDLE  0
typedef int SDHANDLE ;
#endif


#if defined(WIN64) || defined(LINUX64)
#define BITLEN	64
#endif

//#if 64 == BITLEN
//#define SD_ERR    0xFFFFFFFFFFFFFFFF
//#define SDINFINITE  0xFFFFFFFFFFFFFFFF
//#else
//#define SD_ERR    0xFFFFFFFF
//#define SDINFINITE  0xFFFFFFFF
//#endif

inline void SDCloseHandle(SDHANDLE handle)
{
#ifdef WINDOWS
    CloseHandle(handle);
#endif
}


#define SD_OK     0
#define SD_ERR    0xFFFFFFFF

#define SDINFINITE  0xFFFFFFFF

#endif




