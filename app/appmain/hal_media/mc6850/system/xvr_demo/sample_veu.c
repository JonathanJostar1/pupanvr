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
#include "sample_veu.h"
#include "sample_preview.h"

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

#define SAMPLE_DEBUG(fmt...)   \
        if(g_bVencEnableLog) \
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
}venc_auto_test_s;

typedef struct
{
   SIZE_S stSize;
   PAYLOAD_TYPE_E enType;
   SAMPLE_RC_E enRcMode;
   FY_BOOL bSmart;
   FY_U32 skip;
}venc_switch_param_s;

/*
5M: 2560x1944
4M  2560x1440
3M  1920x1440
H8M 1920x2160
H5M 1280x1944
H4M 1280x1440
*/
SIZE_S g_astSize[]={
//    {2560,1952}, /* 5M */
//    {2560,1440},
//    {1920,2160},
    {1920,1440},
    {1920,1088},
    {1280,1952},
    {1280,720},
    {960,1088},
    {352,288},
    {704,576},
};

PAYLOAD_TYPE_E g_aenType[]={
    PT_H265,
    PT_H264};

SAMPLE_RC_E g_aenRcMode[]={
    SAMPLE_RC_AVBR,
    SAMPLE_RC_VBR,
    SAMPLE_RC_CBR};

FY_BOOL g_aSmartEn[]={
    FY_TRUE,
    FY_FALSE};

FY_U32 g_aSkipNum[]={
    0,
    2,
    4};

FY_U32  g_u32VeuTestChnNum = 16;
FY_U32  g_u32GrpCnt = 8;
char g_u32VeuStoragePath[512]="/record/";
FY_U32  g_u32VeuTestFrameRate = 15;

#define RES_TEST_NUM      (sizeof(g_astSize)/sizeof(SIZE_S))
#define PTTYPE_TEST_NUM   (sizeof(g_aenType)/sizeof(PAYLOAD_TYPE_E))
#define RC_TEST_NUM       (sizeof(g_aenRcMode)/sizeof(SAMPLE_RC_E))
#define SMART_TEST_NUM       (sizeof(g_aSmartEn)/sizeof(FY_BOOL))
#define SKIP_TEST_NUM       (sizeof(g_aSkipNum)/sizeof(FY_U32))
#define TOTAL_TEST_NUM       (RES_TEST_NUM * PTTYPE_TEST_NUM * RC_TEST_NUM * SMART_TEST_NUM * SKIP_TEST_NUM)


static venc_auto_test_s gs_stVencAutoTest={TEST_THREAD_STOP};
static pthread_t gs_VencAutoTestPid;
static venc_switch_param_s gs_astTestCase[TOTAL_TEST_NUM];
extern FY_BOOL g_bAgingVencTest;
FY_BOOL g_bVencNvrDemo = FY_FALSE;
VENC_TEST_PARA_S *gp_stVencTestParams = NULL;
SIZE_S g_stVencPicSize={0,0};
static FY_U32  g_bVencEnableLog = FY_FALSE;

