/******************************************************************************
			Copyright (C)  YoFei Corporation. All rights reserved.

sdiniconfigure.h - �����ļ���ȡ��,ʹ��ini�ļ�

******************************************************************************/

#ifndef SDINICONFIG_H
#define SDINICONFIG_H
#include "sdtype.h"
#include <string>
#include <map>

namespace SSCP
{
    /**
    * @defgroup groupproperties key-value�����ö�д���������ಢ���̰߳�ȫ
    * @ingroup  SSCP
    * @{
    */
    class CSDIniConfigure
    {
    public:
        CSDIniConfigure( const CHAR *pCfgFile = NULL );
        ~CSDIniConfigure(void);

        /**
        * @brief
        * ��pFilePath��������ļ��е�key��valueֵ��ӵ����CProperties��
        * @param pFilePath : ������ļ���
        * @return ��ӵ�key��value�Ե�����
        * @remark �ļ���ʽΪ��
        * #���������ע�ͣ�����ע��һ��
        * key1=value1
        * key2=value2
        * #ע�⣺���ڷ���=ǰ��Ŀո�ֱ���key��value��һ����
        * #һ��ֻ����һ��key-value
        */
        BOOL ReadConfig( const CHAR *pFilePath, const CHAR *pLocate = NULL );

        /**
        * @brief
        * ��pBuffer�е�key��valueֵ��ӵ����CProperties��
        * @param pBuffer : �����Buffer
        * @return ��ӵ�key��value�Ե�����
        * @remark �ļ���ʽΪ��
        * #���������ע�ͣ�����ע��һ��
        * key1=value1
        * key2=value2
        * #ע�⣺���ڷ���=ǰ��Ŀո�ֱ���key��value��һ����
        * #һ��ֻ����һ��key-value
        */
        BOOL ReadConfigFromMem( const CHAR *pBuffer );

        /**
        * @brief
        * �����е�key-value�Զ���ӵ�pFilePath��������ļ��У������ļ���ԭ�е�����
        * @param pFilePath : ������ļ���
        * @return ��ӳɹ�����true��ʧ�ܷ���false
        * @remark �ļ���ʽΪ��
        * key1=value1
        * key2=value2
        * #ע�⣺���ڷ���=ǰ��Ŀո�ֱ���key��value��һ����
        * #һ��ֻ����һ��key-value
        */
        BOOL WriteConfig( const CHAR *pFilePath );

        /**
        * @brief
        * ��ȡkey��Ӧ��value
        * @param key : ������Ҫ��ѯ��key
        * @param value : value��ŵ��ڴ滺����
        * @param length : value�ڴ滺��������󳤶�
        */
        std::string GetProperty( const std::string &key );

        /**
        * @brief
        * ���һ��key��value�ԣ����ԭ�д�key��������
        * @param key : �����key
        * @param value : �����value
        * @return void
        */
        void SetProperty( const std::string &key, const std::string &value );

        /**
        * @brief
        * �Ƴ�һ��key��value��
        * @param key : �Ƴ���key
        * @return ���ԭ���д�key������true�����ԭ��û�д�key������false
        */
        void RemoveProperty( const std::string &key );

    private:
        std::string ToString();

        typedef std::map<std::string, std::string> PropertiesMap;
        typedef PropertiesMap::iterator Iterator;

        PropertiesMap m_properties;
    };
}
#endif
