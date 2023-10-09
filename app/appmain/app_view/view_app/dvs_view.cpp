#include<stdio.h>
#include<unistd.h>

#include "tlog.h"
#include "TAppComm.h"
#include "TAppGlobalConfig.h"
#include "git_log_version_number.h"

#include "hal_media.h"
#include "view_main.h"

int sys_init()
{
	/**日记初始化*/
	TLog::getInstance()->log_init();
	LOG(INFO) << "系统初始化开始" << endl;

	LOG(INFO) << "软件版本:" << TAppComm::getAppVersion() << endl;
	LOG(INFO) << "编译信息:" << TAppComm::getAppBuildDataInfo() << endl;
	LOG(INFO) << "GIT信息:" << TAppComm::getAppGitVersionInfo() << endl;

	/**配置初始化*/
	TAppGlobalConfig::getInstance()->load();
	TAppGlobalConfig::getInstance()->save();

	return 0;
}

int main(int argc, char** argv)
{
	int ret = 0;
	ret = sys_init();
	if(ret != 0)
	{
		LOG(ERROR) << "sys_init failure! ret:" << ret << endl;
		return -1;
	}

#if 0
	ret = hal_media_init();
	if(ret != 0)
	{
		LOG(ERROR) << "hal_media_init failure!" << endl;
		return -1;
	}
#endif

	ret = view_init();
	if(ret != 0)
	{
		LOG(ERROR) << "view_init failure!" << endl;
		return -1;
	}

	ret = view_process();
	{
		LOG(ERROR) << "view_process failure!" << endl;
		return -1;
	}

	return 0;
}
