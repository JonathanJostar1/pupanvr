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
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"


/* g_s32VBSource: 0 to module common vb, 1 to private vb, 2 to user vb
   And don't forget to set the value of VBSource file "load3535" */
FY_S32 g_s32VBSource = 0;
//VB_POOL g_ahVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};



FY_VOID	SAMPLE_COMM_VDEC_Sysconf(VB_CONF_S *pstVbConf, SIZE_S *pstSize)
{
	memset(pstVbConf, 0, sizeof(VB_CONF_S));
	pstVbConf->u32MaxPoolCnt = 3;
	pstVbConf->astCommPool[0].u32BlkSize = (pstSize->u32Width *	pstSize->u32Height * 3)>> 1;
	pstVbConf->astCommPool[0].u32BlkCnt	= 1;

	pstVbConf->astCommPool[1].u32BlkSize = ((pstSize->u32Width *	pstSize->u32Height * 3)>> 1)/4;
	pstVbConf->astCommPool[1].u32BlkCnt	= 4;

	pstVbConf->astCommPool[2].u32BlkSize = ((pstSize->u32Width *	pstSize->u32Height * 3)>> 1)/9;
	pstVbConf->astCommPool[2].u32BlkCnt	= 9;
}


FY_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf(VB_CONF_S *pstModVbConf,
    PAYLOAD_TYPE_E enType, SIZE_S *pstSize, FY_S32 s32ChnNum)
{
	int i;
	FY_S32 PicSize;
	FY_S32  fbCnt = 4;
	MPP_CHN_S chn;

	chn.enModId = FY_ID_VDEC;
	chn.s32DevId= 0;

	memset(pstModVbConf, 0,	sizeof(VB_CONF_S));
	pstModVbConf->u32MaxPoolCnt	= 1;

	for(i=0;i<s32ChnNum;i++)
	{
		chn.s32ChnId = i;
		VB_PIC_BLK_SIZE(pstSize->u32Width, pstSize->u32Height, enType, PicSize);
		pstModVbConf->astCommPool[i].u32BlkSize	= PicSize;
		pstModVbConf->astCommPool[i].u32BlkCnt	= fbCnt;
		FY_MPI_SYS_GetMemConf(&chn,pstModVbConf->astCommPool[i].acMmzName);
	}
	pstModVbConf->u32MaxPoolCnt	= s32ChnNum;
}


FY_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf_ext(VB_CONF_S *pstModVbConf,
    PAYLOAD_TYPE_E *enTypes, SIZE_S *pstSize, FY_S32 s32ChnNum,FY_S32  fbCnt)
{
	int i;
	FY_S32 PicSize;
	PAYLOAD_TYPE_E enType;
	MPP_CHN_S chn;

	chn.enModId = FY_ID_VDEC;
	chn.s32DevId= 0;

	memset(pstModVbConf, 0,	sizeof(VB_CONF_S));
	pstModVbConf->u32MaxPoolCnt	= 1;
	for(i=0;i<s32ChnNum;i++)
	{
		chn.s32ChnId = i;
		enType = enTypes[i];
		VB_PIC_BLK_SIZE(pstSize[i].u32Width, pstSize[i].u32Height, enType, PicSize);
		pstModVbConf->astCommPool[i].u32BlkSize	= PicSize;
		pstModVbConf->astCommPool[i].u32BlkCnt	= fbCnt;
		FY_MPI_SYS_GetMemConf(&chn,pstModVbConf->astCommPool[i].acMmzName);
	}
	pstModVbConf->u32MaxPoolCnt	= s32ChnNum;
}

FY_S32	SAMPLE_COMM_VDEC_InitModCommVb(VB_CONF_S *pstModVbConf)
{
    FY_MPI_VB_ExitModCommPool(VB_UID_VDEC);

    if(0 == g_s32VBSource)
    {
        CHECK_RET(FY_MPI_VB_SetModPoolConf(VB_UID_VDEC, pstModVbConf), "FY_MPI_VB_SetModPoolConf");
        CHECK_RET(FY_MPI_VB_InitModCommPool(VB_UID_VDEC), "FY_MPI_VB_InitModCommPool");
    }

    return FY_SUCCESS;
}


FY_VOID	SAMPLE_COMM_VDEC_ChnAttr(FY_S32 s32ChnNum,
	VDEC_CHN_ATTR_S	*pstVdecChnAttr, PAYLOAD_TYPE_E	enTypes[],	SIZE_S *pstSize)
{
	FY_S32 i;
	PAYLOAD_TYPE_E	enType;

    for(i=0; i<s32ChnNum; i++)
    {
		enType = enTypes[i];
        pstVdecChnAttr[i].enType       = enType;
		pstVdecChnAttr[i].u32BufSize   = 4 * pstSize[i].u32Width * pstSize[i].u32Height/5;
        pstVdecChnAttr[i].u32Priority  = 5;
        pstVdecChnAttr[i].u32PicWidth  = pstSize[i].u32Width;
        pstVdecChnAttr[i].u32PicHeight = pstSize[i].u32Height;
        if (PT_H264 == enType || PT_MP4VIDEO == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 2;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 0;
        }
        else if (PT_JPEG == enType || PT_MJPEG == enType)
        {
            pstVdecChnAttr[i].stVdecJpegAttr.enMode = VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecJpegAttr.enJpegFormat = JPG_COLOR_FMT_YCBCR420;
        }
        else if(PT_H265 == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_STREAM;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 4;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 1;
        }
    }
}


FY_VOID	SAMPLE_COMM_VDEC_VoAttr(FY_S32 s32ChnNum, VO_DEV VoDev ,VO_PUB_ATTR_S *pstVoPubAttr, VO_VIDEO_LAYER_ATTR_S *pstVoLayerAttr)
{
    FY_S32 u32Width=0, u32Height=0;

    /*********** set the pub attr of VO ****************/
    if (0 == VoDev)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_720P50;
        pstVoPubAttr->enIntfType = VO_INTF_BT1120 | VO_INTF_VGA;
    }
    else if (1 == VoDev)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_720P50;
        pstVoPubAttr->enIntfType = VO_INTF_VGA;
    }
    else if (VoDev>=2 && VoDev <=3)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_PAL;
        pstVoPubAttr->enIntfType = VO_INTF_CVBS;
    }
    pstVoPubAttr->u32BgColor = VO_BKGRD_BLUE;


    /***************** set the layer attr of VO  *******************/
    if(pstVoPubAttr->enIntfSync == VO_OUTPUT_720P50)
    {
        u32Width  = 1280;
        u32Height = 720;
    }
    else if (pstVoPubAttr->enIntfSync == VO_OUTPUT_PAL)
    {
        u32Width  = 720;
        u32Height = 576;
    }
    pstVoLayerAttr->stDispRect.s32X		  = 0;
    pstVoLayerAttr->stDispRect.s32Y		  = 0;
    pstVoLayerAttr->stDispRect.u32Width	  = u32Width;
    pstVoLayerAttr->stDispRect.u32Height  = u32Height;
    pstVoLayerAttr->stImageSize.u32Width  = u32Width;
    pstVoLayerAttr->stImageSize.u32Height = u32Height;
    pstVoLayerAttr->bDoubleFrame		  = FY_FALSE;
    pstVoLayerAttr->bClusterMode          = FY_FALSE;
    pstVoLayerAttr->u32DispFrmRt		  = 30;
    pstVoLayerAttr->enPixFormat			  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
}


