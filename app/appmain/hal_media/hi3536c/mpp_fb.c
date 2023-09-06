/*
 * mpp_fb.c
 *
 *  Created on: 2021年12月12日
 *      Author: jhting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <assert.h>

#include "mpp_def.h"
#include "sample_comm.h"



//for gui
static HiFbInfo stHiG0Info;

//for mouse
static HiFbInfo stHiG3Info;


static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

static HiFbInfo* getFbInfoByLayerID(int layer)
{
	if(GRAPHICS_LAYER_G0 == layer)
	{
		return &stHiG0Info;
	}else if(GRAPHICS_LAYER_HC0 == layer)
	{
		return &stHiG3Info;
	}else{
		assert(0);
		return NULL;
	}
}


int hifb_open(int layer)
{
	int ret;
	int fd;
	HI_BOOL bShow;
	HIFB_POINT_S stPoint = {0, 0};
	char file[32] = "/dev/fb0";
	HIFB_COLORKEY_S	stColorKey;

	HiFbInfo* pstHiFBInfo = NULL;

	if(GRAPHICS_LAYER_G0 == layer)
	{
		pstHiFBInfo = &stHiG0Info;
		pstHiFBInfo->layerId = GRAPHICS_LAYER_G0;
	}
	else if(GRAPHICS_LAYER_HC0 == layer)
	{
		pstHiFBInfo = &stHiG3Info;
		pstHiFBInfo->layerId = GRAPHICS_LAYER_HC0;
	}else{
		assert(0);
	}

	struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;

	HI_U8 *pShowScreen = NULL;

	switch (layer)
	{
		case GRAPHICS_LAYER_G0:
			snprintf(file, 32, "/dev/fb0");
			break;
		case GRAPHICS_LAYER_HC0:
			snprintf(file, 32, "/dev/fb3");
			break;
		default:
			snprintf(file, 32, "/dev/fb0");
			break;
	}
	/* 1. open framebuffer device overlay 0 */
	fd = open(file, O_RDWR, 0);
	if(fd < 0)
	{
		MPP_PRT("open %s failed!\n",file);
		return HI_FAILURE;
	}

	bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        MPP_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_FAILURE;
    }
	 /* 2. set the screen original position */
	 if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        MPP_PRT("set screen original show position failed!\n");
		close(fd);
        return HI_FAILURE;
    }
	/* 3. get the variable screen info */
	 if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        MPP_PRT("Get variable screen info failed!\n");
		close(fd);
        return HI_FAILURE;
    }

	/* 4. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    	*/
 	if(HIFB_COLOR_FMT_PIXEL_BYTES == 4)
 	{
		var.transp= s_a32;
	    var.red = s_r32;
	    var.green = s_g32;
	    var.blue = s_b32;
 	}else{
		var.transp= s_a16;
	    var.red = s_r16;
	    var.green = s_g16;
	    var.blue = s_b16;
	}

	var.bits_per_pixel = HIFB_COLOR_FMT_PIXEL_BYTES * 8;
	var.activate = FB_ACTIVATE_NOW;

	switch(layer)
    {
        case GRAPHICS_LAYER_HC0:
        {
            var.xres_virtual = pstHiFBInfo->resolution_x;
            var.yres_virtual = pstHiFBInfo->resolution_y;
            var.xres = pstHiFBInfo->resolution_x;
            var.yres = pstHiFBInfo->resolution_y;
        }
		break;
        default:
        {
            var.xres_virtual = pstHiFBInfo->resolution_x;
            var.yres_virtual = pstHiFBInfo->resolution_y;
            var.xres = pstHiFBInfo->resolution_x;
            var.yres = pstHiFBInfo->resolution_y;
        }
    }

	/* 5. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        MPP_PRT("Put variable screen info failed!\n");
		close(fd);
        return HI_FAILURE;
    }

    /* 6. get the fix screen info */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        MPP_PRT("Get fix screen info failed!\n");
		close(fd);
        return HI_FAILURE;
    }

	/*all layer surport colorkey*/
	//if(layer == GRAPHICS_LAYER_HC0)
	{
		stColorKey.bKeyEnable = HI_TRUE;
		if(GRAPHICS_LAYER_HC0 == layer)
		{
			stColorKey.bKeyEnable = HI_TRUE;
			stColorKey.u32Key = 0x0;
		}else{
			if(HIFB_COLOR_FMT_PIXEL_BYTES == 4)
				stColorKey.u32Key = 0x080808;
			else
				stColorKey.u32Key = 0x421;
		}

		if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
		{
	    	MPP_PRT("FBIOPUT_COLORKEY_HIFB!\n");
	    	close(fd);
	    	return HI_FAILURE;
		}
	}
    //u32FixScreenStride = fix.line_length;   /*fix screen stride*/

    /* 7. map the physical video memory for user use */

	if(pstHiFBInfo->refresh_mode == STANDARD_MODE)
	{
		pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

		if(MAP_FAILED == pShowScreen)
	    {
	        MPP_PRT("mmap framebuffer failed!\n");
			close(fd);
	        return HI_FAILURE;
	    }

		memset(pShowScreen, 0xdd, fix.smem_len);
	}
	HIFB_ALPHA_S stAlpha;
	memset(&stAlpha, 0x0, sizeof(HIFB_ALPHA_S));

	if (ioctl(fd, FBIOGET_ALPHA_HIFB, &stAlpha) < 0)
    {
        MPP_PRT("FBIOGET_ALPHA_HIFB failed!\n");
		close(fd);
		munmap(pShowScreen, pstHiFBInfo->fix.smem_len);
        return HI_FAILURE;
    }

	if(GRAPHICS_LAYER_G0 == layer || GRAPHICS_LAYER_HC0 == layer)
	{
		stAlpha.bAlphaEnable = HI_FALSE;
		stAlpha.bAlphaChannel = HI_FALSE;
		stAlpha.u8GlobalAlpha = 255;
		stAlpha.u8Alpha0 = 255;
		stAlpha.u8Alpha1 = 0;
		if (ioctl(fd, FBIOPUT_ALPHA_HIFB, &stAlpha) < 0)
	    {
	        MPP_PRT("FBIOGET_ALPHA_HIFB failed!\n");
			close(fd);
			munmap(pShowScreen, pstHiFBInfo->fix.smem_len);
	        return HI_FAILURE;
	    }
	}
	if(GRAPHICS_LAYER_G0 == layer && pstHiFBInfo->refresh_mode == BUFF_MODE)
	{
		HIFB_LAYER_INFO_S stLayerInfo = {0};
		stLayerInfo.BufMode = HIFB_LAYER_BUF_DOUBLE_IMMEDIATE;
		stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
		if (ioctl(fd, FBIOPUT_LAYER_INFO, &stLayerInfo) < 0)
	    {
	        MPP_PRT("FBIOGET_ALPHA_HIFB failed!\n");
			close(fd);
	        return HI_FAILURE;
	    }

		ret = HI_MPI_SYS_MmzAlloc(&(pstHiFBInfo->stCanvasBuf.stCanvas.u32PhyAddr), ((void**)&pstHiFBInfo->pCanvasBufVir),
					NULL, NULL, var.xres*var.yres * HIFB_COLOR_FMT_PIXEL_BYTES);
		if (HI_FAILURE == ret)
		{
			MPP_PRT("allocate memory (maxW*maxH*2 bytes) failed\n");
			close(fd);
			return HI_FAILURE;
		}

		pstHiFBInfo->stCanvasBuf.stCanvas.u32Height = var.yres;
		pstHiFBInfo->stCanvasBuf.stCanvas.u32Width = var.xres;
		pstHiFBInfo->stCanvasBuf.stCanvas.u32Pitch = var.xres * HIFB_COLOR_FMT_PIXEL_BYTES;
		pstHiFBInfo->stCanvasBuf.stCanvas.enFmt = HIFB_COLOR_FMT;
		memset(pstHiFBInfo->pCanvasBufVir, 0x0, pstHiFBInfo->stCanvasBuf.stCanvas.u32Pitch*pstHiFBInfo->stCanvasBuf.stCanvas.u32Height);
	}

	bShow = HI_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        MPP_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_FAILURE;
    }

	if(pstHiFBInfo->refresh_mode == STANDARD_MODE)
	{
		ioctl(fd, FBIOPAN_DISPLAY, &var);
	}

	pstHiFBInfo->fd = fd;
	pstHiFBInfo->fix = fix;
	pstHiFBInfo->var = var;
	pstHiFBInfo->pShowScreen = pShowScreen;
	printf("layer:%d var.xres:%d, var.yres:%d\n", layer, var.xres, var.yres);

	return HI_SUCCESS;
}


