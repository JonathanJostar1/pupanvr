#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>


#include "sample_comm.h"
#include "sample_vo.h"
#include "sample_comm_vdec.h"
#include "sample_playback.h"
#include "sample_zoom.h"
#include "sample_ui.h"
#include "mpi_fb.h"
#include "mpi_tde.h"


FY_BOOL g_bRotateGraphic = 0;
FY_BOOL g_bRotatePlay = FY_TRUE;
pthread_t g_pRotate = -1;
pthread_t g_pRotateGraphic = -1;
FY_U32 g_rotPhyAddr;
FY_VOID *g_pRotVirAddr = NULL;
TDE2_SURFACE_S g_stScreenSur;
FY_U32 g_u32PhyAddrScreen;
SAMPLE_VO_MODE_E g_RotMux;
FY_U32 u32PX, u32Y, u32But, u32Rit;
FY_U32 g_u32Width, g_u32Height;
FY_S32 g_u32ChnNum;


static const FY_CHAR *pszUiFile[] =
{
    "res/UI_rotate0_1080x1920.bits",
    "res/UI_rotate1_1080x1920.bits",
};

static void flush_stdin()
{
    char c = 0;
    while((c = getchar()) != '\n' && c != EOF) ;
}

FY_S32 Sample_Rotate_Fill(FY_U32 u32PartX, FY_U32 u32PartY, FY_U32 u32Button, FY_U32 u32Rit, TDE2_SURFACE_S *pstScreen)
{
    FY_S32 s32Ret = 0;
    TDE2_RECT_S stShowRect;
    TDE_HANDLE s32Handle;
    FY_U32 u32FillData = 0;

    stShowRect.s32Xpos = u32PartX;
    stShowRect.s32Ypos = u32PartY;
    stShowRect.u32Width = pstScreen->u32Width - u32PartX - u32Rit;
    stShowRect.u32Height = pstScreen->u32Height - u32PartY - u32Button;

    s32Ret = FY_TDE2_Open();
    if(s32Ret)
    {
        printf("Open tde failed!\n");
        return -1;
    }

    s32Handle = FY_TDE2_BeginJob();
    if(FY_ERR_TDE_INVALID_HANDLE == s32Handle)
    {
        printf("start job failed!\n");
        return -1;
    }

    u32FillData = 0x0;
    s32Ret = FY_TDE2_QuickFill(s32Handle, pstScreen, &stShowRect, u32FillData);
    if (s32Ret < 0)
    {
        printf("FY_TDE2_QuickFill failed! ret:0x%x\n",s32Ret);
        FY_TDE2_CancelJob(s32Handle);
        return -1;
    }

    s32Ret = FY_TDE2_EndJob(s32Handle, FY_TRUE, 0, 0);
    if(s32Ret < 0)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x\n", s32Ret);
        return -1;
    }

    return s32Ret;
}

