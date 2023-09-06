#ifndef __FY_COMM_REGION_H__
#define __FY_COMM_REGION_H__

#include "fy_common.h"
#include "fy_comm_video.h"
#include "fy_errno.h"
#include "fy_defines.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/**********************************************************************
    PART I      - Macro
**********************************************************************/
#define RGN_MAX_BMP_UPDATE_NUM 16


/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/

typedef FY_U32 RGN_HANDLE;

/* type of video regions */
typedef enum fyRGN_TYPE_E
{
    OVERLAY_RGN = 0,    /*video overlay region */
    COVER_RGN,          /*video cover region*/
    COVEREX_RGN,        /*Not support*/
    OVERLAYEX_RGN,      /*Not support*/
    LINE_RGN,           /*Not support*/
    MOSAIC_RGN,         /*video mosaic region*/
    RGN_BUTT
} RGN_TYPE_E;

/*Not used*/
typedef enum fyINVERT_COLOR_MODE_E
{
    LESSTHAN_LUM_THRESH = 0,   /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */
    MORETHAN_LUM_THRESH,       /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
    INVERT_COLOR_BUTT
}INVERT_COLOR_MODE_E;

/*Not used*/
typedef enum fyRGN_AREA_TYPE_E
{
    AREA_RECT = 0,
    AREA_QUAD_RANGLE,
    AREA_BUTT
} RGN_AREA_TYPE_E;

typedef enum fyMOSAIC_BLK_SIZE_E
{
    MOSAIC_BLK_SIZE_8 = 0,    /*Not support,block size 8*8 of MOSAIC*/
    MOSAIC_BLK_SIZE_16,       /*block size 16*16 of MOSAIC*/
    MOSAIC_BLK_SIZE_32,       /*block size 32*32 of MOSAIC*/
    MOSAIC_BLK_SIZE_64,       /*Not support,block size 64*64 of MOSAIC*/
    MOSAIC_BLK_SIZE_BUTT
} MOSAIC_BLK_SIZE_E;

/*Not used*/
typedef enum fyRGN_COORDINATE_E
{
    RGN_ABS_COOR = 0,   /*Absolute coordinate*/
    RGN_RATIO_COOR      /*Ratio coordinate*/
}RGN_COORDINATE_E;

/*structure type define*/

/*Not used*/
typedef struct fyOVERLAY_QP_INFO_S
{
	FY_BOOL	 bQpDisable;
    FY_BOOL  bAbsQp;
    FY_S32   s32Qp;
}OVERLAY_QP_INFO_S;

/*Not used*/
typedef struct fyOVERLAY_INVERT_COLOR_S
{
    SIZE_S              stInvColArea;   /*it must be multipe of 16 but not more than 64.*/
    FY_U32              u32LumThresh;   /*the threshold to decide whether invert the OSD's color or not.*/
    INVERT_COLOR_MODE_E enChgMod;
    FY_BOOL             bInvColEn;      /*the switch of inverting color.*/
}OVERLAY_INVERT_COLOR_S;

typedef struct fyOVERLAY_ATTR_S
{
    PIXEL_FORMAT_E enPixelFmt;  /* bitmap pixel format*/
    FY_U32 u32BgColor;          /* background color, pixel format depends on "enPixelFmt" */
    SIZE_S stSize;              /* region size */
	FY_U32 u32CanvasNum;        /* Not support*/
}OVERLAY_ATTR_S;

typedef struct fyOVERLAY_CHN_ATTR_S
{
    POINT_S stPoint;            /* start point */
    FY_U32 u32FgAlpha;          /* foreground transparence */
    FY_U32 u32BgAlpha;          /* background transparence */
    FY_U32 u32Layer;            /* Not support,OVERLAY region layer*/
    OVERLAY_QP_INFO_S stQpInfo; /* Not support,QP infomation when venc*/
    OVERLAY_INVERT_COLOR_S stInvertColor;   /* Not support,invertColor infomation*/
}OVERLAY_CHN_ATTR_S;

/*Not used*/
typedef struct fyRGN_QUADRANGLE_S
{
    FY_BOOL bSolid;            /* whether solid or dashed framework */
    FY_U32 u32LineWidth;       /* Line Width of framework, valid when dashed framework */
    POINT_S stPoint[4];        /* points of quadrilateral*/
} RGN_QUADRANGLE_S;

typedef struct fyCOVER_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;        /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        	/* config of rect*/
        RGN_QUADRANGLE_S    stQuadRangle;  	/* Not support,config of arbitary quadrilateral COVER */
    };
    FY_U32 u32Color;
    FY_U32 u32Layer;   						/* Not support,COVER region layer range:[0,3] */
	RGN_COORDINATE_E enCoordinate;           /*Not support,ratio coordiante or abs coordinate*/
}COVER_CHN_ATTR_S;

typedef struct fyCOVEREX_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;       /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        /* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;  /* Not support,config of arbitary quadrilateral COVER */
    };
    FY_U32 u32Color;
    FY_U32 u32Layer;   /* Not support,COVEREX region layer range:[0,7] */
}COVEREX_CHN_ATTR_S;

