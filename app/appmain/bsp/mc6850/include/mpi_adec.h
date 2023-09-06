#ifndef __MPI_ADEC_H__
#define __MPI_ADEC_H__

#include "fy_common.h"
#include "fy_comm_aio.h"
#include "fy_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

FY_S32 FY_MPI_ADEC_CreateChn(ADEC_CHN AdChn, ADEC_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_ADEC_DestroyChn(ADEC_CHN AdChn);
FY_S32 FY_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, FY_BOOL bBlock);
FY_S32 FY_MPI_ADEC_ClearChnBuf(ADEC_CHN AdChn);
FY_S32 FY_MPI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_S *pstFrmInfo, FY_BOOL bBlock);
FY_S32 FY_MPI_ADEC_ReleaseFrame(ADEC_CHN AdChn, AUDIO_FRAME_S *pstFrmInfo);

/***************************follow is unsurpport function******************************/
FY_S32 FY_MPI_ADEC_SendEndOfStream(ADEC_CHN AdChn, FY_BOOL bInstant);
FY_S32 FY_MPI_ADEC_RegeisterDecoder(FY_S32 *ps32Handle, ADEC_DECODER_S *pstDecoder);
FY_S32 FY_MPI_ADEC_UnRegisterDecoder(FY_S32 s32Handle);
FY_S32 FY_MPI_ADEC_SetPlySize(ADEC_CHN AdChn, FY_U32 u32size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_ADEC_H__ */

