#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "fy_common.h"
#include "fy_comm_video.h"
#include "fy_comm_vpss.h"
#include "fy_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*Standard API*/
FY_S32 FY_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
FY_S32 FY_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

FY_S32 FY_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
FY_S32 FY_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

FY_S32 FY_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
FY_S32 FY_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

FY_S32 FY_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
FY_S32 FY_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);

FY_S32 FY_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);
FY_S32 FY_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

FY_S32 FY_MPI_VPSS_SetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);
FY_S32 FY_MPI_VPSS_GetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);

FY_S32 FY_MPI_VPSS_SetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, FY_U32 u32Depth);
FY_S32 FY_MPI_VPSS_GetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, FY_U32 *pu32Depth);

FY_S32 FY_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
                               VIDEO_FRAME_INFO_S *pstVideoFrame, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,  VIDEO_FRAME_INFO_S *pstVideoFrame);

FY_S32 FY_MPI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVideoFrame, FY_U32 u32FrameIndex);
FY_S32 FY_MPI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame);

FY_S32 FY_MPI_VPSS_SetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);
FY_S32 FY_MPI_VPSS_GetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);

FY_S32 FY_MPI_VPSS_SetModParam(VPSS_MOD_PARAM_S *pstModParam);
FY_S32 FY_MPI_VPSS_GetModParam(VPSS_MOD_PARAM_S *pstModParam);

/*Extension API*/
FY_S32 FY_MPI_VPSS_GetCPY(VPSS_GRP VpssGrp, VPSS_PIC_DATA_S *pstCpy);
FY_S32 FY_MPI_VPSS_GetBGM(VPSS_GRP VpssGrp,  VPSS_PIC_DATA_S *pstBgm);
FY_S32 FY_MPI_VPSS_GetYcMean(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,VPSS_PIC_DATA_S *pstYcMean);
FY_S32 FY_MPI_VPSS_ReleasePicData(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_PIC_DATA_S *pstPicData);

FY_S32 FY_MPI_VPSS_SetYcnr(VPSS_GRP VpssGrp, VPSS_YCNR_INFO_S *pstYcnr);
FY_S32 FY_MPI_VPSS_GetYcnr(VPSS_GRP VpssGrp,VPSS_YCNR_INFO_S *pstYcnr);

FY_S32 FY_MPI_VPSS_SetApc(VPSS_GRP VpssGrp, VPSS_APC_INFO_S * pstApc);
FY_S32 FY_MPI_VPSS_GetApc(VPSS_GRP VpssGrp, VPSS_APC_INFO_S * pstApc);

FY_S32 FY_MPI_VPSS_GetHistStat(VPSS_GRP VpssGrp,VPSS_HIST_STAT_S *pstHist);

FY_S32 FY_MPI_VPSS_SetGlobleDispInitCfg(VPSS_GLOBLE_DISPLAY_INIT_CFG * pstGlobleDispCfg);
FY_S32 FY_MPI_VPSS_GetGlobleDispInitCfg(VPSS_GRP VpssGrp,VPSS_GLOBLE_DISPLAY_INIT_CFG *pstGlobleDispCfg);

FY_S32 FY_MPI_VPSS_SetMosaic(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_MASK_INFO_S *pstMosaic);
FY_S32 FY_MPI_VPSS_GetMosaic(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_MASK_INFO_S *pstMosaic);

FY_S32 FY_MPI_VPSS_SetPic(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LOGO_INFO *pstPic);
FY_S32 FY_MPI_VPSS_GetPic(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LOGO_INFO *pstPic);

FY_S32 FY_MPI_VPSS_SetPicColor(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LOGO_COLOR_INFO *pstColorInfo);
FY_S32 FY_MPI_VPSS_GetPicColor(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LOGO_COLOR_INFO *pstColorInfo);

FY_S32 FY_MPI_VPSS_SetGrpEffect(VPSS_GRP VpssGrp, VPU_GRP_EFFECT_S *pstVpssEffect);
FY_S32 FY_MPI_VPSS_GetGrpEffect(VPSS_GRP VpssGrp, VPU_GRP_EFFECT_S *pstVpssEffect);

FY_S32 FY_MPI_VPSS_SetScalerCoeff(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPU_SCALER_COEFF *pstScaler);
FY_S32 FY_MPI_VPSS_GetScalerCoeff(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPU_SCALER_COEFF *pstScaler);

/*Unsupport API*/
FY_S32 FY_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

FY_S32 FY_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
FY_S32 FY_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

FY_S32 FY_MPI_VPSS_SetChnParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PARAM_S *pstChnParam);
FY_S32 FY_MPI_VPSS_GetChnParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PARAM_S *pstChnParam);

FY_S32 FY_MPI_VPSS_SetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);
FY_S32 FY_MPI_VPSS_GetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);

FY_S32 FY_MPI_VPSS_SetGrpAdvancedParam(VPSS_GRP VpssGrp, VPSS_NR_ADVANCED_PARAM_S *pstVpssParam);
FY_S32 FY_MPI_VPSS_GetGrpAdvancedParam(VPSS_GRP VpssGrp, VPSS_NR_ADVANCED_PARAM_S *pstVpssParam);

FY_S32 FY_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);
FY_S32 FY_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);

FY_S32 FY_MPI_VPSS_SetGrpDelay(VPSS_GRP VpssGrp, FY_U32 u32Delay);
FY_S32 FY_MPI_VPSS_GetGrpDelay(VPSS_GRP VpssGrp, FY_U32 *pu32Delay);

FY_S32 FY_MPI_VPSS_SetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);
FY_S32 FY_MPI_VPSS_GetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);

FY_S32 FY_MPI_VPSS_SetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);
FY_S32 FY_MPI_VPSS_GetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);

FY_S32 FY_MPI_VPSS_SetChnOverlay(VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_U32 u32OverlayMask);
FY_S32 FY_MPI_VPSS_GetChnOverlay(VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_U32 *pu32OverlayMask);

FY_S32 FY_MPI_VPSS_GetGrpRegionLuma(VPSS_GRP VpssGrp,VPSS_REGION_INFO_S *pstRegionInfo,
                                                FY_U32 *pu32LumaData,FY_S32 s32MilliSec);
FY_S32 FY_MPI_VPSS_GetChnRegionLuma(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_REGION_INFO_S *pstRegionInfo,
                                                FY_U32 *pu32LumaData,FY_S32 s32MilliSec);

FY_S32 FY_MPI_VPSS_EnableUserFrameRateCtrl(VPSS_GRP VpssGrp);
FY_S32 FY_MPI_VPSS_DisableUserFrameRateCtrl(VPSS_GRP VpssGrp);

FY_S32 FY_MPI_VPSS_SetPreScaleMode(VPSS_GRP VpssGrp, VPSS_PRESCALE_MODE_E enVpssPreScaleMode);
FY_S32 FY_MPI_VPSS_GetPreScaleMode(VPSS_GRP VpssGrp, VPSS_PRESCALE_MODE_E* penVpssPreScaleMode);

FY_S32 FY_MPI_VPSS_AttachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VB_POOL hVbPool);
FY_S32 FY_MPI_VPSS_DetachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

FY_S32 FY_MPI_VPSS_SetRotate(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATE_E enRotate);
FY_S32 FY_MPI_VPSS_GetRotate(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATE_E *penRotate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

