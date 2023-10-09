#include<stdio.h>
#include<unistd.h>

#include "tlog.h"
#include "TAppComm.h"
#include "TDvsAppConfig.h"
#include "TRecordManage.h"
#include "git_log_version_number.h"

#include "hal_media.h"

int sys_init()
{
	/**日记初始化*/
	TLog::getInstance()->log_init("DVS_APP");
	LOG(INFO) << "系统初始化开始" << endl;

	LOG(INFO) << "软件版本:" << TAppComm::getAppVersion() << endl;
	LOG(INFO) << "编译信息:" << TAppComm::getAppBuildDataInfo() << endl;
	LOG(INFO) << "GIT信息:" << TAppComm::getAppGitVersionInfo() << endl;

	/**配置初始化*/
	TDvsAppConfig::getInstance()->load();
	TDvsAppConfig::getInstance()->save();

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

	ret = hal_media_init();
	if(ret != 0)
	{
		LOG(ERROR) << "hal_media_init failure!" << endl;
		return -1;
	}

	if(!TRecordManage::getInstance()->sysRecordInit())
	{
		LOG(ERROR) << "sysRecordInit failure!" << endl;
		return -1;
	}
	/**启动录相系统管理服务*/
	TRecordManage::getInstance()->start();

	do{
		sleep(1);
	}while(1);


	return 0;
}
