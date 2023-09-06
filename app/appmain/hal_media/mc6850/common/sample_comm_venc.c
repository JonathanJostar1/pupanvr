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

SAMPLE_VENC_GETSTRM_PARA_S gs_stGetStrmPara;
SAMPLE_VENC_GETSTRM_PARA_S gs_stGetJpegePara;
pthread_t gs_VencPid, gs_VencInsertDataPid;
pthread_t gs_JpegePid;

extern char *g_pTestParaString[];

#define TEST_VENC_MAX_CHN_NUM 64

#if (VENC_MAX_CHN_NUM > 0)
#undef TEST_VENC_MAX_CHN_NUM
#define TEST_VENC_MAX_CHN_NUM VENC_MAX_CHN_NUM
#endif

static FY_U32 g_aFileLen[TEST_VENC_MAX_CHN_NUM]={0};
FY_BOOL g_bAgingVencLightOn = FY_TRUE;
FY_BOOL g_bAgingVencTest = FY_FALSE;

extern void TEST_VENC_PrintTestParams(VENC_TEST_PARA_S *pTestParams);
extern FY_S32 TEST_VENC_GetTestParams(Config *pstTestCfg, VENC_TEST_PARA_S *pstTestParams, FY_U32 u32TestItem);

/******************************************************************************
* function : Set venc memory location
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_MemConfig(FY_VOID)
{
    FY_S32 i = 0;
    FY_S32 s32Ret;

    FY_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVENC;

    /* group, venc max chn is 64*/
    for(i=0;i<64;i++)
    {
        stMppChnVENC.enModId = FY_ID_VENC;
        stMppChnVENC.s32DevId = 0;
        stMppChnVENC.s32ChnId = i;

        pcMmzName = NULL;

        /*venc*/
        s32Ret = FY_MPI_SYS_SetMemConf(&stMppChnVENC,pcMmzName);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_SYS_SetMemConf with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }

    return FY_SUCCESS;
}