char *TEST_VENC_GenOutFileName(char *path, VENC_TEST_PARA_S *pstVencParams, SAMPLE_FILETYPE_E enFileType)
{
    char skip[128];
    char acOutPath[MAX_FILE_NAME_LEN];
    FY_U32 u32DirLen=0;
    char *pOutFilePath = path;

    memset(acOutPath, 0, MAX_FILE_NAME_LEN);

    if(access(g_u32VeuStoragePath, F_OK) == 0)
    {
        //
        sprintf(acOutPath, g_u32VeuStoragePath);
        u32DirLen = strlen(g_u32VeuStoragePath);
    }

    sprintf(acOutPath+u32DirLen, "record");

    if(access(acOutPath, F_OK) != 0)
    {   // mkdir ./record
        if(mkdir(acOutPath, 0777)==-1)
        {
            printf("mkdir '%s' folder failed\n", acOutPath);
            goto ERROR_EXIT;
        }
    }

    // mkdir normal/smart, path record/(single|skip)/(h264|h265)/(normal|smart)
    if(pstVencParams->bEnableSmart)
        sprintf(acOutPath+strlen(acOutPath), "/smart");
    else
        sprintf(acOutPath+strlen(acOutPath), "/normal");

    if(access(acOutPath, F_OK) != 0)
    {
        if(mkdir(acOutPath, 0777)==-1)
        {
            printf("mkdir '%s' folder failed\n", acOutPath);
            goto ERROR_EXIT;
        }
    }

    chmod(acOutPath, 0777);

    if(pstVencParams->u32Base == 0)
        sprintf(skip, "noskip");
    else
        sprintf(skip, "skip%d", pstVencParams->bEnablePred/*SvcN*/);


    sprintf(pOutFilePath, "%s/veu_ch%d_%dx%d_%s_%s_%s_%s_0.%s",
        acOutPath,
        pstVencParams->u32ChanId,
        pstVencParams->stPicSize.u32Width, pstVencParams->stPicSize.u32Height,
        TEST_VENC_GetRCString(pstVencParams->enRcMode),
        skip,
        (pstVencParams->bEnableSmart)?"smt":"norm",
        (pstVencParams->enType == PT_H264)?"264":"265",
        (enFileType == SAMPLE_FT_MP4)?"mp4":((pstVencParams->enType == PT_H264)?"264":"265"));

    chmod(pOutFilePath, 0777);

    SAMPLE_DEBUG("=====Output file path: %s\n\n", pOutFilePath);
    return NULL;

ERROR_EXIT:
    return NULL;
}

FY_VOID sample_encode_gen_test_params()
{
     FY_U32 res, type, rc, smart, skip;
    venc_switch_param_s *pTestCase=gs_astTestCase;

    for(res=0; res<RES_TEST_NUM; res++)
    {
        for(type=0; type<PTTYPE_TEST_NUM; type++)
        {
            for(rc=0; rc<RC_TEST_NUM; rc++)
            {
                for(smart=0; smart<SMART_TEST_NUM; smart++)
                {
                    for(skip=0; skip<SKIP_TEST_NUM; skip++)
                    {
                        pTestCase->stSize.u32Width  = g_astSize[res].u32Width;
                        pTestCase->stSize.u32Height = g_astSize[res].u32Height;
                        pTestCase->enType           = g_aenType[type];
                        pTestCase->enRcMode         = g_aenRcMode[rc];
                        pTestCase->bSmart           = g_aSmartEn[smart];
                        pTestCase->skip             = g_aSkipNum[skip];
                        pTestCase++;
                    }
                }
            }
        }
    }

    return;
}

