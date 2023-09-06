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

#include "sample_comm.h"

//static SIZE_S s_stPreSize;
#ifdef TEST_21A
#define FY_MPI_VI_SetDevAttr HI_MPI_VI_SetDevAttr
#define FY_MPI_VI_EnableDev HI_MPI_VI_EnableDev
#define FY_MPI_VI_SetChnAttr HI_MPI_VI_SetChnAttr
#define FY_MPI_VI_SetChnMinorAttr HI_MPI_VI_SetChnMinorAttr
#define FY_MPI_VI_EnableChn HI_MPI_VI_EnableChn
#define FY_MPI_VI_DisableChn HI_MPI_VI_DisableChn
#define FY_MPI_VI_DisableDev HI_MPI_VI_DisableDev
#define FY_MPI_VI_GetChnAttr HI_MPI_VI_GetChnAttr

/*****************************************************************************
* function : set vi mask.
*****************************************************************************/
void SAMPLE_COMM_VI_SetMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{
    switch (ViDev % 4)
    {
        case 0:
            pstDevAttr->au32CompMask[0] = 0xFF;
            if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0x00FF0000;
            }
            else if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;
        case 1:
            pstDevAttr->au32CompMask[0] = 0xFF00;
            if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;
        case 2:
            pstDevAttr->au32CompMask[0] = 0xFF0000;
            if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0xFF;
            }
            else if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;
        case 3:
            pstDevAttr->au32CompMask[0] = 0xFF000000;
            if (VI_MODE_BT1120_INTERLEAVED == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;
        default:
            break;
    }
}
#endif
FY_S32 SAMPLE_VI_GetChnInterval(FY_U32 u32ViDevCnt, SAMPLE_VI_PARAM_S *pstViParam)
{
    switch (u32ViDevCnt)
    {    
        //fy01 : 2 dev, 8 chn
        case 2:
            pstViParam->s32ViDevCnt = 2;
            pstViParam->s32ViDevInterval = 1;
            pstViParam->s32ViChnCnt = 8;
            pstViParam->s32ViChnInterval = 1;
            break;
			
		//fy01 : just for test 1 device, 1 chn	
		case 1:
			pstViParam->s32ViDevCnt = 1;
			pstViParam->s32ViDevInterval = 1;
			pstViParam->s32ViChnCnt = 1;
			pstViParam->s32ViChnInterval = 1;
			break;

        default:
            SAMPLE_PRT("u32ViDevCnt invaild!\n");
            return FY_FAILURE;
    }
    return FY_SUCCESS;
}

