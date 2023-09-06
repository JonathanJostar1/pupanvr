#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "mpi_sys.h"
#include "mpi_fb.h"
#include "sample_comm.h"
#include "sample_ui.h"
//#include "detrect_draw.h"
#include "fy_comm_nna.h"
#include "mpi_nna.h"
#include "mpi_tde.h"
#include "sample_vpu.h"
#include "nna_sample.h"

#define NNA_PERSON_CFG "persondet_512_288.nbg"
#define NNA_PERSON_CAR_CFG "c2det_512_288.nbg"
#define NNA_FACE_CFG "face_det_512_288.nbg"

#define DET_IMAGE_WIDTH    (512)
#define DET_IMAGE_HEIGHT   (288)

#define DEFAULT_SCREEN_W 1920
#define DEFAULT_SCREEN_H 1080

#define VPSS_GRP_DEFAULT 0
#define VPSS_GRP_HWACCEL 1
#define YC_CHN_DEFAULT VPSS_CHN1
#define RGB_CHN_DEFAULT VPSS_CHN5

#define DEFAULT_W 960
#define DEFAULT_H 1088

#define DEFAULT_CHN2_W 576//224
#define DEFAULT_CHN2_H 324//224
#define PIC_CHN2_H 324//126

#define DEFAULT_SCREEN_W 1920
#define DEFAULT_SCREEN_H 1080

#define DEFAULT_YC_TIMEOUT 2000

#define COLOR_MOTION 0x83E0
#define COLOR_FLASSIER 0xE083

FY_S32 g_nna_TDEfd=-1;
static pthread_t g_sample_nna_thrd = 0;
static FY_S32 g_nna_FBfd=-1;
static FY_U32 g_nna_FBPhyAddr=0;
static pthread_mutex_t  g_nna_Mutex = PTHREAD_MUTEX_INITIALIZER;
static FY_U8 g_nna_det_num=0;
static FY_TYPE_E g_nna_det_type=FN_DET_TYPE_PERSON;
static FY_BOOL g_nna_test_start = FY_FALSE;
static FY_BOOL g_nna_EnDrawBox = FY_TRUE;

#define DEFAULT_LINE_W 1
#define ACTURE_LINE_W 4

#define NNA_UI_DEV_NAME "/dev/fb0"

char *aeDetType[FN_DET_TYPE_PERSON_V0] = {"person", "person&car", "face"};

FY_S32 nna_ui_get_draw_addr()
{
    return g_nna_FBPhyAddr;
}

FY_S32 nna_ui_clear_screen()
{
	FY_S32 ret;
	TDE2_SURFACE_S stDst;
    TDE2_RECT_S stDstRect;
    FY_S32 tde_handle;
    FY_U32 addr = nna_ui_get_draw_addr();

    if(addr == 0)
        return FY_FAILURE;

    tde_handle = FY_TDE2_BeginJob();
	stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stDst.u32Width = DEFAULT_SCREEN_W;
    stDst.u32Height = DEFAULT_SCREEN_H;
    stDst.u32Stride = DEFAULT_SCREEN_W*2;
    stDst.u32PhyAddr = addr;
    stDst.bAlphaMax255 = 1;
    stDst.bAlphaExt1555 = 0;
    stDst.u8Alpha0 = 0;
    stDst.u8Alpha1 = 255;
    stDstRect.s32Xpos = 0;
    stDstRect.s32Ypos = 0;
    stDstRect.u32Width = DEFAULT_SCREEN_W;
    stDstRect.u32Height = DEFAULT_SCREEN_H;
	ret = FY_TDE2_QuickFill(tde_handle, &stDst, &stDstRect, 0);
    if (ret)
    {
        printf("nna_ui_clear_screen failed! ret:0x%x(%d)\n",ret,ret);
        FY_TDE2_CancelJob(tde_handle);
        return ret;
    }

	ret = FY_TDE2_EndJob(tde_handle, 1, 1, 500);
    if (ret)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x(%d)\n",ret,ret);
    }

    return ret;
}