void hifb_close(int layer)
{
	HiFbInfo* pstHiFBInfo = NULL;

	pstHiFBInfo = getFbInfoByLayerID(layer);

	if(pstHiFBInfo->fd <= 0)
	{
		return;
	}

	if(pstHiFBInfo->refresh_mode == STANDARD_MODE)
	{
		munmap(pstHiFBInfo->pShowScreen, pstHiFBInfo->fix.smem_len);
		pstHiFBInfo->pShowScreen = NULL;
	}

	if(GRAPHICS_LAYER_G0 == layer && pstHiFBInfo->refresh_mode == BUFF_MODE)
	{
		HI_MPI_SYS_MmzFree(pstHiFBInfo->stCanvasBuf.stCanvas.u32PhyAddr, pstHiFBInfo->pCanvasBufVir);
		pstHiFBInfo->pCanvasBufVir = NULL;
	}

	int bShow = HI_FALSE;
    ioctl(pstHiFBInfo->fd, FBIOPUT_SHOW_HIFB, &bShow);
	close(pstHiFBInfo->fd);
	pstHiFBInfo->fd = -1;

}


unsigned long hifb_getLayerVirtAddr(int layer)
{
	HiFbInfo* pstHiFBInfo = NULL;

	pstHiFBInfo = getFbInfoByLayerID(layer);

	return pstHiFBInfo->fix.smem_start;
}