/*****************************************************************************
* function : star vi dev (cfg vi_dev_attr; set_dev_cfg; enable dev)
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_StartDev(VI_DEV ViDev,VI_DEV_ATTR_S * pstViDevAttr)
{
    FY_S32 s32Ret;
    VI_DEV_ATTR_S stViDevAttr;
    //memset(&stViDevAttr,0,sizeof(stViDevAttr));
    if(NULL == pstViDevAttr)
    {
        SAMPLE_PRT("param pstViDevAttr is NULL!\n");
        return FY_FAILURE;
    }

    memcpy(&stViDevAttr,pstViDevAttr,sizeof(stViDevAttr));
    
#ifdef TEST_21A
    stViDevAttr.s32AdChnId[0] = -1;
    stViDevAttr.s32AdChnId[1] = -1;
    stViDevAttr.s32AdChnId[2] = -1;
    stViDevAttr.s32AdChnId[3] = -1;
    SAMPLE_COMM_VI_SetMask(ViDev,&stViDevAttr);
#endif
    //SAMPLE_VI_DevCfg(ViDev,&stViDevAttr);
#if 0
//set dev attr set according viu 0.5 version
    if(0 == ViDev)
    {
        stViDevAttr.enWorkMode = VI_WORK_MODE_2Multiplex;
        stViDevAttr.u32AutoDetMode = 0;
        stViDevAttr.enIntfMode= 0;
        stViDevAttr.en2MuxMode = 3; //force sep(disable channel ID detection
        stViDevAttr.en4MuxMode = 0; //force sep(disable channel ID detection
    }
    else if(1 == ViDev)
    {        
        stViDevAttr.enWorkMode = VI_WORK_MODE_2Multiplex;
        stViDevAttr.u32AutoDetMode = 0;
        stViDevAttr.enIntfMode= 0;
        stViDevAttr.en2MuxMode = 0; //force sep(disable channel ID detection
        stViDevAttr.en4MuxMode = 0; //force sep(disable channel ID detection
    }
#endif

    s32Ret = FY_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("FY_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    s32Ret = FY_MPI_VI_EnableDev(ViDev);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("FY_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return FY_SUCCESS;
}

/*****************************************************************************
* function : star vi chn
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn,stViChnInfo *pstViChnInfo)
{
    FY_S32 s32Ret;
    VI_CHN_ATTR_S stMinorChnAttr;
    stViChnInfo ViChnInfo;

    if(NULL == pstViChnInfo)
    {
        SAMPLE_PRT("param pstViChnInfo is NULL!\n");
        return FY_FAILURE;
    }
    memset(&stMinorChnAttr,0,sizeof(VI_CHN_ATTR_S));
    memset(&ViChnInfo,0,sizeof(stViChnInfo));

  //  SAMPLE_VI_ChnCfg(ViChn,&ViChnInfo);
    memcpy(&ViChnInfo,pstViChnInfo,sizeof(stViChnInfo));

    s32Ret = FY_MPI_VI_SetChnAttr(ViChn, &(ViChnInfo.stChnAttr));
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }
    
    if(ViChnInfo.bMinor)
    {
        memcpy(&stMinorChnAttr,&(ViChnInfo.stChnAttr),sizeof(VI_CHN_ATTR_S));
        stMinorChnAttr.stDestSize.u32Width = ViChnInfo.u32MDstWidth;
        stMinorChnAttr.stDestSize.u32Height = ViChnInfo.u32MDstHeight;
        stMinorChnAttr.s32SrcFrameRate = ViChnInfo.s32MSrcFrameRate;
        stMinorChnAttr.s32DstFrameRate = ViChnInfo.s32MDstFrameRate;

        //s_stPreSize.u32Width = ViChnInfo.u32MDstWidth;
        //s_stPreSize.u32Height = ViChnInfo.u32MDstHeight;
        
        s32Ret = FY_MPI_VI_SetChnMinorAttr(ViChn, &stMinorChnAttr);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
    }
    
    s32Ret = FY_MPI_VI_EnableChn(ViChn);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    return FY_SUCCESS;
}

/*****************************************************************************
* function : star vi according to product type
*            if vi input is hd, we will start sub-chn for cvbs preview
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_Start(FY_U32 u32ViDevCnt,stViCnfInfo *psViCnfInfo)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    FY_S32 i;
    FY_S32 s32Ret;
    SAMPLE_VI_PARAM_S stViParam;

    if(NULL == psViCnfInfo)
    {
        SAMPLE_PRT("param psViCnfInfo is NULL!\n");
        return FY_FAILURE;
    }
    
    /*** get parameter from Sample_Vi_Mode ***/
    s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
    if (FY_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("vi get param failed!\n");
        return FY_FAILURE;
    }
        
    /*** Start VI Dev ***/
    for(i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = SAMPLE_COMM_VI_StartDev(ViDev,&(psViCnfInfo->stViDevAttr));
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed with %#x\n", s32Ret);
            return FY_FAILURE;
        }
    }

    if(psViCnfInfo->stViInfo.u32CompRate > 0)
    {
        VI_EC_LEVEL_E ecLevel;

        ecLevel = psViCnfInfo->stViInfo.u32CompRate;

        FY_MPI_VI_SetEC(ecLevel);
    }
	
    /*** Start VI Chn ***/
    for(i=0; i<stViParam.s32ViChnCnt; i++)
    {
        ViChn = i * stViParam.s32ViChnInterval;
		//tmp modify for t2828
		if(psViCnfInfo->ViChnInfo.stChnAttr.enScanMode == VI_SCAN_INTERLACED){
			FY_MPI_VI_SetSkipMode(ViChn, VI_SKIP_YES);
		}
		else
			FY_MPI_VI_SetSkipMode(ViChn, VI_SKIP_NONE);
        s32Ret = SAMPLE_COMM_VI_StartChn(ViChn,&(psViCnfInfo->ViChnInfo));
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("call SAMPLE_COMM_VI_StarChn failed with %#x\n", s32Ret);
            return FY_FAILURE;
        } 
    }

    return FY_SUCCESS;
}
/*****************************************************************************
* function : stop vi accroding to product type
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_Stop(FY_U32 u32ViDevCnt)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    FY_S32 i;
    FY_S32 s32Ret;
    SAMPLE_VI_PARAM_S stViParam;

    s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
    if (FY_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return FY_FAILURE;
    }

    /*** Stop VI Chn ***/
    for(i=0;i<stViParam.s32ViChnCnt;i++)
    {
        /* Stop vi phy-chn */
        ViChn = i * stViParam.s32ViChnInterval;
        s32Ret = FY_MPI_VI_DisableChn(ViChn);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopChn failed with %#x\n",s32Ret);
            return FY_FAILURE;
        }
    }

    /*** Stop VI Dev ***/
    for(i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = FY_MPI_VI_DisableDev(ViDev);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopDev failed with %#x\n", s32Ret);
            return FY_FAILURE;
        }
    }

    return FY_SUCCESS;
}