FY_VOID SAMPLE_COMM_VDEC_ThreadParam(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend,
									VDEC_CHN_ATTR_S	*pstVdecChnAttr, char *pStreamFileNames[], FY_U32 **pFramePos, FY_BOOL loop)
{
    int i;

    for(i=0; i<s32ChnNum; i++)
    {
		sprintf(pstVdecSend[i].cFileName, pStreamFileNames[i], i);
        pstVdecSend[i].s32MilliSec     = 1000;
        pstVdecSend[i].s32ChnId        = i;
        pstVdecSend[i].s32IntervalTime = 1;
        pstVdecSend[i].u64PtsInit      = 0;
        pstVdecSend[i].u64PtsIncrease  = 0;
        pstVdecSend[i].eCtrlSinal      = VDEC_CTRL_START;
		pstVdecSend[i].bLoopSend	   = loop;//FY_TRUE;
        pstVdecSend[i].bManuSend       = FY_FALSE;
        pstVdecSend[i].enType          = pstVdecChnAttr[i].enType;
		pstVdecSend[i].s32MinBufSize   = (pstVdecChnAttr[i].u32PicWidth	* pstVdecChnAttr[i].u32PicHeight * 3)>>2;
		if (1)//PT_H264	== pstVdecChnAttr[i].enType	||	PT_MP4VIDEO== pstVdecChnAttr[i].enType)
		{
			pstVdecSend[i].s32StreamMode = pstVdecChnAttr[i].stVdecVideoAttr.enMode;
		}
		else
		{
			pstVdecSend[i].s32StreamMode = VIDEO_MODE_FRAME;
		}
		pstVdecSend[i].send_times =	0;
        if(pFramePos) {
            pstVdecSend[i].pFramePos  = pFramePos[i];
        } else {
            pstVdecSend[i].pFramePos  = NULL;
        }
		pstVdecSend->bFAV = FY_FALSE;
	}
}


FY_VOID SAMPLE_COMM_VDEC_ThreadParam_ext(VdecSendThreadParam *pThreadParam, VdecThreadParam *pstVdecSend)
{
    int i;

    for(i=0; i<pThreadParam->s32ChnNum; i++)
    {
		sprintf(pstVdecSend[i].cFileName,  pThreadParam->pStreamFileNames[i], i);
        pstVdecSend[i].s32MilliSec     = -1;
        pstVdecSend[i].s32ChnId        = i;
        pstVdecSend[i].s32IntervalTime = 1;
        pstVdecSend[i].u64PtsInit      = pThreadParam->ptsinit;
		if(pThreadParam->framerate>0)
        	pstVdecSend[i].u64PtsIncrease  = 1000*1000/pThreadParam->framerate;
		else
			pstVdecSend[i].u64PtsIncrease  = 0;
        pstVdecSend[i].eCtrlSinal      = VDEC_CTRL_START;
		pstVdecSend[i].bLoopSend	   = pThreadParam->loop;
        pstVdecSend[i].bManuSend       = FY_FALSE;
        pstVdecSend[i].enType          = pThreadParam->pstVdecChnAttr[i].enType;
		pstVdecSend[i].s32MinBufSize   = (pThreadParam->pstVdecChnAttr[i].u32PicWidth	* pThreadParam->pstVdecChnAttr[i].u32PicHeight * 3)>>2;
		if (1)//PT_H264	== pstVdecChnAttr[i].enType	||	PT_MP4VIDEO== pstVdecChnAttr[i].enType)
		{
			pstVdecSend[i].s32StreamMode = pThreadParam->pstVdecChnAttr[i].stVdecVideoAttr.enMode;
		}
		else
		{
			pstVdecSend[i].s32StreamMode = VIDEO_MODE_FRAME;
		}
		pstVdecSend[i].send_times =	0;

        if(pThreadParam->pFramePos) {
            pstVdecSend[i].pFramePos  = pThreadParam->pFramePos[i];
        } else {
            pstVdecSend[i].pFramePos  = NULL;
        }
	}
}


static void SAMPLE_COMM_VDEC_SendStream_Seek(FILE *fpStrm, FY_U8 *pu8Buf, VdecThreadParam *pstVdecThreadParam)
{
    VDEC_STREAM_S stStream;
    FY_BOOL bFindStart, bFindEnd;
    FY_S32 s32Ret,  i,  start = 0;
    FY_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FY_U64 u64pts = 0;
    FY_S32 len;
    FY_BOOL sHasReadStream = FY_FALSE;



    u64pts = pstVdecThreadParam->u64PtsInit;

    while (1)
    {
        if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_PAUSE)
        {
            sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
            continue;
        }

        if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_MP4VIDEO) )
        {
            bFindStart = FY_FALSE;
            bFindEnd   = FY_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindStart = FY_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindEnd = FY_TRUE;
                    break;
                }
            }

            s32ReadLen = i;
            if (bFindStart == FY_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == FY_FALSE)
            {
                s32ReadLen = i+4;
            }

        }
        else if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_H264) )
        {
            bFindStart = FY_FALSE;
            bFindEnd   = FY_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                       (
                           ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                           (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                        )
                   )
                {
                    bFindStart = FY_TRUE;
                    i += 8;
                    break;
                }
            }

            for (; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                            (
                                  tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
                                  ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                                  (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                              )
                   )
                {
                    bFindEnd = FY_TRUE;
                    break;
                }
            }

            if(i > 0) s32ReadLen = i;
            if (bFindStart == FY_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == FY_FALSE)
            {
                s32ReadLen = i+8;
            }
        }
		else if	(pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME &&	pstVdecThreadParam->enType == PT_H265)
		{
			FY_BOOL	 bNewPic = FY_FALSE;
			bFindStart = FY_FALSE;
			bFindEnd   = FY_FALSE;
			fseek(fpStrm, s32UsedBytes,	SEEK_SET);
			s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);

			if (s32ReadLen == 0)
			{
				if (pstVdecThreadParam->bLoopSend == FY_TRUE)
				{
					s32UsedBytes = 0;
					fseek(fpStrm, 0, SEEK_SET);
					s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
				}
				else
				{
					break;
				}
			}

			for	(i=0; i<s32ReadLen-6; i++)
			{
				bNewPic	= (pu8Buf[i+0] == 0	&& pu8Buf[i+1] == 0	&& pu8Buf[i+2] == 1
					&&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D)	<= 0x2A) ||	(pu8Buf[i+3]&0x1F) == 0x1)
					&&((pu8Buf[i+5]&0x80) == 0x80));//first	slice segment

				if (bNewPic)
				{
					bFindStart = FY_TRUE;
					i += 4;
					break;
				}
			}

			for	(; i<s32ReadLen-6; i++)
			{
				bNewPic	= (pu8Buf[i+0] == 0	&& pu8Buf[i+1] == 0	&& pu8Buf[i+2] == 1
					&&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D)	<= 0x2A) ||	(pu8Buf[i+3]&0x1F) == 0x1)
					&&((pu8Buf[i+5]&0x80) == 0x80));//first	slice segment

				if (  pu8Buf[i	] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
					&&(((pu8Buf[i+3]&0x7D) == 0x40)	|| ((pu8Buf[i+3]&0x7D) == 0x42)	|| ((pu8Buf[i+3]&0x7D) == 0x44)|| bNewPic)
				   )
				{
					bFindEnd = FY_TRUE;
					break;
				}
			}

			s32ReadLen = i;

			if (bFindStart == FY_FALSE)
			{
				printf("hevc can not find start	code! %d s32ReadLen	0x%x +++++++++++++\n", pstVdecThreadParam->s32ChnId,s32ReadLen);
			}
			else if	(bFindEnd == FY_FALSE)
			{
				s32ReadLen = i+6;
			}

        }
        else if ((pstVdecThreadParam->enType == PT_MJPEG) )
        {
            bFindStart = FY_FALSE;
            bFindEnd   = FY_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }


            for (i=0; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    start = i;
                    bFindStart = FY_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i<s32ReadLen-4; i++)
            {
                if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                {
                     len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                     i += 1 + len;
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    bFindEnd = FY_TRUE;
                    break;
                }
            }
            s32ReadLen = i;
            if (bFindStart == FY_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == FY_FALSE)
            {
                s32ReadLen = i+2;
            }
        }
        else if ((pstVdecThreadParam->enType == PT_JPEG) )
        {
            if (FY_TRUE != sHasReadStream)
            {

                bFindStart = FY_FALSE;
                bFindEnd   = FY_FALSE;

                fseek(fpStrm, s32UsedBytes, SEEK_SET);
                s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                if (s32ReadLen == 0)
                {
                    if (pstVdecThreadParam->bLoopSend)
                    {
                        s32UsedBytes = 0;
                        fseek(fpStrm, 0, SEEK_SET);
                        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                    }
                    else
                    {
                        break;
                    }
                }


                for (i=0; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                    {
                        start = i;
                        bFindStart = FY_TRUE;
                        i = i + 2;
                        break;
                    }
                }

                for (; i<s32ReadLen-4; i++)
                {
                    if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                    {
                         len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                         i += 1 + len;
                    }
                    else
                    {
                        break;
                    }
                }

                for (; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                    {
                        bFindEnd = FY_TRUE;
                        break;
                    }
                }
                s32ReadLen = i;
                if (bFindStart == FY_FALSE)
                {
                    printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
    					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
                }
                else if (bFindEnd == FY_FALSE)
                {
                    s32ReadLen = i+2;
                }
                 sHasReadStream = FY_TRUE;
            }
        }
        else
        {
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }
        }

        stStream.u64PTS  = u64pts;
        stStream.pu8Addr = pu8Buf + start;
        stStream.u32Len  = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? FY_TRUE: FY_FALSE;
        stStream.bEndOfStream = FY_FALSE;

        s32Ret=FY_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        pstVdecThreadParam->cUserCmd = 0;
        if (FY_SUCCESS != s32Ret)
        {
            usleep(100000);
        }
        else
        {
            s32UsedBytes = s32UsedBytes +s32ReadLen + start;
            u64pts += pstVdecThreadParam->u64PtsIncrease;
			pstVdecThreadParam->send_times++;
        }
        usleep(10000);
    }
}