HiFbInfo* hifb_getLayerInfo(int layer)
{
	return getFbInfoByLayerID(layer);
}


void hifb_g0Refresh()
{
	HI_S32 s32Ret;

	if(stHiG0Info.refresh_mode == STANDARD_MODE)
		return;

	s32Ret = ioctl(stHiG0Info.fd, FBIOGET_VBLANK_HIFB);
	 if(s32Ret < 0)
	{
		MPP_PRT("wait for FBIOGET_VBLANK_HIFB failed!\n");
	}

	stHiG0Info.stCanvasBuf.UpdateRect.x = 0;
	stHiG0Info.stCanvasBuf.UpdateRect.y = 0;
	stHiG0Info.stCanvasBuf.UpdateRect.w = stHiG0Info.var.xres;
	stHiG0Info.stCanvasBuf.UpdateRect.h = stHiG0Info.var.yres;

	//printf("hifb_g0Refresh x:%d y:%d w:%d h:%d\n", stHiG0Info.stCanvasBuf.UpdateRect.x, stHiG0Info.stCanvasBuf.UpdateRect.y,
	//	stHiG0Info.stCanvasBuf.UpdateRect.w, stHiG0Info.stCanvasBuf.UpdateRect.h);

    s32Ret = ioctl(stHiG0Info.fd, FBIO_REFRESH, &stHiG0Info.stCanvasBuf);
    if(s32Ret < 0)
	{
		MPP_PRT("REFRESH failed!\n");
	}
}

