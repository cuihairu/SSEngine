/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdrandom.h: interface for the CSDRandom class.

******************************************************************************/

#ifndef SDRANDOM_H
#define SDRANDOM_H
/**
* @file sdrandom.h
* @author lw
* @brief �����ϵ��
*
**/
#include "sdtype.h"
#include <stdlib.h>

namespace SSCP
{
    /**
    * @defgroup grouprandom ���������
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * C-API��ʽ���������������
    * @param dwSeed �����������
    * @return void
    */
    void SDSetSeed(UINT32 dwSeed);

    /**
    * @brief
    * C-API��ʽ����ȡ�����
    * @return ���������
    */
    INT32 SDRandom();

    /**
    * @brief
    * C-API��ʽ����ȡ�����
    * @return ����ָ����Χ�ڵ������
    */
    UINT32 SDRandom(UINT32 dwRange);

    /**
    *@brief ����������ӿ�
    */
    class CSDRandom
    {
    public:
        CSDRandom();

        /**
        * @brief
        * �������������
        * @param dwSeed �����������
        * @return void
        */
        inline void SetSeed(UINT32 dwSeed)
        {
            m_seed = dwSeed;
        }

        /**
        * @brief
        * ��ȡ�����
        * @return ���������
        */
        UINT32 Random();


        /**
        * @brief
        * ��ȡ���������Χ��[0, unRange - 1]
        * @param unRange �����ֵ
        * @return ���������
        */
        UINT32 Random(UINT32 dwRange);


        /**
        * @brief
        * ��ȡ���������Χ��[unMinRange, unMaxRange - 1]
        * @param dwMin ����Сֵ
        * @param dwMax �����ֵ
        * @return ���������
        */
        UINT32 Random(UINT32 dwMin, UINT32 dwMax);


    private:
        UINT32 m_seed;
    };

    /** @} */
}

#endif
