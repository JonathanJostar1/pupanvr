#ifndef __FY_MPI_VDA_H__
#define __FY_MPI_VDA_H__

#include "fy_common.h"
#include "fy_comm_vda.h"
#include "fy_errno.h"
#include "fy_comm_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

FY_S32 MPI_VDA_MD_Init(FY_VDA_INIT_cfg* pCfg);
FY_S32 MPI_VDA_MD_DeInit(FY_U32 id);
FY_S32 MPI_VDA_MD_SetConfig(FY_VDA_MD_CFG_t *pMdCfg);
FY_S32 MPI_VDA_MD_GetConfig(FY_VDA_MD_CFG_t *pMdCfg);
FY_S32 MPI_VDA_MD_CacheResult(FY_U32 id);
FY_S32 MPI_VDA_MD_GetResult(FY_U32 id,FY_MDConfig_Ex_Result_t *pMDStatus);
FY_VOID MPI_VDA_MD_Print(FY_U32 id,FY_U32 timeout);

FY_S32 MPI_VDA_CD_Init(FY_VDA_INIT_cfg* pCfg);
FY_S32 MPI_VDA_CD_DeInit(FY_U32 id);
FY_S32 MPI_VDA_CD_SetConfig(FY_VDA_CD_INFO_t *pCDCfg);
FY_S32 MPI_VDA_CD_GetConfig(FY_VDA_CD_INFO_t *pCDCfg);
FY_S32 MPI_VDA_CD_GetResult(FY_U32 id,FY_U32 *pCDStatus);
FY_S32 MPI_VDA_MD_CD_Check(FY_VDA_TYPE type,FY_S32 id);
FY_S32 MPI_VDA_MD_IOE_Check(FY_VDA_TYPE type,FY_S32 id);

FY_S32 MPI_VDA_CD_Update_BG(FY_U32 id,FY_S32 vpssGrp,FY_S32 vpassChn);
FY_S32 MPI_VDA_CD_GetConstancy(FY_U32 id,FY_Rect_t* pRect,FY_VOID* pConstan);
FY_S32 FY_IOE_Open(void);
FY_S32 FY_IOE_Close(void);
FY_S32 FY_IOE_OPERATE(IOE_PARAM_T* ioe_param);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_MPI_VDA_H__ */