int hifb_getfbMode()
{
	return 	stHiG0Info.refresh_mode;
}

int hifb_setCursorPos(int x, int y)
{
	HIFB_POINT_S stPoint = {0, 0};
	stPoint.s32XPos= x;
	stPoint.s32YPos= y;
	/*
	if(x < 0 || y < 0)
	{
		return HI_FAILURE;
	}

	if(x > stHiG0Info.var.xres || y > stHiG0Info.var.yres)
	{
		return HI_FAILURE;
	}
	*/
	if(stHiG3Info.fd <= 0)
		return HI_FAILURE;
	 /* 2. set the screen original position */
	 if (ioctl(stHiG3Info.fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        MPP_PRT("set screen original show position failed!\n");
        return HI_FAILURE;
    }

	return HI_SUCCESS;
}

int hifb_setAlphaEx(int value)
{
	if(stHiG0Info.fd <= 0)
	{
		return HI_FAILURE;
	}

	HIFB_ALPHA_S stAlpha;
	memset(&stAlpha, 0x0, sizeof(HIFB_ALPHA_S));

	if (ioctl(stHiG0Info.fd, FBIOGET_ALPHA_HIFB, &stAlpha) < 0)
    {
        MPP_PRT("FBIOGET_ALPHA_HIFB failed!\n");
        return HI_FAILURE;
    }

	if(value >= 100)
	{
		value = 255;
		stAlpha.bAlphaChannel = HI_FALSE;
	}
	else
	{
		value = value % 100;
		value = 127 + (value / (float)100) * 128;
		stAlpha.bAlphaChannel = HI_TRUE;
	}

	stAlpha.bAlphaEnable = HI_FALSE;
	stAlpha.u8GlobalAlpha = value;
	stAlpha.u8Alpha0 = 255;
	stAlpha.u8Alpha1 = 0;
	if (ioctl(stHiG0Info.fd, FBIOPUT_ALPHA_HIFB, &stAlpha) < 0)
    {
        MPP_PRT("FBIOGET_ALPHA_HIFB failed!\n");
        return HI_FAILURE;
    }

	return HI_SUCCESS;
}




int mpp_fb_init(int xRes, int yRes, int fbdevDoubleBufferMode)
{
	int ret;
	memset(&stHiG0Info, 0, sizeof(HiFbInfo));
	memset(&stHiG3Info, 0, sizeof(HiFbInfo));

	stHiG0Info.resolution_x = xRes;
	stHiG0Info.resolution_y = yRes;

	stHiG3Info.resolution_x = 48;
	stHiG3Info.resolution_y = 48;

	if(fbdevDoubleBufferMode)
	{
		stHiG0Info.refresh_mode = BUFF_MODE;
	}
	else{
		stHiG0Info.refresh_mode = STANDARD_MODE;
	}
	stHiG3Info.refresh_mode = STANDARD_MODE;

	if(stHiG0Info.refresh_mode == BUFF_MODE)
	{
		printf("GRAPHICS_LAYER use BUFF_MODE\n");
	}else{
		printf("GRAPHICS_LAYER use STANDARD_MODE\n");
	}

	//ret = hifb_open(GRAPHICS_LAYER_HC0);
	//assert(ret == 0);
	return hifb_open(GRAPHICS_LAYER_G0);

	return ret == 0;
}

void mpp_fb_exit()
{
	hifb_close(GRAPHICS_LAYER_HC0);
	hifb_close(GRAPHICS_LAYER_G0);
}


int mpp_fb_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo)
{
	if(stHiG0Info.fd <= 0)
	{
		return -1;
	}

	*fbp = (char*)stHiG0Info.pShowScreen;
	memcpy(pStVinfo, &stHiG0Info.var, sizeof(stHiG0Info.var));
	memcpy(pStFixinfo, &stHiG0Info.fix, sizeof(stHiG0Info.fix));
	return 0;
}