/******************************************************************************
* function : venc bind vpss
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_BindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_BOOL bEnableBgm)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    if(bEnableBgm)
    {
		// 1. vpss->bgm
		SAMPLE_PRT("### bind vpu and bgm\n");
        stSrcChn.enModId = FY_ID_VPSS;
        stSrcChn.s32DevId = VpssGrp;
        stSrcChn.s32ChnId = VpssChn;

        stDestChn.enModId = FY_ID_BGM;
        stDestChn.s32DevId = 0;
        stDestChn.s32ChnId = VeChn;

        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != FY_SUCCESS)
        {
                SAMPLE_PRT("bind vpu and bgm failed with %#x! VeChn=%d\n", s32Ret, VeChn);
            return FY_FAILURE;
        }

		// 2. venc->bgmsw
		SAMPLE_PRT("### bind veu and bgmsw\n");
        stSrcChn.enModId = FY_ID_VENC;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = VeChn;

        stDestChn.enModId = FY_ID_BGMSW;
        stDestChn.s32DevId = 0;
        stDestChn.s32ChnId = VeChn;

        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("bind venc and bgmsw failed with %#x! VeChn=%d\n", s32Ret, VeChn);
            return FY_FAILURE;
        }
    }

    return s32Ret;
}



FY_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_BOOL bEnableBgm)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = FY_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    if(bEnableBgm)
    {
		// 1. vpss->bgm
        stSrcChn.enModId = FY_ID_VPSS;
        stSrcChn.s32DevId = VpssGrp;
        stSrcChn.s32ChnId = VpssChn;

        stDestChn.enModId = FY_ID_BGM;
        stDestChn.s32DevId = 0;
        stDestChn.s32ChnId = VeChn;

        s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }

		// 2. venc->vou
        stSrcChn.enModId = FY_ID_VENC;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = VeChn;

        stDestChn.enModId = FY_ID_BGMSW;
        stDestChn.s32DevId = 0;
        stDestChn.s32ChnId = VeChn;

        s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }

    return s32Ret;
}

char *TEST_VENC_GetRCString(SAMPLE_RC_E enRCMode)
{
  switch(enRCMode)
  {
    case SAMPLE_RC_CBR:
       return "cbr";

    case SAMPLE_RC_VBR:
      return "vbr";

    case SAMPLE_RC_AVBR:
      return "avbr";

    case SAMPLE_RC_QVBR:
      return "qvbr";

    case SAMPLE_RC_FIXQP:
      return "fixqp";

    case SAMPLE_RC_QPMAP:
      return "qpmap";

    default:
      break;
  }

  return NULL;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_SaveStream(VENC_CHN VencChn, VENC_TEST_PARA_S *pTestPara, FILE* pFd, VENC_STREAM_S* pstStream, FY_U32 u32SeqNum, SAMPLE_FILETYPE_E enFileType)
{
    VENC_REC_STREAM_INFO_S stRecStrInfo;
    int i;

    if(pFd == NULL)
        return FY_SUCCESS;

    if(enFileType == SAMPLE_FT_MP4)
    {
        stRecStrInfo.u32ChnId       = VencChn;
        stRecStrInfo.enStreamType   = pTestPara->enType;
        stRecStrInfo.u32Width       = pTestPara->stPicSize.u32Width;
        stRecStrInfo.u32Height      = pTestPara->stPicSize.u32Height;
        stRecStrInfo.bEnableSmart   = pTestPara->bEnableSmart;
        stRecStrInfo.u32SeqNum      = u32SeqNum;
        stRecStrInfo.u32FrameRate   = pTestPara->fr32DstFrmRate;
        VENC_RECFILE_VidoeEs2Ps(&stRecStrInfo, pFd, pstStream);
    }
    else // raw stream
    {
        for (i = 0; i < pstStream->u32PackCount; i++)
        {
            fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
            pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
            fflush(pFd);
        }
    }

    return FY_SUCCESS;
}

FY_S32 TEST_VENC_ConstrChanAttr(VENC_CHN VencChn, VENC_TEST_PARA_S *pTestParams, VENC_CHN_ATTR_S *pstVencChnAttr)
{
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_CBR_S    stH264Cbr;
    VENC_ATTR_H264_VBR_S    stH264Vbr;
	VENC_ATTR_H264_AVBR_S    stH264AVbr;
    VENC_ATTR_H264_FIXQP_S  stH264FixQp;
    VENC_ATTR_H265_S        stH265Attr;
    VENC_ATTR_H265_CBR_S    stH265Cbr;
    VENC_ATTR_H265_VBR_S    stH265Vbr;
    VENC_ATTR_H265_AVBR_S    stH265AVbr;
    VENC_ATTR_H265_FIXQP_S  stH265FixQp;
#if 0
	VENC_ATTR_H264_QVBR_S    stH264QVbr;
	VENC_ATTR_H264_QPMAP_S  stH264QpMap;
	VENC_ATTR_H265_QVBR_S    stH265QVbr;
	VENC_ATTR_H265_QPMAP_S  stH265QpMap;
#endif

    pstVencChnAttr->stVeAttr.enType = pTestParams->enType;
    pstVencChnAttr->stRcAttr.pRcAttr = NULL;
    pstVencChnAttr->stVeAttr.bEnableSmart = pTestParams->bEnableSmart;
    pstVencChnAttr->stVeAttr.u32SkipMode = pTestParams->u32Base;//u32Mode;

    switch (pTestParams->enType)
    {
        case PT_H264:
        {
            stH264Attr.u32MaxPicWidth = pTestParams->stPicSize.u32Width;
            stH264Attr.u32MaxPicHeight = pTestParams->stPicSize.u32Height;
            stH264Attr.u32PicWidth = pTestParams->stPicSize.u32Width;/*the picture width*/
            stH264Attr.u32PicHeight = pTestParams->stPicSize.u32Height;/*the picture height*/
            stH264Attr.u32BufSize  = pTestParams->stPicSize.u32Width * pTestParams->stPicSize.u32Height * 0.75;/*stream buffer size*/
            stH264Attr.u32Profile  = pTestParams->u32Profile;/*0: baseline; 1:MP; 2:HP;  3:svc_t */
            stH264Attr.bByFrame = FY_TRUE;/*get stream mode is slice mode or frame mode?*/
            memcpy(&pstVencChnAttr->stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
            if (SAMPLE_RC_CBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop            = pTestParams->u32Gop;
                stH264Cbr.u32StatTime       = pTestParams->u32StatTime; /* stream rate statics time(unit: s) */
                stH264Cbr.u32SrcFrmRate      = pTestParams->u32SrcFrmRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate; /* target frame rate */
                stH264Cbr.u32BitRate = pTestParams->u32BitRate;
                stH264Cbr.u32FluctuateLevel = 0; /* average bit rate */

                stH264Cbr.u32InitQP = pTestParams->u32MinQp;
                stH264Cbr.u32MaxRatePercent = 200;
                stH264Cbr.u32IFrmMaxBits = 0;
                stH264Cbr.s32IPQpDelta = 0;
                stH264Cbr.s32IBitProp = 5;
                stH264Cbr.s32PBitProp = 1;

                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop = pTestParams->u32Gop;
                stH264FixQp.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH264FixQp.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH264FixQp.u32IQp = 20;
                stH264FixQp.u32PQp = 20;
                stH264FixQp.u32BQp = 20;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop = pTestParams->u32Gop;
                stH264Vbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH264Vbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH264Vbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH264Vbr.u32MinQp = pTestParams->u32MinQp;
                stH264Vbr.u32MaxQp = pTestParams->u32MaxQp;
                stH264Vbr.u32MaxBitRate = pTestParams->u32MaxBitRate;

                stH264Vbr.u32MinIQp = pTestParams->u32MinQp;
                stH264Vbr.u32MaxIQp = pTestParams->u32MaxQp;
                stH264Vbr.u32InitQp = pTestParams->u32MinQp;
                stH264Vbr.u32MaxRatePercent = 200;
                stH264Vbr.u32IFrmMaxBits = 0;
                stH264Vbr.s32IPQpDelta = 0;
                stH264Vbr.s32IBitProp = 5;
                stH264Vbr.s32PBitProp = 1;
                stH264Vbr.u32FluctuateLevel = 0; /* average bit rate */

                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop = pTestParams->u32Gop;
                stH264AVbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH264AVbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH264AVbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH264AVbr.u32MaxBitRate = pTestParams->u32MaxBitRate;
                stH264AVbr.u32MinQp = pTestParams->u32MinQp;
                stH264AVbr.u32MaxQp = pTestParams->u32MaxQp;
                stH264AVbr.u32MinIQp = pTestParams->u32MinQp;
                stH264AVbr.u32MaxIQp = pTestParams->u32MaxQp;
                stH264AVbr.u32InitQp= pTestParams->u32MinQp;
                stH264AVbr.u32MaxRatePercent = 200;
                stH264AVbr.u32IFrmMaxBits = 0;
                stH264AVbr.s32IPQpDelta = 3;
                stH264AVbr.s32IBitProp = 5;
                stH264AVbr.s32PBitProp = 1;
                stH264AVbr.u32FluctuateLevel = 0;
                stH264AVbr.u32StillRatePercent = 30;
                stH264AVbr.u32MaxStillQp = 25;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264AVbr, &stH264AVbr, sizeof(VENC_ATTR_H264_AVBR_S));
            }
