/******************************************************************************
		Copyright (C) YoFei Corporation. All rights reserved.

	sdserverid.h - ��Ϸ�������еķ�����id������
******************************************************************************/


#ifndef SDSERVERID_H
#define SDSERVERID_H
/**
* @file sdserverid.h
* @author wangkui
* @brief ServerID��
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
    * ��ServerID������ת��Ϊ�ַ���
    * @param dwServerID : ���ֵ�ServerID
    * @param dwMask : ���룬�����ַ�����ServerID��ÿһλռ���ֵ�ServerID�Ķ���λ
    * �磬���maskΪ0x08080808��
    * �����ֵ�ServerID��31��24λ�����ַ�����ServerID�ĵ�һλpart1��
    * ���ֵ�ServerID��23��16�����ַ�����ServerID�ĵڶ�λpart2��
    * ���ֵ�ServerID��15��8�����ַ�����ServerID�ĵ���λpart3��
    * ���ֵ�ServerID��7��0�����ַ�����ServerID�ĵ���λpart4��
    * @return ת��Ϊ�ַ�����ServerID
    * @remark : �ַ���ServerID�ĸ�ʽΪ: part1-part2-part3-part4.
    */
    std::string SSAPI SDServerIDUtoa(UINT32 dwServerID, UINT32 dwMask = 0x08080808);

    /**
    * @brief
    * ��ServerID���ַ���ת��Ϊ����
    * @param pszServerID : �ַ�����ServerID
    * @param dwMask : ���룬�����ַ�����ServerID��ÿһλռ���ֵ�ServerID�Ķ���λ
    * �磬���maskΪ0x08080808��
    * ���ַ�����ServerID�ĵ�һλpart1�������ֵ�ServerID��31��24λ��
    * �ַ�����ServerID�ĵ�һλpart2�������ֵ�ServerID��23��16λ��
    * �ַ�����ServerID�ĵ�һλpart3�������ֵ�ServerID��15��8λ��
    * �ַ�����ServerID�ĵ�һλpart4�������ֵ�ServerID��7��0λ��
    * @return ת��Ϊ���ֵ�ServerID
    * @remark : �ַ���ServerID�ĸ�ʽΪ: part1-part2-part3-part4.
    */
    UINT32 SSAPI SDServerIDAtou(const CHAR *pszServerID, UINT32 dwMask = 0x08080808);

