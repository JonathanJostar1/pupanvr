#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <signal.h>

#include "mpi_tde.h"
#include "fy_comm_tde.h"
#include "mpi_fb.h"
#include "fy_comm_vo.h"
#include "mpi_sys.h"
#include "mpi_vo.h"
#include "sample_comm.h"

#include "sample_ui.h"

#define CURSOR_LENGTH       32
#define BACKGROUNGD_WIDTH   1920
#define BACKGROUNGD_HEIGHT  1080

#define PARTIAL_UI_SHOW
#define CURSOR_SHOW
#define ALPHA_CHANGE
//#define FMT_ARGB8888
//#define PERFORMACE_TEST

#ifndef FMT_ARGB8888
#define CURSOR_NAME      "res/cursor.bits"
#define COLOR_FORMAT     TDE2_COLOR_FMT_ARGB1555
#else
#define CURSOR_NAME      "res/cursor_8888.bits"
#define COLOR_FORMAT     TDE2_COLOR_FMT_ARGB8888
#endif

static const FY_CHAR *pszBgNames[] =
{
#ifndef FMT_ARGB8888
    "res/UI1_1080p.bits",
    "res/UI2_1080p.bits",
    "res/UI3_1080p.bits",
#else
    "res/UI1_1080p_8888.bits",
#endif
};

static const FY_CHAR *pColorNames = "green.bits";

static int ui_mode = 0;
static int ui_state = FY_SUCCESS;
static FY_BOOL uiRun = FY_FALSE;
static FY_BOOL uiAuto = FY_FALSE;
static FY_BOOL uiManual = FY_FALSE;
static FY_BOOL uiCompress = FY_FALSE;
static int num_color = 0;
FY_U8 type[2];
FY_BOOL bShowCursorPro = 0;
FY_BOOL bDrawGraphicPro = 0;
pthread_t pFb0 = -1;
pthread_t pFb1 = -1;
pthread_t pGraphic0 = -1;
pthread_t pGraphic1 = -1;


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

