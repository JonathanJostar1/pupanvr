#ifndef __MPI_VI_H__
#define __MPI_VI_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "fy_comm_vi.h"
#include "fy_comm_vb.h"


/**********************************************************************************************
    - Standard API
***********************************************************************************************/
FY_S32 FY_MPI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
FY_S32 FY_MPI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

FY_S32 FY_MPI_VI_EnableDev(VI_DEV ViDev);
FY_S32 FY_MPI_VI_DisableDev(VI_DEV ViDev);

FY_S32 FY_MPI_VI_SetChnAttr(VI_CHN ViChn, const VI_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VI_GetChnAttr(VI_CHN ViChn, VI_CHN_ATTR_S *pstAttr);

FY_S32 FY_MPI_VI_SetChnMinorAttr(VI_CHN ViChn,const VI_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VI_GetChnMinorAttr(VI_CHN ViChn,VI_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_VI_ClearChnMinorAttr(VI_CHN ViChn);

FY_S32 FY_MPI_VI_EnableChn(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DisableChn(VI_CHN ViChn);

FY_S32 FY_MPI_VI_GetFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S*pstFrameInfo, FY_S32 s32MilliSec);
FY_S32 FY_MPI_VI_ReleaseFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
FY_S32 FY_MPI_VI_SetFrameDepth(VI_CHN ViChn, FY_U32 u32Depth);
FY_S32 FY_MPI_VI_GetFrameDepth(VI_CHN ViChn, FY_U32 *pu32Depth);

FY_S32 FY_MPI_VI_SetUserPic(VI_CHN ViChn, VI_USERPIC_ATTR_S *pstUsrPic);
FY_S32 FY_MPI_VI_EnableUserPic(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DisableUserPic(VI_CHN ViChn);

FY_S32 FY_MPI_VI_GetFd(VI_CHN ViChn);
FY_U32 FY_MPI_VI_CloseFd();

FY_S32 FY_MPI_VI_Query(VI_CHN ViChn, VI_CHN_STAT_S *pstStat);

FY_S32 FY_MPI_VI_EnableChnInterrupt(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DisableChnInterrupt(VI_CHN ViChn);

FY_S32 FY_MPI_VI_GetChnLuma(VI_CHN ViChn, VI_CHN_LUM_S *pstLuma);

FY_S32 FY_MPI_VI_SetSkipMode(VI_CHN ViChn, VI_SKIP_MODE_E enSkipMode);
FY_S32 FY_MPI_VI_GetSkipMode(VI_CHN ViChn, VI_SKIP_MODE_E* penSkipMode);

FY_S32 FY_MPI_VI_AttachVbPool(VI_CHN ViChn, VB_POOL hVbPool);
FY_S32 FY_MPI_VI_DetachVbPool(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DetachVbPoolByPoolId(VI_CHN ViChn, VB_POOL hVbPool);

FY_S32 FY_MPI_VI_SetModParam(VI_MOD_PARAM_S *pstModParam);
FY_S32 FY_MPI_VI_GetModParam(VI_MOD_PARAM_S *pstModParam);

FY_S32 FY_MPI_VI_SetBlock(VI_CHN ViChn,VI_BLOCK_S *pstBlock);
FY_S32 FY_MPI_VI_GetBlock(VI_CHN ViChn,VI_BLOCK_S *pstBlock);

/**********************************************************************************************
    - Extension API
***********************************************************************************************/

FY_S32 FY_MPI_VI_SetEC(VI_EC_LEVEL_E enEcLevel);
FY_S32 FY_MPI_VI_GetEC(VI_EC_LEVEL_E *penEcLevel);

FY_S32 FY_MPI_VI_SetChnVBCnt(VI_CHN ViChn, FY_U32 u32VBCnt);
FY_S32 FY_MPI_VI_ExtCmd(VI_CHN ViChn, FY_U16 cmd, void* param, int size);

/**********************************************************************************************
    - Unsupport API
***********************************************************************************************/
FY_S32 FY_MPI_VI_SetVbiAttr(VI_CHN ViChn, VI_VBI_ARG_S *pstVbiAttr);
FY_S32 FY_MPI_VI_GetVbiAttr(VI_CHN ViChn, VI_VBI_ARG_S *pstVbiAttr);
FY_S32 FY_MPI_VI_SetVbiMode(VI_CHN ViChn, VI_VBI_MODE_E enMode);
FY_S32 FY_MPI_VI_GetVbiMode(VI_CHN ViChn, VI_VBI_MODE_E *enMode);
FY_S32 FY_MPI_VI_EnableVbi(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DisableVbi(VI_CHN ViChn);
FY_S32 FY_MPI_VI_EnableCascadeChn(VI_CHN ViChn);
FY_S32 FY_MPI_VI_DisableCascadeChn(VI_CHN ViChn);

FY_S32 FY_MPI_VI_BindChn(VI_CHN ViChn, const VI_CHN_BIND_ATTR_S *pstChnBindAttr);
FY_S32 FY_MPI_VI_UnBindChn(VI_CHN ViChn);
FY_S32 FY_MPI_VI_GetChnBind(VI_CHN ViChn, VI_CHN_BIND_ATTR_S *pstChnBindAttr);

FY_S32 FY_MPI_VI_SetDevAttrEx(VI_DEV ViDev, const VI_DEV_ATTR_EX_S *pstDevAttrEx);
FY_S32 FY_MPI_VI_GetDevAttrEx(VI_DEV ViDev, VI_DEV_ATTR_EX_S *pstDevAttrEx);

FY_S32 FY_MPI_VI_GetFileHandle(FY_S32 s32Id);

FY_S32 FY_MPI_VI_EnableDllSlave(VI_DEV ViDev);
FY_S32 FY_MPI_VI_DisableDllSlave(VI_DEV ViDev);

FY_S32 FY_MPI_VI_SetSkipModeEx(VI_CHN ViChn, VI_SKIP_MODE_EX_S* pstModeEx);
FY_S32 FY_MPI_VI_GetSkipModeEx(VI_CHN ViChn, VI_SKIP_MODE_EX_S* pstModeEx);

FY_S32 FY_MPI_VI_SetRotate(VI_CHN ViChn, const ROTATE_E enRotate);
FY_S32 FY_MPI_VI_GetRotate(VI_CHN ViChn, ROTATE_E *penRotate);

FY_S32 FY_MPI_VI_SetDLL(VI_DEV ViDev,VI_DLL_CTRL_S *pstDllCtrl);
FY_S32 FY_MPI_VI_GetDLL(VI_DEV ViDev, VI_DLL_STATUS_S * pstDllStatus);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VI_H__ */



