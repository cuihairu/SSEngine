#ifndef __SD_CSVFILE_H_
#define __SD_CSVFILE_H_
#include "sdtype.h"
#include <vector>
#include <string>
using namespace std ;
namespace SSCP 
{
	class  CSDCsvBase
	{
	public:
		CSDCsvBase(){}

		~CSDCsvBase(){}

		void    Clear(){m_vLine.resize(0);}

		// get line count
		INT32 GetLineCount();

		// get item count of line
		INT32 GetItemCount(INT32 line);

		// insert a empty line in the "line" pos
		INT32 InsertLine(INT32 line);

		// insert a empty line to the end
		INT32 AddLine();

		// insert a item data
		INT32 InsertItem(INT32 line, INT32 item, const CHAR *data);

		// delete a item data
		INT32 DelItem(INT32 line, INT32 item);

		// delete one line data
		INT32 DelLine(INT32 line);

		//empty item
		INT32 EmptyItem(INT32 line, INT32 item);

		// change a item data
		INT32 ModifyItem(INT32 line, INT32 item, const CHAR *data);

		// get a item data
		INT32 ReadData(INT32 line, INT32 item, CHAR *data, INT32 size);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, T* obItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, T& obItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, CHAR& cItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, UINT8& byItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, INT16& shItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, UINT16& wItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, INT32& nItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, UINT32& dwItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, float& fItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, double& fItem);

		template <typename T>
		INT32 ReadData(INT32 line, INT32 item, std::string& strItem);

	protected:
		std::vector<std::vector<std::string> > m_vLine;
	private:
	};

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, T& obItem )
	{
		return -1;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, T* obItem )
	{
		return -1;  
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, CHAR& cItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		cItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, UINT8& byItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		byItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, INT16& shItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		shItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, UINT16& wItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		wItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, INT32& nItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		nItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, UINT32& dwItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		dwItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, float& fItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		fItem = static_cast<float>(atof(itemvec[item].c_str()));
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, double& fItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		fItem = atof(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	INT32 CSDCsvBase::ReadData( INT32 line, INT32 item, std::string& strItem )
	{
		if (line >= (INT32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (INT32)itemvec.size())
		{
			return -1;
		}

		strItem = itemvec[item];
		return 0;
	}

	class CSDCsvString:public CSDCsvBase
	{
		/*
		×Ö·û´®¸ñÊ½:
		{.,..,...,...}...{.,..,...,...}
		*/
	public:
		CSDCsvString(){}
		~CSDCsvString(){}

		// init CCsvfile for a string
		INT32 Init(const std::string& strData);

		//save data to a string
		INT32 FlushData(std::string& strOutPut);

	protected:
	private:
	};

	class CSDCsvfile:public CSDCsvBase
	{
	public:
		CSDCsvfile(){}
		~CSDCsvfile(){}

		// init CCsvfile for a .csv file path
		INT32 Init(const CHAR *path);

		//save data to db file immediately
		INT32 FlushData();

		INT32 ReadCsvString(INT32 line, INT32 item, CSDCsvString& obCsvString);

	private:
		//file path
		std::string m_strFilePath;
	};
}
#endif //#ifndef _CSVFILE_H_
