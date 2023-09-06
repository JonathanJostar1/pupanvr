#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <sys/prctl.h>
#include "param_config.h"
#include "mpi_venc.h"
#include "sample_comm.h"
#include "sample_vpu.h"

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

#define SAMPLE_DEBUG(fmt...)   \
    if(g_bJpegeEnableLog) \
    { \
        do {\
            printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
            printf(fmt);\
           }while(0);\
    }

#define ALIGN_UP(x, a)              ((x+a-1)&(~(a-1)))

typedef struct
{
    thread_state_e enThreadStat;
}jpege_auto_test_s;

typedef struct
{
   SIZE_S stSize;
}jpege_switch_param_s;

static SIZE_S g_astSize[]={
    {1920,1440},
    {1920,1088},
    {1280,1952},
    {1280,720},
    {960,1088},
    {352,288},
    {704,576},
};

#define RES_TEST_NUM      (sizeof(g_astSize)/sizeof(SIZE_S))

static jpege_auto_test_s gs_stJpegeAutoTest={TEST_THREAD_STOP};
static pthread_t gs_JpegeAutoTestPid;
static jpege_switch_param_s gs_astTestCase[RES_TEST_NUM];
static FY_U32  g_u32JpegeTestChnNum = 1;
static FY_U32  g_u32JpegeSnapNum = 1;
static char g_u32JpegeStoragePath[512]="/record/";
static FY_U32  g_bJpegeNvrDemo = FY_FALSE;
static FY_U32  g_bJpegeEnableLog = FY_FALSE;

FY_U32 TEST_JPEGE_GenOutFileName(char *path, VENC_TEST_PARA_S *pstJpegeParams)
{
    char acOutPath[MAX_FILE_NAME_LEN];
    FY_U32 u32DirLen=0;
    char *pOutFilePath = path;

    memset(acOutPath, 0, MAX_FILE_NAME_LEN);

    if(access(g_u32JpegeStoragePath, F_OK) == 0)
    {
        sprintf(acOutPath, g_u32JpegeStoragePath);
        u32DirLen = strlen(g_u32JpegeStoragePath);
    }

    sprintf(acOutPath+u32DirLen, "jpeg");

    if(access(acOutPath, F_OK) != 0)
    {   // mkdir
        if(mkdir(acOutPath, 0777)==-1)
        {
            SAMPLE_PRT("mkdir '%s' folder failed, pls check directory permession\n", acOutPath);
            goto ERROR_EXIT;
        }
    }

	chmod(acOutPath, 0777);

    sprintf(pOutFilePath, "%s/ch%d_%dx%d_0.jpg",
        acOutPath,
        pstJpegeParams->u32ChanId,
        pstJpegeParams->stPicSize.u32Width, pstJpegeParams->stPicSize.u32Height);

	chmod(pOutFilePath, 0777);

    SAMPLE_DEBUG("=====Output file path: %s\n\n", pOutFilePath);
    return FY_SUCCESS;

ERROR_EXIT:
    return FY_FAILURE;
}

FY_VOID sample_jpege_gen_test_params()
{
    FY_U32 res;
    jpege_switch_param_s *pTestCase=gs_astTestCase;

    for(res=0; res<RES_TEST_NUM; res++)
    {
        pTestCase->stSize.u32Width  = g_astSize[res].u32Width;
        pTestCase->stSize.u32Height = g_astSize[res].u32Height;
        pTestCase++;
    }

    return;
}

