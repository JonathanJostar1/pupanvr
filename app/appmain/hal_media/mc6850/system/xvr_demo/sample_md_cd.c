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

#include "fy_comm_vda.h"
#include "mpi_vda.h"
#include "mpi_vpss.h"

typedef struct _MD_CD_Param
{
	FY_BOOL bUse;
	FY_U32 u32Chn;
	FY_S32 s32Grp;
	VPSS_CHN_MODE_S stOrigVpssMode;
}MD_CD_Param;

typedef struct _MD_CD_Type
{
	FY_BOOL bInit;
	MD_CD_Param uMD[MAX_MD_CHANNEL_NUM];
	MD_CD_Param uCD[MAX_MD_CHANNEL_NUM];
}MD_CD_Type;

MD_CD_Type g_MotionDetection;

#define CHECK_MD_ID_VALID(x) \
	do{\
		if(x >= MAX_MD_CHANNEL_NUM)\
		{\
			printf("id is invalid!!!\n");\
			return FY_FAILURE;\
		}\
	}while(0)

#define CHECK_MD_PARAM_INITED \
	do{\
		if(!g_MotionDetection.bInit)\
		{\
			memset(&g_MotionDetection,0,sizeof(MD_CD_Type));\
			g_MotionDetection.bInit=FY_TRUE;\
		}\
	}while(0)

static pthread_t g_md_cd_thrd=-1;
static pthread_mutex_t g_md_cd_Mutex;
static FY_BOOL isNeedPrtLog=FY_TRUE;
FY_BOOL isThrdExit=FY_FALSE;
static FY_U8 g_prt_type=0;

FY_S32 SAMPLE_Set_ChnMode(FY_U32 type,FY_U32 id,FY_S32 vpssGrp,SIZE_S* pSize);


/******************************************************************************
* funciton : stop vda, and stop vda thread -- MD
******************************************************************************/
FY_VOID SAMPLE_VDA_MdStop(VPSS_GRP VpssGrp, FY_U32 u32Chn)
{
	FY_U32 i;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_MotionDetection.uMD[i].bUse && g_MotionDetection.uMD[i].s32Grp==VpssGrp &&g_MotionDetection.uMD[i].u32Chn==u32Chn)
		{
			MPI_VDA_MD_DeInit(i);
			FY_MPI_VPSS_SetChnMode(g_MotionDetection.uMD[i].s32Grp,VPSS_CHN0,&g_MotionDetection.uMD[i].stOrigVpssMode);
			memset(&g_MotionDetection.uMD[i],0,sizeof(MD_CD_Param));
			break;
		}
	}
}

FY_VOID SAMPLE_VDA_MdStopAll()
{
	FY_U32 i;

	pthread_mutex_lock(&g_md_cd_Mutex);
	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_MotionDetection.uMD[i].bUse)
		{
			MPI_VDA_MD_DeInit(i);
			FY_MPI_VPSS_SetChnMode(g_MotionDetection.uMD[i].s32Grp,VPSS_CHN0,&g_MotionDetection.uMD[i].stOrigVpssMode);
		}
	}
	memset(g_MotionDetection.uMD,0,sizeof(MD_CD_Param)*MAX_MD_CHANNEL_NUM);
	pthread_mutex_unlock(&g_md_cd_Mutex);
}


/******************************************************************************
* funciton : stop vda, and stop vda thread -- CD
******************************************************************************/
FY_VOID SAMPLE_VDA_CdStop(VPSS_GRP VpssGrp, FY_U32 u32Chn)
{
	FY_U32 i;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_MotionDetection.uCD[i].bUse && g_MotionDetection.uCD[i].s32Grp==VpssGrp &&g_MotionDetection.uCD[i].u32Chn==u32Chn)
		{
			MPI_VDA_CD_DeInit(i);
			FY_MPI_VPSS_SetChnMode(g_MotionDetection.uCD[i].s32Grp,VPSS_CHN0,&g_MotionDetection.uCD[i].stOrigVpssMode);
			memset(&g_MotionDetection.uCD[i],0,sizeof(MD_CD_Param));
			break;
		}
	}
}