/*****************************************************************************
* function : Vi chn bind vpss group
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_BindVpss(FY_U32 u32ViDevCnt)
{
    FY_S32 j, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN ViChn;

    s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
    if (FY_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return FY_FAILURE;
    }
    
    VpssGrp = 0;
    for (j=0; j<stViParam.s32ViChnCnt; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval;
        
        stSrcChn.enModId = FY_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;
    
        stDestChn.enModId = FY_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

		printf("the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }
        VpssGrp ++;
    }
    return FY_SUCCESS;
}

/*****************************************************************************
* function : Vi chn bind vpss group mixcap
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_BindVpss_MixCap(FY_U32 u32ViDevCnt,FY_U32 g_mix_flag,FY_U32 u32GrpNum)
{
    FY_S32 j, s32Ret,i;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN ViChn;
	FY_U32 bind_group_numb;

	if(g_mix_flag)
		bind_group_numb = u32GrpNum /2 ;
	else
		bind_group_numb = u32GrpNum;
	
    s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
    if (FY_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return FY_FAILURE;
    }
    
    VpssGrp = 0;
    for (j=0; j<stViParam.s32ViChnCnt; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval;
        
        stSrcChn.enModId = FY_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;
    
        stDestChn.enModId = FY_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

		printf("bind_group_numb is %d\n",bind_group_numb);

		//for(i=0;i<bind_group_numb;i++){
		for(i=bind_group_numb;i<bind_group_numb*2;i++)
		{
			stDestChn.s32DevId = VpssGrp+i;
			printf("the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
			s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
			if (s32Ret != FY_SUCCESS)
			{
				SAMPLE_PRT("failed with %#x!\n", s32Ret);
				return FY_FAILURE;
			}

		}

		if(g_mix_flag){
			stSrcChn.s32ChnId = SUBCHN(ViChn);

			//for(i=bind_group_numb;i<bind_group_numb*2;i++)
			for(i=0;i<bind_group_numb;i++)
			{
				stDestChn.s32DevId = VpssGrp+i;
				printf("the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
				s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
				if (s32Ret != FY_SUCCESS)
				{
					SAMPLE_PRT("failed with %#x!\n", s32Ret);
					return FY_FAILURE;
				}

			}
		}
        VpssGrp ++;
    }
    return FY_SUCCESS;
}


/*****************************************************************************
* function : Vi chn unbind vpss group
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_UnBindVpss(FY_U32 u32ViDevCnt)
{
    FY_S32 i, j, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_DEV ViDev;
    VI_CHN ViChn;

    s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
    if (FY_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return FY_FAILURE;
    }
    
    VpssGrp = 0;    
    for (i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;

        for (j=0; j<stViParam.s32ViChnCnt; j++)
        {
            ViChn = j * stViParam.s32ViChnInterval;
            
            stSrcChn.enModId = FY_ID_VIU;
            stSrcChn.s32DevId = ViDev;
            stSrcChn.s32ChnId = ViChn;
        
            stDestChn.enModId = FY_ID_VPSS;
            stDestChn.s32DevId = VpssGrp;
            stDestChn.s32ChnId = 0;

			s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
			if (s32Ret != FY_SUCCESS)
			{
				SAMPLE_PRT("failed with %#x!\n", s32Ret);
				return FY_FAILURE;
			}
            VpssGrp ++;
        }
    }
    return FY_SUCCESS;
}

/*****************************************************************************
* function : Vi chn unbind vpss group
*****************************************************************************/
FY_S32 SAMPLE_COMM_VI_UnBindVpss_MixCap(FY_U32 u32ViDevCnt,FY_U32 g_mix_flag,FY_U32 u32GrpNum)
	{
		FY_S32 i, j, s32Ret;
		VPSS_GRP VpssGrp;
		MPP_CHN_S stSrcChn;
		MPP_CHN_S stDestChn;
		SAMPLE_VI_PARAM_S stViParam;
		VI_DEV ViDev;
		VI_CHN ViChn;
		
		FY_U32 bind_group_numb;
		if(g_mix_flag)
			bind_group_numb = u32GrpNum /2 ;
		else
			bind_group_numb = u32GrpNum;
	
		s32Ret = SAMPLE_VI_GetChnInterval(u32ViDevCnt, &stViParam);
		if (FY_SUCCESS !=s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
			return FY_FAILURE;
		}
		
		VpssGrp = 0;	
		for (i=0; i<stViParam.s32ViDevCnt; i++)
		{
			ViDev = i * stViParam.s32ViDevInterval;
	
			for (j=0; j<stViParam.s32ViChnCnt; j++)
			{
				ViChn = j * stViParam.s32ViChnInterval;
				
				stSrcChn.enModId = FY_ID_VIU;
				stSrcChn.s32DevId = ViDev;
				stSrcChn.s32ChnId = ViChn;
			
				stDestChn.enModId = FY_ID_VPSS;
				stDestChn.s32DevId = VpssGrp;
				stDestChn.s32ChnId = 0;
			
				//for(i=0;i<bind_group_numb;i++){
				for(i=bind_group_numb;i<bind_group_numb*2;i++)
				{
					stDestChn.s32DevId = VpssGrp+i;
					printf("the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
					s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
					if (s32Ret != FY_SUCCESS)
					{
						SAMPLE_PRT("failed with %#x!\n", s32Ret);
						return FY_FAILURE;
					}
	
				}
	
				if(g_mix_flag){
					stSrcChn.s32ChnId = SUBCHN(ViChn);
	
					//for(i=bind_group_numb;i<bind_group_numb*2;i++)
					for(i=0;i<bind_group_numb;i++)
					{
						stDestChn.s32DevId = VpssGrp+i;
						printf("the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
						s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
						if (s32Ret != FY_SUCCESS)
						{
							SAMPLE_PRT("failed with %#x!\n", s32Ret);
							return FY_FAILURE;
						}
	
					}
				}
				VpssGrp ++;
			}
		}
		return FY_SUCCESS;
	}


/******************************************************************************
* function : read frame
******************************************************************************/
FY_VOID SAMPLE_COMM_VI_ReadFrame(FILE * fp, FY_U8 * pY, FY_U8 * pU, FY_U8 * pV, FY_U32 width, FY_U32 height, FY_U32 stride, FY_U32 stride2)
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
 
/******************************************************************************
* function : Plan to Semi
******************************************************************************/
FY_S32 SAMPLE_COMM_VI_PlanToSemi(FY_U8 *pY, FY_S32 yStride,
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

/******************************************************************************
* function : Get from YUV
******************************************************************************/
FY_S32 SAMPLE_COMM_VI_GetVFrameFromYUV(FILE *pYUVFile, FY_U32 u32Width, FY_U32 u32Height,FY_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
    FY_U32             u32LStride;
    FY_U32             u32CStride;
    FY_U32             u32LumaSize;
    FY_U32             u32ChrmSize;
    FY_U32             u32Size;
    VB_BLK VbBlk;
    FY_U32 u32PhyAddr;
    FY_U8 *pVirAddr;

    u32LStride  = u32Stride;
    u32CStride  = u32Stride;

    u32LumaSize = (u32LStride * u32Height);
    u32ChrmSize = (u32CStride * u32Height) >> 2;/* YUV 420 */
    u32Size = u32LumaSize + (u32ChrmSize << 1);

    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = FY_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
    if (VB_INVALID_HANDLE == VbBlk)
    {
        SAMPLE_PRT("FY_MPI_VB_GetBlock err! size:%d\n",u32Size);
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
    SAMPLE_PRT("pool id :%d, phyAddr:%x,virAddr:%x\n" ,pstVFrameInfo->u32PoolId,u32PhyAddr,(int)pVirAddr);

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
    pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;
    pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_INTERLACED;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */

    /* read Y U V data from file to the addr ----------------------------------------------*/
    SAMPLE_COMM_VI_ReadFrame(pYUVFile, pstVFrameInfo->stVFrame.pVirAddr[0],
       pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.pVirAddr[2],
       pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height,
       pstVFrameInfo->stVFrame.u32Stride[0], pstVFrameInfo->stVFrame.u32Stride[1] >> 1 );

    /* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
    SAMPLE_COMM_VI_PlanToSemi(pstVFrameInfo->stVFrame.pVirAddr[0], pstVFrameInfo->stVFrame.u32Stride[0],
      pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.pVirAddr[2], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height);

    FY_MPI_SYS_Munmap(pVirAddr, u32Size);
    return 0;
}

FY_S32 SAMPLE_COMM_VI_ChangeCapSize(VI_CHN ViChn, FY_U32 u32CapWidth, FY_U32 u32CapHeight,FY_U32 u32Width, FY_U32 u32Height)
{
    VI_CHN_ATTR_S stChnAttr;
    FY_S32 S32Ret = FY_SUCCESS;
    S32Ret = FY_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
    if(FY_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "FY_MPI_VI_GetChnAttr failed\n");
    }
    stChnAttr.stCapRect.u32Width = u32CapWidth;
    stChnAttr.stCapRect.u32Height = u32CapHeight;
    stChnAttr.stDestSize.u32Width = u32Width;
    stChnAttr.stDestSize.u32Height = u32Height;

    S32Ret = FY_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if(FY_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "FY_MPI_VI_SetChnAttr failed\n");
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VI_ChangeDestSize(VI_CHN ViChn, FY_U32 u32Width, FY_U32 u32Height)
{
    VI_CHN_ATTR_S stChnAttr;
    FY_S32 S32Ret = FY_SUCCESS;
    S32Ret = FY_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
    if(FY_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "FY_MPI_VI_GetChnAttr failed\n");
    }
    stChnAttr.stDestSize.u32Width = u32Width;
    stChnAttr.stDestSize.u32Height = u32Height;

    S32Ret = FY_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if(FY_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "FY_MPI_VI_SetChnAttr failed\n");
    }

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VI_ChangeMixCap(VI_CHN ViChn,FY_BOOL bMixCap,FY_U32 FrameRate)
{
    VI_CHN_ATTR_S stChnAttr,stChnMinorAttr;
    FY_S32 S32Ret = FY_SUCCESS;
    S32Ret = FY_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
    if(FY_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "FY_MPI_VI_GetChnAttr failed");
    }

    if(FY_TRUE == bMixCap)
    {
        memcpy(&stChnMinorAttr, &stChnAttr, sizeof(VI_CHN_ATTR_S));
        stChnMinorAttr.stDestSize.u32Width = D1_WIDTH / 2;

        stChnAttr.s32DstFrameRate = FrameRate;

        S32Ret = FY_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
        if (FY_SUCCESS != S32Ret)
        {
            SAMPLE_PRT("call FY_MPI_VI_SetChnAttr failed with %#x\n", S32Ret);
            return FY_FAILURE;
        } 
        S32Ret = FY_MPI_VI_SetChnMinorAttr(ViChn, &stChnMinorAttr);
        if (FY_SUCCESS != S32Ret)
        {
            SAMPLE_PRT("call FY_MPI_VI_SetChnMinorAttr failed with %#x\n", S32Ret);
            return FY_FAILURE;
        } 
    }
    else
    {
        stChnAttr.s32DstFrameRate = stChnAttr.s32SrcFrameRate;
        S32Ret = FY_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
        if (FY_SUCCESS != S32Ret)
        {
            SAMPLE_PRT("call FY_MPI_VI_SetChnAttr failed with %#x\n", S32Ret);
            return FY_FAILURE;
        } 
    }
    return FY_SUCCESS;
}



FY_S32 SAMPLE_COMM_VI_Dump(VI_CHN ViChn, FY_U32 u32Cnt,stViCnfInfo *psViCnfInfo)
{
    FY_S32 i;
    //VIDEO_FRAME_INFO_S stFrame;
    VIDEO_FRAME_INFO_S astFrame;
    FY_CHAR szYuvName[128];
    FY_CHAR szPixFrm[20];
    FILE *pfd[2];
    FY_S32 s32MilliSec = 2000;
	FY_U32 depth = 2,u32Width=0,u32Height=0;
    //FY_CHAR filename[40];

	pfd[0]=NULL;
	pfd[1]=NULL;

	if(u32Cnt == 0)
		return FY_SUCCESS;
    if (FY_MPI_VI_SetFrameDepth(ViChn, depth))
    {
        printf("FY_MPI_VI_SetFrameDepth err, vi chn %d \n", ViChn);
        return -1;
    }

    /* make file name */
    if(PIXEL_FORMAT_YUV_SEMIPLANAR_420 == psViCnfInfo->ViChnInfo.stChnAttr.enPixFormat)
    {
        strcpy(szPixFrm,"sp420");
    }
    else if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == psViCnfInfo->ViChnInfo.stChnAttr.enPixFormat)
    {
        strcpy(szPixFrm,"sp422");
    }
    else if (PIXEL_FORMAT_YUV_400 == psViCnfInfo->ViChnInfo.stChnAttr.enPixFormat)
    {
         strcpy(szPixFrm,"single");
    }
    else 
    {

    }
	u32Width = psViCnfInfo->ViChnInfo.stChnAttr.stDestSize.u32Width;
	u32Height = psViCnfInfo->ViChnInfo.stChnAttr.stDestSize.u32Height;
	
    if(VI_SCAN_INTERLACED == psViCnfInfo->ViChnInfo.stChnAttr.enScanMode)
		u32Height = u32Height*2;
    sprintf(szYuvName, "./vi_chn_main_%d_%dx%d_%s_%d.yuv", ViChn,
        u32Width, u32Height,szPixFrm,u32Cnt);
	printf("Dump YUV frame of vi chn %d  to file: width = %d, height = %d \n", ViChn, 
        u32Width, u32Height);
    
    /* open file */
    pfd[0] = fopen(szYuvName, "wb");
    if (NULL == pfd[0])
    {
        return -1;
    }
	if(psViCnfInfo->ViChnInfo.bMinor){
	    sprintf(szYuvName, "./vi_chn_minor_%d_%dx%d_%s_%d.yuv", ViChn,
	        psViCnfInfo->ViChnInfo.u32MDstWidth, psViCnfInfo->ViChnInfo.u32MDstHeight,szPixFrm,u32Cnt);
		printf("Dump YUV frame of vi chn %d  to file: width = %d, height = %d \n", ViChn, 
	        psViCnfInfo->ViChnInfo.u32MDstWidth, psViCnfInfo->ViChnInfo.u32MDstHeight);
	    
	    /* open file */
	    pfd[1] = fopen(szYuvName, "wb");
	    if (NULL == pfd[1])
	    {
	        return -1;
	    }

	}

    /* get VI frame  */
    for (i=0; i<u32Cnt; i++)
    {
        if (FY_MPI_VI_GetFrame(ViChn, &astFrame, s32MilliSec) != FY_SUCCESS)
        {
            printf("get vi chn %d frame err\n", ViChn);
            printf("only get %d frame\n", i);
            break;
        }
		SAMPLE_COMM_YUV_DUMP(&astFrame.stVFrame, pfd[0]);

        /* release frame after using */
        FY_MPI_VI_ReleaseFrame(ViChn, &astFrame);

		if(psViCnfInfo->ViChnInfo.bMinor){
			//Get Minor data
	        if (FY_MPI_VI_GetFrame(SUBCHN(ViChn), &astFrame, s32MilliSec)  != FY_SUCCESS)
	        {
	            printf("get vi chn %d frame err\n", ViChn);
	            printf("only get %d frame\n", i);
	            break;
	        }
			SAMPLE_COMM_YUV_DUMP(&astFrame.stVFrame, pfd[1]);
		        /* release frame after using */
	        FY_MPI_VI_ReleaseFrame(SUBCHN(ViChn), &astFrame);
		}
    }
	FY_MPI_VI_SetFrameDepth(ViChn, 0);
	chmod(szYuvName,0777);

	if(pfd[0])
	    fclose(pfd[0]);
	if(pfd[1])
		fclose(pfd[1]);

	return 0;
}
	

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
