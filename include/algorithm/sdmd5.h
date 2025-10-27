/******************************************************************************
Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/
#ifndef SDMD5_H
#define SDMD5_H
/**
* @file sdmd5.h
* @author wangkui
* @brief ����MD5�����
*
**/
#include "sdtype.h"

namespace SSCP
{

    typedef struct stMD5Context
    {
        UINT32 state[4];			/**<state (ABCD)*/
        UINT32 count[2];			/**<number of bits, modulo 2^64 (lsb first)*/
        UCHAR buffer[64];	/**<input buffer*/
    } SMD5Context;


    /**
    * @brief ��ʼ��MD5��������
    * @param pCtx : [�����������]MD5��������,����һ���µ�MD5��������
    * @return void
    */
    void SDMD5Init(SMD5Context *pCtx);

    /**
    * @brief ����MD5�룬����һ��MD5����ϢժҪ������������һ����Ϣ�飬������������
    * @param pCtx : [�����������]MD5��������
    * @param pInput : ������Ϣ��
    * @param dwInLen : ����Ϣ��ĳ���
    * @return void
    */
    void SDMD5Update(SMD5Context *pCtx, UCHAR *pInput, UINT32 dwInLen);

    /**
    * @brief ���һ��MD5����ϢժҪ������д����ϢժҪ�����������
    * @param acDigest : [�����������]���ɵ�MD5��
    * @param pCtx : [�����������]MD5��������
    * @return void
    */
    void SDMD5Final(UCHAR acDigest[16], SMD5Context *pCtx);

    /**
    * @brief
    * get md5 for a string buffer ��ȡһ���ַ�����md5��
    * @param acDigest : [in/out]array to store md5 val [�����������]�洢md5�������
    * @param pInput : input string �����ַ���
    * @param dwInLen : the length fo input string �����ַ����ĳ���
    * @return void
    */
    void SSAPI SDMD5(UCHAR acDigest[16], UCHAR* pInput, UINT32 dwInLen);


	/**
    * @brief
    * �����ļ����ݵ�md5У����,ע�����MD5ʱʹ���˶����Ʒ�ʽ��ȡ,
    * �����ͬһ���ļ���md5ֵ��һ��,��ȷ���ļ��Ķ�ȡ��ʽ.
	* @param acDigest : [�����������]�洢md5�������
    * @param pszFileName : �ļ���
	* @param bFileMapping : �Ƿ�ʹ���ļ�ӳ��
    * @return �Ƿ�ɹ���trueΪ�ɹ���falseΪ���ɹ�
    */
    BOOL SSAPI SDFileMD5(UCHAR acDigest[16], const CHAR* pszFileName, BOOL bFileMapping = TRUE);

}

#endif