FY_VOID SAMPLE_VDA_CdStopAll()
{
	FY_U32 i;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(g_MotionDetection.uCD[i].bUse)
		{
			MPI_VDA_CD_DeInit(i);
			FY_MPI_VPSS_SetChnMode(g_MotionDetection.uCD[i].s32Grp,VPSS_CHN0,&g_MotionDetection.uCD[i].stOrigVpssMode);
		}
	}
	memset(g_MotionDetection.uCD,0,sizeof(MD_CD_Param)*MAX_MD_CHANNEL_NUM);
}

/******************************************************************************
* funciton : start vda MD mode
******************************************************************************/
FY_S32 SAMPLE_VDA_MdStart(VPSS_GRP VdaGrp, VPSS_CHN u32Chn, SIZE_S *pstSize,FY_BOOL bOpenLog)
{
	FY_VDA_INIT_cfg param;
	FY_VDA_MD_CFG_t cfg;
	FY_U32 i,notUseID=MAX_MD_CHANNEL_NUM;
	SIZE_S pic_size;

	CHECK_MD_PARAM_INITED;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(!g_MotionDetection.uMD[i].bUse && notUseID==MAX_MD_CHANNEL_NUM)
		{
			notUseID=i;
		}
		if(g_MotionDetection.uMD[i].bUse && g_MotionDetection.uMD[i].s32Grp==VdaGrp &&g_MotionDetection.uMD[i].u32Chn==u32Chn)
		{
			break;
		}
	}

	if(i==MAX_MD_CHANNEL_NUM && notUseID!=MAX_MD_CHANNEL_NUM)
	{
		g_MotionDetection.uMD[notUseID].s32Grp = VdaGrp;
		g_MotionDetection.uMD[notUseID].u32Chn = u32Chn;
		g_MotionDetection.uMD[notUseID].bUse = FY_TRUE;
	}
	CHECK_MD_ID_VALID(notUseID);

	pic_size.u32Width=944;
	pic_size.u32Height=1088;
	SAMPLE_Set_ChnMode(VDA_TYPE_MD,notUseID,VdaGrp,&pic_size);

	/* step 1: init vda md param */
	param.ID=notUseID;
	param.maxChnNum=MAX_MD_CHANNEL_NUM;
	param.VpssGrp=VdaGrp;
	param.VpssChn=u32Chn;
	param.threadMode=DRV_THREAD_MODE;
	param.outputMode=DS_ONE_SIXTEENTH;
	param.bOpenLog=bOpenLog;
	memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
	MPI_VDA_MD_Init(&param);
	cfg.ID=notUseID;
	cfg.enable=FY_FALSE;
	cfg.framedelay=1;
	cfg.threshold=45;
	cfg.resultNum=1;
	cfg.checkMode=VDA_CHECK_FRAME;
	cfg.stype=MD_FRM_DIFF_ALL;
	MPI_VDA_MD_SetConfig(&cfg);
	memset(&cfg,0,sizeof(FY_VDA_MD_CFG_t));
	cfg.ID=notUseID;
	MPI_VDA_MD_GetConfig(&cfg);
	cfg.enable=FY_TRUE;
	MPI_VDA_MD_SetConfig(&cfg);

    return FY_SUCCESS;
}

FY_S32 SAMPLE_VDA_MdStartAll(FY_U32 uChnCnt,SIZE_S *pstSize,FY_BOOL bOpenLog)
{
	FY_VDA_INIT_cfg param;
	FY_VDA_MD_CFG_t cfg;
	FY_U32 i;
	SIZE_S pic_size;
	FY_S32 sGrpId[MAX_MD_CHANNEL_NUM]={3,2,1,0,4,5,6,7};
	FY_U32 uChnId[MAX_MD_CHANNEL_NUM]={VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0};

	CHECK_MD_PARAM_INITED;

	for(i=0;i<uChnCnt;i++)
	{
		if(!g_MotionDetection.uMD[i].bUse)
		{
			g_MotionDetection.uMD[i].s32Grp = sGrpId[i];
			g_MotionDetection.uMD[i].u32Chn = uChnId[i];
			g_MotionDetection.uMD[i].bUse = FY_TRUE;

			pic_size.u32Width=960;
			pic_size.u32Height=540;
			SAMPLE_Set_ChnMode(VDA_TYPE_MD,i,sGrpId[i],&pic_size);

			/* step 1: init vda md param */
			param.ID=i;
			param.maxChnNum=MAX_MD_CHANNEL_NUM;
			param.VpssGrp=sGrpId[i];
			param.VpssChn=uChnId[i];
			param.threadMode=DRV_THREAD_MODE;
			param.outputMode=DS_ONE_SIXTEENTH;
			param.bOpenLog=bOpenLog;
			memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
			MPI_VDA_MD_Init(&param);
			cfg.ID=i;
			cfg.enable=FY_FALSE;
			cfg.framedelay=1;
			cfg.threshold=45;
			cfg.resultNum=1;
			cfg.checkMode=VDA_CHECK_FRAME;
			cfg.stype=MD_FRM_DIFF_ONLY;
			MPI_VDA_MD_SetConfig(&cfg);
			memset(&cfg,0,sizeof(FY_VDA_MD_CFG_t));
			cfg.ID=i;
			MPI_VDA_MD_GetConfig(&cfg);
			cfg.enable=FY_TRUE;
			MPI_VDA_MD_SetConfig(&cfg);
		}
	}

    return FY_SUCCESS;
}

