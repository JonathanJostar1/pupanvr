#ifndef TAPPGLOBALCONFIG_H
#define TAPPGLOBALCONFIG_H

#include "appConfig.h"

class TAppGlobalConfig: public AppConfig
{
public:
    static TAppGlobalConfig* getInstance();

private:
    TAppGlobalConfig(const string& fileName);
    ~TAppGlobalConfig();
    void AppDefaultConfig();

    void        setDeviceName(const char* devName);
    const char* getDeviceName();
    
private:
    static TAppGlobalConfig*   m_instance;
private:
    string      m_devName;
    
};

#endif