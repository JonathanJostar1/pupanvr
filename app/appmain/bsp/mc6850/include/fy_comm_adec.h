#ifndef  __FY_COMM_ADEC_H__
#define  __FY_COMM_ADEC_H__


#include "fy_type.h"
#include "fy_common.h"
#include "fy_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum fyADEC_MODE_E
{
	ADEC_MODE_PACK = 0,/*frame mode*/
	ADEC_MODE_STREAM ,/*Not support*/
	ADEC_MODE_BUTT
}ADEC_MODE_E;

typedef enum fyEN_ADEC_ERR_CODE_E
{
	ADEC_ERR_DECODER_ERR	 = 64,
	ADEC_ERR_BUF_LACK
} EN_ADEC_ERR_CODE_E;

typedef enum fyADEC_PROC_E
{
	ADEC_CHN_STATUS,
	ADEC_EN_TYPE,
	ADEC_FRAME_CNT,
	ADEC_FRAME_SIZE,
	ADEC_FS_RATE,
	ADEC_CHN_WIDTH,
	ADEC_DECODE_MODE,
	ADEC_AAC_INVLID_SIZE,
	ADEC_TOTAL_POOL,
	ADEC_CACHE_SIZE,
	ADEC_READ_NUM,
	ADEC_WRITE_NUM,
	ADEC_LAST_WRITE_NUM,
	ADEC_GET_CNT,
	ADEC_LAST_GET_CNT,
	ADEC_PROC_BUTT
}ADEC_PROC_E;

#define MAX_ADEC_FRM_CNT	300

typedef struct fyADEC_CH_ATTR_S
{
	PAYLOAD_TYPE_E		enType;
	FY_U32			u32FrmCnt;
	FY_U32			u32FrmSize;
	AUDIO_SAMPLE_RATE_E	fs_rate;
	AUDIO_BIT_WIDTH_E	chn_width;
	ADEC_MODE_E		enMode;
	FY_VOID			*pValue;
}ADEC_CHN_ATTR_S;

typedef struct _ProcParam_s
{
	FY_U32			u32ProcInfo[ADEC_PROC_BUTT];
}PROC_PARAM_S;

typedef struct _ADECProcInfo_s
{
	PROC_PARAM_S		chn[ADEC_MAX_CHN_NUM];
}ADEC_PROC_INFO_S;

/******************************follow is unsurpport function***************************/
typedef struct fyADEC_DECODER_S
{
	PAYLOAD_TYPE_E		enType;
	FY_CHAR			aszName[16];
	FY_S32			(*pfnOpenDecoder)(FY_VOID *pDecoderAttr, FY_VOID **ppDecoder); /*struct ppDecoder is packed by user,user malloc and free memory for this struct */
	FY_S32			(*pfnDecodeFrm)(FY_VOID *pDecoder, FY_U8 **pu8Inbuf,FY_S32 *ps32LeftByte,
				FY_U16 *pu16Outbuf,FY_U32 *pu32OutLen,FY_U32 *pu32Chns);
	FY_S32			(*pfnGetFrmInfo)(FY_VOID *pDecoder, FY_VOID *pInfo);
	FY_S32			(*pfnCloseDecoder)(FY_VOID *pDecoder);
	FY_S32			(*pfnResetDecoder)(FY_VOID *pDecoder);
} ADEC_DECODER_S;

/* invlalid device ID */
#define FY_ERR_ADEC_INVALID_DEVID     FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define FY_ERR_ADEC_INVALID_CHNID     FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define FY_ERR_ADEC_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define FY_ERR_ADEC_EXIST             FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define FY_ERR_ADEC_UNEXIST           FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define FY_ERR_ADEC_NULL_PTR          FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define FY_ERR_ADEC_NOT_CONFIG        FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define FY_ERR_ADEC_NOT_SUPPORT       FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define FY_ERR_ADEC_NOT_PERM          FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define FY_ERR_ADEC_NOMEM             FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define FY_ERR_ADEC_NOBUF             FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define FY_ERR_ADEC_BUF_EMPTY         FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define FY_ERR_ADEC_BUF_FULL          FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define FY_ERR_ADEC_SYS_NOTREADY      FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* decoder internal err */
#define FY_ERR_ADEC_DECODER_ERR       FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define FY_ERR_ADEC_BUF_LACK          FY_DEF_ERR(FY_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_BUF_LACK)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __FY_COMM_ADEC_H__*/

