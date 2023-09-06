/******************************************************************************
  Copyright (C) 2018, YGTek. Co., Ltd.

 ******************************************************************************
    Modification:  2018-12 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

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
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "mpi_tde.h"
#include "sample_comm.h"


#define VALUE_BETWEEN(x,min,max) (((x)>=(min)) && ((x) <= (max)))
#define MAX_BLK_CNT 64
#define BLK_CNT 2 //*VPSS_GRP_NUM_TEST //*4


static pthread_t gs_VpssPid;

SAMPLE_VPSS_SENDSTRM_PARA_S gs_stSendFrmPara;

#ifdef TEST_21A
#define FY_MPI_VPSS_CreateGrp HI_MPI_VPSS_CreateGrp
#define FY_MPI_VPSS_GetGrpParam	HI_MPI_VPSS_GetGrpParam
#define FY_MPI_VPSS_SetGrpParam HI_MPI_VPSS_SetGrpParam
#define FY_MPI_VPSS_SetChnAttr HI_MPI_VPSS_SetChnAttr
#define FY_MPI_VPSS_GetChnMode	HI_MPI_VPSS_GetChnMode
#define	FY_MPI_VPSS_SetChnMode	HI_MPI_VPSS_SetChnMode
#define	FY_MPI_VPSS_EnableChn	HI_MPI_VPSS_EnableChn
#define	FY_MPI_VPSS_StartGrp	HI_MPI_VPSS_StartGrp
#define	FY_MPI_VPSS_StopGrp	HI_MPI_VPSS_StopGrp
#define	FY_MPI_VPSS_DisableChn	HI_MPI_VPSS_DisableChn
#define	FY_MPI_VPSS_DestroyGrp	HI_MPI_VPSS_DestroyGrp
#define FY_MPI_VPSS_GetDepth HI_MPI_VPSS_GetDepth
#define	FY_MPI_VPSS_SetDepth	HI_MPI_VPSS_SetDepth
#define	FY_MPI_VPSS_GetChnFrame	HI_MPI_VPSS_GetChnFrame
#define	FY_MPI_VPSS_ReleaseChnFrame	HI_MPI_VPSS_ReleaseChnFrame
#define FY_MPI_VPSS_SendFrame	HI_MPI_VPSS_SendFrame
#endif

#define TEST_GOSD
#define SHOW_GOSD

#ifdef TEST_GOSD
#include "loadbmp.h"

static const FY_CHAR *pszImageNames[] =
{
    "res/mm.bmp",
    "res/foot.bmp",
    "res/mm.bmp",
    "res/foot.bmp",
    "res/gimp.bmp",
    "res/gsame.bmp",
    "res/keys.bmp"
};

#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif

#define N_IMAGES (FY_S32)((sizeof (pszImageNames) / sizeof (pszImageNames[0])))

#define SHOW_GOSD_NUM 4
#define SHOW_MOSAIC_NUM 4

#define GOSD_BEGIN 0
#define MOSAIC_BEGIN 512

#define DEFAULT_WIDTH  1920
#define DEFAULT_HEIGHT 1080
#define DEFAULT_S32X 0
#define DEFAULT_S32Y 0
#define DEFAULT_RGN_HEIGHT 90

#define RGN_FixelFormt PIXEL_FORMAT_RGB_1555
#define OSD_SURFACE_FixelFormat OSD_COLOR_FMT_RGB1555
FY_U32 g_handle = 0xff;
static FY_U32 gu32ChipID = 0;

static FY_S32 g_picWidthSrc = 332;
static FY_S32 g_picHeightSrc = 20;
static FY_CHAR g_fileNameIn[128] = "res/vppu_resizeOut_332x20.bits";
static FY_U32 g_phyAddr;
static FY_VOID *g_pVirAddr = NULL;

FY_S32 g_byteStep = 2;

static FY_U32 u32MaxOverlay = 8;
static FY_U32 g_heigt_div = 1;

FY_U32 g_invert[VPSS_MAX_GRP_NUM];
static FY_U32 rgb8888torgb1555(FY_U32 rgb8888)
{
	FY_U32 color,alpha = 1;
	if((rgb8888 >> 24) == 0)
		alpha = 0;
	color = (alpha << 15) | (((rgb8888 >> 19) & 0x1f) << 10) |
		(((rgb8888 >> 11) & 0x1f) << 5) | ((rgb8888 >> 3) & 0x1f);
	return color;
}

static FY_U32 rgb8888torgb4444(FY_U32 rgb8888)
{
	FY_U32 color;
	color = (((rgb8888 >> 28) & 0xf) << 12) | (((rgb8888 >> 20) & 0xf) << 8) |
		(((rgb8888 >> 12) & 0xf) << 4) | (((rgb8888 >> 4) & 0xf));
	return color;
}

static FY_U32 rgb8888torgb0565(FY_U32 rgb8888)
{
	FY_U32 color;
	color = (((rgb8888 >> 19) & 0x1f) << 11) | (((rgb8888 >> 10) & 0x3f) << 5) |
		(((rgb8888 >> 3) & 0x1f));
	return color;
}

FY_S32 test_load_pic(FY_VOID)
{
    FILE *pfd =NULL;

    printf("\n--------TDE quick resize test begin--------\n");

    FY_MPI_SYS_MmzAlloc(&g_phyAddr, &g_pVirAddr, "test-ARGB1555", NULL, g_picWidthSrc * g_picHeightSrc * g_byteStep);

    #if 1
    pfd = fopen(g_fileNameIn,"rb");
    if(pfd == NULL)
    {
        printf("open %s  fail!\n", g_fileNameIn);
        return -1;
    }
    fread(g_pVirAddr, 1, g_picWidthSrc * g_picHeightSrc * g_byteStep, pfd);
    fclose(pfd);
    #else
    memcpy(g_pVirAddr,GOSD_DATA,GOSD_DATA_SIZE);
    #endif
    return 0;
}

FY_S32 test_unload_pic(FY_VOID)
{
    FY_MPI_SYS_MmzFree(g_phyAddr, NULL);
    return 0;
}
FY_S32 test_tde_quickResize(RGN_CANVAS_INFO_S *pstCanvasInfo)
{
    FY_S32 ret = 0;
    TDE_HANDLE s32Handle;
    TDE2_SURFACE_S stSrc;
    TDE2_RECT_S stSrcRect;
    TDE2_SURFACE_S stDst;
    TDE2_RECT_S stDstRect;

    ret = FY_TDE2_Open();
    s32Handle = FY_TDE2_BeginJob();

    /* resize param start */
    stSrc.enColorFmt = RGN_FixelFormt;
    stSrc.u32Width = g_picWidthSrc;
    stSrc.u32Height = g_picHeightSrc;
    stSrc.u32Stride = g_picWidthSrc * 2;
    stSrc.u32PhyAddr = g_phyAddr;
    stSrc.bAlphaMax255 = 1;
    stSrc.bAlphaExt1555 = 0;
    stSrc.u8Alpha0 = 0;
    stSrc.u8Alpha1 = 255;
    stSrcRect.s32Xpos = 0;
    stSrcRect.s32Ypos = 0;
    stSrcRect.u32Width = g_picWidthSrc;
    stSrcRect.u32Height = g_picHeightSrc;

    //trans to argb1555
    stDst.enColorFmt = RGN_FixelFormt;
    stDst.u32Width = pstCanvasInfo->stSize.u32Width;
    stDst.u32Height = pstCanvasInfo->stSize.u32Height;
    stDst.bAlphaMax255 = 1;
    stDst.bAlphaExt1555 = 0;
    stDst.u8Alpha0 = 0;
    stDst.u8Alpha1 = 255;
    stDstRect.s32Xpos = 0;
    stDstRect.s32Ypos = 0;
    stDstRect.u32Width = pstCanvasInfo->stSize.u32Width;
    stDstRect.u32Height = pstCanvasInfo->stSize.u32Height;

    stDst.u32Stride = pstCanvasInfo->u32Stride;
    stDst.u32PhyAddr = pstCanvasInfo->u32PhyAddr;
#if 0
    printf("src(w*h: %d x %d)  dst(w*h: %d x %d),src stride:%d, dst stride:%d\n",\
        stSrc.u32Width, stSrc.u32Height, stDst.u32Width, stDst.u32Height,stSrc.u32Stride,stDst.u32Stride);
#endif
    /* resize_param end */
    ret = FY_TDE2_QuickResize(s32Handle, &stSrc, &stSrcRect, &stDst, &stDstRect);
    if (ret)
    {
        FY_TDE2_CancelJob(s32Handle);
        printf("FY_TDE2_QuickResize failed! ret:0x%x(%d)\n",ret,ret);
        goto exit;
    }

    ret = FY_TDE2_EndJob(s32Handle, 1, 1, 500);
    if (ret)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x(%d)\n",ret,ret);
        goto exit;
    }

exit:
    FY_TDE2_Close();
    return ret;
}


