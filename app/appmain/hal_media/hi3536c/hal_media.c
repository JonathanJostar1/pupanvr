/*
 * hal_media.c
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#include "hal_media.h"
#include "mpp_def.h"

int 	hal_media_init()
{
	int ret = 0;
	MPP_PRT("hal_media_init begin!\n");
	ret = mpp_sys_init();
	if(ret != 0)
	{
		MPP_PRT("mpp_sys_init failure!\n");
	}

	ret = mpp_vo_init();
	if(ret != 0)
	{
		MPP_PRT("mpp_vo_init failure!\n");
	}

	MPP_PRT("hal_media_init ok!\n");
	return 0;
}

void	hal_media_exit()
{
	mpp_vo_exit();
	mpp_sys_exit();

	MPP_PRT("hal_media_exit!\n");
	return;
}

int hal_media_fbInit()
{
	int xres = 1920;
	int yres = 1080;
	int ret = 0;

	MPP_PRT("mpp_fb_init xres:%d yres:%d\n", xres, yres);
	ret = mpp_fb_init(xres, yres, 0);

	return ret;
}

void hal_media_fbexit()
{
	return;
}

int     hal_media_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo)
{
	return mpp_fb_get_framebufferInfo(fbp, pStVinfo, pStFixinfo);
}
