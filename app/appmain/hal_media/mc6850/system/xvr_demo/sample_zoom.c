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
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>

#include "sample_comm.h"
#include "sample_vo.h"
#include "sample_comm_vdec.h"
#include "sample_playback.h"
#include "sample_zoom.h"


#define REFRESH_BACKUP_FRAME        1


FY_S32 g_s32ChnNum = 1;
FY_U32 g_u32Width = 1920;
FY_U32 g_u32Height = 1080;
FY_U32 g_u32ZoomChn = 0;
FY_BOOL g_bPlay = FY_TRUE;
FY_BOOL g_bZoom = FY_FALSE;
FY_BOOL g_bPause = FY_FALSE;

pthread_t g_pPlayBack = -1;
SAMPLE_VO_MODE_E g_mux;;

static void flush_stdin()
{
    char c = 0;
    while((c = getchar()) != '\n' && c != EOF) ;
}

static int Sample_Zoom_Usage()
{
    //show the zoom step
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: ZOOM Step" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_CHAR('i', "zoom in by step");
    MENU_ITEM_CHAR('o', "zoom out by step");
    MENU_ITEM_CHAR('w', "move point up");
    MENU_ITEM_CHAR('s', "move point down");
    MENU_ITEM_CHAR('a', "move point left");
    MENU_ITEM_CHAR('d', "move point right");
    MENU_ITEM_CHAR('q', "Quit to zoom chan");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice:");
    return 0;
}

static int Sample_Zoom_Channel_menu(FY_S32 chnId)
{
    FY_S32 i;
    //show the zoom channel
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: ZOOM Chan" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    for(i=0;i<chnId;i++){
        MENU_ITEM_CHAR_NUM(i+97, "play  to zoom channel", i);
        MENU_ITEM_CHAR_NUM(i+65, "pause to zoom channel", i);
    }
    MENU_ITEM_CHAR('q', "Quit to zoom test");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice:");
    return 0;
}

static FY_S32 Sample_Zoom_VgsInit(VGS_CHN VgsChn)
{
    VGS_CHN_PARA_S VgsMode = {
        .enChnMode = VGS_CHN_MODE_AUTO,
        .u32Width = 1920,
        .u32Height = 1080,
        .enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    };

    FY_MPI_VGS_CreateChn(VgsChn);
    FY_MPI_VGS_SetChnMode(VgsChn, 0, &VgsMode);

    return FY_SUCCESS;
}

static FY_S32 Sample_Zoom_PipInit(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer = SAMPLE_VO_LAYER_VPIP;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stChnAttr;

    memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stLayerAttr.stImageSize.u32Width = ALIGN_BACK(g_u32Width / 3, 8);
    stLayerAttr.stImageSize.u32Height = ALIGN_BACK(g_u32Height / 3, 2);
    stLayerAttr.stDispRect.u32Width   = ALIGN_BACK(g_u32Width / 3, 8);
    stLayerAttr.stDispRect.u32Height  = ALIGN_BACK(g_u32Height / 3, 2);
    stLayerAttr.stDispRect.s32X       = g_u32Width - stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stDispRect.s32Y       = g_u32Height - stLayerAttr.stDispRect.u32Height;

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);

    stChnAttr.stRect.u32Width = ALIGN_BACK(g_u32Width / 3, 8);
    stChnAttr.stRect.u32Height  = ALIGN_BACK(g_u32Height / 3, 2);
    stChnAttr.stRect.s32X = g_u32Width - stChnAttr.stRect.u32Width;
    stChnAttr.stRect.s32Y = g_u32Height - stChnAttr.stRect.u32Height;
    stChnAttr.u32Priority       = 0;
    stChnAttr.bDeflicker        = FY_FALSE;

    s32Ret = FY_MPI_VO_SetChnAttr(VoLayer, 0, &stChnAttr);
    if (s32Ret != FY_SUCCESS)
    {
        printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__, s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_SetChnFrameRate(VoLayer, 0, 30);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_EnableChn(VoLayer, 0);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

static FY_S32 Sample_Zoom_PipDeinit(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer = SAMPLE_VO_LAYER_VPIP;

    FY_MPI_VO_DisableChn(VoLayer, 0);
    s32Ret = SAMPLE_COMM_VO_StopLayer(VoLayer);

    return s32Ret;
}

static FY_VOID Sample_Zoom_init(FY_S32 chnId)
{
    FY_S32 i;

    for(i=0;i<g_s32ChnNum;i++){
        SAMPLE_COMM_VGS_UnBindVo(i, SAMPLE_VO_LAYER_VHD0, i);
        FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0, i);
        FY_MPI_VGS_DestroyChn(i);
    }

    Sample_Zoom_PipInit();
    SAMPLE_COMM_VO_StartChn(SAMPLE_VO_LAYER_VHD0, VO_MODE_1MUX);

    Sample_Zoom_VgsInit(chnId);
    Sample_Zoom_VgsInit(16);
    SAMPLE_COMM_VGS_BindVo(chnId, SAMPLE_VO_LAYER_VHD0, 0);
    SAMPLE_COMM_VGS_BindVo(16, SAMPLE_VO_LAYER_VPIP, 0);
    //SAMPLE_COMM_VDEC_BindVgs(chnId, 16);
}

