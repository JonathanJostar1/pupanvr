
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "fy_common.h"
#include "fy_comm_video.h"
#include "fy_comm_venc.h"
#include "fy_comm_vb.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/**********************************************************************************************
        Standard API
***********************************************************************************************/
FY_S32 FY_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VENC_DestroyChn(VENC_CHN VeChn);
FY_S32 FY_MPI_VENC_SetChnlPriority(VENC_CHN VeChn, FY_U32 u32Priority);
FY_S32 FY_MPI_VENC_GetChnlPriority(VENC_CHN VeChn, FY_U32 *pu32Priority);
FY_S32 FY_MPI_VENC_ResetChn(VENC_CHN VeChn);
FY_S32 FY_MPI_VENC_StartRecvPic(VENC_CHN VeChn);
FY_S32 FY_MPI_VENC_StartRecvPicEx(VENC_CHN VeChn, VENC_RECV_PIC_PARAM_S *pstRecvParam);
FY_S32 FY_MPI_VENC_StopRecvPic(VENC_CHN VeChn);
FY_S32 FY_MPI_VENC_Query(VENC_CHN VeChn, VENC_CHN_STAT_S *pstStat);

FY_S32 FY_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstAttr);

FY_S32 FY_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

FY_S32 FY_MPI_VENC_InsertUserData(VENC_CHN VeChn, FY_U8 *pu8Data, FY_U32 u32Len);

FY_S32 FY_MPI_VENC_SendFrame(VENC_CHN VeChn, VIDEO_FRAME_INFO_S *pstFrame ,FY_S32 s32MilliSec);
FY_S32 FY_MPI_VENC_SendFrameEx(VENC_CHN VeChn, USER_FRAME_INFO_S *pstFrame, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VENC_SetFrameRate(VENC_CHN VeChn, const VENC_FRAME_RATE_S *pstFrameRate);
FY_S32 FY_MPI_VENC_GetFrameRate(VENC_CHN VeChn, VENC_FRAME_RATE_S *pstFrameRate);

FY_S32 FY_MPI_VENC_SetMaxStreamCnt(VENC_CHN VeChn, FY_U32 u32MaxStrmCnt);
FY_S32 FY_MPI_VENC_GetMaxStreamCnt(VENC_CHN VeChn, FY_U32 *pu32MaxStrmCnt);

FY_S32 FY_MPI_VENC_RequestIDR(VENC_CHN VeChn,FY_BOOL bInstant);

FY_S32 FY_MPI_VENC_EnableAdvSmartP( VENC_CHN VeChn, FY_BOOL bEnableAdvSmartP );

FY_S32 FY_MPI_VENC_GetFd(VENC_CHN VeChn);
FY_S32 FY_MPI_VENC_CloseFd(VENC_CHN VeChn);

FY_S32 FY_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_PARAM_REF_S *pstRefParam);
FY_S32 FY_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_PARAM_REF_S *pstRefParam);

FY_S32 FY_MPI_VENC_SetRoiCfg(VENC_CHN VeChn, VENC_ROI_CFG_S *pstVencRoiCfg);
FY_S32 FY_MPI_VENC_GetRoiCfg(VENC_CHN VeChn, FY_U32 u32Index, VENC_ROI_CFG_S *pstVencRoiCfg);

FY_S32 FY_MPI_VENC_SetRoiCfgEx(VENC_CHN VeChn, VENC_ROI_CFG_EX_S *pstVencRoiCfg);
FY_S32 FY_MPI_VENC_GetRoiCfgEx(VENC_CHN VeChn, FY_U32 u32Index, VENC_ROI_CFG_EX_S *pstVencRoiCfg);

FY_S32 FY_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_PARAM_H264_DBLK_S *pstH264Dblk);
FY_S32 FY_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_PARAM_H264_DBLK_S *pstH264Dblk);

FY_S32 FY_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_PARAM_H264_VUI_S *pstH264Vui);
FY_S32 FY_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_PARAM_H264_VUI_S *pstH264Vui);

FY_S32 FY_MPI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_PARAM_H265_DBLK_S *pstH265Dblk);
FY_S32 FY_MPI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_PARAM_H265_DBLK_S *pstH265Dblk);

FY_S32 FY_MPI_VENC_SetH265Vui(VENC_CHN VeChn, const VENC_PARAM_H265_VUI_S *pstH265Vui);
FY_S32 FY_MPI_VENC_GetH265Vui(VENC_CHN VeChn, VENC_PARAM_H265_VUI_S *pstH265Vui);

FY_S32 FY_MPI_VENC_SetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pstIntraRefresh);
FY_S32 FY_MPI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pstIntraRefresh);

FY_S32 FY_MPI_VENC_SetSuperFrameCfg(VENC_CHN VeChn, const VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);
FY_S32 FY_MPI_VENC_GetSuperFrameCfg(VENC_CHN VeChn,VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);

FY_S32 FY_MPI_VENC_AttachVbPool(VENC_CHN VeChn, VB_POOL hVbPool);
FY_S32 FY_MPI_VENC_DetachVbPool(VENC_CHN VeChn);

FY_S32 FY_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_PARAM_JPEG_S *pstJpegParam);
FY_S32 FY_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_PARAM_JPEG_S *pstJpegParam);

FY_S32 FY_MPI_VENC_SetCrop(VENC_CHN VeChn, const VENC_CROP_CFG_S *pstCropCfg);
FY_S32 FY_MPI_VENC_GetCrop(VENC_CHN VeChn, VENC_CROP_CFG_S *pstCropCfg);

