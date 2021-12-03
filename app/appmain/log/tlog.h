
#include "glog/logging.h"

#include<iostream>

using namespace std;

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