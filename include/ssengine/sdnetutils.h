/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/



#ifndef SDNETUTILS_H
#define SDNETUTILS_H

#include "sdtype.h"

/**
* @file sdnetutils.h
* @author lw
* @brief ���繤����
*
**/
#ifdef WINDOWS
#include <WinSock2.h>
#include "stdio.h" 
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#endif

#include <string>
#include "sdtype.h"

namespace SSCP
{

    /**
    * @defgroup groupnetutil �������
    * @ingroup  SSCP
    * @{
    */


    typedef  struct hostent SDHostent;

    /**
    * @brief
    * IPv4 �����͵�ip��ַת��Ϊ��ָ��ĵ�ַ��ʾ 0.0.0.0
    * ��IPv6��ַת��Ϊ�ַ�����ʾ��ʽ
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : ��Ҫ��ת���������ַ,
    * @param pDst : ת����������ַ��ŵĿռ�
    * @param size : �����ַ��ſռ����󳤶�
    * @return ת����������ַ��ŵĿռ�(`pDst'), ����ʧ�ܷ���NULL
    */
    const CHAR* SSAPI SDNetInetNtop(INT32 af, const void *pSrc, CHAR *pDst, size_t size);

    /**
    * @brief
    *  ���ַ�����ʾ��IP��ַת��Ϊ���ͻ�IPv6�ṹ��ʾ
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : ��Ҫ��ת���������ַ
    * @param pDst : ת����������ַ��ŵĿռ�
    * @  ��ȷ����TRUE ,���src����ʾ�ĵ�ַ��Ч,�����ַ�岻ƥ��,�򷵻�FALSE
    */
    BOOL SSAPI SDNetInetPton(INT32 af, const CHAR *pSrc, void *pDst);

    /**
    * @brief
    * ��UINT32�������ݴ�����˳��ת��Ϊ����˳��
    * @param hostlong : UINT32��������˳������
    * @return UINT32��������˳������
    */
    inline UINT32 SSAPI SDNtohl(UINT32 netlong)
    {
        return ntohl(netlong);
    }

    /**
    * @brief
    * ��UINT16�������ݴ�����˳��ת��Ϊ����˳��
    * @param hostlong : UINT16��������˳������
    * @return UINT16��������˳������
    */
    inline UINT16 SSAPI SDNtohs(UINT16 netshort)
    {
        return ntohs(netshort);
    }

    /**
    * @brief
    * ��UINT32�������ݴ�����˳��ת��Ϊ����˳��
    * @param hostlong : UINT32��������˳������
    * @return UINT32��������˳������
    */
    inline UINT32 SSAPI SDHtonl(UINT32 hostlong)
    {
        return htonl(hostlong);
    }


    inline UINT64 SSAPI SDHtonll(UINT64 number)
    {
        return ( htonl( UINT32((number >> 32) & 0xFFFFFFFF)) |
                 (UINT64(htonl(UINT32(number & 0xFFFFFFFF))) << 32));
    }

    inline UINT64 SSAPI SDNtohll(UINT64 number)
    {
        return ( ntohl( UINT32((number >> 32) & 0xFFFFFFFF) ) |
                 (UINT64 (ntohl(UINT32(number & 0xFFFFFFFF)))  << 32));
    }


    /**
    * @brief
    * ��UINT16�������ݴ�����˳��ת��Ϊ����˳��
    * @param hostlong : UINT16��������˳������
    * @return UINT16��������˳������
    */
    inline UINT16  SSAPI SDHtons(UINT16 hostlong)
    {
        return htons(hostlong);
    }

    /**
    * @brief
    * ת��һ���ַ����͵�IPv4�ĵ�ַΪһ�������͵�IP��ַ
    * @param pszAddr : �ַ����͵�IPv4�ĵ�ַ
    * @return �����͵�IP��ַ
    */
    inline ULONG SSAPI SDInetAddr(const CHAR* pszAddr)
    {
        return inet_addr(pszAddr);
    }

    /**
    * @brief
    * ת��һ�������͵�IPv4�ĵ�ַΪһ���ַ����͵�IP��ַ
    * @param lIp : �����͵�IPv4�ĵ�ַ
    * @return �ַ����͵�IP��ַ�������������NULL
    */
    inline CHAR*  SSAPI SDInetNtoa(LONG lIp)
    {
        struct in_addr addr;
#ifdef WINDOWS
        addr.S_un.S_addr = lIp;
#else
        addr.s_addr = lIp;
#endif
        return inet_ntoa(addr);
    }

    /**
    * @brief
    * ��ȡ��������Ӧ��IP��ַ
    * @param name : ������
    * @return ������IP��ַ
    */
    LONG SSAPI SDGetIpFromName(const CHAR * pszName);

	/**
	* @brief
	* ��ȡ������	
	* @return ������������
	*/
    std::string SSAPI SDGetHostName();

	/**
	* @brief
	* ��ȡ��������ȡ������Ϣ	
	* @return ������������Ϣ
	*/
    SDHostent* SSAPI SDGetHostByName(const CHAR * pszName); // ��������

    /**
    * @brief
    * ��ȡ��ǰ�����ı���IP��ַ
    * @param ip :  ���ڻ�ȡ���ص�IP��ַ������
    * @param count : ���鳤��
    * @return ����IP��ַ������
    */
    INT32 SSAPI SDGetLocalIp(ULONG dwIp[], UINT32 dwCount);

//TODO get mac address
    /** @} */

}//

#endif /// 