/******************************************************************************
* funciton : start vda CD mode
******************************************************************************/
FY_S32 SAMPLE_VDA_CdStart(VPSS_GRP VdaGrp, VPSS_CHN u32Chn, SIZE_S *pstSize,FY_BOOL bOpenLog)
{
    FY_VDA_INIT_cfg param;
	FY_VDA_CD_INFO_t cfg;
	FY_U32 i,notUseID=MAX_MD_CHANNEL_NUM;
	SIZE_S pic_size;

	CHECK_MD_PARAM_INITED;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(!g_MotionDetection.uCD[i].bUse && notUseID==MAX_MD_CHANNEL_NUM)
		{
			notUseID=i;
		}
		if(g_MotionDetection.uCD[i].bUse && g_MotionDetection.uCD[i].s32Grp==VdaGrp &&g_MotionDetection.uCD[i].u32Chn==u32Chn)
		{
			break;
		}
	}

	if(i==MAX_MD_CHANNEL_NUM && notUseID!=MAX_MD_CHANNEL_NUM)
	{
		g_MotionDetection.uCD[notUseID].s32Grp = VdaGrp;
		g_MotionDetection.uCD[notUseID].u32Chn = u32Chn;
		g_MotionDetection.uCD[notUseID].bUse = FY_TRUE;
	}
	CHECK_MD_ID_VALID(notUseID);

	pic_size.u32Width=960;
	pic_size.u32Height=540;
	SAMPLE_Set_ChnMode(VDA_TYPE_CD,notUseID,VdaGrp,&pic_size);

	/* step 1: init vda md param */
	param.ID=notUseID;
	param.maxChnNum=MAX_MD_CHANNEL_NUM;
	param.VpssGrp=VdaGrp;
	param.VpssChn=u32Chn;
	param.threadMode=DRV_THREAD_MODE;
	param.outputMode=DS_ONE_SIXTEENTH;
	param.bOpenLog=bOpenLog;
	memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
	MPI_VDA_CD_Init(&param);

	cfg.ID=notUseID;
	cfg.enable=FY_TRUE;
	cfg.framedelay=1;
	cfg.level=CD_LEVEL_LOW;
	MPI_VDA_CD_Update_BG(notUseID,VdaGrp,u32Chn);
	MPI_VDA_CD_SetConfig(&cfg);

    return FY_SUCCESS;
}

