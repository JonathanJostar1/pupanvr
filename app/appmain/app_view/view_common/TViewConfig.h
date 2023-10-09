#ifndef TVIEWCONFIG_H
#define TVIEWCONFIG_H

#pragma once

#include "appConfig.h"

class TViewConfig:public AppConfig
{
public:
    static TViewConfig* getInstance();
private:
    TViewConfig(const string& fileName);
    ~TViewConfig();

    void AppDefaultConfig();
private:
    static TViewConfig* m_instance;
};

#endif