static FY_VOID Sample_Zoom_Deinit(FY_S32 chnId)
{
    FY_S32 i;

    //SAMPLE_COMM_VDEC_UnBindVgs(chnId, 16);
    SAMPLE_COMM_VGS_UnBindVo(16, SAMPLE_VO_LAYER_VPIP, 0);
    SAMPLE_COMM_VGS_UnBindVo(chnId, SAMPLE_VO_LAYER_VHD0, 0);

    FY_MPI_VGS_DestroyChn(16);
    FY_MPI_VGS_DestroyChn(chnId);
    FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0, 0);

    Sample_Zoom_PipDeinit();
    SAMPLE_COMM_VO_StartChn(SAMPLE_VO_LAYER_VHD0, g_mux);

    for(i=0;i<g_s32ChnNum;i++){
        Sample_Zoom_VgsInit(i);
        SAMPLE_COMM_VGS_BindVo(i, SAMPLE_VO_LAYER_VHD0, i);
    }
}

static FY_VOID Sample_Zoom_Channel(FY_S32 chnId, FY_BOOL bPause)
{
    FY_CHAR ch;
    VGS_CROP_PARA_S stCropInfo;
    FY_S32 step = 10;
    FY_S32 ret = 0;
#ifndef REFRESH_BACKUP_FRAME
    VIDEO_FRAME_INFO_S stVideoFrame;
#endif

    if(chnId >= g_s32ChnNum){
        return;
    }

    g_bZoom = FY_TRUE;
    Sample_Zoom_init(chnId);
    stCropInfo.stCropRect.s32X = 0;
    stCropInfo.stCropRect.s32Y = 0;
    stCropInfo.stCropRect.u32Width = 1000;
    stCropInfo.stCropRect.u32Height = 1000;

    if(bPause){
        FY_MPI_VGS_EnableBackupFrame(chnId);
        usleep(30*1000);
        g_bPause = FY_TRUE;
    }

    while(1)
    {
        Sample_Zoom_Usage();
        ch = getchar();
        if('\n' == ch)
        {
            continue;
        }
        getchar();
        switch (ch)
        {
            case 'i':
            case 'I':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.enCropCoordinate = VGS_CROP_RATIO_COOR;
                stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;
                stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;
                stCropInfo.stCropRect.u32Width = stCropInfo.stCropRect.u32Width - 2 * step;
                stCropInfo.stCropRect.u32Height = stCropInfo.stCropRect.u32Height - 2* step;

                if(stCropInfo.stCropRect.u32Width < 4000 / g_u32Width || stCropInfo.stCropRect.u32Height < 4000 / g_u32Height ||
                    stCropInfo.stCropRect.u32Width > 1000 || stCropInfo.stCropRect.u32Height > 1000){

                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X - step;
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y - step;
                    stCropInfo.stCropRect.u32Width = stCropInfo.stCropRect.u32Width + 2 * step;
                    stCropInfo.stCropRect.u32Height = stCropInfo.stCropRect.u32Height + 2* step;
                    //printf("check vgs chn%d crop in [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                    //    stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                    break;
                }

                if(stCropInfo.stCropRect.s32Y < 0 || stCropInfo.stCropRect.s32Y > 999){
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;
                }
                if(stCropInfo.stCropRect.s32X < 0 || stCropInfo.stCropRect.s32X > 999){
                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;
                }

                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);
                if(ret){
                    printf("set vgs chn%d crop failed! [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                        stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                }else{
                    //printf("set vgs chn%d crop ok! [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                    //    stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                }

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 'o':
            case 'O':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.enCropCoordinate = VGS_CROP_RATIO_COOR;
                stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X - step;
                stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y - step;
                stCropInfo.stCropRect.u32Width = stCropInfo.stCropRect.u32Width + 2 * step;
                stCropInfo.stCropRect.u32Height = stCropInfo.stCropRect.u32Height + 2* step;

                if(stCropInfo.stCropRect.u32Width < 4000 / g_u32Width || stCropInfo.stCropRect.u32Height < 4000 / g_u32Height ||
                    stCropInfo.stCropRect.u32Width > 1000 || stCropInfo.stCropRect.u32Height > 1000){

                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;
                    stCropInfo.stCropRect.u32Width = stCropInfo.stCropRect.u32Width - 2 * step;
                    stCropInfo.stCropRect.u32Height = stCropInfo.stCropRect.u32Height - 2* step;
                    //printf("check vgs chn%d crop out [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                    //    stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                    break;
                }

                if(stCropInfo.stCropRect.s32Y < 0 || stCropInfo.stCropRect.s32Y > 999){
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;
                }
                if(stCropInfo.stCropRect.s32X < 0 || stCropInfo.stCropRect.s32X > 999){
                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;
                }

                ret = FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);
                if(ret){
                    printf("set vgs chn%d crop failed! [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                        stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                }else{
                    //printf("set vgs chn%d crop ok! [%d, %d, %d, %d]\n", chnId, stCropInfo.stCropRect.s32X,
                    //    stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
                }

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 'w':
            case 'W':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y - step;

                if(stCropInfo.stCropRect.s32Y < 0 || stCropInfo.stCropRect.s32Y > 999){
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;
                    break;
                }

                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 's':
            case 'S':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y + step;

                if(stCropInfo.stCropRect.s32Y < 0 || stCropInfo.stCropRect.s32Y > 999){
                    stCropInfo.stCropRect.s32Y = stCropInfo.stCropRect.s32Y - step;
                    break;
                }

                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 'a':
            case 'A':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X - step;

                if(stCropInfo.stCropRect.s32X < 0 || stCropInfo.stCropRect.s32X > 999){
                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;
                    break;
                }

                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 'd':
            case 'D':
            {
                stCropInfo.bEnable = 1;
                stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X + step;

                if(stCropInfo.stCropRect.s32X < 0 || stCropInfo.stCropRect.s32X > 999){
                    stCropInfo.stCropRect.s32X = stCropInfo.stCropRect.s32X - step;
                    break;
                }

                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);

                if(bPause){
#ifdef REFRESH_BACKUP_FRAME
                    FY_MPI_VGS_RefreshChn(chnId);
#else
                    FY_MPI_VGS_GetChnFrame(chnId, &stVideoFrame, 0);
                    FY_MPI_VGS_SendFrame(chnId, &stVideoFrame, FY_TRUE, 200);
#endif
                }

                break;
            }
            case 'q':
            case 'Q':
            {
                stCropInfo.bEnable = 0;
                FY_MPI_VGS_SetChnCrop(chnId, &stCropInfo);
                Sample_Zoom_Deinit(chnId);
                g_bPause = FY_FALSE;
                g_bZoom = FY_FALSE;
                break;
            }
            default :
            {
                printf("input invaild! please try again.\n");
                break;
            }
        }

        if(g_bZoom == FY_FALSE){
            break;
        }
    }
}

