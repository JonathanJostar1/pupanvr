#include<stdio.h>
#include<unistd.h>

#include "tlog.h"

int sys_init()
{
	/*日记初始化*/
	TLog::getInstance()->log_init();
	LOG(INFO) << "初始化开始..." << endl;
	/*配置初始化*/

}

int main(int argc, char** argv)
{
	sys_init();

	
	while(1)
	{
		
		sleep(1);
	}
	return 0;
}