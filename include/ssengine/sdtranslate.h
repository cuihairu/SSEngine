/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.


******************************************************************************/

#ifndef SDU_TRANSLATE_H
#define SDU_TRANSLATE_H
/**
* @file sdtranslate.h
* @author edeng_2000
* @brief sdu ∞Ê±æ–≈œ¢
*
**/

#include "sdmacros.h"
#include "sdtype.h"
#include <string>
using namespace std ;

#if SDU_WITH_LIBICONV
#include "iconv.h"
#include "localcharset.h"

namespace SSCP
{
	class CSDTranslate
	{
	public:
		CSDTranslate();
		
		~CSDTranslate();


	public:
		size_t Translate(CHAR *cpSrc, 
						 size_t nSrcLen, 
						 CHAR * cpDesc,
						 size_t nDescLen ) ;
		bool   Init(const char *cpFromCharset,const char *cpToCharset);
	private:
		iconv_t    m_hHandle;
		string	   m_strFromCharset ;
		string	   m_strToCharset ;
	};


	const CHAR * SSAPI SDGetLocaleCharset (void);
}



#endif

#endif