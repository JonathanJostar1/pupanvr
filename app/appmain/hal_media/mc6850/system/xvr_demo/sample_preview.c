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
#include <time.h>
#include <sys/prctl.h>

#include "sample_comm.h"
#include "sample_preview.h"

#if(defined(MC6650)||defined(MC6850))
#define VPU_PREVIEW_CHN VPSS_CHN2
#else
#define VPU_PREVIEW_CHN VPSS_CHN3
#endif
FY_S32 s32VpssGrpCnt = 8;
FY_S32 g_bShowTime = 1;
static int g_preview_5M = 0;
static pthread_t g_preview_pid;
FY_S32 g_step =150;
FY_U32 g_s32Sx = 0,g_s32Sy=0,g_Vu32Width = 960,g_Vu32Height=540;
#define PREVIEW_MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define PREVIEW_MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct fysamplepreview
{
    FY_BOOL bThreadStart;
    FY_U32  s32Cnt;
    FY_BOOL bAutoVo;
    FY_BOOL bShow;
    FY_BOOL bAutoDelay;
    FY_BOOL bManuDelay;
    led_onoff led_cb;
}sample_preview_param;

sample_preview_param g_preview_param;
FY_S32  sample_preview_stop_show();


SAMPLE_VO_MODE_E g_enVoMode = VO_MODE_BUTT;
VO_INTF_SYNC_E   g_vpintf = VO_OUTPUT_1080P60;

static FY_BOOL g_vpuBindvou = FY_FALSE;
static FY_BOOL g_vpuStart   = FY_FALSE;

static pthread_mutex_t   g_GosdMutex = PTHREAD_MUTEX_INITIALIZER;

#define FY_GOSD_LOCK()  	 (void)pthread_mutex_lock(&g_GosdMutex);
#define FY_GOSD_UNLOCK()  	 (void)pthread_mutex_unlock(&g_GosdMutex);




FY_S32 test_vi_default_params(stViCnfInfo *pstViTestPara)
{
    FY_S32 s32Ret = FY_SUCCESS;
    SIZE_S stSize;

    /*=============================================
            1. configure vi paras
      =============================================*/
    memset(pstViTestPara, 0, sizeof(stViCnfInfo));

    stSize.u32Width = 1920;
    stSize.u32Height = 1080;

    pstViTestPara->stViInfo.u32DevNum      = 1;
    pstViTestPara->stViInfo.enNorm         = 0;
    pstViTestPara->stViInfo.u32CompRate    = 0;

    pstViTestPara->stViDevAttr.enIntfMode   = VI_MODE_BT656;
    pstViTestPara->stViDevAttr.enWorkMode   = VI_WORK_MODE_2Multiplex;
    pstViTestPara->stViDevAttr.enClkEdge    = VI_CLK_EDGE_DOUBLE;
    pstViTestPara->stViDevAttr.enDataSeq    = VI_INPUT_DATA_YVYU;

    pstViTestPara->ViChnInfo.stChnAttr.stCapRect.s32X         = 0;
    pstViTestPara->ViChnInfo.stChnAttr.stCapRect.s32Y         = 0;
    pstViTestPara->ViChnInfo.stChnAttr.stCapRect.u32Width     = stSize.u32Width;
    pstViTestPara->ViChnInfo.stChnAttr.stCapRect.u32Height    = stSize.u32Height;
    pstViTestPara->ViChnInfo.stChnAttr.stDestSize.u32Width    = stSize.u32Width;
    pstViTestPara->ViChnInfo.stChnAttr.stDestSize.u32Height   = stSize.u32Height;
    pstViTestPara->ViChnInfo.stChnAttr.enCapSel               = VI_CAPSEL_BOTH;
    pstViTestPara->ViChnInfo.stChnAttr.enScanMode             = VI_SCAN_PROGRESSIVE;
    pstViTestPara->ViChnInfo.stChnAttr.enPixFormat            = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    pstViTestPara->ViChnInfo.stChnAttr.s32SrcFrameRate        = 30;
    pstViTestPara->ViChnInfo.stChnAttr.s32DstFrameRate        = 30;
    pstViTestPara->ViChnInfo.stChnAttr.stSensorSize.u32Width  = stSize.u32Width;
    pstViTestPara->ViChnInfo.stChnAttr.stSensorSize.u32Height = stSize.u32Height;
    pstViTestPara->ViChnInfo.stChnAttr.enDataSeq              = VI_INPUT_DATA_UYVY;

    pstViTestPara->ViChnInfo.bMinor           = 0;
    pstViTestPara->ViChnInfo.u32MDstWidth     = stSize.u32Width/2;
    pstViTestPara->ViChnInfo.u32MDstHeight    = stSize.u32Height/2;
    pstViTestPara->ViChnInfo.s32MSrcFrameRate = 25;
    pstViTestPara->ViChnInfo.s32MDstFrameRate = 25;

    return s32Ret;
}