/******************************************************************************
* funciton : load bmp from file
******************************************************************************/
FY_S32 SAMPLE_RGN_LoadBmp(const char *filename, BITMAP_S *pstBitmap, FY_BOOL bFil, FY_U32 u16FilColor,OSD_COLOR_FMT_E enColorFmt,PIXEL_FORMAT_E enPixelFormat)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;
    FY_U32 u32BytePerPix = 0;

    if(GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        printf("GetBmpInfo err!\n");
        return FY_FAILURE;
    }
	//SAMPLE_PRT("the filename is %s, the biwidth is %d, biheight is %d\n",filename,bmpInfo.bmiHeader.biWidth,bmpInfo.bmiHeader.biHeight);
	Surface.enColorFmt = enColorFmt;
	if((Surface.enColorFmt == OSD_COLOR_FMT_RGB8888)||(Surface.enColorFmt == OSD_COLOR_FMT_RGB888))
	    u32BytePerPix      = 4;
	else
		u32BytePerPix      = 2;

    pstBitmap->pData = malloc(u32BytePerPix * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));

    if(NULL == pstBitmap->pData)
    {
        printf("malloc osd memroy err!\n");
        return FY_FAILURE;
    }

    CreateSurfaceByBitMap(filename, &Surface, (FY_U8*)(pstBitmap->pData));

    pstBitmap->u32Width      = Surface.u16Width;
    pstBitmap->u32Height     = Surface.u16Height;
    pstBitmap->enPixelFormat = enPixelFormat;


    int i,j;
    FY_U16 *pu16Temp;
    pu16Temp = (FY_U16*)pstBitmap->pData;

    if (bFil)
    {
        for (i=0; i<pstBitmap->u32Height; i++)
        {
            for (j=0; j<pstBitmap->u32Width; j++)
            {
                if (u16FilColor == *pu16Temp)
                {
                    *pu16Temp &= 0x7FFF;
                }
                pu16Temp++;
            }
        }
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_VPSS_SET_OVERLAY_NUM(FY_U32 u32Num)
{
    if((u32Num>0) && (u32Num <= 8))
        u32MaxOverlay = u32Num;
    else
        SAMPLE_PRT("the overlay number:%d is invaild\n",u32Num);
    if(1 == u32MaxOverlay)
        g_heigt_div = 4;
    else
        g_heigt_div = 2;
    return 0;
}

FY_S32 SAMPLE_VPSS_OVERLAY_HANDLE(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    RGN_HANDLE hRgn_base=-1;
    if(0 == gu32ChipID){
    	if((VPSS_CHN2 == VpssChn)||(VPSS_CHN4 == VpssChn))
            return -1;

    	if(VPSS_CHN3 == VpssChn)
    		hRgn_base = GOSD_BEGIN+(VpssGrp*3*u32MaxOverlay+(VpssChn-1)*u32MaxOverlay); //One group max overlay number: 3*8,one chn max:8
    	else
    		hRgn_base = GOSD_BEGIN+(VpssGrp*3*u32MaxOverlay+VpssChn*u32MaxOverlay); //One group max overlay number: 3*8,one chn max:8
    }
#if(defined(MC6650)||defined(MC6850))
    else{
    	if((VPSS_CHN5 == VpssChn))
            return -1;
		hRgn_base = GOSD_BEGIN+(VpssGrp*5*u32MaxOverlay+VpssChn*u32MaxOverlay); //One group max overlay number: 4*8,one chn max:8
    }
#endif
    return hRgn_base;
}
FY_S32 SAMPEL_VPSS_OVERLAY_Detach(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
#ifdef SHOW_GOSD
	FY_U32 i =0;
	RGN_HANDLE hRgn_base,hRgn;
//	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;

    hRgn_base = SAMPLE_VPSS_OVERLAY_HANDLE(VpssGrp,VpssChn);
    if(-1 == hRgn_base)
        return -1;

	for(i=0;i<u32MaxOverlay;i++)
	{
		hRgn = hRgn_base + i;
	    stMppChn.enModId = FY_ID_VPSS;
		stMppChn.s32DevId = VpssGrp;
		stMppChn.s32ChnId = VpssChn;

        CHECK_RET(FY_MPI_RGN_DetachFromChn(hRgn, &stMppChn), "FY_MPI_RGN_AttachToChn");

	}
#endif
    return s32Ret;
}

FY_S32 SAMPEL_VPSS_OVERLAY_Attach_RGB1555(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
#ifdef SHOW_GOSD
	FY_U32 i =0;
	RGN_HANDLE hRgn_base,hRgn;
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;
	RGN_CHN_ATTR_S stRgnChnAttr;
    VPSS_CHN_MODE_S stVpssMode; //for create rand range.
    FY_U32 u32Sx = 0,u32Width=0,rgnHeight = 0;
    FY_BOOL bInvalid = FY_FALSE;
	RGN_CANVAS_INFO_S stCanvasInfo;

    hRgn_base = SAMPLE_VPSS_OVERLAY_HANDLE(VpssGrp,VpssChn);
    if(-1 == hRgn_base)
        return -1;

	memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));

	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
	if (FY_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get Vpss chn mode failed!\n");
	}

	if(stVpssMode.u32Width < 64){
		//SAMPLE_PRT("Chn output width is too samll, can not set pic!\n");
		return FY_FAILURE;
	}

	for(i=0;i<u32MaxOverlay;i++)
	{

		hRgn = hRgn_base + i;
        memset(&stRgnChnAttr,0,sizeof(RGN_CHN_ATTR_S));
        memset(&stRgnAttr,0,sizeof(RGN_ATTR_S));

	    stMppChn.enModId = FY_ID_VPSS;
		stMppChn.s32DevId = VpssGrp;
		stMppChn.s32ChnId = VpssChn;

        bInvalid = FY_FALSE;
	    s32Ret = FY_MPI_RGN_GetAttr(hRgn, &stRgnAttr);
        s32Ret |= FY_MPI_RGN_GetDisplayAttr(hRgn,&stMppChn,&stRgnChnAttr);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("get region info error !\n");
            continue;
        }

        u32Width = stRgnAttr.unAttr.stOverlay.stSize.u32Width;

        u32Sx = DEFAULT_S32X;
        //u32Sy = DEFAULT_S32Y + i*(DEFAULT_RGN_HEIGHT);

        if(!stRgnChnAttr.bShow || (stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X + stRgnAttr.unAttr.stOverlay.stSize.u32Width > stVpssMode.u32Width) \
                    || (stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y + stRgnAttr.unAttr.stOverlay.stSize.u32Height > stVpssMode.u32Height))
            bInvalid = FY_TRUE;
        if((u32Sx+u32Width) != stVpssMode.u32Width  || bInvalid) {
            rgnHeight = stVpssMode.u32Height / (u32MaxOverlay + 1);
            CHECK_RET(FY_MPI_RGN_DetachFromChn(hRgn,&stMppChn), "FY_MPI_RGN_DetachFromChn");
            stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = (FY_U32)(u32Sx*stVpssMode.u32Width/DEFAULT_WIDTH);
		    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = i*(rgnHeight);//(FY_U32)(u32Sy*stVpssMode.u32Height/DEFAULT_HEIGHT);

            stRgnAttr.unAttr.stOverlay.stSize.u32Width = (stVpssMode.u32Width-stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X)/2;
            stRgnAttr.unAttr.stOverlay.stSize.u32Height = rgnHeight/g_heigt_div;//(FY_U32)(DEFAULT_RGN_HEIGHT*stVpssMode.u32Height/DEFAULT_HEIGHT);

            s32Ret = FY_MPI_RGN_SetAttr(hRgn, &stRgnAttr);
            if (FY_SUCCESS != s32Ret) {
                SAMPLE_PRT("FY_MPI_RGN_SetAttr error !\n");
                continue;
            }

        }

		stRgnChnAttr.bShow	= FY_TRUE;
	    stRgnChnAttr.enType = OVERLAY_RGN;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0xff; //0x00 ~ 0xff
		stRgnChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0xFF;
    	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);

    	if (FY_SUCCESS != s32Ret)
    	{
    		SAMPLE_PRT("get Vpss chn mode failed!\n");
    	}

        CHECK_RET(FY_MPI_RGN_AttachToChn(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_AttachToChn");

        memset(&stCanvasInfo,0,sizeof(RGN_CANVAS_INFO_S));
        CHECK_RET(FY_MPI_RGN_GetCanvasInfo(hRgn,&stCanvasInfo), "FY_MPI_RGN_GetCanvasInfo");

        test_tde_quickResize(&stCanvasInfo);
        //test_16to2bit(&stCanvasInfo);
        FY_MPI_RGN_UpdateCanvas(hRgn);
        //Region_logov2_osd(VpssGrp,VpssChn,hRgn,pixeldepth,i,invert);
	}

#endif
    return s32Ret;
}


FY_S32 SAMPEL_VPSS_OVERLAY_Attach(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 pixeldepth,FY_U32 invert)
{
    FY_S32 s32Ret = FY_SUCCESS;
#ifdef SHOW_GOSD
	FY_U32 i =0;
	RGN_HANDLE hRgn_base,hRgn;
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;
	RGN_CHN_ATTR_S stRgnChnAttr;
    VPSS_CHN_MODE_S stVpssMode; //for create rand range.
    FY_U32 u32Sx = 0,u32Width=0,rgnHeight = 0;
    FY_BOOL bInvalid = FY_FALSE;

    hRgn_base = SAMPLE_VPSS_OVERLAY_HANDLE(VpssGrp,VpssChn);
    if(-1 == hRgn_base)
        return -1;

	memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));

	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
	if (FY_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get Vpss chn mode failed!\n");
	}

	if(stVpssMode.u32Width < 64){
		//SAMPLE_PRT("Chn output width is too samll, can not set pic!\n");
		return FY_FAILURE;
	}

	for(i=0;i<u32MaxOverlay;i++)
	{

		hRgn = hRgn_base + i;
        memset(&stRgnChnAttr,0,sizeof(RGN_CHN_ATTR_S));
        memset(&stRgnAttr,0,sizeof(RGN_ATTR_S));

	    stMppChn.enModId = FY_ID_VPSS;
		stMppChn.s32DevId = VpssGrp;
		stMppChn.s32ChnId = VpssChn;

        bInvalid = FY_FALSE;
	    s32Ret = FY_MPI_RGN_GetAttr(hRgn, &stRgnAttr);
        s32Ret |= FY_MPI_RGN_GetDisplayAttr(hRgn,&stMppChn,&stRgnChnAttr);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("get region info error !\n");
            continue;
        }

        u32Width = stRgnAttr.unAttr.stOverlay.stSize.u32Width;

        u32Sx = DEFAULT_S32X;
        //u32Sy = DEFAULT_S32Y + i*(DEFAULT_RGN_HEIGHT);

        if(!stRgnChnAttr.bShow || (stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X + stRgnAttr.unAttr.stOverlay.stSize.u32Width > stVpssMode.u32Width) \
                    || (stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y + stRgnAttr.unAttr.stOverlay.stSize.u32Height > stVpssMode.u32Height))
            bInvalid = FY_TRUE;
        if((u32Sx+u32Width) != stVpssMode.u32Width  || bInvalid) {
            //SAMPLE_PRT("Hanle:%d detach from chn :%d\n",hRgn,VpssChn);
            //CHECK_RET(FY_MPI_RGN_Destroy(hRgn), "FY_MPI_RGN_Destroy");

            rgnHeight = stVpssMode.u32Height / 9;
            CHECK_RET(FY_MPI_RGN_DetachFromChn(hRgn,&stMppChn), "FY_MPI_RGN_DetachFromChn");
            stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = (FY_U32)(u32Sx*stVpssMode.u32Width/DEFAULT_WIDTH);
		    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = i*(rgnHeight);//(FY_U32)(u32Sy*stVpssMode.u32Height/DEFAULT_HEIGHT);

            stRgnAttr.unAttr.stOverlay.stSize.u32Width = stVpssMode.u32Width-stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X;
            stRgnAttr.unAttr.stOverlay.stSize.u32Height = rgnHeight;//(FY_U32)(DEFAULT_RGN_HEIGHT*stVpssMode.u32Height/DEFAULT_HEIGHT);
/*
            SAMPLE_PRT("hRgn:%d--x,y,w,h is %d,%d,%d,%d,chn mode w/h is %d,%d\n",hRgn,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y,stRgnAttr.unAttr.stOverlay.stSize.u32Width ,\
            stRgnAttr.unAttr.stOverlay.stSize.u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
*/

            s32Ret = FY_MPI_RGN_SetAttr(hRgn, &stRgnAttr);
            if (FY_SUCCESS != s32Ret) {
                SAMPLE_PRT("FY_MPI_RGN_SetAttr error !\n");
                continue;
            }

        }

		stRgnChnAttr.bShow	= FY_TRUE;
	    stRgnChnAttr.enType = OVERLAY_RGN;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0xff; //0x00 ~ 0xff
		stRgnChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0xFF;
#if 0
        if(pixeldepth == 1)
            stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1BPP;
        else if(pixeldepth == 2)
            stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_2BPP;
        else if(pixeldepth == 4)
            stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_4BPP;
        else if(pixeldepth == 8)
            stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_8BPP;

        stRgnAttr.enType = OVERLAY_RGN;
       if(bInvalid || ((u32Sx+u32Width) != stVpssMode.u32Width)){
            /*
            SAMPLE_PRT("hanle:%d--x,y,w,h is %d,%d,%d,%d,chn mode w/h is %d,%d\n",hRgn,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y,stRgnAttr.unAttr.stOverlay.stSize.u32Width ,\
            stRgnAttr.unAttr.stOverlay.stSize.u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
            */
            //CHECK_RET(FY_MPI_RGN_Create(hRgn, &stRgnAttr), "FY_MPI_RGN_Create");
        }
