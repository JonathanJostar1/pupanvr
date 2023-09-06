#ifndef _FY_MPI_DSP_H_
#define _FY_MPI_DSP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "fy_comm_dsp.h"
#include "fy_debug.h"

/***** Load DSP Bin *****/
FY_S32 FY_MPI_SVP_DSP_LoadBin(const FY_CHAR *pszBinFileName,SVP_DSP_MEM_TYPE_E enMemType);

/***** Enable DSP Core *****/
FY_S32 FY_MPI_SVP_DSP_EnableCore(SVP_DSP_ID_E enDspId);

/***** Disable DSP Core *****/
FY_S32 FY_MPI_SVP_DSP_DisableCore(SVP_DSP_ID_E enDspId);

/***** Dectect request *****/
FY_S32 FY_MPI_SVP_DSP_RPC(SVP_DSP_HANDLE *phHandle,SVP_DSP_REQUEST_ENTITY_S *pstEntity,      SVP_DSP_ID_E enDspId);


/*****query the status of a called function *****/
FY_S32 FY_MPI_SVP_DSP_Query(SVP_DSP_ID_E enDspId,SVP_DSP_HANDLE hHandle,FY_BOOL *pbFinish,FY_BOOL bBlock);

/*****Enable DSP Core*****/
FY_S32 FY_MPI_SVP_DSP_PowerOn(SVP_DSP_ID_E enDspId);

/*****Disable DSP Core*****/
FY_S32 FY_MPI_SVP_DSP_PowerOff(SVP_DSP_ID_E enDspId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_FY_MPI_DSP_H_*/

