#ifndef SSCP_SGPKG_H_20070709
#define SSCP_SGPKG_H_20070709

#include "sdnet.h"
#include "sdnetutils.h"
namespace SSCP{

#define SDPKG_MARK16    0xAAEE
#define SDPKG_MARK32    0xAAEF

#pragma pack(push, 1)

//
//
//
struct SSDPkgHead16
{
    UINT16  wMark;
    UINT16  wDataLen;
    UINT16  wCheckSum;
};

struct SSDPkgHead32
{
    UINT16  wMark;
    UINT32  dwDataLen;
    UINT16  wCheckSum;
};

#pragma pack(pop)

//
// wDataLen: data length after SSDPkgHead16, not include length of SSDPkgHead16
// return:  total packet length, include length of SSDPkgHead16 and length of 
//          data
//
inline INT32 BuildSDPkgHead16(SSDPkgHead16* pstHead, UINT16 wDataLen)
{
    pstHead->wMark		= SDPKG_MARK16;
    pstHead->wDataLen	= wDataLen;
    pstHead->wCheckSum	= (pstHead->wDataLen ^ 0xBBCC) & 0x88AA;

    pstHead->wMark		= SDHtons(pstHead->wMark);
    pstHead->wDataLen	= SDHtons(pstHead->wDataLen);
    pstHead->wCheckSum	= SDHtons(pstHead->wCheckSum);

    return wDataLen + sizeof(SSDPkgHead16);
}

inline INT32 BuildSDPkgHead32(SSDPkgHead32* pstHead, UINT32 dwDataLen)
{
    pstHead->wMark		= SDPKG_MARK32;
    pstHead->dwDataLen	= dwDataLen;
    pstHead->wCheckSum	= ( (pstHead->dwDataLen>>16)^(pstHead->dwDataLen)^0xBBCC) & 0x88AA;

    pstHead->wMark		= SDHtons(pstHead->wMark);
    pstHead->dwDataLen	= SDHtonl(pstHead->dwDataLen);
    pstHead->wCheckSum	= SDHtons(pstHead->wCheckSum);

    return dwDataLen + sizeof(SSDPkgHead32);
}

inline INT32 CheckSDPkgHead(const char* pPkgHead, UINT32 dwLen)
{
    if(dwLen < sizeof(UINT16))
        return 0;

    UINT16 wMark = SDNtohs(*(UINT16*)pPkgHead);
    if(SDPKG_MARK16 == wMark)
    {
        if(dwLen < sizeof(SSDPkgHead16))
            return 0;

        SSDPkgHead16 stTmpHead16;
        stTmpHead16.wMark       = SDNtohs( ((SSDPkgHead16*)pPkgHead)->wMark );
        stTmpHead16.wDataLen    = SDNtohs( ((SSDPkgHead16*)pPkgHead)->wDataLen );
        stTmpHead16.wCheckSum   = SDNtohs( ((SSDPkgHead16*)pPkgHead)->wCheckSum );

        if(stTmpHead16.wCheckSum != ( (stTmpHead16.wDataLen ^ 0xBBCC) & 0x88AA ) )
            return -1;

        return stTmpHead16.wDataLen + sizeof(SSDPkgHead16);
    }
    else if(SDPKG_MARK32 == wMark)
    {
        if(dwLen < sizeof(SSDPkgHead32))
            return 0;

        SSDPkgHead32 stTmpHead32;
        stTmpHead32.wMark       = SDNtohs( ((SSDPkgHead32*)pPkgHead)->wMark );
        stTmpHead32.dwDataLen    = SDNtohl( ((SSDPkgHead32*)pPkgHead)->dwDataLen );
        stTmpHead32.wCheckSum   = SDNtohs( ((SSDPkgHead32*)pPkgHead)->wCheckSum );

        if(stTmpHead32.wCheckSum !=  ( ((stTmpHead32.dwDataLen>>16)^(stTmpHead32.dwDataLen)^0xBBCC) & 0x88AA) )
            return -1;

        return stTmpHead32.dwDataLen + sizeof(SSDPkgHead32);
    }

    return -1;
}

inline UINT32 GetSDPkgDataOffset(const char* pPkg, UINT32 dwLen)
{
    UINT16 wMark = SDNtohs(*(UINT16*)pPkg);
    if(SDPKG_MARK16 == wMark)
    {
        return sizeof(SSDPkgHead16);
    }
    else if(SDPKG_MARK32 == wMark)
    {
        return sizeof(SSDPkgHead32);
    }
    else
    {
        return 0;
    }
}

class CSDPacketParser : public ISSPacketParser
{
public:
    CSDPacketParser(void){}
    virtual ~CSDPacketParser(void){}

    virtual INT32 SSAPI ParsePacket(const char* pBuf, UINT32 dwLen)
    {
        INT32 nPkgLen = CheckSDPkgHead(pBuf, dwLen);

        if(nPkgLen < 0)
            return -1;

        if(nPkgLen <= (INT32)dwLen)
            return nPkgLen;

        return 0;
    }
};

}

#endif