FY_S32 sample_jpege_get_default_params(VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32ChanNum, FY_BOOL bNvrDemo)
{
    FY_U32 ch;
    VI_CHN_STAT_S stStat;
    VENC_TEST_PARA_S *pstChnPara=pastJpegeTestPara;

    if(pstChnPara == NULL)
    {
        SAMPLE_PRT("pstChnPara ptr is NULL!\n");
        return FY_FAILURE;
    }

    for(ch=0; ch<u32ChanNum; ch++)
    {
        pstChnPara->u32ChanId           = ch;

        if(!bNvrDemo && FY_SUCCESS == FY_MPI_VI_Query((ch)%VIU_MAX_CHN_NUM, &stStat)) 
        { // vi->jpege 0->0, 1->1, 2->2, 3->3, 0->4, 1->5, 2->6, 3->7
            pstChnPara->stPicSize.u32Width  = stStat.u32PicWidth; 
            pstChnPara->stPicSize.u32Height = stStat.u32PicHeight;   
            SAMPLE_PRT("jpege ch%d: get vi(ch%d) wxh = %dx%d\n", ch, ch%VIU_MAX_CHN_NUM, stStat.u32PicWidth, stStat.u32PicHeight);

            //vpu odd group max output res: 1920x1088 
            if(pstChnPara->stPicSize.u32Width > 1920)
                pstChnPara->stPicSize.u32Width = 1920;
            
            if(pstChnPara->stPicSize.u32Height > 1088)
                pstChnPara->stPicSize.u32Height = 1088;            
        }
        else
        {
            pstChnPara->stPicSize.u32Width  = 960; 
            pstChnPara->stPicSize.u32Height = 1088; 
        }
        
        pstChnPara->u32SrcFrmRate       = 15;  // frame rate
        pstChnPara->fr32DstFrmRate      = 15;  // frame rate

        if(bNvrDemo)
        {
            pstChnPara->u32VpuGrpId    = 1;
            pstChnPara->u32VpuChanId    = ch;
        }
        else
        {
            /* =====================
            vpu max group number is 8
            jpeg  ---- vpu
            chn0,4     grp1,chn0
            chn1,5     grp3,chn0
            chn2,6     grp5,chn0
            chn3,7     grp7,chn0
            ========================*/
            pstChnPara->u32VpuGrpId     = (ch>=VIU_MAX_CHN_NUM)?((ch-VIU_MAX_CHN_NUM)*2+1):(ch*2+1);
            pstChnPara->u32VpuChanId    = 0; //if width>=960, chanid must be 0
        }

        pstChnPara++;
    }

    return FY_SUCCESS;
}

FY_S32 sample_jpege_switch_params(VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32ChanNum, FY_U32 idx)
{
    jpege_switch_param_s *pTestCase=&gs_astTestCase[idx];
    VENC_TEST_PARA_S *pstChnPara=pastJpegeTestPara;
    FY_U32 ch;

    if(pstChnPara == NULL)
    {
        SAMPLE_PRT("pstChnPara ptr is NULL!\n");
        return FY_FAILURE;
    }

    //only change main stream params
    for(ch=0; ch<u32ChanNum; ch++)
    {
        pstChnPara->stPicSize.u32Width  = pTestCase->stSize.u32Width;
        pstChnPara->stPicSize.u32Height = pTestCase->stSize.u32Height;

        pstChnPara++;
    }

    SAMPLE_DEBUG("====switch test params====, idx=%d!\n", idx);
    return FY_SUCCESS;
}

FY_S32 sample_jpege_set_vpss_param(VENC_TEST_PARA_S *pstChnParams)
{
    FY_S32 s32Ret = FY_SUCCESS;
	VPSS_CHN_MODE_S stVpssChnMode;

    SAMPLE_DEBUG("pstChnParams->u32VpuGrpId=%d, pstChnParams->u32VpuChanId=%d\n", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
	s32Ret = FY_MPI_VPSS_GetChnMode(pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, &stVpssChnMode);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get Vpss grp%d chn%d mode failed!\n", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
        return s32Ret;
    }

	stVpssChnMode.enChnMode = VPSS_CHN_MODE_USER;
	stVpssChnMode.u32Width  = pstChnParams->stPicSize.u32Width;
	stVpssChnMode.u32Height = pstChnParams->stPicSize.u32Height;
	stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;//COMPRESS_MODE_TILE_224;
    stVpssChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stVpssChnMode.mainCfg.bBgmYEn = FY_FALSE;
    stVpssChnMode.stFrameRate.s32SrcFrmRate = 15;
    stVpssChnMode.stFrameRate.s32DstFrmRate = 2;

	s32Ret = FY_MPI_VPSS_SetChnMode(pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, &stVpssChnMode);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set Vpss grp%d chn%d mode failed!\n", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
        return s32Ret;
    }

    return s32Ret;
}

