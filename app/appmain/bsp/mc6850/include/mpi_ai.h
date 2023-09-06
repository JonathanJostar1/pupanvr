#ifndef __MPI_AI_H__
#define __MPI_AI_H__

#include "fy_type.h"
#include "fy_common.h"
#include "fy_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

FY_S32 FY_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId,FY_U32 cmd, FY_VOID *pstAttr);
FY_S32 FY_MPI_AI_GetChnData(AUDIO_DEV AiDevId,AI_CHN AiChn, FY_VOID *pstAttr);
FY_S32 FY_MPI_AI_Enable(AUDIO_DEV AiDevId);
FY_S32 FY_MPI_AI_Disable(AUDIO_DEV AiDevId);
FY_S32 FY_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, FY_S32 s32MilliSec);
FY_S32 FY_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm);
FY_S32 FY_MPI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn,AUDIO_FRAME_E* pFrmInfo);
FY_S32 FY_MPI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, fyAUDIO_MACRO_E *pstChnParam);
FY_S32 FY_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);

/******************************follow is expand function********************************/
FY_S32 FY_MPI_Select_By_FD(FY_S32 type,FY_S32 fd,FY_S32 sec,FY_S32 usec);
FY_S32 FY_MPI_AI_set_dma_size(AUDIO_DEV AiDevId,FY_U32 uSize);
FY_S32 FY_MPI_AI_set_buffer(AUDIO_DEV AiDevId, AO_CHN AiChn,FY_U32 id);
FY_S16 FY_MPI_volume_ctl_increase(FY_U32 id,FY_U16* pcm_buf,FY_U32 size);
FY_S16 FY_MPI_volume_ctl_decrease(FY_U32 id,FY_U16* pcm_buf,FY_U32 size);
FY_VOID FY_MPI_volume_ctl_init(FY_U32 id);
FY_S32 FY_MPI_AI_pause(FY_U32 AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_resume(FY_U32 AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_ACW_exit(FY_U32 AiDevId);
FY_S32 FY_MPI_AI_SetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E enTrackMode);
FY_S32 FY_MPI_AI_GetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E *penTrackMode);
FY_S32 FY_MPI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, VOLUME_TYPE type, FY_S32 s32VolumeDb);
FY_S32 FY_MPI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, VOLUME_TYPE type, FY_S32 *ps32VolumeDb);
FY_S32 FY_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE_CONFIG_S *pstVqeConfig);
FY_S32 FY_MPI_AI_GetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S *pstVqeConfig);
FY_S32 FY_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate,FY_S32 param);
FY_S32 FY_MPI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);
FY_S32 FY_MPI_AI_SaveFile(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);
FY_S32 FY_MPI_AI_QueryFileStatus(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAVE_FILE_INFO_S* pstFileStatus);
FY_S32 FY_MPI_AI_ClrPubAttr(AUDIO_DEV AiDevId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AI_H__ */

