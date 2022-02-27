
#include "glog/logging.h"

#include<iostream>
#include<stdio.h>

using namespace std;

#ifndef __TLOG_H__
#define __TLOG_H__

class TLog
{
public:
    static TLog *getInstance();
    bool log_init();

private:
    TLog();
    ~TLog();

private:
    static TLog* m_instance;
};

#endif
