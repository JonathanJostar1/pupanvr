#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/list.h>

#include "fy_type.h"
#include "fy_errno.h"


#ifndef __MOD_EXT_H__
#define __MOD_EXT_H__

#define FYL_MMZ_NAME_LEN 32
#define FYL_MMB_NAME_LEN 16

#define MAX_MPP_MODULES FY_ID_LOG_BUTT

#define VERSION_MAGIC	20150804

#define MAX_MOD_NAME    16

typedef enum fyMOD_NOTICE_ID_E
{
    MOD_NOTICE_STOP = 0x11,
}MOD_NOTICE_ID_E;

typedef enum fyMOD_STATE_E
{
    MOD_STATE_FREE = 0x11,
    MOD_STATE_BUSY = 0X22,
}MOD_STATE_E;

typedef FY_S32 FN_MOD_Init(FY_VOID *);
typedef FY_VOID FN_MOD_Exit(FY_VOID);
typedef FY_VOID FN_MOD_Notify(MOD_NOTICE_ID_E enNoticeId);
typedef FY_VOID FN_MOD_QueryState(MOD_STATE_E *pstState);
typedef FY_U32 FN_MOD_VerChecker(FY_VOID);

typedef struct fyMPP_MODULE_S
{
    struct list_head list;
    struct module *pstOwner;

    FY_CHAR aModName[MAX_MOD_NAME];
    MOD_ID_E enModId;

    FN_MOD_Init         *pfnInit;
    FN_MOD_Exit         *pfnExit;
    FN_MOD_QueryState   *pfnQueryState;
    FN_MOD_Notify       *pfnNotify;
    FN_MOD_VerChecker   *pfnVerChecker;

    FY_BOOL  bInited;

    FY_VOID *pstExportFuncs;
    FY_VOID *pData;

    FY_CHAR *pVersion;
}UMAP_MODULE_S;


extern FY_CHAR *CMPI_GetModuleName(MOD_ID_E enModId);
extern UMAP_MODULE_S *CMPI_GetModuleById(MOD_ID_E enModId);
extern FY_VOID *CMPI_GetModuleFuncById(MOD_ID_E enModId);

extern FY_VOID CMPI_StopModules(FY_VOID);
extern FY_S32  CMPI_QueryModules(FY_VOID);
extern FY_S32  CMPI_InitModules(FY_VOID);
extern FY_VOID CMPI_ExitModules(FY_VOID);
extern FY_S32  CMPI_RegisterModule(UMAP_MODULE_S *pstModules);
extern FY_VOID CMPI_UnRegisterModule(MOD_ID_E enModId);

#define FUNC_ENTRY(type,id)  ((type*)CMPI_GetModuleFuncById(id))
#define CHECK_FUNC_ENTRY(id) (CMPI_GetModuleFuncById(id) != NULL)
#define FUNC_ENTRY_NULL(id)  (!CHECK_FUNC_ENTRY(id))


#endif /*  __MOD_EXT_H__ */

