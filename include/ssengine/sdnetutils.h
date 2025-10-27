/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/



#ifndef SDNETUTILS_H
#define SDNETUTILS_H

#include "sdtype.h"

/**
* @file sdnetutils.h
* @author lw
* @brief 网络工具类
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
    * @defgroup groupnetutil 网络操作
    * @ingroup  SSCP
    * @{
    */


    typedef  struct hostent SDHostent;

    /**
    * @brief
    * IPv4 将整型的ip地址转换为点分隔的地址表示 0.0.0.0
    * 或将IPv6地址转换为字符串表示方式
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址,
    * @param pDst : 转化后的网络地址存放的空间
    * @param size : 网络地址存放空间的最大长度
    * @return 转化后的网络地址存放的空间(`pDst'), 或者失败返回NULL
    */
    const CHAR* SSAPI SDNetInetNtop(INT32 af, const void *pSrc, CHAR *pDst, size_t size);

    /**
    * @brief
    *  将字符串表示的IP地址转换为整型或IPv6结构表示
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址
    * @param pDst : 转化后的网络地址存放的空间
    * @  正确返回TRUE ,如果src所表示的地址无效,或与地址族不匹配,则返回FALSE
    */
    BOOL SSAPI SDNetInetPton(INT32 af, const CHAR *pSrc, void *pDst);

    /**
    * @brief
    * 将UINT32类型数据从网络顺序转化为主机顺序
    * @param hostlong : UINT32类型网络顺序数据
    * @return UINT32类型主机顺序数据
    */
    inline UINT32 SSAPI SDNtohl(UINT32 netlong)
    {
        return ntohl(netlong);
    }

    /**
    * @brief
    * 将UINT16类型数据从网络顺序转化为主机顺序
    * @param hostlong : UINT16类型网络顺序数据
    * @return UINT16类型主机顺序数据
    */
    inline UINT16 SSAPI SDNtohs(UINT16 netshort)
    {
        return ntohs(netshort);
    }

    /**
    * @brief
    * 将UINT32类型数据从主机顺序转化为网络顺序
    * @param hostlong : UINT32类型主机顺序数据
    * @return UINT32类型网络顺序数据
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
    * 将UINT16类型数据从主机顺序转化为网络顺序
    * @param hostlong : UINT16类型主机顺序数据
    * @return UINT16类型网络顺序数据
    */
    inline UINT16  SSAPI SDHtons(UINT16 hostlong)
    {
        return htons(hostlong);
    }

    /**
    * @brief
    * 转化一个字符串型的IPv4的地址为一个数字型的IP地址
    * @param pszAddr : 字符串型的IPv4的地址
    * @return 数字型的IP地址
    */
    inline ULONG SSAPI SDInetAddr(const CHAR* pszAddr)
    {
        return inet_addr(pszAddr);
    }

    /**
    * @brief
    * 转化一个数字型的IPv4的地址为一个字符串型的IP地址
    * @param lIp : 数字型的IPv4的地址
    * @return 字符串型的IP地址，如果出错，返回NULL
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
    * 获取机器名对应的IP地址
    * @param name : 机器名
    * @return 机器的IP地址
    */
    LONG SSAPI SDGetIpFromName(const CHAR * pszName);

	/**
	* @brief
	* 获取主机名	
	* @return 机器的主机名
	*/
    std::string SSAPI SDGetHostName();

	/**
	* @brief
	* 获取主机名获取主机信息	
	* @return 机器的主机信息
	*/
    SDHostent* SSAPI SDGetHostByName(const CHAR * pszName); // 域名解析

    /**
    * @brief
    * 获取当前机器的本地IP地址
    * @param ip :  用于获取本地的IP地址的数组
    * @param count : 数组长度
    * @return 本地IP地址的数量
    */
    INT32 SSAPI SDGetLocalIp(ULONG dwIp[], UINT32 dwCount);

//TODO get mac address
    /** @} */

}//

#endif /// 
