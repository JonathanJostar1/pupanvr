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

#include "sample_comm.h"

FY_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, FY_U32 *pu32W,FY_U32 *pu32H, FY_U32 *pu32Frm)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_960H_NTSC :   *pu32W = 480;  *pu32H = 960;  *pu32Frm = 60; break;
        case VO_OUTPUT_PAL       :   *pu32W = 720;  *pu32H = 576;  *pu32Frm = 25; break;
        case VO_OUTPUT_NTSC      :   *pu32W = 720;  *pu32H = 480;  *pu32Frm = 30; break;
        case VO_OUTPUT_576P50    :   *pu32W = 720;  *pu32H = 576;  *pu32Frm = 50; break;
        case VO_OUTPUT_480P60    :   *pu32W = 720;  *pu32H = 480;  *pu32Frm = 60; break;
        case VO_OUTPUT_800x480_60:   *pu32W = 800;  *pu32H = 480;  *pu32Frm = 60; break;
        case VO_OUTPUT_800x600_60:   *pu32W = 800;  *pu32H = 600;  *pu32Frm = 60; break;
        case VO_OUTPUT_800x1280_60:  *pu32W = 800;  *pu32H = 1280; *pu32Frm = 60; break;
        case VO_OUTPUT_720P25    :   *pu32W = 1280; *pu32H = 720;  *pu32Frm = 25; break;
        case VO_OUTPUT_720P30    :   *pu32W = 1280; *pu32H = 720;  *pu32Frm = 30; break;
        case VO_OUTPUT_720P50    :   *pu32W = 1280; *pu32H = 720;  *pu32Frm = 50; break;
        case VO_OUTPUT_720P60    :   *pu32W = 1280; *pu32H = 720;  *pu32Frm = 60; break;
        case VO_OUTPUT_1080I50   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 25; break;
        case VO_OUTPUT_1080I60   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 30; break;
        case VO_OUTPUT_1080P24   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 24; break;
        case VO_OUTPUT_1080P25   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 25; break;
        case VO_OUTPUT_1080P30   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 30; break;
        case VO_OUTPUT_1080P50   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 50; break;
        case VO_OUTPUT_1080P60   :   *pu32W = 1920; *pu32H = 1080; *pu32Frm = 60; break;
        case VO_OUTPUT_1024x600_60:  *pu32W = 1024; *pu32H = 600;  *pu32Frm = 60; break;
        case VO_OUTPUT_1024x768_60:  *pu32W = 1024; *pu32H = 768;  *pu32Frm = 60; break;
        case VO_OUTPUT_1280x1024_60: *pu32W = 1280; *pu32H = 1024; *pu32Frm = 60; break;
        case VO_OUTPUT_1366x768_60:  *pu32W = 1366; *pu32H = 768;  *pu32Frm = 60; break;
        case VO_OUTPUT_1440x900_60:  *pu32W = 1440; *pu32H = 900;  *pu32Frm = 60; break;
        case VO_OUTPUT_1280x800_60:  *pu32W = 1280; *pu32H = 800;  *pu32Frm = 60; break;
        case VO_OUTPUT_1600x1200_60: *pu32W = 1600; *pu32H = 1200; *pu32Frm = 60; break;
        case VO_OUTPUT_1680x1050_60: *pu32W = 1680; *pu32H = 1050; *pu32Frm = 60; break;
        case VO_OUTPUT_1920x1200_60: *pu32W = 1920; *pu32H = 1200; *pu32Frm = 60; break;
        case VO_OUTPUT_2560x1440_30: *pu32W = 2560; *pu32H = 1440; *pu32Frm = 30; break;
        case VO_OUTPUT_2560x1440_60: *pu32W = 2560; *pu32H = 1440; *pu32Frm = 60; break;
        case VO_OUTPUT_3840x2160_30: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 30; break;
        case VO_OUTPUT_3840x2160_60: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 60; break;
        case VO_OUTPUT_4096x2160_30: *pu32W = 4096; *pu32H = 2160; *pu32Frm = 30; break;
        case VO_OUTPUT_640x480_60:   *pu32W = 640;  *pu32H = 480;  *pu32Frm = 60; break;
        case VO_OUTPUT_240x320_60:   *pu32W = 240;  *pu32H = 320;  *pu32Frm = 60; break;
        case VO_OUTPUT_USER    :     *pu32W = 240;  *pu32H = 320;  *pu32Frm = 30; break;
        default:
            SAMPLE_PRT("vo enIntfSync not support!\n");
            return FY_FAILURE;
    }
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_GetChnRect(VO_CHN voChn, SAMPLE_VO_MODE_E enMode, SIZE_S *pstSize, RECT_S* pstRect)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;
    FY_U32 u32Square = 0;
    FY_U32 u32CellH = 0;
    FY_U32 u32CellW = 0;
    FY_S32 s32XO = 0, s32YO = 0;

    if (NULL == pstSize || NULL == pstRect) {
        s32Ret = FY_ERR_VO_NULL_PTR;
        goto EXIT;
    }

    switch (enMode) {
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
        case VO_MODE_1B_5S:
            u32WndNum = 6;
            u32Square = 3;
            break;
        case VO_MODE_1B_7S:
            u32WndNum = 8;
            u32Square = 4;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            u32Square = 2;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            u32Square = 5;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            u32Square = 6;
            break;
        case VO_MODE_49MUX:
            u32WndNum = 49;
            u32Square = 7;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
            u32Square = 8;
            break;
        default:
            break;
    }

    if (VO_MODE_1L_1R == enMode) {
        u32CellW = ALIGN_BACK(pstSize->u32Width/u32Square,  16);
        u32CellH = ALIGN_BACK(pstSize->u32Height, 2);
        s32XO = ALIGN_BACK((pstSize->u32Width - u32CellW*u32Square)/2, 2);
        s32YO = ALIGN_BACK((pstSize->u32Height - u32CellH)/2, 2);
    } else {
        u32CellW = ALIGN_BACK(pstSize->u32Width/u32Square,  16);
        u32CellH = ALIGN_BACK(pstSize->u32Height/u32Square, 2);
        s32XO = ALIGN_BACK((pstSize->u32Width - u32CellW*u32Square)/2, 2);
        s32YO = ALIGN_BACK((pstSize->u32Height - u32CellH*u32Square)/2, 2);
    }


    if (voChn < u32WndNum) {
        if (VO_MODE_1L_1R == enMode) {
            if (0 == voChn) {
                pstRect->s32X = s32XO;
                pstRect->s32Y = s32YO;
                pstRect->u32Width = u32CellW;
                pstRect->u32Height = u32CellH;
            } else if (1 == voChn){
                pstRect->s32X = s32XO + u32CellW;
                pstRect->s32Y = s32YO;
                pstRect->u32Width = u32CellW;
                pstRect->u32Height = u32CellH;
            }
        } else {
            if (VO_MODE_1B_5S == enMode || VO_MODE_1B_7S == enMode) {
                if (0 == voChn) {
                    pstRect->s32X = s32XO;
                    pstRect->s32Y = s32YO;
                } else if (voChn >= u32WndNum - u32Square) {
                    pstRect->s32X = u32CellW * (voChn + u32Square - u32WndNum) + s32XO;
                    pstRect->s32Y = u32CellH * (u32Square - 1) + s32YO;
                } else {
                    pstRect->s32X = u32CellW * (u32Square - 1) + s32XO;
                    pstRect->s32Y = u32CellH * (voChn - 1) + s32YO;
                }
            } else {
                pstRect->s32X = u32CellW * (voChn % u32Square) + s32XO;
                pstRect->s32Y = u32CellH * (voChn / u32Square) + s32YO;
            }

            if ((0 == voChn) && (VO_MODE_1B_5S == enMode || VO_MODE_1B_7S == enMode)) {
                pstRect->u32Width = u32CellW * (u32Square -1);
                pstRect->u32Height = u32CellH * (u32Square -1);
            } else {
                pstRect->u32Width = u32CellW;
                pstRect->u32Height = u32CellH;
            }
        }
    }