#if 0
            else if (SAMPLE_RC_QVBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
                stH264QVbr.u32Gop = pTestParams->u32Gop;
                stH264QVbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH264QVbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH264QVbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH264QVbr.u32TargetBitRate = pTestParams->u32MaxBitRate;

                //added v0.5
                stH264QVbr.u32InitQp = 0; //?
                stH264QVbr.u32MaxRatePercent = 0; //?
                stH264QVbr.u32IFrmMaxBits = 0;
                stH264QVbr.s32IPQpDelta = 0;
                stH264QVbr.s32IBitProp = 0;
                stH264QVbr.s32PBitProp = 0;

                stH264QVbr.u32FluctuateLevel = 0;
                stH264QVbr.s32BitPercentUL = 0;
                stH264QVbr.s32BitPercentLL = 0;
                stH264QVbr.s32PsnrFluctuateUL = 0;
                stH264QVbr.s32PsnrFluctuateLL = 0;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264QVbr, &stH264QVbr, sizeof(VENC_ATTR_H264_QVBR_S));
            }
            else if (SAMPLE_RC_QPMAP == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop = pTestParams->u32Gop;
                stH264QpMap.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH264QpMap.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH264QpMap.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH264QpMap.enQpMapMode = 0;
                stH264QpMap.bQpMapAbsQp = FY_TRUE;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH264QpMap, &stH264QpMap, sizeof(VENC_ATTR_H264_QPMAP_S));
            }
#endif
            else
            {
                SAMPLE_PRT("H264 enRcMode (%d) not support!\n", pTestParams->enRcMode);
                return FY_FAILURE;
            }
        }
        break;
        case PT_H265:
        {
            stH265Attr.u32MaxPicWidth = pTestParams->stPicSize.u32Width;
            stH265Attr.u32MaxPicHeight = pTestParams->stPicSize.u32Height;
            stH265Attr.u32PicWidth = pTestParams->stPicSize.u32Width;/*the picture width*/
            stH265Attr.u32PicHeight = pTestParams->stPicSize.u32Height;/*the picture height*/
            stH265Attr.u32BufSize  = pTestParams->stPicSize.u32Width * pTestParams->stPicSize.u32Height * 0.75;/*stream buffer size*/
		    stH265Attr.u32Profile  = pTestParams->u32Profile;
            stH265Attr.bByFrame = FY_TRUE;/*get stream mode is slice mode or frame mode?*/
            memcpy(&pstVencChnAttr->stVeAttr.stAttrH265e, &stH265Attr, sizeof(VENC_ATTR_H265_S));
            if (SAMPLE_RC_CBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = pTestParams->u32Gop;
                stH265Cbr.u32StatTime       = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrmRate      = pTestParams->u32SrcFrmRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate; /* target frame rate */
                stH265Cbr.u32BitRate = pTestParams->u32BitRate;

                stH265Cbr.u32FluctuateLevel = 0; /* average bit rate */
                stH265Cbr.u32InitQP = pTestParams->u32MinQp;
                stH265Cbr.u32MaxRatePercent = 200;
                stH265Cbr.u32IFrmMaxBits = 0;
                stH265Cbr.s32IPQpDelta = 0;
                stH265Cbr.s32IBitProp = 5;
                stH265Cbr.s32PBitProp = 1;

                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop            = pTestParams->u32Gop;
                stH265FixQp.u32SrcFrmRate      = pTestParams->u32SrcFrmRate; /* input (vi) frame rate */
                stH265FixQp.fr32DstFrmRate = pTestParams->fr32DstFrmRate; /* target frame rate */
                stH265FixQp.u32IQp = 20;
                stH265FixQp.u32PQp = 23;
                stH265FixQp.u32BQp = 25;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop = pTestParams->u32Gop;
                stH265Vbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH265Vbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH265Vbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH265Vbr.u32MinQp = pTestParams->u32MinQp;
                stH265Vbr.u32MaxQp = pTestParams->u32MaxQp;
                stH265Vbr.u32MaxBitRate = pTestParams->u32MaxBitRate;

				stH265Vbr.u32MinIQp = pTestParams->u32MinQp;
				stH265Vbr.u32MaxIQp = pTestParams->u32MaxQp;
				stH265Vbr.u32InitQp = pTestParams->u32MinQp;
				stH265Vbr.u32MaxRatePercent = 200;
				stH265Vbr.u32IFrmMaxBits = 0;
				stH265Vbr.s32IPQpDelta = 0;
				stH265Vbr.s32IBitProp = 5;
				stH265Vbr.s32PBitProp = 1;
				stH265Vbr.u32FluctuateLevel = 0; /* average bit rate */

                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop = pTestParams->u32Gop;
                stH265AVbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH265AVbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH265AVbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH265AVbr.u32MaxBitRate = pTestParams->u32MaxBitRate;

                stH265AVbr.u32MinQp = pTestParams->u32MinQp;
                stH265AVbr.u32MaxQp = pTestParams->u32MaxQp;
                stH265AVbr.u32MinIQp = pTestParams->u32MinQp;
                stH265AVbr.u32MaxIQp = pTestParams->u32MaxQp;
                stH265AVbr.u32InitQp= pTestParams->u32MinQp;
                stH265AVbr.u32MaxRatePercent = 200;
                stH265AVbr.u32IFrmMaxBits = 0;
                stH265AVbr.s32IPQpDelta = 3;
                stH265AVbr.s32IBitProp = 5;
                stH265AVbr.s32PBitProp = 1;
                stH265AVbr.u32FluctuateLevel = 0;
                stH265AVbr.u32StillRatePercent = 30;
                stH265AVbr.u32MaxStillQp = 25;

                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265AVbr, &stH265AVbr, sizeof(VENC_ATTR_H265_AVBR_S));
            }
