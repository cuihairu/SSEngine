#ifndef SDMAPCONFIGURE_H
#define SDMAPCONFIGURE_H
#include "sdtype.h"
#include "sdhashmap.h"
namespace SSCP
{

    class CSDMapConfigure
    {
    public:

        virtual BOOL ReadConfig( const char * pCfgFile, const CHAR * pLocate = NULL )
        {
            return TRUE;
        }

        virtual BOOL WriteConfig( const char * pCfgFile )
        {
            return TRUE;
        }

        virtual std::string GetProperty( const std::string & prop )
        {
            PropertyMap::iterator itr = m_properties.find(prop);
            if (itr != m_properties.end())
            {
                return itr->second;
            }
            return "";
        }

        virtual void SetProperty( const std::string & prop, const std::string & value )
        {
            m_properties.insert(std::make_pair(prop, value));
        }

    private:
        typedef HashMap<std::string, std::string> PropertyMap;
        PropertyMap m_properties;
    };
}

#endif // 