FY_S32 Sample_Rotate_DrawGraphic(FY_S32 fd, TDE2_SURFACE_S *pstScreen, TDE2_SURFACE_S *pstBackGround, struct fb_var_screeninfo *pstVarInfo)
{
    FY_S32 s32Ret = FY_SUCCESS;
    TDE2_RECT_S stSrcRect, stRotRect;
    TDE2_SURFACE_S stRotSur;
    TDE_HANDLE s32Handle;
    VGS_TASK_ATTR_S stVgsTask;

    stSrcRect.s32Xpos = 0;
    stSrcRect.s32Ypos = 0;
    stSrcRect.u32Width = 1080;
    stSrcRect.u32Height = 1920;

    if(!g_bRotateGraphic)
        return -1;

    s32Handle = FY_TDE2_BeginJob();
    if(FY_ERR_TDE_INVALID_HANDLE == s32Handle)
    {
        printf("start job failed!\n");
        return -1;
    }

    stRotSur.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stRotSur.u32PhyAddr = g_rotPhyAddr;
    stRotSur.u32Width = pstScreen->u32Height;
    stRotSur.u32Height = pstScreen->u32Width;
    stRotSur.u32Stride = pstScreen->u32Height * 2;
    stRotSur.bAlphaMax255 = FY_TRUE;
    stRotSur.bAlphaExt1555 = FY_TRUE;
    stRotSur.u8Alpha0 = 0;
    stRotSur.u8Alpha1 = 255;

    stRotRect.s32Xpos = 0;
    stRotRect.s32Ypos = 0;
    stRotRect.u32Width = pstScreen->u32Height;
    stRotRect.u32Height = pstScreen->u32Width;

    s32Ret = FY_TDE2_QuickResize(s32Handle, pstBackGround, &stSrcRect, &stRotSur, &stRotRect);
    if (s32Ret < 0)
    {
        printf("FY_TDE2_QuickResize failed! ret:0x%x\n",s32Ret);
        FY_TDE2_CancelJob(s32Handle);
        return -1;
    }

    s32Ret = FY_TDE2_EndJob(s32Handle, FY_TRUE, 0, 0);
    if(s32Ret < 0)
    {
        printf("FY_TDE2_EndJob failed! ret:0x%x\n", s32Ret);
        return -1;
    }

    if(!g_bRotateGraphic)
        return -1;

    //Sample_Rotate_Fill(u32PX, u32Y, u32But, u32Rit, stRotSur);
    Sample_Rotate_Fill(u32Y, u32Rit, u32PX, u32But, &stRotSur);

    memset(&stVgsTask, 0 ,sizeof(stVgsTask));
    stVgsTask.stImgIn.stVFrame.u32Width = stRotSur.u32Width;
    stVgsTask.stImgIn.stVFrame.u32Height = stRotSur.u32Height;
    stVgsTask.stImgIn.stVFrame.enPixelFormat = PIXEL_FORMAT_RGB_1555;
    stVgsTask.stImgIn.stVFrame.u32PhyAddr[0] = g_rotPhyAddr;
    stVgsTask.stImgIn.stVFrame.u32PhyAddr[1] = g_rotPhyAddr + stRotSur.u32Width * stRotSur.u32Height;
    stVgsTask.stImgIn.stVFrame.u32Stride[0] = stRotSur.u32Width;
    stVgsTask.stImgIn.stVFrame.enCompressMode = COMPRESS_MODE_NONE;

    stVgsTask.stImgOut.stVFrame.u32Width = g_stScreenSur.u32Width;
    stVgsTask.stImgOut.stVFrame.u32Height = g_stScreenSur.u32Height;
    stVgsTask.stImgOut.stVFrame.enPixelFormat = PIXEL_FORMAT_RGB_1555;
    stVgsTask.stImgOut.stVFrame.u32PhyAddr[0] = g_u32PhyAddrScreen;
    stVgsTask.stImgOut.stVFrame.u32PhyAddr[1] = g_u32PhyAddrScreen + g_stScreenSur.u32Width * g_stScreenSur.u32Height;
    stVgsTask.stImgOut.stVFrame.u32Stride[0] = g_stScreenSur.u32Width;
    stVgsTask.stImgOut.stVFrame.enCompressMode = COMPRESS_MODE_NONE;

    s32Ret = FY_MPI_VGS_DoRotate(&stVgsTask, VGS_ROTATE_90);
    if (s32Ret < 0)
    {
        FY_MPI_VGS_CancelJob(s32Handle);
        printf("FY_MPI_VGS_AddRotateTask error, s32Ret:0x%x.\n", s32Ret);
        return -1;
    }

    if (ioctl(fd, FBIOPAN_DISPLAY, pstVarInfo) < 0)
    {
        printf("display failed\n");
        return -1;
    }

    if(g_bRotateGraphic)
        sleep(1);

    return 0;
}