#if 0
            else if (SAMPLE_RC_QVBR == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
                stH265QVbr.u32Gop = pTestParams->u32Gop;
                stH265QVbr.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH265QVbr.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH265QVbr.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH265QVbr.u32TargetBitRate = pTestParams->u32BitRate;
                stH265QVbr.u32InitQp = 0;
                stH265QVbr.u32MaxRatePercent = 0;
                stH265QVbr.u32IFrmMaxBits = 0;
                stH265QVbr.s32IPQpDelta = 0;
                stH265QVbr.s32IBitProp = 0;
                stH265QVbr.s32PBitProp = 0;
                stH265QVbr.u32FluctuateLevel = 0;
                stH265QVbr.s32BitPercentUL = 0;
                stH265QVbr.s32BitPercentLL = 0;
                stH265QVbr.s32PsnrFluctuateUL = 0;
                stH265QVbr.s32PsnrFluctuateLL = 0;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265QVbr, &stH265QVbr, sizeof(VENC_ATTR_H265_QVBR_S));
            }
            else if (SAMPLE_RC_QPMAP == pTestParams->enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop = pTestParams->u32Gop;
                stH265QpMap.u32StatTime  = pTestParams->u32StatTime; /* stream rate statics time(s) */
                stH265QpMap.u32SrcFrmRate = pTestParams->u32SrcFrmRate;
                stH265QpMap.fr32DstFrmRate = pTestParams->fr32DstFrmRate;
                stH265QpMap.enQpMapMode = 0;
                stH265QpMap.bQpMapAbsQp = FY_TRUE;
                memcpy(&pstVencChnAttr->stRcAttr.stAttrH265QpMap, &stH265QpMap, sizeof(VENC_ATTR_H265_QPMAP_S));
            }
#endif
            else
            {
                SAMPLE_PRT("H265 enRcMode (%d) not support!\n", pTestParams->enRcMode);
                return FY_FAILURE;
            }
        }
        break;
        default:
            SAMPLE_PRT("enType (%d) not support!\n", pTestParams->enType);
            return FY_ERR_VENC_NOT_SUPPORT;
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VENC_Init(VENC_PARAM_MOD_EXT_S *pstModParamExt)
{
    VENC_PARAM_MOD_EXT_S stModParamExt;

    if(pstModParamExt == NULL)
    {  // default mode: dynamic buffer mode
        memset(&stModParamExt, 0, sizeof(stModParamExt));
        pstModParamExt = &stModParamExt;
#if 1 //dynamic stream buffer mode
        pstModParamExt->bOnlyH265                 = FY_FALSE;
        pstModParamExt->enStrmBufMode             = STRMBUF_MODE_DYNAMIC_BUF;
        pstModParamExt->u32MaxPicWidth            = SAMPLE_MAX_WIDTH;
        pstModParamExt->u32MaxPicHeight           = SAMPLE_MAX_HEIGHT;
#else // static stream buffer mode
        pstModParamExt->bOnlyH265                 = FY_FALSE;
        pstModParamExt->enStrmBufMode             = STRMBUF_MODE_STATIC_BUF;
        pstModParamExt->u32SizeModeCnt            = 2;
        pstModParamExt->stSizeMode[0].u32ChnNum   = 8;
        pstModParamExt->stSizeMode[0].u32Width    = SAMPLE_MAX_MAIN_CHAN_WIDTH;
        pstModParamExt->stSizeMode[0].u32Height   = SAMPLE_MAX_MAIN_CHAN_HEIGHT;
        pstModParamExt->stSizeMode[0].u32BufSize  = 0; // use default size:w*h*0.75
        pstModParamExt->stSizeMode[1].u32ChnNum   = 8;
        pstModParamExt->stSizeMode[1].u32Width    = SAMPLE_MAX_SUB_CHAN_WIDTH;
        pstModParamExt->stSizeMode[1].u32Height   = SAMPLE_MAX_SUB_CHAN_HEIGHT;
        pstModParamExt->stSizeMode[1].u32BufSize  = 0; // use default size:w*h*0.75
        pstModParamExt->u32MaxPicWidth            = SAMPLE_MAX_WIDTH;
        pstModParamExt->u32MaxPicHeight           = SAMPLE_MAX_HEIGHT;
#endif
    }

    return FY_MPI_VENC_SetModParamExt(pstModParamExt);
}

