/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

sdindexer.h - �̰߳�ȫ�Ľ�һ�����ӳ��Ϊһ��IDֵ��IDֵ�ķ�Χ��1-maxSize��0Ϊ��ЧID
******************************************************************************/


#ifndef SDINDEXER_H
#define SDINDEXER_H
#include "sdatomic.h"
#include <algorithm>
#include "sdatomic.h"
#include "detail/sdindexer_impl.h"

/**
* @file sdindexer.h
* @author wangkui
* @brief ������ӳ��Ϊ����������ֵ
*
**/
namespace SSCP 
{

    /**
    * @defgroup groupindexer �������༯��
    * @ingroup  SSCP
    * @{
    */

#ifndef SDINVALID_INDEX 
#define SDINVALID_INDEX 0  //������Ч������Ϊ0 
#endif 

	/**
	* @brief ��������
	*
	*/
	template <class T, UINT32 maxSize = 1024, class ValueEqual = DefaultEqual<T> > 
	class SDIndexer
	{
	public:
		/**
		* @brief
		* ����һ��IDֵ��
		* @param value : ��ID��Ӧ��Ӧ������
		* @return �����IDֵ�������ЧΪSDINVAID_INDEX
		*/
		inline UINT32 Alloc( T & data)
		{
			return m_indexerImpl.Alloc(data); 
		}

		/**
		* @brief
		* �������ݲ��Ҷ�Ӧ��ID ��ʹ������Ƚ��٣�����ʱ�����Ҫ�����Ҹ��Ӷ�ΪO(n)
		* @param value : ��ID��Ӧ��Ӧ������
		* @return �����IDֵ�����û�д����ݣ�������ЧSDINVAID_INDEX
		*/
		inline UINT32 Find(const T & data)
		{
			return m_indexerImpl.Find(data); 
		}

		/**
		* @brief
		* ����IDֵ��ȡ��Ӧ�����ݣ����ʧ��Ϊ�����͵�Ĭ��ֵ
		* @param idx : �����IDֵ
		* @return ��IDֵ��Ӧ������
		*/
		inline T & Get(UINT32 idx)
		{
			return m_indexerImpl.Get(idx); 
		}

		/**
		* @brief
		* �жϸ�IDֵ��Ӧ�������Ƿ���Ч���Ƿ������Ч����
		* @param idx : �����IDֵ
		* @return ��IDֵ�Ƿ���Ч
		*/
		inline BOOL Exist(UINT32 idx)
		{
			return m_indexerImpl.Exist(idx); 
		}

		/**
		* @brief
		* �ͷŸ�IDֵ�����ظ�ID��Ӧ������ 
		* @param idx : �����IDֵ
		* @return ��IDֵ��Ӧ������
		*/
		inline T& Free(UINT32 idx )
		{
			return m_indexerImpl.Free(idx); 
		}

	private:
		SDIndexerImpl<T, maxSize, ValueEqual> m_indexerImpl; 
	};

}// namespace SSCP 

/** @} */
#endif // 


