#ifndef __TDvsAppConfig_h__
#define __TDvsAppConfig_h__

#include "appConfig.h"

class TDvsAppConfig: public AppConfig
{
public:
    static TDvsAppConfig* getInstance();

private:
    TDvsAppConfig(const string& fileName);
    ~TDvsAppConfig();
    void AppDefaultConfig();

    void        setDeviceName(const char* devName);
    const char* getDeviceName();
    
private:
    static TDvsAppConfig*   m_instance;
private:
    string      m_devName;
    
};

#endif