#endif
    	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
    	if (FY_SUCCESS != s32Ret)
    	{
    		SAMPLE_PRT("get Vpss chn mode failed!\n");
    	}

        if((stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X + stRgnAttr.unAttr.stOverlay.stSize.u32Width > stVpssMode.u32Width) \
            || (stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y + stRgnAttr.unAttr.stOverlay.stSize.u32Height > stVpssMode.u32Height)){
            /*
            SAMPLE_PRT("return hRgn:%d :x,y,w,h is %d,%d,%d,%d,chn mode w/h is %d,%d\n",hRgn,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y,stRgnAttr.unAttr.stOverlay.stSize.u32Width ,\
            stRgnAttr.unAttr.stOverlay.stSize.u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
            */
            //return 0;
            /*
            bInvalid = FY_TRUE;

            rgnHeight = stVpssMode.u32Width / (OVERLAY_MAX_NUM_VPSS + 4);
            //CHECK_RET(FY_MPI_RGN_DetachFromChn(hRgn,&stMppChn), "FY_MPI_RGN_Create");
            stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = (FY_U32)(u32Sx*stVpssMode.u32Width/DEFAULT_WIDTH);
		    stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = i*(rgnHeight);//(FY_U32)(u32Sy*stVpssMode.u32Height/DEFAULT_HEIGHT);
            */
        }
        /*
        SAMPLE_PRT("attach hRgn:%d :x,y,w,h is %d,%d,%d,%d,chn mode w/h is %d,%d\n",hRgn,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X,stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y,stRgnAttr.unAttr.stOverlay.stSize.u32Width ,\
            stRgnAttr.unAttr.stOverlay.stSize.u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
        */
        CHECK_RET(FY_MPI_RGN_AttachToChn(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_AttachToChn");

        if((g_invert[VpssGrp] != invert)||((u32Sx+u32Width) != stVpssMode.u32Width) || bInvalid){
		    Region_logov2_osd(VpssGrp,VpssChn,hRgn,pixeldepth,i,invert);
        }
        else if(0==i){
            Region_logov2_osd(VpssGrp,VpssChn,hRgn,pixeldepth,i,invert);
        }

        //Region_logov2_osd(VpssGrp,VpssChn,hRgn,pixeldepth,i,invert);

	}
    g_invert[VpssGrp] = invert;
#endif
    return s32Ret;
}

FY_S32 SAMPLE_VPSS_Overlay_Destory(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
	FY_S32 s32Ret = FY_SUCCESS;
#ifdef SHOW_GOSD
	FY_U32 i =0;
	RGN_HANDLE hRgn_base,hRgn;

    hRgn_base = SAMPLE_VPSS_OVERLAY_HANDLE(VpssGrp,VpssChn);
    if(-1 == hRgn_base)
        return -1;


	for(i=0;i<u32MaxOverlay;i++)
	{
		hRgn = hRgn_base +i;
        //printf("create rgn handle:%d,VpssGrp:%d,VpssChn:%d\n",hRgn,VpssGrp,VpssChn);
        CHECK_RET(FY_MPI_RGN_Destroy(hRgn), "FY_MPI_RGN_Destroy");
	}
#endif
	return s32Ret;
}

FY_S32 SAMPLE_VPSS_Overlay_Create(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 pixeldepth,FY_U32 chipId)
{
	FY_S32 s32Ret = FY_SUCCESS;
#ifdef SHOW_GOSD
	FY_U32 i =0;
	RGN_HANDLE hRgn_base,hRgn;
	RGN_ATTR_S stRgnAttr;
//    FY_U32 u32Width = 1920, u32Height = 1080;
    gu32ChipID = chipId;
    hRgn_base = SAMPLE_VPSS_OVERLAY_HANDLE(VpssGrp,VpssChn);
    if(-1 == hRgn_base)
        return -1;
	if(pixeldepth == 1)
		stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1BPP;
	else if(pixeldepth == 2)
		stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_2BPP;
	else if(pixeldepth == 4)
		stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_4BPP;
	else if(pixeldepth == 8)
		stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_8BPP;
    else if(pixeldepth == 16)
        stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;

    stRgnAttr.enType = OVERLAY_RGN;

    //printf("%s,%d:Grp:%d,Chn:%d,create region hand_begin:%d\n",__FUNCTION__,__LINE__,VpssGrp,VpssChn,hRgn_base);
	for(i=0;i<u32MaxOverlay;i++)
	{
		hRgn =hRgn_base + i;

        stRgnAttr.unAttr.stOverlay.stSize.u32Width = DEFAULT_WIDTH;
        stRgnAttr.unAttr.stOverlay.stSize.u32Height = DEFAULT_RGN_HEIGHT;

        //CHECK_RET(FY_MPI_RGN_Create(hRgn, &stRgnAttr), "FY_MPI_RGN_Create");
        s32Ret = FY_MPI_RGN_Create(hRgn, &stRgnAttr);
        if(FY_SUCCESS != s32Ret){
            printf("Grp:%d,Chn:%d,create region error hand:%d\n",VpssGrp,VpssChn,hRgn);
        }
	}
#endif
	return s32Ret;
}


