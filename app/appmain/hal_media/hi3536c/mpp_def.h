/*
 * mpp_def.h
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_
#define APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_

#include<stdio.h>
#include<string.h>

#include "hifb.h"
#include "sample_comm.h"

#define MPP_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

typedef struct{

}StHalMediaContextInfo;

StHalMediaContextInfo*	mpp_sys_getSthalMediaContextInfo();


#define HIFB_COLOR_FMT				HIFB_FMT_ARGB8888
#define HIFB_COLOR_FMT_PIXEL_BYTES	(4)

//#define HIFB_COLOR_FMT					HIFB_FMT_ARGB1555
//#define HIFB_COLOR_FMT_PIXEL_BYTES		(2)


typedef enum{
	STANDARD_MODE = 0,
	BUFF_MODE = 1
}FBRefreshMode;

typedef struct{
	int resolution_x;
	int resolution_y;

	int fd;
	int layerId;

	FBRefreshMode refresh_mode;

	unsigned char *pShowScreen;
	unsigned char *pCanvasBufVir;
	HIFB_BUFFER_S stCanvasBuf;

	struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
}HiFbInfo;


#define GRAPHICS_LAYER_HC0 3
#define GRAPHICS_LAYER_G0  0
#define GRAPHICS_LAYER_G1  1


int 	mpp_sys_init();
void 	mpp_sys_exit();

int 	mpp_vo_init();
void 	mpp_vo_exit();

int 	mpp_fb_init(int xRes, int yRes, int fbdevDoubleBufferMode);
void 	mpp_fb_exit();
int     mpp_fb_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo);

#endif /* APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_ */
