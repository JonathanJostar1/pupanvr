/*#TAG_RELEASE_FH865X#*/
#ifndef __FY_NNA_MPI_H__
#define __FY_NNA_MPI_H__
/**NNA**/

#include "fy_type.h"
#include "fy_comm_nna.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {

#endif
#endif /* __cplusplus */

FY_S32 FY_NNA_Init();

FY_S32 FY_NNA_DET_Init(FY_VOID **detHandle, FY_U32 modId, FY_NN_INIT_PARAM_T *param);

FY_S32 FY_NNA_DET_Process(FY_VOID *detHandle, FY_IMAGE_T *src,  FY_DETECTION_T *out, FY_S32 timeoutMs);

FY_S32 FY_NNA_DET_Forward(FY_VOID *detHandle, FY_U32 chan, FY_DETECTION_T *out, FY_S32 timeoutMs);

FY_S32 FY_NNA_DET_SetParam(FY_VOID *detHandle, FY_DET_SETPARAM_T *setParams);

FY_S32 FY_NNA_DET_Exit(FY_VOID *detHandle);

FY_S32 FY_NNA_Close();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

