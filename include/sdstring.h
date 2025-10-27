/******************************************************************************
Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/


#ifndef SDSTRING_H
#define SDSTRING_H
/**
* @file sdstring.h
* @author lw
* @brief string处理系列
*
**/
#include "sdtype.h"
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <vector>


/**
* @file sdstring.h
* @author lw
* @brief some function for string process
*
**/

#if defined(WINDOWS)
#define SDSYMBOL_RT  "\r\n"
#else
#include <wchar.h>
#include <wctype.h>
#define SDSYMBOL_RT  "\n"
#endif

namespace SSCP
{

    /**
    * @defgroup groupstring 字符串处理
    * @brief 字符串处理不做边界检测
    * @ingroup  SSCP
    * @{
    */

    /** @see isalnum */
#define SDIsAlNum(c) (isalnum(((UCHAR)(c))))
    /** @see isalpha */
#define SDIsAlpha(c) (isalpha(((UCHAR)(c))))
    /** @see iscntrl */
#define SDIsCntrl(c) (iscntrl(((UCHAR)(c))))
    /** @see isdigit */
#define SDIsDigit(c) (isdigit(((UCHAR)(c))))
    /** @see isgraph */
#define SDISGraph (c) (isgraph(((UCHAR)(c))))
    /** @see islower*/
#define SDIsLower(c) (islower(((UCHAR)(c))))
    /** @see isascii */
#ifdef isascii
#define SDIsAscii(c) (isascii(((UCHAR)(c))))
#else
#define SDIsAscii(c) (((c) & ~0x7f)==0)
#endif
    /** @see isprint */
#define SDIsPrint(c) (isprint(((UCHAR)(c))))
    /** @see ispunct */
#define SDIsPunct(c) (ispunct(((UCHAR)(c))))
    /** @see isspace */
#define SDIsSpace(c) (isspace(((UCHAR)(c))))
    /** @see isupper */
#define SDIsUpper(c) (isupper(((UCHAR)(c))))
    /** @see isxdigit */
#define SDIsXdigit(c) (isxdigit(((UCHAR)(c))))
    /** @see tolower */
#define SDToLower(c) (tolower(((UCHAR)(c))))
    /** @see toupper */
#define SDToUpper(c) (toupper(((UCHAR)(c))))


    /**
    * @brief
    * 将字符串开始和结束处设置为0
    * @param string : 以0为结束符的字符串
    * @return:void
    */
    template <class T>
    inline void ZeroString(T& Destination) throw()
    {
#if  defined(WINDOWS)
        (static_cast<CHAR[sizeof(Destination)]>(Destination));
#endif
        Destination[0] = '\0';
        Destination[sizeof(Destination)-1] = '\0';
    }


    /**
    * @brief
    * 获取字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : 字符串的长度
    */
    inline UINT32 SSAPI SDStrlen( const CHAR *string )
    {
        return (UINT32)strlen(string);
    }

    /**
    * @brief
    * 获取Unicode字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : Unicode字符串的长度
    */
    inline UINT32 SSAPI SDWcslen( const wchar_t *string )
    {
        return (UINT32)wcslen(string);
    }