static void SAMPLE_COMM_VDEC_SendStream_Pos(FILE *fpStrm, FY_U8 *pu8Buf, VdecThreadParam *pstVdecThreadParam)
{
    VDEC_STREAM_S stStream;
    FY_S32 s32Ret,  i,  start = 0;
    FY_S32 s32ReadLen = 0;
    FY_U64 u64pts = 0;
    FY_U32 u32Cnt;


    u64pts = pstVdecThreadParam->u64PtsInit;
    if(NULL == pstVdecThreadParam->pFramePos) {
        return;
    }
    i = 0;
    u32Cnt = pstVdecThreadParam->pFramePos[0];

    while (1)
    {
        if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_PAUSE)
        {
            sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
            continue;
        }

        ++i;
        if(i > u32Cnt) {
            if (pstVdecThreadParam->bLoopSend) {
                i = 1;
            } else {
                break;
            }
        }
        start = pstVdecThreadParam->pFramePos[2 * i];
        s32ReadLen = pstVdecThreadParam->pFramePos[2 * i + 1];
        fseek(fpStrm, start, SEEK_SET);
        s32ReadLen = fread(pu8Buf, 1, s32ReadLen, fpStrm);

        stStream.u64PTS  = u64pts;
        stStream.pu8Addr = pu8Buf;
        stStream.u32Len  = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? FY_TRUE: FY_FALSE;
        stStream.bEndOfStream = FY_FALSE;

        s32Ret=FY_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        pstVdecThreadParam->cUserCmd = 0;
        if (FY_SUCCESS != s32Ret)
        {
            usleep(100);
        }
        else
        {
            if(pstVdecThreadParam->u64PtsInit >= 0) {
                u64pts += pstVdecThreadParam->u64PtsIncrease;
            }
			pstVdecThreadParam->send_times++;
        }
        usleep(1000);

    }
}


FY_VOID * SAMPLE_COMM_VDEC_SendStream(FY_VOID *pArgs)
{
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
    FILE *fpStrm=NULL;
    FY_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;


	char  task_name[64];

	sprintf(task_name,"vdec%d_sendstream",pstVdecThreadParam->s32ChnId);

	prctl(PR_SET_NAME, task_name);

    if(pstVdecThreadParam->cFileName != 0)
    {
        fpStrm = fopen(pstVdecThreadParam->cFileName, "rb");
        if(fpStrm == NULL)
        {
            printf("SAMPLE_TEST:can't open file %s in send stream thread:%d\n",pstVdecThreadParam->cFileName, pstVdecThreadParam->s32ChnId);
            return (FY_VOID *)(FY_FAILURE);
        }
    }
    //printf("SAMPLE_TEST:chn %d, stream file:%s, bufsize: %d\n",
    //pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
    if(pu8Buf == NULL)
    {
        printf("SAMPLE_TEST:can't alloc %d in send stream thread:%d\n", pstVdecThreadParam->s32MinBufSize, pstVdecThreadParam->s32ChnId);
        fclose(fpStrm);
        return (FY_VOID *)(FY_FAILURE);
    }
    fflush(stdout);


    if(NULL == pstVdecThreadParam->pFramePos)
        SAMPLE_COMM_VDEC_SendStream_Seek(fpStrm, pu8Buf, pstVdecThreadParam);
    else
        SAMPLE_COMM_VDEC_SendStream_Pos(fpStrm, pu8Buf, pstVdecThreadParam);



    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = FY_TRUE;
    FY_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    //printf("SAMPLE_TEST:send steam thread %d return ...\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != FY_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (FY_VOID *)FY_SUCCESS;
}


