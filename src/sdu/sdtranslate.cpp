#include "ssengine/sdtranslate.h"

#if SDU_WITH_LIBICONV
#  include <errno.h>
#endif

namespace SSCP {

CSDTranslate::CSDTranslate()
#if SDU_WITH_LIBICONV
    : m_handle(reinterpret_cast<iconv_t>(-1))
#endif
{}

CSDTranslate::~CSDTranslate() {
#if SDU_WITH_LIBICONV
    if (m_handle != reinterpret_cast<iconv_t>(-1)) {
        iconv_close(m_handle);
        m_handle = reinterpret_cast<iconv_t>(-1);
    }
#endif
}

bool CSDTranslate::Init(const char* cpFromCharset, const char* cpToCharset) {
#if SDU_WITH_LIBICONV
    if (!cpFromCharset || !cpToCharset) {
        return false;
    }
    if (m_handle != reinterpret_cast<iconv_t>(-1)) {
        iconv_close(m_handle);
        m_handle = reinterpret_cast<iconv_t>(-1);
    }
    m_handle = iconv_open(cpToCharset, cpFromCharset);
    if (m_handle == reinterpret_cast<iconv_t>(-1)) {
        return false;
    }
    m_from = cpFromCharset;
    m_to = cpToCharset;
    return true;
#else
    (void)cpFromCharset;
    (void)cpToCharset;
    return true;
#endif
}

size_t CSDTranslate::Translate(char* cpSrc, size_t nSrcLen, char* cpDesc, size_t nDescLen) {
#if SDU_WITH_LIBICONV
    if (m_handle == reinterpret_cast<iconv_t>(-1) || !cpSrc || !cpDesc) {
        return static_cast<size_t>(-1);
    }
    char* inBuf = cpSrc;
    size_t inBytes = nSrcLen;
    char* outBuf = cpDesc;
    size_t outBytes = nDescLen;
    size_t result = iconv(m_handle, &inBuf, &inBytes, &outBuf, &outBytes);
    if (result == static_cast<size_t>(-1)) {
        return static_cast<size_t>(-1);
    }
    return nDescLen - outBytes;
#else
    if (!cpSrc || !cpDesc || nDescLen == 0) {
        return static_cast<size_t>(-1);
    }
    size_t copyLen = nSrcLen < nDescLen - 1 ? nSrcLen : nDescLen - 1;
    std::memcpy(cpDesc, cpSrc, copyLen);
    cpDesc[copyLen] = '\\0';
    return copyLen;
#endif
}

const CHAR* SSAPI SDGetLocaleCharset(void) {
#if SDU_WITH_LIBICONV
    return locale_charset();
#else
    return "UTF-8";
#endif
}

} // namespace SSCP