static FY_VOID	* Sample_Zoom_GetImageSend(FY_VOID	*pArgs)
{
    int i, ret;
    FY_BOOL bStopStream = FY_FALSE;
    VIDEO_FRAME_INFO_S videoFrame;
    VdecGetImageThreadParam* parm =	(VdecGetImageThreadParam*)pArgs;

    parm->eCtrlSinal = VDEC_CTRL_START;

    while(1)
    {
        if(parm->eCtrlSinal == VDEC_CTRL_STOP){
            break;
        }

        if(g_bPause){
            for(i=0;i<parm->u32ChnCnt;i++)
                FY_MPI_VDEC_StopRecvStream(i);
            bStopStream = FY_TRUE;
            continue;
        }
        if(!g_bPause && bStopStream){
            for(i=0;i<parm->u32ChnCnt;i++)
                FY_MPI_VDEC_StartRecvStream(i);
            bStopStream = FY_FALSE;
            continue;
        }

        for(i=0;i<parm->u32ChnCnt;i++)
        {
            ret = FY_MPI_VDEC_GetImage(i, &videoFrame, 1000);
            if(ret){
                //printf("FY_MPI_VDEC_GetImage(%d,..)	failed! [0x%x]\n", i, ret);
                continue;
            }

            if(g_bZoom){
                if(g_u32ZoomChn == i){
                    ret = FY_MPI_VGS_SendFrame(i,&videoFrame,1,200);
                    ret = FY_MPI_VGS_SendFrame(16,&videoFrame,1,200);
                }
            }else{
                ret = FY_MPI_VGS_SendFrame(i,&videoFrame,1,200);//
            }

            ret	= FY_MPI_VDEC_ReleaseImage(i,&videoFrame);
            if(ret)
                printf("FY_MPI_VDEC_ReleaseImage(%d,..)	failed! [0x%x]\n", i, ret);
        }
        usleep(parm->s32IntervalTime*1000);
    }

    return NULL;
}

