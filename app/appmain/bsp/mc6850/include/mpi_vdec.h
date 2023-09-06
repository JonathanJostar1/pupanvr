#ifndef  __MPI_VDEC_H__
#define  __MPI_VDEC_H__

#include "fy_common.h"
#include "fy_comm_video.h"
#include "fy_comm_vb.h"
#include "fy_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

FY_S32 FY_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);

FY_S32 FY_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
FY_S32 FY_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_Query(VDEC_CHN VdChn,VDEC_CHN_STAT_S *pstStat);

FY_S32 FY_MPI_VDEC_GetFd(VDEC_CHN VdChn);
FY_S32 FY_MPI_VDEC_CloseFd(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_ResetChn(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);
FY_S32 FY_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);

FY_S32 FY_MPI_VDEC_SetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);
FY_S32 FY_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);

FY_S32 FY_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VDEC_GetImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo,FY_S32 s32MilliSec);
FY_S32 FY_MPI_VDEC_ReleaseImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo);

FY_S32 FY_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData);

FY_S32 FY_MPI_VDEC_SetRotate(VDEC_CHN VdChn, ROTATE_E enRotate);
FY_S32 FY_MPI_VDEC_GetRotate(VDEC_CHN VdChn, ROTATE_E *penRotate);

FY_S32 FY_MPI_VDEC_GetChnLuma(VDEC_CHN VdChn, VDEC_CHN_LUM_S *pstLuma);

FY_S32 FY_MPI_VDEC_SetUserPic(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstUsrPic);
FY_S32 FY_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, FY_BOOL bInstant);
FY_S32 FY_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode);
FY_S32 FY_MPI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode);

FY_S32 FY_MPI_VDEC_SetChnVBCnt(VDEC_CHN VdChn, FY_U32 u32BlkCnt);
FY_S32 FY_MPI_VDEC_GetChnVBCnt(VDEC_CHN VdChn, FY_U32 *pu32BlkCnt);

FY_S32 FY_MPI_VDEC_AttachVbPool(VDEC_CHN VdChn, VDEC_CHN_POOL_S *pstPool);
FY_S32 FY_MPI_VDEC_DetachVbPool(VDEC_CHN VdChn);

FY_S32 FY_MPI_VDEC_SetModParam(VDEC_MOD_PARAM_S *pstModParam);
FY_S32 FY_MPI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam);

/**********************************************************************************************
    - Extension API
***********************************************************************************************/
FY_S32 FY_MPI_VDEC_Init();
FY_S32 FY_MPI_VDEC_Exit();

FY_S32 FY_MPI_VDEC_QueryEx(VDEC_CHN VdChn, VDEC_CHN_STAT_EX_S *pstStatEx);

FY_S32 FY_MPI_VDEC_ExtApi(FY_S32 cmd, FY_VOID* param);

FY_S32 FY_MPI_VDEC_RegisterSender(void);
FY_S32 FY_MPI_VDEC_SetTaskBufSize(FY_S32 size);
FY_S32 FY_MPI_VDEC_SetCmdQDepth(FY_S32 depth);
FY_S32 FY_MPI_VDEC_SetProcFile(const char *path);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __MPI_VDEC_H__ */

