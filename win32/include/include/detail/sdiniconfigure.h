/******************************************************************************
			Copyright (C)  YoFei Corporation. All rights reserved.

sdiniconfigure.h - 配置文件读取类,使用ini文件

******************************************************************************/

#ifndef SDINICONFIG_H
#define SDINICONFIG_H
#include "sdtype.h"
#include <string>
#include <map>

namespace SSCP
{
    /**
    * @defgroup groupproperties key-value的配置读写操作，此类并非线程安全
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
        * 将pFilePath所代表的文件中的key－value值添加到这个CProperties中
        * @param pFilePath : 输入的文件名
        * @return 添加的key－value对的数量
        * @remark 文件形式为：
        * #这个符号是注释，可以注释一行
        * key1=value1
        * key2=value2
        * #注意：等于符号=前后的空格分别是key和value的一部分
        * #一行只能有一对key-value
        */
        BOOL ReadConfig( const CHAR *pFilePath, const CHAR *pLocate = NULL );

        /**
        * @brief
        * 将pBuffer中的key－value值添加到这个CProperties中
        * @param pBuffer : 输入的Buffer
        * @return 添加的key－value对的数量
        * @remark 文件形式为：
        * #这个符号是注释，可以注释一行
        * key1=value1
        * key2=value2
        * #注意：等于符号=前后的空格分别是key和value的一部分
        * #一行只能有一对key-value
        */
        BOOL ReadConfigFromMem( const CHAR *pBuffer );

        /**
        * @brief
        * 将所有的key-value对都添加到pFilePath所代表的文件中，覆盖文件中原有的内容
        * @param pFilePath : 输入的文件名
        * @return 添加成功返回true，失败返回false
        * @remark 文件形式为：
        * key1=value1
        * key2=value2
        * #注意：等于符号=前后的空格分别是key和value的一部分
        * #一行只能有一对key-value
        */
        BOOL WriteConfig( const CHAR *pFilePath );

        /**
        * @brief
        * 获取key对应的value
        * @param key : 这是需要查询的key
        * @param value : value存放的内存缓冲区
        * @param length : value内存缓冲区的最大长度
        */
        std::string GetProperty( const std::string &key );

        /**
        * @brief
        * 添加一个key－value对，如果原有此key，覆盖它
        * @param key : 输入的key
        * @param value : 输入的value
        * @return void
        */
        void SetProperty( const std::string &key, const std::string &value );

        /**
        * @brief
        * 移除一个key－value对
        * @param key : 移除的key
        * @return 如果原来有此key，返回true；如果原来没有此key，返回false
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