/******************************************************************************
* funciton : Start venc stream mode (h264, mjpeg)
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, VENC_TEST_PARA_S *pTestParams)
{
    FY_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;

    s32Ret = TEST_VENC_ConstrChanAttr(VencChn, pTestParams, &stVencChnAttr);
    if (FY_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    /******************************************
     step 1:  Create Venc Channel
     ******************************************/

    SAMPLE_PRT("FY_MPI_VENC_CreateChn [%d]! ===\n", \
               VencChn);
    s32Ret = FY_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    VENC_CHN_ATTR_S stAttr;
    s32Ret = FY_MPI_VENC_GetChnAttr(VencChn, &stAttr);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = FY_MPI_VENC_SetChnAttr(VencChn, &stAttr);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    VENC_PARAM_INTRA_REFRESH_S stIntraRefresh;
    if(pTestParams->bIntraRefreshEn)
    {
	  SAMPLE_PRT("### test bIntraRefreshEn\n");
      stIntraRefresh.bISliceEnable = 1;
      stIntraRefresh.bRefreshEnable = 1;
      stIntraRefresh.u32RefreshLineNum = 20;
      stIntraRefresh.u32ReqIQp = 15;
      stIntraRefresh.u32RefreshStep = 1;

      s32Ret = FY_MPI_VENC_SetIntraRefresh(VencChn, &stIntraRefresh);
      if (FY_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("FY_MPI_VENC_SetIntraRefresh [%d] faild with %#x! ===\n", \
                     VencChn, s32Ret);
          return s32Ret;
      }
    }
    if(pTestParams->u32Base>0)
    {
      VENC_PARAM_REF_S stVencParamRef;
      stVencParamRef.u32Base = pTestParams->u32Base;
      stVencParamRef.u32Enhance = pTestParams->u32Enhance;
      stVencParamRef.bEnablePred = FY_TRUE;
      stVencParamRef.u32Mode = pTestParams->u32Base; //0: ²»ÌøÖ¡£¬ 1: µ¥²ãÌøÖ¡£¬ 2: Ë«²ãÌøÖ¡
      stVencParamRef.u32SvcM = pTestParams->u32Enhance;
      stVencParamRef.u32SvcN = pTestParams->bEnablePred;

      s32Ret = FY_MPI_VENC_SetRefParam(VencChn, &stVencParamRef);
      if (FY_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("Set ref param failed!\n");
          return FY_FAILURE;
      }
    }

    if(pTestParams->bEnableSmart)
    {
		SAMPLE_PRT("### Smart Enable\n");
        s32Ret = FY_MPI_VENC_EnableAdvSmartP(VencChn, FY_TRUE);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_EnableAdvSmartP faild with%#x! \n", s32Ret);
            return FY_FAILURE;
        }
    }

#if 0
		SAMPLE_PRT("### test supper frame\n");
        VENC_SUPERFRAME_CFG_S stSuperFrmParam;

        stSuperFrmParam.enSuperFrmMode = SUPERFRM_DISCARD;
        stSuperFrmParam.enRcPriority = VENC_RC_PRIORITY_FRAMEBITS_FIRST;
        stSuperFrmParam.u32SuperIFrmBitsThr  = 1000000;
        stSuperFrmParam.u32SuperPFrmBitsThr  = 60000;
        stSuperFrmParam.u32SuperBFrmBitsThr  = 20000;

        s32Ret = FY_MPI_VENC_SetSuperFrameCfg(VencChn, &stSuperFrmParam);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_SetSuperFrameCfg faild with%#x! \n", s32Ret);
            return FY_FAILURE;
        }


        memset(&stSuperFrmParam, 0, sizeof(VENC_SUPERFRAME_CFG_S));
        s32Ret = FY_MPI_VENC_GetSuperFrameCfg(VencChn, &stSuperFrmParam);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_SetSuperFrameCfg faild with%#x! \n", s32Ret);
            return FY_FAILURE;
        }
        printf("get super cfg: mode=%d, ipb=%d-%d\n", stSuperFrmParam.enSuperFrmMode,
          stSuperFrmParam.u32SuperIFrmBitsThr, stSuperFrmParam.u32SuperPFrmBitsThr);

