/******************************************************************************
Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/


#ifndef SDSTRING_H
#define SDSTRING_H
/**
* @file sdstring.h
* @author lw
* @brief string����ϵ��
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
    * @defgroup groupstring �ַ�������
    * @brief �ַ����������߽���
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
    * ���ַ�����ʼ�ͽ���������Ϊ0
    * @param string : ��0Ϊ���������ַ���
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
    * ��ȡ�ַ����ĳ���
    * @param string : ��0Ϊ���������ַ���
    * @return : �ַ����ĳ���
    */
    inline UINT32 SSAPI SDStrlen( const CHAR *string )
    {
        return (UINT32)strlen(string);
    }

    /**
    * @brief
    * ��ȡUnicode�ַ����ĳ���
    * @param string : ��0Ϊ���������ַ���
    * @return : Unicode�ַ����ĳ���
    */
    inline UINT32 SSAPI SDWcslen( const wchar_t *string )
    {
        return (UINT32)wcslen(string);
    }

    /**
    * @brief
    * ��ȡ�ַ����ĳ���
    * @param pszStr : �ַ���
    * @param sizeInBytes :����ⳤ��
    * @return ��ȡ�ַ����ĳ��ȣ�����ַ����ĳ��ȳ���sizeInBytes������sizeInBytes
    */
    inline UINT32 SSAPI SDStrnlen(const CHAR *pszStr, UINT32 sizeInBytes )
    {
        return (UINT32)strnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * ��ȡUnicode�ַ����ĳ���
    * @param pszStr : Unicode�ַ���
    * @param sizeInBytes :����ⳤ��
    * @return ��ȡUnicode�ַ����ĳ��ȣ����Unicode�ַ����ĳ��ȳ���sizeInBytes������sizeInBytes
    */
    inline UINT32 SSAPI SDWcsnlen(const wchar_t *pszStr, size_t sizeInBytes)
    {
        return (UINT32)wcsnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * ��Ŀ���ַ��������һ���ַ���
    * @param strDestination : ��0Ϊ��������Ŀ���ַ���
    * @param strSource : ��0Ϊ��������Դ�ַ���
    * @return ����ַ�����ʹ�õ���Ŀ���ַ����ռ䣬��Ԥ���㹻�Ŀռ�
    */
    inline CHAR* SSAPI SDStrcat(CHAR *strDestination, const CHAR *strSource )
    {
        return strcat(strDestination, strSource);
    }

    /**
    * @brief
    * ��Ŀ���ַ��������һ���ַ���
    * @param strDestination : ��0Ϊ��������Ŀ���ַ���
    * @param dstLen : Ŀ�괮�ڴ���ܳ���,����ϲ���ĳ��ȴ��ڴ˳���,������кϲ�,���ؿմ�,���޸�Ŀ�괮
    * @param strSource : ��0Ϊ��������Դ�ַ���
    * @return ����ַ�����ʹ�õ���Ŀ���ַ����ռ�,����ϲ���ĳ��ȴ��ڴ˳���,������кϲ�,����NULL,���޸�Ŀ�괮
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
    * ��Ŀ���ַ��������һ���ַ���
    * @param strDestination : ��0Ϊ��������Ŀ���ַ�������
    * @param strSource : ��0Ϊ��������Դ�ַ���
    * @return ����ַ�����ʹ�õ���Ŀ���ַ����ռ�
    */
    template <size_t len>
    CHAR* SSAPI SDStrSafeCat( CHAR (&strDestination)[len], const CHAR *strSource )
    {
        return SDStrSafeCat(strDestination, len, strSource);
    }

    /**
    * @brief
    * ��Ŀ��Unicode�ַ��������һ��Unicode�ַ���
    * @param strDestination : ��0Ϊ��������Ŀ��Unicode�ַ���
    * @param strSource : ��0Ϊ��������ԴUnicode�ַ���
    * @return ���Unicode�ַ�����ʹ�õ���Ŀ��Unicode�ַ����ռ䣬��Ԥ���㹻�Ŀռ�
    */
    inline wchar_t*  SSAPI SDWcscat(wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscat(strDestination, strSource);
    }

    /**
    * @brief
    * ��Ŀ���ַ��������һ���ַ���
    * @param strDestination : ��0Ϊ��������Ŀ���ַ���
    * @param strSource : Դ�ַ���
    * @param len : Դ�ַ����������ӳ���
    * @return ����ַ�����ʹ�õ���Ŀ���ַ����ռ䣬��Ԥ���㹻�Ŀռ�
    */
    inline CHAR* SSAPI SDStrncat(CHAR *strDestination, const CHAR *strSource, UINT32 len)
    {
        return strncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * ��Ŀ��Unicode�ַ��������һ��Unicode�ַ���
    * @param strDestination : ��0Ϊ��������Ŀ��Unicode�ַ���
    * @param strSource : ԴUnicode�ַ���
    * @param len : ԴUnicode�ַ����������ӳ���
    * @return ���Unicode�ַ�����ʹ�õ���Ŀ��Unicode�ַ����ռ䣬��Ԥ���㹻�Ŀռ�
    */
    inline wchar_t* SSAPI SDWcsncat(wchar_t *strDestination, const wchar_t *strSource, UINT32 len)
    {
        return wcsncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * �����ַ���
    * @param strDestination : ��0Ϊ��������Ŀ�껺�������뱣֤���㹻�Ŀռ�
    * @param strSource : ��0Ϊ��������Դ�ַ���
    * @return Ŀ���ַ���
    */
    inline CHAR* SSAPI SDStrcpy( CHAR *strDestination, const CHAR *strSource )
    {
        return strcpy(strDestination, strSource);
    }

    /**
    * @brief
    * ��ȫ�����ַ���
    * @param strDestination : ��Ҫ�����ַ�����Ŀ�껺����
    * @param numberOfElements : Ŀ���ַ�����󳤶�
    * @param strSource : ��0��β��Դ�ַ���
    * @return ����Ŀ�괮,���Ŀ�껺�����ĳ���С��Դ��,����NULL
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
    * ��ȫ�����ַ���
    * @param strDestination : ��Ҫ�����ַ�����Ŀ���ַ�������
    * @param strSource : ��0��β��Դ�ַ���
    * @return ����Ŀ�괮,���Ŀ�껺�����ĳ���С��Դ��,���ؿմ�
    */
    template <size_t len>
    CHAR* SSAPI SDStrSafeCpy( CHAR (&strDestination)[len], const CHAR *strSource )
    {
        return SDStrSafeCpy(strDestination,len,strSource);
    }

    /**
    * @brief
    * ����Unicode�ַ���
    * @param strDestination : ��0��β��Ŀ�껺�������뱣֤���㹻�Ŀռ�
    * @param strSource : ��0��β��Դ�ַ���
    * @return Ŀ���ַ���
    */
    inline wchar_t* SSAPI SDWcscpy( wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscpy(strDestination, strSource);
    }

    /**
    * @brief
    * ��ȫ����Unicode�ַ���
    * @param strDestination : ��Ҫ����Unicode�ַ�����Ŀ�껺����
    * @param numberOfElements : Ŀ����Unicode������󳤶�
    * @param strSource : ��0��β��ԴUnicode�ַ���
    * @return ����Ŀ�괮,���Ŀ�껺�����ĳ���С��Դ��,����NULL
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
    * ��ȫ����Unicode�ַ���
    * @param strDestination : ��Ҫ����Unicode�ַ�����Ŀ���ַ�������
    * @param strSource : ��0��β��ԴUnicode�ַ���
    * @return ����Ŀ�괮,���Ŀ�껺�����ĳ���С��Դ��,���ؿմ�
    */
    template <size_t len>
    inline wchar_t* SSAPI SDWcsSafecpy( wchar_t (&strDestination)[len], const wchar_t *strSource )
    {
        return SDWcsSafecpy(strDestination, len, strSource);
    }

    /**
    * @brief
    * �����ַ���
    * @param strDestination : ��0��β��Ŀ���ַ���
    * @param strSource : ��0��β��Դ�ַ���
    * @param len : ��󿽱�����
    * @return ����Ŀ���ַ���
    */
    inline CHAR* SSAPI SDStrncpy( CHAR *strDestination, const CHAR *strSource, UINT32 len )
    {
        return strncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * ����Unicode�ַ���
    * @param strDestination : ��0��β��Ŀ��Unicode�ַ���
    * @param strSource : ��0��β��ԴUnicode�ַ���
    * @param len : ��󿽱�����
    * @return ����Ŀ��Unicode�ַ���
    */
    inline wchar_t* SSAPI SDWcsncpy( wchar_t *strDestination, const wchar_t *strSource, UINT32 len )
    {
        return wcsncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * �Ƚ��ַ���
    * @param string1 : ��0�������ַ���1
    * @param string2 : ��0�������ַ���2
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
    */
    inline INT32 SSAPI SDStrcmp( const CHAR *string1, const CHAR *string2 )
    {
        return strcmp(string1, string2);
    }

    /**
    * @brief
    * �Ƚ�Unicode�ַ���
    * @param string1 : ��0������Unicode�ַ���1
    * @param string2 : ��0������Unicode�ַ���2
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
    */
    inline INT32 SSAPI SDWcscmp( const wchar_t *string1, const wchar_t *string2 )
    {
        return wcscmp(string1, string2);
    }

    /**
    * @brief
    * �Ƚ��ַ���
    * @param string1 : ��0�������ַ���1
    * @param string2 : ��0�������ַ���2
    * @param count : ���Ƚϳ���
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
    */
    inline INT32 SSAPI SDStrncmp( const CHAR *string1, const CHAR *string2, UINT32 count )
    {
        return strncmp( string1, string2, count );
    }

    /**
    * @brief
    * �Ƚ�Unicode�ַ���
    * @param string1 : ��0������Unicode�ַ���1
    * @param string2 : ��0������Unicode�ַ���2
    * @param count : ���Ƚϳ���
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
    */
    inline INT32 SSAPI SDWcsncmp( const wchar_t *string1, const wchar_t *string2, size_t count )
    {
        return wcsncmp( string1, string2, count );
    }

    /**
    * @brief
    * ��һ���ַ����в���ĳ���ַ���λ��
    * @param string : ��0�������ַ���
    * @param c : ��Ҫ�����ҵ��ַ��ַ�
    * @return ָ����ҵĸ��ַ��ĵ�һ�γ��ֵ�λ�ã�����û���ҵ�����NULL
    */
    inline CHAR* SSAPI SDStrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strchr(string, c);
    }

    /**
    * @brief
    * ��һ��Unicode�ַ����в���ĳ���ַ���λ��
    * @param string : ��0������Unicode�ַ���
    * @param c : ��Ҫ�����ҵ��ַ��ַ�
    * @return ָ����ҵĸ��ַ��ĵ�һ�γ��ֵ�λ�ã�����û���ҵ�����NULL
    */
    inline wchar_t* SSAPI SDWcschr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcschr(string, c);
    }

    /**
    * @brief
    * ��һ���ַ������������ĳ���ַ���λ��
    * @param string : ��0�������ַ���
    * @param c : ��Ҫ�����ҵ��ַ��ַ�
    * @return ָ����ҵĸ��ַ������һ�γ��ֵ�λ�ã�����û���ҵ�����NULL
    */
    inline CHAR* SSAPI SDStrrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strrchr(string ,c);
    }

    /**
    * @brief
    * ��һ��Unicode�ַ������������ĳ���ַ���λ��
    * @param string : ��0������Unicode�ַ���
    * @param c : ��Ҫ�����ҵ��ַ��ַ�
    * @return ָ����ҵĸ��ַ������һ�γ��ֵ�λ�ã�����û���ҵ�����NULL
    */
    inline wchar_t* SSAPI SDWcsrchr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcsrchr(string, c);
    }

    /**
    * @brief
    * �����ַ���Сд���Ƚ��ַ���
    * @param string1 : ��0�������ַ���1
    * @param string2 : ��0�������ַ���2
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
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
    * �����ַ���Сд���Ƚ��ַ���
    * @param string1 : ��0�������ַ���1
    * @param string2 : ��0�������ַ���2
    * @param count : ���Ƚϳ���
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
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
    * �����ַ���Сд���Ƚ�Unicode�ַ���
    * @param string1 : ��0������Unicode�ַ���1
    * @param string2 : ��0������Unicode�ַ���2
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
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
    * �����ַ���Сд���Ƚ�Unicode�ַ���
    * @param string1 : ��0������Unicode�ַ���1
    * @param string2 : ��0������Unicode�ַ���2
    * @param count : ���Ƚϳ���
    * @return �ȽϽ��
    * < 0 �ַ���1С���ַ���2
    * 0 �ַ���1�����ַ���2
    * > 0 �ַ���1�����ַ���2
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
    * ��ĳ���ַ����з�Ϊ����ַ���
    * @param pszStr : ��һ�����뱻�зֵ��ַ�����֮������NULL
    * @param delim : �ָ��ַ������硰:������,������ ����
    * @return ���η��ر��и�����ַ��������û���µ����ַ���������NULL
    */
    inline CHAR* SSAPI SDStrtok(CHAR *pszStr, const CHAR *delim)
    {
        return strtok(pszStr, delim);
    }

    /**
    * @brief
    * ��ĳ��Unicode�ַ����з�Ϊ���Unicode�ַ���
    * @param pszStr : ��һ�����뱻�зֵ�Unicode�ַ�����֮������NULL
    * @param delim : �ָ�Unicode�ַ������硰:������,������ ����
    * @return ���η��ر��и����Unicode�ַ��������û���µ���Unicode�ַ���������NULL
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
    * ���ַ����е�����ANSI�ַ�ת��ΪСд
    * @param pszStr : ��0��β���ַ���
    * @return ��ת�����ַ�����ʹ��ԭ���ַ����Ŀռ䣬ԭ���ַ��������ƻ�
    */
    CHAR* SSAPI SDStrlwr(CHAR* pszStr);


    /**
    * @brief
    * ���ַ����е�����ANSI�ַ�ת��Ϊ��д
    * @param pszStr : ��0��β���ַ���
    * @return ��ת�����ַ�����ʹ��ԭ���ַ����Ŀռ䣬ԭ���ַ��������ƻ�
    */
    CHAR* SSAPI SDStrupr(CHAR* pszStr);

    /**
    * @brief
    * ��Unicode�ַ����е������ַ�ת��ΪСд
    * @param pszStr : ��0��β��Unicode�ַ���
    * @return ��ת����Unicode�ַ�����ʹ��ԭ��Unicode�ַ����Ŀռ䣬ԭ��Unicode�ַ��������ƻ�
    */
    wchar_t* SSAPI SDWcslwr(wchar_t* pszStr);

    /**
    * @brief
    * ��Unicode�ַ����е������ַ�ת��Ϊ��д
    * @param pszStr : ��0��β��Unicode�ַ���
    * @return ��ת����Unicode�ַ�����ʹ��ԭ��Unicode�ַ����Ŀռ䣬ԭ��Unicode�ַ��������ƻ�
    */
    wchar_t* SSAPI SDWcsupr(wchar_t* pszStr);

    /**
    * @brief
    * ��MultiBytes���ַ���ת��ΪUnicode�ַ���
    * @param dest : ��0��β��MultiBytes���ַ���
    * @param src : ���ת�����Unicode�ַ���
    * @param n : src�����ռ䳤��
    * @return ת�����Unicode��ʵ�ʳ���
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
    * ��Unicode���ַ���ת��ΪMultiBytes�ַ���
    * @param dest : ��0��β��Unicode���ַ���
    * @param src : ���ת�����MultiBytes�ַ���
    * @param n : src�����ռ䳤��
    * @return ת�����MultiBytes��ʵ�ʳ���
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
	* ���ַ���ת��Ϊ32λ�޷�������
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
    UINT32 SSAPI SDAtou(const CHAR* pStr);

	/**
	* @brief
	* ���ַ���ת��Ϊ64λ�޷�������
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
	UINT64 SSAPI SDAtou64(const CHAR* pStr); 

    /**
    * @brief
    * ��һ������ת��Ϊ�ַ���
    * @param pBuf : ת������ַ��洢�ռ�
    * @param buflen : �ַ��洢�ռ����󳤶�
    * @param dwNum : ����ת��������
    * @return ת�����ַ���ʹ�õĿռ䳤��
    */
    inline INT32 SSAPI SDItoa(CHAR* pBuf, UINT32 buflen, INT32 dwNum)
    {
        return SDSnprintf(pBuf, buflen, "%d", dwNum);
    }

	/**
	* @brief
	* ��һ������ת��Ϊ�ַ���
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
    template <UINT32 bufLen>
    inline INT32 SSAPI SDItoa(CHAR (&buf)[bufLen],INT32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%d", dwNum);
    }

	/**
	* @brief
	* ����64λ��������ת��Ϊ�ַ���
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
	inline INT32 SSAPI SDItoa64(CHAR *pBuf, UINT32 buflen, INT64 dqNum)
	{
		return SDSnprintf(pBuf, buflen, SDFMT_I64 , dqNum);
	}

	/**
	* @brief
	* ���޷�������ת��Ϊ�ַ���
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
	inline INT32 SSAPI SDUtoa(CHAR *pBuf, UINT32 buflen, UINT32 dwNum)
	{
		return SDSnprintf(pBuf, buflen, "%u", dwNum);
	}

	/**
	* @brief
	* ���޷�������ת��Ϊ�ַ���
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
    template <UINT32 bufLen>
    inline INT32 SSAPI SDUtoa(CHAR (&buf)[bufLen],INT32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%u", dwNum);
    }

	/**
	* @brief
	* ����64λ�޷�������ת��Ϊ�ַ���
	* @param pBuf : ת������ַ��洢�ռ�
	* @param buflen : �ַ��洢�ռ����󳤶�
	* @param dwNum : ����ת��������
	* @return ת�����ַ���ʹ�õĿռ䳤��
	*/
	template <UINT32 bufLen>
	inline INT32 SSAPI SDUtoa64(CHAR (&buf)[bufLen], UINT64 dqNum)
	{
		return SDSnprintf(buf, bufLen, SDFMT_U64, dqNum);
	}


    /**
    * @brief
    * ���ַ���ת��Ϊ������
    * @param nptr : ����ת������0��β���ַ���
    * @param endptr : [�������]����ΪNULL���������޷�ת�����ַ������ﷵ����ָ��
    * @return ��ȡ�ĸ�����
    */
    inline DOUBLE SSAPI SDStrtod(const CHAR *nptr, CHAR **endptr)
    {
        return strtod(nptr, endptr);
    }

    /**
    * @brief
    * split a string���ַ����ָ�Ϊһ���ַ�����vector
    * @param sSource : ���ָ���ַ���
    * @param delim : �ָ�������:��:������,������ ����
    * @return a vector to store strings splited from sSource
    */
    std::vector<std::string> SSAPI SDSplitStrings(const std::string &sSource, CHAR delim);

    /**
    * @brief
    * ���ַ���ת��Ϊ��д
    * @param pszStr : ����ת�����ַ���
    * @return ת������ַ���
    */
    std::string SSAPI SDStrupr(std::string &pszStr);

    /**
    * @brief
    * ���ַ���ת��ΪСд
    * @param pszStr : ����ת�����ַ���
    * @return ת������ַ���
    */
    std::string SSAPI SDStrlwr(std::string &pszStr);

    /**
    * @brief
    * ��һ���ַ����ÿ�
    * @param Destination : ��Ҫ�ÿյ�����
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
    * ��������Bufferת��Ϊ�ַ���,
    * @param pBuf    ������Bufferָ��
    * @param bufLen  ������Buffer����
    * @param pSplitter ����Ķ��������ݼ����
	* @param lineLen Դ�ַ����ָ���е�ÿ�г���,�ó��Ȳ������ָ����ĳ���
    * @return void
    */
    std::string SSAPI SDBufferToHex(const CHAR *pBuf, UINT32 bufLen, const CHAR *pSplitter = "", INT32 lineLen = -1);


    std::string SSAPI SDIntToHex(INT32 dwNum);

    /**
    * @brief
    * ��str��ߵ�chȥ����ԭ�ַ��������޸�
    * @param pszStr ȥ����ߵ�ch�ַ�
    * @param ch ��ȥ�����ַ�
    * @return ȥ���ַ�ch����ַ���
    */
    std::string SSAPI SDStrTrimLeft(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

    /**
    * @brief
    * ��str�ұߵ�chȥ����ԭ�ַ��������޸�
    * @param pszStr ȥ���ұߵ�ch�ַ�
    * @param ch ��ȥ�����ַ�
    * @return ȥ���ַ�ch����ַ���
    */
    std::string SSAPI SDStrTrimRight(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

    /**
    * @brief
    * ��str�������ߵ�chȥ����ԭ�ַ��������޸�
    * @param pszStr ȥ���������ߵ�ch�ַ�
    * @param ch ��ȥ�����ַ�
    * @return ȥ���ַ�ch����ַ���
    */
    std::string SSAPI SDStrTrim(std::string &pszStr, const CHAR* pTrimStr = " \r\t\n");

    /** @} */

}//namespace SSCP

#endif

