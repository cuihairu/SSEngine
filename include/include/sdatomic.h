/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sdatomic.h - ����ԭ�Ӳ����ĺ���ϵ��

******************************************************************************/

#ifndef SDATOMIC_H
#define SDATOMIC_H

/**
* @file sdatomic.h
* @author lw
* @brief ����ԭ�Ӳ����ĺ���ϵ��
*
**/
#include "sdtype.h"

namespace SSCP
{
    /**
    * @defgroup groupatomic ԭ�Ӳ���
    * @ingroup  SSCP
    * @{
    */

    /*
    * ԭ�Ӳ���32λ��ֵ,ע�ⲻҪ��Win32ģʽ��ʹ��64λ��ԭ�Ӳ����������ڲ�ʹ����64λ��API��
		��ҪWindows Vista���ϵİ汾
    * ��Ҫע����ǣ�ÿ���������ڲ�ʵ�ֶ����ɲ���ϵͳ�Լ�ȷ�ϵ�
    * �ڱ��麯���У�
    * Add����ӷ�
    * Sub�������
    * Inc�����1
    * Dec�����1
    */


    /**
    * @brief
    * ���ڴ���ԭ�Ӷ�ȡһ��32λֵ
    * @param pdwMem : ָ����Ҫ����ȡֵ��ָ��
    * @return mem��ֵ
    */
    INT32 SSAPI SDAtomicRead32(volatile INT32 *pdwMem);

	/**
	* @brief
	* ���ڴ���ԭ�Ӷ�ȡһ��64λֵ, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pqwMem : ָ����Ҫ����ȡֵ��ָ��
	* @return mem��ֵ
	*/
    INT64 SSAPI SDAtomicRead64(volatile INT64 *pqwMem);

    /**
    * @brief
    * ���ڴ���ԭ������һ��32λ��ֵ
    * @param pdwMem : ָ����Ҫ����ֵ��ָ��
    * @param dwValue : ��Ҫ���õ�ֵ
    * @return void
    */
    void SSAPI SDAtomicSet32(volatile INT32 *pdwMem, INT32 dwValue);

	/**
	* @brief
	* ���ڴ���ԭ������һ��64λ��ֵ, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pqwMem : ָ����Ҫ����ֵ��ָ��
	* @param dwValue : ��Ҫ���õ�ֵ
	* @return void
	*/
    void SSAPI SDAtomicSet64(volatile INT64 *pqwMem, INT64 qwValue);

    /**
    * @brief
    * 32λԭ�Ӽӷ�����ԭʼֵ���һ��valֵ
    * @param pdwMem : ָ�򱻼�����ָ��
    * @param dwValue : ����
    * @return mem�б���ǰ��ԭʼֵ
    */
    INT32 SSAPI SDAtomicAdd32(volatile INT32 *pdwMem, INT32 dwValue);

	/**
	* @brief
	* 64λԭ�Ӽӷ�����ԭʼֵ���һ��valֵ, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pdwMem : ָ�򱻼�����ָ��
	* @param dwValue : ����
	* @return mem�б���ǰ��ԭʼֵ
	*/
    INT64 SSAPI SDAtomicAdd64(volatile INT64 *pqwMem, INT64 qwValue);

    /**
    * @brief
    * 32λԭ�Ӽ�������ԭʼֵ��ȥһ��valֵ
    * @param pdwMem : ָ�򱻼�����ָ��
    * @param dwValue : ����
    * @return ����ǰ��ԭֵ
    */
    INT32 SSAPI SDAtomicSub32(volatile INT32 *pdwMem, INT32 dwValue);

	/**
	* @brief
	* 64λԭ�Ӽ�������ԭʼֵ��ȥһ��valֵ, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pdwMem : ָ�򱻼�����ָ��
	* @param dwValue : ����
	* @return ����ǰ��ԭֵ
	*/
    INT64 SSAPI SDAtomicSub64(volatile INT64 *pqwMem, INT64 qwValue);

    /**
    * @brief
    * 32λԭ�Ӽ�1�㷨����ԭʼֵ��1
    * @param pdwMem : ָ�򱻼�����ָ��
    * @return mem�б���ǰ��ԭʼֵ
    */
    INT32 SSAPI SDAtomicInc32(volatile INT32 *pdwMem);
	
