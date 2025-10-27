/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/

#ifndef SDU_UTF8_H
#define SDU_UTF8_H
/**
* @file sdutil.h
* @author edeng_2000
* @brief sdu �汾��Ϣ
*
**/

#include "sdmacros.h"
#include "sdtype.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
namespace SSCP
{
    /**
    * @brief
    * ��UTF8�ļ�
    * @param cpFilename : �ļ�·��
	* @param cpMode : �ļ�������ɱ�ʶ
    * @return ���ش򿪵��ļ����
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	FILE* SSAPI SDUTF8FileOpen( const CHAR* cpFilename, const CHAR* cpMode );
#else
	SDHANDLE SSAPI SDUTF8FileOpen( const CHAR* cpFilename, const CHAR* cpMode );
#endif
    /**
    * @brief
    * �ر�UTF8�ļ�
    * @param hFile : ��Ҫ�رյ��ļ����
    * @return �رճɹ��򷵻�TRUE������FALSE
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	BOOL SSAPI SDUTF8FileClose( const FILE* hFile );
#else
	BOOL SSAPI SDUTF8FileClose( const SDHANDLE hFile );
#endif

    /**
    * @brief
    * ��UTF8�ļ�д����
    * @param pBuffer : ��Ҫд�������ָ��
	* @param nSize :   ��λ������ռ����
	* @param nCount :  ��Ҫд���������Ŀ
	* @param hFile  :  �ļ����
    * @return ����д���ļ����ݳ��ȣ�������ֵΪ-1ʱ����д��ʧ��
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	size_t SSAPI SDUTF8FileWrite( const void* pBuffer ,size_t nSize , size_t nCount,FILE* hFile);
#else
	size_t SSAPI SDUTF8FileWrite( const void* pBuffer ,size_t nSize , size_t nCount,SDHANDLE hFile);
#endif
    /**
    * @brief
    * �ж�ָ���ַ����Ƿ���UTF8����
    * @param pBuffer : ��Ҫд�������ָ��
	* @param nSize :   ��λ������ռ����
    * @return ����TRUE����ָ�������ַ���ΪUTF8���룬����Ϊfalse
    */
	BOOL SSAPI SDIsUTF8(const void* pBuffer, UINT32 nSize) ;
}

#endif