FY_S32 nna_ui_init(FY_S32* pFd)
{
	FY_S32 fd;
	FY_BOOL bShow;
	struct fb_fix_screeninfo stFixInfo;
    struct fb_var_screeninfo stVarInfo;
	struct fb_bitfield stR32 = {10, 5, 0};
    struct fb_bitfield stG32 = {5, 5, 0};
    struct fb_bitfield stB32 = {0, 5, 0};
    struct fb_bitfield stA32 = {15, 1, 0};

	//step.1 open fb
	fd = open(NNA_UI_DEV_NAME, O_RDWR, 0);
    if(fd < 0)
    {
        printf("[nna] open /dev/fb0 failed!\n");
        goto error;
    }

	//step.2 disable fb
	bShow = FY_FALSE;
	if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
	{
		printf("[nna] FBIOPUT_SHOW_FYFB failed!\n");
        goto error;
	}

	//step.3 get the variable screen info
    if (ioctl(fd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("[nna] Get variable screen info failed!\n");
        goto error;
    }

	//step.4 modify the variable screen info
	stVarInfo.xres_virtual	 	= DEFAULT_SCREEN_W;
    stVarInfo.yres_virtual		= DEFAULT_SCREEN_H;
    stVarInfo.xres      		= DEFAULT_SCREEN_W;
    stVarInfo.yres      		= DEFAULT_SCREEN_H;
    stVarInfo.activate  		= FB_ACTIVATE_NOW;
    stVarInfo.bits_per_pixel	= 16;
    stVarInfo.xoffset = 0;
    stVarInfo.yoffset = 0;
    stVarInfo.red   = stR32;
    stVarInfo.green = stG32;
    stVarInfo.blue  = stB32;
    stVarInfo.transp = stA32;
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("[nna] process frame buffer device error\n");
        goto error;
    }

	//step.5 fix the variable screen info
    if (ioctl(fd, FBIOGET_FSCREENINFO, &stFixInfo) < 0)
    {
        printf("[nna] process frame buffer device error\n");
        goto error;
    }
	g_nna_FBPhyAddr = stFixInfo.smem_start;
	*pFd = fd;

    bShow = FY_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        printf("[nna] FBIOPUT_SHOW_FYFB failed!\n");
    }

    FYFB_ALPHA_S stAlpha = {0};
    stAlpha.bAlphaChannel = FY_TRUE;
    stAlpha.bAlphaEnable = FY_TRUE;
    stAlpha.u8Alpha0 = 0;
    stAlpha.u8Alpha1 = 255;
    stAlpha.u8GlobalAlpha = 128;
    if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
    {
        printf("Put alpha info failed!\n");
        goto error;
    }

	return FY_SUCCESS;
error:
	return FY_FAILURE;
}

FY_S32 nna_ui_deinit(FY_S32* pFd)
{
	if(pFd == FY_NULL || *pFd == -1)
	{
		return FY_SUCCESS;
	}
	nna_ui_clear_screen();
	close(*pFd);
	*pFd=-1;
	return FY_SUCCESS;
}

FY_S32 nna_open_tde()
{
	return FY_TDE2_Open();
}

