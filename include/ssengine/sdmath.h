/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/
#ifndef SDMATH_H
#define SDMATH_H
/**
* @file sdmath.h
* @author lw
* @brief ��ѧ������
*
**/
namespace SSCP
{
    /**
    * @defgroup ȡ�����Сֵ
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * �Ƚ��������ݣ�������Сֵ
    * @param const T & a ,const T & b : ���бȽϵ�����ֵ
    * @return  T &  : ������Сֵ
    */
    template<typename T >
    inline const T & SDMin(const T & __a, const T & __b)
    {
        return (__b < __a ) ? __b: __a;
    }

    /**
    * @brief
    * �Ƚ��������ݣ��������ֵ
    * @param const T & a ,const T & b : ���бȽϵ�����ֵ
    * @return T &  : �������ֵ
    */
    template<typename T >
    inline const T & SDMax(const T & __a, const T & __b)
    {
        return (__a < __b)? __b : __a;
    }

    /** @} */

} //namespace SSCP

#endif // 

