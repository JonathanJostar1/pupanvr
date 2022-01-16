/*
 * mpp_sys.c
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_SYS_C_
#define APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_SYS_C_

#include "sample_comm.h"
#include "mpp_def.h"

static StHalMediaContextInfo g_sthalMediaContextInfo;

/*这里应该根据产品的具体需求来定义VB的分配*/
static int _mpp_sys_vb_init()
{
	int s32Ret = 0;
	VB_CONF_S stVbConf;
	HI_U32 u32BlkSize;
	memset(&stVbConf,0,sizeof(VB_CONF_S));

	int u32PicWidth = 1280;
	int u32PicHeight = 720;

	u32BlkSize = CEILING_2_POWER(u32PicWidth,SAMPLE_SYS_ALIGN_WIDTH)\
			* CEILING_2_POWER(u32PicHeight,SAMPLE_SYS_ALIGN_WIDTH) *2;

	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;

	stVbConf.astCommPool[0].u32BlkSize = 1280*720*2;
	stVbConf.astCommPool[0].u32BlkCnt =  6;
	memset(stVbConf.astCommPool[0].acMmzName,0,sizeof(stVbConf.astCommPool[0].acMmzName));

	do{
		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if(s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
			break;
		}

		return 0;
	}while(0);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

StHalMediaContextInfo*	mpp_sys_getSthalMediaContextInfo()
{
	return &g_sthalMediaContextInfo;
}

int 	mpp_sys_init()
{
	int ret = 0;
	ret = _mpp_sys_vb_init();
	if(ret != 0)
	{
		MPP_PRT("_mpp_sys_vb_init failure! ret:%x", ret);
	}

	return 0;
}

void 	mpp_sys_exit()
{
	SAMPLE_COMM_SYS_Exit();
}



#endif /* APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_SYS_C_ */