FY_U32 sample_start_vi()
{
    FY_S32 s32Ret = FY_SUCCESS;
    stViCnfInfo sViCnfInfo;
	memset(&sViCnfInfo,0,sizeof(stViCnfInfo));

	/**Init vi params**/
	test_vi_default_params(&sViCnfInfo);

    s32Ret = SAMPLE_COMM_VI_Start(1,&sViCnfInfo);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VI_Start ERR!");
EXIT:
    return s32Ret;
}

FY_VOID sample_preview_set_5M(int bUse5M)
{
    g_preview_5M = bUse5M;
}

FY_S32 sample_get_grp_number(FY_VOID)
{
    return s32VpssGrpCnt;
}

FY_S32 sample_preview_check(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    //FY_BOOL bLedOn = FY_TRUE;

    for(i=0;i<s32VpssGrpCnt;i++){
        if(sample_vpu_chn_bshow(s32VpssGrpCnt,i)){
            s32Ret |= sample_vpu_check_get_frame(i,VPSS_CHN2);
        }
    }
    return s32Ret;
}

FY_VOID sample_set_osd_showtime(FY_BOOL bFlag)
{
    g_bShowTime = bFlag;
}

FY_S32 sample_get_osd_showtime(FY_BOOL bFlag)
{
    return g_bShowTime;
}


FY_S32 sample_preview_add_osd(FY_BOOL invert)
{
    FY_U32 i = 0;
    for(i=0;i<s32VpssGrpCnt;i++){
        if(sample_vpu_chn_bshow(s32VpssGrpCnt,i)){
            if(g_bShowTime)
                SAMPEL_VPSS_OVERLAY_Attach(i,VPU_PREVIEW_CHN,GOSD_DEFAULT_PIXEDEPTH,invert);
            else
                SAMPEL_VPSS_OVERLAY_Attach_RGB1555(i,VPU_PREVIEW_CHN);
        }
    }
    return 0;
}

FY_S32 sample_preview_delete_osd()
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    for(i=0;i<s32VpssGrpCnt;i++){
        if(sample_vpu_chn_bshow(s32VpssGrpCnt,i)){
            s32Ret = SAMPEL_VPSS_OVERLAY_Detach(i,VPU_PREVIEW_CHN);
            if(FY_SUCCESS != s32Ret)
                SAMPLE_PREVIEW_PRT("SAMPEL_VPSS_OVERLAY_Detach IS ERROR!");
        }
    }
    return 0;
}

static FY_U32  g_bEnablePIP_PRE = FY_FALSE;

FY_VOID sample_PIP_enable_pre(FY_BOOL bEnablePIP)
{
    g_bEnablePIP_PRE = bEnablePIP;
    return;
}


FY_S32 sample_preview_bind()
{
    FY_S32 s32Ret = FY_SUCCESS;

    FY_U32 i = 0;
    VO_LAYER VoLayer;
    VO_CHN VoChn=0;

    VoLayer = SAMPLE_VO_LAYER_VHD0;

    for(i=0;i<s32VpssGrpCnt;i++) {
        if(sample_vpu_chn_bshow(s32VpssGrpCnt,i)){
            //printf("bind the vpu grp:%d, vo chn:%d\n",i,VoChn);
            s32Ret = SAMPLE_COMM_VO_BindVpss(VoLayer,VoChn,i,VPU_PREVIEW_CHN);
            SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_BindVpss ERR!");

			if (g_bEnablePIP_PRE) {   
				s32Ret = SAMPLE_COMM_VO_BindVpss(SAMPLE_VO_LAYER_VPIP,VoChn,i,VPSS_CHN3);
				SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_BindVpss ERR!");
			}

            VoChn++;
        }
    }
EXIT:
    return s32Ret;
}

