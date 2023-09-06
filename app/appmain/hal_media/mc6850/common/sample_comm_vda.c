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
#include "fy_comm_vda.h"
#include "mpi_vda.h"


typedef struct _VDA_ID_MAN
{
	FY_BOOL bUse;
	FY_U32 u32Chn;
	FY_S32 s32Grp;
}VDA_ID_MAN;

typedef struct _MD_CD_E
{
	FY_BOOL bInit;
	VDA_ID_MAN uMD[MAX_MD_CHANNEL_NUM];
	VDA_ID_MAN uCD[MAX_MD_CHANNEL_NUM];
}MD_CD_E;

MD_CD_E g_VDA_PARAM;

#define CHECK_IF_ID_VALID(x) \
	do{\
		if(x >= MAX_MD_CHANNEL_NUM)\
		{\
			printf("id is invalid!!!\n");\
			return FY_FAILURE;\
		}\
	}while(0)

#define CHECK_IF_PARAM_INITED \
	do{\
		if(!g_VDA_PARAM.bInit)\
		{\
			memset(&g_VDA_PARAM,0,sizeof(MD_CD_E));\
			g_VDA_PARAM.bInit=FY_TRUE;\
		}\
	}while(0)

/******************************************************************************
* funciton : stop vda, and stop vda thread -- MD
******************************************************************************/
FY_VOID SAMPLE_COMM_VDA_MdStop(VPSS_GRP VpssGrp, FY_U32 u32Chn)
{
	FY_U32 i;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_VDA_PARAM.uMD[i].bUse && g_VDA_PARAM.uMD[i].s32Grp==VpssGrp &&g_VDA_PARAM.uMD[i].u32Chn==u32Chn)
		{
			MPI_VDA_MD_DeInit(i);
			break;
		}
	}
}

/******************************************************************************
* funciton : stop vda, and stop vda thread -- CD
******************************************************************************/
FY_VOID SAMPLE_COMM_VDA_CdStop(VPSS_GRP VpssGrp, FY_U32 u32Chn)
{
	FY_U32 i;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_VDA_PARAM.uCD[i].bUse && g_VDA_PARAM.uCD[i].s32Grp==VpssGrp &&g_VDA_PARAM.uCD[i].u32Chn==u32Chn)
		{
			MPI_VDA_CD_DeInit(i);
			break;
		}
	}
}


/******************************************************************************
* funciton : start vda MD mode
******************************************************************************/
FY_S32 SAMPLE_COMM_VDA_MdStart(VPSS_GRP VdaGrp, VPSS_CHN u32Chn, SIZE_S *pstSize)
{
	FY_VDA_INIT_cfg param;
	FY_VDA_MD_CFG_t cfg;
	FY_U32 i,notUseID=MAX_MD_CHANNEL_NUM;

	CHECK_IF_PARAM_INITED;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(!g_VDA_PARAM.uMD[i].bUse && notUseID==MAX_MD_CHANNEL_NUM)
		{
			notUseID=i;
		}
		if(g_VDA_PARAM.uMD[i].bUse && g_VDA_PARAM.uMD[i].s32Grp==VdaGrp &&g_VDA_PARAM.uMD[i].u32Chn==u32Chn)
		{
			break;
		}
	}

	if(i==MAX_MD_CHANNEL_NUM && notUseID!=MAX_MD_CHANNEL_NUM)
	{
		g_VDA_PARAM.uMD[notUseID].s32Grp = VdaGrp;
		g_VDA_PARAM.uMD[notUseID].u32Chn = u32Chn;
		g_VDA_PARAM.uMD[notUseID].bUse = FY_TRUE;
	}
	CHECK_IF_ID_VALID(notUseID);
	/* step 1: init vda md param */
	param.ID=notUseID;
	param.maxChnNum=MAX_MD_CHANNEL_NUM;
	param.VpssGrp=VdaGrp;
	param.VpssChn=u32Chn;
	param.threadMode=DRV_THREAD_MODE;
	param.outputMode=DS_ONE_EIGHTH;
	param.bOpenLog=FY_FALSE;
	memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
	MPI_VDA_MD_Init(&param);
	cfg.ID=notUseID;
	cfg.enable=FY_TRUE;
	cfg.framedelay=3;
	cfg.threshold=30;
	cfg.resultNum=1;
	cfg.checkMode=VDA_CHECK_FRAME;
	MPI_VDA_MD_SetConfig(&cfg);

    return FY_SUCCESS;
}
/******************************************************************************
* funciton : start vda CD mode
******************************************************************************/
FY_S32 SAMPLE_COMM_VDA_CdStart(VPSS_GRP VdaGrp, VPSS_CHN u32Chn, SIZE_S *pstSize)
{
    FY_VDA_INIT_cfg param;
	FY_VDA_CD_INFO_t cfg;
	FY_U32 i,notUseID=MAX_MD_CHANNEL_NUM;

	CHECK_IF_PARAM_INITED;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(!g_VDA_PARAM.uCD[i].bUse && notUseID==MAX_MD_CHANNEL_NUM)
		{
			notUseID=i;
		}
		if(g_VDA_PARAM.uCD[i].bUse && g_VDA_PARAM.uCD[i].s32Grp==VdaGrp &&g_VDA_PARAM.uCD[i].u32Chn==u32Chn)
		{
			break;
		}
	}

	if(i==MAX_MD_CHANNEL_NUM && notUseID!=MAX_MD_CHANNEL_NUM)
	{
		g_VDA_PARAM.uCD[notUseID].s32Grp = VdaGrp;
		g_VDA_PARAM.uCD[notUseID].u32Chn = u32Chn;
		g_VDA_PARAM.uCD[notUseID].bUse = FY_TRUE;
	}
	CHECK_IF_ID_VALID(notUseID);
	/* step 1: init vda md param */
	param.ID=notUseID;
	param.maxChnNum=MAX_MD_CHANNEL_NUM;
	param.VpssGrp=VdaGrp;
	param.VpssChn=u32Chn;
	param.threadMode=DRV_THREAD_MODE;
	param.outputMode=DS_ONE_EIGHTH;
	param.bOpenLog=FY_FALSE;
	memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
	MPI_VDA_CD_Init(&param);

	cfg.ID=notUseID;
	cfg.enable=FY_TRUE;
	cfg.framedelay=5;
	cfg.level=CD_LEVEL_LOW;
	MPI_VDA_CD_Update_BG(notUseID,VdaGrp,u32Chn);
	MPI_VDA_CD_SetConfig(&cfg);

    return FY_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