unsigned int read_u32(unsigned char*p)
{
	unsigned int value;
	value = (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | p[3];
	return value;
}

unsigned int read_u24(unsigned char*p)
{
	unsigned int value;
	value = (p[0]<<16) | (p[1]<<8) | p[2];
	return value;
}


int SAMPLE_COMM_VDEC_PaserFAV_Header(char* fileName,struct fav_stream* streams )
{
	FILE *fp=NULL;
	unsigned char header[48];
	int rdsz;
	int audio_cnt = 0;
	int video_cnt = 0;
	struct fav_stream* pstream;

	fp = fopen(fileName, "rb");
	if(fp==NULL)
	{
		 printf("can't open file %s\n",fileName);
		 return -1;
	}
	rdsz = fread(header,1,16,fp );
	if(strcmp((char*)header,"FAV V1"))
	{
		printf("file %s is not fav format %s\n",fileName,header);
		return -1;
	}

	pstream = streams;

	while(1)
	{
		rdsz = fread(header,1,48,fp);
		if(rdsz != 48)
		{
			printf("read file %s for stream header failed!!!\n",fileName);
			return -1;
		}
		if(header[0]=='a')
		{
			pstream->audio_freq = read_u32(&header[12]);
			pstream->audio_chan = read_u32(&header[16]);
			audio_cnt++;
		}
		else if(header[0]=='v')
		{
			pstream->video_width = read_u32(&header[12]);
			pstream->video_height= read_u32(&header[16]);
			video_cnt++;
		}
		else if(header[0]=='\0')
			break;
		else
		{
			printf("file %s is not fav format.\n",fileName);
			return -1;
		}
		pstream->stream_type = header[0];
		strncpy(pstream->codec,(char*)&header[1],10);
		pstream->packet_cnt = read_u32(&header[20]);
		pstream++;
	}

	fclose(fp);

	return audio_cnt + video_cnt;


}

int SAMPLE_COMM_VDEC_PaserFAV_Packet_Header(char* fileName,int stream_cnt, struct fav_stream* streams,struct fav_packet_info* pPackets )
{
	FILE *fp=NULL;
	unsigned char header[48];
	int rdsz;
	int packet_cnt = 0;
	int i;
	struct fav_packet_info* pPacket;
	int pos = 0;

	fp = fopen(fileName, "rb");
	if(fp==NULL)
	{
		 printf("can't open file %s\n",fileName);
		 return -1;
	}
	rdsz = fread(header,1,16,fp );
	if(strcmp((char*)header,"FAV V1"))
	{
		printf("file %s is not fav format %s\n",fileName,header);
		return -1;
	}

	while(1)
	{
		rdsz = fread(header,1,48,fp);
		if(rdsz!=48)
			return -1;
		if(header[0] == '\0') break;
	}

	pos =  16 + (stream_cnt+1)*48;

	for(i=0;i<stream_cnt;i++)
	{
		packet_cnt += streams[i].packet_cnt;
	}
	pos += (packet_cnt*8);

	pPacket = pPackets;

	for(i=0;i<packet_cnt;i++)
	{
		rdsz = fread(header,1,8,fp );
		if(rdsz!=8)
			return -1;
		pPacket->stream_idx = header[0];
		pPacket->stream_type = streams[pPacket->stream_idx].stream_type;
		pPacket->size = read_u24(&header[1]);
		pPacket->pts = read_u32(&header[4]);;
		pPacket->pos = pos;
		pos += pPacket->size;
		pPacket++;
	}

	fclose(fp);

	return packet_cnt;

}


int SAMPLE_COMM_VDEC_PaserFAV_Packet(const VdecThreadParam *pstVdecThreadParam,const int stream_cnt, const struct fav_stream* streams,const int packet_cnt,const struct fav_packet_info* pPackets, const int audio_idx, const int video_idx)
{
#define OUPUT_TO_FILE 0
	FILE *fp=NULL;
	unsigned char* pbuf;
	int rdsz;
	int max_packt_size = 0;
	int i,j;
	const struct fav_packet_info* pPacket;
	const char* fileName = pstVdecThreadParam->cFileName;
#if(OUPUT_TO_FILE)
	FILE *fpa=NULL;
	FILE *fpv=NULL;
	char file_name[256];

	sprintf(file_name,"%s.pcm",fileName);
	fpa = fopen(file_name,"wb+");

	sprintf(file_name,"%s.264",fileName);
	fpv = fopen(file_name,"wb+");
#else
	VDEC_STREAM_S stStream;
	AUDIO_FRAME_S       stAudioFrm;
	FY_S32 ret;
#endif
	fp = fopen(fileName, "rb");
	if(fp==NULL)
	{
		 printf("can't open file %s\n",fileName);
		 return -1;
	}
	pPacket = pPackets;
	for(i=0;i<packet_cnt;i++)
	{
		if(pPacket->size>max_packt_size)
			max_packt_size = pPacket->size;
		pPacket++;
	}


	pbuf = malloc(8*1024*1024);
	if(pbuf==NULL)
	{
		printf("malloc(%d) failed\n",max_packt_size);
		return 0;
	}

LOOP:
	fseek(fp,16+(stream_cnt+1)*48 +packet_cnt*8,SEEK_SET);
	pPacket = pPackets;
	for(i=0;i<packet_cnt;i++)
	{
		if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
		{
			printf("Read AV Packet STOPPED!\n");
			break;
		}
		if( (pPacket->stream_type == 'a' && pPacket->stream_idx!=audio_idx)
			|| (pPacket->stream_type=='v' && pPacket->stream_idx!=video_idx)
			)
		{
			fseek(fp,pPacket->size,SEEK_CUR);
			pPacket++;
			continue;
		}
		else
			rdsz = fread(pbuf,1,pPacket->size,fp );
		if(rdsz != pPacket->size)
		{
			printf("read packet#%d/%d size:%d but only %d read!!!\n",i,packet_cnt,pPacket->size,rdsz);
			break;
		}
		if(pPacket->stream_type == 'a' && pPacket->stream_idx==audio_idx)
		{
#if(OUPUT_TO_FILE)
			fwrite(pbuf,pPacket->size,1,fpa);
#else
			stAudioFrm.enBitwidth = 16;
			stAudioFrm.enSoundmode = (streams[audio_idx].audio_chan==2)? AUDIO_SOUND_MODE_STEREO : AUDIO_SOUND_MODE_MONO;
			stAudioFrm.pVirAddr = pbuf;
			stAudioFrm.u32PhyAddr = 0;
			stAudioFrm.u32Len = pPacket->size;///streams[audio_idx].audio_chan;
			stAudioFrm.u64TimeStamp = 0;
			if(pstVdecThreadParam->s32AudioDev & (1<<I2S_DEV1))
			{
				ret = FY_MPI_AO_SendFrame((AUDIO_DEV)I2S_DEV1,(AO_CHN)pstVdecThreadParam->s32AudioChn,&stAudioFrm,40);
				if(ret)
					printf("FY_MPI_AO_SendFrame(%d,%d) u32Len:%d faild: ret:0x%x\n",I2S_DEV1,pstVdecThreadParam->s32AudioChn,stAudioFrm.u32Len,ret);
			}
			if(pstVdecThreadParam->s32AudioDev & (1<<I2S_DEV3))
			{
				ret = FY_MPI_AO_SendFrame((AUDIO_DEV)I2S_DEV3,(AO_CHN)pstVdecThreadParam->s32AudioChn,&stAudioFrm,500);
				if(ret)
					printf("FY_MPI_AO_SendFrame(%d,%d) u32Len:%d faild: ret:0x%x\n",I2S_DEV3,pstVdecThreadParam->s32AudioChn,stAudioFrm.u32Len,ret);
			}

#endif
		}
		else if(pPacket->stream_type == 'v' && pPacket->stream_idx==video_idx)
		{
#if(OUPUT_TO_FILE)
			fwrite(pbuf,pPacket->size,1,fpv);
#else

			stStream.pu8Addr = pbuf;
			stStream.u32Len  = pPacket->size;
			stStream.u64PTS  = pPacket->pts*1000;
			stStream.bEndOfFrame  = FY_TRUE;
			stStream.bEndOfStream = FY_FALSE;
			for(j=0;j<pstVdecThreadParam->s32VdecCnt;j++)
			{
				ret =  FY_MPI_VDEC_SendStream((VDEC_CHN)j,&stStream,1000);
				if(ret)
					printf("FY_MPI_VDEC_SendStream(%d) faild: ret:0x%x\n",j,ret);
			}
#endif
		}
		pPacket++;

	}

	if(i==packet_cnt && pstVdecThreadParam->bLoopSend && pstVdecThreadParam->eCtrlSinal != VDEC_CTRL_STOP)
	{
		goto LOOP;
	}
	if(!pstVdecThreadParam->bLoopSend)
	{
		printf("Playback FAV to file end!\n");
	}
	free(pbuf);
	fclose(fp);

#if(OUPUT_TO_FILE)
	fclose(fpa);
	fclose(fpv);
#endif

	return i;
}

FY_VOID * SAMPLE_COMM_VDEC_SendFAVSteam(FY_VOID *pArgs)
{
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
	char  task_name[64];
	int i;
	struct fav_stream streams[64];
	struct fav_packet_info* ppacket_info;
	int stream_cnt;
	int packet_cnt = 0;
	int ret = 0;


	sprintf(task_name,"vdec_sendavstream");
	prctl(PR_SET_NAME, task_name);


	stream_cnt = SAMPLE_COMM_VDEC_PaserFAV_Header(pstVdecThreadParam->cFileName,streams);
	if(stream_cnt<0)
	{
		printf("SAMPLE_COMM_VDEC_PaserFAV_Header:%d error!!!\n",stream_cnt);
		return (FY_VOID *)FY_FAILURE;
	}
	for(i=0;i<stream_cnt;i++)
		packet_cnt += streams[i].packet_cnt;
	ppacket_info = malloc(sizeof(struct fav_packet_info) * packet_cnt);
	if(ppacket_info==NULL)
	{
		printf("malloc faliled\n");
        return (FY_VOID *)(FY_FAILURE);
	}
	ret = SAMPLE_COMM_VDEC_PaserFAV_Packet_Header(pstVdecThreadParam->cFileName,stream_cnt,streams,ppacket_info);
	if(ret<0)
	{
		printf("SAMPLE_COMM_VDEC_PaserFAV_Packet_Header:%d error!!!\n",ret);
		return (FY_VOID *)FY_FAILURE;
	}

	SAMPLE_COMM_VDEC_PaserFAV_Packet(pstVdecThreadParam,stream_cnt,streams,packet_cnt,ppacket_info,pstVdecThreadParam->s32AudioIdx,pstVdecThreadParam->s32VideoIdx);

	free(ppacket_info);
    //printf("SAMPLE_TEST:send steam thread %d return ...\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);

    return (FY_VOID *)FY_SUCCESS;
}

FY_VOID SAMPLE_COMM_VDEC_CmdCtrl(FY_S32 s32ChnNum,VdecThreadParam *pstVdecSend)
{
    FY_S32 i;
    FY_S32 s32FrameRate = 0;
    FY_BOOL /*bIsPause = FY_FALSE, */bVoPause = FY_FALSE;
    VDEC_CHN_STAT_S stStat;
    char c=0;
         for (i=0; i<s32ChnNum; i++)
        pstVdecSend[i].cUserCmd = 0;
    while(1)
    {
        printf("\nSAMPLE_TEST:press 'e' to exit; 'p' to pause/resume; 'q' to query!;'s' to step!;'a' to add!;'d' to sub!;\n");

        c = getchar();
        if(10 == c)
        {
            continue;
        }
        getchar();
        if (c == 'e')
        {
            for (i=0; i<s32ChnNum; i++)
            pstVdecSend[i].cUserCmd = 0;
            break;
        }
//        else if (c == 'r')
//        {
//            if (bIsPause)
//            {
//                for (i=0; i<s32ChnNum; i++)
//                pstVdecSend[i].eCtrlSinal = VDEC_CTRL_PAUSE;
//            }
//            else
//            {
//                for (i=0; i<s32ChnNum; i++)
//                pstVdecSend[i].eCtrlSinal = VDEC_CTRL_START;
//            }
//            bIsPause = !bIsPause;
//
//        }
        else if (c == 'p')
        {
            if (bVoPause)
            {
                FY_MPI_VO_ResumeChn(0, 0);
                FY_MPI_VO_ResumeChn(1, 0);
                printf("VO Resume.");
            }
            else
            {
                FY_MPI_VO_PauseChn(0, 0);
                FY_MPI_VO_PauseChn(1, 0);
                printf("VO Pause.");
            }
            bVoPause = !bVoPause;
        }
        else if (c == 'a')
        {
            for(i = 0; i < 2; i++)
            {
                FY_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate >= 120)
                {
                    printf("VO layer %d is larger than 120.", i);
                    continue;
                }
                s32FrameRate += 10;
                FY_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO layer %d is set to %d.", i, s32FrameRate);
             }
        }
        else if (c == 'd')
        {
            for(i = 0; i < 2; i++)
            {
                FY_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate < 10)
                {
                    printf("VO layer %d is less than 10.", i);
                    continue;
                }
                s32FrameRate -= 10;
                FY_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO layer %d is set to %d.", i, s32FrameRate);
             }
        }
        else if (c == 's')
        {
            if (bVoPause == FY_FALSE)
            {
                printf("Firstly press 'p' to pause,then step.");
                continue;
            }
            FY_MPI_VO_StepChn(0, 0);
            FY_MPI_VO_StepChn(1, 0);
            printf("VO Step.");
        }
        else if (c == 'q')
        {
            for (i=0; i<s32ChnNum; i++)
            {
                FY_MPI_VDEC_Query(pstVdecSend[i].s32ChnId, &stStat);
                PRINTF_VDEC_CHN_STATE(pstVdecSend[i].s32ChnId, stStat);
            }
        }
        fflush(stdout);
    }
}

