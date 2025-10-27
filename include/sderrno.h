/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sderrno.h - 设置,获取系统错误码操作的API封装

******************************************************************************/


#ifndef SDERRNO_H
#define SDERRNO_H
/**
* @file sderrno.h
* @author wangkui
* @brief 错误代码
*
**/
#include "sdtype.h"
/**
* @brief Type for specifying an error or status code.
*/
namespace SSCP
{
    /**
    * @defgroup grouperror errno操作
    * @ingroup  SSCP
    * @{
    */

    typedef INT32  SDErrNo;

#ifdef WINDOWS
    /**
    * @brief
    * 设置errno变量
    * @param dwErrCode : 传入的error code
    * @return void
    */
    inline void SSAPI SDSetLastError(DWORD dwErrCode)
    {
        SetLastError(dwErrCode);
    }

    /**
    * @brief
    * 获取errno变量
    * @return 返回errno code
    */
    inline SDErrNo SSAPI SDGetLastError()
    {
        return GetLastError();
    }

    /**
    * @brief
    * 将error变量转换成字符串
    * @param errnum : 传入的error code
    * @return error code对应的字符串
    */
    inline CHAR* SSAPI SDStrError(INT32 errnum)
    {
        return strerror(errnum);
    }

    /**
    * @brief
    * 得到错误情况下的字符串描述
    * @return 错误的字符串描述
    */
    inline CHAR* SSAPI SDLastStrError()
    {
        return _strerror(NULL);
    }
#else
    /**
    * @brief
    * 设置error变量
    * @param dwErrCode : 传入的error code
    * @return void
    */
    inline void SSAPI SDSetLastError(INT32 dwErrCode)
    {
        errno = dwErrCode;
    }

    /**
    * @brief
    * 获取error变量
    * @return 返回error code
    */
    inline SDErrNo SSAPI SDGetLastError()
    {
        return errno;
    }

    /**
    * @brief
    * 将error变量转换成字符串
    * @param errnum : 传入的error code
    * @return errno code对应的字符串
    */
    inline CHAR* SSAPI SDStrError(INT32 errnum)
    {
        return strerror(errnum);
    }

    /**
    * @brief
    * 得到错误情况下的字符串描述
    * @return 错误的字符串描述
    */
    inline CHAR*  SSAPI SDLastStrError()
    {
        return strerror(errno);
    }
#endif // WINDOWS 


} // namespace SSCP

#endif // 