FY_S32 sample_encode_get_default_params(VENC_TEST_PARA_S *pastVencTestPara, FY_U32 u32ChanNum, FY_BOOL bEnableSubstr)
{
    FY_U32 ch;
    FY_U32 step = bEnableSubstr?2:1;
    VENC_TEST_PARA_S *pstChnPara=pastVencTestPara;
    VI_CHN_STAT_S stStat;

    /*====dvr bind===
      vpu         veu
      grp0,chn0   chn0
      grp1,chn1   chn1
      grp2,chn0   chn2
      grp3,chn1   chn3
      grp4,chn0   chn4
      grp5,chn1   chn5
      grp6,chn0   chn6
      grp7,chn1   chn7
      ================*/

    /*====nvr bind===
      vpu         veu
      grp0,chn0   chn0
      grp1,chn0   chn1
      ================*/

    if(pstChnPara == NULL)
    {
        SAMPLE_PRT("pstChnPara ptr is NULL!\n");
        return FY_FAILURE;
    }

    for(ch=0; ch<u32ChanNum; ch+=step)
    {
        // main stream
        pstChnPara->u32ChanId           = ch;
        pstChnPara->enType              = PT_H265;
        pstChnPara->u32Profile          = 1; //main

        if(g_stVencPicSize.u32Width != 0 && g_stVencPicSize.u32Height != 0)
        {
            pstChnPara->stPicSize.u32Width  = g_stVencPicSize.u32Width;
            pstChnPara->stPicSize.u32Height = g_stVencPicSize.u32Height;
        }
        else if(!g_bVencNvrDemo && FY_SUCCESS == FY_MPI_VI_Query((ch/2)%VIU_MAX_CHN_NUM, &stStat))
        { // vi->veu 0->0, 1->2, 2->4, 3->6, 0->8, 1->10, 2->12, 3->14
            pstChnPara->stPicSize.u32Width  = (g_stVencPicSize.u32Width == 0)?stStat.u32PicWidth:g_stVencPicSize.u32Width;
            pstChnPara->stPicSize.u32Height = (g_stVencPicSize.u32Height == 0)?stStat.u32PicHeight:g_stVencPicSize.u32Height;
            SAMPLE_PRT("veu ch%d: get vi(ch%d) wxh = %dx%d\n", ch, (ch/2)%VIU_MAX_CHN_NUM, stStat.u32PicWidth, stStat.u32PicHeight);
        }
        else
        {
            pstChnPara->stPicSize.u32Width  = 960;
            pstChnPara->stPicSize.u32Height = 1088;
        }

        // configure frame rate
        if(pstChnPara->stPicSize.u32Width >= 2500)
        {
            pstChnPara->u32SrcFrmRate = 12;
            pstChnPara->fr32DstFrmRate = 6;
        }
        else if(!g_bVencNvrDemo && pstChnPara->stPicSize.u32Width == 1920 && pstChnPara->stPicSize.u32Height >= 1080)
        {
            pstChnPara->u32SrcFrmRate = 20;
            pstChnPara->fr32DstFrmRate = 10;
        }
        else
        {
            pstChnPara->u32SrcFrmRate       = g_u32VeuTestFrameRate;  // frame rate
            pstChnPara->fr32DstFrmRate      = g_u32VeuTestFrameRate;  // frame rate
        }

        pstChnPara->u32Gop              = 25;   //gop
        pstChnPara->enRcMode            = g_bVencNvrDemo?SAMPLE_RC_VBR:SAMPLE_RC_AVBR;  //rc mode
        pstChnPara->u32StatTime         = 1;
        pstChnPara->u32BitRate          = 1500;
        pstChnPara->u32MaxBitRate       = 1500;
        pstChnPara->u32MinQp            = 15;
        pstChnPara->u32MaxQp            = 50;

        pstChnPara->bEnableSmart    = 0;

        // skip
        pstChnPara->u32Base         = 0;
        pstChnPara->u32Enhance      = 0;
        pstChnPara->bEnablePred     = 0;
        pstChnPara->u32VpuGrpId     = ch%g_u32GrpCnt;

        pstChnPara->u32VpuChanId    = 0;
        pstChnPara++;

        if(bEnableSubstr)
        {
            //sub stream
            pstChnPara->u32ChanId           = ch+1;
            pstChnPara->enType              = PT_H264;
            pstChnPara->u32Profile          = 2; //high
            pstChnPara->stPicSize.u32Width  = 352;
            pstChnPara->stPicSize.u32Height = 288;
            pstChnPara->u32Gop              = 30;  //gop
            pstChnPara->enRcMode            = SAMPLE_RC_CBR;  //rc mode
            pstChnPara->u32StatTime         = 1;
            pstChnPara->u32SrcFrmRate       = 15; // frame rate
            pstChnPara->fr32DstFrmRate      = 15;
            pstChnPara->u32BitRate          = 512; //Kb
            pstChnPara->u32MaxBitRate       = 512;
            pstChnPara->u32MinQp            = 15;
            pstChnPara->u32MaxQp            = 40;
            // skip
            pstChnPara->u32Base         = 0;
            pstChnPara->u32Enhance      = 0;
            pstChnPara->bEnablePred     = 0;

            pstChnPara->u32VpuGrpId     = (ch+1)%g_u32GrpCnt;

            pstChnPara->u32VpuChanId    = 1;

            pstChnPara++;
        }
    }

    return FY_SUCCESS;
}