FY_S32 sample_preview_unbind()
{
    FY_S32 s32Ret = FY_SUCCESS;

    FY_U32 i = 0;
    VO_LAYER VoLayer;
    VO_CHN VoChn=0;

    VoLayer = SAMPLE_VO_LAYER_VHD0;

    for(i=0;i<s32VpssGrpCnt;i++) {
        if(sample_vpu_chn_bshow(s32VpssGrpCnt,i)){
			
			s32Ret = SAMPLE_COMM_VO_UnBindVpss(SAMPLE_VO_LAYER_VPIP,VoChn,i,VPSS_CHN3);
			SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_BindVpss ERR!");

            //printf("unbind the vpu grp:%d, vo chn:%d\n",i,VoChn);
            s32Ret = SAMPLE_COMM_VO_UnBindVpss(VoLayer,VoChn,i,VPU_PREVIEW_CHN);
            SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_BindVpss ERR!");
            VoChn++;
        }
    }
EXIT:
    return s32Ret;
}

void * sample_preview_auto_proc(void *param)
{
    FY_U64 timeStart =0,osdTime =0,curTime = 0;
    FY_U32 invert = 0;
    FY_U32 enIntfSync[4];
    FY_U32 enVoMode[6];
    FY_U32 index = 0,enModeIndex=0;
    /*
	FY_U32 u32SxRand = 0, u32SyRand = 0,u32WidthRand = 0, u32HeightRand = 0;
    FY_S32 s32Sx = 0,s32Sy = 0;
    FY_U32 u32Width = 0,u32Height = 0;
    VPSS_CHN_ATTR_S stChnAttr;
    */
    enIntfSync[0] = VO_OUTPUT_1080P60;
    enIntfSync[1] = VO_OUTPUT_1280x1024_60;
    enIntfSync[2] = VO_OUTPUT_720P60;
    enIntfSync[3] = VO_OUTPUT_1024x768_60;

    enVoMode[0] = VO_MODE_1MUX;
    enVoMode[1] = VO_MODE_4MUX;
    enVoMode[2] = VO_MODE_9MUX;
    enVoMode[3] = VO_MODE_16MUX;
    enVoMode[4] = VO_MODE_1B_5S;
    enVoMode[5] = VO_MODE_1B_7S;

    prctl(PR_SET_NAME, "sample_preview_auto_proc");
    osdTime = timeStart = GetSysTimeBySec();
    while(g_preview_param.bThreadStart){
        if(1 == g_preview_param.bAutoVo){
            if(g_vpintf != enIntfSync[index]){
                sample_preview_stop_show();
                sample_vo_deinit(FY_TRUE);

                sample_vo_init(FY_TRUE, enIntfSync[index]);
                g_vpintf = enIntfSync[index];
                sample_preview_start(enVoMode[0],0);
            }
            enModeIndex = 0;
            while((enModeIndex < 6) && g_preview_param.bThreadStart){
                usleep(1000);
                curTime = GetSysTimeBySec();
                if((curTime - osdTime) >= 1){
                    invert = !invert;
                    FY_GOSD_LOCK();
                    if(g_preview_param.bAutoDelay){
                        usleep(50*1000);
                        g_preview_param.bAutoDelay = FY_FALSE;
                    }
                    sample_preview_add_osd(invert);
                    FY_GOSD_UNLOCK();
                    osdTime = curTime;
                }

                if((curTime - timeStart) >= 10){
                    enModeIndex++;
                    enModeIndex %= 6;
                    //sample_preview_delete_osd();
                    sample_preview_start(enVoMode[enModeIndex],1);
                    timeStart = curTime;
                    osdTime = curTime;
                }

            }
            index = (index+1)%4;
        }
        #if 0
        else if(2 == g_preview_param.bAutoVo){
            while(g_preview_param.bThreadStart && (2 == g_preview_param.bAutoVo)){
                srand(time(NULL));
                u32SxRand = 1000;
                u32SyRand = 900;
                u32WidthRand = 1920;
                u32HeightRand = 1080;

                s32Sx = ALIGN_BACK(rand()%(u32SxRand),4);
                s32Sy = ALIGN_BACK(rand()%(u32SyRand),4);

                while(1){
                    u32Width = ALIGN_UP(rand()%(u32WidthRand)+1,16);
                    u32Height = ALIGN_UP(rand()%(u32HeightRand)+1,16);
                    if(((s32Sx+u32Width)<=1920)&&((s32Sy+u32Height)<=1080))
                    break;
                }

                stChnAttr.cropInfo.bEnable = 1;
                stChnAttr.cropInfo.stRect.s32X = s32Sx;
                stChnAttr.cropInfo.stRect.s32Y = s32Sy;
                stChnAttr.cropInfo.stRect.u32Width = u32Width;
                stChnAttr.cropInfo.stRect.u32Height = u32Height;
                //printf("crop [%d,%d,%d,%d]\n",s32Sx,s32Sy,u32Width,u32Height);
                FY_MPI_VPSS_SetChnAttr(0,VPU_PREVIEW_CHN,&stChnAttr);
                usleep(3000);
            }
        }
        #endif
        else{
            if(g_preview_param.bShow){
                usleep(1000);
                curTime = GetSysTimeBySec();

                if((curTime - osdTime) >= 1){
                    invert = !invert;
                    FY_GOSD_LOCK();
                    if(g_preview_param.bManuDelay){
                        usleep(50*1000);
                        g_preview_param.bManuDelay = FY_FALSE;
                        //continue;
                    }
                    sample_preview_add_osd(invert);
                    FY_GOSD_UNLOCK();
                    osdTime = curTime;
                }
                /*
                if((curTime - ledTime) >= 10){
                    sample_preview_check();
                    ledTime = curTime;
                }
                */
            }
        }
    }

    return NULL;
}

