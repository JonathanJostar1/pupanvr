#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "fy_comm_vo.h"
#include "fy_comm_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**********************************************************************
    Standard API
**********************************************************************/

FY_S32 FY_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
FY_S32 FY_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

FY_S32 FY_MPI_VO_Enable(VO_DEV VoDev);
FY_S32 FY_MPI_VO_Disable(VO_DEV VoDev);

FY_S32 FY_MPI_VO_CloseFd(FY_VOID);

FY_S32 FY_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
FY_S32 FY_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

FY_S32 FY_MPI_VO_SetVideoLayerCompressAttr(VO_LAYER VoLayer, const VO_COMPRESS_ATTR_S *pstCompressAttr);
FY_S32 FY_MPI_VO_GetVideoLayerCompressAttr(VO_LAYER VoLayer, VO_COMPRESS_ATTR_S *pstCompressAttr);

FY_S32 FY_MPI_VO_EnableVideoLayer (VO_LAYER VoLayer);
FY_S32 FY_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

FY_S32 FY_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
FY_S32 FY_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

FY_S32 FY_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, FY_U32 u32Priority);
FY_S32 FY_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, FY_U32 *pu32Priority);

FY_S32 FY_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
FY_S32 FY_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

FY_S32 FY_MPI_VO_SetAttrBegin(VO_LAYER VoLayer);
FY_S32 FY_MPI_VO_SetAttrEnd  (VO_LAYER VoLayer);

FY_S32 FY_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame);

FY_S32 FY_MPI_VO_SetDispBufLen(VO_LAYER VoLayer, FY_U32 u32BufLen);
FY_S32 FY_MPI_VO_GetDispBufLen(VO_LAYER VoLayer, FY_U32 *pu32BufLen);

FY_S32 FY_MPI_VO_EnableChn (VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

FY_S32 FY_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
FY_S32 FY_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

FY_S32 FY_MPI_VO_SetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_PARAM_S *pstChnParam);
FY_S32 FY_MPI_VO_GetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_PARAM_S *pstChnParam);

FY_S32 FY_MPI_VO_SetChnDispPos(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);
FY_S32 FY_MPI_VO_GetChnDispPos(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);

FY_S32 FY_MPI_VO_SetChnField(VO_LAYER VoLayer, VO_CHN VoChn, const VO_DISPLAY_FIELD_E enField);
FY_S32 FY_MPI_VO_GetChnField(VO_LAYER VoLayer, VO_CHN VoChn, VO_DISPLAY_FIELD_E *pField);

FY_S32 FY_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, FY_S32 s32ChnFrmRate);
FY_S32 FY_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, FY_S32 *ps32ChnFrmRate);

FY_S32 FY_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

FY_S32 FY_MPI_VO_PauseChn (VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_RefreshChn( VO_LAYER VoLayer, VO_CHN VoChn);

FY_S32 FY_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

FY_S32 FY_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
FY_S32 FY_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

FY_S32 FY_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, FY_U32 u32Toleration);
FY_S32 FY_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, FY_U32 *pu32Toleration);

FY_S32 FY_MPI_VO_GetChnPts   (VO_LAYER VoLayer, VO_CHN VoChn, FY_U64 *pu64ChnPts);

FY_S32 FY_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);
FY_S32 FY_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);

FY_S32 FY_MPI_VO_SetWbcSource(VO_WBC VoWbc, const VO_WBC_SOURCE_S *pstWbcSource);
FY_S32 FY_MPI_VO_GetWbcSource(VO_WBC VoWbc, VO_WBC_SOURCE_S *pstWbcSources);

FY_S32 FY_MPI_VO_EnableWbc(VO_WBC VoWbc);
FY_S32 FY_MPI_VO_DisableWbc(VO_WBC VoWbc);

FY_S32 FY_MPI_VO_SetWbcAttr(VO_WBC VoWbc, const VO_WBC_ATTR_S *pstWbcAttr);
FY_S32 FY_MPI_VO_GetWbcAttr(VO_WBC VoWbc, VO_WBC_ATTR_S *pstWbcAttr);

FY_S32 FY_MPI_VO_SetWbcMode(VO_WBC VoWbc, VO_WBC_MODE_E enWbcMode);
FY_S32 FY_MPI_VO_GetWbcMode(VO_WBC VoWbc, VO_WBC_MODE_E *penWbcMode);

FY_S32 FY_MPI_VO_SetWbcDepth(VO_WBC VoWbc, FY_U32 u32Depth);
FY_S32 FY_MPI_VO_GetWbcDepth(VO_WBC VoWbc, FY_U32 *pu32Depth);

FY_S32 FY_MPI_VO_GetWbcFrame(VO_WBC VoWbc, VIDEO_FRAME_INFO_S *pstVFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VO_ReleaseWbcFrame(VO_WBC VoWbc, VIDEO_FRAME_INFO_S *pstVFrame);

FY_S32 FY_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
FY_S32 FY_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);

/**********************************************************************
    Extension API
**********************************************************************/
FY_S32 FY_MPI_VO_Suspend(VO_DEV VoDev);
FY_S32 FY_MPI_VO_Resume(VO_DEV VoDev);

FY_S32 FY_MPI_VO_Reset(VO_DEV VoDev);

FY_S32 FY_MPI_VO_SetChnCM(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CM_S *pstVideoCM);
FY_S32 FY_MPI_VO_GetChnCM(VO_LAYER VoLayer, VO_CHN VoChn, VO_CM_S *pstVideoCM);

FY_S32 FY_MPI_VO_SetChnCSC(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CSC_S *pstVideoCSC);
FY_S32 FY_MPI_VO_GetChnCSC(VO_LAYER VoLayer, VO_CHN VoChn, VO_CSC_S *pstVideoCSC);