FY_S32 sample_encode_switch_params(VENC_TEST_PARA_S *pastVencTestPara, FY_U32 u32ChanNum, FY_U32 idx, FY_BOOL bEnableSubstr)
{
    venc_switch_param_s *pTestCase=&gs_astTestCase[idx];
    VENC_TEST_PARA_S *pstChnPara=pastVencTestPara;
    FY_U32 ch,u32TotalCh=bEnableSubstr?u32ChanNum/2:u32ChanNum;

    if(pstChnPara == NULL)
    {
        SAMPLE_PRT("pstChnPara ptr is NULL!\n");
        return FY_FAILURE;
    }

    //only change main stream params

    for(ch=0; ch<u32TotalCh; ch++)
    {
        pstChnPara->stPicSize.u32Width  = pTestCase->stSize.u32Width;
        pstChnPara->stPicSize.u32Height = pTestCase->stSize.u32Height;
        pstChnPara->enType              = pTestCase->enType;
        pstChnPara->enRcMode            = pTestCase->enRcMode;
        pstChnPara->bEnableSmart        = pTestCase->bSmart;

        if(pTestCase->skip == 0)
        {
            pstChnPara->u32Base         = 0;
            pstChnPara->u32Enhance      = 0;
            pstChnPara->bEnablePred     = 0;
        }
        else if(pTestCase->skip == 2)
        {
            pstChnPara->u32Base         = 1;
            pstChnPara->u32Enhance      = 1;
            pstChnPara->bEnablePred     = 2;
        }
        else if(pTestCase->skip == 4)
        {
            pstChnPara->u32Base         = 2;
            pstChnPara->u32Enhance      = 2;
            pstChnPara->bEnablePred     = 4;
        }

        pstChnPara++;
        if(bEnableSubstr)
            pstChnPara++;
    }

    SAMPLE_DEBUG("====switch test params====!\n");

#if 0
    VENC_TEST_PARA_S *pstChnPara=pastVencTestPara;
    printf("===========veu configure=========\n");
    printf("\ttype          = %s\n", (pstChnPara->enType == PT_H264)?"h264":"h265");
    printf("\tw/h           = %u/%u\n", pstChnPara->stPicSize.u32Width, pstChnPara->stPicSize.u32Height);
    printf("\tprofile       = %u  # 0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3];\n", pstChnPara->u32Profile);
    printf("\trc mode       = %u  # 0:CBR, 1:VBR, 2:AVBR, 3:QVBR, 4:FIXQP, 5:QPMAP\n", pstChnPara->enRcMode);
    printf("\tgop num       = %u\n", pstChnPara->u32Gop);
    printf("\tstat time     = %u\n", pstChnPara->u32StatTime);
    printf("\tsrc/dst rate  = %u/%u\n", pstChnPara->u32SrcFrmRate, pstChnPara->fr32DstFrmRate);
    printf("\tavg bitrate   = %u\n", pstChnPara->u32BitRate);
    printf("\tmax bitrate   = %u\n", pstChnPara->u32MaxBitRate);
    printf("\tmax qp        = %u\n", pstChnPara->u32MaxQp);
    printf("\tmin qp        = %u\n", pstChnPara->u32MinQp);
    printf("\tintra refresh = %u\n", pstChnPara->bIntraRefreshEn);
    printf("\tbase          = %d\n", pstChnPara->u32Base);
    printf("\tenhance       = %d\n", pstChnPara->u32Enhance);
    printf("\tpreden        = %d\n", pstChnPara->bEnablePred);
    printf("\tsmart        = %d\n",  pstChnPara->bEnableSmart);
    printf("\tvpu grpid/chnid  = %d/%d\n", pstChnPara->u32VpuGrpId, pstChnPara->u32VpuChanId);
    printf("===========veu configure end=========\n");
#endif
    return FY_SUCCESS;
}