/*m<=r<=n ,rand()%(n-m+1)+m*/
FY_S32 SAMPLE_VPSS_OVERLAY_FUNC(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 brandom)
{
    FY_S32 s32Ret = FY_SUCCESS;
	FY_U32 i =0;
	BITMAP_S stBitmap;

	RGN_HANDLE hRgn;
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;
	RGN_CHN_ATTR_S stRgnChnAttr;

	RGN_CANVAS_INFO_S stCanvasInfo;

	FY_U32 min_w,min_h,w,h;
	FY_U32 stride,stride_src,bpp;

	FY_U32 u32Sx = 0,u32Sy = 0,u32Width=0,u32Height=0;
	FY_U32 u32SxRand = 0, u32SyRand = 0,u32WidthRand = 0, u32HeightRand = 0;
	VPSS_CHN_MODE_S stVpssMode; //for create rand range.


	memset(&stBitmap,0,sizeof(BITMAP_S));

	if((VPSS_CHN2 == VpssChn)||(VPSS_CHN4 == VpssChn)){
		SAMPLE_PRT("CHN2 & CHN4 not support PM & GOSD\n");
		return FY_FAILURE;
	}
	memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));

	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
	if (FY_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get Vpss chn mode failed!\n");
	}

	if(stVpssMode.u32Width < 64){
		SAMPLE_PRT("Chn output width is too samll, can not set pic!\n");
		return FY_FAILURE;
	}

	if(brandom){

		srand(time(NULL));

		u32SxRand = stVpssMode.u32Width - 64;
		u32SyRand = stVpssMode.u32Height - 64;
	}
	else{
		u32Width = ALIGN_BACK(stVpssMode.u32Width/5,4);
		u32Height = u32Width;
	}
	stRgnAttr.enType = OVERLAY_RGN;
	stRgnAttr.unAttr.stOverlay.enPixelFmt = RGN_FixelFormt;
	stRgnAttr.unAttr.stOverlay.u32BgColor = 0xFF6A5ACD;

	if(stRgnAttr.unAttr.stOverlay.enPixelFmt == PIXEL_FORMAT_RGB_8888)
		stRgnAttr.unAttr.stOverlay.u32BgColor	   = stRgnAttr.unAttr.stOverlay.u32BgColor;
	else if(stRgnAttr.unAttr.stOverlay.enPixelFmt == PIXEL_FORMAT_RGB_1555)
		stRgnAttr.unAttr.stOverlay.u32BgColor = rgb8888torgb1555(stRgnAttr.unAttr.stOverlay.u32BgColor);
	else if(stRgnAttr.unAttr.stOverlay.enPixelFmt == PIXEL_FORMAT_RGB_4444)
		stRgnAttr.unAttr.stOverlay.u32BgColor = rgb8888torgb4444(stRgnAttr.unAttr.stOverlay.u32BgColor);
	else if(stRgnAttr.unAttr.stOverlay.enPixelFmt == PIXEL_FORMAT_RGB_565)
		stRgnAttr.unAttr.stOverlay.u32BgColor = rgb8888torgb0565(stRgnAttr.unAttr.stOverlay.u32BgColor);

	if(VPSS_CHN3 == VpssChn)
		hRgn = GOSD_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+(VpssChn-1)*8); //One group max overlay number: 3*8,one chn max:8
	else
		hRgn = GOSD_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+VpssChn*8); //One group max overlay number: 3*8,one chn max:8

	for(i=0;i<SHOW_GOSD_NUM;i++)
	{
		hRgn = hRgn + i;

		if(brandom){
			/*m<=r<=n ,rand()%(n-m+1)+m : m = 0, n =u32SxRand ,u32SyRand*/
			u32Sx = ALIGN_BACK(rand()%(u32SxRand),4);
			u32Sy = ALIGN_BACK(rand()%(u32SyRand),4);

			u32WidthRand = stVpssMode.u32Width - u32Sx - 20;
			u32HeightRand = stVpssMode.u32Height - u32Sy - 20;

			u32Width = ALIGN_UP(rand()%(u32WidthRand)+1,16);
			u32Height = ALIGN_UP(rand()%(u32HeightRand)+1,16);

			SAMPLE_PRT("hRgn %d:rand x/y/w/h is [%d,%d,%d,%d], chn w/h is [%d,%d]\n",hRgn,u32Sx,u32Sy,u32Width,u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
		}
		else{

			u32Sx = 10 + i*(u32Width+5);
			u32Sy = 30;
		}
		stRgnAttr.unAttr.stOverlay.stSize.u32Width = u32Width;
		stRgnAttr.unAttr.stOverlay.stSize.u32Height = u32Height;
		CHECK_RET(FY_MPI_RGN_Create(hRgn, &stRgnAttr), "FY_MPI_RGN_Create");


	    stMppChn.enModId = FY_ID_VPSS;
		stMppChn.s32DevId = VpssGrp;
		stMppChn.s32ChnId = VpssChn;

		memset(&stRgnChnAttr,0,sizeof(RGN_CHN_ATTR_S));
		stRgnChnAttr.bShow	= FY_TRUE;
	    stRgnChnAttr.enType = stRgnAttr.enType;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
		stRgnChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0xFF; //0x00 ~ 0xff
		stRgnChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0xFF;



		s32Ret = SAMPLE_RGN_LoadBmp(pszImageNames[i], &stBitmap, FY_FALSE, 0,OSD_SURFACE_FixelFormat,RGN_FixelFormt);
		if(FY_SUCCESS != s32Ret){
			SAMPLE_PRT("SAMPLE_RGN_LoadBmp error \n");
			return s32Ret;
		}

		stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X =u32Sx;
		stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = u32Sy;

		CHECK_RET(FY_MPI_RGN_AttachToChn(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_AttachToChn");

		if(i < 2)
			CHECK_RET(FY_MPI_RGN_SetBitMap(hRgn, &stBitmap), "FY_MPI_RGN_AttachToChn");

		if(i>=2)
		{

			memset(&stCanvasInfo,0,sizeof(RGN_CANVAS_INFO_S));
			CHECK_RET(FY_MPI_RGN_GetCanvasInfo(hRgn,&stCanvasInfo), "FY_MPI_RGN_GetCanvasInfo");
			min_w = MIN(stCanvasInfo.stSize.u32Width,stBitmap.u32Width);
			min_h = MIN(stCanvasInfo.stSize.u32Height,stBitmap.u32Height);
			stride = stCanvasInfo.u32Stride;
			if(stCanvasInfo.enPixelFmt == PIXEL_FORMAT_RGB_8888)
				bpp = 4;
			else
				bpp = 2;
			stride_src = bpp*stBitmap.u32Width;


			for(h=0;h<min_h;h++)
			{
				for(w=0;w<min_w;w++)
				{
					memcpy(((FY_VOID *)stCanvasInfo.u32VirtAddr + h*stride + w*bpp),(stBitmap.pData + h*stride_src + w*bpp),bpp);
				}
			}
			CHECK_RET(FY_MPI_RGN_UpdateCanvas(hRgn), "FY_MPI_RGN_UpdateCanvas");
		}

		if(NULL != stBitmap.pData){
			free(stBitmap.pData);
			stBitmap.pData = NULL;
		}
		#if 0
		/*change display x/y*/
		CHECK_RET(FY_MPI_RGN_GetDisplayAttr(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_GetDisplayAttr");

		stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 100+i*100;
		stRgnChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 10+80*i;

		CHECK_RET(FY_MPI_RGN_SetDisplayAttr(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_SetDisplayAttr");
		#endif
	}
    return s32Ret;
}

FY_S32 SAMPLE_VPSS_COVER_MOSAIC_FUNC(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 brandom)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i =0;
	RGN_HANDLE hRgn;
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;
	RGN_CHN_ATTR_S stRgnChnAttr;

	FY_U32 u32Sx = 0,u32Sy = 0,u32Width=0,u32Height=0;
	FY_U32 u32SxRand = 0, u32SyRand = 0,u32WidthRand = 0, u32HeightRand = 0;
	VPSS_CHN_MODE_S stVpssMode; //for create rand range.

	if((VPSS_CHN2 == VpssChn)||(VPSS_CHN4 == VpssChn)){
		SAMPLE_PRT("CHN2 & CHN4 not support PM & GOSD\n");
		return FY_FAILURE;
	}

	memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));

	s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
	if (FY_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get Vpss chn mode failed!\n");
	}

	if(stVpssMode.u32Width < 64){
		SAMPLE_PRT("Chn output width is too samll, can not set pic!\n");
		return FY_FAILURE;
	}

	if(brandom){
		u32SxRand = stVpssMode.u32Width - 64;
		u32SyRand = stVpssMode.u32Height - 64;
	}
	else{
		u32Width = ALIGN_BACK(stVpssMode.u32Width/5,4);
		u32Height = u32Width;
	}

	if(VPSS_CHN3 == VpssChn)
		hRgn = MOSAIC_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+(VpssChn-1)*8); //One group max overlay number: 3*8,one chn max:8
	else
		hRgn = MOSAIC_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+VpssChn*8); //One group max overlay number: 3*8,one chn max:8

	srand(time(NULL));
	if(rand()%2)
		stRgnAttr.enType = COVER_RGN;
	else
		stRgnAttr.enType = MOSAIC_RGN;

	for(i=0;i<SHOW_MOSAIC_NUM;i++) {
	    hRgn = hRgn + i;

		if(brandom){
			u32Sx = ALIGN_BACK(rand()%(u32SxRand),4);
			u32Sy = ALIGN_BACK(rand()%(u32SyRand),4);

			u32WidthRand = stVpssMode.u32Width - u32Sx - 20;
			u32HeightRand = stVpssMode.u32Height - u32Sy - 20;

			u32Width = ALIGN_UP(rand()%(u32WidthRand) + 1,16);
			u32Height = ALIGN_UP(rand()%(u32HeightRand) + 1,16);
			SAMPLE_PRT("hRgn %d:rand x/y/w/h is [%d,%d,%d,%d], chn w/h is [%d,%d]\n",hRgn,u32Sx,u32Sy,u32Width,u32Height,stVpssMode.u32Width,stVpssMode.u32Height);
		}
		else{

			u32Sx = 10 + i*(u32Width+5);
			u32Sy = stVpssMode.u32Height - u32Height - 20;
		}

	    CHECK_RET(FY_MPI_RGN_Create(hRgn, &stRgnAttr), "FY_MPI_RGN_Create");

	    stMppChn.enModId = FY_ID_VPSS;
	    stMppChn.s32DevId = VpssGrp;


	    stRgnChnAttr.bShow	= FY_TRUE;
	    stRgnChnAttr.enType = stRgnAttr.enType;

		if(stRgnAttr.enType == COVER_RGN) {
		    stRgnChnAttr.unChnAttr.stCoverChn.u32Layer = 0;
		    stRgnChnAttr.unChnAttr.stCoverChn.stRect.s32X = u32Sx;
		    stRgnChnAttr.unChnAttr.stCoverChn.stRect.s32Y = u32Sy;
		    stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Width = u32Width;
		    stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Height = u32Height;

			if(0==(rand()%4))
			    stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0x00CD00; //Green
			else if(1==(rand()%4))
				stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0x6A5ACD; //purple
			else if(2==(rand()%4))
				stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0xCD96CD; //
			else if(3==(rand()%4))
				stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0x436EEE;

		    stRgnChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;
		}
		else{
			stRgnChnAttr.unChnAttr.stMosaicChn.stRect.s32X = u32Sx;
			stRgnChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = u32Sy;
			stRgnChnAttr.unChnAttr.stMosaicChn.stRect.u32Width = u32Width;
			stRgnChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = u32Height;
			stRgnChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_16;
		}

        stMppChn.s32ChnId = VpssChn;
        CHECK_RET(FY_MPI_RGN_AttachToChn(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_AttachToChn");
	}
/*
	stMppChn.s32ChnId = VpssChn;
    stRgnChnAttr.unChnAttr.stCoverChn.u32Layer = 0;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.s32X = 700;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 500;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 400;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 300;
	CHECK_RET(FY_MPI_RGN_SetDisplayAttr(hRgn, &stMppChn, &stRgnChnAttr), "FY_MPI_RGN_SetDisplayAttr");
*/
	return s32Ret;
}

FY_S32 SAMPLE_VPSS_RGN_Destory(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{

	FY_S32 s32Ret = FY_SUCCESS;
	FY_U32 i =0;
	RGN_HANDLE hRgn;

	//SAMPLE_PRT("destory rgn, vpssGrp:%d, chn:%d\n",VpssGrp,VpssChn);

	if(VPSS_CHN3 == VpssChn)
		hRgn = GOSD_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+(VpssChn-1)*8); //One group max overlay number: 3*8,one chn max:8
	else
		hRgn = GOSD_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+VpssChn*8); //One group max overlay number: 3*8,one chn max:8


	for(i=0;i<SHOW_GOSD_NUM;i++)
	{
		hRgn += i;
		//SAMPLE_PRT("destory SHOW_GOSD_NUM rgn, vpssGrp:%d, chn:%d,hRgn is %d\n",VpssGrp,VpssChn,hRgn);
		s32Ret = FY_MPI_RGN_Destroy(hRgn);
	}


	if(VPSS_CHN3 == VpssChn)
		hRgn = MOSAIC_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+(VpssChn-1)*8); //One group max overlay number: 3*8,one chn max:8
	else
		hRgn = MOSAIC_BEGIN+(VpssGrp*3*OVERLAY_MAX_NUM_VPSS+VpssChn*8); //One group max overlay number: 3*8,one chn max:8


	for(i=SHOW_MOSAIC_NUM;i<1;i++)
	{
		hRgn += i;
		//SAMPLE_PRT("destory SHOW_MOSAIC_NUM rgn, vpssGrp:%d, chn:%d,hRgn is %d\n",VpssGrp,VpssChn,hRgn);
		s32Ret = FY_MPI_RGN_Destroy(hRgn);
	}
	return s32Ret;
}


#endif
/******************************************************************************
* function : Set vpss system memory location
******************************************************************************/
FY_S32 SAMPLE_COMM_VPSS_MemConfig()
{
    FY_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVpss;
    FY_S32 s32Ret, i;

    /*vpss group max is VPSS_MAX_GRP_NUM, not need config vpss chn.*/
    for(i=0;i<VPSS_MAX_GRP_NUM;i++)
    {
        stMppChnVpss.enModId  = FY_ID_VPSS;
        stMppChnVpss.s32DevId = i;
        stMppChnVpss.s32ChnId = 0;

        if(0 == (i%2))
        {
            pcMmzName = NULL;
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vpss*/
        s32Ret = FY_MPI_SYS_SetMemConf(&stMppChnVpss, pcMmzName);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Vpss FY_MPI_SYS_SetMemConf ERR !\n");
            return FY_FAILURE;
        }
    }
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VPSS_START_GLOBLE(SAMPLE_VPSS_GLOBLE_DISPLAY_INIT_CFG *VpssGlobDis)
{
    FY_U32 i;
    FY_S32 s32Ret = FY_SUCCESS;
	FY_U32 GlobCnt = 0;
	VPSS_GLOBLE_DISPLAY_INIT_CFG initConf;
	if(VpssGlobDis == NULL)
		return FY_FAILURE;

	GlobCnt = VpssGlobDis->u32GlobNum;

	for(i=0;i<GlobCnt;i++)
	{
		//if(VpssGlobDis->initcfg[i].benable)
		{
			memset(&initConf,0,sizeof(VPSS_GLOBLE_DISPLAY_INIT_CFG));
			memcpy(&initConf,&(VpssGlobDis->initcfg[i]),sizeof(VPSS_GLOBLE_DISPLAY_INIT_CFG));
			s32Ret = FY_MPI_VPSS_SetGlobleDispInitCfg(&initConf);
		}
	}

	return s32Ret;
}


/*****************************************************************************
* function : start vpss. VPSS chn with frame
*****************************************************************************/
FY_S32 SAMPLE_COMM_VPSS_Start(FY_S32 s32GrpCnt, SIZE_S *pstSize, FY_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr,stVpssInfo *pstVpssInfo)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    VPSS_CHN_ATTR_S stChnAttr = {0};
#ifdef TEST_21A
    VPSS_GRP_PARAM_S stVpssParam = {0};
#endif
    FY_S32 s32Ret;
    FY_S32 i, j;
    VPSS_CHN_MODE_S stVpssMode = {0};
//	VPSS_GLOBLE_DISPLAY_INIT_CFG initConf;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

        stGrpAttr.bIeEn = FY_FALSE;
        stGrpAttr.bNrEn = FY_TRUE;
        stGrpAttr.bDciEn = FY_FALSE;
        stGrpAttr.bHistEn = FY_FALSE;
        stGrpAttr.bEsEn = FY_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }

    if(NULL != pstVpssInfo)
    {
       s32ChnCnt = VPSS_MAX_CHN_NUM;
    }


	/*init globle config */
	if(NULL != pstVpssInfo){
		SAMPLE_COMM_VPSS_START_GLOBLE(&(pstVpssInfo->st_VpssGlobleInfo));

		//s32Ret = FY_MPI_VPSS_GetGlobleDispInitCfg(0,&initConf);
		//printf("Get Globle :the glb_idx is %d, pixfmt is %d,w:%d, h:%d\n",initConf.glob_idx,initConf.glb_pixfmt,initConf.tal_size.u32Width,initConf.tal_size.u32Height);
	}
	/*end config globle output*/


    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
		if(NULL != pstVpssInfo)
		{
			stGrpAttr.enPixFmt = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.enPixFmt;
            stGrpAttr.u32MaxW = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.u32Width;
            stGrpAttr.u32MaxH = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.u32Height;
            stGrpAttr.bNrEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bYcnrEn;
            stGrpAttr.bApcEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bApcEn;
            stGrpAttr.bChromaEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bChromaEn;
            stGrpAttr.bLcEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bLcEn;
            stGrpAttr.bHistEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bYHistEn;
            stGrpAttr.bPurpleEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bPurpleEn;
            stGrpAttr.bYGammaEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.bYGammaEn;
            // stGrpAttr.
			//memcpy(&(stGrpAttr.enCompMode),&(pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssGrpInfo.enCompMode),sizeof(stVpssGrpAttr)-4);
		}
        /*** create vpss group ***/
        s32Ret = FY_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("FY_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
        /*** enable vpss chn, with frame ***/
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            /* Set Vpss Chn attr */

            memset(&stChnAttr,0,sizeof(VPSS_CHN_ATTR_S));

            stChnAttr.cropInfo.bEnable =0;  //default is 0
            stChnAttr.cropInfo.stRect.s32X = 0;
            stChnAttr.cropInfo.stRect.s32Y = 0;
            stChnAttr.cropInfo.stRect.u32Width = pstSize->u32Width;
            stChnAttr.cropInfo.stRect.u32Height = pstSize->u32Height;

            if(NULL != pstVpssInfo)
            {
                memcpy(&(stChnAttr.cropInfo),&(pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnAttr[j]),sizeof(stVpssChnAttr));
            }

            s32Ret = FY_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != FY_SUCCESS)
            {
                SAMPLE_PRT("FY_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return FY_FAILURE;
            }

            memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));
      /*
            s32Ret = FY_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssMode);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("get Vpss chn mode failed!\n");
            }
        */
            stVpssMode.u32Width  = pstSize->u32Width;
            stVpssMode.u32Height = pstSize->u32Height;

            stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
            stVpssMode.CompRate = COMPRESS_RATIO_NONE;
            switch(j)
            {
                case VPSS_CHN0:
                    stVpssMode.mainCfg.bYcMeanEn = FY_FALSE;
					stVpssMode.mainCfg.ycmeanMode = DS_ONE_FOURTH;
                    stVpssMode.mainCfg.bBgmYEn = FY_FALSE;
                    stVpssMode.mainCfg.bCpyEn = FY_FALSE;
					stVpssMode.enCompressMode = COMPRESS_MODE_TILE_224;
					stVpssMode.CompRate = COMPRESS_RATIO_59_PER;
                    break;
                case VPSS_CHN1:
					stVpssMode.u32Width  = 352;
	                stVpssMode.u32Height = 288;
					stVpssMode.enCompressMode = COMPRESS_MODE_TILE_224;
					stVpssMode.CompRate = COMPRESS_RATIO_59_PER;
                    break;
                case VPSS_CHN2:
		            stVpssMode.u32Width  = 352;
		            stVpssMode.u32Height = 288;
                    stVpssMode.aiCfg.AiOutSel = 0;
                    stVpssMode.aiCfg.RGBOutMode = 0;
                    break;
                case VPSS_CHN3:
					stVpssMode.u32Width  = 640;
	                stVpssMode.u32Height = 480;
                	stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
                    stVpssMode.CompRate = COMPRESS_RATIO_60_PER;
					stVpssMode.enCompressMode = COMPRESS_MODE_SLICE;
                    break;
                case VPSS_CHN4:
					stVpssMode.u32Width  = 352;
	                stVpssMode.u32Height = 288;
					stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
                    break;
            }

            if(NULL != pstVpssInfo)
            {
				if(0 == pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bConfig)
					continue;
                stVpssMode.enChnMode = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].enChnMode;
                stVpssMode.u32Width  = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].u32Width;
                stVpssMode.u32Height = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].u32Height;
                stVpssMode.enCompressMode = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].enCompressMode;
                stVpssMode.enPixelFormat = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].enPixelFormat;
                stVpssMode.CompRate = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].CompRate;
				//for globle chn
				stVpssMode.GlobCfg.bUseGloble = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bUseGloble;
				stVpssMode.GlobCfg.glob_idx = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].glob_idx;
				stVpssMode.GlobCfg.pic_pos.s32X = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].s32X;
				stVpssMode.GlobCfg.pic_pos.s32Y = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].s32Y;
				stVpssMode.stFrameRate.s32SrcFrmRate = -1;
                stVpssMode.stFrameRate.s32DstFrmRate = -1;
                switch(j)
                {
                    case VPSS_CHN0:
                        stVpssMode.mainCfg.bYcMeanEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bYcMeanEn;
						stVpssMode.mainCfg.ycmeanMode = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].ycmeanMode;
                        stVpssMode.mainCfg.bBgmYEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bBgmYEn;
                        stVpssMode.mainCfg.bCpyEn = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bCpyEn;
                        break;

                    case VPSS_CHN2:
                        stVpssMode.aiCfg.AiOutSel = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].AiOutSel;
                        stVpssMode.aiCfg.RGBOutMode = pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].RGBOutMode;
                        break;
                }
                s32Ret = FY_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssMode);
                if (s32Ret != FY_SUCCESS)
                {
                    SAMPLE_PRT("FY_MPI_VPSS_SetChnMode failed with %#x,VpssGrp is %dVpssChn is %d\n", s32Ret,VpssGrp,VpssChn);
                    return FY_FAILURE;
                }
            }

            s32Ret = FY_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != FY_SUCCESS)
            {
                SAMPLE_PRT("FY_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return FY_FAILURE;
            }
			#ifdef TEST_GOSD
			if((VPSS_CHN2 !=VpssChn)&&(VPSS_CHN4 !=VpssChn)){
                //if(VPSS_CHN3 ==VpssChn)
                //    SAMPLE_VPSS_Overlay_Create(VpssGrp,VpssChn,4);
				//FY_U32 bRandom = (pstVpssInfo->u32ShowMode/2)?1:0;
				//SAMPLE_VPSS_OVERLAY_FUNC(VpssGrp, VpssChn,bRandom);
				//SAMPLE_VPSS_COVER_MOSAIC_FUNC(VpssGrp, VpssChn,bRandom);
				//SAMPEL_VPSS_OVERLAY_FUNC_PIXDEPTH(VpssGrp, VpssChn,1);
			}
			#endif
		#if 0
			//printf("the group is %d, chn is %d, buserGloble is %d\n",VpssGrp, VpssChn,stVpssMode.GlobCfg.bUseGloble);
			if((1 != pstVpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[j].bUseGloble))
			{
				s32Ret = FY_MPI_VPSS_SetDepth(VpssGrp, VpssChn,2);
				if (s32Ret != FY_SUCCESS)
				{
				    SAMPLE_PRT("FY_MPI_VPSS_SetDepth failed with %#x\n", s32Ret);
				    return FY_FAILURE;
				}
			}
		 #endif
        }

        /*** start vpss group ***/
        s32Ret = FY_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("FY_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            return FY_FAILURE;
        }

    }
    return FY_SUCCESS;
}