//TODO ���ӹ淶

    /**
    *@brief ServerID��
    */
    class CSDServerId
    {
    public:
        CSDServerId(UINT32 dwId = 0, UINT32 dwMask = 0x08080808);
        ~CSDServerId() {}

        /**
        * @brief
        * ����ID
        * @param pszID : ���õ������͵�ID
        * @return void
        */
        void SSAPI SetID(UINT32 dwID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * ����ID
        * @param pszID : ���õ��ַ����͵�ID
        * @return void
        */
        void SSAPI SetID(const CHAR *pszID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * ��������
        * @param dwMask : ���õ�����, ע���������ܺͲ��ܳ���sizeof(UINT32)
        * @return void
        */
        void SSAPI SetMask(UINT32 dwMask);

        /**
        * @brief
        * ��ȡ����
        * @param dwMask : ��ȡ������ֵ
        * @return void
        */
        UINT32 SSAPI GetMask();

        /**
        * @brief
        * ��ȡAreaID�����ַ�����ServerID�ĵ�һλ
        * @return ��ȡAreaID
        */
        UINT32 SSAPI GetAreaId();

        /**
        * @brief
        * ��ȡGroupID�����ַ�����ServerID�ĵڶ�λ
        * @return ��ȡGroupID
        */
        UINT32 SSAPI GetGroupId();

        /**
        * @brief
        * ��ȡServerType�����ַ�����ServerID�ĵ���λ
        * @return ��ȡServerType
        */
        UINT32 SSAPI GetServerType();

        /**
        * @brief
        * ��ȡServerIndex�����ַ�����ServerID�ĵ���λ
        * @return ��ȡServerIndex
        */
        UINT32 SSAPI GetServerIndex();

        /**
        * @brief
        * ��ȡ�ַ����͵�ServerID
        * @return �ַ����͵�ServerID
        */
        std::string SSAPI AsString();

        /**
        * @brief
        * ��ȡ�����͵�ServerID
        * @return �����͵�ServerID
        */
        UINT32 SSAPI AsNumber();

    private:
        UINT32       m_maskLen[4] ; //mask�ĳ��ȣ�������λ
        UINT32       m_subMask[4] ; //mask��ֵ
        UINT32       m_id;          // ����ID
        UINT32       m_mask;        //����,ע���������ܺͲ��ܳ���32
        std::string  m_strId;
    };


    /**
    * @brief
    * ��ServerID������ת��Ϊ�ַ���
    * @param dwServerID : ���ֵ�ServerID
    * @param dwMask : ���룬�����ַ�����ServerID��ÿһλռ���ֵ�ServerID�Ķ���λ
    * �磬���maskΪ0x08080808��
    * �����ֵ�ServerID��31��24λ�����ַ�����ServerID�ĵ�һλpart1��
    * ���ֵ�ServerID��23��16�����ַ�����ServerID�ĵڶ�λpart2��
    * ���ֵ�ServerID��15��8�����ַ�����ServerID�ĵ���λpart3��
    * ���ֵ�ServerID��7��0�����ַ�����ServerID�ĵ���λpart4��
    * @return ת��Ϊ�ַ�����ServerID
    * @remark : �ַ���ServerID�ĸ�ʽΪ: part1-part2-part3-part4.
    */
    std::string SSAPI SDServerIDExUtoa(UINT64 dwServerID, UINT32 dwMask = 0x08080808);

    /**
    * @brief
    * ��ServerID���ַ���ת��Ϊ����
    * @param pszServerID : �ַ�����ServerID
    * @param dwMask : ���룬�����ַ�����ServerID��ÿһλռ���ֵ�ServerID�Ķ���λ
    * �磬���maskΪ0x08080808��
    * ���ַ�����ServerID�ĵ�һλpart1�������ֵ�ServerID��31��24λ��
    * �ַ�����ServerID�ĵ�һλpart2�������ֵ�ServerID��23��16λ��
    * �ַ�����ServerID�ĵ�һλpart3�������ֵ�ServerID��15��8λ��
    * �ַ�����ServerID�ĵ�һλpart4�������ֵ�ServerID��7��0λ��
    * @return ת��Ϊ���ֵ�ServerID
    * @remark : �ַ���ServerID�ĸ�ʽΪ: part1-part2-part3-part4.
    */
    UINT64 SSAPI SDServerIDExAtou(const CHAR* pszServerID, UINT32 dwMask = 0x08080808);

    class CSDServerIdEx
    {
    public:
        CSDServerIdEx(UINT64 dwId = 0, UINT32 dwMask = 0x08080808);
        ~CSDServerIdEx() {}

        /**
        * @brief
        * ����ID
        * @param pszID : ���õ������͵�ID
        * @return void
        */
        void SSAPI SetID(UINT64 dwID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * ����ID
        * @param pszID : ���õ��ַ����͵�ID
        * @return void
        */
        void SSAPI SetID(const CHAR* pszID, UINT32 dwMask = 0x08080808);

        /**
        * @brief
        * ��������
        * @param dwMask : ���õ�����, ע���������ܺͲ��ܳ���sizeof(UINT32)
        * @return void
        */
        void SSAPI SetMask(UINT32 dwMask);

        /**
        * @brief
        * ��ȡ����
        * @param dwMask : ��ȡ������ֵ
        * @return void
        */
        UINT32 SSAPI GetMask();


        /**
        * @brief
        * ��ȡ��Ϸ��id
        * @return UINT32
        */
        UINT32 SSAPI GetGameId();


        /**
        * @brief
        * ��ȡAreaID�����ַ�����ServerID�ĵ�һλ
        * @return ��ȡAreaID
        */
        UINT32 SSAPI GetAreaId();

        /**
        * @brief
        * ��ȡGroupID�����ַ�����ServerID�ĵڶ�λ
        * @return ��ȡGroupID
        */
        UINT32 SSAPI GetGroupId();

        /**
        * @brief
        * ��ȡServerType�����ַ�����ServerID�ĵ���λ
        * @return ��ȡServerType
        */
        UINT32 SSAPI GetServerType();

        /**
        * @brief
        * ��ȡServerIndex�����ַ�����ServerID�ĵ���λ
        * @return ��ȡServerIndex
        */
        UINT32 SSAPI GetServerIndex();

        /**
        * @brief
        * ��ȡ�ַ����͵�ServerID
        * @return �ַ����͵�ServerID
        */
        std::string SSAPI AsString();

        /**
        * @brief
        * ��ȡ�����͵�ServerID
        * @return �����͵�ServerID
        */
        UINT64 SSAPI AsNumber();
    private:
        UINT32        m_maskLen[4] ; //mask�ĳ��ȣ�������λ
        UINT32        m_subMask[4] ; //mask��ֵ
        UINT32       m_mask ; //������ID����
        UINT64       m_id;
        std::string  m_strId;
    };

    /** @} */
}

#endif // 