FY_S32 sample_venc_start(VENC_TEST_PARA_S *pastVencTestPara, FY_U32 u32ChanNum)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VENC_CHN VencChn;
    VENC_TEST_PARA_S *pstChnParams;
    FY_U32  i;
    VPSS_CHN_MODE_S stVpssChnMode;
    FY_BOOL bBgmEn=FY_FALSE;

    /******************************************
     step 3: start veu and vpss bind veu
    ******************************************/
    pstChnParams = pastVencTestPara;
    for(i = 0; i<u32ChanNum; i++)
    {
        VencChn = i;

        if( pstChnParams->bEnableSmart||(pstChnParams->enRcMode == SAMPLE_RC_AVBR) )
            bBgmEn = FY_TRUE;
        else
            bBgmEn = FY_FALSE;

        if(i<g_u32GrpCnt)
        {
            SAMPLE_PRT("pstChnParams->u32VpuGrpId=%d, pstChnParams->u32VpuChanId=%d,chn=%d,g_u32GrpCnt=%d\n",pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId,i,g_u32GrpCnt);
            s32Ret = FY_MPI_VPSS_GetChnMode(pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, &stVpssChnMode);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("get Vpss grp%d chn%d mode failed!\n", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
                goto ENCODE_EXIT1;
            }

            stVpssChnMode.enChnMode = VPSS_CHN_MODE_USER;
            stVpssChnMode.u32Width  = pstChnParams->stPicSize.u32Width;
            stVpssChnMode.u32Height = pstChnParams->stPicSize.u32Height;
            stVpssChnMode.enCompressMode = COMPRESS_MODE_TILE_224;
            stVpssChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
            stVpssChnMode.mainCfg.bBgmYEn = bBgmEn;
            s32Ret = FY_MPI_VPSS_SetChnMode(pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, &stVpssChnMode);
            if (FY_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("set Vpss grp%d chn%d mode failed!\n", pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId);
                goto ENCODE_EXIT2;
            }

            if(!g_bVencNvrDemo && (pstChnParams->u32VpuGrpId%2 == 0))
                s32Ret = SAMPEL_VPSS_OVERLAY_Attach(pstChnParams->u32VpuGrpId,pstChnParams->u32VpuChanId, GOSD_DEFAULT_PIXEDEPTH,0);
        }

        s32Ret = SAMPLE_COMM_VENC_Start(VencChn, pstChnParams);
        if(FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_Start vechn[%d] failed with %#x!\n", VencChn, s32Ret);
            goto ENCODE_EXIT2;
        }


        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, bBgmEn);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto ENCODE_EXIT2;
        }

        if(!g_bAgingVencTest)
            TEST_VENC_GenOutFileName(pstChnParams->pOutfile, pstChnParams, SAMPLE_FT_MP4);
        pstChnParams++;
    }

    s32Ret = SAMPLE_COMM_VENC_StartGetStream(u32ChanNum, pastVencTestPara, SAMPLE_FT_MP4);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start get stream failed!\n");
        goto ENCODE_EXIT2;
    }

    return FY_SUCCESS;

ENCODE_EXIT2:
    pstChnParams = pastVencTestPara;
    for(VencChn = 0; VencChn<u32ChanNum; VencChn++)
    {
        SAMPLE_COMM_VENC_UnBindVpss(VencChn, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, bBgmEn);
        SAMPLE_COMM_VENC_Stop(VencChn);

        pstChnParams++;
    }

ENCODE_EXIT1:
    return s32Ret;
}

FY_S32 sample_venc_stop(VENC_TEST_PARA_S *pastVencTestPara, FY_U32 u32ChanNum)
{
    FY_U32 VencChn;
    VENC_TEST_PARA_S *pstChnParams = pastVencTestPara;

    if(pastVencTestPara == NULL)
    {
        SAMPLE_PRT("encode test pastVencTestPara is NULL!\n");
        return FY_FAILURE;
    }

    SAMPLE_COMM_VENC_StopGetStream();

    for(VencChn = 0; VencChn<u32ChanNum; VencChn++)
    {
        SAMPLE_COMM_VENC_UnBindVpss(VencChn, pstChnParams->u32VpuGrpId, pstChnParams->u32VpuChanId, pstChnParams->bEnableSmart||(pstChnParams->enRcMode == SAMPLE_RC_AVBR));
        SAMPLE_COMM_VENC_Stop(VencChn);

        pstChnParams++;
    }
    return FY_SUCCESS;

}