static int Sample_Zoom_Playback(FY_U32  s32ChnCnt, PAYLOAD_TYPE_E pt_types[], char* filenames[], int vb_cnt, int bothBind, SIZE_S astSizes[])
{
    int i, ret;

    VDEC_CHN_ATTR_S stVdecChnAttr[VDEC_CHN_NUM];
    VdecThreadParam stVdecSend[VDEC_CHN_NUM];
    pthread_t   VdecThread[VDEC_CHN_NUM];
    pthread_t  VdecReadThread;
    VdecGetImageThreadParam stVdecRead;


    SAMPLE_COMM_VDEC_ChnAttr(s32ChnCnt, &stVdecChnAttr[0],  pt_types, astSizes);
    for(i = 0; i < s32ChnCnt; i++) {
        stVdecChnAttr[i].stVdecVideoAttr.enMode = VIDEO_MODE_FRAME;
    }

    ret = SAMPLE_COMM_VDEC_Start(s32ChnCnt, stVdecChnAttr, vb_cnt);
    if(ret) {
        goto FAIL;
    }

    SAMPLE_COMM_VDEC_ThreadParam(g_s32ChnNum, &stVdecSend[0], &stVdecChnAttr[0], filenames, NULL, FY_TRUE);
    SAMPLE_COMM_VDEC_StartSendStream(s32ChnCnt, &stVdecSend[0], &VdecThread[0]);

    if(!bothBind) {
        stVdecRead.u32ChnCnt = s32ChnCnt;
        stVdecRead.s32IntervalTime = 2;
        pthread_create(&VdecReadThread, 0, Sample_Zoom_GetImageSend, (FY_VOID *)&stVdecRead);
    } else {
        for(i = 0; i < s32ChnCnt; i++) {
            ret = SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)i, (VGS_CHN)i);
        }
    }

    while(g_bPlay) {
        sleep(1);
    }

    SAMPLE_COMM_VDEC_StopSendStream(s32ChnCnt, &stVdecSend[0], &VdecThread[0]);

    if(!bothBind) {
        stVdecRead.eCtrlSinal = VDEC_CTRL_STOP;
        pthread_join(VdecReadThread, FY_NULL);
    }

    for(i = 0; i < s32ChnCnt; i++) {
        VDEC_CHN_STAT_S stStat;
        FY_MPI_VDEC_Query(stVdecSend[i].s32ChnId, &stStat);
        PRINTF_VDEC_CHN_STATE(stVdecSend[i].s32ChnId, stStat);
        FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0, i);
    }

FAIL:
    SAMPLE_COMM_VDEC_Stop(s32ChnCnt);
    for(i = 0; i < s32ChnCnt; i++) {
        if(bothBind) {
            ret = SAMPLE_COMM_VDEC_UnBindVgs(i, i);
            if(ret) {
                printf("ZOOM_VDEC_UnBindVgs(vdu, vgs) chn:%d failed!!!\n", i);
                return ret;
            }
        }
    }

    return ret;
}