FY_VOID SAMPLE_COMM_VDEC_StartSendStream(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    FY_S32  i;

	if(pstVdecSend->bFAV)
	{

		int err = 0;
		pthread_attr_t attr;
		err = pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,(128<<10));


		pstVdecSend[0].eCtrlSinal = VDEC_CTRL_START;
		err = pthread_create(&pVdecThread[0], &attr, SAMPLE_COMM_VDEC_SendFAVSteam, (FY_VOID *)&pstVdecSend[0]);
		if(err)
			perror("pthread_create SAMPLE_COMM_VDEC_SendFAVSteam");
		pthread_attr_destroy(&attr);
	}
	else {
	    for(i=0; i<s32ChnNum; i++)
	    {
	        int err = 0;
	        pstVdecSend[i].eCtrlSinal = VDEC_CTRL_START;
	        struct sched_param param;
	        pthread_attr_t attr;
	        err = pthread_attr_init(&attr);
	        FY_ASSERT(!err);

	        err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	        FY_ASSERT(!err);
	        err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	        FY_ASSERT(!err);

	        param.sched_priority = 50;//sched_get_priority_max(SCHED_FIFO);
	        err = pthread_attr_setschedparam(&attr, &param);
	        FY_ASSERT( !err );
	        err = pthread_create(&pVdecThread[i], &attr, SAMPLE_COMM_VDEC_SendStream, (FY_VOID *)&pstVdecSend[i]);
	        if(err) {
	            printf("pthread_create create failed! err = %s(%d)\n", strerror(err), err);
	        }
	        pthread_attr_destroy(&attr);
	    }
	}
}

FY_VOID SAMPLE_COMM_VDEC_StartSendStreamChannel(FY_S32 s32Chn, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
	pstVdecSend[s32Chn].eCtrlSinal=VDEC_CTRL_START;
	pthread_create(&pVdecThread[s32Chn], 0, SAMPLE_COMM_VDEC_SendStream, (FY_VOID *)&pstVdecSend[s32Chn]);
}


FY_VOID SAMPLE_COMM_VDEC_StopSendStream(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    FY_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        FY_MPI_VDEC_StopRecvStream(i);
        if(!pstVdecSend->bFAV && pstVdecSend[i].eCtrlSinal == VDEC_CTRL_START)
        {
            pstVdecSend[i].eCtrlSinal=VDEC_CTRL_STOP;
            pthread_join(pVdecThread[i], FY_NULL);
		}
    }
	if(pstVdecSend->bFAV)
	{
		pstVdecSend->eCtrlSinal=VDEC_CTRL_STOP;
		pthread_join(pVdecThread[0], FY_NULL);
	}

}

FY_VOID SAMPLE_COMM_VDEC_StopSendStreamChannel(FY_S32 s32Chn, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    FY_MPI_VDEC_StopRecvStream(s32Chn);
    pstVdecSend[s32Chn].eCtrlSinal=VDEC_CTRL_STOP;
    pthread_join(pVdecThread[s32Chn], FY_NULL);
}



FY_S32 SAMPLE_COMM_VDEC_Start(FY_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstAttr, FY_U32 u32BlkCnt)
{
    FY_S32  i;
//    FY_U32 u32BlkCnt = 10;
//    VDEC_CHN_POOL_S stPool;

    for(i=0; i<s32ChnNum; i++)
    {
        //if(1 == g_s32VBSource)
        {
            CHECK_CHN_RET(FY_MPI_VDEC_SetChnVBCnt(i, u32BlkCnt), i, "FY_MPI_VDEC_SetChnVBCnt");
        }
        CHECK_CHN_RET(FY_MPI_VDEC_CreateChn(i, &pstAttr[i]), i, "FY_MPI_VDEC_CreateChn");
#if 0
        if (2 == g_s32VBSource)
        {
            stPool.hPicVbPool = g_ahVbPool[0];
            stPool.hPmvVbPool = -1;
            CHECK_CHN_RET(FY_MPI_VDEC_AttachVbPool(i, &stPool), i, "FY_MPI_VDEC_AttachVbPool");
        }
#endif
        CHECK_CHN_RET(FY_MPI_VDEC_StartRecvStream(i), i, "FY_MPI_VDEC_StartRecvStream");
        CHECK_CHN_RET(FY_MPI_VDEC_SetDisplayMode(i, VIDEO_DISPLAY_MODE_PLAYBACK), i, "FY_MPI_VDEC_SetDisplayMode");
    }

    return FY_SUCCESS;
}



