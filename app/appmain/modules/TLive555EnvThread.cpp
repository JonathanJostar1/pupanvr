#include "TLive555EnvThread.h"
#include "tlog.h"

#include<iostream>

using namespace std;

TLive555EnvThread* TLive555EnvThread::m_instance = NULL;  

TLive555EnvThread* TLive555EnvThread::getInstance()
{
    if(!m_instance)
    {
        m_instance = new TLive555EnvThread();
    }    

    return m_instance;
}

UsageEnvironment* TLive555EnvThread::getEnviroment()
{
    return m_env;
}


TLive555EnvThread::TLive555EnvThread()
{
    m_scheduler = BasicTaskScheduler::createNew();
    m_env = BasicUsageEnvironment::createNew(*m_scheduler);
}

TLive555EnvThread::~TLive555EnvThread()
{
    m_env->reclaim();
    m_env = NULL;
    delete m_scheduler;
    m_scheduler = NULL;
}

void TLive555EnvThread::process()
{
    char eventLoopWatchVariable = 0;
    LOG(INFO) << "live555 env thread start!" << endl;
    m_env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
    LOG(INFO) << "live555 env thread exit! code:" << eventLoopWatchVariable << endl;  
}
