/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/

#ifndef SDCRC_H
#define SDCRC_H
/**
* @file sdcrc.h
* @author lw
* @brief crc check
*
**/
#include "sdtype.h"
#include <string>

namespace SSCP
{
    /**
    * @defgroup groupsrc CrcУ��
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * �õ�һ�黺������Ӧ��crcУ����
    * @param pBuf : ����Ļ�����
    * @param dwLen : ����������
    * @param dwCrcVal : ������ڵ���0��С��256������λ
    * @return crcУ����
    */
    UINT32 SSAPI SDCRC(const void* pBuf, UINT32 dwLen, UINT32 dwCrcVal = 0);

    /**
    * @brief
    * �����ļ����ݵ�crcУ����,ע�����CRCʱʹ���˶����Ʒ�ʽ��ȡ,
    * �����ͬһ���ļ���crcֵ��һ��,��ȷ���ļ��Ķ�ȡ��ʽ.
    * @param pszFileName : �ļ���
    * @return �ļ����ݵ�crcУ���룬ʧ�ܷ���0
    */
    UINT32 SSAPI SDFileCRC(const CHAR* pszFileName, BOOL bFileMapping = TRUE);

    /** @} */
}

#endif