EXIT:

    return s32Ret;
}
/******************************************************************************
* function : Set system memory location
******************************************************************************/
FY_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, FY_CHAR *pcMmzName)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stMppChnVO;

    /* config vo dev */
    stMppChnVO.enModId  = FY_ID_VOU;
    stMppChnVO.s32DevId = VoDev;
    stMppChnVO.s32ChnId = 0;
    s32Ret = FY_MPI_SYS_SetMemConf(&stMppChnVO, pcMmzName);
    if (s32Ret)
    {
        SAMPLE_PRT("FY_MPI_SYS_SetMemConf ERR !\n");
        return FY_FAILURE;
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr)
{
    FY_S32 s32Ret = FY_SUCCESS;

    s32Ret = FY_MPI_VO_SetPubAttr(VoDev, pstPubAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_Enable(VoDev);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev)
{
    FY_S32 s32Ret = FY_SUCCESS;

    s32Ret = FY_MPI_VO_Disable(VoDev);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("VoDev:%d,failed with %#x!\n", VoDev,s32Ret);
        return FY_FAILURE;
    }
    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
    FY_S32 s32Ret = FY_SUCCESS;
    s32Ret = FY_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StartCompressedLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_COMPRESS_ATTR_S stCompressAttr = {.bSupportCompress = FY_TRUE,};

    s32Ret = FY_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_SetVideoLayerCompressAttr(VoLayer, &stCompressAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StartSingleLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr, FY_BOOL isSingle)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_PART_MODE_E enPartMode = VO_PART_MODE_SINGLE;
    VO_COMPRESS_ATTR_S stCompressAttr = {.bSupportCompress = FY_TRUE,};

    s32Ret = FY_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_SetVideoLayerPartitionMode(VoLayer, enPartMode);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_SetVideoLayerCompressAttr(VoLayer, &stCompressAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    printf("===============SAMPLE_COMM_VO_StartSingleLayer=================== ok \n");
    return s32Ret;
}


FY_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer)
{
    FY_S32 s32Ret = FY_SUCCESS;

    s32Ret = FY_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }
    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StartChnOne(VO_LAYER VoLayer, VO_CHN voChn, SAMPLE_VO_MODE_E enMode)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_S32 s32ChnFrmRate;

    switch (enMode)
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
        case VO_MODE_1B_5S:
            u32WndNum = 6;
            break;
        case VO_MODE_1B_7S:
            u32WndNum = 8;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            break;
        case VO_MODE_49MUX:
            u32WndNum = 49;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    if (stLayerAttr.u32DispFrmRt <= 0)
    {
        s32ChnFrmRate = 30;
    }
    else if (stLayerAttr.u32DispFrmRt > 30)
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt / 2;
    }
    else
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt;
    }

    if (voChn < u32WndNum)
    {
        s32Ret = SAMPLE_COMM_VO_GetChnRect(voChn, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }

        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = FY_FALSE;

        s32Ret = FY_MPI_VO_SetChnAttr(VoLayer, voChn, &stChnAttr);
        if (s32Ret != FY_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__, s32Ret);
            return FY_FAILURE;
        }

        s32Ret = FY_MPI_VO_SetChnFrameRate(VoLayer, voChn, s32ChnFrmRate);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }

        s32Ret = FY_MPI_VO_EnableChn(VoLayer, voChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_StopChnOne(VO_LAYER VoLayer, VO_CHN voChn, SAMPLE_VO_MODE_E enMode)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;

    switch (enMode)
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
        case VO_MODE_1B_5S:
            u32WndNum = 6;
            break;
        case VO_MODE_1B_7S:
            u32WndNum = 8;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            break;
        case VO_MODE_49MUX:
            u32WndNum = 49;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
    }

    if (voChn < u32WndNum)
    {
        s32Ret = FY_MPI_VO_DisableChn(VoLayer, voChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }
    return s32Ret;
}


