/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.


******************************************************************************/

#ifndef SDSTRENCRYPT_H
#define SDSTRENCRYPT_H
/**
* @file sdstrencrypt.h
* @author lw
* @brief 字符串加密解密系列
*
**/
#include "sdtype.h"
#include <string>


namespace SSCP
{
    /**
    * @defgroup groupencrypt 字符串加密
    * @ingroup  SSCP
    * @{
    */

    /**
    *@brief 字符串加密处理类
    */
    class CSDStrEncrypt
    {
    public:
        CSDStrEncrypt(void) {}
        ~CSDStrEncrypt(void) {}
        static const BYTE CODE_KEY = 0x96;	/**<加密对应的key*/

        //对每个字节高低4bit互换，与KEY进行异或运算

        /**
        * @brief
        * static, encode a string
        * @param sContent : string to be encoded
        * @return void
        */
        static void SSAPI Encode(std::string& sContent);

        /**
        * @brief
        * static, encode a string
        * @param pBuf : pointer to the buf to be encode
        * @param nLen : buf len
        * @return void
        */
        static void SSAPI Encode(BYTE *pBuf, INT32 nLen);

        /**
        * @brief
        * static, decode a string
        * @param sContent : string to be decode
        * @return void
        */
        static void SSAPI Decode(std::string &sContent);

        /**
        * @brief
        * static, decode a string
        * @param pBuf : pointer to the buf to be encode
        * @param nLen : buf len
        * @return void
        */
        static void SSAPI Decode(BYTE *pBuf, INT32 nLen);
    };

    /** @} */

} // namespace SSCP



#endif //

