#include "TViewConfig.h"

TViewConfig* TViewConfig::m_instance = NULL;

TViewConfig* TViewConfig::getInstance()
{
     if(!m_instance)
    {
        string filename = "dvs_view.cfg";
        m_instance = new TViewConfig(filename);
    }  

    return m_instance;    
}

TViewConfig::TViewConfig(const string& fileName)
    :AppConfig(fileName)
{
    AppDefaultConfig();
    load();
}


TViewConfig::~TViewConfig()
{

}

void TViewConfig::AppDefaultConfig()
{
    return;
}

