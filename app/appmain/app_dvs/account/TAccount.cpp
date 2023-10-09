#include<stdio.h>

#include "TAccount.h"
#include "tlog.h"
#include "TDvsAppConfig.h"

TAccount* TAccount::m_instance = NULL;

TAccount* TAccount::getInstance()
{
    if(!m_instance)
    {
        m_instance = new TAccount();
    }

    return m_instance;
}

TAccount::TAccount()
{


}

TAccount::~TAccount()
{


}


bool    TAccount::auth_check(const char* name, const char* pwd)
{
    return true;
}