FY_S32 TDE_CreateSurfaceFromFile(const FY_CHAR *pszFileName, TDE2_SURFACE_S *pstSurface, FY_U8 *pu8Virt)
{
    FILE *fp;
    FY_U32 colorfmt, w, h, stride;

    if((NULL == pszFileName) || (NULL == pstSurface))
    {
        printf("%s, LINE %d, NULL ptr!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    fp = fopen(pszFileName, "rb");
    if(NULL == fp)
    {
        printf("error when open pszFileName %s, line:%d\n", pszFileName, __LINE__);
        return -1;
    }

    fread(&colorfmt, 1, 4, fp);
    fread(&w, 1, 4, fp);
    fread(&h, 1, 4, fp);
    fread(&stride, 1, 4, fp);

    pstSurface->enColorFmt = colorfmt;
    pstSurface->u32Width = w;
    pstSurface->u32Height = h;
    pstSurface->u32Stride = stride;
    pstSurface->u8Alpha0 = 0;
    pstSurface->u8Alpha1 = 255;
    pstSurface->bAlphaMax255 = FY_TRUE;
    pstSurface->bAlphaExt1555 = FY_TRUE;
    pstSurface->u8Alpha0 = 0;
    pstSurface->u8Alpha1 = 255;

    fread(pu8Virt, 1, stride*h, fp);

    fclose(fp);

    return 0;
}

FY_VOID *SAMPLE_TDE_CURSORSHOW(void *pData)
{
    TDE_HANDLE s32Handle;
	FY_S32 s32Ret, fd;
    FY_U32 u32PhyAddr, stepW, stepH;
	FY_U8 *pShowScreen;
    TDE2_SURFACE_S g_stCursor;
	FYFB_POINT_S stPoint = {0, 0};
    FYFB_ALPHA_S stAlpha = {0};
	FY_BOOL bShow;
    FY_VOID *pVirAddr = NULL;
    TDE2_SURFACE_S stSrc;
    TDE2_RECT_S stSrcRect;
    TDE2_RECT_S stDstRect;
    FY_U8 *pTypt = (FY_U8 *)pData;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_U32 screanWidth, screanHeight;
    FYFB_COLORKEY_S stColorKey;

	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;

    if(0 == *pTypt){
        s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stLayerAttr);
        if(s32Ret)
        {
            printf("Curosr failed to get vo layer(%d) info!\n", SAMPLE_VO_LAYER_VHD0);
            return FY_NULL;
        }

        fd = open("/dev/fb2", O_RDWR, 0);
        if(fd < 0)
        {
            printf("open /dev/fb2 failed!\n");
            return FY_NULL;
        }
        prctl(PR_SET_NAME, "UI_HdCursor");
    }else{
        s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
        if(s32Ret)
        {
            printf("Curosr failed to get vo layer(%d) info!\n", SAMPLE_VO_LAYER_VSD0);
            return FY_NULL;
        }

        fd = open("/dev/fb3", O_RDWR, 0);
        if(fd < 0)
        {
            printf("open /dev/fb3 failed!\n");
            return FY_NULL;
        }
        prctl(PR_SET_NAME, "UI_SdCursor");
    }
    screanWidth = stLayerAttr.stImageSize.u32Width;
    screanHeight = stLayerAttr.stImageSize.u32Height;

    bShow = FY_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitFb;
    }

    /* set the screen original position */
    stPoint.s32XPos = 0;
    stPoint.s32YPos = 0;
    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_FYFB, &stPoint) < 0)
    {
        printf("set screen original show position failed!\n");
        goto exitFb;
    }

    /* get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        printf("Get variable screen info failed!\n");
        goto exitFb;
    }

    /* modify the variable screen info */
    //usleep(1*1000*1000);
    var.xres_virtual = CURSOR_LENGTH;
    var.yres_virtual = CURSOR_LENGTH;
    var.xres = CURSOR_LENGTH;
    var.yres = CURSOR_LENGTH;
    var.transp= s_alp;
    var.red = s_red;
    var.green = s_gre;
    var.blue = s_blu;
    var.bits_per_pixel = bitDep;
    var.activate = FB_ACTIVATE_NOW;

    /* set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        printf("Put variable screen info failed!\n");
        goto exitFb;
    }

    /* get the fix screen info */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        printf("Get fix screen info failed!\n");
        goto exitFb;
    }

    /* map the physical video memory for user use */
    u32PhyAddr  = fix.smem_start;
    pShowScreen = mmap(FY_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(NULL == pShowScreen)
    {
        printf("mmap fb2 failed!\n");
        goto exitFb;
    }
    memset(pShowScreen, 0x00, fix.smem_len);

    /* time to play*/
    bShow = FY_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_FYFB failed!\n");
        goto exit;
    }

    /* resize cursor to show */
    g_stCursor.enColorFmt = COLOR_FORMAT;
    g_stCursor.u32PhyAddr = u32PhyAddr;
    g_stCursor.u32Width = CURSOR_LENGTH;
    g_stCursor.u32Height = CURSOR_LENGTH;
    g_stCursor.u32Stride = fix.line_length;
    g_stCursor.bAlphaMax255 = FY_TRUE;
    g_stCursor.bAlphaExt1555 = FY_TRUE;
    g_stCursor.u8Alpha0 = 0;
    g_stCursor.u8Alpha1 = 255;

    if(FY_FAILURE == FY_MPI_SYS_MmzAlloc(&stSrc.u32PhyAddr, &pVirAddr, "ui_cur", NULL, 64*64*byteDep)){
        printf("allocate cursor memory (64*64*2 bytes) failed\n");
        goto exit;
    }

    TDE_CreateSurfaceFromFile(CURSOR_NAME, &stSrc, pVirAddr);

    stSrcRect.s32Xpos = 0;
    stSrcRect.s32Ypos = 0;
    stSrcRect.u32Width = 64;
    stSrcRect.u32Height = 64;

    stDstRect.s32Xpos = 0;
    stDstRect.s32Ypos = 0;
    stDstRect.u32Width = CURSOR_LENGTH;
    stDstRect.u32Height = CURSOR_LENGTH;

    s32Ret = FY_TDE2_Open();

    s32Handle = FY_TDE2_BeginJob();
    if(FY_ERR_TDE_INVALID_HANDLE == s32Handle)
    {
        printf("start job failed!\n");
        goto out;
    }

    s32Ret = FY_TDE2_QuickResize(s32Handle, &stSrc, &stSrcRect, &g_stCursor, &stDstRect);
    if (s32Ret < 0)
    {
        printf("Line:%d, FY_TDE2_QuickResize failed! ret:0x%x\n", __LINE__, s32Ret);
        FY_TDE2_CancelJob(s32Handle);
        goto out;
    }

    s32Ret = FY_TDE2_EndJob(s32Handle, FY_TRUE, FY_FALSE, 0);
    if(s32Ret < 0)
    {
        printf("Line:%d, FY_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);
        goto out;
    }

    stColorKey.bKeyEnable = FY_TRUE;
    stColorKey.u32Key = 0x0;
    if (ioctl(fd, FBIOPUT_COLORKEY_FYFB, &stColorKey) < 0)  //just for cursor
    {
        printf("Put color key failed!\n");
        goto out;
    }

    stAlpha.bAlphaChannel = FY_TRUE;
    stAlpha.bAlphaEnable = FY_TRUE;
    stAlpha.u8Alpha0 = 0;
    stAlpha.u8Alpha1 = 255;
    stAlpha.u8GlobalAlpha = 128;
    if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
    {
        printf("Put alpha info failed!\n");
        goto out;
    }

    if (ioctl(fd, FBIOPAN_DISPLAY, &var) < 0)
    {
        printf("FBIOPAN_DISPLAY failed!\n");
        goto out;
    }

    usleep(200*1000);

    stepW = 4;
    stepH = 4;

    while(bShowCursorPro)
    {
        stPoint.s32XPos += stepW;
        stPoint.s32YPos += stepH;

        if((stPoint.s32XPos + CURSOR_LENGTH) > (screanWidth)){
            stepW = -4;
            stPoint.s32XPos += stepW * 2;
        }else if(stPoint.s32XPos < 0){
            stepW = 4;
            stPoint.s32XPos += stepW * 2;
        }

        if((stPoint.s32YPos + CURSOR_LENGTH) > (screanHeight)){
            stepH = -4;
            stPoint.s32YPos += stepH * 2;
        }else if(stPoint.s32YPos < 0){
            stepH = 4;
            stPoint.s32YPos += stepH * 2;
        }

        if(ioctl(fd, FBIOPUT_SCREEN_ORIGIN_FYFB, &stPoint) < 0)
        {
            printf("LINE:%d: set screen original show position failed!\n", __LINE__);
            goto out;
        }

        usleep(20*1000);
    }

    /* unmap the physical memory */
out:
    FY_TDE2_Close();
    FY_MPI_SYS_MmzFree(stSrc.u32PhyAddr, pVirAddr);
exit:
    memset(pShowScreen, 0, fix.smem_len);
    munmap(pShowScreen, fix.smem_len);
    pShowScreen = NULL;
exitFb:
    close(fd);

    return FY_NULL;
}

