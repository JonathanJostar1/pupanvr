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
#include <sys/prctl.h>

#include "sample_comm.h"

SAMPLE_VENC_GETSTRM_PARA_S gs_stGetJpegePara;
pthread_t gs_JpegePid;

FY_BOOL g_bAgingJpegeLightOn = FY_TRUE;
FY_BOOL g_bAgingJpegTest = FY_FALSE;

static char *g_pTestParaString[]={
  "u32ChanId",
  "enType",
  "u32PicWidth",
  "u32PicHeight",
  "u32Profile",
  "enRcMode",
  "u32Gop",
  "u32StatTime",
  "u32SrcFrmRate",
  "fr32DstFrmRate",
  "u32BitRate",
  "u32MaxBitRate",
  "u32MaxQp",
  "u32MinQp",
  "bIntraRefreshEn",
  "u32Base",
  "u32Enhance",
  "bEnablePred",
  "bEnableSmart",
  "u32PixFmt",
  "u32Rotate",
  "InputFile",
  "u32VpuGrpId",
  "u32VpuChnId",
  "u32RcvNum"
};

void TEST_VENC_PrintTestParams(VENC_TEST_PARA_S *pTestParams)
{
  printf("===========veu channel[%d] start=========\n", pTestParams->u32ChanId);
  printf("\ttype          = %s\n", (pTestParams->enType == PT_H264)?"h264":"h265");
  printf("\tw/h           = %u/%u\n", pTestParams->stPicSize.u32Width, pTestParams->stPicSize.u32Height);
  printf("\tprofile       = %u  # 0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3];\n", pTestParams->u32Profile);
  printf("\trc mode       = %u  # 0:CBR, 1:VBR, 2:AVBR, 3:QVBR, 4:FIXQP, 5:QPMAP\n", pTestParams->enRcMode);
  printf("\tgop num       = %u\n", pTestParams->u32Gop);
  printf("\tstat time     = %u\n", pTestParams->u32StatTime);
  printf("\tsrc/dst rate  = %u/%u\n", pTestParams->u32SrcFrmRate, pTestParams->fr32DstFrmRate);
  printf("\tavg bitrate   = %u\n", pTestParams->u32BitRate);
  printf("\tmax bitrate   = %u\n", pTestParams->u32MaxBitRate);
  printf("\tmax qp        = %u\n", pTestParams->u32MaxQp);
  printf("\tmin qp        = %u\n", pTestParams->u32MinQp);
  printf("\tintra refresh = %u\n", pTestParams->bIntraRefreshEn);
  printf("\tbase          = %d\n", pTestParams->u32Base);
  printf("\tenhance       = %d\n", pTestParams->u32Enhance);
  printf("\tpreden        = %d\n", pTestParams->bEnablePred);
  printf("\tsmart        = %d\n",  pTestParams->bEnableSmart);
  printf("\tInputFile     = %s\n", pTestParams->pInputfile);
  printf("\tvpu grpid/chnid  = %d/%d\n", pTestParams->u32VpuGrpId, pTestParams->u32VpuChanId);
  printf("===========veu channel[%d] end=========\n\n", pTestParams->u32ChanId);
}

