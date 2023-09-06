#ifndef TLIVE555ENVTHREAD_H
#define TLIVE555ENVTHREAD_H


#include "UsageEnvironment.hh"
#include "HashTable.hh"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#include "VThreadBase.h"

class TLive555EnvThread: public VThreadBase
{
public:
    static TLive555EnvThread* getInstance();
    UsageEnvironment* getEnviroment();
public:
    TLive555EnvThread();
    ~TLive555EnvThread();
    void process();
private:
    static TLive555EnvThread*   m_instance;   
    TaskScheduler*              m_scheduler;
    UsageEnvironment*           m_env;
};

#endif