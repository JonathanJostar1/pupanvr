#ifndef __FY_COMM_NNA_H__
#define __FY_COMM_NNA_H__
/**|NNA|**/

#include "fy_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */
#pragma pack(4)

/**********************************************************************
    PART I      - Macro
**********************************************************************/
#define FY_MAX_PATH 256
#define FY_MAX_BBOX_NUM 150
typedef float  FY_FLOAT;

/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/
typedef struct
{
	FY_U32           base;        // 物理地址 | [ ]
	FY_VOID *        vbase;       // 虚拟地址 | [ ]
	FY_U32           size;        // 内存大小 | [ ]
}FY_MEM_INFO;

typedef enum
{
	FN_ROT_0 = 0,		//旋转0度 | [ ]
	FN_ROT_90 = 1,		//旋转90度 | [ ]
	FN_ROT_180 = 2,		//旋转180度 | [ ]
	FN_ROT_270 = 3		//旋转270度 | [ ]
}FY_NNA_ROT;

typedef struct
{
	FY_FLOAT x;		//矩形框x坐标 | [0.0 - 1.0]
	FY_FLOAT y;		//矩形框y坐标 | [0.0 - 1.0]
	FY_FLOAT w;		//矩形框宽度　| [0.0 - 1.0]
	FY_FLOAT h;		//矩形框高度　| [0.0 - 1.0]
}FY_RECT_T;

typedef enum
{
	FN_DET_TYPE_PERSON = 1,    //人形检测 | [ ]
	FN_DET_TYPE_FACE,          //人脸检测 | [ ]
	FN_DET_TYPE_C2,            //人车检测 | [ ]
	FN_DET_TYPE_PERSON_V0 = 11 //非法操作 | [ ]
}FY_TYPE_E;

typedef struct
{
	FY_U16 clsType;	//类别索引　| [ ]
	FY_RECT_T bbox;    	//矩形信息  | [ ]
	FY_FLOAT conf;     	//置信度　　| [0.0 - 1.0]
}FY_DET_BBOX_T;

typedef struct
{
	FY_U64 time_stamp;    //矩形信息对应帧的时间戳　| [ ]
	FY_U32 boxNum;        //矩形个数　　| [ ]
	FY_DET_BBOX_T detBBox[FY_MAX_BBOX_NUM]; //具体矩形信息　| [ ]
}FY_DETECTION_T;

typedef enum
{
	FY_IMAGE_FORMAT_RGB888 = (1 << 1) //输入图像的颜色格式　| [ ]
}FY_IMAGE_TYPE_E;

typedef struct
{
	FY_U8 *nbg_data;
	FY_U32 type;             //网络检测类型	  | [1 - 3 ]
	FY_U32 src_w_in;         //网络检测宽度	  | [ ]
	FY_U32 src_h_in;         //网络检测高度	  | [ ]
	FY_U32 src_c_in;         //网络检测深度　　　　| [ ]
	FY_FLOAT conf_thr;          //网络检测阈值　      | [ ]
	FY_NNA_ROT rotate;          //旋转角度		  | [ ]
	FY_VOID *reserved;         //后续兼容使用　　　   | [ ]
}FY_NN_INIT_PARAM_T;

typedef struct
{
	FY_U32 width;        //输入图像数据的宽　| [ ]
	FY_U32 height;       //输入图像数据的高　| [ ]
	FY_U32 stride;	//输入图像的stride  | [ ]
	FY_U32 timestamp;    //输入图像时间戳　　| [ ]
	FY_IMAGE_TYPE_E imageType; //输入图像的像素格式　| [RGB888]
	FY_MEM_INFO src_data;     //输入图像的地址信息　| [ ]
}FY_IMAGE_T;

typedef struct
{
	FY_FLOAT conf_thr; //网络检测阈值　| [0.0 - 1.0]
}FY_DET_SETPARAM_T;

/**********************************************************************
    PART III    - Error Code
**********************************************************************/
/*Invalid device ID*/
#define FY_ERR_NNA_INVALID_DEVID     FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/*Invalid channel ID*/
#define FY_ERR_NNA_INVALID_CHNID     FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/*At least one parameter is illegal. For example, an illegal enumeration value exists.*/
#define FY_ERR_NNA_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/*The channel exists.*/
#define FY_ERR_NNA_EXIST             FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*The UN exists.*/
#define FY_ERR_NNA_UNEXIST           FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/*A null point is used.*/
#define FY_ERR_NNA_NULL_PTR          FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/*Try to enable or initialize the system, device, or channel before configuring attributes.*/
#define FY_ERR_NNA_NOT_CONFIG        FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/*The operation is not supported currently.*/
#define FY_ERR_NNA_NOT_SURPPORT      FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/*The operation, changing static attributes for example, is not permitted.*/
#define FY_ERR_NNA_NOT_PERM          FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/*A failure caused by the malloc memory occurs.*/
#define FY_ERR_NNA_NOMEM             FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/*A failure caused by the malloc buffer occurs.*/
#define FY_ERR_NNA_NOBUF             FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/*The buffer is empty.*/
#define FY_ERR_NNA_BUF_EMPTY         FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/*No buffer is provided for storing new data.*/
#define FY_ERR_NNA_BUF_FULL          FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/*The system is not ready because it may be not initialized or loaded.
 *The error code is returned when a device file fails to be opened. */
#define FY_ERR_NNA_NOTREADY          FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*The source address or target address is incorrect during the operations such as calling copy_from_user or copy_to_user.*/
#define FY_ERR_NNA_BADADDR           FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/*The resource is busy during the operations such as destroying a VENC channel without deregistering it.*/
#define FY_ERR_NNA_BUSY              FY_DEF_ERR(FY_ID_NNA, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

#pragma pack()
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
