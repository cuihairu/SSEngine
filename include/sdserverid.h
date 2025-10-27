/******************************************************************************
		Copyright (C) YoFei Corporation. All rights reserved.

	sdserverid.h - 游戏服务器中的服务器id处理函数
******************************************************************************/


#ifndef SDSERVERID_H
#define SDSERVERID_H
/**
* @file sdserverid.h
* @author wangkui
* @brief ServerID类
*
**/
#include "sdtype.h"
#include <string>


namespace SSCP
{
    /**
    * @defgroup groupgametools GAME TOOLS
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * 将ServerID从数字转化为字符串
    * @param dwServerID : 数字的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则数字的ServerID的31－24位代表字符串的ServerID的第一位part1，
    * 数字的ServerID的23－16代表字符串的ServerID的第二位part2，
    * 数字的ServerID的15－8代表字符串的ServerID的第三位part3，
    * 数字的ServerID的7－0代表字符串的ServerID的第四位part4，
    * @return 转化为字符串的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    std::string SSAPI SDServerIDUtoa(UINT32 dwServerID, UINT32 dwMask = 0x08080808);

    /**
    * @brief
    * 将ServerID从字符串转化为数字
    * @param pszServerID : 字符串的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则字符串的ServerID的第一位part1代表数字的ServerID的31－24位，
    * 字符串的ServerID的第一位part2代表数字的ServerID的23－16位，
    * 字符串的ServerID的第一位part3代表数字的ServerID的15－8位，
    * 字符串的ServerID的第一位part4代表数字的ServerID的7－0位，
    * @return 转化为数字的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    UINT32 SSAPI SDServerIDAtou(const CHAR *pszServerID, UINT32 dwMask = 0x08080808);

//TODO 增加规范

    /**
    *@brief ServerID类
    */
    class CSDServerId
    {
    public:
        CSDServerId(UINT32 dwId = 0, UINT32 dwMask = 0x08080808);
        ~CSDServerId() {}

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的数字型的ID
        * @return void
        */
        void SSAPI SetID(UINT32 dwID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的字符串型的ID
        * @return void
        */
        void SSAPI SetID(const CHAR *pszID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置掩码
        * @param dwMask : 设置的掩码, 注意掩码数总和不能超过sizeof(UINT32)
        * @return void
        */
        void SSAPI SetMask(UINT32 dwMask);

        /**
        * @brief
        * 获取掩码
        * @param dwMask : 获取的掩码值
        * @return void
        */
        UINT32 SSAPI GetMask();

        /**
        * @brief
        * 获取AreaID，即字符串型ServerID的第一位
        * @return 获取AreaID
        */
        UINT32 SSAPI GetAreaId();

        /**
        * @brief
        * 获取GroupID，即字符串型ServerID的第二位
        * @return 获取GroupID
        */
        UINT32 SSAPI GetGroupId();

        /**
        * @brief
        * 获取ServerType，及字符串型ServerID的第三位
        * @return 获取ServerType
        */
        UINT32 SSAPI GetServerType();

        /**
        * @brief
        * 获取ServerIndex，及字符串型ServerID的第四位
        * @return 获取ServerIndex
        */
        UINT32 SSAPI GetServerIndex();

        /**
        * @brief
        * 获取字符串型的ServerID
        * @return 字符串型的ServerID
        */
        std::string SSAPI AsString();

        /**
        * @brief
        * 获取数字型的ServerID
        * @return 数字型的ServerID
        */
        UINT32 SSAPI AsNumber();

    private:
        UINT32       m_maskLen[4] ; //mask的长度，用来移位
        UINT32       m_subMask[4] ; //mask的值
        UINT32       m_id;          // 数字ID
        UINT32       m_mask;        //掩码,注意掩码数总和不能超过32
        std::string  m_strId;
    };


    /**
    * @brief
    * 将ServerID从数字转化为字符串
    * @param dwServerID : 数字的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则数字的ServerID的31－24位代表字符串的ServerID的第一位part1，
    * 数字的ServerID的23－16代表字符串的ServerID的第二位part2，
    * 数字的ServerID的15－8代表字符串的ServerID的第三位part3，
    * 数字的ServerID的7－0代表字符串的ServerID的第四位part4，
    * @return 转化为字符串的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    std::string SSAPI SDServerIDExUtoa(UINT64 dwServerID, UINT32 dwMask = 0x08080808);

    /**
    * @brief
    * 将ServerID从字符串转化为数字
    * @param pszServerID : 字符串的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则字符串的ServerID的第一位part1代表数字的ServerID的31－24位，
    * 字符串的ServerID的第一位part2代表数字的ServerID的23－16位，
    * 字符串的ServerID的第一位part3代表数字的ServerID的15－8位，
    * 字符串的ServerID的第一位part4代表数字的ServerID的7－0位，
    * @return 转化为数字的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    UINT64 SSAPI SDServerIDExAtou(const CHAR* pszServerID, UINT32 dwMask = 0x08080808);

    class CSDServerIdEx
    {
    public:
        CSDServerIdEx(UINT64 dwId = 0, UINT32 dwMask = 0x08080808);
        ~CSDServerIdEx() {}

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的数字型的ID
        * @return void
        */
        void SSAPI SetID(UINT64 dwID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的字符串型的ID
        * @return void
        */
        void SSAPI SetID(const CHAR* pszID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置掩码
        * @param dwMask : 设置的掩码, 注意掩码数总和不能超过sizeof(UINT32)
        * @return void
        */
        void SSAPI SetMask(UINT32 dwMask);

        /**
        * @brief
        * 获取掩码
        * @param dwMask : 获取的掩码值
        * @return void
        */
        UINT32 SSAPI GetMask();


        /**
        * @brief
        * 获取游戏中id
        * @return UINT32
        */
        UINT32 SSAPI GetGameId();


        /**
        * @brief
        * 获取AreaID，即字符串型ServerID的第一位
        * @return 获取AreaID
        */
        UINT32 SSAPI GetAreaId();

        /**
        * @brief
        * 获取GroupID，即字符串型ServerID的第二位
        * @return 获取GroupID
        */
        UINT32 SSAPI GetGroupId();

        /**
        * @brief
        * 获取ServerType，及字符串型ServerID的第三位
        * @return 获取ServerType
        */
        UINT32 SSAPI GetServerType();

        /**
        * @brief
        * 获取ServerIndex，及字符串型ServerID的第四位
        * @return 获取ServerIndex
        */
        UINT32 SSAPI GetServerIndex();

        /**
        * @brief
        * 获取字符串型的ServerID
        * @return 字符串型的ServerID
        */
        std::string SSAPI AsString();

        /**
        * @brief
        * 获取数字型的ServerID
        * @return 数字型的ServerID
        */
        UINT64 SSAPI AsNumber();
    private:
        UINT32        m_maskLen[4] ; //mask的长度，用来移位
        UINT32        m_subMask[4] ; //mask的值
        UINT32       m_mask ; //服务器ID掩码
        UINT64       m_id;
        std::string  m_strId;
    };

    /** @} */
}

#endif // 