FY_S32 sample_jpege_set_vgs_param(VENC_TEST_PARA_S *pstChnParams)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VGS_CHN_PARA_S stVgsMode;
    
    s32Ret = FY_MPI_VGS_GetChnMode(pstChnParams->u32VpuChanId, 0, &stVgsMode);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get VGS chn%d mode failed!\n", pstChnParams->u32VpuChanId);
        return s32Ret;
    }
    SAMPLE_DEBUG("Get VGS ch%d path0 u32Width=%d, u32Height=%d, enChnMode=%d\n",
        pstChnParams->u32VpuChanId, stVgsMode.u32Width, stVgsMode.u32Height, stVgsMode.enChnMode);

    stVgsMode.u32Width  = pstChnParams->stPicSize.u32Width;
    stVgsMode.u32Height = pstChnParams->stPicSize.u32Height;
    stVgsMode.enChnMode = VGS_CHN_MODE_USER;

    s32Ret = FY_MPI_VGS_SetChnMode(pstChnParams->u32VpuChanId, 1, &stVgsMode);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get VGS chn%d mode failed!\n", pstChnParams->u32VpuChanId);
        return s32Ret;
    }
    SAMPLE_DEBUG("Set VGS ch%d path1 u32Width=%d, u32Height=%d, enChnMode=%d\n",
        pstChnParams->u32VpuChanId, stVgsMode.u32Width, stVgsMode.u32Height, stVgsMode.enChnMode);

    return s32Ret;
}

FY_S32 sample_jpege_start(VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32ChanNum, FY_BOOL bAutoTest, FY_BOOL bNvrDemo)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VENC_CHN JpegeChn;
    VENC_TEST_PARA_S *pstChnParams;
    FY_U32 i;
    FY_CHAR ch = '\0';

    /******************************************
     step 3: start veu and vpss bind jpeg
    ******************************************/
    pstChnParams = pastJpegeTestPara;

    for(i = 0; i<u32ChanNum; i++)
    {
        JpegeChn = i;

        if(bNvrDemo)
        {
            s32Ret = sample_jpege_set_vgs_param(pstChnParams);
            if(FY_SUCCESS != s32Ret)
            {
                goto ENCODE_EXIT1;
            }            
        }
        else if(JpegeChn<VIU_MAX_CHN_NUM)
        {
            s32Ret = sample_jpege_set_vpss_param(pstChnParams);
            if(FY_SUCCESS != s32Ret)
            {
                goto ENCODE_EXIT1;
            }
        }


        s32Ret = SAMPLE_COMM_JPEGE_Start(JpegeChn, pstChnParams);
        if(FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_JPEGE_Start vechn[%d] failed with %#x!\n", JpegeChn, s32Ret);
            goto ENCODE_EXIT1;
        }

        s32Ret = TEST_JPEGE_GenOutFileName(pstChnParams->pOutfile, pstChnParams);
        if(FY_SUCCESS != s32Ret)
        {
            goto ENCODE_EXIT1;
        }

        pstChnParams++;
    }

    SAMPLE_COMM_JPEGE_AgingTest(FY_FALSE);
    if(bAutoTest)
    {
        while(TEST_THREAD_STOP != gs_stJpegeAutoTest.enThreadStat)
        {
    		s32Ret = SAMPLE_COMM_JPEGE_SnapProcess(u32ChanNum, pastJpegeTestPara,
                g_u32JpegeSnapNum, bNvrDemo?FY_ID_VGS:FY_ID_VPSS, &gs_stJpegeAutoTest.enThreadStat);
    		if (FY_SUCCESS != s32Ret)
    		{
    			SAMPLE_PRT("snap process failed!\n");
                //goto ENCODE_EXIT4;
                //while(TEST_THREAD_STOP != gs_stJpegeAutoTest.enThreadStat);
    		}
            sleep(1);

            if(TEST_THREAD_AUTOTEST == gs_stJpegeAutoTest.enThreadStat)
                break;
        }
    }
    else
    {
    	while (ch != 'q')
    	{
			s32Ret = SAMPLE_COMM_JPEGE_SnapProcess(u32ChanNum, pastJpegeTestPara, g_u32JpegeSnapNum, bNvrDemo?FY_ID_VGS:FY_ID_VPSS, &gs_stJpegeAutoTest.enThreadStat);
			if (FY_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("snap process failed!\n");
			}

    		printf("press 'q' to exit snap process!peress ENTER to capture picture to file\n");
    		if((ch = getchar()) == 'q')
    		{
    			break;
    		}
    	}
    }

    return s32Ret;

