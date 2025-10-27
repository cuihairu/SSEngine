#ifndef _VER_CHECK_
#define _VER_CHECK_

#include "sdbase.h"

namespace SSCP
{

UINT32 SSAPI VerToDWORD(const SSSVersion* pstVersion);
bool SSAPI VerCheck(const SSSVersion* pRequestVer, const SSSVersion* pRealVer);

}//end namespace

#endif