FY_S32 TEST_VENC_GetTestParams(Config *pstTestCfg, VENC_TEST_PARA_S *pstTestParams, FY_U32 u32TestItem)
{
    //load test params
    FY_U32 u32ChanNum=0;
    char section[256];
    int chn;
    FY_BOOL ret;
    int i, u32SecLen;
    FY_U32 *puParams;

    sprintf(section, "venc_test_%d", u32TestItem);

    if(!cnf_has_section(pstTestCfg,section))
    {
        printf("No section [%s] in config\n", section);
        return FY_FAILURE;
    }


    cnf_get_value(pstTestCfg, section, "u32ChanNum");

    u32ChanNum = pstTestCfg->re_int;
    printf("\nTest u32ChanNum [%d]\n", u32ChanNum);
    if(u32ChanNum <1 )
    {
        SAMPLE_PRT("u32ChanNum = %d error\n", u32ChanNum);
        return -1;
    }

    u32SecLen = strlen(section);

    puParams = (FY_U32 *)pstTestParams;
    pstTestParams->pInputfile = 0;

    for(chn=0; chn < u32ChanNum; chn++)
    {
        sprintf(section+u32SecLen, "_%d", chn);

        for(i=0; i<(sizeof(g_pTestParaString)/4); i++)
        {
            ret = cnf_get_value(pstTestCfg, section, g_pTestParaString[i]);
            if(!ret)
            {
                printf("get value %s failed\n", g_pTestParaString[i]);
                *puParams = 0;
                //return -1;
            }
            else
            {
                if(!strcmp(g_pTestParaString[i], "InputFile"))
                {
                    if(*puParams == 0)
                    *puParams = (FY_U32)malloc(MAX_FILE_NAME_LEN);

                    sprintf((void *)*puParams, "%s", pstTestCfg->re_string);
                }
                else if(!strcmp(g_pTestParaString[i], "enType"))
                {
                    if(pstTestCfg->re_int == 0)
                        *puParams = PT_H264;
                    else if(pstTestCfg->re_int == 1)
                        *puParams = PT_H265;
                    else if(pstTestCfg->re_int == 2)
                        *puParams = PT_JPEG;
                }
                else
                {
                    *puParams = pstTestCfg->re_int;
                }
            }
            puParams++;

        }
        puParams += MAX_FILE_NAME_LEN/4;

        TEST_VENC_PrintTestParams(&pstTestParams[chn]);
    }

    return u32ChanNum;
}

/******************************************************************************
* function : venc bind vpss
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_BindSrc(VENC_CHN VeChn,MOD_ID_E enBindSrc, FY_S32 u32SrcGrp,FY_S32 u32SrcChan)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = enBindSrc;
    stSrcChn.s32DevId = u32SrcGrp;
    stSrcChn.s32ChnId = u32SrcChan;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}


/******************************************************************************
* function : venc unbind vpss
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_UnBindSrc(VENC_CHN VeChn,MOD_ID_E enBindSrc, FY_S32 u32SrcGrp,FY_S32 u32SrcChan)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = enBindSrc;
    stSrcChn.s32DevId = u32SrcGrp;
    stSrcChn.s32ChnId = u32SrcChan;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

/******************************************************************************
* function : venc bind vpss
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_BindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}


/******************************************************************************
* function : venc unbind vpss
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_UnBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}
/******************************************************************************
* function : JPEGE bind VGS
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_BindVgs(VENC_CHN VeChn,VGS_CHN VgsChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = 1;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}


/******************************************************************************
* function : JPEGE unbind VGS
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_UnBindVgs(VENC_CHN VeChn,VGS_CHN VgsChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = 1;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_JPEGED;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return s32Ret;
}

FY_S32 SAMPLE_COMM_JPEGE_Start(VENC_CHN s32Chn, VENC_TEST_PARA_S *pTestParams)
{
    FY_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_JPEG_S stJpegAttr;

    stPicSize.u32Width = pTestParams->stPicSize.u32Width;
    stPicSize.u32Height = pTestParams->stPicSize.u32Height;

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVeAttr.enType = PT_JPEG;

    stJpegAttr.u32MaxPicWidth  = stPicSize.u32Width;
    stJpegAttr.u32MaxPicHeight = stPicSize.u32Height;
    stJpegAttr.u32PicWidth  = stPicSize.u32Width;
    stJpegAttr.u32PicHeight = stPicSize.u32Height;
    stJpegAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height;
    stJpegAttr.bByFrame = FY_TRUE;/*get stream mode is field mode  or frame mode*/
    stJpegAttr.bSupportDCF = FY_FALSE;
    memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));

    s32Ret = FY_MPI_VENC_CreateChn(VENC_CHNID(s32Chn), &stVencChnAttr);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_CreateChn [%d] faild with %#x!\n",\
                s32Chn, s32Ret);
        return s32Ret;
    }

