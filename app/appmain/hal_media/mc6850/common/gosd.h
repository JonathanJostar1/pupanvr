#ifndef _GOSD_H_
#define _GOSD_H_

#include "type_def.h"

// typedef unsigned int         FH_UINT32;
// typedef unsigned char        FH_UINT8;

// typedef struct fhSIZE_S
// {
// 	unsigned int        u32Width; /**< 宽度像素  */
// 	unsigned int        u32Height; /**<高度像素  */
// } FH_SIZE;

typedef struct fhSIZE_S
{
	FH_UINT32           u32Width; /**< 宽度像素  */
	FH_UINT32           u32Height; /**<高度像素  */
} FH_SIZE;

typedef struct fhPOINT_
{
	FH_UINT32           u32X; /**< 水平坐标 */
	FH_UINT32           u32Y; /**< 垂直坐标 */
}FH_POINT;

typedef struct
{
	FH_SIZE                 vi_size;
}FH_VPU_SIZE;

typedef struct
{
	FH_UINT32               magic_color_en; /*使用magic_color*/
	FH_UINT32               magic_color;    /*magic_color,16bit,出现此颜色设为透明*/
	FH_UINT32               global_alpha_en;/*全局透明度使能*/
	FH_UINT32               global_alpha;   /*全局透明度*/
	FH_UINT32               rgb16_type;     /*位宽16模式:1:ARGB0565,2:ARGB1555,3:ARGB4444*/
	FH_UINT32               extmode;        /*0为LSB填0，1为MSB复制到LSB*/
	FH_UINT32               dtvmode;        /**<0为SDTV模式，1为HDTV模式*/
	FH_UINT32               rgbmode;        /**<0为stdio RGB，1为computer RGB*/
	FH_SIZE                 logo_size;      /**<logo大小 */
}FH_LOGOV2_CFG;

typedef struct
{
	FH_UINT32               logo_enable;   /**< 图片叠加使能 */
	FH_UINT32               logo_idx;      /**< 图片叠加使能 */
	FH_UINT32               logo_pixdepth; /**< 图片像素位宽，此芯片只支持1,2,4 */
	FH_UINT8               *logo_addr;     /**< logo数据的物理地址 */
	FH_LOGOV2_CFG           logo_cfg;      /**< logo 配置 */
	FH_POINT                logo_startpos; /**< logo 左上角起始点 */
	FH_UINT32               stride_value;  /**< 行长度 */
	FH_UINT32               color[256];    /**< 颜色索引库,ARGB8888 */
}FH_VPU_LOGOV2;

#endif