FY_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    FY_S32 i;
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    FY_S32 s32ChnFrmRate;

    switch (enMode)
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
        case VO_MODE_49MUX:
            u32WndNum = 49;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
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

            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    if (stLayerAttr.u32DispFrmRt <= 0)
    {
        s32ChnFrmRate = 30;
    }
    else if (stLayerAttr.u32DispFrmRt > 30)
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt / 2;
    }
    else
    {
        s32ChnFrmRate = stLayerAttr.u32DispFrmRt;
    }

    FY_MPI_VO_SetAttrBegin(VoLayer);

    for (i=0; i<u32WndNum; i++)
    {
        s32Ret = SAMPLE_COMM_VO_GetChnRect(i, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }

        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = FY_FALSE;

        s32Ret = FY_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != FY_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__, s32Ret);
            return FY_FAILURE;
        }

        s32Ret = FY_MPI_VO_SetChnFrameRate(VoLayer, i, s32ChnFrmRate);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
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

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    FY_S32 i;
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 u32WndNum = 0;

    switch (enMode)
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
        case VO_MODE_1B_5S:
            u32WndNum = 6;
            break;
        case VO_MODE_1B_7S:
            u32WndNum = 8;
            break;
        case VO_MODE_1L_1R:
            u32WndNum = 2;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            break;
        case VO_MODE_49MUX:
            u32WndNum = 49;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
    }

    for (i=0; i<u32WndNum; i++)
    {
        s32Ret = FY_MPI_VO_DisableChn(VoLayer, i);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }
    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StartWbc(VO_WBC VoWbc,const VO_WBC_ATTR_S *pstWbcAttr)
{
    FY_S32 s32Ret = FY_SUCCESS;

    s32Ret = FY_MPI_VO_SetWbcDepth(VoWbc, 6);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_SetWbcAttr(VoWbc, pstWbcAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VO_EnableWbc(VoWbc);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_S32 SAMPLE_COMM_VO_StopWbc(VO_WBC VoWbc)
{
    FY_S32 s32Ret = FY_SUCCESS;

    s32Ret = FY_MPI_VO_DisableWbc(VoWbc);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }
    return s32Ret;
}

FY_S32 SAMPLE_COMM_Vpss_BindVpss(VPSS_GRP VpssDestGrp,VO_CHN VoChn,VPSS_GRP VpssSrcGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssSrcGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_VPSS;
    stDestChn.s32DevId = VpssDestGrp;
    stDestChn.s32ChnId = VoChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}


FY_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}
FY_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }
    return s32Ret;
}

