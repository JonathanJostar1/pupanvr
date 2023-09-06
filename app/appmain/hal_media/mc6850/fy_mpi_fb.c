#include "fy_mpi_fb.h"
#include "sample_comm.h"
#include "fy_comm_fb.h"
#include "mpi_fb.h"
#include "fy_mpi_def.h"

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
#include <string.h>

#define FMT_ARGB8888    1

#ifndef FMT_ARGB8888
struct fb_bitfield s_red = {10, 5, 0};
struct fb_bitfield s_gre = {5, 5, 0};
struct fb_bitfield s_blu = {0, 5, 0};
struct fb_bitfield s_alp = {15, 1, 0};
FY_U32 bitDep = 16;
FY_U32 byteDep = 2;
#else
struct fb_bitfield s_red = {16, 8, 0};
struct fb_bitfield s_gre = {8, 8, 0};
struct fb_bitfield s_blu = {0, 8, 0};
struct fb_bitfield s_alp = {24, 8, 0};
FY_U32 bitDep = 32;
FY_U32 byteDep = 4;
#endif



typedef struct{
	int resolution_x;
	int resolution_y;

	int fd;
	int layerId;

	unsigned char *pShowScreen;
	
	struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
}StFyMpiFbInfo;

#ifndef FMT_ARGB8888
#define CURSOR_NAME      "res/cursor.bits"
#define COLOR_FORMAT     TDE2_COLOR_FMT_ARGB1555
#else
#define CURSOR_NAME      "res/cursor_8888.bits"
#define COLOR_FORMAT     TDE2_COLOR_FMT_ARGB8888
#endif

static StFyMpiFbInfo gStFyMpiFbInfoFB0;


int _fb_fb0_init()
{
    //return 0;
#if 1
    FY_BOOL bCompress;
    FY_BOOL bComSupport;
    FY_BOOL bShow;
    FY_S32 i;
    FY_S32 fd; 
    FY_S32 s32Ret;
    FY_U32 u32PhyAddr;
    FY_U8* g_pu8BackGroundVir = NULL;
    FY_U8 *pShowScreen = NULL;
    //TDE2_SURFACE_S g_stBackGround;
    //TDE2_SURFACE_S g_stScreen;
    //FY_U8 *pTypt = (FY_U8 *)pData;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_U32 screanWidth, screanHeight;
    FYFB_ALPHA_S stAlpha = {0};    

    struct fb_fix_screeninfo stFixInfo;
    struct fb_var_screeninfo stVarInfo;

    s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stLayerAttr);
    if(s32Ret)
    {
        printf("failed to get vo layer(%d) info!\n", SAMPLE_VO_LAYER_VHD0);
        goto exitFailure;
    }

    fd = open("/dev/fb0", O_RDWR, 0);
    if(fd < 0)
    {
        printf("open /dev/fb0 failed!\n");
        goto exitFailure;
    }

    bComSupport = FY_FALSE;
    //prctl(PR_SET_NAME, "UI_HD");

    screanWidth = stLayerAttr.stImageSize.u32Width;
    screanHeight = stLayerAttr.stImageSize.u32Height;

    bShow = FY_FALSE;
	if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
	{
		SAMPLE_PRT("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitFailure;
	}

    if(bComSupport){
        bCompress = FY_TRUE;
        if (ioctl(fd, FBIOPUT_COMPRESSION_FYFB, &bCompress) < 0)
        {
            printf(" FBIOPUT_COMPRESSION_FYFB failed!\n");
            goto exitFailure;
        }
    }

     /* get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("Get variable screen info failed!\n");
        goto exitFailure;
    }

    stVarInfo.xres_virtual	 	= ALIGN_UP(screanWidth, 8);
    stVarInfo.yres_virtual		= screanHeight;
    stVarInfo.xres      		= ALIGN_UP(screanWidth, 8);
    stVarInfo.yres      		= screanHeight;
    stVarInfo.activate  		= FB_ACTIVATE_NOW;
    stVarInfo.bits_per_pixel	= bitDep;
    stVarInfo.xoffset = 0;
    stVarInfo.yoffset = 0;
    stVarInfo.red   = s_red;
    stVarInfo.green = s_gre;
    stVarInfo.blue  = s_blu;
    stVarInfo.transp = s_alp;

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("process frame buffer device error\n");
        goto exitFailure;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &stFixInfo) < 0)
    {
        printf("process frame buffer device error\n");
        goto exitFailure;
    }

    u32PhyAddr  = stFixInfo.smem_start;
    pShowScreen   = mmap(NULL, stFixInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (NULL == pShowScreen)
    {
        printf("mmap fb0 failed!\n");
        goto exitFailure;
    }

    memset(pShowScreen, 0x00, stFixInfo.smem_len);

#if 0
    g_stScreen.enColorFmt = COLOR_FORMAT;
    g_stScreen.u32PhyAddr = u32PhyAddr;
    g_stScreen.u32Width = screanWidth;
    g_stScreen.u32Height = screanHeight;
    g_stScreen.u32Stride = stFixInfo.line_length;
    g_stScreen.bAlphaMax255 = FY_TRUE;
    g_stScreen.bAlphaExt1555 = FY_TRUE;
    g_stScreen.u8Alpha0 = 0;
    g_stScreen.u8Alpha1 = 255;
#endif

    printf("====================stFixInfo.line_length:%d================\n", stFixInfo.line_length);

    bShow = FY_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitFailure;
    }
#if 0
    if (ioctl(fd, FBIOPAN_DISPLAY, stVarInfo) < 0)
    {
        printf("display failed\n");
        goto exitFailure;
    }
#endif
#if 0
    stAlpha.bAlphaChannel = FY_TRUE;
    stAlpha.bAlphaEnable = FY_TRUE;
    stAlpha.u8Alpha0 = 0;
    stAlpha.u8Alpha1 = 255;
    stAlpha.u8GlobalAlpha = 128;
    if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
    {
        printf("Put alpha info failed!\n");
        goto exitFailure;
    }
#endif
    gStFyMpiFbInfoFB0.resolution_x = screanWidth;
    gStFyMpiFbInfoFB0.resolution_y = screanWidth;
    gStFyMpiFbInfoFB0.fd = fd;
    gStFyMpiFbInfoFB0.pShowScreen = pShowScreen;
    memcpy(&gStFyMpiFbInfoFB0.fix, &stFixInfo, sizeof(stFixInfo));
    memcpy(&gStFyMpiFbInfoFB0.var, &stVarInfo, sizeof(stVarInfo));
    return 0;

exitFailure:
    if(pShowScreen)
    {
        munmap(pShowScreen, stFixInfo.smem_len);    
    }

    if(fd > 0)
    {
        close(fd);
    }
    return -1;
#endif

}

int     fy_mpi_fb_init()
{
    memset(&gStFyMpiFbInfoFB0, 0, sizeof(gStFyMpiFbInfoFB0));
    int ret = _fb_fb0_init();
    if(ret != 0)
    {
        FY_MPI_PRT("fy_mpi_fb_init failure!\n");
    }
    return 0;
}

void    fy_mpi_fb_exit()
{

    return;
}


int     fy_mpi_fb_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo)
{
    if(gStFyMpiFbInfoFB0.fd <= 0)
    {
        return -1;
    }

    *fbp = (char*)gStFyMpiFbInfoFB0.pShowScreen;
    memcpy(pStVinfo, &gStFyMpiFbInfoFB0.var, sizeof(gStFyMpiFbInfoFB0.var));
    memcpy(pStFixinfo, &gStFyMpiFbInfoFB0.fix, sizeof(gStFyMpiFbInfoFB0.fix));
	return 0;
}