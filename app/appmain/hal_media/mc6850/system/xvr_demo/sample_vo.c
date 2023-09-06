#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "sample_comm.h"
#include "sample_vo.h"
#include "sample_ui.h"
#include "sample_vpu.h"

static FY_BOOL gs_bEnableVhd0Single = FY_FALSE;
static FY_BOOL gs_bEnableCompress = FY_TRUE;
static FY_BOOL gs_bEnableLcd = FY_FALSE;
static FY_BOOL gs_bEnableWbc = FY_FALSE;
static FY_BOOL gs_bEnableWbcBind = FY_FALSE;

extern FY_S32 SAMPLE_COMM_AUDIO_StartHdmi();


FY_S32 sample_vo_enable_lcd(FY_BOOL bEnable)
{
    FY_S32 s32Ret = FY_SUCCESS;

    if (bEnable != gs_bEnableLcd) {
        gs_bEnableLcd = bEnable;
    }

    return s32Ret;
}

FY_S32 sample_vo_set_compress(FY_BOOL bEnable)
{
    FY_S32 s32Ret = FY_SUCCESS;

    if (bEnable != gs_bEnableCompress) {
        gs_bEnableCompress = bEnable;
    }

    return s32Ret;
}

static FY_U32  g_bEnablePIP = FY_FALSE;

FY_VOID sample_PIP_enable(FY_BOOL bEnablePIP)
{
    g_bEnablePIP = bEnablePIP;
    return;
}

FY_S32 sample_vo_init(FY_BOOL bIsHD, VO_INTF_SYNC_E enIntf)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_DEV VoDev;
    VO_LAYER VoLayer;
    VO_PUB_ATTR_S stVoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    if (bIsHD) {
        SAMPLE_VO_PRT("start DHD0.");
        VoDev = SAMPLE_VO_DEV_DHD0;
        VoLayer = SAMPLE_VO_LAYER_VHD0;

        stVoPubAttr.enIntfSync = enIntf;
		if (gs_bEnableLcd == FY_TRUE) {
			stVoPubAttr.enIntfType = VO_INTF_LCD;
		}
		else{
			stVoPubAttr.enIntfType = VO_INTF_VGA|VO_INTF_HDMI;
		}
        stVoPubAttr.u32BgColor = 0; //0x00FFFFFF
        s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartDev failed!");

        memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
        s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
            &stLayerAttr.stImageSize.u32Width, \
            &stLayerAttr.stImageSize.u32Height, \
            &stLayerAttr.u32DispFrmRt);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_GetWH failed!");

        stLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;
        stLayerAttr.stDispRect.s32X       = 0;
        stLayerAttr.stDispRect.s32Y       = 0;
        stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
        stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;


        if (gs_bEnableVhd0Single) {
            SAMPLE_VO_PRT("======================start single VHD0.======================\n");
            s32Ret = SAMPLE_COMM_VO_StartSingleLayer(VoLayer, &stLayerAttr, FY_TRUE);
            SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartSingleLayer failed!");
        } else if (gs_bEnableCompress) {
            SAMPLE_VO_PRT("start compressed VHD0.========================\n");
            s32Ret = SAMPLE_COMM_VO_StartCompressedLayer(VoLayer, &stLayerAttr);
            SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartCompressedLayer failed!");
        } else {
            SAMPLE_VO_PRT("==============start VHD0.=========================\n");
            s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
            SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartLayer failed!");
        }

		if (g_bEnablePIP) {			//ENABLE_PIP
			SAMPLE_VO_PRT("start PIP.");
			VoDev = SAMPLE_VO_DEV_DHD0;
       		VoLayer = SAMPLE_VO_LAYER_VPIP;

			stLayerAttr.stDispRect.s32X 	  = 200;
			stLayerAttr.stDispRect.s32Y 	  = 200;
			stLayerAttr.stDispRect.u32Width   = 320;
        	stLayerAttr.stDispRect.u32Height  = 240;

			stLayerAttr.stImageSize.u32Width  = 320;
			stLayerAttr.stImageSize.u32Height  = 320;

            SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
			SAMPLE_COMM_VO_StartChn(VoLayer, VO_MODE_1MUX);
		}
		

		SAMPLE_COMM_AUDIO_StartHdmi();
    } else {
        SAMPLE_VO_PRT("start DSD0.");
        VoDev = SAMPLE_VO_DEV_DSD0;
        VoLayer = SAMPLE_VO_LAYER_VSD0;

        stVoPubAttr.enIntfSync = enIntf/*VO_OUTPUT_PAL*/;
        stVoPubAttr.enIntfType = VO_INTF_CVBS;
        stVoPubAttr.u32BgColor = 0;
        s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartDev failed!");

        memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
        s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
            &stLayerAttr.stImageSize.u32Width, \
            &stLayerAttr.stImageSize.u32Height, \
            &stLayerAttr.u32DispFrmRt);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_GetWH failed!");

        SAMPLE_VO_PRT("start VSD0.");
        stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stLayerAttr.stDispRect.s32X       = 0;
        stLayerAttr.stDispRect.s32Y       = 0;
        stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
        stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;
        s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartLayer failed!");
    }

    if(ui_getState())
        ui_start();

