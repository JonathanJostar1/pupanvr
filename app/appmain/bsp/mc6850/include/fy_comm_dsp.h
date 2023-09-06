
#ifndef _FY_COMM_DSP_H_
#define _FY_COMM_DSP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "fy_errno.h"
#include "fy_common.h"

/**********************************************************************
    PART I      - Macro
**********************************************************************/

typedef FY_S32 SVP_DSP_HANDLE;


/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/
typedef enum fyEN_SVP_DSP_ERR_CODE_E
{
    ERR_SVP_DSP_SYS_TIMEOUT    = 0x40,   /* SVP_DSP process timeout */
    ERR_SVP_DSP_QUERY_TIMEOUT  = 0x41,   /* SVP_DSP query timeout */
    ERR_SVP_DSP_OPEN_FILE      = 0x42,   /* SVP_DSP open file error*/
    ERR_SVP_DSP_READ_FILE      = 0x43,   /* SVP_DSP read file error*/

    ERR_SVP_DSP_BUTT
}EN_SVP_DSP_ERR_CODE_E;
    
/*SVP_DSP core id*/
typedef enum fySVP_DSP_ID_E
{
    SVP_DSP_ID_0 = 0x0,

    SVP_DSP_ID_BUTT
}SVP_DSP_ID_E;

/*SVP_DSP memory type*/
typedef enum fySVP_DSP_MEM_TYPE_E
{
    SVP_DSP_MEM_TYPE_SYS_DDR_DSP_0 = 0x0,
    SVP_DSP_MEM_TYPE_IRAM_DSP_0    = 0x1,
    SVP_DSP_MEM_TYPE_DRAM_0_DSP_0  = 0x2,
    SVP_DSP_MEM_TYPE_DRAM_1_DSP_0  = 0x3,

    SVP_DSP_MEM_TYPE_BUTT
}SVP_DSP_MEM_TYPE_E;

typedef struct fySVP_DSP_REQUEST_ENTITY_S {
	unsigned int src;
	unsigned int dst;
	void *info;
}SVP_DSP_REQUEST_ENTITY_S;

/**********************************************************************
    PART III    - Error Code
**********************************************************************/
#define  FY_ID_SVP_DSP  0xFE
/*Invalid device ID*/
#define FY_ERR_SVP_DSP_INVALID_DEVID     FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/*Invalid channel ID*/
#define FY_ERR_SVP_DSP_INVALID_CHNID     FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/*At least one parameter is illegal. For example, an illegal enumeration value exists.*/
#define FY_ERR_SVP_DSP_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/*The channel exists.*/
#define FY_ERR_SVP_DSP_EXIST             FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*The UN exists.*/
#define FY_ERR_SVP_DSP_UNEXIST           FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/*A null point is used.*/
#define FY_ERR_SVP_DSP_NULL_PTR          FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/*Try to enable or initialize the system, device, or channel before configuring attributes.*/
#define FY_ERR_SVP_DSP_NOT_CONFIG        FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/*The operation is not supported currently.*/
#define FY_ERR_SVP_DSP_NOT_SURPPORT      FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/*The operation, changing static attributes for example, is not permitted.*/
#define FY_ERR_SVP_DSP_NOT_PERM          FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/*A failure caused by the malloc memory occurs.*/
#define FY_ERR_SVP_DSP_NOMEM             FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/*A failure caused by the malloc buffer occurs.*/
#define FY_ERR_SVP_DSP_NOBUF             FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/*The buffer is empty.*/
#define FY_ERR_SVP_DSP_BUF_EMPTY         FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/*No buffer is provided for storing new data.*/
#define FY_ERR_SVP_DSP_BUF_FULL          FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/*The system is not ready because it may be not initialized or loaded.
 *The error code is returned when a device file fails to be opened. */
#define FY_ERR_SVP_DSP_NOTREADY          FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*The source address or target address is incorrect during the operations such as calling copy_from_user or copy_to_user.*/
#define FY_ERR_SVP_DSP_BADADDR           FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/*The resource is busy during the operations such as destroying a VENC channel without deregistering it.*/
#define FY_ERR_SVP_DSP_BUSY              FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/*SVP_DSP process timeout*/
#define FY_ERR_SVP_DSP_SYS_TIMEOUT       FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, ERR_SVP_DSP_SYS_TIMEOUT)
/*SVP_DSP query timeout*/
#define FY_ERR_SVP_DSP_QUERY_TIMEOUT     FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, ERR_SVP_DSP_QUERY_TIMEOUT)
/*SVP_DSP open file error*/
#define FY_ERR_SVP_DSP_OPEN_FILE         FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, ERR_SVP_DSP_OPEN_FILE)
/*SVP_DSP read file error*/
#define FY_ERR_SVP_DSP_READ_FILE         FY_DEF_ERR(FY_ID_SVP_DSP, EN_ERR_LEVEL_ERROR, ERR_SVP_DSP_READ_FILE)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_FY_COMM_DSP_H_*/
