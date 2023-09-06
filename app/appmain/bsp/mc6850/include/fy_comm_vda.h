#ifndef __FY_COMM_VDA_H__
#define __FY_COMM_VDA_H__

#include "fy_common.h"
#include "fy_errno.h"
#include <linux/types.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
#include "fy_comm_vpss.h"

#define MB_SIZE				16
#define MB_NUM				2
#define MD_FILTER_NUM			5
#define MD_FILTER_MB			8
#define CD_H_FEATURE_MB_NUM		8
#define CD_V_FEATURE_MB_NUM		6
#define MD_AREA_MAX_NUM			32

#define MAX_MD_CHANNEL_NUM		32

typedef enum{
	VDA_CHECK_FRAME,
	VDA_CHECK_TIME
}FY_VDA_CHECK_TYPE;

typedef enum{
	CD_LEVEL_HIGH,
	CD_LEVEL_MID,
	CD_LEVEL_LOW
}FY_CD_LEVEL_E;

typedef enum{
	DRV_THREAD_MODE,
	USER_THREAD_MODE
}FY_MD_THREAD_MODE;

typedef enum
{
	MD_FRM_DIFF_ONLY,
	MD_FRM_DIFF_ALL
}FY_MD_TYPE;

typedef enum
{
	VDA_TYPE_MD,
	VDA_TYPE_CD
}FY_VDA_TYPE;

typedef struct
{
	FY_U8			MDhappenFlag;
	FY_U8			u8ChgValue;
	FY_U16			u16ChgValue;
	FY_U32			u32ChgValue;
} FY_VDA_MD_STATUS_t;


typedef struct{
	FY_U32			fTopLeftX;
	FY_U32			fTopLeftY;
	FY_U32			fWidth;
	FY_U32			fHeigh;
}FY_Rect_t;

typedef struct{
	FY_U32			horizontal_count;	/*  macro width  */
	FY_U32			vertical_count;		/*  macro height  */
	FY_U32			moveRectNum; 		/*  moved rect number  */
	FY_U32			cached_number;		/*  has cached result number based on frm  */
	FY_U32			curMode;
	FY_U64			time;
	FY_U8*			start;			/*  result for every macro in one frm  */
	FY_U8*			pMDValue;		/*  frame diff value  */
	FY_Rect_t*		rect;			/*  max move rect  */
}FY_MDConfig_Ex_Result_t;

typedef struct
{
	FY_U32			ID;
	FY_U32			enable;
	FY_U32			threshold;
	FY_U32			framedelay;
	FY_U32			resultNum;
	FY_VDA_CHECK_TYPE	checkMode;
	FY_MD_TYPE		stype;
	FY_Rect_t		*md_area;
	FY_VDA_MD_STATUS_t	*md_status;
} FY_VDA_MD_CFG_t;

typedef struct{
	FY_U32			ID;
	FY_U32			maxChnNum;
	FY_S32			VpssGrp;
	FY_S32			VpssChn;
	FY_MD_THREAD_MODE	threadMode;
	DS_MODE_E		outputMode;
	SIZE_S			mdSize;
	FY_BOOL			bOpenLog;
}FY_VDA_INIT_cfg;

typedef struct
{
	FY_U32			ID;
	FY_U32			enable;
	FY_U32			framedelay;
	FY_U32			changevalue;
	FY_CD_LEVEL_E		level;
	FY_VDA_CHECK_TYPE	checkMode;
} FY_VDA_CD_INFO_t;

typedef struct ioe_param_t{
	FY_S32          	img_y_only;                /* 0: both YC; 1:Y-only */
	FY_S32          	img_yuv420;                /* 0: yuv422,1:yuv420   */
	FY_S32          	img_uv_order;
	FY_U32			md_y_threshold;            /*Threshold threshold of motion detection image difference (Y threshold)*/
	FY_U32			md_u_threshold;            /*Threshold threshold of motion detection image difference (U threshold)*/
	FY_U32			md_v_threshold;            /*Threshold threshold of motion detection image difference (V threshold)*/
	FY_U32			img_src_size_w;
	FY_U32			img_src_size_h;
	FY_U32			img_src_pitch_w;
	FY_U32			img0_base_addr_y;
	FY_U32			img0_base_addr_c;
	FY_U32			img1_base_addr_y;
	FY_U32			img1_base_addr_c;
	FY_U32			img_des_size_w;
	FY_U32			img_des_size_h;
	FY_U32			img_des_base_addr_y;
	FY_U32			img_des_base_addr_c;
	FY_U32			pixel_num_threshold;      /*Motion detection sub-block noise filtering threshold*/
	FY_U32			timeout;
}IOE_PARAM_T;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __FY_COMM_VDA_H__ */


