/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdconfigure.h - �����ļ���ȡ��,ʹ��xml��ini�ļ�

******************************************************************************/

#ifndef SDCONFIGURE_H
#define SDCONFIGURE_H
/**
* @file sdconfigure.h
* @author wangkui
* @brief ���������
*
**/
#include <string>
#include <vector>
#include <sstream>
#include "detail/sdxmlconfigure.h"
#include "detail/sdiniconfigure.h"
#include "detail/sdmapconfigure.h"
#include <strstream>

namespace SSCP
{
    /**
    * @defgroup groupconfigure �����ļ���д�����಻���̰߳�ȫ��
    * @ingroup  SSCP
    * @{
    */

    /**
    * @brief
    * ģ���ַ���ת��
    * @param value : �������Ҫת�����ַ���
    * @return ת�����ֵ
    */
    template <typename T>
    T SDConvterTrait( const std::string & value )
    {
        return value;
    };

    /**
    * @brief
    * �ַ���ת��Ϊint
    * @param value : �������Ҫת�����ַ���
    * @return ת�����ֵ
    */
    template <> inline INT32 SDConvterTrait<INT32>(const std::string& value) { return atoi(value.c_str()); }

    /**
    *@brief �����ļ���ȡ��
    */
    template<typename ConfigType>
    class CSDConfigureImpl
    {
    public:
        CSDConfigureImpl()
        {
        }

        /**
        * @brief
        * ����һ�������ļ�
        * @param pszFileName : �����ļ���·��
        * @param pLocate :�ļ��ı����ʽ Ĭ��ΪUTF-8
        * @return ��ȡ�ɹ�����true�����󷵻�false
        */

        BOOL Read( const CHAR * pszFileName, const CHAR* pLocate = NULL )
        {
            return m_configure.ReadConfig(pszFileName);
        }


        BOOL Write( const CHAR * pszFileName )
        {
            return m_configure.WriteConfig(pszFileName);
        }


        /**
        * @brief
        * ��ȡһ��������
        * @param p : �����������
        * @param def : Ĭ��ֵ�����û�д����������ָ����Ĭ��ֵ
        * @return �����������
        */
        template <typename T>
        T GetProperty( const CHAR *pszProp, const T & def = T() )
        {
            std::string result = m_configure.GetProperty(pszProp);
            return result.empty() ? def : SDConvterTrait<T>(result);
        }

        std::string  GetProperty( const CHAR *pszProp )
        {
            return m_configure.GetProperty(pszProp);
        }

        /**
        * @brief
        * ����һ��������˽ӿ���ʱδʵ��
        * @param pProp : �����������
        * @param value : �������ֵ
        * @return oid
        */
        template <typename T>
        void SetProperty( const CHAR * pszProp, const T & value )
        {
            std::stringstream strsteam;
            strsteam << value;
            m_configure.SetProperty(pszProp, strsteam.str());
        }

    private:
        ConfigType m_configure;
    };

    typedef CSDConfigureImpl<CSDMapConfigure> CSDMapConfig;
    typedef CSDConfigureImpl<CSDXmlConfigure> CSDXmlConfig;
    typedef CSDConfigureImpl<CSDIniConfigure> CSDIniConfig;


    /** @} */

}


#endif //