FY_S32 sample_preview_start_autothread(FY_VOID)
{
    g_preview_param.bThreadStart = FY_TRUE;
    return pthread_create(&g_preview_pid, 0, sample_preview_auto_proc, (FY_VOID*)&g_preview_param);
}

FY_S32 sample_preview_stop_autothread(FY_VOID)
{
    if (FY_TRUE == g_preview_param.bThreadStart)
    {
        g_preview_param.bThreadStart = FY_FALSE;
        g_preview_param.bShow = FY_FALSE;
        if (g_preview_pid)
        {
            pthread_join(g_preview_pid, 0);
            g_preview_pid = 0;
        }
    }
    return FY_SUCCESS;
}

FY_S32 sample_preview_set_led_cb(led_onoff pLedOnoff)
{
    if(NULL == g_preview_param.led_cb)
        g_preview_param.led_cb = pLedOnoff;
    return 0;
}

FY_S32 sample_set_vpu_modparam(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_MOD_PARAM_S stVpssMod;
    memset(&stVpssMod,0,sizeof(VPSS_MOD_PARAM_S));
    FY_MPI_VPSS_GetModParam(&stVpssMod);

    stVpssMod.u32BgmNum = 17;
    stVpssMod.u32CpyNum = 0;
    stVpssMod.u32YMeanNum = 10;
    stVpssMod.u32MaxWidth = 4096;
    stVpssMod.u32MaxHeight = 2160;//1088;
    FY_MPI_VPSS_SetModParam(&stVpssMod);

    return s32Ret;
}

FY_S32 sample_preview_disable_osd()
{
    g_preview_param.bShow   = FY_FALSE;
    return FY_SUCCESS;
}

FY_S32 sample_preview_init(FY_U32 u32GrpCnt)
{

    VPSS_GRP_ATTR_S stGrpAttr;
    FY_S32 s32Ret = FY_SUCCESS;
    SAMPLE_VO_MODE_E enVoMode;

    s32VpssGrpCnt = u32GrpCnt;

    if(u32GrpCnt > 16) {
        enVoMode = VO_MODE_16MUX;
    } else if(u32GrpCnt > 8) {
        enVoMode = VO_MODE_1B_7S;
    } else {
        enVoMode = VO_MODE_4MUX;
    }
    //sample_start_vi();
	test_load_pic();	//also for encoding

    s32Ret = sample_set_vpu_modparam();
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_set_vpu_modparam ERR!");

    s32Ret = sample_vpu_init_param(&stGrpAttr);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_vpu_init_param ERR!");

    stGrpAttr.bNrEn       = 1;
    stGrpAttr.bYGammaEn   = 1;
    stGrpAttr.bChromaEn   = 1;
    stGrpAttr.bApcEn      = 1;
    stGrpAttr.bLcEn       = 1;

    if(g_preview_5M){
        stGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stGrpAttr.u32MaxW = 3840;
        stGrpAttr.bNrEn       = 0;
    }
    else
        stGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;

    s32Ret = sample_vpu_start(0,s32VpssGrpCnt, &stGrpAttr,1);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "sample_vpu_start ERR!");

    //s32Ret = sample_vi_bind_vpu_mix(s32VpssGrpCnt);
    s32Ret = sample_vi_bind_vpu(s32VpssGrpCnt);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "SAMPLE_COMM_VI_BindVpss_MixCap ERR!");

    sample_preview_start(enVoMode,0);
    return s32Ret;

