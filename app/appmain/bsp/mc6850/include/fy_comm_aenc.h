#ifndef  __FY_COMM_AENC_H__
#define  __FY_COMM_AENC_H__

#include "fy_type.h"
#include "fy_common.h"
#include "fy_comm_aio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum fyEN_AENC_ERR_CODE_E
{
	AENC_ERR_ENCODER_ERR     = 64 ,
	AENC_ERR_VQE_ERR        = 65
} EN_AENC_ERR_CODE_E;

typedef enum fyAENC_PROC_E
{
	AENC_CHN_STATUS,
	AENC_EN_TYPE,
	AENC_FRAME_CNT,
	AENC_FRAME_SIZE,
	AENC_ENCODE_SIZE,
	AENC_FS_RATE,
	AENC_CHN_WIDTH,
	AENC_TRACK_MODE,
	AENC_DATA_STATUS,
	AENC_PROC_BUTT
}AENC_PROC_E;

#define MAX_AENC_FRM_CNT	300

typedef struct fyAENC_CH_ATTR_S
{
	PAYLOAD_TYPE_E		enType;
	FY_U32			u32FrmCnt;
	FY_U32			u32FrmSize;
	AUDIO_SAMPLE_RATE_E	fs_rate;
	AUDIO_BIT_WIDTH_E	chn_width;
	AUDIO_SOUND_MODE_E	track_mode;
	FY_VOID			*pValue;
}AENC_CHN_ATTR_S;

/***************************follow is unsurpport function******************************/
typedef struct fyAENC_ENCODER_S
{
	PAYLOAD_TYPE_E		enType;
	FY_U32			u32MaxFrmLen;
	FY_CHAR			aszName[16];
	FY_S32			(*pfnOpenEncoder)(FY_VOID *pEncoderAttr, FY_VOID **ppEncoder);
	FY_S32			(*pfnEncodeFrm)(FY_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
				FY_U8 *pu8Outbuf,FY_U32 *pu32OutLen);
	FY_S32			(*pfnCloseEncoder)(FY_VOID *pEncoder);
} AENC_ENCODER_S;

typedef struct _Proc_t
{
	FY_U32			u32ProcInfo[AENC_PROC_BUTT];
}PROC_T;

typedef struct _AENCProcInfo_s
{
	PROC_T			chn[AENC_MAX_CHN_NUM];
}AENC_PROC_INFO_S;

/* invlalid device ID */
#define FY_ERR_AENC_INVALID_DEVID     FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define FY_ERR_AENC_INVALID_CHNID     FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define FY_ERR_AENC_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define FY_ERR_AENC_EXIST             FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define FY_ERR_AENC_UNEXIST           FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define FY_ERR_AENC_NULL_PTR          FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define FY_ERR_AENC_NOT_CONFIG        FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define FY_ERR_AENC_NOT_SUPPORT       FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define FY_ERR_AENC_NOT_PERM          FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define FY_ERR_AENC_NOMEM             FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define FY_ERR_AENC_NOBUF             FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define FY_ERR_AENC_BUF_EMPTY         FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define FY_ERR_AENC_BUF_FULL          FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define FY_ERR_AENC_SYS_NOTREADY      FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* encoder internal err */
#define FY_ERR_AENC_ENCODER_ERR       FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define FY_ERR_AENC_VQE_ERR       FY_DEF_ERR(FY_ID_AENC, EN_ERR_LEVEL_ERROR, AENC_ERR_VQE_ERR)



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __FY_COMM_AENC_H__*/