#endif

#if 0
        SAMPLE_PRT("### test h264 vui\n");
        VENC_PARAM_H264_VUI_S stH264Vui;
        memset(&stH264Vui, 0, sizeof(VENC_PARAM_H264_VUI_S));
        stH264Vui.stVuiTimeInfo.timing_info_present_flag = 1;
        stH264Vui.stVuiTimeInfo.num_units_in_tick = 2250;
        stH264Vui.stVuiTimeInfo.time_scale = 90000;

        s32Ret = FY_MPI_VENC_SetH264Vui(VencChn, &stH264Vui);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("FY_MPI_VENC_SetH264Vui faild with%#x! \n", s32Ret);
            return FY_FAILURE;
        }
#endif

#if 0
            SAMPLE_PRT("### test h264 roi\n");
            /*if row layer bitrate control enabled, roi index range is 1~7*/
            VENC_ROI_CFG_S stRoiCfg, stRoiCfgGet;
            memset(&stRoiCfg, 0, sizeof(VENC_ROI_CFG_S));
            stRoiCfg.u32Index = 1;
            stRoiCfg.bEnable = FY_TRUE;
            stRoiCfg.bAbsQp = FY_TRUE;
            stRoiCfg.s32Qp = 10;
            stRoiCfg.stRect.s32X = 0;
            stRoiCfg.stRect.s32Y = 0;
            stRoiCfg.stRect.u32Width = 160;
            stRoiCfg.stRect.u32Height = 80;

            s32Ret = FY_MPI_VENC_SetRoiCfg(VencChn, &stRoiCfg);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("FY_MPI_VENC_SetH264Vui faild with%#x! \n", s32Ret);
                return FY_FAILURE;
            }

            memset(&stRoiCfgGet, 0, sizeof(VENC_ROI_CFG_S));
            s32Ret = FY_MPI_VENC_GetRoiCfg(VencChn, stRoiCfg.u32Index, &stRoiCfgGet);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("FY_MPI_VENC_SetSuperFrameCfg faild with%#x! \n", s32Ret);
                return FY_FAILURE;
            }
            if(memcmp(&stRoiCfg, &stRoiCfgGet, sizeof(VENC_ROI_CFG_S)) !=0)
                printf("test roi cfg api failed, cfg:%d, bEnable:%d, bAbsQp:%d, s32X=%d\n",
                stRoiCfgGet.u32Index,stRoiCfgGet.bEnable, stRoiCfgGet.bAbsQp,  stRoiCfg.stRect.s32X);

#endif

#if 0
    VENC_PARAM_H264_DBLK_S stH264DblkSet;
    VENC_PARAM_H264_DBLK_S stH264DblkGet;

    stH264DblkSet.disable_deblocking_filter_idc = 1;
    stH264DblkSet.slice_alpha_c0_offset_div2 = 5;
    stH264DblkSet.slice_beta_offset_div2 = 5;

    s32Ret = FY_MPI_VENC_SetH264Dblk(VencChn, &stH264DblkSet);
    if (FY_SUCCESS != s32Ret)
    {
      SAMPLE_PRT("FY_MPI_VENC_SetH264Dblk [%d] faild with %#x! ===\n", \
                 VencChn, s32Ret);
      return FY_FAILURE;
    }

    s32Ret = FY_MPI_VENC_GetH264Dblk(VencChn, &stH264DblkGet);
    if (FY_SUCCESS != s32Ret)
    {
      SAMPLE_PRT("FY_MPI_VENC_SetH264Dblk [%d] faild with %#x! ===\n", \
                 VencChn, s32Ret);
      return FY_FAILURE;
    }

    if(0 != memcmp(&stH264DblkSet, &stH264DblkGet, sizeof(VENC_PARAM_H264_DBLK_S)))
    {
      SAMPLE_PRT("\n\t===Set/get H264 DBLOCK test failed\n\n");
      return FY_FAILURE;
    }
    else
      SAMPLE_PRT("\n\t===Set/get H264 DBLOCK test success\n\n");

#endif

    s32Ret = FY_MPI_VENC_StartRecvPic(VencChn);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return FY_FAILURE;
    }
    return FY_SUCCESS;
}


/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn)
{
    FY_S32 s32Ret;

    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = FY_MPI_VENC_StopRecvPic(VencChn);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        //return FY_FAILURE;
    }

    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = FY_MPI_VENC_DestroyChn(VencChn);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("FY_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        return FY_FAILURE;
    }

    return FY_SUCCESS;
}