FY_S32 SAMPLE_COMM_VDEC_Start_Channel(FY_S32 s32Chn, VDEC_CHN_ATTR_S *pstAttr, FY_U32 u32BlkCnt)
{
	CHECK_CHN_RET(FY_MPI_VDEC_SetChnVBCnt(s32Chn, u32BlkCnt), s32Chn, "FY_MPI_VDEC_SetChnVBCnt");
    CHECK_CHN_RET(FY_MPI_VDEC_CreateChn(s32Chn, &pstAttr[s32Chn]), s32Chn, "FY_MPI_VDEC_CreateChn");
    CHECK_CHN_RET(FY_MPI_VDEC_StartRecvStream(s32Chn), s32Chn, "FY_MPI_VDEC_StartRecvStream");
	CHECK_CHN_RET(FY_MPI_VDEC_SetDisplayMode(s32Chn, VIDEO_DISPLAY_MODE_PLAYBACK), s32Chn, "FY_MPI_VDEC_SetDisplayMode");

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VDEC_Stop(FY_S32 s32ChnNum)
{
    FY_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        CHECK_CHN_RET(FY_MPI_VDEC_StopRecvStream(i), i, "FY_MPI_VDEC_StopRecvStream");
        CHECK_CHN_RET(FY_MPI_VDEC_DestroyChn(i), i, "FY_MPI_VDEC_DestroyChn");
    }

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VDEC_Stop_Channel(FY_S32 s32Chn)
{
    CHECK_CHN_RET(FY_MPI_VDEC_StopRecvStream(s32Chn), s32Chn, "FY_MPI_VDEC_StopRecvStream");
    CHECK_CHN_RET(FY_MPI_VDEC_DestroyChn(s32Chn), s32Chn, "FY_MPI_VDEC_DestroyChn");

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VDEC_BindVgs(VDEC_CHN VdChn, VGS_CHN VgsChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = FY_ID_VGS;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VgsChn;

    CHECK_RET(FY_MPI_SYS_Bind(&stSrcChn, &stDestChn), "FY_MPI_SYS_Bind");

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VGS_BindVo(VGS_CHN VgsChn,VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(FY_MPI_SYS_Bind(&stSrcChn, &stDestChn), "FY_MPI_SYS_Bind");

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VDEC_BindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(FY_MPI_SYS_Bind(&stSrcChn, &stDestChn), "FY_MPI_SYS_Bind");

    return FY_SUCCESS;
}



FY_S32 SAMPLE_COMM_VDEC_UnBindVgs(VDEC_CHN VdChn, VGS_CHN VgsChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = FY_ID_VGS;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VgsChn;

    CHECK_RET(FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "FY_MPI_SYS_UnBind");

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VGS_UnBindVo(VGS_CHN VgsChn,VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "FY_MPI_SYS_UnBind");

    return FY_SUCCESS;
}


FY_S32 SAMPLE_COMM_VDEC_UnBindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = FY_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "FY_MPI_SYS_UnBind");

    return FY_SUCCESS;
}



FY_S32 SAMPLE_VDEC_VGS_Init(FY_S32 s32ChnNum, int square)
{
	int i;
	VGS_CHN VgsChn;
    VGS_CHN_PARA_S VgsMode = {
        .enChnMode = VGS_CHN_MODE_AUTO,//VGS_CHN_MODE_USER,
        .u32Width = 1280 / square,
        .u32Height = 720 / square,
        .enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    };

    VgsMode.u32Width = ALIGN_BACK(VgsMode.u32Width, 8);
    VgsMode.u32Height = ALIGN_BACK(VgsMode.u32Height, 8);

	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;

	    CHECK_CHN_RET(FY_MPI_VGS_CreateChn(VgsChn),i,"FY_MPI_VGS_CreateChn");
	    CHECK_CHN_RET(FY_MPI_VGS_SetChnMode(VgsChn, 0, &VgsMode),i,"FY_MPI_VGS_SetChnMode");
	}
    return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_VGS_DeInit(FY_S32 s32ChnNum)
{
	int i;
	VGS_CHN VgsChn;
	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;
	    CHECK_CHN_RET(FY_MPI_VGS_DestroyChn(VgsChn),i, "FY_MPI_VGS_DestroyChn");
	}

	return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_Stop_VGS_OneChanne(FY_S32 s32Chn )
{
	CHECK_CHN_RET(FY_MPI_VGS_DestroyChn(s32Chn),s32Chn, "FY_MPI_VGS_DestroyChn");
	return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_Start_VGS_OneChanne(FY_S32 s32Chn,int square )
{
	VGS_CHN VgsChn;
	VGS_CHN_PARA_S vpgsMode = {
	    .enChnMode = VGS_CHN_MODE_AUTO,//VGS_CHN_MODE_USER,
	    .u32Width = 1280 / square,
	    .u32Height = 720 / square,
	    .enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
	};

	vpgsMode.u32Width = ALIGN_BACK(vpgsMode.u32Width, 8);
	vpgsMode.u32Height = ALIGN_BACK(vpgsMode.u32Height, 8);

	VgsChn = (VGS_CHN)s32Chn;

	CHECK_CHN_RET(FY_MPI_VGS_CreateChn(VgsChn),s32Chn,"FY_MPI_VGS_CreateChn");
	CHECK_CHN_RET(FY_MPI_VGS_SetChnMode(VgsChn, 0, &vpgsMode),s32Chn,"FY_MPI_VGS_SetChnMode");

	return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_VO_Init(int enMode)
{
    FY_S32 s32Ret = FY_SUCCESS;

	s32Ret = SAMPLE_COMM_VO_StartChn(SAMPLE_VO_LAYER_VHD0,enMode);
	return s32Ret;
}

FY_S32 SAMPLE_VDEC_VO_DeInit(int enMode)
{
    FY_S32 s32Ret = FY_SUCCESS;

	s32Ret = SAMPLE_COMM_VO_StopChn(SAMPLE_VO_LAYER_VHD0,enMode);
	return s32Ret;
}

FY_S32 SAMPLE_VDEC_VO_StopChannel(FY_S32 s32Chn)
{
	FY_MPI_VO_DisableChn(SAMPLE_VO_LAYER_VHD0,(VO_CHN)s32Chn);

	return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_VO_StartChannel(FY_S32 s32Chn,int enMode)
{
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    VO_CHN_ATTR_S stChnAttr;

	FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stLayerAttr);
	SAMPLE_COMM_VO_GetChnRect(s32Chn, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);

	stChnAttr.u32Priority		= 0;
	stChnAttr.bDeflicker		= FY_FALSE;

	FY_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD0, s32Chn, &stChnAttr);
	FY_MPI_VO_SetChnFrameRate(SAMPLE_VO_LAYER_VHD0, s32Chn, 15);
	FY_MPI_VO_EnableChn(SAMPLE_VO_LAYER_VHD0,(VO_CHN)s32Chn);

	return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_VGS_Bind_VO(FY_S32 s32ChnNum)
{
	int i;
	VGS_CHN VgsChn;
	VO_LAYER VoLayer;
	VO_CHN VoChn;

	VoLayer = SAMPLE_VO_LAYER_VHD0;
	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;
		VoChn = i;

		CHECK_CHN_RET(SAMPLE_COMM_VGS_BindVo(VgsChn,VoLayer,VoChn),i,"SAMPLE_COMM_VGS_BindVo");
	}

	return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_VGS_UnBind_VO(FY_S32 s32ChnNum)
{
	int i;
	VGS_CHN VgsChn;
	VO_LAYER VoLayer;
	VO_CHN VoChn;

	VoLayer = SAMPLE_VO_LAYER_VHD0;
	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;
		VoChn = i;

		CHECK_CHN_RET(SAMPLE_COMM_VGS_UnBindVo(VgsChn,VoLayer,VoChn),i,"SAMPLE_COMM_VGS_UnBindVo");
	}

	return FY_SUCCESS;
}


/******************************************************************************
* function : Set system memory location
******************************************************************************/
FY_S32 SAMPLE_COMM_VDEC_MemConfig(FY_VOID)
{
    FY_S32 i = 0;
    FY_S32 s32Ret = FY_SUCCESS;

    FY_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVDEC;

    for(i=0; i<VDEC_MAX_CHN_NUM; i++)
    {
        stMppChnVDEC.enModId = FY_ID_VDEC;
        stMppChnVDEC.s32DevId = 0;
        stMppChnVDEC.s32ChnId = i;

        if( 1)//0 == (i%2))
        {
            pcMmzName = NULL;
        }
        else
        {
            pcMmzName = "ddr1";
        }

        s32Ret = FY_MPI_SYS_SetMemConf(&stMppChnVDEC,pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("FY_MPI_SYS_SetMemConf ERR !\n");
            return FY_FAILURE;
        }
    }

    return FY_SUCCESS;
}


/******************************************************************************
* function : SAMPLE_COMM_VDEC_Load_UserPic
******************************************************************************/
FY_S32 SAMPLE_COMM_VDEC_Load_UserPic(char* fname, FY_U32 w,FY_U32 h, PIXEL_FORMAT_E format,VIDEO_FRAME_INFO_S* pUserFrame)
{
	FILE  *fp   = NULL;
	FY_U32 u32Size;
	VB_POOL pool_id = VB_INVALID_POOLID;
	VB_BLK  VbBlk = VB_INVALID_HANDLE;
	FY_U32 u32PhyAddr = 0;
	FY_U8 *pVirAddr = NULL;

	if(format != PIXEL_FORMAT_YUV_SEMIPLANAR_420 && format != PIXEL_FORMAT_YUV_SEMIPLANAR_422)
		return FY_FAILURE;

	fp = fopen(fname, "rb");
	if(NULL == fp) {
		printf("open file %s failed!\n",fname);
		return FY_FAILURE;
	}

	u32Size = w*h;
	if(format == PIXEL_FORMAT_YUV_SEMIPLANAR_420 )
		u32Size += u32Size/2;
	else
		u32Size += u32Size;

	pool_id = FY_MPI_VB_CreatePool(u32Size,1,NULL);
	if(pool_id == VB_INVALID_POOLID)
	{
		printf("FY_MPI_VB_CreatePool failed!\n");
		goto FAIL;
	}

	VbBlk = FY_MPI_VB_GetBlock(pool_id, u32Size,NULL);
	if(VbBlk == VB_INVALID_HANDLE)
	{
		printf("FY_MPI_VB_GetBlock failed!\n");
		goto FAIL;
	}

	u32PhyAddr = FY_MPI_VB_Handle2PhysAddr(VbBlk);
	pVirAddr = (FY_U8 *) FY_MPI_SYS_Mmap(u32PhyAddr, u32Size);
	if (NULL == pVirAddr)
	{
		printf("FY_MPI_SYS_Mmap failed!\n");
		goto FAIL;
	}
	fread(pVirAddr, 1, u32Size, fp);
	fclose(fp);

	memset(pUserFrame,0,sizeof(VIDEO_FRAME_INFO_S));
	pUserFrame->u32PoolId = pool_id;
	pUserFrame->stVFrame.u32Width = w;
	pUserFrame->stVFrame.u32Height = h;
	pUserFrame->stVFrame.u32Field = VIDEO_FIELD_FRAME;
	pUserFrame->stVFrame.enPixelFormat = format;
	pUserFrame->stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	pUserFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pUserFrame->stVFrame.u32PhyAddr[0] = u32PhyAddr;
	pUserFrame->stVFrame.u32PhyAddr[1] = u32PhyAddr +  w*h;
	pUserFrame->stVFrame.pVirAddr[0] = pVirAddr;
	pUserFrame->stVFrame.pVirAddr[1] = pVirAddr +  w*h;
	pUserFrame->stVFrame.u32Stride[0] = w;
	pUserFrame->stVFrame.u32Stride[1] = format==PIXEL_FORMAT_YUV_SEMIPLANAR_420? (w/2) : w;
	pUserFrame->stVFrame.s16OffsetTop = 0;
	pUserFrame->stVFrame.s16OffsetBottom = 0;
	pUserFrame->stVFrame.s16OffsetLeft = 0;
	pUserFrame->stVFrame.s16OffsetRight = 0;
	pUserFrame->stVFrame.u32PrivateData = VbBlk;

	return FY_SUCCESS;

FAIL:

	if(VbBlk != VB_INVALID_HANDLE)
		FY_MPI_VB_ReleaseBlock(VbBlk);
	if(pool_id!= VB_INVALID_POOLID)
		FY_MPI_VB_DestroyPool(pool_id);
	if(fp!=NULL)
		fclose(fp);
	return FY_FAILURE;

}


/******************************************************************************
* function : SAMPLE_COMM_VDEC_Release_UserPic
******************************************************************************/
FY_S32 SAMPLE_COMM_VDEC_Release_UserPic(VIDEO_FRAME_INFO_S* pUserFrame)
{
	VB_BLK  VbBlk;
	FY_U8 *pVirAddr = NULL;
	FY_U32 u32Size;

	VbBlk = pUserFrame->stVFrame.u32PrivateData;
	if(VbBlk == VB_INVALID_POOLID)
		return FY_FAILURE;
	pVirAddr = (FY_U8 *)pUserFrame->stVFrame.pVirAddr[0];

	u32Size = pUserFrame->stVFrame.u32Width* pUserFrame->stVFrame.u32Height;
	if(pUserFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_420)
		u32Size += u32Size/2;
	else
		u32Size += u32Size;

	FY_MPI_SYS_Munmap(pVirAddr,u32Size);
	FY_MPI_VB_ReleaseBlock(VbBlk);
	pUserFrame->stVFrame.u32PrivateData = VB_INVALID_POOLID;

	FY_MPI_VB_DestroyPool(pUserFrame->u32PoolId);

	return FY_SUCCESS;
}

/******************************************************************************
* function : 3 types GetImages:
*                VdecGetImages:   get and release frame
*                VdecVGSGetImages: send the frame to VGS
*                VdecVOGetImages : send the frame to VOU
******************************************************************************/

FY_VOID	* VdecGetImages(FY_VOID	*pArgs)
{
	VdecGetImageThreadParam* parm =	(VdecGetImageThreadParam*)pArgs;
	FY_BOOL	bUseSelect;
	fd_set read_fds;
	struct timeval TimeoutVal;
	int	vdec_fd[VDEC_MAX_CHN_NUM];
	int	i,ret;
	int	maxfd =	-1;
	VIDEO_FRAME_INFO_S videoFrame;

	bUseSelect = 0;//parm->bUseSelect;

	parm->eCtrlSinal = VDEC_CTRL_START;

	if(bUseSelect)
	{
		for(i=0;i<parm->u32ChnCnt;i++)
		{
			vdec_fd[i] = FY_MPI_VDEC_GetFd(i);
		}
	}

	while(1)
	{
		if	(parm->eCtrlSinal == VDEC_CTRL_STOP)
		{
			break;
		}

		 if(bUseSelect)
		 {
			TimeoutVal.tv_sec =	parm->s32IntervalTime;
			TimeoutVal.tv_usec = 0;

			FD_ZERO(&read_fds);
			maxfd =	-1;
			for(i=0;i<parm->u32ChnCnt;i++)
			{
				if(vdec_fd[i] >	maxfd)
				{
					maxfd =	vdec_fd[i];
				}

				FD_SET(vdec_fd[i],&read_fds);
			}


			ret	= select(maxfd + 1,&read_fds,NULL,NULL,&TimeoutVal);
			if(ret ==1)
			{
				printf("select(...)	has	data!\n");
			}
			else
			{
				printf("select(%d,...,[%ld]) ret = %d\n",maxfd +1,TimeoutVal.tv_sec, ret);
				continue;
			}
		}

		for(i=0;i<parm->u32ChnCnt;i++)
		{
			if(!bUseSelect || (bUseSelect && FD_ISSET(vdec_fd[i], &read_fds)))
			{
				ret	= FY_MPI_VDEC_GetImage(i,&videoFrame,0);//bUseSelect? 0 : parm->s32IntervalTime*1000);
				if(ret)	continue;

				ret	= FY_MPI_VDEC_ReleaseImage(i,&videoFrame);
				if(ret)
					printf("FY_MPI_VDEC_ReleaseImage(%d,..)	failed!!!\n",i);
				else
				{

				}
			}
		}

		if(!bUseSelect) usleep(parm->s32IntervalTime*1000);
	}

	return NULL;
}


FY_VOID	* VdecVGSGetImages(FY_VOID	*pArgs)
{
	VdecGetImageThreadParam* parm =	(VdecGetImageThreadParam*)pArgs;
	FY_BOOL	bUseSelect;
	fd_set read_fds;
	struct timeval TimeoutVal;
	int	vdec_fd[VDEC_MAX_CHN_NUM];
	int	i,ret;
	int	maxfd =	-1;
	VIDEO_FRAME_INFO_S videoFrame;

	bUseSelect = 0;//parm->bUseSelect;

	parm->eCtrlSinal = VDEC_CTRL_START;

	if(bUseSelect)
	{
		for(i=0;i<parm->u32ChnCnt;i++)
		{
			vdec_fd[i] = FY_MPI_VDEC_GetFd(i);
		}
	}

	while(1)
	{
		if	(parm->eCtrlSinal == VDEC_CTRL_STOP)
		{
			break;
		}

		 if(bUseSelect)
		 {
			TimeoutVal.tv_sec =	parm->s32IntervalTime;
			TimeoutVal.tv_usec = 0;

			FD_ZERO(&read_fds);
			maxfd =	-1;
			for(i=0;i<parm->u32ChnCnt;i++)
			{
				if(vdec_fd[i] >	maxfd)
				{
					maxfd =	vdec_fd[i];
				}

				FD_SET(vdec_fd[i],&read_fds);
			}


			ret	= select(maxfd + 1,&read_fds,NULL,NULL,&TimeoutVal);
			if(ret ==1)
			{
				printf("select(...)	has	data!\n");
			}
			else
			{
				printf("select(%d,...,[%ld]) ret = %d\n",maxfd +1,TimeoutVal.tv_sec, ret);
				continue;
			}
		}

		for(i=0;i<parm->u32ChnCnt;i++)
		{
			if(!bUseSelect || (bUseSelect && FD_ISSET(vdec_fd[i], &read_fds)))
			{
				ret	= FY_MPI_VDEC_GetImage(i,&videoFrame,2000);//bUseSelect? 0 : parm->s32IntervalTime*1000);
				if(ret) {
                    printf("FY_MPI_VDEC_GetImage(%d,..) failed!\n",i);
                    continue;
				}

				ret = FY_MPI_VGS_SendFrame(i,&videoFrame,1,200);//0:preview 1:playback
				if(ret){
					printf("FY_MPI_VGS_SendFrame(%d,..) failed!\n",i);
                }
				ret	= FY_MPI_VDEC_ReleaseImage(i,&videoFrame);
				if(ret)
					printf("FY_MPI_VDEC_ReleaseImage(%d,..)	failed!!!\n",i);
				else
				{

				}
			}
		}

		if(!bUseSelect) usleep(parm->s32IntervalTime*1000);
	}

	return NULL;
}

FY_VOID	* VdecVOGetImages(FY_VOID	*pArgs)
{
	VdecGetImageThreadParam* parm =	(VdecGetImageThreadParam*)pArgs;
	FY_BOOL	bUseSelect;
	fd_set read_fds;
	struct timeval TimeoutVal;
	int	vdec_fd[VDEC_MAX_CHN_NUM];
	int	i,ret;
	int	maxfd =	-1;
	VIDEO_FRAME_INFO_S videoFrame;


	bUseSelect = 0;//parm->bUseSelect;

	parm->eCtrlSinal = VDEC_CTRL_START;

	if(bUseSelect)
	{
		for(i=0;i<parm->u32ChnCnt;i++)
		{
			vdec_fd[i] = FY_MPI_VDEC_GetFd(i);
		}
	}

	while(1)
	{
		if	(parm->eCtrlSinal == VDEC_CTRL_STOP)
		{
			break;
		}

		 if(bUseSelect)
		 {
			TimeoutVal.tv_sec =	parm->s32IntervalTime;
			TimeoutVal.tv_usec = 0;

			FD_ZERO(&read_fds);
			maxfd =	-1;
			for(i=0;i<parm->u32ChnCnt;i++)
			{
				if(vdec_fd[i] >	maxfd)
				{
					maxfd =	vdec_fd[i];
				}

				FD_SET(vdec_fd[i],&read_fds);
			}


			ret	= select(maxfd + 1,&read_fds,NULL,NULL,&TimeoutVal);
			if(ret ==1)
			{
				printf("select(...)	has	data!\n");
			}
			else
			{
				printf("select(%d,...,[%ld]) ret = %d\n",maxfd +1,TimeoutVal.tv_sec, ret);
				continue;
			}
		}

		for(i=0;i<parm->u32ChnCnt;i++)
		{
			if(!bUseSelect || (bUseSelect && FD_ISSET(vdec_fd[i], &read_fds)))
			{
				ret	= FY_MPI_VDEC_GetImage(i,&videoFrame,0);//bUseSelect? 0 : parm->s32IntervalTime*1000);
				if(ret)	continue;

				ret = FY_MPI_VO_SendFrame(SAMPLE_VO_LAYER_VHD0,i,&videoFrame,1000);

				ret	= FY_MPI_VDEC_ReleaseImage(i,&videoFrame);
				if(ret)
					printf("FY_MPI_VDEC_ReleaseImage(%d,..)	failed!!!\n",i);
				else
				{

				}
			}
		}

		if(!bUseSelect) usleep(parm->s32IntervalTime*1000);
	}

	return NULL;
}



FY_S32 SAMPLE_VDEC_ParseStream(char *fname, PAYLOAD_TYPE_E type, FY_U32 *pFrame, FY_U32 size)
{

    FILE *fpStrm = NULL;
    FY_BOOL bFindStart, bFindEnd;
    FY_S32  i,  start = 0;
    FY_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FY_S32 s32MinBufSize = 512 * 1024;
    FY_S32 s32Cnt = 0;

    fpStrm = fopen(fname, "rb");
    if (NULL == fpStrm) {
        printf("stream file: %s open failed!\n", fname);
        return -1;
    }
    if((type != PT_H265) && (type != PT_H264)) {
        printf("not support the type: %d!\n", type);
        return -1;
    }

    FY_U8 *pu8Buf = malloc(s32MinBufSize);

    while (1) {
        if (type == PT_H264)  {
            bFindStart = FY_FALSE;
            bFindEnd   = FY_FALSE;
            start = s32UsedBytes;

            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, s32MinBufSize, fpStrm);
            if (s32ReadLen == 0) {
                break;
            }

            for (i = 0; i < s32ReadLen - 8; i++) {
                int tmp = pu8Buf[i + 3] & 0x1F;
                if (  pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                        (
                            ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
                            (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)
                        )
                   ) {
                    bFindStart = FY_TRUE;
                    i += 8;
                    break;
                }
            }

            for (; i < s32ReadLen - 8; i++) {
                int tmp = pu8Buf[i + 3] & 0x1F;
                if (  pu8Buf[i  ] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                        (
                            tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
                            ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
                            (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)
                        )
                   ) {
                    bFindEnd = FY_TRUE;
                    break;
                }
            }

            if (i > 0) {
                s32ReadLen = i;
            }
            if (bFindStart == FY_FALSE) {
                //printf("file: %s can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                //       fname, s32ReadLen, s32UsedBytes);
                goto exit;
            } else if (bFindEnd == FY_FALSE) {
                s32ReadLen = i + 8;

                //printf("file: %s can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                //       fname, s32ReadLen, s32UsedBytes);
                goto exit;
            }
        } else if   (type == PT_H265)       {
            FY_BOOL  bNewPic = FY_FALSE;
            bFindStart = FY_FALSE;
            bFindEnd   = FY_FALSE;
            start = s32UsedBytes;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, s32MinBufSize, fpStrm);

            if (s32ReadLen == 0) {
                break;

            }

            for (i = 0; i < s32ReadLen - 6; i++) {
                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                           && (((pu8Buf[i + 3] & 0x7D) >= 0x0 && (pu8Buf[i + 3] & 0x7D) <= 0x2A) || (pu8Buf[i + 3] & 0x1F) == 0x1)
                           && ((pu8Buf[i + 5] & 0x80) == 0x80)); //first slice segment

                if (bNewPic) {
                    bFindStart = FY_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i < s32ReadLen - 6; i++) {
                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                           && (((pu8Buf[i + 3] & 0x7D) >= 0x0 && (pu8Buf[i + 3] & 0x7D) <= 0x2A) || (pu8Buf[i + 3] & 0x1F) == 0x1)
                           && ((pu8Buf[i + 5] & 0x80) == 0x80)); //first slice segment

                if (  pu8Buf[i  ] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                        && (((pu8Buf[i + 3] & 0x7D) == 0x40) || ((pu8Buf[i + 3] & 0x7D) == 0x42) || ((pu8Buf[i + 3] & 0x7D) == 0x44) || bNewPic)
                   ) {
                    bFindEnd = FY_TRUE;
                    break;
                }
            }

            s32ReadLen = i;

            if (bFindStart == FY_FALSE) {
                //printf("file: %s can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                //       fname, s32ReadLen, s32UsedBytes);
                goto exit;
            } else if (bFindEnd == FY_FALSE) {
                s32ReadLen = i + 8;

                //printf("file: %s can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                //       fname, s32ReadLen, s32UsedBytes);
                goto exit;
            }

        }
        //fill the start and len
        ++s32Cnt;
        if(s32Cnt >= size) {
            goto exit;
        }
        pFrame[2 * s32Cnt] = start;
        pFrame[2 * s32Cnt + 1] = s32ReadLen;
        //printf("[%d]: %d - %d\n", s32Cnt, start, s32ReadLen);
        s32UsedBytes = s32UsedBytes +s32ReadLen;
    }

exit:
    if(fpStrm) {
        fclose(fpStrm);
    }
    if(pu8Buf) {
        free(pu8Buf);
    }
    pFrame[0] = s32Cnt;
    pFrame[1] = s32Cnt;
    printf("%s: found %d frames in file %s: type = %d\n", __func__, s32Cnt, fname, type);
    return s32Cnt;

}





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
