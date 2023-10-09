#ifndef TJSONRPCSERVERPROCESS_H
#define TJSONRPCSERVERPROCESS_H

#pragma once

#include "VThreadBase.h"

class TJsonRPCServerProcess:public VThreadBase
{
public:
    static TJsonRPCServerProcess* getInstance();
    bool    setJsonRpcServerPort(int port);
private:
    TJsonRPCServerProcess();
    ~TJsonRPCServerProcess();
private:
    void process();
private:
    static TJsonRPCServerProcess*   m_instance;    
    int     m_server_port;
};

#endif