    /**
    * @brief
    * 获取字符串的长度
    * @param pszStr : 字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取字符串的长度，如果字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline UINT32 SSAPI SDStrnlen(const CHAR *pszStr, UINT32 sizeInBytes )
    {
        return (UINT32)strnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * 获取Unicode字符串的长度
    * @param pszStr : Unicode字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取Unicode字符串的长度，如果Unicode字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline UINT32 SSAPI SDWcsnlen(const wchar_t *pszStr, size_t sizeInBytes)
    {
        return (UINT32)wcsnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline CHAR* SSAPI SDStrcat(CHAR *strDestination, const CHAR *strSource )
    {
        return strcat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param dstLen : 目标串内存的总长度,如果合并后的长度大于此长度,不会进行合并,返回空串,不修改目标串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间,如果合并后的长度大于此长度,不会进行合并,返回NULL,不修改目标串
    */
    inline CHAR * SSAPI SDStrSafeCat(CHAR * strDestination, UINT32 dstLen, const CHAR * strSource)
    {
        if (dstLen < strlen(strDestination) + strlen(strSource) + 1)
        {
            return NULL;
        }
        return strcat(strDestination,strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串数组
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间
    */
    template <size_t len>
    CHAR* SSAPI SDStrSafeCat( CHAR (&strDestination)[len], const CHAR *strSource )
    {
        return SDStrSafeCat(strDestination, len, strSource);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 以0为结束符的源Unicode字符串
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline wchar_t*  SSAPI SDWcscat(wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 源字符串
    * @param len : 源字符串的最大添加长度
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline CHAR* SSAPI SDStrncat(CHAR *strDestination, const CHAR *strSource, UINT32 len)
    {
        return strncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 源Unicode字符串
    * @param len : 源Unicode字符串的最大添加长度
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline wchar_t* SSAPI SDWcsncat(wchar_t *strDestination, const wchar_t *strSource, UINT32 len)
    {
        return wcsncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0为结束符的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0为结束符的源字符串
    * @return 目标字符串
    */
    inline CHAR* SSAPI SDStrcpy( CHAR *strDestination, const CHAR *strSource )
    {
        return strcpy(strDestination, strSource);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标缓冲区
    * @param numberOfElements : 目标字符串最大长度
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    inline CHAR* SDStrSafeCpy( char *strDestination, size_t numberOfElements, const char *strSource )
    {
        if (numberOfElements  < strlen(strSource) +1)
        {
            return NULL;
        }

        return strncpy(strDestination,strSource,numberOfElements);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标字符串数组
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回空串
    */
    template <size_t len>
    CHAR* SSAPI SDStrSafeCpy( CHAR (&strDestination)[len], const CHAR *strSource )
    {
        return SDStrSafeCpy(strDestination,len,strSource);
    }

    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0结尾的源字符串
    * @return 目标字符串
    */
    inline wchar_t* SSAPI SDWcscpy( wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscpy(strDestination, strSource);
    }

    /**
    * @brief
    * 安全拷贝Unicode字符串
    * @param strDestination : 需要拷贝Unicode字符串的目标缓冲区
    * @param numberOfElements : 目标字Unicode符串最大长度
    * @param strSource : 以0结尾的源Unicode字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    inline wchar_t * SDWcsSafeCpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource )
    {
        if (numberOfElements  < wcslen(strSource) +1)
        {
            return NULL;
        }

        return wcsncpy(strDestination,strSource,numberOfElements);
    }

    /**
    * @brief
    * 安全拷贝Unicode字符串
    * @param strDestination : 需要拷贝Unicode字符串的目标字符串数组
    * @param strSource : 以0结尾的源Unicode字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回空串
    */
    template <size_t len>
    inline wchar_t* SSAPI SDWcsSafecpy( wchar_t (&strDestination)[len], const wchar_t *strSource )
    {
        return SDWcsSafecpy(strDestination, len, strSource);
    }

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0结尾的目标字符串
    * @param strSource : 以0结尾的源字符串
    * @param len : 最大拷贝长度
    * @return 返回目标字符串
    */
    inline CHAR* SSAPI SDStrncpy( CHAR *strDestination, const CHAR *strSource, UINT32 len )
    {
        return strncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标Unicode字符串
    * @param strSource : 以0结尾的源Unicode字符串
    * @param len : 最大拷贝长度
    * @return 返回目标Unicode字符串
    */
    inline wchar_t* SSAPI SDWcsncpy( wchar_t *strDestination, const wchar_t *strSource, UINT32 len )
    {
        return wcsncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDStrcmp( const CHAR *string1, const CHAR *string2 )
    {
        return strcmp(string1, string2);
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDWcscmp( const wchar_t *string1, const wchar_t *string2 )
    {
        return wcscmp(string1, string2);
    }

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDStrncmp( const CHAR *string1, const CHAR *string2, UINT32 count )
    {
        return strncmp( string1, string2, count );
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDWcsncmp( const wchar_t *string1, const wchar_t *string2, size_t count )
    {
        return wcsncmp( string1, string2, count );
    }

    /**
    * @brief
    * 在一个字符串中查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline CHAR* SSAPI SDStrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strchr(string, c);
    }

    /**
    * @brief
    * 在一个Unicode字符串中查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline wchar_t* SSAPI SDWcschr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcschr(string, c);
    }

    /**
    * @brief
    * 在一个字符串中逆向查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    inline CHAR* SSAPI SDStrrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strrchr(string ,c);
    }

    /**
    * @brief
    * 在一个Unicode字符串中逆向查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    inline wchar_t* SSAPI SDWcsrchr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcsrchr(string, c);
    }

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDStrcasecmp(const CHAR *s1, const CHAR *s2)
    {
#if defined(WINDOWS)
        return _stricmp(s1, s2);
#else
        return strcasecmp(s1, s2);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDStrncasecmp(const CHAR *s1, const CHAR *s2, UINT32 count)
    {
#if defined(WINDOWS)
        return _strnicmp(s1, s2, count);
#else
        return strncasecmp(s1, s2, count);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDWcscasecmp(const wchar_t *s1, const wchar_t *s2)
    {
#if defined(WINDOWS)
        return _wcsicmp(s1, s2);
#else
        return wcscasecmp(s1, s2);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32 SSAPI SDWcsncasecmp(const wchar_t *s1, const wchar_t *s2, UINT32 count)
    {
#if defined(WINDOWS)
        return _wcsnicmp(s1, s2, count);
#else
        return wcsncasecmp(s1, s2, count);
#endif
    }

    /**
    * @brief
    * 将某个字符串切分为多个字符串
    * @param pszStr : 第一次输入被切分的字符串，之后输入NULL
    * @param delim : 分割字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子字符串，如果没有新的子字符串，返回NULL
    */
    inline CHAR* SSAPI SDStrtok(CHAR *pszStr, const CHAR *delim)
    {
        return strtok(pszStr, delim);
    }

    /**
    * @brief
    * 将某个Unicode字符串切分为多个Unicode字符串
    * @param pszStr : 第一次输入被切分的Unicode字符串，之后输入NULL
    * @param delim : 分割Unicode字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子Unicode字符串，如果没有新的子Unicode字符串，返回NULL
    */
    inline wchar_t* SSAPI SDWcstok(wchar_t *pszStr, const wchar_t *delim)
    {
#if defined(WINDOWS)
        return wcstok(pszStr, delim);
#else
        return wcstok(pszStr, delim, NULL);
#endif
    }


    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为小写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    CHAR* SSAPI SDStrlwr(CHAR* pszStr);


    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为大写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    CHAR* SSAPI SDStrupr(CHAR* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为小写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    wchar_t* SSAPI SDWcslwr(wchar_t* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为大写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    wchar_t* SSAPI SDWcsupr(wchar_t* pszStr);

    /**
    * @brief
    * 将MultiBytes的字符串转化为Unicode字符串
    * @param dest : 以0结尾的MultiBytes的字符串
    * @param src : 存放转化后的Unicode字符串
    * @param n : src的最大空间长度
    * @return 转化后的Unicode的实际长度
    */
    inline UINT32 SSAPI SDMbsToWcs(wchar_t *dest, const CHAR *src, UINT32 n)
    {
#if defined(WINDOWS)
        return MultiByteToWideChar(CP_ACP, 0, src, n, dest, n);
#else
        return mbstowcs(dest, src, n);
#endif
    }

    /**
    * @brief
    * 将Unicode的字符串转化为MultiBytes字符串
    * @param dest : 以0结尾的Unicode的字符串
    * @param src : 存放转化后的MultiBytes字符串
    * @param n : src的最大空间长度
    * @return 转化后的MultiBytes的实际长度
    */
    inline UINT32 SSAPI SDWcsToMbs(CHAR *dest, const wchar_t *src, UINT32 n)
    {
#if defined(WINDOWS)
        return WideCharToMultiByte(CP_ACP, 0, src, n, dest, n, NULL, NULL);
#else
        return wcstombs(dest, src, n);
#endif
    }

///////////////////////////////////////////////////////////////////////////
#define SDSprintf sprintf
#define SDSwprintf swprintf

#if defined(WINDOWS)
#define SDSnprintf _snprintf
#else
#define SDSnprintf snprintf
#endif

#if defined(WINDOWS)
#define SDSnwprintf _snwprintf
#else
#define SDSnwprintf swprintf
#endif


///////////////////////////////////////////////////////////////////////
#define SDAtoi atoi

#if defined(WINDOWS)
#define SDAtoi64 _atoi64
#else
#define SDAtoi64 atoll
#endif

	/**
	* @brief
	* 将字符串转换为32位无符号数字
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    UINT32 SSAPI SDAtou(const CHAR* pStr);

	/**
	* @brief
	* 将字符串转换为64位无符号数字
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	UINT64 SSAPI SDAtou64(const CHAR* pStr); 

    /**
    * @brief
    * 将一个数字转化为字符串
    * @param pBuf : 转化后的字符存储空间
    * @param buflen : 字符存储空间的最大长度
    * @param dwNum : 将被转化的数字
    * @return 转化后字符串使用的空间长度
    */
    inline INT32 SSAPI SDItoa(CHAR* pBuf, UINT32 buflen, INT32 dwNum)
    {
        return SDSnprintf(pBuf, buflen, "%d", dwNum);
    }

	/**
	* @brief
	* 将一个数字转化为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    template <UINT32 bufLen>
    inline INT32 SSAPI SDItoa(CHAR (&buf)[bufLen],INT32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%d", dwNum);
    }

	/**
	* @brief
	* 将有64位符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline INT32 SSAPI SDItoa64(CHAR *pBuf, UINT32 buflen, INT64 dqNum)
	{
		return SDSnprintf(pBuf, buflen, SDFMT_I64 , dqNum);
	}

	/**
	* @brief
	* 将无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline INT32 SSAPI SDUtoa(CHAR *pBuf, UINT32 buflen, UINT32 dwNum)
	{
		return SDSnprintf(pBuf, buflen, "%u", dwNum);
	}

	/**
	* @brief
	* 将无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    template <UINT32 bufLen>
    inline INT32 SSAPI SDUtoa(CHAR (&buf)[bufLen],INT32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%u", dwNum);
    }

	/**
	* @brief
	* 将有64位无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	template <UINT32 bufLen>
	inline INT32 SSAPI SDUtoa64(CHAR (&buf)[bufLen], UINT64 dqNum)
	{
		return SDSnprintf(buf, bufLen, SDFMT_U64, dqNum);
	}


    /**
    * @brief
    * 将字符串转化为浮点数
    * @param nptr : 将被转化的以0结尾的字符串
    * @param endptr : [输出参数]若不为NULL，则遇到无法转化的字符从这里返回其指针
    * @return 获取的浮点数
    */
    inline DOUBLE SSAPI SDStrtod(const CHAR *nptr, CHAR **endptr)
    {
        return strtod(nptr, endptr);
    }

    /**
    * @brief
    * split a string将字符串分割为一个字符串的vector
    * @param sSource : 被分割的字符串
    * @param delim : 分隔符，如:“:”，“,”，“ ”等
    * @return a vector to store strings splited from sSource
    */
    std::vector<std::string> SSAPI SDSplitStrings(const std::string &sSource, CHAR delim);

    /**
    * @brief
    * 将字符串转化为大写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string SSAPI SDStrupr(std::string &pszStr);

    /**
    * @brief
    * 将字符串转化为小写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string SSAPI SDStrlwr(std::string &pszStr);

    /**
    * @brief
    * 将一个字符串置空
    * @param Destination : 需要置空的类型
    * @return void
    */
    template <class T>
    inline void SSAPI SDZeroString(T &Destination) throw()
    {
      //  (static_cast<CHAR[sizeof(Destination)]>(Destination));
        Destination[0] = '\0';
        Destination[sizeof(Destination)-1] = '\0';
    }


    /**
    * @brief
    * 将二进制Buffer转换为字符串,
    * @param pBuf    二进制Buffer指针
    * @param bufLen  二进制Buffer长度
    * @param pSplitter 输出的二进制数据间隔符
	* @param lineLen 源字符串分割多行的每行长度,该长度不包含分隔符的长度
    * @return void
    */
    std::string SSAPI SDBufferToHex(const CHAR *pBuf, UINT32 bufLen, const CHAR *pSplitter = "", INT32 lineLen = -1);


    std::string SSAPI SDIntToHex(INT32 dwNum);

    /**
    * @brief
    * 将str左边的ch去掉，原字符串将被修改
    * @param pszStr 去掉左边的ch字符
    * @param ch 被去掉的字符
    * @return 去掉字符ch后的字符串
    */
    std::string SSAPI SDStrTrimLeft(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

    /**
    * @brief
    * 将str右边的ch去掉，原字符串将被修改
    * @param pszStr 去掉右边的ch字符
    * @param ch 被去掉的字符
    * @return 去掉字符ch后的字符串
    */
    std::string SSAPI SDStrTrimRight(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

    /**
    * @brief
    * 将str左右两边的ch去掉，原字符串将被修改
    * @param pszStr 去掉左右两边的ch字符
    * @param ch 被去掉的字符
    * @return 去掉字符ch后的字符串
    */
    std::string SSAPI SDStrTrim(std::string &pszStr, const CHAR* pTrimStr = " \r\t\n");

    /** @} */

}//namespace SSCP

#endif