#if 1
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    s32Ret = FY_MPI_VENC_StartRecvPic(VENC_CHNID(s32Chn));
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return FY_FAILURE;
    }
#endif

    return FY_SUCCESS;

}

FY_S32 SAMPLE_COMM_JPEGE_Stop(VENC_CHN VencChn)
{
    FY_S32 s32Ret;

    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = FY_MPI_VENC_StopRecvPic(VENC_CHNID(VencChn));
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        return FY_FAILURE;
    }

    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = FY_MPI_VENC_DestroyChn(VENC_CHNID(VencChn));
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        return FY_FAILURE;
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_JPEGE_SaveStream(char *path, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    FILE *pFile;
    char *pIdxStart;
    int idx=0;
    int i;

    pIdxStart = strrchr(path, '_');
    idx = atoi(pIdxStart+1);

    sprintf(pIdxStart+1, "%d.jpg", ++idx);

    pFile = fopen(path, "wb");
    if (pFile == NULL)
    {
        SAMPLE_PRT("open file err\n");
        return FY_FAILURE;
    }


    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, pFile);
    }

    // check jpeg EOF
    pstData = &pstStream->pstPack[pstStream->u32PackCount-1];
    if(*(pstData->pu8Addr + pstData->u32Len - 2) != 0xFF ||
           *(pstData->pu8Addr + pstData->u32Len - 1) != 0xD9)
        SAMPLE_PRT("==========jpege error, end data = [0x%x, 0x%x]\n",
        *(pstData->pu8Addr + pstData->u32Len - 2),
        *(pstData->pu8Addr + pstData->u32Len - 1));

    fflush(pFile);
    fclose(pFile);

    return FY_SUCCESS;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_SnapProcess(FY_U32 u32ChanNum, VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32SnapNum, MOD_ID_E enBindSrc, thread_state_e *bStop)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    FY_S32 s32Ret;