FY_VOID *Sample_Rotate_UIThr(void *pData)
{
    FY_BOOL bShow;
    FY_S32 i, fd, s32Ret;
    FY_U8* g_pu8BackGroundVir = NULL;
    FY_U8 *pShowScreen;
    TDE2_SURFACE_S g_stBackGround;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_U32 screanWidth, screanHeight;
    FYFB_ALPHA_S stAlpha = {0};
    static int res_num = 0;

    struct fb_fix_screeninfo stFixInfo;
    struct fb_var_screeninfo stVarInfo;

    struct fb_bitfield stR = {10, 5, 0};
    struct fb_bitfield stG = {5, 5, 0};
    struct fb_bitfield stB = {0, 5, 0};
    struct fb_bitfield stA = {15, 1, 0};

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
    prctl(PR_SET_NAME, "ui");

    screanWidth = stLayerAttr.stImageSize.u32Width;
    screanHeight = stLayerAttr.stImageSize.u32Height;

    if (FY_FAILURE == FY_MPI_SYS_MmzAlloc(&(g_stBackGround.u32PhyAddr), ((void**)&g_pu8BackGroundVir), "zoom_bkg", NULL, 1080*1920*2))
    {
        printf("allocate zoom_bkg memory failed\n");
        goto exitFb;
    }
    if (FY_FAILURE == FY_MPI_SYS_MmzAlloc(&g_rotPhyAddr, &g_pRotVirAddr, "zoom_rot", NULL, 1920*1080*2))
    {
        printf("allocate memory (%d bytes) failed\n", 1920*1080*2);
        goto exitMmz;
    }

    bShow = FY_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_FYFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_FYFB failed!\n");
        goto exitMmz2;
    }

    /* get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("Get variable screen info failed!\n");
        goto exitMmz2;
    }

    stVarInfo.xres_virtual      = screanWidth;
    stVarInfo.yres_virtual      = screanHeight;
    stVarInfo.xres              = screanWidth;
    stVarInfo.yres              = screanHeight;
    stVarInfo.activate          = FB_ACTIVATE_NOW;
    stVarInfo.bits_per_pixel    = 16;
    stVarInfo.xoffset = 0;
    stVarInfo.yoffset = 0;
    stVarInfo.red   = stR;
    stVarInfo.green = stG;
    stVarInfo.blue  = stB;
    stVarInfo.transp = stA;

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &stVarInfo) < 0)
    {
        printf("process frame buffer device error\n");
        goto exitMmz2;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &stFixInfo) < 0)
    {
        printf("process frame buffer device error\n");
        goto exitMmz2;
    }

    g_u32PhyAddrScreen  = stFixInfo.smem_start;
    pShowScreen   = mmap(NULL, stFixInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (NULL == pShowScreen)
    {
        printf("mmap fb0 failed!\n");
        goto exitMmz2;
    }
    memset(pShowScreen, 0x00, stFixInfo.smem_len);

    g_stScreenSur.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    g_stScreenSur.u32PhyAddr = g_u32PhyAddrScreen;
    g_stScreenSur.u32Width = screanWidth;
    g_stScreenSur.u32Height = screanHeight;
    g_stScreenSur.u32Stride = stFixInfo.line_length;
    g_stScreenSur.bAlphaMax255 = FY_TRUE;
    g_stScreenSur.bAlphaExt1555 = FY_TRUE;
    g_stScreenSur.u8Alpha0 = 0;
    g_stScreenSur.u8Alpha1 = 255;

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
        goto exitMmz;
    }

    s32Ret = FY_TDE2_Open();
    if(s32Ret)
    {
        printf("Open tde failed!\n");
        goto exitMap;
    }

    res_num = (FY_S32)((sizeof (pszUiFile) / sizeof (pszUiFile[0])));

    i = 0;
    while(g_bRotateGraphic)
    {
        TDE_CreateSurfaceFromFile(pszUiFile[i++], &g_stBackGround, g_pu8BackGroundVir);
        if(i > res_num - 1)
            i = 0;

        s32Ret = Sample_Rotate_DrawGraphic(fd, &g_stScreenSur, &g_stBackGround, &stVarInfo);
        if(s32Ret)
            printf("draw globle UI failed!\n");
    }

    FY_TDE2_Close();


exitMap:
    memset(pShowScreen, 0, stFixInfo.smem_len);
    munmap(pShowScreen, stFixInfo.smem_len);
    pShowScreen = NULL;
exitMmz2:
    FY_MPI_SYS_MmzFree(g_rotPhyAddr, g_pRotVirAddr);
    g_pRotVirAddr = NULL;
exitMmz:
    FY_MPI_SYS_MmzFree(g_stBackGround.u32PhyAddr, g_pu8BackGroundVir);
    g_pu8BackGroundVir = NULL;
exitFb:
    close(fd);

    return FY_NULL;
}


FY_S32 Sample_Rotate_UiShow()
{
    FY_S32 s32Ret = 0;
    g_bRotateGraphic = 1;

    s32Ret = pthread_create(&g_pRotateGraphic, 0, Sample_Rotate_UIThr, NULL);
    if(s32Ret){
        printf("create thread to draw graphic failed!\n");
        return s32Ret;
    }

    return s32Ret;
}

FY_VOID Sample_Rotate_UiStop()
{
    if(g_bRotateGraphic){
        g_bRotateGraphic = 0;
        pthread_join(g_pRotateGraphic,0);
    }

    return;
}

FY_S32 Sample_Rotate_GetVoChnRet(VO_CHN voChn, RECT_S* pstRect, FY_U32 u32X, FY_U32 u32Y, FY_U32 u32But, FY_U32 u32Rit)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;
    FY_U32 u32Square = 0;
    FY_U32 u32CellH = 0;
    FY_U32 u32CellW = 0;

    if ( NULL == pstRect) {
        s32Ret = FY_ERR_VO_NULL_PTR;
        return s32Ret;
    }

    switch (g_RotMux) {
        case VO_MODE_1MUX:
            u32WndNum = 1;
            u32Square = 1;
            break;
        case VO_MODE_4MUX:
            u32WndNum = 4;
            u32Square = 2;
            break;
        case VO_MODE_9MUX:
            u32WndNum = 9;
            u32Square = 3;
            break;
        case VO_MODE_16MUX:
            u32WndNum = 16;
            u32Square = 4;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            u32Square = 5;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            u32Square = 6;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            u32Square = 2;
            break;
        case VO_MODE_4T_4B:
            u32WndNum = 8;
            u32Square = 4;
            break;
        default:
            break;
    }

    if(VO_MODE_1L_1R == g_RotMux) {
        u32CellW = ALIGN_BACK((g_u32Width - u32Rit - u32X)/2,  2);
        u32CellH = ALIGN_BACK((g_u32Height - u32But - u32Y ), 2);
    }else if(VO_MODE_4T_4B == g_RotMux) {
        u32CellW = ALIGN_BACK((g_u32Width - u32Rit - u32X)/4,  2);
        u32CellH = ALIGN_BACK((g_u32Height - u32But - u32Y )/2, 2);
    } else{
        u32CellW = ALIGN_BACK((g_u32Width - u32Rit - u32X) / u32Square, 2);
        u32CellH = ALIGN_BACK((g_u32Height - u32But - u32Y) /u32Square, 2);
    }

    if (voChn < u32WndNum) {
        pstRect->s32X = u32CellW * (voChn % u32Square) + u32X;
        pstRect->s32Y = u32CellH * (voChn / u32Square) + u32Y;
        pstRect->u32Width = u32CellW;
        pstRect->u32Height = u32CellH;
    }

    return s32Ret;
}

FY_S32 Sample_Rotate_ScreenInit(FY_U32 u32X, FY_U32 u32Y, FY_U32 u32But, FY_U32 u32Lft)
{
    FY_S32 i;
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer = SAMPLE_VO_LAYER_VHD0;
    VO_CHN_ATTR_S stChnAttr;
    FY_U32 u32WndNum = 0;

    switch(g_RotMux)
    {
        case VO_MODE_1MUX:
            u32WndNum = 1;
            break;
        case VO_MODE_4MUX:
            u32WndNum = 4;
            break;
        case VO_MODE_9MUX:
            u32WndNum = 9;
            break;
        case VO_MODE_16MUX:
            u32WndNum = 16;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            break;
        case VO_MODE_1B_5S:
            u32WndNum = 6;
            break;
        case VO_MODE_1B_7S:
            u32WndNum = 8;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            break;
        case VO_MODE_4T_4B:
            u32WndNum = 8;
            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
    }

    FY_MPI_VO_SetAttrBegin(VoLayer);

    for (i=0; i<u32WndNum; i++)
    {
        Sample_Rotate_GetVoChnRet(i, &stChnAttr.stRect, u32X, u32Y, u32But, u32Lft);

        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = FY_FALSE;

        s32Ret = FY_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != FY_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__, s32Ret);
            return FY_FAILURE;
        }

        s32Ret = FY_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }

    FY_MPI_VO_SetAttrEnd(VoLayer);

    return s32Ret;
}

FY_VOID Sample_Rotate_PlayWin(FY_U32 u32X, FY_S32 u32Y, FY_U32 u32But, FY_U32 u32Lft)
{
    FY_S32 i;

    for(i=0;i<g_u32ChnNum;i++){
        //FY_MPI_VDEC_StopRecvStream(i);
        SAMPLE_COMM_VGS_UnBindVo(i, SAMPLE_VO_LAYER_VHD0, i);
        FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0, i);
    }

    Sample_Rotate_ScreenInit(u32X, u32Y, u32But, u32Lft);

    for(i=0;i<g_u32ChnNum;i++){
        SAMPLE_COMM_VGS_BindVo(i, SAMPLE_VO_LAYER_VHD0, i);
        //FY_MPI_VDEC_StartRecvStream(i);
    }
}

static int Sample_Rotate_menu()
{
    int index = 0;
    //show the zoom channel
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: ZOOM Chan" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "show rotate ui");
    MENU_ITEM_INDEX(++index, "stop ui");
    MENU_ITEM_CHAR('q', "Quit to rotate test");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice:");
    return 0;
}

static FY_VOID *Sample_Rotate_Choose(void *pData)
{
    char option = 0;

    while(1) {
        Sample_Rotate_menu();
        option = getchar();
        flush_stdin();
        if(option == 'q'){
            Sample_Rotate_UiStop();
            g_bRotatePlay = FY_FALSE;
            break;
        }

        /*related to ui*/
        u32PX = 360;
        u32Y = 98;
        u32But = 48;
        u32Rit = 0;

        switch (option)
        {
            case '1':
                Sample_Rotate_UiShow();
                Sample_Rotate_PlayWin(u32PX, u32Y, u32But, u32Rit);
                break;
            case '2':
                Sample_Rotate_UiStop();
                Sample_Rotate_PlayWin(0, 0, 0, 0);
                break;
            default :
              break;
            }
        }

    return FY_NULL;
}