ENCODE_EXIT1:
    pstChnParams = pastJpegeTestPara;
    for(JpegeChn = 0; JpegeChn<u32ChanNum; JpegeChn++)
    {        
        SAMPLE_COMM_JPEGE_UnBindSrc(JpegeChn, bNvrDemo?FY_ID_VGS:FY_ID_VPSS, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
        SAMPLE_COMM_JPEGE_Stop(JpegeChn);
        pstChnParams++;
    }

    return s32Ret;
}

FY_S32 sample_jpege_stop(VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32ChanNum, FY_BOOL bNvrDemo)
{
    FY_U32 JpegeChn;
    VENC_TEST_PARA_S *pstChnParams = pastJpegeTestPara;

    if(pastJpegeTestPara == NULL)
    {
        SAMPLE_PRT("pastJpegeTestPara is NULL!\n");
        return FY_FAILURE;
    }

    for(JpegeChn = 0; JpegeChn<u32ChanNum; JpegeChn++)
    {
        SAMPLE_COMM_JPEGE_Stop(JpegeChn);
        SAMPLE_COMM_JPEGE_UnBindSrc(JpegeChn, bNvrDemo?FY_ID_VGS:FY_ID_VPSS, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);

        pstChnParams++;
    }

    return FY_SUCCESS;
}

FY_VOID *sample_jpege_autotest()
{
    FY_U32 idx=0;
    FY_U32 u32Cnt=g_u32JpegeTestChnNum; /*main+sub*/
    VENC_TEST_PARA_S *pastJpegeTestPara=NULL;
    FY_S32 s32Ret = FY_SUCCESS;

    prctl(PR_SET_NAME, "JpegAutoTest");

    pastJpegeTestPara = malloc(sizeof(VENC_TEST_PARA_S) * u32Cnt);
    if(pastJpegeTestPara == NULL)
    {
        SAMPLE_PRT("jpege test malloc mem failed!\n");
        return NULL;
    }

    memset(pastJpegeTestPara, 0, sizeof(VENC_TEST_PARA_S) * u32Cnt);
    sample_jpege_get_default_params(pastJpegeTestPara, u32Cnt, g_bJpegeNvrDemo);

    idx=0;
    while(1)
    {
        s32Ret = sample_jpege_start(pastJpegeTestPara, u32Cnt, FY_TRUE, g_bJpegeNvrDemo);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("start %d channels failed!\n", u32Cnt);
            goto EXIT;
        }

        sample_jpege_stop(pastJpegeTestPara, u32Cnt, g_bJpegeNvrDemo);
        if (TEST_THREAD_STOP == gs_stJpegeAutoTest.enThreadStat)
        {
            //stop test
            break;
        }

        sample_jpege_switch_params(pastJpegeTestPara, u32Cnt, idx);
        idx++;
        idx = (idx == RES_TEST_NUM)?0:idx;
    }

EXIT:
    free(pastJpegeTestPara);
    return NULL;
}

FY_S32 sample_jpege_test_start(FY_BOOL bNvrDemo)
{
    // generate test parames, total 48 test cases
    if(gs_stJpegeAutoTest.enThreadStat != TEST_THREAD_STOP)
    {
        return FY_SUCCESS;
    }

    sample_jpege_gen_test_params();
    g_bJpegeNvrDemo = bNvrDemo;

    // create test thread
    gs_stJpegeAutoTest.enThreadStat = TEST_THREAD_START;
    return pthread_create(&gs_JpegeAutoTestPid, 0, sample_jpege_autotest, (FY_VOID*)&gs_stJpegeAutoTest);
}


