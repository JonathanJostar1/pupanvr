#include "fy_mpi_vo.h"
#include "sample_comm.h"
#include "fy_mpi_def.h"


static FY_BOOL gs_bEnableVhd0Single = FY_FALSE;
static FY_BOOL gs_bEnableCompress = FY_FALSE;
static FY_BOOL gs_bEnableLcd = FY_FALSE;
static FY_BOOL gs_bEnableWbc = FY_FALSE;
static FY_BOOL gs_bEnableWbcBind = FY_FALSE;

static FY_U32  g_bEnablePIP = FY_FALSE;

FY_S32 _vo_init(FY_BOOL bIsHD, VO_INTF_SYNC_E enIntf)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_DEV VoDev;
    VO_LAYER VoLayer;
    VO_PUB_ATTR_S stVoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    if (bIsHD) {
        FY_MPI_PRT("start DHD0.");
        VoDev = SAMPLE_VO_DEV_DHD0;
        VoLayer = SAMPLE_VO_LAYER_VHD0;

        stVoPubAttr.enIntfSync = enIntf;
		if (gs_bEnableLcd == FY_TRUE) {
			stVoPubAttr.enIntfType = VO_INTF_LCD;
		}
		else{
			stVoPubAttr.enIntfType = VO_INTF_VGA|VO_INTF_HDMI;
		}
        stVoPubAttr.u32BgColor = 0x00FF00;
        s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
        if(s32Ret != 0)
        {
            return -1;
        }

        memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
        s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
            &stLayerAttr.stImageSize.u32Width, \
            &stLayerAttr.stImageSize.u32Height, \
            &stLayerAttr.u32DispFrmRt);
        //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_GetWH failed!");
        if(s32Ret != 0)
        {
            return -1;
        }

        stLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;
        stLayerAttr.stDispRect.s32X       = 0;
        stLayerAttr.stDispRect.s32Y       = 0;
        stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
        stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;

        if (gs_bEnableVhd0Single) {
            FY_MPI_PRT("start single VHD0.========================\n");
            s32Ret = SAMPLE_COMM_VO_StartSingleLayer(VoLayer, &stLayerAttr, FY_TRUE);
            if(s32Ret != 0)
            {
                return -1;
            }
            FY_MPI_VO_BindGraphicLayer(SAMPLE_VO_LAYER_VHD0, SAMPLE_VO_DEV_DHD0);
            //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartSingleLayer failed!");
        } else if (gs_bEnableCompress) {
            FY_MPI_PRT("start compressed VHD0.========================\n");
            s32Ret = SAMPLE_COMM_VO_StartCompressedLayer(VoLayer, &stLayerAttr);
            //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartCompressedLayer failed!");
            if(s32Ret != 0)
            {
                return -1;
            }
        } else {
            FY_MPI_PRT("start VHD0.========================\n");
            s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
            //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartLayer failed!");
            if(s32Ret != 0)
            {
                return -1;
            }
        }

		if (g_bEnablePIP) {			//ENABLE_PIP
			FY_MPI_PRT("start PIP.");
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
        FY_MPI_PRT("start DSD0.");
        VoDev = SAMPLE_VO_DEV_DSD0;
        VoLayer = SAMPLE_VO_LAYER_VSD0;

        stVoPubAttr.enIntfSync = enIntf/*VO_OUTPUT_PAL*/;
        stVoPubAttr.enIntfType = VO_INTF_CVBS;
        stVoPubAttr.u32BgColor = 0;
        s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
        //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartDev failed!");
        if(s32Ret != 0)
        {
            return -1;
        }

        memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
        s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
            &stLayerAttr.stImageSize.u32Width, \
            &stLayerAttr.stImageSize.u32Height, \
            &stLayerAttr.u32DispFrmRt);
        //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_GetWH failed!");
        if(s32Ret != 0)
        {
            return -1;
        }

        FY_MPI_PRT("start VSD0.");
        stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stLayerAttr.stDispRect.s32X       = 0;
        stLayerAttr.stDispRect.s32Y       = 0;
        stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
        stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;
        s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
        //SAMPLE_VO_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "SAMPLE_COMM_VO_StartLayer failed!");
        if(s32Ret != 0)
        {
            return -1;
        }
    }

EXIT:

    return s32Ret;
}

int     fy_mpi_vo_init()
{
    int ret = 0;
    ret = _vo_init(FY_TRUE, VO_OUTPUT_1080P60);
    if(ret != 0 )
    {
        FY_MPI_PRT("");
        return -1;
    }

    return 0;
}

void    fy_mpi_vo_exit()
{

}