static FY_S32 Sample_Rotate_Playback(FY_U32  s32ChnCnt, PAYLOAD_TYPE_E pt_types[], char* filenames[], int vb_cnt, SIZE_S astSizes[])
{
    int i, ret;

    VDEC_CHN_ATTR_S stVdecChnAttr[VDEC_CHN_NUM];
    VdecThreadParam stVdecSend[VDEC_CHN_NUM];
    pthread_t   VdecThread[VDEC_CHN_NUM];


    SAMPLE_COMM_VDEC_ChnAttr(s32ChnCnt, &stVdecChnAttr[0],  pt_types, astSizes);
    for(i = 0; i < s32ChnCnt; i++) {
        stVdecChnAttr[i].stVdecVideoAttr.enMode = VIDEO_MODE_FRAME;
    }

    ret = SAMPLE_COMM_VDEC_Start(s32ChnCnt, stVdecChnAttr, vb_cnt);
    if(ret) {
        goto FAIL;
    }

    SAMPLE_COMM_VDEC_ThreadParam(s32ChnCnt, &stVdecSend[0], &stVdecChnAttr[0], filenames, NULL, FY_TRUE);
    SAMPLE_COMM_VDEC_StartSendStream(s32ChnCnt, &stVdecSend[0], &VdecThread[0]);

    for(i = 0; i < s32ChnCnt; i++) {
        ret = SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)i, (VGS_CHN)i);
    }

    while(g_bRotatePlay) {
        sleep(1);
    }

    SAMPLE_COMM_VDEC_StopSendStream(s32ChnCnt, &stVdecSend[0], &VdecThread[0]);


    for(i = 0; i < s32ChnCnt; i++) {
        VDEC_CHN_STAT_S stStat;
        FY_MPI_VDEC_Query(stVdecSend[i].s32ChnId, &stStat);
        PRINTF_VDEC_CHN_STATE(stVdecSend[i].s32ChnId, stStat);
        FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0, i);
    }