EXIT:

    return s32Ret;
}

FY_S32 sample_vo_deinit(FY_BOOL bIsHD)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_DEV VoDev;
    VO_LAYER VoLayer;

    if(ui_getState())
        ui_stop_pushed();

    if (gs_bEnableWbc) {
        sample_vo_stop_wbc(gs_bEnableWbcBind);
    }

	if (g_bEnablePIP) {
		SAMPLE_VO_PRT("stop PIP.");
		VoLayer = SAMPLE_VO_LAYER_VPIP;
		SAMPLE_COMM_VO_StopLayer(VoLayer);
		
		SAMPLE_VO_PRT("stop PIP.");
		//VoDev = SAMPLE_VO_DEV_DHD0;
		//SAMPLE_COMM_VO_StopDev(VoDev);
	}

    if (bIsHD) {
        SAMPLE_VO_PRT("stop VHD0.");
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        SAMPLE_COMM_VO_StopLayer(VoLayer);

        SAMPLE_VO_PRT("stop DHD0.");
        VoDev = SAMPLE_VO_DEV_DHD0;
        SAMPLE_COMM_VO_StopDev(VoDev);
    } else {
        SAMPLE_VO_PRT("stop VSD0.");
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        SAMPLE_COMM_VO_StopLayer(VoLayer);

        SAMPLE_VO_PRT("stop DSD0.");
        VoDev = SAMPLE_VO_DEV_DSD0;
        SAMPLE_COMM_VO_StopDev(VoDev);
    }

    return s32Ret;
}

FY_S32 sample_vo_start_all(FY_BOOL bIsHD, SAMPLE_VO_MODE_E enVoMode)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer;

    if (bIsHD) {
        SAMPLE_VO_PRT("start VHD0 channels.");
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartChn failed!");
    } else {
        SAMPLE_VO_PRT("start VSD0 channels.");
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartChn failed!");
    }

EXIT:
    return s32Ret;
}

FY_S32 sample_vo_stop_all(FY_BOOL bIsHD, SAMPLE_VO_MODE_E enVoMode)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer;

    if (bIsHD) {
        SAMPLE_VO_PRT("stop VHD0 channels.");
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        s32Ret = SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StopChn failed!");
    } else {
        SAMPLE_VO_PRT("stop VSD0 channels.");
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        s32Ret = SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StopChn failed!");
    }

EXIT:
    return s32Ret;
}

FY_S32 sample_vo_start_one(FY_BOOL bIsHD, SAMPLE_VO_MODE_E enVoMode, VO_CHN voChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer;

    if (bIsHD) {
        SAMPLE_VO_PRT("start VHD0 channel[%d].", voChn);
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        s32Ret = SAMPLE_COMM_VO_StartChnOne(VoLayer, voChn, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartChnOne failed!");
    } else {
        SAMPLE_VO_PRT("start VSD0 channel[%d].", voChn);
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        s32Ret = SAMPLE_COMM_VO_StartChnOne(VoLayer, voChn, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartChnOne failed!");
    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vo_stop_one(FY_BOOL bIsHD, SAMPLE_VO_MODE_E enVoMode, VO_CHN voChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer;

    if (bIsHD) {
        SAMPLE_VO_PRT("stop VHD0 channel[%d].", voChn);
        VoLayer = SAMPLE_VO_LAYER_VHD0;
        s32Ret = SAMPLE_COMM_VO_StopChnOne(VoLayer, voChn, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StopChnOne failed!");
    } else {
        SAMPLE_VO_PRT("stop VSD0 channel[%d].", voChn);
        VoLayer = SAMPLE_VO_LAYER_VSD0;
        s32Ret = SAMPLE_COMM_VO_StopChnOne(VoLayer, voChn, enVoMode);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StopChnOne failed!");
    }

EXIT:
    return s32Ret;
}

FY_S32 sample_wbc_vpu_start(FY_S32 s32BeginGrp,PIXEL_FORMAT_E enPixFmt,VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_GRP_ATTR_S stGrpAttr;
    MPP_CHN_S stSrcChn,stVpuChn,stDestChn;

    s32Ret = sample_vpu_init_param(&stGrpAttr);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_vpu_init_param failed!");

    stGrpAttr.enPixFmt = enPixFmt;

    s32Ret = sample_vpu_start(s32BeginGrp,1, &stGrpAttr,0);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR0, "sample_vpu_start failed!");

    /*Bind WBC src to vpu*/
    stSrcChn.enModId    = FY_ID_VOU;
    stSrcChn.s32DevId   = VoWbcDev;
    stSrcChn.s32ChnId   = 0;

    stVpuChn.enModId   = FY_ID_VPSS;
    stVpuChn.s32DevId  = s32BeginGrp;
    stVpuChn.s32ChnId  = 0;

    s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stVpuChn);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR1, "FY_MPI_SYS_Bind failed!");

    /*Bind vpu to target vo display*/
    stVpuChn.enModId    = FY_ID_VPSS;
    stVpuChn.s32DevId   = s32BeginGrp;
    stVpuChn.s32ChnId   = WBC_TO_VO;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    s32Ret = FY_MPI_SYS_Bind(&stVpuChn, &stDestChn);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), ERR2, "FY_MPI_SYS_Bind failed!");

    return s32Ret;
ERR2:
    FY_MPI_SYS_UnBind(&stSrcChn, &stVpuChn);
ERR1:
    sample_vpu_stop(s32BeginGrp,1,VPSS_MAX_CHN_NUM,0);
ERR0:
    return s32Ret;

}

