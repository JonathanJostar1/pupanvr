#include "TDvsAppConfig.h"

TDvsAppConfig* TDvsAppConfig::m_instance = NULL;

TDvsAppConfig* TDvsAppConfig::getInstance()
{
    if(!m_instance)
    {
        string filename = "dvs_app.cfg";
        m_instance = new TDvsAppConfig(filename);
    }  

    return m_instance;
}


TDvsAppConfig::TDvsAppConfig(const string& fileName)
    :AppConfig(fileName)
{
    AppDefaultConfig();
    load();
}

TDvsAppConfig::~TDvsAppConfig()
{

}

void TDvsAppConfig::AppDefaultConfig()
{
    m_devName = "PUPANVR DEMO";

    registerStringValueProcess("devName", &m_devName);      
}

void TDvsAppConfig::setDeviceName(const char* devName)
{
    if(!devName)  
    {
        return;
    }

    m_devName = devName;
}

const char* TDvsAppConfig::getDeviceName()
{
    return m_devName.c_str();
}

