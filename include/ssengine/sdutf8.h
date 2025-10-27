/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/

#ifndef SDU_UTF8_H
#define SDU_UTF8_H
/**
* @file sdutil.h
* @author edeng_2000
* @brief sdu 版本信息
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
    * 打开UTF8文件
    * @param cpFilename : 文件路径
	* @param cpMode : 文件访问许可标识
    * @return 返回打开的文件句柄
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	FILE* SSAPI SDUTF8FileOpen( const CHAR* cpFilename, const CHAR* cpMode );
#else
	SDHANDLE SSAPI SDUTF8FileOpen( const CHAR* cpFilename, const CHAR* cpMode );
#endif
    /**
    * @brief
    * 关闭UTF8文件
    * @param hFile : 需要关闭的文件句柄
    * @return 关闭成功则返回TRUE，否则，FALSE
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	BOOL SSAPI SDUTF8FileClose( const FILE* hFile );
#else
	BOOL SSAPI SDUTF8FileClose( const SDHANDLE hFile );
#endif

    /**
    * @brief
    * 向UTF8文件写内容
    * @param pBuffer : 需要写入的数据指针
	* @param nSize :   单位数据所占长度
	* @param nCount :  需要写入的数据数目
	* @param hFile  :  文件句柄
    * @return 返回写入文件数据长度，当返回值为-1时，则写入失败
    */
#if ( defined ( LINUX64 ) || defined ( WIN64 ) )	
	size_t SSAPI SDUTF8FileWrite( const void* pBuffer ,size_t nSize , size_t nCount,FILE* hFile);
#else
	size_t SSAPI SDUTF8FileWrite( const void* pBuffer ,size_t nSize , size_t nCount,SDHANDLE hFile);
#endif
    /**
    * @brief
    * 判断指定字符串是否是UTF8编码
    * @param pBuffer : 需要写入的数据指针
	* @param nSize :   单位数据所占长度
    * @return 返回TRUE，则指定输入字符串为UTF8编码，否则为false
    */
	BOOL SSAPI SDIsUTF8(const void* pBuffer, UINT32 nSize) ;
}

#endif