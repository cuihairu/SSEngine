/******************************************************************************
Copyright (C) YoFei Corporation. All rights reserved.

sdfifo.h - �Ƚ��ȳ�������С�

******************************************************************************/

#ifndef SDFIFO_H
#define SDFIFO_H
/**
* @file sdfifo.h
* @author lw
* @brief �Ƚ��ȳ�����
*
**/
/**
* @file sdfifo.h
* @author wk
* @brief �Ƚ��ȳ�������С�
*
**/
#include "sdtype.h"
#include "detail/sdfifodqueue.h"
#include "detail/sdfifoqueue.h"
namespace SSCP 
{


	/**
	* @brief �Ƚ��ȳ��������, ֧�ֶ��̺߳͵��߳�, �Ƿ�Ϊ���߳�,��Fifoȷ��
	*
	*/
	template <typename T, typename Fifo = CSDFifoQueue<T, CSDMutex> >
	class CSDFifo
	{

	public:
		/**
		* @brief
		* ����Ϣ���л�ȡ��Ϣ
		* @param ms : ��ȡ��Ϣ�ĳ�ʱʱ��
		* ��ms������ʱ�����һ�Σ������Ƿ��ܻ�ȡ�������������
		* ��ms������ʱ���ȴ���Ӧ��ʱ��
		* @return ��ȡ����Ϣָ�룬�����ʱ������NULL
		*/
		inline  T* SSAPI Pop(UINT32 ms= 0)
		{
			return m_fifo.Pop(ms);
		}

		/**
		* @brief
		* ����Ϣ����д����Ϣ
		* @param pItem : д�����Ϣָ��
		* @return void
		*/
		inline void SSAPI Push(T * pItem)
		{
			m_fifo.Push(pItem);
		}

		/**
		* @brief
		* ��ǰ�ڶ������еĿɶ�����Ϣ��
		* @return void
		*/
		inline size_t SSAPI ReadSize()
		{
			return m_fifo.ReadSize();
		}

		/**
		* @brief
		* ��ǰд�����е���Ϣ����
		* @return �Ѿ�д����Ϣ����
		*/
		inline size_t SSAPI WriteSize()
		{
			return m_fifo.WriteSize();
		}
	private:
		Fifo  m_fifo;
	};
}

#endif // 
