#ifndef __MPI_AO_H__
#define __MPI_AO_H__

#include "fy_type.h"
#include "fy_common.h"
#include "fy_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

FY_S32 FY_MPI_AO_SetPubAttr(AUDIO_DEV AoDevId,FY_U32 cmd, FY_VOID *pstAttr);
FY_S32 FY_MPI_AO_GetPubAttr(AUDIO_DEV AoDevId, fyAUDIO_MACRO_E *pstAttr);
FY_S32 FY_MPI_AO_Enable(AUDIO_DEV AoDevId);
FY_S32 FY_MPI_AO_Disable(AUDIO_DEV AoDevId);
FY_S32 FY_MPI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_SetChnParam(AUDIO_DEV AoDevId, AO_CHN AoChn,AUDIO_FRAME_E* pFrmInfo);
FY_S32 FY_MPI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_FRAME_S *pstData, FY_S32 s32MilliSec);
FY_S32 FY_MPI_AO_GetFd(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_set_dma_size(AUDIO_DEV AoDevId,FY_U32 uSize);
FY_S32 FY_MPI_AO_set_buffer(AUDIO_DEV AoDevId, AO_CHN AoChn,FY_U32 id);
FY_S32 FY_MPI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAMPLE_RATE_E enInSampleRate,FY_U32 param);
FY_S32 FY_MPI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn,FY_U32 param);
FY_S32 FY_MPI_AO_ClearChnBuf(AUDIO_DEV AoDevId ,AO_CHN AoChn);
FY_S32 FY_MPI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_SetVolume(AUDIO_DEV AoDevId, VOLUME_TYPE type, FY_S32 s32VolumeDb);
FY_S32 FY_MPI_AO_GetVolume(AUDIO_DEV AoDevId, VOLUME_TYPE type, FY_S32 *ps32VolumeDb);
FY_S32 FY_MPI_AO_SetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E enTrackMode);
FY_S32 FY_MPI_AO_GetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E *penTrackMode);
FY_S32 FY_MPI_AO_QueryChnStat(AUDIO_DEV AoDevId ,AO_CHN AoChn, AO_CHN_STATE_S *pstStatus);
FY_S32 FY_MPI_AO_SetMute(AUDIO_DEV AoDevId, FY_BOOL bEnable, AUDIO_FADE_S *pstFade);
FY_S32 FY_MPI_AO_GetMute(AUDIO_DEV AoDevId, FY_BOOL *pbEnable, AUDIO_FADE_S *pstFade);
FY_S32 FY_MPI_AO_SaveFile(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAVE_FILE_INFO_S* pstSaveFileInfo);
FY_S32 FY_MPI_AO_QueryFileStatus(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_FILE_STATUS_S* pstFileStatus);
FY_S32 FY_MPI_AO_ClrPubAttr(AUDIO_DEV AoDevId);
FY_S32 FY_MPI_AO_SetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE_CONFIG_S *pstVqeConfig);
FY_S32 FY_MPI_AO_GetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE_CONFIG_S *pstVqeConfig);
FY_S32 FY_MPI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
FY_S32 FY_MPI_AO_SetHpfParam(AUDIO_DEV AoDevId, AO_CHN AoChn,FY_U32 u32Para);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AO_H__ */