FY_S32 sample_wbc_vpu_stop(FY_S32 s32BeginGrp,VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    MPP_CHN_S stSrcChn,stVpuChn,stDestChn;

    stSrcChn.enModId    = FY_ID_VOU;
    stSrcChn.s32DevId   = VoWbcDev;
    stSrcChn.s32ChnId   = 0;


    stVpuChn.enModId   = FY_ID_VPSS;
    stVpuChn.s32ChnId  = 0;
    stVpuChn.s32DevId  = s32BeginGrp;

    s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stVpuChn);
    SAMPLE_VO_CHECK((FY_SUCCESS != s32Ret), "FY_MPI_SYS_UnBind failed!");

    stVpuChn.enModId    = FY_ID_VPSS;
    stVpuChn.s32DevId   = s32BeginGrp;
    stVpuChn.s32ChnId   = WBC_TO_VO;

    stDestChn.enModId   = FY_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    s32Ret = FY_MPI_SYS_UnBind(&stVpuChn, &stDestChn);
    SAMPLE_VO_CHECK((FY_SUCCESS != s32Ret),"FY_MPI_SYS_UnBind failed!");

    s32Ret = sample_vpu_stop(s32BeginGrp,1,VPSS_MAX_CHN_NUM,0);

    return s32Ret;

}

FY_S32 sample_vo_start_wbc(FY_BOOL bBind)
{
    FY_S32 s32Ret = FY_SUCCESS;
	VO_WBC VoWbc = SAMPLE_VO_DEV_DHD0;
    VO_WBC_ATTR_S stWbcAttr;
    VO_WBC_SOURCE_S stWbcSource;

    sample_vo_start_all(FY_FALSE, VO_MODE_1MUX);

    stWbcSource.enSourceType = VO_WBC_SOURCE_DEV;
    stWbcSource.u32SourceId = SAMPLE_VO_DEV_DHD0;

    s32Ret = SAMPLE_COMM_WBC_BindVo(VoWbc,&stWbcSource);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_WBC_BindVo failed!");

    /*******start Wbc*********/
    s32Ret = SAMPLE_COMM_VO_GetWH(VO_OUTPUT_720P60, \
        &stWbcAttr.stTargetSize.u32Width, \
        &stWbcAttr.stTargetSize.u32Height, \
        &stWbcAttr.u32FrameRate);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_GetWH failed!");

    stWbcAttr.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stWbcAttr.u32FrameRate = 25;
    s32Ret = SAMPLE_COMM_VO_StartWbc(VoWbc,&stWbcAttr);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartWbc failed!");

    if (bBind) {
        s32Ret = sample_wbc_vpu_start(WBC_VPU_GRP_BEGION,stWbcAttr.enPixelFormat,SAMPLE_VO_DEV_DHD0, SAMPLE_VO_LAYER_VSD0, 0);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "sample_wbc_vpu_start failed!");
    }

    gs_bEnableWbc = FY_TRUE;
    gs_bEnableWbcBind = bBind;

EXIT:

    return s32Ret;
}

FY_S32 sample_vo_stop_wbc(FY_BOOL bBind)
{
    FY_S32 s32Ret = FY_SUCCESS;
	VO_WBC VoWbc = SAMPLE_VO_DEV_DHD0;

    if (bBind) {
    	s32Ret = sample_wbc_vpu_stop(WBC_VPU_GRP_BEGION,SAMPLE_VO_DEV_DHD0, SAMPLE_VO_LAYER_VSD0, 0);
        SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "sample_wbc_vpu_stop failed!");
    }

    sample_vo_stop_all(FY_FALSE, VO_MODE_1MUX);

    s32Ret = SAMPLE_COMM_VO_StopWbc(VoWbc);
    SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StopWbc failed!");

    gs_bEnableWbc = FY_FALSE;
    gs_bEnableWbcBind = FY_FALSE;

EXIT:
    return s32Ret;
}


