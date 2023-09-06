/*
 * hal_media.c
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#include "hal_media.h"
#include "fy_mpi_def.h"
#include "fy_mpi_sys.h"
#include "fy_mpi_vo.h"
#include "fy_mpi_fb.h"

int 	hal_media_init()
{

	int ret = 0;
	FY_MPI_PRT("hal_media_init begin!\n");
	ret = fy_mpi_sys_init();
	if(ret != 0)
	{
		FY_MPI_PRT("hal_media_init failure!\n");
	}

	ret = fy_mpi_vo_init();
	if(ret != 0)
	{
		FY_MPI_PRT("fy_mpi_vo_init failure!\n");
	}

	ret= fy_mpi_fb_init();
	if(ret != 0)
	{
		FY_MPI_PRT("fy_mpi_fb_init failure!\n");
	}

	FY_MPI_PRT("hal_media_init ok!\n");

	return 0;
}

void	hal_media_exit()
{
	fy_mpi_fb_exit();
	fy_mpi_vo_exit();
	fy_mpi_sys_exit();
	FY_MPI_PRT("hal_media_exit!\n");
	return;
}

int hal_media_fbInit()
{
	int xres = 1920;
	int yres = 1080;
	int ret = 0;

	FY_MPI_PRT("mpp_fb_init xres:%d yres:%d\n", xres, yres);
	//ret = mpp_fb_init(xres, yres, 0);

	return ret;
}

void hal_media_fbexit()
{
	return;
}


int     hal_media_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo)
{
	return fy_mpi_fb_get_framebufferInfo(fbp, pStVinfo, pStFixinfo);
}