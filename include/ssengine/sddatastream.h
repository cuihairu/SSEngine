/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

	sdnetdata.h - 网络传输数据封装类实现

******************************************************************************/

#ifndef SDNETDATA_H
#define SDNETDATA_H
/**
* @file sdnetdata.h
* @author wangkui
* @brief 网络数据转化系列
*
**/
#include <stdio.h>
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupnetdata 网络传输封装
    * @ingroup  SSCP
    * @{
    */

    /**
    *@brief 网络传输数据封装类实现
    */
    class CSDDataStream
    {
    public:
        CSDDataStream();
        virtual ~CSDDataStream();

        /**
        * @brief
        * 初始化net data
        * @param pszBuf : 输入数据buffer
        * @param dwBufLen : buffer的长度
        * @return void
        */
        void SSAPI Prepare(CHAR *pszBuf, INT32 dwBufLen);

        /**
        * @brief
        * 对数据区重置
        * @return void
        */
        void SSAPI Reset();

        /**
        * @brief
        * 得到封装好的数据缓冲区
        * @return 封装好的数据缓冲区指针
        */
        inline CHAR* SSAPI GetData()
        {
            return m_pBuf;
        }

        /**
        * @brief
        * 得到封装好的数据缓冲区长度
        * @return 封装好的数据缓冲区长度
        */
        inline INT32 SSAPI GetDataLen()
        {
            return m_pos;
        }

        /**
        * @brief
        * 数据包添加byte类型数据（1字节）
        * @param byValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        inline INT32 SSAPI AddByte(UCHAR byValue);

        /**
        * @brief
        * 数据包获取并删除byte类型数据（1字节）
        * @param byValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelByte( UCHAR  &byValue);

        /**
        * @brief
        * 数据包添加char类型数据（1字节）
        * @param chChar : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddChar(CHAR  chChar);

        /**
        * @brief
        * 数据包获取并删除char类型数据（1字节）
        * @param chChar : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelChar(CHAR &chChar);

        /**
        * @brief
        * 数据包添加USHORT类型数据（2字节）
        * @param wValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddWord(UINT16 wValue);

        /**
        * @brief
        * 数据包获取并删除USHORT类型数据（1字节）
        * @param wValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelWord(UINT16 &wValue);

        /**
        * @brief
        * 数据包添加short类型数据（2字节）
        * @param sValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddShort(INT16 sValue);

        /**
        * @brief
        * 数据包获取并删除short类型数据（2字节）
        * @param sValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelShort(INT16 &sValue);

        /**
        * @brief
        * 数据包添加unsigned int类型数据
        * @param dwValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddDword(UINT32 dwValue);

        /**
        * @brief
        * 数据包获取并删除unsigned int类型数据
        * @param dwValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelDword(UINT32 &dwValue);

        /**
        * @brief
        * 数据包添加int类型数据
        * @param iValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddInt(INT32 iValue);

        /**
        * @brief
        * 数据包获取并删除int类型数据
        * @param iValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelInt(INT32 &iValue);

        /**
        * @brief
        * 数据包添加UINT64类型数据(8字节)
        * @param uullValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddUint64(UINT64 uullValue);

        /**
        * @brief
        * 数据包获取并删除UINT64类型数据(8字节)
        * @param uullValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelUint64(UINT64& uullValue);

        /**
        * @brief
        * 数据包添加INT64类型数据(8字节)
        * @param llValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddInt64(INT64 llValue);

        /**
        * @brief
        * 数据包获取并删除INT64类型数据(8字节)
        * @param llValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelInt64(INT64 &llValue);

        /**
        * @brief
        * 数据包添加float类型数据
        * @param fValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddFloat(FLOAT fValue);

        /**
        * @brief
        * 数据包获取并删除float类型数据
        * @param fValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelFloat(FLOAT &fValue);

        /**
        * @brief
        * 数据包添加double类型数据
        * @param dbValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddDouble(DOUBLE dbValue);

        /**
        * @brief
        * 数据包获取并删除double类型数据
        * @param dbValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelDouble(DOUBLE &dbValue);

        /**
        * @brief
        * 数据包添加string类型数据
        * @param pszStr : 添加的数据内容
        * @param dwBufLen : 添加的数据长度
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddString(const CHAR *pszStr, INT32 dwBufLen);

        /**
        * @brief
        * 数据包获取并删除字符串类型数据
        * @param pszOut :得到的数据内容
        * @param dwBufLen : 得到的数据长度
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelString(CHAR *pszOut, INT32 dwBufLen);

        /**
        * @brief
        * 数据包添加一块buffer数据
        * @param pszBuf : 添加的buf地址
        * @param dwBufLen : 添加的buf长度
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI AddBuf(const UCHAR *pszBuf, INT32 dwBufLen);

        /**
        * @brief
        * 数据包获取并删除一块buffer数据
        * @param pszBuf :输入缓冲区得到buffer内容
        * @param dwBufLen : 得到的数据长度
        * @return 数据包当前所指的位置
        */
        INT32 SSAPI DelBuf(UCHAR *pszBuf, INT32 dwBufLen);

    private:
        CHAR    *m_pBuf;
        INT32   m_bufLen;
        INT32   m_pos;
    };

    /** @} */
}
#endif
