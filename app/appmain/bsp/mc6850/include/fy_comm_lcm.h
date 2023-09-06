#ifndef __FY_COMM_LCM_H__
#define __FY_COMM_LCM_H__

#include "fy_type.h"
#include "fy_errno.h"
#include "fy_common.h"
#include "fy_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


typedef FY_S32 LCM_DEV;

/**********************************************************************
	PART I		- Macro
**********************************************************************/
#define LCM_CMD_DATA_LEN		(256)

#define LCM_DEVICE_NUM			(1)

/**********************************************************************
	PART II		- Structure and Enumeration
**********************************************************************/

typedef enum {
	LCM_SCREEN_NONE				= 0x0,
	LCM_SCREEN_MCU				= 0x1,
	LCM_SCREEN_RGB				= 0x2,
	LCM_SCREEN_MIPI 			= 0x3,
	LCM_SCREEN_DUMMY			,
} LCM_SCREEN_ID_E;


typedef struct {
	LCM_SCREEN_ID_E				enScreen;
	FY_U32						u32Width;
	FY_U32						u32Height;
	FY_U32						u32FPS;
} LCM_ATTR_S;

typedef struct {
	FY_U8						u8Cmd;
	FY_U8						u8WaitMs;
	FY_U16						u16DataLen;
	FY_U8						pu8Data[LCM_CMD_DATA_LEN];
} LCM_CMD_S;

/**********************************************************************
	PART III	- Error Code
**********************************************************************/
typedef enum {
	ERR_LCM_NOT_ENABLE			= 0x45,
	ERR_LCM_NOT_DISABLE			= 0x46,
	ERR_LCM_NOT_CONFIG			= 0x47,
	ERR_LCM_INVALID_DEVID		= 0x48,
	ERR_LCM_BUTT
}EN_LCM_ERR_CODE_E;

/* System define error code */
#define FY_ERR_LCM_BUSY					FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define FY_ERR_LCM_NO_MEM				FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define FY_ERR_LCM_NULL_PTR				FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define FY_ERR_LCM_SYS_NOTREADY			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define FY_ERR_LCM_ILLEGAL_PARAM		FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define FY_ERR_LCM_NOT_SUPPORT			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define FY_ERR_LCM_NOT_PERMIT			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define FY_ERR_LCM_NOT_ENABLE			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, ERR_LCM_NOT_ENABLE)
#define FY_ERR_LCM_NOT_DISABLE			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, ERR_LCM_NOT_DISABLE)
#define FY_ERR_LCM_NOT_CONFIG			FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, ERR_LCM_NOT_CONFIG)
#define FY_ERR_LCM_INVALID_DEVID		FY_DEF_ERR(FY_ID_LCM, EN_ERR_LEVEL_ERROR, ERR_LCM_INVALID_DEVID)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __FY_COMM_VO_H__ */