FY_S32 SAMPLE_COMM_WBC_BindVo(VO_WBC VoWbc,VO_WBC_SOURCE_S *pstWbcSource)
{
    FY_S32 s32Ret = FY_SUCCESS;
    s32Ret = FY_MPI_VO_SetWbcSource(VoWbc, pstWbcSource);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }
    return s32Ret;
}
FY_S32 SAMPLE_COMM_VO_BindVoWbc(VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = FY_ID_VOU;
    stSrcChn.s32DevId   = VoWbcDev;
    stSrcChn.s32ChnId   = 0;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

FY_S32 SAMPLE_COMM_VO_UnBindVoWbc(VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stDestChn;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;

    return FY_MPI_SYS_UnBind(NULL, &stDestChn);
}

FY_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = FY_ID_VIU;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = ViChn;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

FY_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stDestChn;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;

    return FY_MPI_SYS_UnBind(NULL, &stDestChn);
}

#if (HDMI_SUPPORT == 1)
static FY_VOID SAMPLE_COMM_VO_HdmiConvertSync(VO_INTF_SYNC_E enIntfSync,
    FY_HDMI_VIDEO_FMT_E *penVideoFmt)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_PAL;
            break;
        case VO_OUTPUT_NTSC:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_NTSC;
            break;
        case VO_OUTPUT_1080P24:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080P_24;
            break;
        case VO_OUTPUT_1080P25:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080P_25;
            break;
        case VO_OUTPUT_1080P30:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080P_30;
            break;
        case VO_OUTPUT_720P50:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_720P_50;
            break;
        case VO_OUTPUT_720P60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_720P_60;
            break;
        case VO_OUTPUT_1080I50:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080i_50;
            break;
        case VO_OUTPUT_1080I60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080i_60;
            break;
        case VO_OUTPUT_1080P50:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080P_50;
            break;
        case VO_OUTPUT_1080P60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_1080P_60;
            break;
        case VO_OUTPUT_576P50:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_576P_50;
            break;
        case VO_OUTPUT_480P60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_480P_60;
            break;
        case VO_OUTPUT_800x600_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_800X600_60;
            break;
        case VO_OUTPUT_1024x768_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_1024X768_60;
            break;
        case VO_OUTPUT_1280x1024_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_1280X1024_60;
            break;
        case VO_OUTPUT_1366x768_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_1366X768_60;
            break;
        case VO_OUTPUT_1440x900_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_1440X900_60;
            break;
        case VO_OUTPUT_1280x800_60:
            *penVideoFmt = FY_HDMI_VIDEO_FMT_VESA_1280X800_60;
            break;

        default :
            SAMPLE_PRT("Unkonw VO_INTF_SYNC_E value!\n");
            break;
    }

    return;
}