FY_VOID* SAMPLE_COMM_VENC_InsertUserData(FY_VOID* p)
{
    SAMPLE_VENC_GETSTRM_PARA_S* pstPara;
    FY_S32 s32ChnTotal;
    FY_S32 s32Ret;
    FY_U8 acUserData[TEST_VENC_MAX_CHN_NUM][32];
    int i, len=32;
    VENC_CHN_STAT_S stStat;

    pstPara = (SAMPLE_VENC_GETSTRM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    // initial user data
    for(i=0; i<s32ChnTotal; i++)
    {
        memset(acUserData[i], i+0x5a, len);
    }

    while (FY_TRUE == pstPara->bThreadStart)
    {
        for (i = 0; i < s32ChnTotal; i++)
        {
            s32Ret = FY_MPI_VENC_Query(pstPara->pTestPara[i].u32ChanId, &stStat);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("FY_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                continue;
            }

#if 0
            s32Ret = FY_MPI_VENC_InsertUserData(pstPara->pTestPara[i].u32ChanId,
            acUserData[pstPara->pTestPara[i].u32ChanId], len);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("insert ch%d user data failed!\n", pstPara->pTestPara[i].u32ChanId);
                continue;
            }
#endif
        }

        sleep(5);
    }

    return NULL;
}

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
FY_VOID* SAMPLE_COMM_VENC_GetVencStreamProc(FY_VOID* p)
{
    FY_S32 i,j;
    FY_S32 s32ChnTotal;
    SAMPLE_VENC_GETSTRM_PARA_S* pstPara;
    FILE* pFile[TEST_VENC_MAX_CHN_NUM];
    VENC_STREAM_S stStream;
    FY_S32 s32Ret;
    FY_U32 u32TotalRecNum=0;
    FY_S32 u32ContinQueryFailCnt=0;
    FY_U32 u32ChnRecNum[TEST_VENC_MAX_CHN_NUM]={0};
    FY_S32 VencFd[TEST_VENC_MAX_CHN_NUM];
    FY_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;

   	prctl(PR_SET_NAME, "GetVeuStream");

    pstPara = (SAMPLE_VENC_GETSTRM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal > TEST_VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("encode channel counter %d exceed max channel number %d\n", s32ChnTotal, TEST_VENC_MAX_CHN_NUM);
        return NULL;
    }

    for (i = 0; i < s32ChnTotal; i++)
    {
            /* decide the stream file name, and open file to save stream */
        if(!g_bAgingVencTest)
        {
                if(pstPara->pTestPara[i].pOutfile == NULL)
                {
                    SAMPLE_PRT("encode output file is NULL!\n");
                    return NULL;
                }

                pFile[i] = fopen(pstPara->pTestPara[i].pOutfile, "wb");
                if (!pFile[i])
                {
                    SAMPLE_PRT("open ch%d output file[%s] failed!\n", i, pstPara->pTestPara[i].pOutfile);
                    return NULL;
                }

        }

       /* Set Venc Fd. */
        VencFd[i] = FY_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                   VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        g_aFileLen[i] = 0;
        chmod(pstPara->pTestPara[i].pOutfile, 0777);
    }


    memset(&stStream, 0, sizeof(stStream));
    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * 10);
    if (NULL == stStream.pstPack)
    {
        SAMPLE_PRT("malloc stream pack failed!\n");
        return NULL;
    }

    /******************************************
     step 2:  Start to get streams of each channel.
     ******************************************/
    while (FY_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }


        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd+1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("venc select failed!\n");
            break;
        }
        else if (0 == s32Ret)
        {
            SAMPLE_PRT("venc select time out!\n");
            continue;
        }
        else
        {

            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    if(g_bAgingVencTest)
                    {
                        if(u32ContinQueryFailCnt>1000)
                        {
                            g_bAgingVencLightOn = FY_FALSE;
                            //SAMPLE_PRT("no stream============\n");
                        }
                    }
                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    VENC_CHN_STAT_S stStat;
                    s32Ret = FY_MPI_VENC_Query(pstPara->pTestPara[i].u32ChanId, &stStat);
                    if (FY_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("FY_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }
                    /*******************************************************
        			step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
        			 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
        			 {						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
        				continue;
        			 }
        			 *******************************************************/

                    if (0 == stStat.u32LeftStreamFrames)
                    {
                        //SAMPLE_PRT("NOTE: chn%d is null!\n", i);
                        u32ContinQueryFailCnt++;
                        continue;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
        	        s32Ret = FY_MPI_VENC_GetStream(pstPara->pTestPara[i].u32ChanId, &stStream, FY_TRUE);
        	        if (FY_SUCCESS != s32Ret)
        	        {
                        continue;
        	        }

                    u32ContinQueryFailCnt = 0;

                    if(g_bAgingVencTest)
                    {
                        g_bAgingVencLightOn = FY_TRUE;
                        //SAMPLE_PRT("got stream============\n");
                    }

                    if(!g_bAgingVencTest)
                    {
        		        /*******************************************************
        		         step 2.5 : save frame to file
        		        *******************************************************/
        //			    SAMPLE_PRT("receive encoded stream cnt %d\n", u32RcvCnt++);
        		        s32Ret = SAMPLE_COMM_VENC_SaveStream(i, &(pstPara->pTestPara[i]), pFile[i], &stStream, u32ChnRecNum[i], pstPara->enFileType);
                        if (FY_SUCCESS != s32Ret)
                        {
           			        SAMPLE_PRT("save stream failed!\n");
        		            s32Ret = FY_MPI_VENC_ReleaseStream(pstPara->pTestPara[i].u32ChanId, &stStream);
           			        break;
                        }
                        for (j = 0; j < stStream.u32PackCount; j++)
                        {
                            g_aFileLen[i] += (stStream.pstPack[j].u32Len - stStream.pstPack[j].u32Offset);
                        }

                        int idx;
                        char *pIdxStart;

                        if(g_aFileLen[i] >= 2*1024*1024*1024LL-1024)
                        {
                            // exceed max size, recreate new output file
                            if(pFile[i])
                                fclose(pFile[i]);

                            pIdxStart = strrchr(pstPara->pTestPara[i].pOutfile, '_');
                            idx = 1;

                            sprintf(pIdxStart+1, "%d.mp4", idx);

                            printf("===========output file exceed 2GB, create new file %s\n", pstPara->pTestPara[i].pOutfile);

                            pFile[i] = fopen(pstPara->pTestPara[i].pOutfile, "wb");
                            if (!pFile[i])
                            {
                                SAMPLE_PRT("open output file[%s] failed!\n", pstPara->pTestPara[i].pOutfile);
        			                s32Ret = FY_MPI_VENC_ReleaseStream(pstPara->pTestPara[i].u32ChanId, &stStream);
            			        break;
                            }

                            g_aFileLen[i] = 0;
                        }
                    }

        	        /*******************************************************
        	         step 2.6 : release stream
        	         *******************************************************/
        	        s32Ret = FY_MPI_VENC_ReleaseStream(pstPara->pTestPara[i].u32ChanId, &stStream);
        	        if (FY_SUCCESS != s32Ret)
        	        {
        	          break;
        	        }

                    u32ChnRecNum[i]++;
        	        u32TotalRecNum++;

        	        if(0 != pstPara->pTestPara[0].u32RcvNum)
        	        {
        	            if(u32TotalRecNum >= pstPara->pTestPara[0].u32RcvNum)
        	            {
        	                SAMPLE_PRT("======(%d) frames got, arrive to specified number (%d)======\n", u32TotalRecNum,pstPara->pTestPara[0].u32RcvNum);
        	                goto EXIT;
                        }
                    }
                }
            }
        }
    }