typedef struct fyMOSAIC_CHN_ATTR_S
{
    RECT_S stRect;                 /*location of MOSAIC*/
    MOSAIC_BLK_SIZE_E enBlkSize;   /*block size of MOSAIC*/
    FY_U32 u32Layer;               /*Not support,MOSAIC region layer range:[0,3] */
}MOSAIC_CHN_ATTR_S;

typedef struct fyOVERLAYEX_ATTR_S
{
    PIXEL_FORMAT_E enPixelFmt;
    FY_U32 u32BgColor;  /* background color, pixel format depends on "enPixelFmt" */
    SIZE_S stSize;      /* region size */
    FY_U32 u32CanvasNum;/* Not support*/
}OVERLAYEX_ATTR_S;

/*Not used*/
typedef struct fyOVERLAYEX_CHN_ATTR_S
{
    POINT_S stPoint;    /* start point */
    FY_U32 u32FgAlpha;  /* foreground transparence */
    FY_U32 u32BgAlpha;  /* background transparence */
    FY_U32 u32Layer;    /* OVERLAYEX region layer */
}OVERLAYEX_CHN_ATTR_S;

/*Not used*/
typedef struct fyLINE_CHN_ATTR_S
{
	FY_U32  				u32LineWidth;       /* width of line */
	FY_U32  				u32LineColor;       /* color of line */
	POINT_S                 stLinePoints[2];    /* startpoint and endpoint of line */
}LINE_CHN_ATTR_S;

typedef union fyRGN_ATTR_U
{
    OVERLAY_ATTR_S      stOverlay;      /* attribute of overlay region */
    OVERLAYEX_ATTR_S    stOverlayEx;    /* Not support,attribute of overlayex region */
} RGN_ATTR_U;

typedef union fyRGN_CHN_ATTR_U
{
    OVERLAY_CHN_ATTR_S      stOverlayChn;      /* attribute of overlay region */
    COVER_CHN_ATTR_S        stCoverChn;        /* attribute of cover region */
    COVEREX_CHN_ATTR_S      stCoverExChn;      /* Not support,attribute of coverex region */
    OVERLAYEX_CHN_ATTR_S    stOverlayExChn;    /* Not support,attribute of overlayex region */
	LINE_CHN_ATTR_S    	    stLineChn;         /* Not support,attribute of drawline region */
    MOSAIC_CHN_ATTR_S       stMosaicChn;       /* attribute of mosic region */
} RGN_CHN_ATTR_U;

/* attribute of a region */
typedef struct fyRGN_ATTR_S
{
    RGN_TYPE_E enType;  /* region type */
    RGN_ATTR_U unAttr;  /* region attribute */
} RGN_ATTR_S;

/* attribute of a region */
typedef struct fyRGN_CHN_ATTR_S
{
    FY_BOOL           bShow;
    RGN_TYPE_E        enType;     /* region type */
    RGN_CHN_ATTR_U    unChnAttr;  /* region attribute */
} RGN_CHN_ATTR_S;

typedef struct fyRGN_BMP_UPDATE_S
{
    POINT_S             stPoint;
    BITMAP_S            stBmp;
    FY_U32              u32Stride;
} RGN_BMP_UPDATE_S;

typedef struct fyRGN_BMP_UPDATE_CFG_S
{
    FY_U32              u32BmpCnt;
    RGN_BMP_UPDATE_S    astBmpUpdate[RGN_MAX_BMP_UPDATE_NUM];
} RGN_BMP_UPDATE_CFG_S;

typedef struct fyRGN_CANVAS_INFO_S
{
    FY_U32         u32PhyAddr;
    FY_U32         u32VirtAddr;
    SIZE_S         stSize;
    FY_U32         u32Stride;
    PIXEL_FORMAT_E enPixelFmt;
} RGN_CANVAS_INFO_S;

/**********************************************************************
    PART III    - Error Code
**********************************************************************/

/* PingPong buffer change when set attr, it needs to remap memory in mpi interface */
#define FY_NOTICE_RGN_BUFFER_CHANGE  FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_NOTICE, FY_SUCCESS)

/* invlalid device ID */
#define FY_ERR_RGN_INVALID_DEVID     FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define FY_ERR_RGN_INVALID_CHNID     FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define FY_ERR_RGN_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define FY_ERR_RGN_EXIST             FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define FY_ERR_RGN_UNEXIST           FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define FY_ERR_RGN_NULL_PTR          FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define FY_ERR_RGN_NOT_CONFIG        FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define FY_ERR_RGN_NOT_SUPPORT       FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define FY_ERR_RGN_NOT_PERM          FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define FY_ERR_RGN_NOMEM             FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define FY_ERR_RGN_NOBUF             FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define FY_ERR_RGN_BUF_EMPTY         FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define FY_ERR_RGN_BUF_FULL          FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define FY_ERR_RGN_BADADDR           FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define FY_ERR_RGN_BUSY              FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define FY_ERR_RGN_NOTREADY          FY_DEF_ERR(FY_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FY_COMM_REGION_H__ */