static FY_S32 SAMPLE_COMM_HdmiHotPlugEvent(FY_VOID *pPrivateData)
{
    FY_S32          			s32Ret = FY_SUCCESS;
    HDMI_CALLBACK_ARGS_S     	*pArgs  = (HDMI_CALLBACK_ARGS_S*)pPrivateData;
    FY_HDMI_ID_E      	 		hHdmi   =  pArgs->enHdmi;
    FY_HDMI_ATTR_S             	stHdmiAttr;
    FY_HDMI_SINK_CAPABILITY_S   stSinkCap;
    FY_HDMI_INFOFRAME_S         stHdmiInfoFrame;

    printf("\n --- hotplug event handling ---\n");

    s32Ret = FY_MPI_HDMI_GetAttr(hHdmi, &stHdmiAttr);
    if(FY_SUCCESS != s32Ret)
    {
        printf("FY_MPI_HDMI_GetAttr ERROR \n");
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_HDMI_GetSinkCapability(hHdmi, &stSinkCap);
    if(FY_SUCCESS != s32Ret)
    {
        printf("FY_MPI_HDMI_GetSinkCapability ERROR \n");
        return FY_FAILURE;
    }

	if (FY_FALSE == stSinkCap.bConnected )
	{
		printf("stSinkCap.bConnected is FY_FALSE!\n");
		return FY_FAILURE;
	}

    stHdmiAttr.enVidOutMode = FY_HDMI_VIDEO_MODE_YCBCR444;

    if(FY_TRUE == stSinkCap.bSupportHdmi)
    {
        stHdmiAttr.bEnableHdmi = FY_TRUE;
        if(FY_TRUE != stSinkCap.bSupportYCbCr)
        {
            stHdmiAttr.enVidOutMode = FY_HDMI_VIDEO_MODE_RGB444;
        }
    }
    else
    {
        stHdmiAttr.enVidOutMode = FY_HDMI_VIDEO_MODE_RGB444;
        //read real edid ok && sink not support hdmi,then we run in dvi mode
        stHdmiAttr.bEnableHdmi = FY_FALSE;
    }

    if(FY_TRUE == stHdmiAttr.bEnableHdmi)
    {
        stHdmiAttr.bEnableVideo = FY_TRUE;

        stHdmiAttr.enDeepColorMode = FY_HDMI_DEEP_COLOR_OFF;
        stHdmiAttr.bxvYCCMode = FY_FALSE;

        stHdmiAttr.bEnableAudio = FY_FALSE;
        stHdmiAttr.enSoundIntf = FY_HDMI_SND_INTERFACE_I2S;
        stHdmiAttr.bIsMultiChannel = FY_FALSE;

        stHdmiAttr.enBitDepth = FY_HDMI_BIT_DEPTH_16;

        stHdmiAttr.bEnableAviInfoFrame = FY_TRUE;
        stHdmiAttr.bEnableAudInfoFrame = FY_TRUE;
        stHdmiAttr.bEnableSpdInfoFrame = FY_FALSE;
        stHdmiAttr.bEnableMpegInfoFrame = FY_FALSE;

        stHdmiAttr.bDebugFlag = FY_FALSE;
        stHdmiAttr.bHDCPEnable = FY_FALSE;

        stHdmiAttr.b3DEnable = FY_FALSE;

        FY_MPI_HDMI_GetInfoFrame(hHdmi, FY_INFOFRAME_TYPE_AVI, &stHdmiInfoFrame);
        stHdmiInfoFrame.unInforUnit.stAVIInfoFrame.enOutputType = stHdmiAttr.enVidOutMode;
        FY_MPI_HDMI_SetInfoFrame(hHdmi, &stHdmiInfoFrame);
    }
    else
    {
        stHdmiAttr.bEnableVideo = FY_TRUE;

        stHdmiAttr.enVidOutMode = FY_HDMI_VIDEO_MODE_RGB444;
        stHdmiAttr.enDeepColorMode = FY_HDMI_DEEP_COLOR_OFF;

        stHdmiAttr.bEnableAudio = FY_FALSE;

        stHdmiAttr.bEnableAviInfoFrame = FY_FALSE;
        stHdmiAttr.bEnableAudInfoFrame = FY_FALSE;
    }

	if (	pArgs->eForceFmt >= FY_HDMI_VIDEO_FMT_1080P_60
		&& 	pArgs->eForceFmt < FY_HDMI_VIDEO_FMT_BUTT
		&& 	stSinkCap.bVideoFmtSupported[pArgs->eForceFmt]	)
	{
		printf("set force format=%d\n",pArgs->eForceFmt);
		stHdmiAttr.enVideoFmt = pArgs->eForceFmt;
	}
	else
	{
		printf("not support expected format=%d, we set native format=%d\n",pArgs->eForceFmt,stSinkCap.enNativeVideoFormat);
		stHdmiAttr.enVideoFmt = stSinkCap.enNativeVideoFormat;
	}

    s32Ret = FY_MPI_HDMI_SetAttr(hHdmi, &stHdmiAttr);
    if(FY_SUCCESS != s32Ret)
    {
        printf("FY_MPI_HDMI_SetAttr ERROR \n");
        return FY_FAILURE;
    }

    /* FY_MPI_HDMI_SetAttr must before FY_MPI_HDMI_Start! */
    s32Ret = FY_MPI_HDMI_Start(hHdmi);
    if(FY_SUCCESS != s32Ret)
    {
        printf("FY_MPI_HDMI_Start ERROR \n");
        return FY_FAILURE;
    }

    return s32Ret;

}

static FY_S32 SAMPLE_COMM_HdmiUnPlugEvent(FY_VOID *pPrivateData)
{
	FY_S32          s32Ret = FY_SUCCESS;
    HDMI_CALLBACK_ARGS_S     *pArgs  = (HDMI_CALLBACK_ARGS_S*)pPrivateData;
    FY_HDMI_ID_E    hHdmi   =  pArgs->enHdmi;

    printf("\n --- UnPlug event handling. --- \n");

    s32Ret = FY_MPI_HDMI_Stop(hHdmi);
	if(FY_SUCCESS != s32Ret)
    {
        printf("FY_MPI_HDMI_Stop ERROR \n");
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_VOID SAMPLE_COMM_HdmiCallbackEvent(FY_HDMI_EVENT_TYPE_E event, FY_VOID *pPrivateData)
{
	printf("\ncallback fun HDMI_EventProc handling event:%d(0x%02x)\n",event,event);
    switch ( event )
    {
        case FY_HDMI_EVENT_HOTPLUG:
			printf("[HDMI EVENT]==>FY_HDMI_EVENT_HOTPLUG \n");
            SAMPLE_COMM_HdmiHotPlugEvent(pPrivateData);
            break;
        case FY_HDMI_EVENT_NO_PLUG:
			printf("[HDMI EVENT]==>FY_HDMI_EVENT_NO_PLUG \n");
            SAMPLE_COMM_HdmiUnPlugEvent(pPrivateData);
            break;
        case FY_HDMI_EVENT_EDID_FAIL:
			printf("[HDMI EVENT]==>FY_HDMI_EVENT_EDID_FAIL \n");
            break;
        case FY_HDMI_EVENT_HDCP_FAIL:
			printf("[HDMI EVENT]==>FY_HDMI_EVENT_HDCP_FAIL \n");
            break;
        case FY_HDMI_EVENT_HDCP_SUCCESS:
			printf("[HDMI EVENT]==>FY_HDMI_EVENT_HDCP_SUCCESS \n");
            break;
        case FY_HDMI_EVENT_HDCP_USERSETTING:
            printf("[HDMI EVENT]==>FY_HDMI_EVENT_HDCP_USERSETTING \n");
            break;
        default:
			printf("[HDMI EVENT]==>un-known event:%d\n",event);
            return;
    }
    return;
}

FY_S32 SAMPLE_COMM_VO_HdmiCallbackStart(VO_INTF_SYNC_E enIntfSync, HDMI_CALLBACK_ARGS_S *pstCallbackArgs)
{
    FY_HDMI_VIDEO_FMT_E enVideoFmt;
    FY_HDMI_INIT_PARA_S stHdmiPara;

    SAMPLE_COMM_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);
    pstCallbackArgs->eForceFmt = enVideoFmt;
    pstCallbackArgs->enHdmi = FY_HDMI_ID_0;

    stHdmiPara.enForceMode = FY_HDMI_FORCE_HDMI;
    stHdmiPara.pCallBackArgs = (void *)pstCallbackArgs;
    stHdmiPara.pfnHdmiEventCallback = SAMPLE_COMM_HdmiCallbackEvent;

    FY_MPI_HDMI_Init(&stHdmiPara);

    FY_MPI_HDMI_Open(FY_HDMI_ID_0);

    printf("HDMI start success.\n");
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync)
{
    FY_HDMI_ATTR_S      stAttr;
    FY_HDMI_VIDEO_FMT_E enVideoFmt;
    FY_HDMI_INIT_PARA_S stHdmiPara;

    SAMPLE_COMM_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);

    stHdmiPara.pfnHdmiEventCallback = NULL;
    stHdmiPara.pCallBackArgs = NULL;
    stHdmiPara.enForceMode = FY_HDMI_FORCE_HDMI;
    FY_MPI_HDMI_Init(&stHdmiPara);

    FY_MPI_HDMI_Open(FY_HDMI_ID_0);

    FY_MPI_HDMI_GetAttr(FY_HDMI_ID_0, &stAttr);

    stAttr.bEnableHdmi = FY_TRUE;

    stAttr.bEnableVideo = FY_TRUE;
    stAttr.enVideoFmt = enVideoFmt;

    stAttr.enVidOutMode = FY_HDMI_VIDEO_MODE_YCBCR444;
    stAttr.enDeepColorMode = FY_HDMI_DEEP_COLOR_OFF;
    stAttr.bxvYCCMode = FY_FALSE;

    stAttr.bEnableAudio = FY_FALSE;
    stAttr.enSoundIntf = FY_HDMI_SND_INTERFACE_I2S;
    stAttr.bIsMultiChannel = FY_FALSE;

    stAttr.enBitDepth = FY_HDMI_BIT_DEPTH_16;

    stAttr.bEnableAviInfoFrame = FY_TRUE;
    stAttr.bEnableAudInfoFrame = FY_TRUE;
    stAttr.bEnableSpdInfoFrame = FY_FALSE;
    stAttr.bEnableMpegInfoFrame = FY_FALSE;

    stAttr.bDebugFlag = FY_FALSE;
    stAttr.bHDCPEnable = FY_FALSE;

    stAttr.b3DEnable = FY_FALSE;

    FY_MPI_HDMI_SetAttr(FY_HDMI_ID_0, &stAttr);

    FY_MPI_HDMI_Start(FY_HDMI_ID_0);

    printf("HDMI start success.\n");
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VO_HdmiStop(FY_VOID)
{
    FY_MPI_HDMI_Stop(FY_HDMI_ID_0);
    FY_MPI_HDMI_Close(FY_HDMI_ID_0);
    FY_MPI_HDMI_DeInit();

    return FY_SUCCESS;
}
#endif

