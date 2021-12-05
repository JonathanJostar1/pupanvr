/*
 * TAppConfig.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_APPCONFIG_TAPPCONFIG_H_
#define APPMAIN_APPMAIN_APPCONFIG_TAPPCONFIG_H_

#include "TAppComm.h"

class TAppConfig
{
public:
	static TAppConfig* getInstance();
	/**
	 *加载配置
	 */
	bool	loadConfig();
	/**
	 *保存配置
	 */
	bool	saveConfig();

	string	getDevName();
	void	setDevName(const string &devName);
private:
	TAppConfig();
	virtual ~TAppConfig();
private:
	void	loadDefaultConfig();
private:
	static TAppConfig* m_instance;
	/*设备名称*/
	string	m_devName;
};

#endif /* APPMAIN_APPMAIN_APPCONFIG_TAPPCONFIG_H_ */