/*****************************************************************************
* function : disable vi dev
*****************************************************************************/
FY_S32 SAMPLE_COMM_VPSS_Stop(FY_S32 s32GrpCnt, FY_S32 s32ChnCnt)
{
    FY_S32 i, j;
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = FY_MPI_VPSS_StopGrp(VpssGrp);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;

			#ifdef TEST_GOSD
			if((2!=VpssChn)&&(4!=VpssChn))
				SAMPLE_VPSS_RGN_Destory(VpssGrp,VpssChn);
			#endif

            s32Ret = FY_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            if (s32Ret != FY_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return FY_FAILURE;
            }
        }

        s32Ret = FY_MPI_VPSS_DestroyGrp(VpssGrp);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }

    return FY_SUCCESS;
}


/*******************The follow functions are for testing vpu send and save frames.************************/


/******************************************************************************
* function : read frame
******************************************************************************/
FY_VOID SAMPLE_COMM_ReadFrame(FILE * fp, FY_U8 * pY, FY_U8 * pU, FY_U8 * pV, FY_U32 width, FY_U32 height, FY_U32 stride, FY_U32 stride2)
{
    FY_U8 * pDst;
    FY_U32 read_len =0;
    FY_U32 u32Row;

    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        read_len = fread( pDst, width, 1, fp );
        if(read_len == 0)
        {
            fseek(fp, 0, SEEK_SET);
            read_len = fread( pDst, width, 1, fp );
        }
        pDst += stride;
    }

    pDst = pU;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        read_len = fread( pDst, width/2, 1, fp );
        if(read_len == 0)
        {
            fseek(fp, 0, SEEK_SET);
            read_len = fread( pDst, width/2, 1, fp );
        }
        pDst += stride2;
    }

    pDst = pV;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        if(read_len == 0)
        {
            fseek(fp, 0, SEEK_SET);
            fread( pDst, width/2, 1, fp );
        }
        pDst += stride2;
    }
}


/******************************************************************************
* function : Plan to Semi
******************************************************************************/
FY_S32 SAMPLE_COMM_PlanToSemi(FY_U8 *pY, FY_S32 yStride,
                       FY_U8 *pU, FY_S32 uStride,
                       FY_U8 *pV, FY_S32 vStride,
                       FY_S32 picWidth, FY_S32 picHeight)
{
    FY_S32 i;
    FY_U8* pTmpU, *ptu;
    FY_U8* pTmpV, *ptv;
    FY_S32 s32HafW = uStride >>1 ;
    FY_S32 s32HafH = picHeight >>1 ;
    FY_S32 s32Size = s32HafW*s32HafH;

    pTmpU = malloc( s32Size ); ptu = pTmpU;
    pTmpV = malloc( s32Size ); ptv = pTmpV;

    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);
#ifdef TEST_21A
    for(i = 0;i<s32Size>>1;i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;
    }

    for(i = 0;i<s32Size>>1;i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }
#else
    for(i = 0;i<s32Size>>1;i++)
    {
        *pU++ = *pTmpU++;
        *pU++ = *pTmpV++;
    }

    for(i = 0;i<s32Size>>1;i++)
    {
        *pV++ = *pTmpU++;
        *pV++ = *pTmpV++;
    }
#endif
    free( ptu );
    free( ptv );

    return FY_SUCCESS;
}