ERR1:
    sample_vpu_stop(0,s32VpssGrpCnt,VPSS_MAX_CHN_NUM,1);
ERR0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

FY_S32 sample_preview_deinit(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;

    //sample_vi_unbind_vpu_mix(s32VpssGrpCnt);
    sample_vi_unbind_vpu(s32VpssGrpCnt);
    sample_preview_stop();
    sample_vpu_stop(0,s32VpssGrpCnt,VPSS_MAX_CHN_NUM,1);
    g_preview_param.led_cb = NULL;
    test_unload_pic();
    return s32Ret;
}


FY_S32  sample_preview_stop_show(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;

    sample_vpu_disable_preview(s32VpssGrpCnt, VPU_PREVIEW_CHN);

    if(VO_MODE_BUTT != g_enVoMode)
        sample_vo_stop_all(FY_TRUE, g_enVoMode);

    g_enVoMode = VO_MODE_BUTT;
    return s32Ret;
}

FY_S32  sample_preview_stop(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;

    sample_preview_stop_autothread();

    if(g_vpuStart)
    {
        sample_preview_stop_show();
        if(g_vpuBindvou){
            s32Ret = sample_preview_unbind();
            if(FY_SUCCESS != s32Ret)
                SAMPLE_PREVIEW_PRT("SAMPLE_COMM_VO_UnBindVpss ERR!");
            g_vpuBindvou = FY_FALSE;
        }
        g_vpuStart = FY_FALSE;
    }
    return s32Ret;
}

FY_S32 sample_preview_start(SAMPLE_VO_MODE_E enVoMode,FY_BOOL autoFlag)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_CHN_ATTR_S stChnAttr;

    if(!g_vpuBindvou){
        s32Ret = sample_preview_bind();
        SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "SAMPLE_COMM_VO_BindVpss ERR!");
        g_vpuBindvou = FY_TRUE;
    }
    if(2 != autoFlag){
        memset(&stChnAttr,0,sizeof(VPSS_CHN_ATTR_S));
        FY_MPI_VPSS_SetChnAttr(0,VPU_PREVIEW_CHN,&stChnAttr);
		if (g_bEnablePIP_PRE) 
			FY_MPI_VPSS_SetChnAttr(0,VPSS_CHN2,&stChnAttr);
    }
    if(VO_MODE_BUTT != g_enVoMode){
        g_preview_param.bShow   = FY_FALSE;
        FY_GOSD_LOCK();
        if(autoFlag)
            g_preview_param.bAutoDelay  = FY_TRUE;
        else
            g_preview_param.bManuDelay  = FY_TRUE;
        //change preview window , delete osd show
        s32Ret = sample_preview_delete_osd();
        if(FY_SUCCESS != s32Ret)
            SAMPLE_PREVIEW_PRT("sample_preview_delete_osd ERR!");

        s32Ret = sample_vpu_disable_preview(s32VpssGrpCnt,VPU_PREVIEW_CHN);

        s32Ret = sample_vo_stop_all(FY_TRUE, g_enVoMode);
    }

    s32Ret = sample_vo_start_all(FY_TRUE, enVoMode);

    sample_vpu_enable_preview(s32VpssGrpCnt, VPU_PREVIEW_CHN);
	if (g_bEnablePIP_PRE) 
		sample_vpu_enable_preview(s32VpssGrpCnt, VPSS_CHN2);
    if(2 == autoFlag){
        stChnAttr.cropInfo.bEnable = 1;
        stChnAttr.cropInfo.stRect.s32X = g_s32Sx;
        stChnAttr.cropInfo.stRect.s32Y = g_s32Sy;
        stChnAttr.cropInfo.stRect.u32Width = g_Vu32Width;
        stChnAttr.cropInfo.stRect.u32Height = g_Vu32Height;
        FY_MPI_VPSS_SetChnAttr(0,VPU_PREVIEW_CHN,&stChnAttr);
    }

    if(VO_MODE_BUTT != g_enVoMode)
        FY_GOSD_UNLOCK();

    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_vo_init ERR!");

    g_enVoMode = enVoMode;

    //if(autoFlag)
    g_preview_param.bAutoVo = autoFlag;
    g_preview_param.bShow   = FY_TRUE;
    if(FY_FALSE == g_preview_param.bThreadStart)
        sample_preview_start_autothread();

    g_vpuStart = FY_TRUE;
    return s32Ret;