FY_S32 TDE_CursorShowSample()
{
    FY_S32 s32Ret = 0;

    bShowCursorPro = 1;

    type[0] = 0;
    s32Ret = pthread_create(&pFb0, 0, SAMPLE_TDE_CURSORSHOW, &type[0]);
    if(s32Ret){
        printf("create thread to show cursor failed!\n");
        return s32Ret;
    }

#ifdef PERFORMACE_TEST
    type[1] = 1;
    s32Ret = pthread_create(&pFb1, 0, SAMPLE_TDE_CURSORSHOW, &type[1]);
    if(s32Ret){
        printf("create thread to show cursor failed!\n");
        return s32Ret;
    }
#endif

    return s32Ret;
}

FY_S32 TDE_DrawUiBySize(FY_S32 fd, TDE2_SURFACE_S *pstScreen,
    TDE2_SURFACE_S *pstBackGround, FY_BOOL bPartail, struct fb_var_screeninfo *pstVarInfo)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 width, height;
    TDE2_RECT_S stSrcRect;
    TDE2_RECT_S stShowRect;
    TDE_HANDLE s32Handle;
#ifdef ALPHA_CHANGE
    FYFB_ALPHA_S stAlpha = {0};
    FY_S32 uiAlpha = 128;
#endif

    if(bPartail){
        width = pstScreen->u32Width/2;
        height = pstScreen->u32Height/2;

        stShowRect.s32Xpos = width/2;
        stShowRect.s32Ypos = height/2;
    }else{
        width = pstScreen->u32Width;
        height = pstScreen->u32Height;

        stShowRect.s32Xpos = 0;
        stShowRect.s32Ypos = 0;
    }
    stShowRect.u32Width = width;
    stShowRect.u32Height = height;

    stSrcRect.s32Xpos = 0;
    stSrcRect.s32Ypos = 0;
    if(ui_mode == 0){
        stSrcRect.u32Width = BACKGROUNGD_WIDTH;
        stSrcRect.u32Height = BACKGROUNGD_HEIGHT;
    }else{
        stSrcRect.u32Width = 640;       //depends on file green.bits
        stSrcRect.u32Height = 480;
    }

    s32Handle = FY_TDE2_BeginJob();
    if(FY_ERR_TDE_INVALID_HANDLE == s32Handle)
    {
        printf("start job failed!\n");
        ui_state = FY_FAILURE;
        return -1;
    }

    s32Ret = FY_TDE2_QuickResize(s32Handle, pstBackGround, &stSrcRect, pstScreen, &stShowRect);
    if (s32Ret < 0)
    {
        printf("FY_TDE2_QuickResize failed! ret:0x%x\n",s32Ret);
        ui_state = FY_FAILURE;
        FY_TDE2_CancelJob(s32Handle);
        return -1;
    }

    s32Ret = FY_TDE2_EndJob(s32Handle, FY_TRUE, 0, 0);
    if(s32Ret < 0)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x\n", s32Ret);
        ui_state = FY_FAILURE;
        return -1;
    }

    if (ioctl(fd, FBIOPAN_DISPLAY, pstVarInfo) < 0)
    {
        printf("display failed\n");
        return -1;
    }

    usleep(20*1000);