//    VENC_RECV_PIC_PARAM_S stRecvParam;
    int i;
    VENC_TEST_PARA_S *pstChnParams;
    FY_S32 VencFd[JPEGE_MAX_CHN_NUM];
    FY_S32 maxfd = 0;
    FY_S32 u32EncPics = 0;
    FY_S32 u32ChnSnapCnt[JPEGE_MAX_CHN_NUM]={0};

    /******************************************
     step 1:  Jpege Chn bind to Vpss Chn
    ******************************************/
    pstChnParams = pastJpegeTestPara;
    for (i = 0; i < u32ChanNum; i++)
    {
        s32Ret = SAMPLE_COMM_JPEGE_BindSrc(i, enBindSrc, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
            
//      SAMPLE_PRT("bind veu(%d) to %s(%d-%d)\n", (enBindSrc == FY_ID_VGS)?"vgs":"vpss", 
//                i, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("bind jpeg(%d) to %s(%d-%d) failed!\n", i, (enBindSrc == FY_ID_VGS)?"vgs":"vpss", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
            return FY_FAILURE;
        }
        /******************************************
         step 2:  Start Recv Venc Pictures
        ******************************************/
        s32Ret = FY_MPI_VENC_StartRecvPic(VENC_CHNID(i));
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_StartRecvPic ch%d faild with%#x!\n", i, s32Ret);
            goto EXIT;
        }

        /* decide the stream file name, and open file to save stream */
        if(pstChnParams->pOutfile == NULL)
        {
            SAMPLE_PRT("encode output file is NULL!\n");
            goto EXIT;
        }

       /* Set Venc Fd. */
        VencFd[i] = FY_MPI_VENC_GetFd(VENC_CHNID(i));
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("FY_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
            goto EXIT;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        pstChnParams++;
    }

    memset(&stStream, 0, sizeof(stStream));
    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * 10);
    if (NULL == stStream.pstPack)
    {
        SAMPLE_PRT("malloc memory failed!\n");
        goto EXIT;
    }
    
    pstChnParams = pastJpegeTestPara;
    /******************************************
     step 3:  recv picture
    ******************************************/
    FY_U64 u64StartPts, u64EndPts;
    for(u32EncPics=0; u32EncPics<u32SnapNum*u32ChanNum; )
    {
        if(*bStop == TEST_THREAD_STOP)
        {            
            SAMPLE_PRT("---snap thread stop!\n");
            break;
        }
        
        FD_ZERO(&read_fds);
        for (i = 0; i < u32ChanNum; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        FY_MPI_SYS_GetCurPts(&u64StartPts);

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd+1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("snap select failed!\n");
            goto EXIT;
        }
        else if (0 == s32Ret)
        {
            SAMPLE_PRT("snap time out!\n");
            FY_MPI_SYS_GetCurPts(&u64EndPts);
            goto EXIT;
        }
        else
        {
            pstChnParams = pastJpegeTestPara;
            for (i = 0; i < u32ChanNum; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    s32Ret = FY_MPI_VENC_Query(VENC_CHNID(i), &stStat);
                    if (s32Ret != FY_SUCCESS)
                    {
                        SAMPLE_PRT("FY_MPI_VENC_Query failed with %#x!\n", s32Ret);
                        goto EXIT;
                    }

                    /*******************************************************
                     suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        return FY_SUCCESS;
                     }
                    *******************************************************/
                    if(0 == stStat.u32CurPacks)
                    {
                          SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                          goto EXIT;
                    }

                    s32Ret = FY_MPI_VENC_GetStream(VENC_CHNID(i), &stStream, 0);
                    if (FY_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("FY_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                        goto EXIT;
                    }

                    if(u32ChnSnapCnt[i] < u32SnapNum)
                    {                                                    
                        s32Ret = SAMPLE_COMM_JPEGE_SaveStream(pstChnParams->pOutfile, &stStream);
                        if (FY_SUCCESS != s32Ret)
                        {
                            SAMPLE_PRT("SAMPLE_COMM_JPEGE_SaveStream failed with %#x!\n", s32Ret);
                            s32Ret = FY_MPI_VENC_ReleaseStream(VENC_CHNID(i), &stStream);
                            if (s32Ret)
                            {
                                SAMPLE_PRT("FY_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
                            }
                            goto EXIT;
                        }
                        u32EncPics++;
                        u32ChnSnapCnt[i]++;
                    }

                    s32Ret = FY_MPI_VENC_ReleaseStream(VENC_CHNID(i), &stStream);
                    if (s32Ret)
                    {
                        SAMPLE_PRT("FY_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
                        goto EXIT;
                    }

                }
                pstChnParams++;
            }
        }
    }
    /******************************************
     step 4:  unbind
    ******************************************/
EXIT:
    if (NULL != stStream.pstPack)
    {
        free(stStream.pstPack);
        stStream.pstPack = NULL;
    }
    
    pstChnParams = pastJpegeTestPara;
    for (i = 0; i < u32ChanNum; i++)
    {
        s32Ret = SAMPLE_COMM_JPEGE_UnBindSrc(i, enBindSrc, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
        
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("unbind veu(%d) with %s(%d-%d) failed!\n", i, (enBindSrc == FY_ID_VGS)?"vgs":"vpss", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
            return FY_FAILURE;
        }

        FY_MPI_VENC_StopRecvPic(VENC_CHNID(i));
        
        s32Ret = FY_MPI_VENC_ResetChn(VENC_CHNID(i));
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_ResetChn faild with%#x!\n", s32Ret);
        }
        // set output file idx to 0
        char *pIdxStart;

        pIdxStart = strrchr(pstChnParams->pOutfile, '_');

        sprintf(pIdxStart+1, "0.jpg");

        pstChnParams++;
    }

    return FY_SUCCESS;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
FY_VOID* SAMPLE_COMM_JPEGE_GetJpegStreamProc(FY_VOID* p)
{
    FY_S32 i, flen=0;
    FY_S32 s32ChnTotal;
    SAMPLE_VENC_GETSTRM_PARA_S* pstPara;
    FILE* pFile[JPEGE_MAX_CHN_NUM];
    FILE *pFileCmp = NULL;
    VENC_STREAM_S stStream;
    FY_S32 s32Ret;
    FY_U32 u32RecNum=0;
    FY_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    FY_S32 VencFd[JPEGE_MAX_CHN_NUM];
    //FY_S32 u32RcvCnt=0;
    FY_U8 *pAgeCompData=NULL;
    VENC_CHN_STAT_S stStat;

   	prctl(PR_SET_NAME, "GetJpegStream");

    pstPara = (SAMPLE_VENC_GETSTRM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal > JPEGE_MAX_CHN_NUM)
    {
        SAMPLE_PRT("encode channel counter exceed max channel number %d\n", JPEGE_MAX_CHN_NUM);
        return NULL;
    }

    for (i = 0; i < s32ChnTotal; i++)
    {

        if(!g_bAgingJpegTest)
        {
            /* decide the stream file name, and open file to save stream */
            if(pstPara->pTestPara[i].pOutfile == NULL)
            {
                SAMPLE_PRT("encode output file is NULL!\n");
                return NULL;
            }

            pFile[i] = fopen(pstPara->pTestPara[i].pOutfile, "wb");
            if (!pFile[i])
            {
                SAMPLE_PRT("open output file[%s] failed!\n", pstPara->pTestPara[i].pOutfile);
                return NULL;
            }
            chmod(pstPara->pTestPara[i].pOutfile, 0777);
        }

       /* Set Venc Fd. */
        VencFd[i] = FY_MPI_VENC_GetFd(VENC_CHNID(i));
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }

    memset(&stStream, 0, sizeof(stStream));
    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * 10);
    if (NULL == stStream.pstPack)
    {
        SAMPLE_PRT("malloc memory failed!\n");
        return NULL;
    }

    /******************************************
     step 2:  Start to get streams of each channel.
     ******************************************/
    while (FY_TRUE == pstPara->bThreadStart)
    {
//        usleep(30000);

        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 5;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get jpege stream time out, exit thread\n");
            if(g_bAgingJpegTest)
            {
                g_bAgingJpegeLightOn = FY_FALSE;
                //SAMPLE_PRT("no jpeg encoding data============\n");
            }
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    s32Ret = FY_MPI_VENC_Query(VENC_CHNID(i), &stStat);
                    if (s32Ret != FY_SUCCESS)
                    {
                        SAMPLE_PRT("FY_MPI_VENC_Query failed with %#x!\n", s32Ret);
                        goto EXIT;
                    }

                    if(0 == stStat.u32CurPacks)
                    {
                          SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                          return FY_SUCCESS;
                    }

			        s32Ret = FY_MPI_VENC_GetStream(VENC_CHNID(pstPara->pTestPara[i].u32ChanId), &stStream, 0);
			        if (FY_SUCCESS != s32Ret)
			        {
			            continue;
			        }


			        /*******************************************************
			         step 2.5 : save frame to file
			        *******************************************************/
//			        SAMPLE_PRT("receive jpeg stream cnt %d\n", u32RecNum++);
                    if(!g_bAgingJpegTest)
                    {
    			        s32Ret = SAMPLE_COMM_JPEGE_SaveStream(pstPara->pTestPara[i].pOutfile, &stStream);
                        if (FY_SUCCESS != s32Ret)
                        {
           			        SAMPLE_PRT("save stream failed!\n");
    			            s32Ret = FY_MPI_VENC_ReleaseStream(VENC_CHNID(pstPara->pTestPara[i].u32ChanId), &stStream);
           			        break;
                        }
                    }
                    else
                    {
                        // compare data
                        if(pAgeCompData == NULL)
                        {
                            pFileCmp = fopen("jpege.jpg", "rb");
                            if (!pFileCmp)
                            {
                                SAMPLE_PRT("open compare file[%s] failed!\n", "jpege.jpg");
                                return NULL;
                            }
                            fseek(pFileCmp,0L,SEEK_END);
                            flen = ftell(pFileCmp);

                            pAgeCompData = (FY_U8 *)malloc(flen+1);
                            if(pAgeCompData ==NULL)
                            {
                                fclose(pFileCmp);
                                return NULL;
                            }
                            fseek(pFileCmp,0,SEEK_SET);
                            fread(pAgeCompData,flen,1,pFileCmp);

                        }

                        VENC_PACK_S*  pstData;

                        pstData = &stStream.pstPack[0];
                        if(pstData->u32Len-pstData->u32Offset != flen || memcmp(pAgeCompData, pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset))
                        {
                            g_bAgingJpegeLightOn = FY_FALSE;
                            SAMPLE_PRT("check jpeg encoding data error============, len=%d-%d\n", pstData->u32Len-pstData->u32Offset, flen);
#if 0
                            FILE *fp;
                            fp = fopen("jpege_err.jpg", "wb");
                            if (!fp)
                            {
                                SAMPLE_PRT("open save file[%s] failed!\n", "jpege_err.jpg");
                                return NULL;
                            }

                            fwrite(stStream.pu8Addr,stStream.u32Len,1,fp);
                            fflush(fp);
                            fclose(fp);
#endif
                        }
                        else
                        {
                            g_bAgingJpegeLightOn = FY_TRUE;
                            //SAMPLE_PRT("got jpeg encoding data============\n");
                        }
                    }

			        /*******************************************************
			         step 2.6 : release stream
			         *******************************************************/
			        s32Ret = FY_MPI_VENC_ReleaseStream(VENC_CHNID(pstPara->pTestPara[i].u32ChanId), &stStream);
			        if (FY_SUCCESS != s32Ret)
			        {
			          break;
			        }

			        /*******************************************************
			         step 2.7 : free pack nodes
			        *******************************************************/
			        if(0 != pstPara->pTestPara[0].u32RcvNum)
			        {
			            if(u32RecNum >= pstPara->pTestPara[0].u32RcvNum)
			            {
			                SAMPLE_PRT("======(%d) frames got, arrive to specified number (%d)======\n", u32RecNum,pstPara->pTestPara[0].u32RcvNum);
			                goto EXIT;
                        }
                    }
                }
            }
        }
    }