FY_S32 sample_jpege_test_stop()
{
    if (TEST_THREAD_STOP != gs_stJpegeAutoTest.enThreadStat)
    {
        gs_stJpegeAutoTest.enThreadStat = TEST_THREAD_STOP;
        if (gs_JpegeAutoTestPid)
        {
            pthread_join(gs_JpegeAutoTestPid, 0);
            gs_JpegeAutoTestPid = 0;
        }
    }

    return FY_SUCCESS;
}


FY_S32 sample_jpege_manual_test(FY_BOOL bNvrDemo)
{
    FY_U32 u32Cnt=g_u32JpegeTestChnNum; /*main+sub*/
    VENC_TEST_PARA_S *pastJpegeTestPara=NULL;
    FY_U32 u32Ret = FY_SUCCESS;

    pastJpegeTestPara = malloc(sizeof(VENC_TEST_PARA_S) * u32Cnt);
    if(pastJpegeTestPara == NULL)
    {
        SAMPLE_PRT("jpege test malloc mem failed!\n");
        return FY_FAILURE;
    }

    memset(pastJpegeTestPara, 0, sizeof(VENC_TEST_PARA_S) * u32Cnt);
    g_bJpegeNvrDemo = bNvrDemo;
    sample_jpege_get_default_params(pastJpegeTestPara, u32Cnt, bNvrDemo);

    u32Ret = sample_jpege_start(pastJpegeTestPara, u32Cnt, FY_FALSE, bNvrDemo);

    u32Ret = sample_jpege_stop(pastJpegeTestPara, u32Cnt, bNvrDemo);

    free(pastJpegeTestPara);
    return u32Ret;
}


FY_S32 sample_jpege_test_auto_switch()
{
    if (TEST_THREAD_STOP != gs_stJpegeAutoTest.enThreadStat)
    {
        gs_stJpegeAutoTest.enThreadStat = TEST_THREAD_AUTOTEST;
    }

    return FY_SUCCESS;
}

FY_S32 sample_jpege_test_switch_pause()
{
    if (TEST_THREAD_STOP != gs_stJpegeAutoTest.enThreadStat)
    {
        gs_stJpegeAutoTest.enThreadStat = TEST_THREAD_PAUSE;
    }

    return FY_SUCCESS;
}

FY_S32 sample_jpege_set_chn_num(FY_U32 u32ChnNum)
{
    if(u32ChnNum ==0 || u32ChnNum > 8)
    {
        SAMPLE_PRT("exceed max channel number, should be [1, 8]!\n");
        return FY_FAILURE;
    }

    g_u32JpegeTestChnNum = u32ChnNum;
    return FY_SUCCESS;
}

FY_S32 sample_jpege_get_chn_num(FY_U32 *pu32ChnNum)
{
    if(pu32ChnNum == NULL)
        return FY_FAILURE;

    *pu32ChnNum = g_u32JpegeTestChnNum;
    return FY_SUCCESS;
}

FY_S32 sample_jpege_set_snap_num(FY_U32 u32SnapNum)
{
    if(u32SnapNum ==0 || u32SnapNum > 100)
        return FY_FAILURE;

    g_u32JpegeSnapNum = u32SnapNum;
    return FY_SUCCESS;
}

FY_S32 sample_jpege_set_storage_path(char *path)
{
    if(!path)
    {
        SAMPLE_PRT("ERROR! record path is NULL\n");
        return FY_FAILURE;
    }

    if(access(path, F_OK) != 0)
    {
        SAMPLE_PRT("ERROR! record path [%s] not exist\n", path);
        return FY_FAILURE;
    }

    strcpy(g_u32JpegeStoragePath, path);
    return FY_SUCCESS;
}

FY_BOOL sample_jpege_is_started()
{
    return !(gs_stJpegeAutoTest.enThreadStat == TEST_THREAD_STOP);
}

FY_VOID sample_jpege_enable_log(FY_BOOL bEnableLog)
{
    g_bJpegeEnableLog = bEnableLog;
    return;
}