/******************************************************************************
* function : Get from YUV 420->420sp
******************************************************************************/
FY_S32 SAMPLE_COMM_GetVFrameFromYUV(FILE *pYUVFile, FY_U32 u32Width, FY_U32 u32Height,FY_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo,FY_U32 enPixelFormat)
{
    FY_U32             u32LStride;
    FY_U32             u32CStride;
    FY_U32             u32LumaSize;
    FY_U32             u32ChrmSize;
//    FY_U32             u32Size;
    //VB_BLK VbBlk;
    //FY_U32 u32PhyAddr;
    //FY_U8 *pVirAddr;

    //FY_S32 s32Ret = FY_SUCCESS;
//    VIDEO_FRAME_INFO_S  sendVideoFrame;


    u32LStride  = u32Stride;
    u32CStride  = u32Stride;

    u32LumaSize = (u32LStride * u32Height);
    if(enPixelFormat)
        u32ChrmSize = (u32CStride * u32Height)>>1;/* YUV 422 */
    else
        u32ChrmSize = (u32CStride * u32Height) >> 2;/* YUV 420 */

//    u32Size = u32LumaSize + (u32ChrmSize << 1);
#if 0
    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = FY_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
    if (VB_INVALID_HANDLE == VbBlk)
    {
        printf("FY_MPI_VB_GetBlock err! size:%d\n",u32Size);
        return -1;
    }
    u32PhyAddr = FY_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        return -1;
    }

    pVirAddr = (FY_U8 *) FY_MPI_SYS_Mmap(u32PhyAddr, u32Size);
    if (NULL == pVirAddr)
    {
        return -1;
    }

    pstVFrameInfo->u32PoolId = FY_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
        return -1;
    }


    printf("pool id :%d, phyAddr:%x,virAddr:%x\n" ,pstVFrameInfo->u32PoolId,u32PhyAddr,(int)pVirAddr);
#endif

   // pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

   // pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
    pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;
#ifdef TEST_21A
    if(enPixelFormat)
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    else
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;//PIXEL_FORMAT_YUV_SEMIPLANAR_420;
#else
    if(enPixelFormat)
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    else
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;//PIXEL_FORMAT_YUV_SEMIPLANAR_420;
#endif

    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_FRAME;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */

    /* read Y U V data from file to the addr ----------------------------------------------*/
    SAMPLE_COMM_ReadFrame(pYUVFile, pstVFrameInfo->stVFrame.pVirAddr[0],
       pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.pVirAddr[2],
       pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height,
       pstVFrameInfo->stVFrame.u32Stride[0], pstVFrameInfo->stVFrame.u32Stride[1] >> 1 );

#if 1
    /* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
    SAMPLE_COMM_PlanToSemi(pstVFrameInfo->stVFrame.pVirAddr[0], pstVFrameInfo->stVFrame.u32Stride[0],
      pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.pVirAddr[2], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height);
#endif

   // FY_MPI_SYS_Munmap(pVirAddr, u32Size);
    return 0;
}


/******************************************************************************
* function : Get frame from YUV422sp
******************************************************************************/
FY_S32 SAMPLE_COMM_GetVFrameFromYUVsp(FILE *pYUVFile, FY_U32 u32Width, FY_U32 u32Height,FY_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo,FY_U32 enPixelFormat)
{
    FY_U32             u32SizeY,u32SizeUV;
    FY_U32 readLen = 0;
#ifdef TEST_21A
    FY_U8 *pTmpV=NULL;
#endif
   // FY_U32 timeStart =0,timeEnd=0;
//    FY_U32 timeBegin =0;

    if(3 == enPixelFormat)
    {
        u32SizeY =  u32Width*u32Height+SLICE_HEAD_SIZE;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16);
        u32SizeUV =  u32Width*u32Height;// ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16);
        pstVFrameInfo->stVFrame.enCompressMode = COMPRESS_MODE_SLICE;
    }
	else if(1 == enPixelFormat)
    {
        u32SizeY =  u32Width*u32Height;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16);
        u32SizeUV =  u32Width*u32Height;// ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16);
    }
    else
    {
        u32SizeY =   u32Width*u32Height;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16);
        u32SizeUV =   u32Width*u32Height/2;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16)/2;
    }

   // timeBegin = GetSysTimeByUsec();
    //pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32SizeY;


    //pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32SizeY;


    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;

    pstVFrameInfo->stVFrame.u32Stride[0] = u32Width;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32Width;
    if((1 == enPixelFormat) || (3 == enPixelFormat))
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;//PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    else
        pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_FRAME;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */
    //Read Y from file
   // timeStart = GetSysTimeByUsec();
    readLen = fread((FY_U8 *)pstVFrameInfo->stVFrame.pVirAddr[0],1,u32SizeY, pYUVFile);
   // timeEnd = GetSysTimeByUsec();

    //printf("the fread Y %d bytes use time %d us\n",u32Size>>1,(timeEnd-timeStart));

    if(0 == readLen)
    { //repeat
        fseek(pYUVFile, 0, SEEK_SET);
        readLen = fread((FY_U8 *)pstVFrameInfo->stVFrame.pVirAddr[0],1, u32SizeY, pYUVFile);
    }
    //Read UV from file
   // timeStart = GetSysTimeByUsec();
    fread((FY_U8 *)pstVFrameInfo->stVFrame.pVirAddr[1], 1,u32SizeUV,pYUVFile);
  //  timeEnd = GetSysTimeByUsec();
    //printf("the fread Y-UV %d bytes use time %d us\n",u32Size>>1,(timeEnd-timeStart));
#ifdef TEST_21A
    pTmpV = pstVFrameInfo->stVFrame.pVirAddr[1];

    int i =0;
    FY_U8  temp_value = 0;
  //  timeStart = GetSysTimeByUsec();
    for (i = 0; i < u32SizeUV; i+=2)
    {
        temp_value = pTmpV[i];
        pTmpV[i] = pTmpV[i+1];
        pTmpV[i+1] = temp_value;
    }
    //timeEnd = GetSysTimeByUsec();
    //printf("the UV trans use %dus\n",(timeEnd-timeStart));

    #endif
    //timeEnd = GetSysTimeByUsec();
    //printf("---------the function use time %d us\n",(timeEnd-timeBegin));
    return 0;
}



/* sp420 to p420 ; sp422 to p422  */
void SAMPLE_COMM_YUV_DUMP(VIDEO_FRAME_S * pVBuf, FILE *pfd)
{
#ifdef TEST_21A
    unsigned int w, h;
    char * pVBufVirt_Y;
    char * pVBufVirt_C;
    char * pMemContent;
    unsigned char TmpBuff[8192];
    FY_U32 phy_addr,size;
	FY_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    FY_U32 u32UvHeight;/* uv height when saved for planar type */

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
    }
    else
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    //printf("phy_addr:%x, size:%d\n", phy_addr, size);
    pUserPageAddr[0] = (FY_CHAR *) FY_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }
    //printf("stride: %d,%d\n",pVBuf->u32Stride[0],pVBuf->u32Stride[1] );

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Y......");
    fflush(stderr);
    for(h=0; h<pVBuf->u32Height; h++)
    {
        pMemContent = pVBufVirt_Y + h*pVBuf->u32Stride[0];
        fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
    }
    fflush(pfd);


    /* save U ----------------------------------------------------------------*/
    fprintf(stderr, "U......");
    fflush(stderr);
    for(h=0; h<u32UvHeight; h++)
    {
        pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

        pMemContent += 1;

        for(w=0; w<pVBuf->u32Width/2; w++)
        {
            TmpBuff[w] = *pMemContent;
            pMemContent += 2;
        }
        fwrite(TmpBuff, pVBuf->u32Width/2, 1, pfd);
    }
    fflush(pfd);

    /* save V ----------------------------------------------------------------*/
    fprintf(stderr, "V......");
    fflush(stderr);
    for(h=0; h<u32UvHeight; h++)
    {
        pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

        for(w=0; w<pVBuf->u32Width/2; w++)
        {
            TmpBuff[w] = *pMemContent;
            pMemContent += 2;
        }
        fwrite(TmpBuff, pVBuf->u32Width/2, 1, pfd);
    }
    fflush(pfd);

    fprintf(stderr, "done %d!\n", pVBuf->u32TimeRef);
    fflush(stderr);

    FY_MPI_SYS_Munmap(pUserPageAddr[0], size);
#else
    FY_U8 * pVBufVirt_Y;
    FY_U8 * pVBufVirt_UV;
    //FY_U8 * pVBufVirt_U;
    //FY_U8 * pVBufVirt_V;
	FY_U32 u32Size[3];
    //FY_U32 i=0,j=0,k=0;
    //int a=0;
    FY_U32 phy_addr,size,size1,size2;


    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;

    phy_addr = pVBuf->u32PhyAddr[0];

	#if 0
	u32Size[0] = *(FY_U32 *)pVBuf->pHeaderVirAddr[0];
	u32Size[1] = *(FY_U32 *)pVBuf->pHeaderVirAddr[1];
	u32Size[2] = *(FY_U32 *)pVBuf->pHeaderVirAddr[2];
	#endif
	if(PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat){
		u32Size[0] = pVBuf->u32Width * pVBuf->u32Height ;
		u32Size[1] = pVBuf->u32Width * pVBuf->u32Height/2;
	}
	else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat){
		u32Size[0] = pVBuf->u32Width * pVBuf->u32Height;
		u32Size[1] = pVBuf->u32Width * pVBuf->u32Height;
	}
	else{
		u32Size[0] = pVBuf->u32Width * pVBuf->u32Height;
		u32Size[1] = pVBuf->u32Width * pVBuf->u32Height;
	}

    size1 = u32Size[0];
    //printf("phy_addr is 0x%x,enPixelFormat is %d, size1 is %d ,enCompressMode is %d\n",phy_addr,enPixelFormat,size1,pVBuf->enCompressMode);
    if(size1 == 0){
        printf("error ! the data size is 0 !  \n");
        return;
    }
    if(pVBuf->enCompressMode == COMPRESS_MODE_SLICE)
    {
    	size = u32Size[0] + u32Size[1];
    	phy_addr = phy_addr-SLICE_HEAD_SIZE;
		//printf("slice phy_addr is 0x%x,enPixelFormat is %d, size is %d, valid size is 0x%x \n",phy_addr,enPixelFormat,size,*(FY_U32 *)pVBuf->pHeaderVirAddr[0]);
        pVBufVirt_Y = FY_MPI_SYS_Mmap(phy_addr, size+SLICE_HEAD_SIZE);
        fwrite(pVBufVirt_Y, 1, (size+SLICE_HEAD_SIZE), pfd);

        FY_MPI_SYS_Munmap(pVBufVirt_Y, (size+SLICE_HEAD_SIZE));
    }
    else if((pVBuf->enCompressMode == COMPRESS_MODE_TILE_224)||(pVBuf->enCompressMode == COMPRESS_MODE_TILE_192)||(pVBuf->enCompressMode == COMPRESS_MODE_TILE_256))
    {
        pVBufVirt_Y= FY_MPI_SYS_Mmap(phy_addr, size1);
        fwrite(pVBufVirt_Y, 1, size1, pfd);

        FY_MPI_SYS_Munmap(pVBufVirt_Y, size1);
    }
	else if(VIDEO_FIELD_INTERLACED == pVBuf->u32Field){
		printf("saving......CVBS......\n");
		phy_addr = pVBuf->u32PhyAddr[0];
		size = pVBuf->u32Width * pVBuf->u32Height * 2;
		pVBufVirt_Y = FY_MPI_SYS_Mmap(phy_addr, size);
		if(NULL == pVBufVirt_Y) {
			printf("mmap the phyaddr: 0x%x failed!\n", phy_addr);
			return;
		}
		{
			int i = 0;
			FY_U8* pty  = pVBufVirt_Y;
			FY_U8* pby  = pVBufVirt_Y + pVBuf->u32Width * pVBuf->u32Height / 2;
			FY_U8* pc  = pVBufVirt_Y + pVBuf->u32Width * pVBuf->u32Height;
			for(i = 0; i < pVBuf->u32Height / 2; i++) {
				fwrite(pty + i * pVBuf->u32Width, 1, pVBuf->u32Width, pfd);
				fwrite(pby + i * pVBuf->u32Width, 1, pVBuf->u32Width, pfd);
			}
			fwrite(pc, 1, pVBuf->u32Width * pVBuf->u32Height, pfd);
		}
		FY_MPI_SYS_Munmap((void*)pVBufVirt_Y, size);
	}
    else
    {
        pVBufVirt_Y = (FY_U8  *) FY_MPI_SYS_Mmap(phy_addr, size1);
        if (NULL == pVBufVirt_Y)
        {
            printf("mmap error\n");
            return;
        }

        /* save Y ----------------------------------------------------------------*/
        printf("saving......Y......\n");
        fwrite(pVBufVirt_Y, 1,size1,pfd);
        fflush(pfd);


        size2 = u32Size[1];
        phy_addr = pVBuf->u32PhyAddr[1];

        pVBufVirt_UV = (FY_U8  *) FY_MPI_SYS_Mmap(phy_addr, size2);
        if (NULL == pVBufVirt_UV)
        {
            printf("mmap error\n");
            return;
        }

        /* save UV ----------------------------------------------------------------*/
        printf("saving......UV......\n");
        fwrite(pVBufVirt_UV, 1,size2,pfd);
        fflush(pfd);
        FY_MPI_SYS_Munmap(pVBufVirt_Y, size1);
        FY_MPI_SYS_Munmap(pVBufVirt_UV, size2);
    }