FY_S32 SAMPLE_VDA_CdStartAll(SIZE_S *pstSize,FY_BOOL bOpenLog)
{
    FY_VDA_INIT_cfg param;
	FY_VDA_CD_INFO_t cfg;
	FY_U32 i;
	SIZE_S pic_size;
	FY_S32 sGrpId[MAX_MD_CHANNEL_NUM]={3,4,5,0,1,2,6,7};
	FY_U32 uChnId[MAX_MD_CHANNEL_NUM]={VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0,VPSS_CHN0};

	CHECK_MD_PARAM_INITED;

	for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
	{
		if(!g_MotionDetection.uCD[i].bUse)
		{
			g_MotionDetection.uCD[i].s32Grp = sGrpId[i];
			g_MotionDetection.uCD[i].u32Chn = uChnId[i];
			g_MotionDetection.uCD[i].bUse = FY_TRUE;

			pic_size.u32Width=960;
			pic_size.u32Height=540;
			SAMPLE_Set_ChnMode(VDA_TYPE_CD,i,sGrpId[i],&pic_size);

			/* step 1: init vda md param */
			param.ID=i;
			param.maxChnNum=MAX_MD_CHANNEL_NUM;
			param.VpssGrp=sGrpId[i];
			param.VpssChn=uChnId[i];
			param.threadMode=DRV_THREAD_MODE;
			param.outputMode=DS_ONE_SIXTEENTH;
			param.bOpenLog=bOpenLog;
			memcpy(&param.mdSize,pstSize,sizeof(SIZE_S));
			MPI_VDA_CD_Init(&param);

			cfg.ID=i;
			cfg.enable=FY_TRUE;
			cfg.framedelay=1;
			cfg.level=CD_LEVEL_HIGH;
			MPI_VDA_CD_SetConfig(&cfg);
			MPI_VDA_CD_Update_BG(i,sGrpId[i],uChnId[i]);
		}
	}

    return FY_SUCCESS;
}


FY_S32 SAMPLE_Set_ChnMode(FY_U32 type,FY_U32 id,FY_S32 vpssGrp,SIZE_S* pSize)
{
	VPSS_CHN_MODE_S stVpssMode={0};

	//FY_MPI_VPSS_Set_LogLevel(7);
	memset(&stVpssMode,0,sizeof(VPSS_CHN_MODE_S));
	switch(type)
	{
		case VDA_TYPE_MD:
			if (FY_MPI_VPSS_GetChnMode(vpssGrp,VPSS_CHN0,&g_MotionDetection.uMD[id].stOrigVpssMode) != FY_SUCCESS)
		    {
		    	printf("md get mode error!!!\n");
		        return FY_FAILURE;
		    }
			printf("[MD_CD_TEST] md yc mode=%d\n",g_MotionDetection.uMD[id].stOrigVpssMode.mainCfg.ycmeanMode);
			memcpy(&stVpssMode,&g_MotionDetection.uMD[id].stOrigVpssMode,sizeof(VPSS_CHN_MODE_S));
			break;
		case VDA_TYPE_CD:
			if (FY_MPI_VPSS_GetChnMode(vpssGrp,VPSS_CHN0,&g_MotionDetection.uCD[id].stOrigVpssMode) != FY_SUCCESS)
		    {
		    	printf("cd get mode error!!!\n");
		        return FY_FAILURE;
		    }
			printf("[MD_CD_TEST] cd yc mode=%d\n",g_MotionDetection.uCD[id].stOrigVpssMode.mainCfg.ycmeanMode);
			memcpy(&stVpssMode,&g_MotionDetection.uCD[id].stOrigVpssMode,sizeof(VPSS_CHN_MODE_S));
			break;
		default:
		break;
	}

	printf("[VDA]w=%d,h=%d\n",stVpssMode.u32Width,stVpssMode.u32Height);

    if (FY_MPI_VPSS_DisableChn(vpssGrp,VPSS_CHN0) != FY_SUCCESS)
    {
    	printf("disable error!!!\n");
        return FY_FAILURE;
    }

	//if(stVpssMode.stFrameRate.s32SrcFrmRate==-1 && stVpssMode.stFrameRate.s32DstFrmRate==-1)
	{
		stVpssMode.stFrameRate.s32SrcFrmRate=25;
		stVpssMode.stFrameRate.s32DstFrmRate=15;
		stVpssMode.enCompressMode = COMPRESS_MODE_TILE_224;//COMPRESS_MODE_NONE;
		stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;//PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    	stVpssMode.bDouble = FY_FALSE;
	}

	if(stVpssMode.u32Width==0 && stVpssMode.u32Height==0)
	{
	    stVpssMode.u32Width = pSize->u32Width;
	    stVpssMode.u32Height = pSize->u32Height;
		if(stVpssMode.enChnMode != VPSS_CHN_MODE_USER)
		{
			stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
		}
	}

	if(!stVpssMode.mainCfg.bYcMeanEn)
	{
		stVpssMode.mainCfg.bYcMeanEn = 1;
	    stVpssMode.mainCfg.ycmeanMode =  DS_ONE_SIXTEENTH;
		stVpssMode.YcMeanSel = 2;
	}

	if (FY_MPI_VPSS_SetChnMode(vpssGrp,VPSS_CHN0,&stVpssMode) != FY_SUCCESS)
    {
    	printf("set mode error!!!\n");
        return FY_FAILURE;
    }

	if (FY_MPI_VPSS_GetChnMode(vpssGrp,VPSS_CHN0,&stVpssMode) != FY_SUCCESS)
    {
    	printf("get mode error!!!\n");
        return FY_FAILURE;
    }

    if (FY_MPI_VPSS_EnableChn(vpssGrp,VPSS_CHN0) != FY_SUCCESS)
    {
    	printf("enable error!!!\n");
        return FY_FAILURE;
    }
	//FY_MPI_VPSS_Set_LogLevel(3);
	return FY_SUCCESS;
}