EXIT:
    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    if(!g_bAgingVencTest)
    {
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
FY_S32 SAMPLE_COMM_VENC_StartGetStream(FY_S32 s32Cnt, VENC_TEST_PARA_S *pTestParams, SAMPLE_FILETYPE_E enFileType)
{
    FY_S32 ret = 0;
    gs_stGetStrmPara.bThreadStart = FY_TRUE;
    gs_stGetStrmPara.s32Cnt = s32Cnt;
    gs_stGetStrmPara.pTestPara = pTestParams;
    gs_stGetStrmPara.enFileType = enFileType;

    ret = pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProc, (FY_VOID*)&gs_stGetStrmPara);
    if(ret != FY_SUCCESS)
    {
        SAMPLE_PRT("==============create thread SAMPLE_COMM_VENC_GetVencStreamProc failed======\n");
        return ret;
    }

    ret = pthread_create(&gs_VencInsertDataPid, 0, SAMPLE_COMM_VENC_InsertUserData, (FY_VOID*)&gs_stGetStrmPara);
    if(ret != FY_SUCCESS)
    {
        SAMPLE_PRT("==============create thread SAMPLE_COMM_VENC_InsertUserData failed======\n");
        return ret;
    }
    return ret;
}


/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
FY_S32 SAMPLE_COMM_VENC_StopGetStream()
{
    if (FY_TRUE == gs_stGetStrmPara.bThreadStart)
    {
        gs_stGetStrmPara.bThreadStart = FY_FALSE;
        if (gs_VencPid)
        {
            pthread_join(gs_VencPid, 0);
            gs_VencPid = 0;
        }

        if(gs_VencInsertDataPid)
        {
            pthread_join(gs_VencInsertDataPid, 0);
            gs_VencInsertDataPid = 0;
        }
    }
    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VENC_AgingTest(FY_BOOL bAgingTest)
{
    g_bAgingVencTest = bAgingTest;
    return FY_SUCCESS;
}

FY_VOID SAMPLE_COMM_VENC_ReadOneFrame( FILE * fp, FY_U8 * pY, FY_U8 * pU, FY_U8 * pV,
                                              FY_U32 width, FY_U32 height, FY_U32 stride, FY_U32 stride2)
{
    FY_U8 * pDst;
    FY_U32 u32Row;

    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        fread( pDst, width, 1, fp );
        pDst += stride;
    }

    pDst = pU;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }

    pDst = pV;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }

}

FY_S32 SAMPLE_COMM_VENC_PlanToSemi(FY_U8 *pY, FY_S32 yStride,
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
    if((pTmpU==FY_NULL)||(pTmpV==FY_NULL))
    {
        printf("malloc buf failed\n");
        return FY_FAILURE;
    }

    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);

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

    free( ptu );
    free( ptv );

    return FY_SUCCESS;
}