FY_S32 nna_draw_box(FY_U32 pAddr,FY_U32 x,FY_U32 y,FY_U32 w,FY_U32 h,FY_U32 color)
{
	FY_S32 ret;
	TDE2_SURFACE_S stDst;
    TDE2_RECT_S stDstRect;


	if(x>DEFAULT_SCREEN_W)
	{
		x=DEFAULT_SCREEN_W;
	}
	if(y>DEFAULT_SCREEN_H)
	{
		y=DEFAULT_SCREEN_H;
	}
	//printf("[acture]x %d y %d w %d h %d\n",x,y,w,h);
	if((x+w)>DEFAULT_SCREEN_W)
	{
		printf("[nna] width error(%d -- %d)\n",x,w);
		goto error;
	}
	if((DEFAULT_H > DEFAULT_SCREEN_H) && (y+h)>DEFAULT_H)
	{
		printf("[nna] width error(%d -- %d)\n",y,h);
		goto error;
	}
	else if((y+h)>DEFAULT_SCREEN_H)
	{
		h -= (y+h)-DEFAULT_SCREEN_H;
	}
    FY_S32 tde_handle;

    tde_handle = FY_TDE2_BeginJob();

	stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stDst.u32Width = DEFAULT_SCREEN_W;
    stDst.u32Height = DEFAULT_SCREEN_H;
    stDst.u32Stride = DEFAULT_SCREEN_W*2;
    stDst.u32PhyAddr = pAddr;
    stDst.bAlphaMax255 = 1;
    stDst.bAlphaExt1555 = 0;
    stDst.u8Alpha0 = 0;
    stDst.u8Alpha1 = 255;
    stDstRect.s32Xpos = x;
    stDstRect.s32Ypos = y;
    stDstRect.u32Width = w>ACTURE_LINE_W?ACTURE_LINE_W:DEFAULT_LINE_W;
    stDstRect.u32Height = h;
	//printf("[fill 1]%d-%d-%d-%d\n",stDstRect.s32Xpos,stDstRect.s32Ypos,stDstRect.u32Width,stDstRect.u32Height);
	ret = FY_TDE2_QuickFill(tde_handle, &stDst, &stDstRect, color);
	if (ret)
    {
        goto error;
    }
	stDstRect.s32Xpos = x;
    stDstRect.s32Ypos = y;
    stDstRect.u32Width = w;
    stDstRect.u32Height = h>ACTURE_LINE_W?ACTURE_LINE_W:DEFAULT_LINE_W;
	//printf("[fill 2]%d-%d-%d-%d\n",stDstRect.s32Xpos,stDstRect.s32Ypos,stDstRect.u32Width,stDstRect.u32Height);
	ret = FY_TDE2_QuickFill(tde_handle, &stDst, &stDstRect, color);
	if (ret)
    {
        goto error;
    }
	stDstRect.s32Xpos = w>ACTURE_LINE_W*2?((x+w)-ACTURE_LINE_W):((x+w)-DEFAULT_LINE_W);
    stDstRect.s32Ypos = y;
    stDstRect.u32Width = w>ACTURE_LINE_W*2?ACTURE_LINE_W:DEFAULT_LINE_W;
    stDstRect.u32Height = h;
	//printf("[fill 3]%d-%d-%d-%d\n",stDstRect.s32Xpos,stDstRect.s32Ypos,stDstRect.u32Width,stDstRect.u32Height);
	ret = FY_TDE2_QuickFill(tde_handle, &stDst, &stDstRect, color);
	if (ret)
    {
        goto error;
    }
	stDstRect.s32Xpos = x;
    stDstRect.s32Ypos = h>ACTURE_LINE_W*2?((y+h)-ACTURE_LINE_W):((y+h)-DEFAULT_LINE_W);
    stDstRect.u32Width = w;
    stDstRect.u32Height = h>ACTURE_LINE_W*2?ACTURE_LINE_W:DEFAULT_LINE_W;;
	//printf("[fill 4]%d-%d-%d-%d\n",stDstRect.s32Xpos,stDstRect.s32Ypos,stDstRect.u32Width,stDstRect.u32Height);
	ret = FY_TDE2_QuickFill(tde_handle, &stDst, &stDstRect, color);
	if (ret)
    {
        goto error;
    }

	ret = FY_TDE2_EndJob(tde_handle, 1, 1, 500);
    if (ret)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x(%d)\n",ret,ret);
    }

    struct fb_var_screeninfo stVarInfo;
    if (ioctl(g_nna_FBfd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("process frame buffer device error\n");
        return FY_FAILURE;
    }


    if (ioctl(g_nna_FBfd, FBIOPAN_DISPLAY, &stVarInfo) < 0)
    {
        printf("display failed\n");
        return FY_FAILURE;
    }

    return ret;

error:
    printf("[nna] FY_TDE2_QuickFill failed! ret:0x%x(%d)\n",ret,ret);
    FY_TDE2_CancelJob(tde_handle);
	return FY_FAILURE;
}

FY_VOID nna_close_tde()
{
    return FY_TDE2_Close();
}

FY_S32 nna_det_init(FY_U32 num)
{
    int ret;

    /* ======2. init ui ======*/
	nna_ui_init(&g_nna_FBfd);
    ret = nna_open_tde();
    if(ret < 0)
    {
        SAMPLE_NNA_PRT("open tde failed errno:%d\n", ret);
        return FY_FAILURE;
    }
    nna_ui_clear_screen();

    return FY_SUCCESS;
}

