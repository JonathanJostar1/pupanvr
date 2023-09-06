#include "TAppGlobalConfig.h"

TAppGlobalConfig* TAppGlobalConfig::m_instance = NULL;

TAppGlobalConfig* TAppGlobalConfig::getInstance()
{
    if(!m_instance)
    {
        string filename = "appconfig.cfg";
        m_instance = new TAppGlobalConfig(filename);
    }  

    return m_instance;
}


TAppGlobalConfig::TAppGlobalConfig(const string& fileName)
    :AppConfig(fileName)
{
    AppDefaultConfig();
    load();
}

TAppGlobalConfig::~TAppGlobalConfig()
{

}

void TAppGlobalConfig::AppDefaultConfig()
{
    m_devName = "PUPANVR DEMO";

    registerStringValueProcess("devName", &m_devName);      
}

void TAppGlobalConfig::setDeviceName(const char* devName)
{
    if(!devName)  
    {
        return;
    }

    m_devName = devName;
}

const char* TAppGlobalConfig::getDeviceName()
{
    return m_devName.c_str();
}