	/**
	* @brief
	* 64λԭ�Ӽ�1�㷨����ԭʼֵ��1, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pdwMem : ָ�򱻼�����ָ��
	* @return mem�б���ǰ��ԭʼֵ
	*/
    INT64 SSAPI SDAtomicInc64(volatile INT64 *pqwMem);

    /**
    * @brief
    * 32λԭ�Ӽ�1�㷨����ԭʼֵ��1
    * @param pdwMem : ָ�򱻼�����ָ��
    * @return mem�б���ǰ��ԭʼֵ
    */
    INT32 SSAPI SDAtomicDec32(volatile INT32 *pdwMem);

	/**
	* @brief
	* 64λԭ�Ӽ�1�㷨����ԭʼֵ��1, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pdwMem : ָ�򱻼�����ָ��
	* @return mem�б���ǰ��ԭʼֵ
	*/
    INT64 SSAPI SDAtomicDec64(volatile INT64 *pqwMem);

    /**
    * @brief
    * 32λԭ�ӵ�CAS(Compare and Swap)�㷨����32λֵpdwMemָ���ֵ��dwCmp�Ƚϣ�
    * ���һ�£���dwValue��ֵ��ֵ��*pdwMem
    * @param pdwMem : ָ�򱻱Ƚϵ�ֵ��ָ��
    * @param dwValue : ��memָ���ֵ��cmpһ�£�����ֵ��ֵ
    * @param dwCmp : �Ƚϵ�ֵ
    * @return mem�б���ֵǰ��ԭʼֵ
    */
    INT32 SSAPI SDAtomicCas32(volatile INT32 *pdwMem, INT32 dwValue, INT32 dwCmp);

	/**
	* @brief
	* 64λԭ�ӵ�CAS(Compare and Swap)�㷨����64λֵmemָ���ֵ��cmp�Ƚ�, ��ҪWindows Vista���ϲ���ϵͳ
	* ���һ�£���with��ֵ��ֵ��*pdwMem
	* @param pdwMem : ָ�򱻱Ƚϵ�ֵ��ָ��
	* @param dwValue : ��memָ���ֵ��cmpһ�£�����ֵ��ֵ
	* @param dwCmp : �Ƚϵ�ֵ
	* @return mem�б���ֵǰ��ԭʼֵ
	*/
    INT64 SSAPI SDAtomicCas64(volatile INT64 *pqwMem, INT64 qwValue, INT64 qwCmp);

    /**
    * @brief
    * �����㷨������val��memָ���ֵ
    * @param pdwMem : ָ�򱻽�����ֵ��ָ��
    * @param dwValue : ������ֵ
    * @return mem�б�����ǰ��ԭʼֵ
    */
    INT32 SSAPI SDAtomicXchg32(volatile INT32 *pdwMem, INT32 dwValue);

	/**
	* @brief
	* �����㷨������val��memָ���ֵ, ��ҪWindows Vista���ϲ���ϵͳ
	* @param pdwMem : ָ�򱻽�����ֵ��ָ��
	* @param dwValue : ������ֵ
	* @return mem�б�����ǰ��ԭʼֵ
	*/
    INT64 SSAPI SDAtomicXchg64(volatile INT64 *pqwMem, INT64 qwValue);

    /**
    * @brief
    * ԭ�ӵ�ָ���CAS(Compare and Swap)�㷨����memָ���ָ����cmpָ��Ƚϣ�
    * ���һ�£���withָ����memָ���ָ�뽻��
    * @param pPtr : ָ�򱻱Ƚϵ�ָ���ָ��
    * @param dwValue : ��memָ���ָ����cmpһ�£�����ֵ��ָ��
    * @param pCmp : �Ƚϵ�ָ��
    * @return mem��ԭʼ��ָ��
    */
    void* SSAPI SDAtomicCasptr(volatile void **pPtr, void *pWith, const void *pCmp);

    /**
    * @brief
    * ����ָ���㷨
    * @param pPtr : pointer to the pointerָ�򱻽�����ָ���ָ��
    * @param pWith : what to swap it with��������ָ��
    * @return mem�б�������ԭʼָ��
    */
    void* SSAPI SDAtomicXchgptr(volatile void **pPtr, void *pWith);

    /** @} */
}

#endif

