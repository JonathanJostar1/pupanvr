/*
 * mpp_vo.c
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#include "sample_comm.h"
#include "mpp_def.h"

int 	mpp_vo_init()
{
	int s32Ret = 0;
	VO_DEV VoDev;
	VO_LAYER VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;

	VoDev = SAMPLE_VO_DEV_DHD0;
	VoLayer = SAMPLE_VO_DEV_DHD0;

	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;

	memset(&stVoPubAttr, 0, sizeof(VO_PUB_ATTR_S));
	memset(&stVoLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

	stVoPubAttr.u32BgColor = 0x00FFFF;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	stVoPubAttr.enIntfType = VO_INTF_VGA;// | VO_INTF_HDMI;

	do{
		s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
		if(s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
			break;
		}

		stVoLayerAttr.bClusterMode = HI_FALSE;
		stVoLayerAttr.bDoubleFrame = HI_FALSE;
		stVoLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;

		s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
			&stVoLayerAttr.stDispRect.u32Width, &stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
		if (s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			break;
		}
		stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
		stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
		s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stVoLayerAttr);
		if(s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartLayer fail for %#x!\n", s32Ret);
			break;
		}

		s32Ret = mpp_fb_init(stVoLayerAttr.stDispRect.u32Width, stVoLayerAttr.stDispRect.u32Height, 0);
		if(s32Ret !=  HI_SUCCESS)
		{
			SAMPLE_PRT("mpp_fb_init fail for %#x!\n", s32Ret);
			break;
		}

		return HI_SUCCESS;
	}while(0);


	mpp_vo_exit();
	return s32Ret;
}

void 	mpp_vo_exit()
{
	VO_DEV VoDev;
	VO_LAYER VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;

	VoDev = SAMPLE_VO_DEV_DHD0;
	VoLayer = SAMPLE_VO_DEV_DHD0;

	mpp_fb_exit();

	SAMPLE_COMM_VO_StopLayer(VoLayer);
	SAMPLE_COMM_VO_StopDev(VoDev);
}


