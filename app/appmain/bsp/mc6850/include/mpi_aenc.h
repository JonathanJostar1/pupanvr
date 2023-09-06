#ifndef __MPI_AENC_H__
#define __MPI_AENC_H__

#include "fy_common.h"
#include "fy_comm_aio.h"
#include "fy_comm_aenc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

FY_S32 FY_MPI_AENC_CreateChn(AENC_CHN AeChn, AENC_CHN_ATTR_S *pstAttr);
FY_S32 FY_MPI_AENC_DestroyChn(AENC_CHN AeChn);
FY_S32 FY_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, FY_BOOL bBlock);
FY_S32 FY_MPI_AENC_ClearChnBuf(AENC_CHN AeChn);
FY_S32 FY_MPI_AENC_SendFrame(AENC_CHN AeChn, AUDIO_FRAME_S *pstFrmInfo, FY_BOOL bBlock);
FY_S32 FY_MPI_AENC_ReleaseFrame(AENC_CHN AeChn, AUDIO_FRAME_S *pstFrmInfo);

/***************************follow is unsurpport function******************************/
FY_S32 FY_MPI_AENC_SendEndOfStream(AENC_CHN AeChn, FY_BOOL bInstant);
FY_S32 FY_MPI_AENC_RegeisterDecoder(FY_S32 *ps32Handle, AENC_ENCODER_S *pstDecoder);
FY_S32 FY_MPI_AENC_UnRegisterDecoder(FY_S32 s32Handle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AENC_H__ */