#endif
}


FY_S32 SAMPLE_COMM_VpssDump(VPSS_GRP Grp,VPSS_CHN Chn,FY_U32 u32FrameCnt,FY_U32 u32Width,FY_U32 u32Height,FY_U32 u32PixelFormat,FY_U32 u32CompMode,FY_U32 buserGlobe,FY_U32 compRate)
{
    VIDEO_FRAME_INFO_S stFrame;
    FY_CHAR szYuvName[128];
    FY_CHAR szPixFrm[20];
	FY_U32 u32Size[3];
    FILE *pfd;
    VPSS_GRP VpssGrp = Grp;
    VPSS_CHN VpssChn = Chn;
    FY_U32 u32Cnt = u32FrameCnt;
    FY_U32 u32Depth = 2;
    FY_U32 u32OrigDepth = 0;
    VPSS_CHN_MODE_S stOrigVpssMode, stVpssMode={0};
    FY_S32 s32MilliSec = -1;
    FY_S32 s32ErrNum = 0;
#if 1
	if(1 != buserGlobe) {

	    if (FY_MPI_VPSS_GetDepth(VpssGrp, VpssChn, &u32OrigDepth) != FY_SUCCESS)
	    {
	        printf("get depth error!!!\n");
	        return -1;
	    }
	    if (FY_MPI_VPSS_SetDepth(VpssGrp,VpssChn,u32Depth)!=FY_SUCCESS)
	    {
	    	printf("set depth error!!!\n");
	        return -1;
	    }
		if(COMPRESS_MODE_SLICE != u32CompMode){
			memset(&stOrigVpssMode,0,sizeof(VPSS_CHN_MODE_S));

		    if (FY_MPI_VPSS_GetChnMode(VpssGrp,VpssChn,&stOrigVpssMode) != FY_SUCCESS)
		    {
		    	printf("get mode error!!!\n");
		        return -1;
		    }


		    memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));

		    stVpssMode = stOrigVpssMode;
		    stVpssMode.enChnMode = VPSS_CHN_MODE_USER;

			stVpssMode.enPixelFormat = u32PixelFormat;

		    stVpssMode.u32Width = u32Width;
		    stVpssMode.u32Height = u32Height;
		    stVpssMode.enCompressMode = u32CompMode;

			stVpssMode.enPixelFormat = u32PixelFormat;
			printf("the u32PixelFormat is %d, compress mode is %d\n",u32PixelFormat,u32CompMode);
			if(COMPRESS_MODE_TILE_224 == u32CompMode){
				stVpssMode.CompRate = compRate;
			}
			else if(COMPRESS_MODE_SLICE == u32CompMode){
				stVpssMode.CompRate = compRate;
			}

		    if (FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stVpssMode) != FY_SUCCESS)
		    {
		    	printf("set mode error!!!\n");
		        return -1;
		    }
		}

	}
#endif
    memset(&stFrame,0,sizeof(VIDEO_FRAME_INFO_S));
    while (FY_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec)!=FY_SUCCESS)
    {
    	printf("get frame error!!!\n");
        usleep(40000);
        s32ErrNum++;
        if(s32ErrNum>3)
            return 0;
    }

    /* make file name */
    if(PIXEL_FORMAT_YUV_SEMIPLANAR_422== stFrame.stVFrame.enPixelFormat){
		if(COMPRESS_MODE_SLICE == stFrame.stVFrame.enCompressMode)
	        strcpy(szPixFrm,"sp422_slice");
		else
			strcpy(szPixFrm,"sp422");
	}
    else if(PIXEL_FORMAT_YUV_SEMIPLANAR_420== stFrame.stVFrame.enPixelFormat){
		if(COMPRESS_MODE_TILE_192 == stFrame.stVFrame.enCompressMode)
			strcpy(szPixFrm,"sp420_tile_50");
		else if(COMPRESS_MODE_TILE_224 == stFrame.stVFrame.enCompressMode)
			strcpy(szPixFrm,"sp420_tile_59");
		else if(COMPRESS_MODE_TILE_256 == stFrame.stVFrame.enCompressMode)
	        strcpy(szPixFrm,"sp420_tile_67");
		else
			strcpy(szPixFrm,"sp420");

	}
	else if(PIXEL_FORMAT_YUV_420_BLOCK == stFrame.stVFrame.enPixelFormat)
		strcpy(szPixFrm,"sp420_block");

    sprintf(szYuvName, "./stream/vpss_grp%d_chn%d_%dx%d_%s_%d_%d.yuv", VpssGrp, VpssChn,
        stFrame.stVFrame.u32Width, stFrame.stVFrame.u32Height,szPixFrm,compRate,u32Cnt);
    printf("Dump YUV frame of vpss chn %d  to file: \"%s\"\n", VpssChn, szYuvName);
    fflush(stdout);

	if(1 != buserGlobe)
	    FY_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
    /* open file */
    pfd = fopen(szYuvName, "wb");

    if (NULL == pfd)
    {
        printf("file handle is NULL\n");
        return -1;
    }

    /* get frame  */
    while (u32Cnt--)
    {
        memset(&stFrame,0,sizeof(stFrame));
		stFrame.stVFrame.pHeaderVirAddr[0] = &u32Size[0];
		stFrame.stVFrame.pHeaderVirAddr[1] = &u32Size[1];
		stFrame.stVFrame.pHeaderVirAddr[2] = &u32Size[2];
        if (FY_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec) != FY_SUCCESS)
        {
            printf("Get frame fail \n");
			u32Cnt++;
            usleep(1000);
            continue;
        }
        if(stFrame.stVFrame.u32PhyAddr[0]==0)
            continue;
        SAMPLE_COMM_YUV_DUMP(&stFrame.stVFrame, pfd);

        printf("Get VpssGrp %d frame %d!!\n", VpssGrp,u32Cnt);
        /* release frame after using, globe chn can not release */
		if(1 != buserGlobe)
	        FY_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
    }

    fclose(pfd);
	chmod(szYuvName,0777);
	if(1 != buserGlobe) {
	    FY_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32OrigDepth);
		if(COMPRESS_MODE_SLICE != u32CompMode)
		    FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stOrigVpssMode);
	}
    return 0;
}



FY_S32 SAMPLE_COMM_VpssDump_ALL(stVpssInfo *pst_VpssInfo)
{
    VIDEO_FRAME_INFO_S stFrame;
    FY_CHAR szYuvName[128];
    FY_CHAR szPixFrm[20];
	FY_U32 u32Size[3];
    FILE *pfd;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    FY_U32 u32Cnt = pst_VpssInfo->stDumpFramInfo.DumpFrameInfo[0].u32FrameCnt;
    FY_U32 u32Depth = 2;
    FY_U32 u32OrigDepth = 0;
    FY_S32 s32MilliSec = -1;
    VPSS_CHN_MODE_S stOrigVpssMode, stVpssMode={0};

	printf("u32Cnt is %d \n",u32Cnt);
	if(0 == u32Cnt){
		u32Cnt = 3;
	}
	for(VpssGrp=0;VpssGrp<pst_VpssInfo->u32GrpNum;VpssGrp++){
		for(VpssChn=0;VpssChn<VPSS_MAX_CHN_NUM;VpssChn++){
			u32Cnt = pst_VpssInfo->stDumpFramInfo.DumpFrameInfo[0].u32FrameCnt;
			if((pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bConfig)){
				if(1 == pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bUseGloble){
                    continue;
                }
                else{
					if (FY_MPI_VPSS_GetDepth(VpssGrp, VpssChn, &u32OrigDepth) != FY_SUCCESS){
						printf("get depth error!!!\n");
						return -1;
					}
					if (FY_MPI_VPSS_SetDepth(VpssGrp,VpssChn,u32Depth)!=FY_SUCCESS) {
						printf("set depth error!!!\n");
						return -1;
					}

                    memset(&stOrigVpssMode,0,sizeof(VPSS_CHN_MODE_S));
        			stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;

                    if (FY_MPI_VPSS_GetChnMode(VpssGrp,VpssChn,&stOrigVpssMode) != FY_SUCCESS)
                    {
                        printf("get mode error!!!\n");
                        return -1;
                    }


                    memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));
                    stVpssMode = stOrigVpssMode;
                    stVpssMode.u32Width = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].u32Width;
        		    stVpssMode.u32Height = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].u32Height;
                    stVpssMode.enPixelFormat = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].enPixelFormat;
                    stVpssMode.enCompressMode = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].enCompressMode;
                    stVpssMode.CompRate = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].CompRate;
                    stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
                    stVpssMode.GlobCfg.bUseGloble = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bUseGloble;
				    stVpssMode.GlobCfg.glob_idx = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].glob_idx;
				    stVpssMode.GlobCfg.pic_pos.s32X = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].s32X;
				    stVpssMode.GlobCfg.pic_pos.s32Y = pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].s32Y;

                    if (FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stVpssMode) != FY_SUCCESS)
                    {
                        printf("set mode error!!!\n");
                        return -1;
                    }
                }

				memset(&stFrame,0,sizeof(VIDEO_FRAME_INFO_S));
			    while (FY_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec)!=FY_SUCCESS)
			    {
			    	printf("get frame error!!!\n");
			        usleep(40000);
			    }

			    /* make file name */
			    if(PIXEL_FORMAT_YUV_SEMIPLANAR_422== stFrame.stVFrame.enPixelFormat){
					if(COMPRESS_MODE_SLICE == stFrame.stVFrame.enCompressMode)
				        strcpy(szPixFrm,"sp422_slice");
					else
						strcpy(szPixFrm,"sp422");
				}
			    else if(PIXEL_FORMAT_YUV_SEMIPLANAR_420== stFrame.stVFrame.enPixelFormat){
					if(COMPRESS_MODE_TILE_192 == stFrame.stVFrame.enCompressMode)
						strcpy(szPixFrm,"sp420_tile_50");
					else if(COMPRESS_MODE_TILE_224 == stFrame.stVFrame.enCompressMode)
						strcpy(szPixFrm,"sp420_tile_59");
					else if(COMPRESS_MODE_TILE_256 == stFrame.stVFrame.enCompressMode)
						strcpy(szPixFrm,"sp420_tile_67");
					else
						strcpy(szPixFrm,"sp420");
				}
				else if(PIXEL_FORMAT_YUV_420_BLOCK == stFrame.stVFrame.enPixelFormat)
					strcpy(szPixFrm,"sp420_block");

			    sprintf(szYuvName, "./stream/vpss_grp%d_chn%d_%dx%d_%s_%d.yuv", VpssGrp, VpssChn,
			        stFrame.stVFrame.u32Width, stFrame.stVFrame.u32Height,szPixFrm,u32Cnt);
			    printf("Dump YUV frame of vpss grp %d chn %d  to file: \"%s\"\n",VpssGrp,VpssChn, szYuvName);
			    fflush(stdout);

				if(1 != pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bUseGloble)
				    FY_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
			    /* open file */
			    pfd = fopen(szYuvName, "wb");

			    if (NULL == pfd)
			    {
			        printf("file handle is NULL\n");
			        return -1;
			    }

			    /* get frame  */
			    while (u32Cnt--)
			    {
			        memset(&stFrame,0,sizeof(stFrame));
					stFrame.stVFrame.pHeaderVirAddr[0] = &u32Size[0];
					stFrame.stVFrame.pHeaderVirAddr[1] = &u32Size[1];
					stFrame.stVFrame.pHeaderVirAddr[2] = &u32Size[2];
			        if (FY_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec) != FY_SUCCESS)
			        {
			            printf("Get frame fail \n");
						u32Cnt++;
			            usleep(1000);
			            continue;
			        }
			        if(stFrame.stVFrame.u32PhyAddr[0]==0)
			            continue;
			        SAMPLE_COMM_YUV_DUMP(&stFrame.stVFrame, pfd);

			        printf("Get VpssGrp %d frame %d!!\n", VpssGrp,u32Cnt);
			        /* release frame after using, globe chn can not release */
					if(1 != pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bUseGloble)
				        FY_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
			    }

			    fclose(pfd);
				chmod(szYuvName,0777);
				if(1 != pst_VpssInfo->st_VpssGrpInfo[VpssGrp].VpssChnMode[VpssChn].bUseGloble) {
				    FY_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32OrigDepth);
                    FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stOrigVpssMode);
				}
			}

		}
	}
    return 0;
}