ERR1:
    sample_vo_stop_all(FY_TRUE, enVoMode);

ERR0:
    sample_vo_deinit(FY_TRUE);
    return s32Ret;
}

FY_S32 sample_ele_preview_start(FY_BOOL bUp)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_CHN_ATTR_S stChnAttr;
    FY_S32 s32Sx = 0,s32Sy = 0;
    FY_S32 u32Width = 0,u32Height = 0;
    FY_S32 u32MaxWidth = 0, u32MaxHeight = 0;
    VI_CHN_STAT_S stVistat;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    s32Sx = g_s32Sx;
    s32Sy = g_s32Sy;
    FY_MPI_VI_Query(0,&stVistat);

    s32Ret = FY_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &stLayerAttr);

    u32MaxWidth = stVistat.u32PicWidth;
    u32MaxHeight = stVistat.u32PicHeight;

    u32Width = g_Vu32Width;
    u32Height = g_Vu32Height;
    if(1 == bUp){
        u32Width = PREVIEW_MIN(g_Vu32Width + g_step,u32MaxWidth);
        u32Height = PREVIEW_MIN(g_Vu32Height + g_step,u32MaxHeight);
    }
    else if(0 == bUp){
        u32Width = PREVIEW_MAX(g_Vu32Width - g_step,60);
        u32Height = PREVIEW_MAX(g_Vu32Height - g_step,60);
    }

    u32Width  = PREVIEW_MAX(u32Width,stLayerAttr.stDispRect.u32Width/4);
    u32Height = PREVIEW_MAX(u32Height,stLayerAttr.stDispRect.u32Height/4);

    g_Vu32Width = u32Width;
    g_Vu32Height = u32Height;


    stChnAttr.cropInfo.bEnable = (2 != bUp);
    stChnAttr.cropInfo.stRect.s32X = s32Sx;
    stChnAttr.cropInfo.stRect.s32Y = s32Sy;
    stChnAttr.cropInfo.stRect.u32Width = u32Width;
    stChnAttr.cropInfo.stRect.u32Height = u32Height;
    //printf("crop [%d,%d,%d,%d]\n",s32Sx,s32Sy,u32Width,u32Height);
    FY_MPI_VPSS_SetChnAttr(0,VPU_PREVIEW_CHN,&stChnAttr);

    return s32Ret;
}

FY_S32 sample_stress_preview_init(FY_U32 u32GrpCnt)
{

    VPSS_GRP_ATTR_S stGrpAttr;
    FY_S32 s32Ret = FY_SUCCESS;
    SAMPLE_VO_MODE_E enVoMode;

    s32VpssGrpCnt = u32GrpCnt;

    if(u32GrpCnt > 16) {
        enVoMode = VO_MODE_64MUX;
    } else if(u32GrpCnt > 8) {
        enVoMode = VO_MODE_1B_7S;
    } else {
        enVoMode = VO_MODE_4MUX;
    }
    //sample_start_vi();
	test_load_pic();	//also for encoding

    s32Ret = sample_set_vpu_modparam();
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_set_vpu_modparam ERR!");

    s32Ret = sample_vpu_init_param(&stGrpAttr);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_vpu_init_param ERR!");

    stGrpAttr.bNrEn       = 1;
    stGrpAttr.bYGammaEn   = 1;
    stGrpAttr.bChromaEn   = 1;
    stGrpAttr.bApcEn      = 1;
    stGrpAttr.bLcEn       = 1;

    if(g_preview_5M){
        stGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stGrpAttr.u32MaxW = 3840;
        stGrpAttr.bNrEn       = 0;
    }
    else
        stGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;

    s32Ret = sample_vpu_start(0,s32VpssGrpCnt, &stGrpAttr,1);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "sample_vpu_start ERR!");

    //s32Ret = sample_vi_bind_vpu_mix(s32VpssGrpCnt);
    s32Ret = sample_vi_bind_vpu(s32VpssGrpCnt);
    SAMPLE_PRE_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "SAMPLE_COMM_VI_BindVpss_MixCap ERR!");

    sample_preview_start(enVoMode,0);
    return s32Ret;

ERR1:
    sample_vpu_stop(0,s32VpssGrpCnt,VPSS_MAX_CHN_NUM,1);
ERR0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
