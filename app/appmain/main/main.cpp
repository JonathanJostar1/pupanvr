#include<stdio.h>
#include<unistd.h>

#include "tlog.h"
#include "TAppComm.h"
#include "TAppConfig.h"
#include "TRecordManage.h"
#include "git_log_version_number.h"

int sys_init()
{
	/**日记初始化*/
	TLog::getInstance()->log_init();
	LOG(INFO) << "系统初始化开始" << endl;

	LOG(INFO) << "软件版本:" << TAppComm::getAppVersion() << endl;
	LOG(INFO) << "编译信息:"<< TAppComm::getAppBuildDataInfo() << endl;
	LOG(INFO) << "GIT信息:"<< TAppComm::getAppGitVersionInfo() << endl;

	/**配置初始化*/
	TAppConfig::getInstance()->loadConfig();


	return 0;
}

int main(int argc, char** argv)
{
	sys_init();

	/**启动录相系统管理服务*/
	TRecordManage::getInstance()->start();

	while(1)
	{
		
		sleep(1);
	}

	return 0;
}