FY_S32 FY_MPI_VENC_SetJpegBufThreshod(FY_U32 u32BufThreshold);
FY_S32 FY_MPI_VENC_GetJpegBufThreshod(FY_U32 *pu32Threshold);

/**********************************************************************************************
    - Extension API
***********************************************************************************************/
FY_S32 FY_MPI_VENC_SetModParamExt(VENC_PARAM_MOD_EXT_S *pstModParamExt);
FY_S32 FY_MPI_VENC_GetModParamExt(VENC_PARAM_MOD_EXT_S *pstModParamExt);

/**********************************************************************
        Unsupport API
**********************************************************************/
FY_S32 FY_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);
FY_S32 FY_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);

FY_S32 FY_MPI_VENC_SetH264InterPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);
FY_S32 FY_MPI_VENC_GetH264InterPred(VENC_CHN VeChn, VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);

FY_S32 FY_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);
FY_S32 FY_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);

FY_S32 FY_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_PARAM_H264_TRANS_S *pstH264Trans);
FY_S32 FY_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_PARAM_H264_TRANS_S *pstH264Trans);

FY_S32 FY_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);
FY_S32 FY_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);

FY_S32 FY_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);
FY_S32 FY_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);

FY_S32 FY_MPI_VENC_SetRefParamEx(VENC_CHN VeChn, const VENC_PARAM_REF_EX_S *pstRefParam);
FY_S32 FY_MPI_VENC_GetRefParamEx(VENC_CHN VeChn, VENC_PARAM_REF_EX_S *pstRefParam);


FY_S32 FY_MPI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H265_SLICE_SPLIT_S *pstSliceSplit);
FY_S32 FY_MPI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_PARAM_H265_SLICE_SPLIT_S *pstSliceSplit);

FY_S32 FY_MPI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_PARAM_H265_PU_S *pstPredUnit);
FY_S32 FY_MPI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_PARAM_H265_PU_S *pstPredUnit);

FY_S32 FY_MPI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pstStreamBufInfo);


FY_S32 FY_MPI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_PARAM_H265_TRANS_S *pstH265Trans);
FY_S32 FY_MPI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_PARAM_H265_TRANS_S *pstH265Trans);

FY_S32 FY_MPI_VENC_SetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E enRcPriority);
FY_S32 FY_MPI_VENC_GetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E *penRcPriority);

FY_S32 FY_MPI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_PARAM_H265_ENTROPY_S *pstH265Entropy);
FY_S32 FY_MPI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_PARAM_H265_ENTROPY_S *pstH265Entropy);

FY_S32 FY_MPI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_PARAM_H265_SAO_S *pstH265Sao);
FY_S32 FY_MPI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_PARAM_H265_SAO_S *pstH265Sao);


FY_S32 FY_MPI_VENC_SetH265Timing(VENC_CHN VeChn, const VENC_PARAM_H265_TIMING_S *pstH265Timing);
FY_S32 FY_MPI_VENC_GetH265Timing(VENC_CHN VeChn, VENC_PARAM_H265_TIMING_S *pstH265Timing);

FY_S32 FY_MPI_VENC_SetH264Poc(VENC_CHN VeChn, const VENC_PARAM_H264_POC_S *pstH264Poc);
FY_S32 FY_MPI_VENC_GetH264Poc(VENC_CHN VeChn, VENC_PARAM_H264_POC_S *pstH264Poc);

FY_S32 FY_MPI_VENC_SetColor2Grey(VENC_CHN VeChn, const VENC_COLOR2GREY_S *pstChnColor2Grey);
FY_S32 FY_MPI_VENC_GetColor2Grey(VENC_CHN VeChn, VENC_COLOR2GREY_S *pstChnColor2Grey);

FY_S32 FY_MPI_VENC_SetFrameLostStrategy(VENC_CHN VeChn, const VENC_PARAM_FRAMELOST_S *pstFrmLostParam);
FY_S32 FY_MPI_VENC_GetFrameLostStrategy(VENC_CHN VeChn, VENC_PARAM_FRAMELOST_S *pstFrmLostParam);

FY_S32 FY_MPI_VENC_GetSSERegion(VENC_CHN VeChn, FY_U32 u32Index,VENC_SSE_CFG_S *pstSSECfg);
FY_S32 FY_MPI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S * pstSSECfg);

FY_S32 FY_MPI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);
FY_S32 FY_MPI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);

FY_S32 FY_MPI_VENC_SetRcAdvParam(VENC_CHN VeChn, const VENC_RC_ADVPARAM_S *pstRcAdvParam);
FY_S32 FY_MPI_VENC_GetRcAdvParam(VENC_CHN VeChn, VENC_RC_ADVPARAM_S *pstRcAdvParam);

FY_S32 FY_MPI_VENC_SetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );
FY_S32 FY_MPI_VENC_GetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );

FY_S32 FY_MPI_VENC_EnableIDR(VENC_CHN VeChn, FY_BOOL bEnableIDR);
FY_S32 FY_MPI_VENC_SetModParam(VENC_PARAM_MOD_S *pstModParam);
FY_S32 FY_MPI_VENC_GetModParam(VENC_PARAM_MOD_S *pstModParam);

FY_S32 FY_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_PARAM_MJPEG_S *pstMjpegParam);
FY_S32 FY_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_PARAM_MJPEG_S *pstMjpegParam);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */

