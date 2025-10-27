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
			m_aObject[m_dwFreeHead].obj = object;   //从头拿一个
			m_dwFreeHead = m_aObject[m_dwFreeHead].next; // 更新队头
			m_aObject[dwID].next = (UINT32)-1;//next是－1表示在使用中
			-- m_freeSize;		
			return dwID;
		}

		BOOL Find(UINT32 dwID, T& pObject)
		{
			if ((dwID >= m_dwSize) )
			{
				return FALSE;
			}

			if (m_aObject[dwID].next != (UINT32)(-1)) //不是在使用中
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

			if (m_aObject[dwID].next != (UINT32)(-1))  //不是在使用中
				return ;
			++m_freeSize;
			if ( m_freeSize == 1) //还回来的第一个节点，是头又是尾
			{
				m_dwFreeHead = m_dwFreeTail = dwID;
				m_aObject[m_dwFreeHead].next = m_dwSize;
				return;
			}
			m_aObject[dwID].next = m_dwSize;  //这个表示无后继了
			m_aObject[m_dwFreeTail].next = dwID; //放到队尾
			m_dwFreeTail = dwID;  //更新队尾		
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

		//  获取当前使用率
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