FY_S32 SAMPLE_GET_VpssGrp()
{
	FY_BOOL isExit=FY_FALSE;
	int vpssGrp=0;
	char c = 0;

	while(1)
	{
		printf("\n\n=============================================================\n\n");
		printf("Option[0 ~ VPSS_MAX_GRP_NUM]:\n\n");
		printf("	0~256:	vpss grp number\n\n");
		printf("==============================================================\n\n");
		printf("\033[1;31;44m Enter your choice [0~VPSS_MAX_GRP_NUM]: \033[0m");
		scanf("%d",&vpssGrp);
		printf("SAMPLE_GET_VpssGrp=%d\n",vpssGrp);
		if(vpssGrp>=0 && vpssGrp<=VPSS_MAX_GRP_NUM)
		{
			isExit=FY_TRUE;
		}
		if(isExit)
		{
			break;
		}
	}
	while ( (c = getchar()) != '\n' && c != EOF ) ;

	return vpssGrp;
}

FY_S32 SAMPLE_GET_VdaChnCount()
{
	FY_BOOL isExit=FY_FALSE;
	int ChnCount=0;
	char c = 0;

	while(1)
	{
		printf("\n\n=============================================================\n\n");
		printf("Option[1 ~ 8]:\n\n");
		printf("	1~8:	vda channel count\n\n");
		printf("==============================================================\n\n");
		printf("\033[1;31;44m Enter your choice [1~8]: \033[0m");
		scanf("%d",&ChnCount);
		printf("SAMPLE_GET_VdaChnCount=%d\n",ChnCount);
		if(ChnCount>0 && ChnCount<=MAX_MD_CHANNEL_NUM)
		{
			isExit=FY_TRUE;
		}
		if(isExit)
		{
			break;
		}
	}
	while ( (c = getchar()) != '\n' && c != EOF ) ;

	return ChnCount;
}


FY_VOID *SAMPLE_prt_callback(FY_VOID *pdata)
{
	FY_U8 i=0;

	while(1)
	{
		if(isThrdExit)
		{
			pthread_exit(NULL);
			pthread_mutex_destroy(&g_md_cd_Mutex);
			break;
		}
		if(isNeedPrtLog)
		{
			pthread_mutex_lock(&g_md_cd_Mutex);
			for(i=0;i<MAX_MD_CHANNEL_NUM;i++)
			{
				if(g_MotionDetection.uMD[i].bUse)
				{
					printf("\033[32m####################[MD]grp=%d####################\033[0m\n",g_MotionDetection.uMD[i].s32Grp);
					MPI_VDA_MD_Print(i,2);
				}
				if(g_MotionDetection.uCD[i].bUse)
				{
					FY_U32 result;
					MPI_VDA_CD_GetResult(i, &result);
					printf("\033[31m[CD-%d][result]=%d \033[0m\n\n",i+1,result);
				}
			}
			pthread_mutex_unlock(&g_md_cd_Mutex);
			usleep(200*1000);
		}
		else
		{
			usleep(1000*1000);
		}
	}
}

