/*
 * TAppComm.cpp
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#include "TAppComm.h"
#include "app_info_def.h"
#include "git_log_version_number.h"

#include<string.h>
#include<stdio.h>
#include<unistd.h>

TAppComm::TAppComm()
{
	// TODO Auto-generated constructor stub

}

const char* TAppComm::getAppVersion()
{
	return APP_VERSION;
}

string TAppComm::getAppBuildDataInfo()
{
    char buff1[8];
    int iday = 0, iyear = 0;
    int imonth = 0;
    const char *pMonth[] = {"Jua", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    memset(buff1, 0, sizeof(buff1));
    const char* Date = __DATE__;
    if(sscanf(Date, "%4s %d %d", buff1, &iday, &iyear) != 3)
    {
        return false;
    }

    int i;
    for(i = 0; i < 12; i++)
    {
        if(memcmp(buff1, pMonth[i], 3) == 0)
        {
            imonth = i + 1;
            break;
        }
    }

    const char* timeBuild = __TIME__;
    int h = 0,m = 0,s = 0;
    if(sscanf(timeBuild, "%d:%d:%d", &h, &m, &s) != 3)
    {
        return false;
    }

    char rltbuff[64];
    snprintf(rltbuff, sizeof(rltbuff), "%04d%02d%02d-%02d%02d%02d", iyear, imonth, iday, h, m, s);

    return rltbuff;
}

string TAppComm::getAppGitVersionInfo()
{
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "%s %s %s", GIT_COMMIT, GIT_LOG_DATE, GIT_COMMIT_AUTH);
	return buffer;
}

TAppComm::~TAppComm()
{
	// TODO Auto-generated destructor stub
}

