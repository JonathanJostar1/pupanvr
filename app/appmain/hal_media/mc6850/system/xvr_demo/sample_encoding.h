#ifndef __SAMPLE_COMM_ENCORDING_H__
#define __SAMPLE_COMM_ENCORDING_H__

#include "fy_common.h"
#include "sample_vpu.h"
#include "sample_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

FY_S32 sample_encoding_vpu_init(FY_U32 u32EncType,FY_U32 u32GrpCnt);
FY_S32 sample_encoding_vpu_deinit(FY_U32 u32EncType,FY_U32 u32GrpCnt);
FY_S32 sample_encoding_perf_init();
FY_VOID sample_encoding_perf_deinit();
FY_S32 sample_encoding_nvr_init(FY_U32 u32ChnNum);
FY_S32 sample_encoding_nvr_deinit(FY_U32 u32ChnNum);
FY_S32 sample_jpege_nvr_init(FY_U32 u32ChnNum);
FY_S32 sample_jpege_nvr_deinit(FY_U32 u32ChnNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_COMM_PLAYBACK_H__