FY_S32 FY_MPI_VO_EnableGraphicLayer(GRAPHIC_LAYER GraphicLayer);
FY_S32 FY_MPI_VO_DisableGraphicLayer(GRAPHIC_LAYER GraphicLayer);
FY_S32 FY_MPI_VO_ShowGraphicLayer(GRAPHIC_LAYER GraphicLayer);
FY_S32 FY_MPI_VO_HideGraphicLayer(GRAPHIC_LAYER GraphicLayer);
FY_S32 FY_MPI_VO_SetGraphicLayerPos(GRAPHIC_LAYER GraphicLayer, const POINT_S *pstPos);
FY_S32 FY_MPI_VO_SetGraphicLayerSize(GRAPHIC_LAYER GraphicLayer, const SIZE_S *pstSize);
FY_S32 FY_MPI_VO_SetGraphicLayerAlpha(GRAPHIC_LAYER GraphicLayer, const ALPHA_S *pstAlpha);
FY_S32 FY_MPI_VO_UpdateGraphicLayer(GRAPHIC_LAYER GraphicLayer, VIDEO_FRAME_INFO_S *pstVFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VO_GetGraphicLayerFrame(GRAPHIC_LAYER GraphicLayer, VIDEO_FRAME_INFO_S *pstVFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VO_ReleaseGraphicLayerFrame(GRAPHIC_LAYER GraphicLayer, VIDEO_FRAME_INFO_S *pstVFrame);

FY_S32 FY_MPI_VO_GetHdmiAudio(VO_DEV VoDev, VO_HDMI_AUDIO_S *pstHdmiAudio);
FY_S32 FY_MPI_VO_SetHdmiAudio(VO_DEV VoDev, VO_HDMI_AUDIO_S *pstHdmiAudio);

FY_S32 FY_MPI_VO_GetHdmiEDID(VO_DEV VoDev, VO_HDMI_EDID_S *pstHdmiEdid);

FY_S32 FY_MPI_VO_GetHdmiSink(VO_DEV VoDev, VO_HDMI_SINK_S *pstHdmiSink);
FY_S32 FY_MPI_VO_SetHdmiFmt(VO_DEV VoDev, VO_HDMI_FMT_S *pstHdmiFmt);
FY_S32 FY_MPI_VO_GetHdmiFmt(VO_DEV VoDev, VO_HDMI_FMT_S *pstHdmiFmt);



FY_S32 FY_MPI_VO_GetHdmiPolarity(VO_DEV VoDev, VO_HDMI_POL_S *pstHdmiPol);
FY_S32 FY_MPI_VO_SetHdmiPolarity(VO_DEV VoDev, VO_HDMI_POL_S *pstHdmiPol);


FY_S32 FY_MPI_VO_FillChn(VO_LAYER VoLayer, VO_CHN VoChn, FY_U32 u32RGB);

/**********************************************************************
    Unsupported API
**********************************************************************/
FY_S32 FY_MPI_VO_QueryChnStat(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

FY_S32 FY_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
FY_S32 FY_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

FY_S32 FY_MPI_VO_ClearChnBuffer(VO_LAYER VoLayer, VO_CHN VoChn, FY_BOOL bClrAll);

FY_S32 FY_MPI_VO_SetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, FY_U32 u32Threshold);
FY_S32 FY_MPI_VO_GetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, FY_U32 *pu32Threshold);

FY_S32 FY_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
					FY_U32 *pu32LumaData, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
FY_S32 FY_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

FY_S32 FY_MPI_VO_SetCascadeAttr(const VO_CAS_ATTR_S *pstCasAttr);
FY_S32 FY_MPI_VO_GetCascadeAttr(VO_CAS_ATTR_S *pstCasAttr);

FY_S32 FY_MPI_VO_EnableCascadeDev (VO_DEV VoCasDev);
FY_S32 FY_MPI_VO_DisableCascadeDev(VO_DEV VoCasDev);

FY_S32 FY_MPI_VO_SetCascadePattern(VO_DEV VoCasDev, FY_U32 u32Pattern);
FY_S32 FY_MPI_VO_GetCascadePattern(VO_DEV VoCasDev, FY_U32 *pu32Pattern);

FY_S32 FY_MPI_VO_CascadePosBindChn(FY_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);
FY_S32 FY_MPI_VO_CascadePosUnBindChn(FY_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);

FY_S32 FY_MPI_VO_EnableCascade (FY_VOID);
FY_S32 FY_MPI_VO_DisableCascade(FY_VOID);

FY_S32 FY_MPI_VO_GetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);
FY_S32 FY_MPI_VO_SetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);

FY_S32 FY_MPI_VO_SetDevFrameRate(VO_DEV VoDev, FY_U32 u32FrameRate);
FY_S32 FY_MPI_VO_GetDevFrameRate(VO_DEV VoDev, FY_U32 *pu32FrameRate);

FY_S32 FY_MPI_VO_EnableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 FY_MPI_VO_DisableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);

FY_S32 FY_MPI_VO_GetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);

FY_S32 FY_MPI_VO_SetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);

FY_S32 FY_MPI_VO_SetVtth(VO_DEV VoDev, FY_U32 u32Vtth);

FY_S32 FY_MPI_VO_GetVtth(VO_DEV VoDev, FY_U32* pu32Vtth);

FY_S32 FY_MPI_VO_QueryDevIntfStatus(VO_INTF_TYPE_E enDevInter,VO_DEVINTF_STATUS_S* pstStatus);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VO_H__ */