FY_VOID* sample_encode_autotest()
{
    FY_U32 idx=0, i,j;
    FY_U32 u32Cnt=g_u32VeuTestChnNum; /*main+sub*/
    VENC_TEST_PARA_S *pastVencTestPara=NULL;
    FY_BOOL bInvert = 0;
    FY_U32 u32GrpCnt = g_u32GrpCnt;
    FY_BOOL bEnableSubstr = g_bVencNvrDemo?FY_FALSE:FY_TRUE;
    FY_S32 s32Ret = FY_SUCCESS;

    prctl(PR_SET_NAME, "VeuAutoTest");

    g_u32GrpCnt = sample_get_grp_number();
    if(g_u32GrpCnt > 32)
       SAMPLE_PRT("vpu grp cnt [%d] exceed 16\n", g_u32GrpCnt);
    SAMPLE_PRT("vpu grp cnt [%d] \n", g_u32GrpCnt);

    if(gp_stVencTestParams == NULL)
    {
        pastVencTestPara = malloc(sizeof(VENC_TEST_PARA_S) * u32Cnt);
        if(pastVencTestPara == NULL)
        {
            SAMPLE_PRT("encode test malloc mem failed!\n");
            return NULL;
        }

        memset(pastVencTestPara, 0, sizeof(VENC_TEST_PARA_S) * u32Cnt);
        sample_encode_get_default_params(pastVencTestPara, u32Cnt, bEnableSubstr);
    }
    else
    {
        pastVencTestPara = gp_stVencTestParams;
    }

    SAMPLE_COMM_VENC_Init(NULL);

    SAMPLE_PRT("=====veu: total channel number %d, u32GrpCnt=%d\n", u32Cnt, u32GrpCnt);
    while(1)
    {
        s32Ret = sample_venc_start(pastVencTestPara, u32Cnt);
        if (FY_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("start veu %d channels failed!\n", u32Cnt);
            goto EXIT0;
        }

        for(i=0; i<5;)
        {
            if (TEST_THREAD_STOP == gs_stVencAutoTest.enThreadStat)
            {
                //stop test
                break;
            }

            if (TEST_THREAD_AUTOTEST == gs_stVencAutoTest.enThreadStat)
            {
                i++;
            }
            else
                ; //not switch params


            bInvert = !bInvert;

            for(j=0;j<u32GrpCnt; j++)
            {
                //SAMPEL_VPSS_OVERLAY_Attach(j,0, GOSD_DEFAULT_PIXEDEPTH,bInvert);
            }

            sleep(1);
        }

        sample_venc_stop(pastVencTestPara, u32Cnt);
        if (TEST_THREAD_STOP == gs_stVencAutoTest.enThreadStat)
        {
            //stop test
            break;
        }

        idx++;
        idx = (idx == TOTAL_TEST_NUM)?0:idx;

        sample_encode_switch_params(pastVencTestPara, u32Cnt, idx, bEnableSubstr);
    }

EXIT0:
    free(pastVencTestPara);
    gp_stVencTestParams = NULL;
    return NULL;
}

FY_S32 sample_encode_test_start(FY_BOOL bAgingTest, FY_BOOL bNvrDemo)
{
    // generate test parames, total 48 test cases
    if(gs_stVencAutoTest.enThreadStat != TEST_THREAD_STOP)
    {
        return FY_SUCCESS;
    }

    sample_encode_gen_test_params();
    g_bVencNvrDemo = bNvrDemo;

    // create test thread
    gs_stVencAutoTest.enThreadStat = TEST_THREAD_START;

    SAMPLE_COMM_VENC_AgingTest(bAgingTest);
    return pthread_create(&gs_VencAutoTestPid, 0, sample_encode_autotest, (FY_VOID*)&gs_stVencAutoTest);
}

FY_S32 sample_encode_test_stop()
{
    if (TEST_THREAD_STOP != gs_stVencAutoTest.enThreadStat)
    {
        gs_stVencAutoTest.enThreadStat = TEST_THREAD_STOP;
        if (gs_VencAutoTestPid)
        {
            pthread_join(gs_VencAutoTestPid, 0);
            gs_VencAutoTestPid = 0;
        }
    }

    g_bVencNvrDemo = FY_FALSE;
    g_bAgingVencTest = 0;
    gp_stVencTestParams = NULL;
    return FY_SUCCESS;
}