EXIT:
    if(!g_bAgingJpegTest)
    {
        /*******************************************************
        * step 3 : close save-file
        *******************************************************/
        for (i = 0; i < s32ChnTotal; i++)
        {
            fclose(pFile[i]);
            chmod(pstPara->pTestPara[i].pOutfile, 0777);
        }
    }

    if (NULL != stStream.pstPack)
    {
        free(stStream.pstPack);
        stStream.pstPack = NULL;
    }
    
    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_StartGetStream(FY_S32 s32Cnt, VENC_TEST_PARA_S *pTestParams)
{
    gs_stGetJpegePara.bThreadStart = FY_TRUE;
    gs_stGetJpegePara.s32Cnt = s32Cnt;
    gs_stGetJpegePara.pTestPara = pTestParams;

    return pthread_create(&gs_JpegePid, 0, SAMPLE_COMM_JPEGE_GetJpegStreamProc, (FY_VOID*)&gs_stGetJpegePara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
FY_S32 SAMPLE_COMM_JPEGE_StopGetStream()
{
    if (FY_TRUE == gs_stGetJpegePara.bThreadStart)
    {
        gs_stGetJpegePara.bThreadStart = FY_FALSE;
        if (gs_JpegePid)
        {
            pthread_join(gs_JpegePid, 0);
            gs_JpegePid = 0;
        }
    }
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_JPEGE_AgingTest(FY_BOOL bAgingTest)
{
    g_bAgingJpegTest = bAgingTest;
    return FY_SUCCESS;
}