static FY_VOID *Sample_Zoom_Choose(void *pData)
{
    char option = 0;

    while(1) {
        Sample_Zoom_Channel_menu(g_s32ChnNum);
        option = getchar();
        flush_stdin();

        if(option == 'q'){
            g_bPlay = FY_FALSE;
            break;
        }

        switch (option)
        {
            case 'a':
                g_u32ZoomChn = 0;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_FALSE);
                break;
            case 'A':
                g_u32ZoomChn = 0;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_TRUE);
                break;
            case 'b':
                g_u32ZoomChn = 1;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_FALSE);
                break;
            case 'B':
                g_u32ZoomChn = 1;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_TRUE);
                break;
            case 'c':
                g_u32ZoomChn = 2;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_FALSE);
                break;
            case 'C':
                g_u32ZoomChn = 2;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_TRUE);
                break;
            case 'd':
                g_u32ZoomChn = 3;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_FALSE);
                break;
            case 'D':
                g_u32ZoomChn = 3;
                Sample_Zoom_Channel(g_u32ZoomChn, FY_TRUE);
                break;
            default :
              break;
            }
        }

    return FY_NULL;
}

int Sample_ZOOM_Start(FY_S32 s32ChnCnt)
{
    FY_S32 i, ret;
    int vb_cnt = 4;
    int bBind = 0;
    SIZE_S stSize;
    VB_CONF_S stModVbConf;
    VDEC_MOD_PARAM_S mod_parm;
    PAYLOAD_TYPE_E play_types[VDEC_CHN_NUM];
    char* play_files[VDEC_CHN_NUM];
    SIZE_S astSizes[VDEC_CHN_NUM];
    char* stream_264   = "/nfs/stream/normal/h264/h264_960x1080_15fps_real_ip_15gop_800kbps.h264";
    char* stream_265 = "/nfs/stream/normal/h265/h265_1920x1080P_30fps_8Mbps.265";
    //char* stream_D1 = "/nfs/stream/normal/h264/D1.264";
    //char* stream_d1   = "/nfs/stream/us/D1_704x576.264";

    stSize.u32Width  = STREAM_HD_WIDTH;
    stSize.u32Height = STREAM_HD_HEIGHT;

    for(i = 0; i < VDEC_CHN_NUM; i++) {
        if(i==0){
            play_types[i]         = PT_H264;
            play_files[i]         = stream_264;
            astSizes[i].u32Width  = STREAM_HD_WIDTH;
            astSizes[i].u32Height = STREAM_HD_HEIGHT;
        }else if(i==1){
            play_types[i]         = PT_H265;
            play_files[i]         = stream_265;
            astSizes[i].u32Width  = STREAM_HD_WIDTH;
            astSizes[i].u32Height = STREAM_HD_HEIGHT;
        }else if(i==2){
            play_types[i]         = PT_H265;
            play_files[i]         = stream_265;
            astSizes[i].u32Width  = STREAM_HD_WIDTH;
            astSizes[i].u32Height = STREAM_HD_HEIGHT;
        }else{
            play_types[i]         = PT_H264;
            play_files[i]         = stream_264;
            astSizes[i].u32Width  = STREAM_HD_WIDTH;
            astSizes[i].u32Height = STREAM_HD_HEIGHT;
        }
    }

    if(s32ChnCnt == 1) {
        g_mux = VO_MODE_1MUX;
        g_s32ChnNum = 1;
    } else if(s32ChnCnt == 4) {
        g_mux = VO_MODE_4MUX;
        g_s32ChnNum = 4;
    } else {
        g_mux = VO_MODE_1MUX;
        g_s32ChnNum = 1;
    }

    ret = pthread_create(&g_pPlayBack, 0, Sample_Zoom_Choose, NULL);
    if(ret){
        printf("create thread to choose zoom channel failed!\n");
        goto Exit;
    }

    FY_MPI_VDEC_GetModParam(&mod_parm);
    if(mod_parm.u32VBSource == 0) {
        SAMPLE_COMM_VDEC_ModCommPoolConf_ext(&stModVbConf, play_types, &stSize, g_s32ChnNum, vb_cnt);
        SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
    }

    SAMPLE_VDEC_VO_Init(g_mux);
    SAMPLE_VDEC_VGS_Init(g_s32ChnNum, 1);
    SAMPLE_VDEC_VGS_Bind_VO(g_s32ChnNum);

    g_bPlay = FY_TRUE;
    Sample_Zoom_Playback(g_s32ChnNum, play_types, play_files, vb_cnt, bBind, astSizes);

    if(g_pPlayBack) {
        pthread_join(g_pPlayBack, FY_NULL);
    }

    SAMPLE_VDEC_VGS_UnBind_VO(g_s32ChnNum);
    SAMPLE_VDEC_VGS_DeInit(g_s32ChnNum);

Exit:
    return 0;
}