FAIL:
    SAMPLE_COMM_VDEC_Stop(s32ChnCnt);
    for(i = 0; i < s32ChnCnt; i++) {
        ret = SAMPLE_COMM_VDEC_UnBindVgs(i, i);
        if(ret) {
            printf("ZOOM_VDEC_UnBindVgs(vdu, vgs) chn:%d failed!!!\n", i);
            return ret;
        }
    }

    return ret;
}

FY_S32 Sample_Rotate_Start(FY_S32 s32ChnNum, VO_INTF_TYPE_E output)
{
    FY_S32 i, ret;
    int vb_cnt = 3;
    SIZE_S stSize;
    VB_CONF_S stModVbConf;
    VDEC_MOD_PARAM_S mod_parm;
    PAYLOAD_TYPE_E play_types[VDEC_CHN_NUM];
    char* play_files[VDEC_CHN_NUM];
    SIZE_S astSizes[VDEC_CHN_NUM];
    char* stream_264   = "/nfs/stream/normal/h264/h264_960x1080_15fps_real_ip_15gop_800kbps.h264";
    //char* stream_265 = "/nfs/stream/normal/h265/h265_1920x1080P_30fps_8Mbps.265";

    stSize.u32Width  = STREAM_HD_WIDTH;
    stSize.u32Height = STREAM_HD_HEIGHT;

    for(i = 0; i < VDEC_CHN_NUM; i++) {
        play_types[i]         = PT_H264;
        play_files[i]         = stream_264;
        astSizes[i].u32Width  = STREAM_HD_WIDTH;
        astSizes[i].u32Height = STREAM_HD_HEIGHT;
    }

    if(s32ChnNum == 2) {
        g_RotMux = VO_MODE_1L_1R;
    } else if(s32ChnNum == 8) {
        g_RotMux = VO_MODE_4T_4B;
    } else {
        g_RotMux = VO_MODE_1MUX;
    }
    g_u32ChnNum = s32ChnNum;
    g_u32Width = 1920;
    g_u32Height = 1080;

    ret = pthread_create(&g_pRotate, 0, Sample_Rotate_Choose, NULL);
    if(ret){
        printf("create thread to rotate contral failed!\n");
        return 0;
    }

    sample_vo_deinit(FY_TRUE);
    sample_vo_init(FY_TRUE, output);

    FY_MPI_VDEC_GetModParam(&mod_parm);
    if(mod_parm.u32VBSource == 0) {
        SAMPLE_COMM_VDEC_ModCommPoolConf_ext(&stModVbConf, play_types, &stSize, s32ChnNum, vb_cnt);
        SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
    }

    Sample_Rotate_ScreenInit(0, 0, 0, 0);
    SAMPLE_VDEC_VGS_Init(s32ChnNum, 1);
    SAMPLE_VDEC_VGS_Bind_VO(s32ChnNum);

    for(i=0;i<s32ChnNum;i++){
        FY_MPI_VO_SetChnFrameRate(0, i, 20);
        FY_MPI_VGS_SetRotate(i, 0, ROTATE_270);
    }

    g_bRotatePlay = FY_TRUE;
    Sample_Rotate_Playback(s32ChnNum, play_types, play_files, vb_cnt, astSizes);

    SAMPLE_VDEC_VGS_UnBind_VO(s32ChnNum);
    SAMPLE_VDEC_VGS_DeInit(s32ChnNum);

    return 0;
}


