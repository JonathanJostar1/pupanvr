/*
 * TAppConfig.cpp
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#include "TAppConfig.h"
#include "iniparse/iniparser.h"
#include "iniparse/dictionary.h"

#include "tlog.h"

#include<stdio.h>

TAppConfig* TAppConfig::m_instance = NULL;
static dictionary *gIniConfigDic = NULL;

#define	APPSYSCONFIGFILE	"/etc/config/appconfig.ini"
#define APPCONFIG_entry_name	"appconfig"



TAppConfig::TAppConfig()
{
	loadDefaultConfig();
}

TAppConfig* TAppConfig::getInstance()
{
	if(!m_instance)
	{
		m_instance = new TAppConfig();
	}

	return m_instance;
}

TAppConfig::~TAppConfig()
{
	if(gIniConfigDic)
	{
		iniparser_freedict(gIniConfigDic);
	}
}

bool TAppConfig::loadConfig()
{
	if(gIniConfigDic)
	{
		iniparser_freedict(gIniConfigDic);
	}

	loadDefaultConfig();

	gIniConfigDic = iniparser_load(APPSYSCONFIGFILE);
	if(!gIniConfigDic)
	{
		LOG(ERROR) << "iniparser_load failure! " << APPSYSCONFIGFILE << endl;
		return false;
	}

	m_devName = iniparser_getstring(gIniConfigDic, "appconfig:devName", NULL);

	return true;
}

void TAppConfig::loadDefaultConfig()
{
	m_devName = "默认名称";

}

bool TAppConfig::saveConfig()
{
	if(!gIniConfigDic)
	{
		return false;
	}

	FILE* fp = fopen(APPSYSCONFIGFILE, "w");
	if(!fp)
	{
		LOG(ERROR) << "打开文件失败!" << endl;
		return false;
	}

	iniparser_dump_ini(gIniConfigDic, fp);

	fclose(fp);

	return true;
}

string TAppConfig::getDevName()
{
	return m_devName;
}

void TAppConfig::setDevName(const string &devName)
{
	m_devName = devName;
}