FY_VOID SAMPLE_set_log_type()
{
	char option,ch1;
	FY_BOOL isExit=FY_FALSE;
	while(1)
	{
		printf("\n\n=============================================================\n\n");
		printf("Option[0 ~ 2]:\n\n");
		printf("  0:	PrintTwoDimDomin\n  1:	PrtPoint\n  2:	PrintDiffValue\n\n");
		printf("==============================================================\n\n");
		printf("\033[1;31;44m Enter your choice [0/1/2/q]: \033[0m");
		option = getchar();
		if(10 == option)
		{
			continue;
		}
		ch1 = getchar();
		if(10 != ch1)
		{
			while((ch1 = getchar()) != '\n' && ch1 != EOF);
			{
					;
			}
			continue;
		}
		switch(option)
		{
			case '0':
				g_prt_type=0;
				isExit=FY_TRUE;
				break;
			case '1':
				g_prt_type=1;
				isExit=FY_TRUE;
				break;
			case '2':
				g_prt_type=2;
				isExit=FY_TRUE;
				break;
			case 'q':
				isExit=FY_TRUE;
			default:
				isExit=FY_FALSE;
			break;
		}
		if(isExit)
		{
			break;
		}
	}
}


FY_S32 sample_md_start(FY_U32 cmd)
{
	SIZE_S pic_size;
	FY_S32 vpssGrp;
	FY_BOOL bOpenLog=FY_FALSE;
	FY_U32 uChnCnt=0;

	if(0)//(cmd!=5)
	{
		char option,ch1;
		FY_BOOL isExit=FY_FALSE;
		while(1)
		{
			printf("\n\n=============================================================\n\n");
			printf("Option[0 ~ 1]:\n\n");
			printf("    0:        close log \n    1:        open log\n\n");
			printf("==============================================================\n\n");
			printf("\033[1;31;44m Enter your choice [0/1/q]: \033[0m");
			option = getchar();
			if(10 == option)
			{
				continue;
			}
			ch1 = getchar();
			if(10 != ch1)
			{
				while((ch1 = getchar()) != '\n' && ch1 != EOF);
				{
						;
				}
				continue;
			}
			switch(option)
			{
				case '1':
					bOpenLog=FY_TRUE;
					isExit=FY_TRUE;
					break;
				case '0':
					bOpenLog=FY_FALSE;
					isExit=FY_TRUE;
					break;
				case 'q':
					goto exit;
				default:
					isExit=FY_FALSE;
				break;
			}
			if(isExit)
			{
				break;
			}
		}
	}
	FY_IOE_Open();
	pic_size.u32Width=352;
	pic_size.u32Height=288;
	if(cmd!=4 && cmd!=5)
	{
		SAMPLE_VDA_MdStopAll();
		SAMPLE_VDA_CdStopAll();
	}
	isThrdExit=FY_FALSE;
	if(g_md_cd_thrd == -1)
	{
		pthread_mutex_init(&g_md_cd_Mutex, 0);
		pthread_create(&g_md_cd_thrd,0, SAMPLE_prt_callback, FY_NULL);
	}
	switch(cmd)
	{
		case 1:
		{
			vpssGrp = SAMPLE_GET_VpssGrp();
			SAMPLE_VDA_MdStart(vpssGrp,VPSS_CHN0,&pic_size,bOpenLog);
		}
		break;
		case 2:
		{
			uChnCnt = SAMPLE_GET_VdaChnCount();
			SAMPLE_VDA_MdStartAll(uChnCnt,&pic_size, bOpenLog);
		}
		break;
		case 3:
		{
			printf("\033[32m#####stop md enter!#####\033[0m\n");
			pthread_mutex_lock(&g_md_cd_Mutex);
			if(g_md_cd_thrd != -1)
			{
				isThrdExit=FY_TRUE;
				//pthread_join(&g_md_cd_thrd,0);
				g_md_cd_thrd=-1;
			}
			FY_IOE_Close();
			pthread_mutex_unlock(&g_md_cd_Mutex);
			printf("\033[32m#####stop md exit!#####\033[0m\n");
		}
		break;
		case 4:
			isNeedPrtLog = !isNeedPrtLog;
			if(isNeedPrtLog)
			{
				printf("\033[32m#####Open MD Log!#####\033[0m\n");
			}
			else
			{
				printf("\033[32m#####Close MD Log!#####\033[0m\n");
			}
			break;
		case 5:
			SAMPLE_set_log_type();
			break;
		case 6:
		{
			vpssGrp = SAMPLE_GET_VpssGrp();
			SAMPLE_VDA_CdStart(vpssGrp,VPSS_CHN0,&pic_size,bOpenLog);
		}
		break;
		case 7:
		{
			SAMPLE_VDA_CdStartAll(&pic_size, bOpenLog);
		}
		break;
		default:
		goto exit;
	}
exit:
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


