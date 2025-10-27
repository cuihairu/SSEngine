/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sderrno.h - ����,��ȡϵͳ�����������API��װ

******************************************************************************/


#ifndef SDERRNO_H
#define SDERRNO_H
/**
* @file sderrno.h
* @author wangkui
* @brief �������
*
**/
#include "sdtype.h"
/**
* @brief Type for specifying an error or status code.
*/
namespace SSCP
{
    /**
    * @defgroup grouperror errno����
    * @ingroup  SSCP
    * @{
    */

    typedef INT32  SDErrNo;

#ifdef WINDOWS
    /**
    * @brief
    * ����errno����
    * @param dwErrCode : �����error code
    * @return void
    */
    inline void SSAPI SDSetLastError(DWORD dwErrCode)
    {
        SetLastError(dwErrCode);
    }

    /**
    * @brief
    * ��ȡerrno����
    * @return ����errno code
    */
    inline SDErrNo SSAPI SDGetLastError()
    {
        return GetLastError();
    }

    /**
    * @brief
    * ��error����ת�����ַ���
    * @param errnum : �����error code
    * @return error code��Ӧ���ַ���
    */
    inline CHAR* SSAPI SDStrError(INT32 errnum)
    {
        return strerror(errnum);
    }

    /**
    * @brief
    * �õ���������µ��ַ�������
    * @return ������ַ�������
    */
    inline CHAR* SSAPI SDLastStrError()
    {
        return _strerror(NULL);
    }
#else
    /**
    * @brief
    * ����error����
    * @param dwErrCode : �����error code
    * @return void
    */
    inline void SSAPI SDSetLastError(INT32 dwErrCode)
    {
        errno = dwErrCode;
    }

    /**
    * @brief
    * ��ȡerror����
    * @return ����error code
    */
    inline SDErrNo SSAPI SDGetLastError()
    {
        return errno;
    }

    /**
    * @brief
    * ��error����ת�����ַ���
    * @param errnum : �����error code
    * @return errno code��Ӧ���ַ���
    */
    inline CHAR* SSAPI SDStrError(INT32 errnum)
    {
        return strerror(errnum);
    }

    /**
    * @brief
    * �õ���������µ��ַ�������
    * @return ������ַ�������
    */
    inline CHAR*  SSAPI SDLastStrError()
    {
        return strerror(errno);
    }
#endif // WINDOWS 


} // namespace SSCP

#endif // 
