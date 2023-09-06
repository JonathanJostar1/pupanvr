#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "fy_common.h"
#include "fy_comm_video.h"
#include "fy_comm_vgs.h"
#include "fy_comm_tde.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/* Standard API */
FY_S32 FY_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

FY_S32 FY_MPI_VGS_EndJob(VGS_HANDLE hHandle);

FY_S32 FY_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

FY_S32 FY_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask);

FY_S32 FY_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_LINE_S astVgsDrawLine[], FY_U32 u32ArraySize);


/* Extension API */
FY_S32 FY_MPI_VGS_AddCombineTask(VGS_HANDLE hHandle, VIDEO_FRAME_S *pstVFrmIn, RECT_S *pstInRect, VIDEO_FRAME_S *pstVFrmOut, RECT_S *pstOutRect);

FY_S32 FY_MPI_VGS_AddFillColorTask(VGS_HANDLE hHandle, VIDEO_FRAME_INFO_S *pstVFrm, RECT_S *pstRect, FY_U32 u32RGB);

FY_S32 FY_MPI_VGS_AddDeCompressTask(VGS_HANDLE phHandle, const VIDEO_FRAME_S* pstVFrmIn, VIDEO_FRAME_S *pstVFrmOut);

FY_S32 FY_MPI_VGS_AddFmtConvertTask(VGS_HANDLE hHandle, VIDEO_FRAME_S* pstFrmIn, VIDEO_FRAME_S* pstFrmOut, TDE2_SURFACE_S* pstSurIn, TDE2_SURFACE_S* pstSurOut);

FY_S32 FY_MPI_VGS_DoRotate(VGS_TASK_ATTR_S *pstTask, VGS_ROTATE_E enRotate);

FY_S32 FY_MPI_VGS_CreateChn(VGS_CHN VgsChn);

FY_S32 FY_MPI_VGS_DestroyChn(VGS_CHN VgsChn);

FY_S32 FY_MPI_VGS_SetChnSyncMode(VGS_CHN VgsChn, FY_BOOL bAsync, FY_U32 u32BufDepth);

FY_S32 FY_MPI_VGS_SetRotate(VGS_CHN VgsChn, VGS_PTH VgsPth, ROTATE_E enRotate);

FY_S32 FY_MPI_VGS_SetChnMode(VGS_CHN VgsChn, VGS_PTH VgsPth, VGS_CHN_PARA_S *pstVgsMode);

FY_S32 FY_MPI_VGS_GetChnMode(VGS_CHN VgsChn, VGS_PTH VgsPth, VGS_CHN_PARA_S *pstVgsMode);

FY_S32 FY_MPI_VGS_SetChnCrop(VGS_CHN VgsChn,  VGS_CROP_PARA_S *pstCropInfo);

FY_S32 FY_MPI_VGS_GetChnCrop(VGS_CHN VgsChn,  VGS_CROP_PARA_S *pstCropInfo);

FY_S32 FY_MPI_VGS_SendFrame(VGS_CHN VgsChn, const VIDEO_FRAME_INFO_S *pstVFrm, FY_BOOL bPlayMode, FY_U32 u32TimeOut);

FY_S32 FY_MPI_VGS_GetPthFrame(VGS_CHN VgsChn, VGS_PTH VgsPth, VIDEO_FRAME_INFO_S *pstVideoFrame, FY_S32 s32MilliSec);

FY_S32 FY_MPI_VGS_ReleasePthFrame(VGS_CHN VgsChn, VGS_PTH VgsPth);

FY_S32 FY_MPI_VGS_ChnQuery(VGS_CHN VgsChn, VGS_CHN_STS_S *pstChnSts);

FY_S32 FY_MPI_VGS_EnableBackupFrame(VGS_CHN VgsChn);

FY_S32 FY_MPI_VGS_DisableBackupFrame(VGS_CHN VgsChn);

FY_S32 FY_MPI_VGS_RefreshChn(VGS_CHN VgsChn);

FY_S32 FY_MPI_VGS_GetChnFrame(VGS_CHN VgsChn, VIDEO_FRAME_INFO_S *pstVideoFrame, FY_U32 u32FrameIndex);


/* Unsupport API */
FY_S32 FY_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_COVER_S astVgsAddCover[], FY_U32 u32ArraySize);

FY_S32 FY_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_OSD_S astVgsAddOsd[], FY_U32 u32ArraySize);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VGS_H__ */