FY_S32 nna_set_vpss_mode(FY_S32 vpssGrp,FY_S32 vpssChn)
{
    VPSS_CHN_MODE_S stVpssMode={0};
	VPSS_CHN_MODE_S sVpssMode;
    int ret;
    FY_U32 depeth=1;

    memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));
	ret=FY_MPI_VPSS_GetChnMode(vpssGrp,vpssChn,&sVpssMode);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, failure, "FY_MPI_VPSS_GetChnMode grp%d chn%d failed with ret = %#x\n", vpssGrp, vpssChn, ret);

    ret=FY_MPI_VPSS_DisableChn(vpssGrp,vpssChn);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, failure, "FY_MPI_VPSS_DisableChn grp%d chn%d failed with ret = %#x\n", vpssGrp, vpssChn, ret);


    stVpssMode.bDouble = FY_FALSE;
    stVpssMode.u32Width = DET_IMAGE_WIDTH;
    stVpssMode.u32Height = DET_IMAGE_HEIGHT;
    stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
    stVpssMode.enCompressMode = COMPRESS_MODE_NONE;
    stVpssMode.enPixelFormat=PIXEL_FORMAT_RGB_888;//PIXEL_FORMAT_RGB_888;
    stVpssMode.aiCfg.MeansR=0;
    stVpssMode.aiCfg.MeansG=0;
    stVpssMode.aiCfg.MeansB=0;
    stVpssMode.aiCfg.Scale=0x80;
    stVpssMode.aiCfg.AiOutSel=0;//0:rgb 1:yuv
    stVpssMode.aiCfg.RGBOutMode=2; //0: planar 1: interlave 2:ARGB interlave
    stVpssMode.aiCfg.AlphaValue=0x86;
    stVpssMode.aiCfg.ARGBOrder=1;//
    ret=FY_MPI_VPSS_SetChnMode(vpssGrp,vpssChn,&stVpssMode);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, failure, "FY_MPI_VPSS_SetChnMode grp%d chn%d failed with ret = %#x\n", vpssGrp, vpssChn, ret);

    ret=FY_MPI_VPSS_SetDepth(vpssGrp,vpssChn,depeth);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, failure, "FY_MPI_VPSS_SetDepth grp%d chn%d failed with ret = %#x\n", vpssGrp, vpssChn, ret);

    ret=FY_MPI_VPSS_EnableChn(vpssGrp,vpssChn);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, failure, "FY_MPI_VPSS_EnableChn grp%d chn%d failed with ret = %#x\n", vpssGrp, vpssChn, ret);
    return FY_SUCCESS;
failure:
    return FY_FAILURE;
}

static FY_S32 nna_get_draw_rect(FY_RECT_T *pDetRect, RECT_S *pDrawRect, FY_U32 u32WinNum, FY_U32 u32WinIdx)
{
    FY_S32 ret;
    VO_CHN_ATTR_S stChnAttr;

    ret = FY_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD0, u32WinIdx, &stChnAttr);
    SAMPLE_NNA_CHECK_EXPR_RET(ret!=FY_SUCCESS, FY_FAILURE, "FY_MPI_VO_GetChnAttr u32WinIdx[%d], ret = %#x\n", u32WinIdx, ret);


    pDrawRect->s32X = stChnAttr.stRect.s32X + pDetRect->x * (float)stChnAttr.stRect.u32Width;
    pDrawRect->s32Y = stChnAttr.stRect.s32Y + pDetRect->y * (float)stChnAttr.stRect.u32Height;
    pDrawRect->u32Width = pDetRect->w * (float)stChnAttr.stRect.u32Width;
    pDrawRect->u32Height = pDetRect->h * (float)stChnAttr.stRect.u32Height;

//    SAMPLE_NNA_PRT("nna_get_draw_rect, get window[%d] [%d-%d-%d-%d]\n", u32WinIdx, pDrawRect->s32X, pDrawRect->s32Y, pDrawRect->u32Width, pDrawRect->u32Height);

    return FY_SUCCESS;
}

FY_S32 nna_load_cfg_file(const char *file_path, FY_U8 **ppNetCfgData)
{
    FILE* file;
    FY_S32 length, act;
	FY_S32 ret = FY_FAILURE;

    SAMPLE_NNA_CHECK_EXPR_RET(file_path == NULL, FY_FAILURE, "file_path is null\n");
    file = fopen(file_path, "rb");
    SAMPLE_NNA_CHECK_EXPR_RET(file == NULL, FY_FAILURE, "open %s err.\n", file_path);

    fseek(file, 0L, SEEK_END);
    length = ftell(file);
    *ppNetCfgData = malloc(length);
    SAMPLE_NNA_CHECK_EXPR_GOTO(*ppNetCfgData == NULL, exit, "malloc cfg data %d failed.\n", length);

    fseek(file, 0L, SEEK_SET);
    act = fread(*ppNetCfgData, 1, length, file);
    SAMPLE_NNA_CHECK_EXPR_GOTO(act != length, exit_1, "read %s failed.\n", file_path);

    fclose(file);
	return FY_SUCCESS;

exit_1:
    free(*ppNetCfgData);
exit:
    fclose(file);
    return ret;
}