void* SAMPLE_COMM_VPSS_SendFrameProc(void *param)
{
	FY_U32   u32Size=0;
        VB_BLK VbBlk[MAX_BLK_CNT];
        FY_U32 u32PhyAddr;
        FY_U32 u32Width;
        FY_U32 u32Height;
        FY_U32 u32PixelFormat;
        FY_U8 *pVirAddr;
        FY_U32 i =0;
        FY_U32 ret =0;
        FILE * pfd[VPSS_MAX_GRP_NUM];
        FY_U32 u32BufIdx;
        FY_U32 u32FrameIndex = 0;

        VIDEO_FRAME_INFO_S sendVideoFrame[MAX_BLK_CNT];

        memset(&sendVideoFrame,0,sizeof(VIDEO_FRAME_INFO_S)*BLK_CNT);

        SAMPLE_VPSS_SENDSTRM_PARA_S  * sendParam = NULL;
        sendParam = (SAMPLE_VPSS_SENDSTRM_PARA_S  *)param;

        u32PixelFormat = sendParam->u32PixelFormat;

        /* open YUV file */
        for(i=0;i<sendParam->s32Cnt;i++)
        {
            pfd[i] = fopen(sendParam->SendFrameInfo[i].inputFile, "rb");
            if (!pfd[i])
            {
                printf("open file -> %s fail \n", (char *)sendParam->SendFrameInfo[i].inputFile);
                goto ERROR;
            }
        }

        for(u32BufIdx=0;u32BufIdx<sendParam->s32Cnt*BLK_CNT;u32BufIdx++)
        {
            i=u32BufIdx%(sendParam->s32Cnt);

            u32Width = sendParam->SendFrameInfo[i].u32Width;//sendParam->u32Width;
            u32Height = sendParam->SendFrameInfo[i].u32Height;

            if(3 == u32PixelFormat)
                u32Size =  u32Width*u32Height*2 + SLICE_HEAD_SIZE;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16)*2;
            else if(1 == u32PixelFormat)
				u32Size =  u32Width*u32Height*2;
            else if(0 == u32PixelFormat)
                u32Size =   u32Width*u32Height*3/2;//ALIGN_UP(u32Width, 16)* ALIGN_UP(u32Height, 16)*3/2;

            //for(j=0;j<BLK_CNT;j++)
            {
                VbBlk[u32BufIdx] = FY_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
                if (VB_INVALID_HANDLE == VbBlk[u32BufIdx])
                {
                    printf("FY_MPI_VB_GetBlock err! size:%d\n",u32Size);
                    goto ERROR;
                }
                u32PhyAddr = FY_MPI_VB_Handle2PhysAddr(VbBlk[u32BufIdx]);
                if (0 == u32PhyAddr)
                {
                    goto ERROR;
                }

                pVirAddr = (FY_U8 *) FY_MPI_SYS_Mmap(u32PhyAddr, u32Size);
                if (NULL == pVirAddr)
                {
                    goto ERROR;
                }

                sendVideoFrame[u32BufIdx].u32PoolId = FY_MPI_VB_Handle2PoolId(VbBlk[u32BufIdx]);
                if (VB_INVALID_POOLID == sendVideoFrame[u32BufIdx].u32PoolId)
                {
                    goto RELEASE_BLK;
                }
                 sendVideoFrame[u32BufIdx].stVFrame.u32PhyAddr[0] = u32PhyAddr;
                 sendVideoFrame[u32BufIdx].stVFrame.pVirAddr[0] = pVirAddr;
                 //u32BufIdx ++;
            }
        }

        u32BufIdx = 0;
        while(gs_stSendFrmPara.bThreadStart)
        {
            //for(i=0;i<BLK_CNT;i++)
            for(i=0;i<sendParam->s32Cnt;i++)
            {
                u32Width = sendParam->SendFrameInfo[i].u32Width;//sendParam->u32Width;
                u32Height = sendParam->SendFrameInfo[i].u32Height;
                #if 1 //for Y..., U...., V....
                if(0 == u32PixelFormat)
                {
                           /* read YUV file. WARNING: we only support planar 420) */
                    if (SAMPLE_COMM_GetVFrameFromYUV(pfd[i], u32Width, u32Height, u32Width, &sendVideoFrame[u32BufIdx],u32PixelFormat))
                    {
                        	fclose(pfd[i]);
                        	printf("SAMPLE_COMM_GetVFrameFromYUV error!\n");
                        	goto RELEASE_BLK;
                    }
                }
                else
                #endif
                //for Y......, UVUVUV......
                {
                    if (SAMPLE_COMM_GetVFrameFromYUVsp(pfd[i], u32Width, u32Height, u32Width, &sendVideoFrame[u32BufIdx],u32PixelFormat))
                    {
                        	fclose(pfd[i]);
                        	printf("SAMPLE_COMM_GetVFrameFromYUV error!\n");
                        	goto RELEASE_BLK;
                    }
                }
                u32FrameIndex += 2;
                sendVideoFrame[u32BufIdx].stVFrame.u32TimeRef = u32FrameIndex;
                //SendFrame
                {
                    ret = FY_MPI_VPSS_SendFrame(i, &sendVideoFrame[u32BufIdx], 0);
                    //printf("========================send frame\n");
                    if(0 != ret)
                        printf("error code is 0x%x\n",ret);
                    else
                        ;//printf("send success! \n");
                    usleep(10000);
                }
                u32BufIdx ++;
                u32BufIdx = (u32BufIdx == (BLK_CNT*sendParam->s32Cnt))?0:(u32BufIdx);
            }
        }
RELEASE_BLK:
    for(i=0;i<BLK_CNT*sendParam->s32Cnt;i++)
    {
        FY_MPI_SYS_Munmap(sendVideoFrame[i].stVFrame.pVirAddr[0],u32Size);
        FY_MPI_VB_ReleaseBlock(VbBlk[i]);
    }
ERROR:
    for(i=0;i<sendParam->s32Cnt;i++)
    {
        if(pfd[i])
            fclose(pfd[i]);
    }
    return NULL;
}

FY_S32 SAMPLE_COMM_VPSS_StartSendStream()
{

    gs_stSendFrmPara.bThreadStart = FY_TRUE;
    return pthread_create(&gs_VpssPid, 0, SAMPLE_COMM_VPSS_SendFrameProc, (FY_VOID*)&gs_stSendFrmPara);
}

FY_S32 SAMPLE_COMM_VPSS_StopSendStream()
{
    if (FY_TRUE == gs_stSendFrmPara.bThreadStart)
    {
        gs_stSendFrmPara.bThreadStart = FY_FALSE;
        if (gs_VpssPid)
        {
            pthread_join(gs_VpssPid, 0);
            gs_VpssPid = 0;
        }
    }
    return FY_SUCCESS;
}



#if 0
FY_S32 SAMPLE_COMM_DisableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize)
{
    FY_S32 s32Ret;
    VPSS_PRESCALE_INFO_S stPreScaleInfo;

    stPreScaleInfo.bPreScale = FY_FALSE;
    stPreScaleInfo.stDestSize.u32Width = stSize.u32Width;
    stPreScaleInfo.stDestSize.u32Height = stSize.u32Height;
    s32Ret = FY_MPI_VPSS_SetPreScale(VpssGrp, &stPreScaleInfo);
    if (s32Ret != FY_SUCCESS)
	{
	    SAMPLE_PRT("FY_MPI_VPSS_SetPreScale failed with %#x!\n", s32Ret);
	    return FY_FAILURE;
	}

    return s32Ret;
}
FY_S32 SAMPLE_COMM_EnableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize)
{
    FY_S32 s32Ret;
    VPSS_PRESCALE_INFO_S stPreScaleInfo;

    stPreScaleInfo.bPreScale = FY_TRUE;
    stPreScaleInfo.stDestSize.u32Width = stSize.u32Width;
    stPreScaleInfo.stDestSize.u32Height = stSize.u32Height;
    s32Ret = FY_MPI_VPSS_SetPreScale(VpssGrp, &stPreScaleInfo);
    if (s32Ret != FY_SUCCESS)
	{
	    SAMPLE_PRT("FY_MPI_VPSS_SetPreScale failed with %#x!\n", s32Ret);
	    return FY_FAILURE;
	}

    return s32Ret;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
