/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdrandom.h: interface for the CSDRandom class.

******************************************************************************/

#ifndef SDRANDOM_H
#define SDRANDOM_H
/**
* @file sdrandom.h
* @author lw
* @brief 随机数系列
*
**/
#include "sdtype.h"
#include <stdlib.h>

namespace SSCP
{
    /**
    * @defgroup grouprandom 随机数操作
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * C-API方式，设置随机数种子
    * @param dwSeed ：随机数种子
    * @return void
    */
    void SDSetSeed(UINT32 dwSeed);

    /**
    * @brief
    * C-API方式，获取随机数
    * @return 返回随机数
    */
    INT32 SDRandom();

    /**
    * @brief
    * C-API方式，获取随机数
    * @return 返回指定范围内的随机数
    */
    UINT32 SDRandom(UINT32 dwRange);

    /**
    *@brief 随机数操作接口
    */
    class CSDRandom
    {
    public:
        CSDRandom();

        /**
        * @brief
        * 设置随机数种子
        * @param dwSeed ：随机数种子
        * @return void
        */
        inline void SetSeed(UINT32 dwSeed)
        {
            m_seed = dwSeed;
        }

        /**
        * @brief
        * 获取随机数
        * @return 返回随机数
        */
        UINT32 Random();


        /**
        * @brief
        * 获取随机数，范围是[0, unRange - 1]
        * @param unRange ：最大值
        * @return 返回随机数
        */
        UINT32 Random(UINT32 dwRange);


        /**
        * @brief
        * 获取随机数，范围是[unMinRange, unMaxRange - 1]
        * @param dwMin ：最小值
        * @param dwMax ：最大值
        * @return 返回随机数
        */
        UINT32 Random(UINT32 dwMin, UINT32 dwMax);


    private:
        UINT32 m_seed;
    };

    /** @} */
}

#endif
