/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.
******************************************************************************/

#ifndef SDU_TRANSLATE_H
#define SDU_TRANSLATE_H
/**
* @file sdtranslate.h
* @author edeng_2000
* @brief Character set translation helpers
*/

#include "sdmacros.h"
#include "sdtype.h"
#include <string>

#if SDU_WITH_LIBICONV
#  include "iconv.h"
#  include "localcharset.h"
#else
#  include <cstring>
#endif

namespace SSCP {

class CSDTranslate {
public:
    CSDTranslate();
    ~CSDTranslate();

    size_t Translate(char* cpSrc, size_t nSrcLen, char* cpDesc, size_t nDescLen);
    bool Init(const char* cpFromCharset, const char* cpToCharset);

private:
#if SDU_WITH_LIBICONV
    iconv_t m_handle;
    std::string m_from;
    std::string m_to;
#endif
};

const char* SDGetLocaleCharset(void);

} // namespace SSCP

#endif // SDU_TRANSLATE_H