#ifdef ALPHA_CHANGE
    if(ui_mode == 0){
        while(uiAlpha >= 0 && bDrawGraphicPro)
        {
            if(!uiAuto)
            {
                while(1)
                {
                    if(uiManual || uiAuto){
                        uiManual = FY_FALSE;
                        break;
                    }
                    sleep(1);
                }
            }

            stAlpha.bAlphaChannel = FY_TRUE;
            stAlpha.bAlphaEnable = FY_TRUE;
            stAlpha.u8Alpha0 = 0;
            stAlpha.u8Alpha1 = 255;
            stAlpha.u8GlobalAlpha = uiAlpha;
            if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
            {
                printf("Put alpha info failed!\n");
                break;
            }

            uiAlpha = uiAlpha - 42;

            if(!bDrawGraphicPro)
                break;
#ifdef PERFORMACE_TEST
            usleep(333*1000);
#else
            sleep(1);
#endif
        }
    }else{
        stAlpha.bAlphaChannel = FY_TRUE;
        stAlpha.bAlphaEnable = FY_TRUE;
        stAlpha.u8Alpha0 = 0;
        stAlpha.u8Alpha1 = 255;
        stAlpha.u8GlobalAlpha = 50;
        if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
        {
            printf("Put alpha info failed!\n");
            return -1;
        }

        while(bDrawGraphicPro){
            sleep(1);
        }
    }
#else
    sleep(1);
#endif

    return 0;
}