FY_VOID* sample_nna_detect_thread(FY_VOID* pAtr)
{
	FY_S32 ret;
	FY_S32 i,chn;
    FY_U32 modid;
    FY_BOOL u32ChnNum = *(FY_BOOL *)pAtr;
    FY_IMAGE_T stImageInfo;
    FY_DETECTION_T stDetResult;
    FY_NN_INIT_PARAM_T stInitParam;
    FY_VOID *detHandle[32] = {NULL};
    FY_U8 *pNetCfgData =NULL;
    char *pNetCfgFile =NULL;
    FY_FLOAT fConfid = 0.65;
    FY_S32 vpssGrp;

    /* ======1. init nna ======*/
    // load model config file
    switch(g_nna_det_type)
    {
        case FN_DET_TYPE_PERSON:
            pNetCfgFile = NNA_PERSON_CFG;
            fConfid = 0.8;
            break;
        case FN_DET_TYPE_C2:
            pNetCfgFile = NNA_PERSON_CAR_CFG;
            fConfid = 0.65;
            break;
        case FN_DET_TYPE_FACE:
            pNetCfgFile = NNA_FACE_CFG;
            fConfid = 0.8;
            break;
        default:
            break;
    }

    ret = FY_NNA_Init();
    SAMPLE_NNA_CHECK_EXPR_RET(ret != FY_SUCCESS, NULL, "FY_NNA_Init failed with ret = %#x\n", ret);

    modid = 0;
    memset(&stInitParam, 0x0, sizeof(FY_NN_INIT_PARAM_T));

    SAMPLE_NNA_PRT("======%s detect start, load config %s, confid = %f\n", aeDetType[g_nna_det_type-1], pNetCfgFile, fConfid);
    ret = nna_load_cfg_file(pNetCfgFile, &pNetCfgData);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret != FY_SUCCESS, exit_0, "load file '%s' falied\n", pNetCfgFile);

    stInitParam.type = g_nna_det_type;
    stInitParam.src_w_in = DET_IMAGE_WIDTH;
    stInitParam.src_h_in = DET_IMAGE_HEIGHT;
    stInitParam.src_c_in = 4;
    stInitParam.conf_thr = fConfid;
    stInitParam.rotate = FN_ROT_0;
    stInitParam.nbg_data = pNetCfgData;
    ret = FY_NNA_DET_Init(&detHandle[modid], modid,  &stInitParam);
    SAMPLE_NNA_CHECK_EXPR_GOTO(ret!=FY_SUCCESS, exit_1, "FY_NNA_DET_Init failed with ret = %#x\n", ret);

    memset(&stDetResult,0,sizeof(FY_DETECTION_T));

    int cnt=0;
	while(g_nna_test_start)
	{

    	for(chn=0;chn<u32ChnNum;chn++)
		{
            /*==== 1. get src data from vpu====*/

            VIDEO_FRAME_INFO_S frameinfo;
            memset(&frameinfo,0,sizeof(VIDEO_FRAME_INFO_S));
            vpssGrp = chn*2;
            if ((ret = FY_MPI_VPSS_GetChnFrame(vpssGrp,RGB_CHN_DEFAULT,&frameinfo,DEFAULT_YC_TIMEOUT)) != FY_SUCCESS)
            {
                printf("[nna]FY_MPI_VPSS_GetChnFrame error: 0x%x ,grp %d -- chn %d\n", ret,vpssGrp,RGB_CHN_DEFAULT);
                usleep(500);
                continue;
            }

#ifdef DUMP_ARGB
            //dupm data
            if(cnt == 0)
            {
                char *pVirtAddr = FY_MPI_SYS_Mmap(frameinfo.stVFrame.u32PhyAddr[0], frameinfo.stVFrame.u32Width*frameinfo.stVFrame.u32Height*4);
                FILE *file = fopen("dump.argb", "wb");
                if(file == NULL)
                    printf("open dump.argb falied\n");

                printf("=================w=%d, h=%d\n", frameinfo.stVFrame.u32Width, frameinfo.stVFrame.u32Height);
                fwrite(pVirtAddr, frameinfo.stVFrame.u32Width*frameinfo.stVFrame.u32Height*4, 1, file);
                fclose(file);
            }
#endif
            cnt++;

            pthread_mutex_lock(&g_nna_Mutex);
            /*==== 2. detect start ====*/
            stImageInfo.width = frameinfo.stVFrame.u32Width;
            stImageInfo.height = frameinfo.stVFrame.u32Height;
            stImageInfo.imageType = FY_IMAGE_FORMAT_RGB888;
            stImageInfo.src_data.base = frameinfo.stVFrame.u32PhyAddr[0];
            stImageInfo.src_data.vbase = frameinfo.stVFrame.pVirAddr[0];
            stImageInfo.src_data.size =  stImageInfo.width * stImageInfo.height * 4;

            ret = FY_NNA_DET_Process(detHandle[0], &stImageInfo, &stDetResult, -1);
            if(ret != FY_SUCCESS)
                SAMPLE_NNA_PRT("FY_NNA_DET_Process  failed with ret = %#x\n", ret);

            pthread_mutex_unlock(&g_nna_Mutex);

            if ((ret = FY_MPI_VPSS_ReleaseChnFrame(vpssGrp, RGB_CHN_DEFAULT, &frameinfo))!= FY_SUCCESS)
            {
                SAMPLE_NNA_PRT("FY_MPI_VPSS_ReleaseChnFrame error: 0x%x ,grp %d -- chn %d\n", ret,vpssGrp,RGB_CHN_DEFAULT);
            }

            /*==== 3. get detect result from nna ====*/
//			SAMPLE_NNA_PRT("[nna][%d]pos_cnt %d\n",chn,stDetResult.boxNum);
            if(g_nna_EnDrawBox)
            {
                nna_ui_clear_screen();
                for(i=0;i<stDetResult.boxNum;i++)
                {
                   RECT_S drawRect;

                   ret = nna_get_draw_rect(&stDetResult.detBBox[i].bbox, &drawRect, u32ChnNum, chn);
                   if(ret != FY_SUCCESS)
                       continue;

                   ret = nna_draw_box(nna_ui_get_draw_addr(),drawRect.s32X,drawRect.s32Y,drawRect.u32Width,drawRect.u32Height,
                       (stDetResult.detBBox[i].clsType == 0)?COLOR_FLASSIER:COLOR_MOTION);
                }
            }
	    }

	    usleep(5*1000);
	}

    FY_NNA_DET_Exit(detHandle[0]);
    nna_close_tde();
