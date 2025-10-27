/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/
#ifndef SDMATH_H
#define SDMATH_H
/**
* @file sdmath.h
* @author lw
* @brief 数学计算类
*
**/
namespace SSCP
{
    /**
    * @defgroup 取最大最小值
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * 比较两个数据，返回最小值
    * @param const T & a ,const T & b : 进行比较的连个值
    * @return  T &  : 返回最小值
    */
    template<typename T >
    inline const T & SDMin(const T & __a, const T & __b)
    {
        return (__b < __a ) ? __b: __a;
    }

    /**
    * @brief
    * 比较两个数据，返回最大值
    * @param const T & a ,const T & b : 进行比较的连个值
    * @return T &  : 返回最大值
    */
    template<typename T >
    inline const T & SDMax(const T & __a, const T & __b)
    {
        return (__a < __b)? __b : __a;
    }

    /** @} */

} //namespace SSCP

#endif // 