FY_VOID *SAMPLE_TDE_GRAPHICDRAW(void *pData)
{
    FY_BOOL bCompress;
    FY_BOOL bComSupport;
    FY_BOOL bShow;
    FY_S32 i, fd, s32Ret;
    FY_U32 u32PhyAddr;
    FY_U8* g_pu8BackGroundVir = NULL;
    FY_U8 *pShowScreen;
    TDE2_SURFACE_S g_stBackGround;
    TDE2_SURFACE_S g_stScreen;
    FY_U8 *pTypt = (FY_U8 *)pData;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_U32 screanWidth, screanHeight;
    FYFB_ALPHA_S stAlpha = {0};

    struct fb_fix_screeninfo stFixInfo;
    struct fb_var_screeninfo stVarInfo;

    if(0 == *pTypt){
        s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stLayerAttr);
        if(s32Ret)
        {
            printf("failed to get vo layer(%d) info!\n", SAMPLE_VO_LAYER_VHD0);
            return FY_NULL;
        }

        fd = open("/dev/fb0", O_RDWR, 0);
        if(fd < 0)
        {
            printf("open /dev/fb0 failed!\n");
            return FY_NULL;
        }
        bComSupport = FY_TRUE;
        prctl(PR_SET_NAME, "UI_HD");
    }else{
        s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VSD0, &stLayerAttr);
        if(s32Ret)
        {
            printf("failed to get vo layer(%d) info!\n", SAMPLE_VO_LAYER_VSD0);
            return FY_NULL;
        }

        fd = open("/dev/fb1", O_RDWR, 0);
        if(fd < 0)
        {
            printf("open /dev/fb1 failed!\n");
            return FY_NULL;
        }
        bComSupport = FY_FALSE;
        prctl(PR_SET_NAME, "UI_SD");
    }
    screanWidth = stLayerAttr.stImageSize.u32Width;
    screanHeight = stLayerAttr.stImageSize.u32Height;

    if (FY_FAILURE == FY_MPI_SYS_MmzAlloc(&(g_stBackGround.u32PhyAddr), ((void**)&g_pu8BackGroundVir), "ui_bg", NULL, BACKGROUNGD_WIDTH*BACKGROUNGD_HEIGHT*byteDep))
    {
        printf("allocate memory 0 (%d bytes) failed\n", BACKGROUNGD_WIDTH*BACKGROUNGD_HEIGHT*byteDep);
        goto exitFb;
    }

	bShow = FY_FALSE;
	if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
	{
		SAMPLE_PRT("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitMmz;
	}

    if(bComSupport){
        if(uiCompress){
            bCompress = FY_TRUE;
        }else{
            bCompress = FY_FALSE;
        }
        if (ioctl(fd, FBIOPUT_COMPRESSION_FYFB, &bCompress) < 0)
        {
            printf(" FBIOPUT_COMPRESSION_FYFB failed!\n");
            goto exitMap;
        }
    }

    /* get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("Get variable screen info failed!\n");
        goto exitMmz;
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
        goto exitMmz;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &stFixInfo) < 0)
    {
        printf("process frame buffer device error\n");
        goto exitMmz;
    }

    u32PhyAddr  = stFixInfo.smem_start;
    pShowScreen   = mmap(NULL, stFixInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (NULL == pShowScreen)
    {
        printf("mmap fb0 failed!\n");
        goto exitMmz;
    }
    memset(pShowScreen, 0x00, stFixInfo.smem_len);

    g_stScreen.enColorFmt = COLOR_FORMAT;
    g_stScreen.u32PhyAddr = u32PhyAddr;
    g_stScreen.u32Width = screanWidth;
    g_stScreen.u32Height = screanHeight;
    g_stScreen.u32Stride = stFixInfo.line_length;
    g_stScreen.bAlphaMax255 = FY_TRUE;
    g_stScreen.bAlphaExt1555 = FY_TRUE;
    g_stScreen.u8Alpha0 = 0;
    g_stScreen.u8Alpha1 = 255;

    bShow = FY_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitMap;
    }

    stAlpha.bAlphaChannel = FY_TRUE;
    stAlpha.bAlphaEnable = FY_TRUE;
    stAlpha.u8Alpha0 = 0;
    stAlpha.u8Alpha1 = 255;
    stAlpha.u8GlobalAlpha = 128;
    if (ioctl(fd, FBIOPUT_ALPHA_FYFB, &stAlpha) < 0)
    {
        printf("Put alpha info failed!\n");
        goto exitMap;
    }

    s32Ret = FY_TDE2_Open();
    if(s32Ret)
    {
        printf("Open tde failed!\n");
        ui_state = FY_FAILURE;
        goto exitMap;
    }

    if(ui_mode == 0){
        num_color = (FY_S32)((sizeof (pszBgNames) / sizeof (pszBgNames[0])));
    }else{
        num_color = 1;
        pszBgNames[0] = pColorNames;
        printf("UI open file named %s\n", pszBgNames[0]);
    }

    i = 0;
    while(bDrawGraphicPro)
    {
        TDE_CreateSurfaceFromFile(pszBgNames[i++], &g_stBackGround, g_pu8BackGroundVir);
        if(i > num_color - 1)
            i = 0;

        s32Ret = TDE_DrawUiBySize(fd, &g_stScreen, &g_stBackGround, 0, &stVarInfo);
        if(s32Ret)
            printf("draw globle UI failed!\n");

#ifdef PARTIAL_UI_SHOW
        if(ui_mode == 0){
            memset(pShowScreen, 0x00, stFixInfo.smem_len);

            s32Ret = TDE_DrawUiBySize(fd, &g_stScreen, &g_stBackGround, 1, &stVarInfo);
            if(s32Ret)
                printf("draw partial UI failed!\n");
        }
#endif
    }

    FY_TDE2_Close();
exitMap:
    memset(pShowScreen, 0, stFixInfo.smem_len);
    munmap(pShowScreen, stFixInfo.smem_len);
    pShowScreen = NULL;
exitMmz:
    FY_MPI_SYS_MmzFree(g_stBackGround.u32PhyAddr, g_pu8BackGroundVir);
    g_pu8BackGroundVir = NULL;
exitFb:
    close(fd);

    return FY_NULL;
}

FY_S32 TDE_GraphicDrawSample()
{
    FY_S32 s32Ret = 0;

    bDrawGraphicPro = 1;

    s32Ret = pthread_create(&pGraphic0, 0, SAMPLE_TDE_GRAPHICDRAW, &type[0]);
    if(s32Ret){
        printf("create thread to draw graphic failed!\n");
        return s32Ret;
    }
    if(ui_mode == 1)
        printf("create thread to draw graphic ok!\n");

#ifdef PERFORMACE_TEST
    type[1] = 1;
    s32Ret = pthread_create(&pGraphic1, 0, SAMPLE_TDE_GRAPHICDRAW, &type[1]);
    if(s32Ret){
        printf("create thread to draw graphic failed!\n");
        return s32Ret;
    }
#endif

    return s32Ret;
}

FY_S32 ui_start()
{
    FY_S32 s32Ret = 0;
    uiManual = FY_FALSE;
    uiAuto = FY_TRUE;
    FILE *fpPara = NULL;
    int len = 0;

    fpPara = fopen("/sys/module/fyfb/parameters/fbc", "rb");
    FY_U8 *pu8Buf = malloc(4);
    len = fread(pu8Buf, 1, 4, fpPara);
    if (len > 0 && *pu8Buf > 48) {
        uiCompress = FY_TRUE;
    }
    fclose(fpPara);

#ifdef CURSOR_SHOW
    if(ui_mode == 0){
        s32Ret = TDE_CursorShowSample();
        if(s32Ret != FY_SUCCESS)
        {
            return -1;
        }
    }
#endif

    s32Ret = TDE_GraphicDrawSample();
    if(s32Ret != FY_SUCCESS)
    {
        return -1;
    }

    uiRun = FY_TRUE;

    return s32Ret;
}

FY_VOID ui_manualSwitch()
{
    uiManual = FY_TRUE;
    uiAuto = FY_FALSE;
}

FY_VOID ui_autoSwitch()
{
    uiManual = FY_FALSE;
    uiAuto = FY_TRUE;
}

FY_VOID ui_stop()
{
    uiRun = FY_FALSE;
    uiAuto = FY_TRUE;

    if(bShowCursorPro){
        bShowCursorPro = 0;
        pthread_join(pFb0,0);
#ifdef PERFORMACE_TEST
        pthread_join(pFb1,0);
#endif
    }

    if(bDrawGraphicPro){
        bDrawGraphicPro = 0;
        pthread_join(pGraphic0,0);
#ifdef PERFORMACE_TEST
        pthread_join(pGraphic1,0);
#endif
    }

    return;
}

FY_VOID ui_stop_pushed()
{
    uiAuto = FY_TRUE;

    if(bShowCursorPro){
        bShowCursorPro = 0;
        pthread_join(pFb0,0);
#ifdef PERFORMACE_TEST
        pthread_join(pFb1,0);
#endif
    }

    if(bDrawGraphicPro){
        bDrawGraphicPro = 0;
        pthread_join(pGraphic0,0);
#ifdef PERFORMACE_TEST
        pthread_join(pGraphic1,0);
#endif
    }

    return;
}

FY_S32 ui_testmode(int testmode)
{
    if (testmode > 0) {
        ui_mode = 1;
    } else {
        ui_mode = 0;
    }

    return 0;
}

FY_BOOL ui_getState()
{
    return uiRun;
}

FY_S32 ui_query()
{
    return ui_state;
}