exit_1:
    free(pNetCfgData);
exit_0:
    FY_NNA_Close();
	return FY_NULL;
}

FY_S32 sample_nna_test_start(FY_U8 num, FY_TYPE_E eDetType)
{
    int i;
    int ret = FY_SUCCESS;

    /* ======1. set vpu mode ======*/
    sample_nna_test_stop();

	for(i=0;i<num;i++)
	{
		nna_set_vpss_mode(i*2,RGB_CHN_DEFAULT);
	}

    ret = nna_det_init(num);
    if(ret == FY_FAILURE)
        return ret;

	g_nna_det_num= num;

    g_nna_det_type = eDetType;

	if( !g_nna_test_start )
	{
        g_nna_test_start = FY_TRUE;
		ret = pthread_create(&g_sample_nna_thrd, 0, sample_nna_detect_thread, &g_nna_det_num);
	}
	return ret;

}
FY_S32 sample_nna_test_stop()
{
	if(g_nna_test_start)
	{
		g_nna_test_start = FY_FALSE;

        if(g_sample_nna_thrd)
    		pthread_join(g_sample_nna_thrd, NULL);

		nna_ui_deinit(&g_nna_FBfd);
	}
	return FY_SUCCESS;
}

FY_S32 sample_nna_set_chn_num(FY_U32 u32ChnNum)
{
    if(u32ChnNum < 1 || u32ChnNum > 16) {
        printf("\n\tchannel number %d should be [1,%d]\n", u32ChnNum, 16);
        return FY_FAILURE;
    }

    g_nna_det_num = u32ChnNum;

    return FY_SUCCESS;
}

FY_S32 sample_nna_get_chn_num(FY_U32 *pu32ChnNum)
{
    if(pu32ChnNum == NULL)
        return FY_FAILURE;

    *pu32ChnNum = g_nna_det_num;
    return FY_SUCCESS;
}

FY_S32 sample_nna_disable_draw_box(FY_BOOL bDisable)
{
    g_nna_EnDrawBox = !bDisable;

    return FY_SUCCESS;
}
