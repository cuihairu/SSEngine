#ifndef SD_IDPOOL_H
#define SD_IDPOOL_H
#include <list>
#include "sdtype.h"
#include "sdutil.h"
using namespace std;
namespace SSCP
{
	template <typename T>
	class CSDIDPool
	{
		typedef struct Node
		{
			T obj;
			UINT32 next;
		}Ele;
	public:
		BOOL Init(UINT32 dwSize)
		{			
			m_aObject = SDSAFE_NEW Ele[dwSize];
			memset(m_aObject,0,dwSize*sizeof(Ele));
			for ( UINT32 i = 0; i < dwSize; i++ )
				m_aObject[i].next = i+1;
			m_dwSize = m_freeSize = dwSize;
			m_dwFreeHead = 0;
			m_dwFreeTail = dwSize-1;
			return TRUE;
		}

		CSDIDPool(): m_aObject(NULL), m_dwSize(0) {}

		~CSDIDPool()
		{
			SDSAFE_DELETE [] m_aObject;
		}

		UINT32 Alloc(T object)
		{
			if ( 0 == m_freeSize) return (UINT32)(-1);
			UINT32 dwID = m_dwFreeHead;
			m_aObject[m_dwFreeHead].obj = object;   //��ͷ��һ��
			m_dwFreeHead = m_aObject[m_dwFreeHead].next; // ���¶�ͷ
			m_aObject[dwID].next = (UINT32)-1;//next�ǣ�1��ʾ��ʹ����
			-- m_freeSize;		
			return dwID;
		}

		BOOL Find(UINT32 dwID, T& pObject)
		{
			if ((dwID >= m_dwSize) )
			{
				return FALSE;
			}

			if (m_aObject[dwID].next != (UINT32)(-1)) //������ʹ����
				 return FALSE;

			pObject = m_aObject[dwID].obj;
			return TRUE;
		}

		void Free(UINT32 dwID)
		{
			if ((dwID >= m_dwSize) )
			{
				return;
			}

			if (m_aObject[dwID].next != (UINT32)(-1))  //������ʹ����
				return ;
			++m_freeSize;
			if ( m_freeSize == 1) //�������ĵ�һ���ڵ㣬��ͷ����β
			{
				m_dwFreeHead = m_dwFreeTail = dwID;
				m_aObject[m_dwFreeHead].next = m_dwSize;
				return;
			}
			m_aObject[dwID].next = m_dwSize;  //�����ʾ�޺����
			m_aObject[m_dwFreeTail].next = dwID; //�ŵ���β
			m_dwFreeTail = dwID;  //���¶�β		
		}

		std::list<UINT32>::size_type GetFreeCount() const
		{
			return m_freeSize;
		}

		BOOL IsEmpty() const
		{
			return ( m_freeSize == m_dwSize );
		}

		BOOL IsFull() const
		{
			return ( m_freeSize == 0 );
		}

		std::list<UINT32>::size_type Capacity() const
		{
			return m_dwSize;
		}

		//  ��ȡ��ǰʹ����
		UINT32 GetUSG()
		{
			return 100 - m_freeSize*100/m_dwSize;
		}
		
	private:
		UINT32			m_dwSize;
		UINT32			m_freeSize;
		Ele *			m_aObject;
		UINT32			m_dwFreeHead;
		UINT32			m_dwFreeTail;	
	};
}
#endif