FY_S32 sample_encode_test_auto_switch()
{
    if (TEST_THREAD_STOP != gs_stVencAutoTest.enThreadStat)
    {
        gs_stVencAutoTest.enThreadStat = TEST_THREAD_AUTOTEST;
    }

    return FY_SUCCESS;
}

FY_S32 sample_encode_test_switch_pause()
{
    if (TEST_THREAD_STOP != gs_stVencAutoTest.enThreadStat)
    {
        gs_stVencAutoTest.enThreadStat = TEST_THREAD_PAUSE;
    }

    return FY_SUCCESS;
}

FY_S32 sample_encode_set_chn_num(FY_U32 u32ChnNum)
{
    if(u32ChnNum < 1 || u32ChnNum > VENC_MAX_TEST_CHAN_NUM) {
        printf("\n\tchannel number %d should be [1,%d]\n", u32ChnNum, VENC_MAX_TEST_CHAN_NUM);
        return FY_FAILURE;
    }

    g_u32VeuTestChnNum = u32ChnNum;
//    g_u32GrpCnt = u32ChnNum;

    return FY_SUCCESS;
}

FY_S32 sample_encode_get_chn_num(FY_U32 *pu32ChnNum)
{
    if(pu32ChnNum == NULL)
        return FY_FAILURE;

    *pu32ChnNum = g_u32VeuTestChnNum;
    return FY_SUCCESS;
}

FY_S32 sample_encode_set_storage_path(char *path)
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

    strcpy(g_u32VeuStoragePath, path);
    return FY_SUCCESS;
}

FY_S32 sample_encode_set_frame_rate(FY_U32 u32FrameRate)
{
    if(u32FrameRate ==0 || u32FrameRate > 100)
    {
        SAMPLE_PRT("ERROR! frame rate (%d) in not in range[1, 100]\n", u32FrameRate);
        return FY_FAILURE;
    }

    g_u32VeuTestFrameRate = u32FrameRate;

    return FY_SUCCESS;
}

FY_S32 sample_encode_config_chan(VENC_TEST_PARA_S *pChanConfig, FY_U32 u32ChnNum)
{
    VENC_TEST_PARA_S *pTestPara = gp_stVencTestParams;
    if(pChanConfig == NULL)
    {
        SAMPLE_PRT("ERROR! pParaConfig is NULL!\n");
        return FY_FAILURE;
    }

    if(pTestPara == NULL)
    {
        pTestPara = malloc(sizeof(VENC_TEST_PARA_S) * u32ChnNum);
        if(pTestPara == NULL)
        {
            SAMPLE_PRT("encode test malloc mem failed!\n");
            return FY_FAILURE;
        }
    }

    memcpy(pTestPara, pChanConfig, sizeof(VENC_TEST_PARA_S) * u32ChnNum);
    gp_stVencTestParams = pTestPara;
    g_u32VeuTestChnNum = u32ChnNum;

    return FY_SUCCESS;

}

FY_BOOL sample_encode_is_started()
{
    return !(gs_stVencAutoTest.enThreadStat == TEST_THREAD_STOP);
}

FY_S32 sample_encode_set_resolution( FY_U32 w, FY_U32 h)
{
    if(w > VENC_MAX_WIDTH || w < VENC_MIN_WIDTH)
    {
        SAMPLE_PRT("ERROR! width (%d) should be in [%d, %d]\n", w, VENC_MIN_WIDTH, VENC_MAX_WIDTH);
        return FY_FAILURE;
    }

    if(h > VENC_MAX_HEIGHT || h < VENC_MIN_HEIGHT)
    {
        SAMPLE_PRT("ERROR! height (%d) should be in [%d, %d]\n", w, VENC_MIN_HEIGHT, VENC_MAX_HEIGHT);
        return FY_FAILURE;
    }

    g_stVencPicSize.u32Width = w;
    g_stVencPicSize.u32Height = h;

    return FY_SUCCESS;
}

FY_VOID sample_encode_enable_log(FY_BOOL bEnableLog)
{
    g_bVencEnableLog = bEnableLog;
    return;
}


