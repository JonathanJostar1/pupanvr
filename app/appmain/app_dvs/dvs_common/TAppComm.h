/*
 * TAppComm.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_COMMON_TAPPCOMM_H_
#define APPMAIN_APPMAIN_COMMON_TAPPCOMM_H_

#include<string>

using namespace std;

class TAppComm
{
public:
	/**
	 *获取应用版本信息
	 */
	static const char* getAppVersion();
	/**
	 *获取应用编译时间信息
	 */
	static string getAppBuildDataInfo();
	/**
	 *获取应用GIT版本信息
	 */
	static string getAppGitVersionInfo();
public:
	TAppComm();
	virtual ~TAppComm();
};

#endif /* APPMAIN_APPMAIN_COMMON_TAPPCOMM_H_ */
