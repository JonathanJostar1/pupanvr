#ifndef __FY_COMM_VPSS_H__
#define __FY_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "fy_type.h"
#include "fy_common.h"
#include "fy_errno.h"
#include "fy_comm_video.h"

/**********************************************************************
    PART I      - Macro
**********************************************************************/

#define VPSS_MAX_MASK_AREA             (8)
#define VPSS_MAX_LOGO_AREA             (8)
#define GAIN_NODES                    (12)
#define VPSS_MAX_GOSD_COLOR_IDX        (256)
#define SLICE_HEAD_SIZE   64

/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/

typedef   FY_S32 VPSS_GRP;  /* [0, 128] */
typedef   FY_S32 VPSS_CHN;  /* [0, 4] */

/*Define error number*/
typedef enum fyEN_VPU_ERR_CODE_E
{

    EN_ERR_VPU_GRP_HAS_BINDED      = 0x40,
    EN_ERR_VPU_GRP_NOT_BINDED      = 0x41,

    ERR_VPU_NOT_ENABLE             = 0x45,
    ERR_VPU_NOT_DISABLE            = 0x46,
    ERR_VPU_NOT_CONFIG             = 0x47,

    ERR_VPU_CHN_NOT_DISABLE        = 0x48,
    ERR_VPU_CHN_NOT_ENABLE         = 0x49,
    ERR_VPU_CHN_NOT_CONFIG         = 0x4a,
    ERR_VPU_CHN_GET_TIMEOUT        = 0x4b,

    ERR_VPU_GRP_INVALID_ID         = 0x55,
    ERR_VPU_GRP_NOT_CREATE         = 0x56,

    ERR_VPU_GRP_NOT_DESTROY        = 0x58,
    ERR_VPU_GRP_CHN_FULL           = 0x59,
    ERR_VPU_GRP_WAIT_FRAME_TIMEOUT = 0x5a,
    ERR_VPU_GRP_CHN_NOT_EMPTY      = 0x5b,
    ERR_VPU_DST_NOT_BIND           = 0x5c,
    ERR_VPU_GRP_INVALID_BASE_PTS   = 0x5d,
    ERR_VPU_GRP_NOT_START          = 0x5e,
    ERR_VPU_GRP_NOT_STOP           = 0x5f,
    ERR_VPU_GRP_INVALID_FRMRATE    = 0x60,
    ERR_VPU_GLOBLE_NOT_ENABLE      = 0x61,
    ERR_VPU_SET_SCALER_LEVEL_ERROR = 0x62,

    ERR_VPU_BUTT
}EN_VPU_ERR_CODE_E;

/*Define 4 video frame,not support*/
typedef enum fyVPSS_FRAME_WORK_E
{
    VPSS_FRAME_WORK_LEFT     =  0,  /*Not Support*/
    VPSS_FRAME_WORK_RIGHT    =  1,  /*Not Support*/
    VPSS_FRAME_WORK_BOTTOM   =  2,  /*Not Support*/
    VPSS_FRAME_WORK_TOP      =  3,  /*Not Support*/
    VPSS_FRAME_WORK_BUTT
}VPSS_FRAME_WORK_E;

/*Define de-interlace mode*/
typedef enum  fyVPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_NODIE     = 0,
    VPSS_DIE_MODE_AUTO      = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/* Define image feild select mode */
typedef enum fyVPSS_CAPSEL_E
{
    VPSS_CAPSEL_BOTH = 0,             /* top and bottom field */
    VPSS_CAPSEL_TOP,                  /* top field */
    VPSS_CAPSEL_BOTTOM,               /* bottom field */

    VPSS_CAPSEL_BUTT
} VPSS_CAPSEL_E;

/*Define coordinate mode*/
typedef enum fyVPSS_CROP_COORDINATE_E
{
    VPSS_CROP_RATIO_COOR = 0,   /*Ratio coordinate*/
    VPSS_CROP_ABS_COOR          /*Absolute coordinate*/
}VPSS_CROP_COORDINATE_E;

/*Define bgm/ycmean downsample macroblock mode*/
typedef enum
{
    DS_NONE          = 1,   /*None*/
    DS_ONE_FOURTH    = 4,   /*4x4 pixel one block*/
    DS_ONE_EIGHTH    = 8,   /*8x8 pixel one block*/
    DS_ONE_SIXTEENTH = 16   /*16x16 pixel one block*/
}DS_MODE_E;

/*
1D compress rate:50%(128B),63%(160B),75%(192B),(x/(128*2)==x/(256)), only support yuv422
TILE compress rate:50%(192B),59%(224B), 67%(256B, (x/(64*4*1.5)==x/(384)) only support yuv420
slice compress rate:40%~70%, one step :5% only support yuv422
*/
typedef enum
{
    COMPRESS_RATIO_NONE   = 100, /**No compress*/
    COMPRESS_RATIO_75_PER = 75,  /**compress rate=75%*/
    COMPRESS_RATIO_70_PER = 70,
    COMPRESS_RATIO_67_PER = 67,
    COMPRESS_RATIO_65_PER = 65,
    COMPRESS_RATIO_63_PER = 63,
    COMPRESS_RATIO_60_PER = 60,
    COMPRESS_RATIO_59_PER = 59,
    COMPRESS_RATIO_55_PER = 55,
    COMPRESS_RATIO_50_PER = 50,
    COMPRESS_RATIO_45_PER = 45,
    COMPRESS_RATIO_40_PER = 40,
    COMPRESS_RATIO_35_PER = 35,
    COMPRESS_RATIO_30_PER = 30
}COMPRESS_RATIO_E;

/*Define vpss channel's work mode*/
typedef enum fyVPSS_CHN_MODE_E
{
    VPSS_CHN_MODE_AUTO = 0, /*Auto mode*/
    VPSS_CHN_MODE_USER      /*User mode*/
}VPSS_CHN_MODE_E;

/*Not used. Define vpss prescale work mode*/
typedef enum fyVPSS_PRESCALE_MODE_E
{
    VPSS_PRESCALE_MODE_DEFAULT = 0,  /*use vpss to prescale*/
    VPSS_PRESCALE_MODE_OTHER,        /*use vgs to prescale*/
    VPSS_PRESCALE_MODE_BUTT
}VPSS_PRESCALE_MODE_E;

/*Define vpss scale direction*/
typedef enum
{
	DIRECTION_H=0,  /*scale horizontally*/
	DIRECTION_V,    /*scale vertically*/
}PIC_DIRECTION;

/***structure type define***/

/*Define attributes of vpss channel*/
typedef struct fyVPSS_CHN_ATTR_S
{
    FY_BOOL bSpEn;      /*Not Support,Sharpen enable*/
    FY_BOOL bUVInvert;  /*Not Support,UV Invert enable*/
    FY_BOOL bBorderEn;  /*Not Support,Frame enable*/
    BORDER_S  stBorder; /*Not Support*/

    /*Extension Members*/
    CROP_INFO_S cropInfo;   /*CROP rectangular info*/
}VPSS_CHN_ATTR_S;

/*Not used*/
typedef struct fyVPSS_GRP_PARAM_S
{
    FY_U32 u32Contrast;          /*strength of dymanic contrast improve*/
    FY_U32 u32DieStrength;       /*strength of de-interlace,not used now*/
    FY_U32 u32IeStrength;        /*strength of image enhance*/
    FY_U32 u32SfStrength;        /*strength of y space filter*/
    FY_U32 u32TfStrength;        /*strength of y time filter*/
    FY_U32 u32CfStrength;        /*strength of c space filter*/
    FY_U32 u32CTfStrength;       /*strength of c time filter*/
    FY_U32 u32CvbsStrength;      /*strength of cvbs*/
    FY_U32 u32DeMotionBlurring;  /*strength of de motion blurring,not used now*/
}VPSS_GRP_PARAM_S;

/*Not used. Define detailed NR params for grp image process*/
typedef struct fyVPSS_NR_ADVANCED_PARAM_S
{
    FY_U32 u32Mdz;
    FY_U32 u32HTfRelaStrength;   /*relation strength of hard NR chn */

    /*only for weak nr chn*/
    FY_U32 u32Edz;
    FY_U32 u32WTfRelaStrength;   /*relation strength of weak NR chn */
}VPSS_NR_ADVANCED_PARAM_S;

/*Not used. Define attributes of CLIP function*/
typedef struct fyVPSS_CROP_INFO_S
{
    FY_BOOL bEnable;                            /*CROP enable*/
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /*Coordinate mode of the crop start point*/
    RECT_S  stCropRect;                         /*CROP rectangular*/
}VPSS_CROP_INFO_S;

/*Define attributes of YCbCr convert to RGB*/
typedef struct{
	FY_U32 y2rGain;
	FY_U32 cb2rGain;
	FY_U32 cr2rGain;

	FY_U32 y2gGain;
	FY_U32 cb2gGain;
	FY_U32 cr2gGain;

	FY_U32 y2bGain;
	FY_U32 cb2bGain;
	FY_U32 cr2bGain;

	FY_U32 compensateR;
	FY_U32 compensateG;
	FY_U32 compensateB;
} AI_YCbCr2RGB_S;

/*Define attributes of RGB*/
typedef struct{
	FY_U32 rOffset;
	FY_U32 gOffset;
	FY_U32 bOffset;
}AI_RGBCLAMP_S;

/*Define attributes of chn0 output bgm/ycmean/cpy */
typedef struct{
	FY_U32 bYcMeanEn;       /*ycmean enable*/
	DS_MODE_E ycmeanMode;   /*macroblock mode: 4,8,16*/
	FY_U32 bBgmYEn;         /*bgm enable*/
	FY_U32 bCpyEn;          /*cpy enable*/
} MAIN_CHN_INFO_S;

/*Define attributes of chn2*/
typedef struct{
	FY_U32 AiOutSel;                /*0:yuv 1:rgb*/
	FY_U32 RGBOutMode;              /*0:three output r,g,b  1: one output rgb*/
	FY_U32 RGBClampMode;
	FY_U32 AlphaValue;
	FY_U32 ARGBOrder;
	FY_U32 MeansR;
	FY_U32 MeansG;
	FY_U32 MeansB;
	FY_U32 Scale;
	AI_YCbCr2RGB_S yCbCr2RgbCfg;    /*yuv convert to rgb*/
	AI_RGBCLAMP_S rgbClampCfg;      /*rgb offset*/
} AI_CHN_INFO_S;

typedef struct fyVPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    FY_U32  u32MaxW;            /*MAX width of the group*/
    FY_U32  u32MaxH;            /*MAX height of the group*/
    PIXEL_FORMAT_E enPixFmt;    /*Pixel format*/

    FY_BOOL bIeEn;              /*Image enhance enable*/
    FY_BOOL bDciEn;             /*Not support,Dynamic contrast Improve enable*/
    FY_BOOL bNrEn;              /*Noise reduce enable*/
    FY_BOOL bHistEn;            /*Hist enable*/
    FY_BOOL bEsEn;              /*Not support,Edge smooth enable*/
    VPSS_DIE_MODE_E enDieMode;  /*Not support,De-interlace enable*/

    /*Extension Members*/
    FY_BOOL bApcEn;             /*apc enable*/
    FY_BOOL bPurpleEn;          /*purple enable*/
    FY_BOOL bYGammaEn;          /*ygamma enable*/
    FY_BOOL bChromaEn;          /*chnroma enable*/
    FY_BOOL bLcEn;              /*local chroma enable*/
}VPSS_GRP_ATTR_S;

/*Define vpss frame control info*/
typedef struct fyVPSS_FRAME_RATE_S
{
    FY_S32  s32SrcFrmRate;        /* Input frame rate of a  group*/
    FY_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

typedef struct
{
	FY_U32 bUseGloble;
	FY_S32 glob_idx;
	POINT_S pic_pos;
}VPSS_CHN_GLOBLE_CFG;

/*Define attributes of vpss channel's work mode*/
typedef struct fyVPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E  enChnMode;     /*Vpss channel's work mode*/
    FY_U32 u32Width;                /*Width of target image*/
    FY_U32 u32Height;               /*Height of target image*/
    FY_BOOL bDouble;                /*Not support,Field-frame transfer, only valid for VPSS_CHN2*/
    VPSS_FRAME_RATE_S stFrameRate;  /*Framerate control*/
    ASPECT_RATIO_S stAspectRatio;   /*Not support*/
    PIXEL_FORMAT_E  enPixelFormat;  /*Pixel format of target image*/
    COMPRESS_MODE_E enCompressMode; /*Compression mode of the output*/

    /*Extension Members*/
    COMPRESS_RATIO_E CompRate;      /*Compression rate of the output,if compress mode is tile or slice*/
    VPSS_CHN_GLOBLE_CFG GlobCfg;    /**/
    union
    {
        MAIN_CHN_INFO_S mainCfg;    /*Config channel 0 when used enable ycmean, bgm or cpy*/
        AI_CHN_INFO_S aiCfg;        /*Config channel 2 when use bgm output*/
    };

	FY_U32 YcMeanSel;				/*0: 1: 2:*/
	FY_U32 OutClipShort;			/**/
}VPSS_CHN_MODE_S;


/*Not used. Define detailed params for channel image process*/
typedef struct fyVPSS_CHN_PARAM_S
{
    FY_U32 u32SpStrength;
    FY_U32 u32SfStrength;
    FY_U32 u32TfStrength;
    FY_U32 u32CfStrength;
    FY_U32 u32DeMotionBlurring;
} VPSS_CHN_PARAM_S;

/*Not used. Define vpss prescale info*/
typedef struct fyVPSS_PRESCALE_INFO_S
{
    FY_BOOL bPreScale;       /* prescale enable*/
    SIZE_S  stDestSize;      /* destination size*/
}VPSS_PRESCALE_INFO_S;

/*Not used.Define vpss filter info*/
typedef struct fyVPSS_SIZER_INFO_S
{
    FY_BOOL bSizer;
    SIZE_S  stSize;
}VPSS_SIZER_INFO_S;


/*Not used.Define attributes of vpss extend channel*/
typedef struct fyVPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN        s32BindChn;             /*channel bind to*/
    FY_U32          u32Width;               /*Width of target image*/
    FY_U32          u32Height;              /*Height of target image*/
    FY_S32          s32SrcFrameRate;        /*Frame rate of source*/
    FY_S32          s32DstFrameRate;        /*Frame rate of extend chn input&output*/
    PIXEL_FORMAT_E  enPixelFormat;          /*Pixel format of target image*/
}VPSS_EXT_CHN_ATTR_S;

typedef struct fyVPSS_REGION_INFO_S
{
    RECT_S *pstRegion;      /*region attribute*/
    FY_U32 u32RegionNum;    /*count of the region*/
}VPSS_REGION_INFO_S;

typedef struct fyVPSS_MOD_PARAM_S
{
    FY_U32  u32VpssVbSource;    /*Not support*/
    FY_U32  u32MaxWidth;        /*Max width of private vb*/
    FY_U32  u32MaxHeight;       /*Max width of private vb*/
    FY_U32  u32BgmNum;          /*The count of bgm private vb*/
    FY_U32  u32CpyNum;          /*The count of cpy private vb*/
    FY_U32  u32YMeanNum;        /*The count of ymean private vb*/
} VPSS_MOD_PARAM_S;

typedef struct fyVPSS_LOGO_COLOR
{
	FY_U32  logo_idx;                           /*The index of osd*/
	FY_U32  idx_offset;                         /*The index of color,0:0-127 1:128-255*/
	FY_U32  color[VPSS_MAX_GOSD_COLOR_IDX/2];   /*color info*/
}VPSS_LOGO_COLOR_INFO;

typedef struct fyVPU_LOGO_CFG
{
	PIXEL_FORMAT_E logoPiexlFmt;  /*Pixel format of osd*/
	FY_U32 bCmpEn;                /*ARGB1555 cmp en */
	unsigned char *logo_addr;     /*phyaddress*/
	POINT_S logo_startpos;        /*Start point of osd*/
	POINT_S logo_cutpos;          /*Crop point of osd*/
	FY_U32 stride;                /*Stride  of osd*/
	SIZE_S logo_size;             /*Width and height of osd*/
	POINT_S cmp_logo_startpos;   /*ARGB1555 cmp crop pos*/
	SIZE_S cmp_logo_size;        /*ARGB1555 cmp crop size*/
	FY_U32 tofullen;
	FY_U32 extmode;
	FY_U32 pichdtv;
	FY_U32 rgbmode;             /*0: stdio RGB, 1: computer RGB*/ //picrange
	FY_U32 bGlobalAlphaEn;       /*The value of global alpha*/
	FY_U32 globalAlpha;          /*The value of global alpha*/
	FY_U32 bMagicColorEn;        /*magic color enable*/
	FY_U32 magicColor;           /*format is ARGB0565, magic color is transparent*/
}VPSS_LOGO_CFG;

typedef struct
{
	FY_U32 logo_idx;            /*The index of osd*/
	FY_U32 bLogoEn;             /*OSD enable,0:disable 1:enable*/
	VPSS_LOGO_CFG logo_cfg;     /*The index of osd*/
}VPSS_LOGO_INFO;

typedef struct
{
	FY_U32  masaic_mode;        /*Masaic mode,0:cover mode 1:mosaic mode*/
	FY_U32  color;              /*Cover mode color*/
	FY_U32  masaic_size;        /*Masaic mode,0:16*16,1:32*32*/
}MASK_CFG;

typedef struct
{
	FY_U32  mask_enable[VPSS_MAX_MASK_AREA];    /*Control masaic enable*/
	RECT_S  area_value[VPSS_MAX_MASK_AREA];     /*Control cover enable*/
	MASK_CFG    masaic;                         /*Info of masaic or cover*/
}VPSS_MASK_INFO_S;

/*NR3D,APC,PURPLE,YGAMMA,CHROMA,LOCALCHROMA*/

/*Advanced nr3d parameters, usually not used */
typedef struct fyVPSS_YCNR_INFO_S
{
	FY_U32 swshift;
	FY_U32 YDscaler;
	FY_U32 UTH1;
	FY_U32 VTH1;
	FY_U32 UTH2;
	FY_U32 VTH2;
	FY_U32 UTH3;
	FY_U32 VTH3;
	FY_U32 ULScaler;
	FY_U32 VLScaler;
	FY_U32 LdLTH;
	FY_U32 Twshift;
}VPSS_YCNR_INFO_S;

typedef struct fyVPSS_APC_INFO_S
{
	FY_U32 pgain;   //apc de pgain 0~255
	FY_U32 ngain;   //apc de ngain 0~255
	FY_U32 edgeLv;  //apc es pgain 0~255
	FY_U32 detailLv;//apc es ngain 0~255
} VPSS_APC_INFO_S;

/*Advanced hist parameters, usually not used */
typedef struct{
	FY_S32 milliSec;
	FY_U32 histBin[33][2];
}VPSS_HIST_STAT_S;

typedef struct
{
	FY_S32 glob_idx;            /*globle image index*/
	FY_BOOL bEnable;            /*1:enable,malloc buffer*/
	PIXEL_FORMAT_E glb_pixfmt;  /*Pixel format*/
	FY_U32 frame_rate;          /*Not used now*/
	SIZE_S tal_size;            /*globle image info*/
}VPSS_GLOBLE_DISPLAY_INIT_CFG;

typedef struct fyVPSS_PIC_DATA_S
{
    FY_S32      milliSec;       /*timeout value*/
    FY_U32      u32PoolId;      /*buffer pool id*/
    FY_U32      u32Width;       /*width of frame*/
    FY_U32      u32Height;      /*height of frame*/
    FY_U32      u32PhyAddr;     /*phyadrr of frame*/
    FY_VOID     *pVirAddr;      /*virtual addr of frame*/
    FY_U64      u64pts;         /*pts of data*/
    FY_U32      u32TimeRef;     /*frame index*/
}VPSS_PIC_DATA_S;

typedef struct fyVPU_GRP_EFFECT_S
{
    FY_U32 u32Contrast;         /*Contrast vaule of image */
    FY_U32 u32Brightness;       /*Brightness vaule of image */
    FY_U32 u32IeStrength;       /*Sharpness vaule of image */
    FY_U32 u32Saturation;       /*Saturation vaule of image */
}VPU_GRP_EFFECT_S;

typedef struct
{
	PIC_DIRECTION scaler_dir;   /*Scalce direction*/
	FY_U32 level;               /*Scalce level:0-15*/
}VPU_SCALER_COEFF;


/**********************************************************************
    PART III    - Error Code
**********************************************************************/

#define FY_ERR_VPSS_NULL_PTR                    FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define FY_ERR_VPSS_NOTREADY                    FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define FY_ERR_VPSS_INVALID_DEVID               FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define FY_ERR_VPSS_INVALID_CHNID               FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define FY_ERR_VPSS_EXIST                       FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define FY_ERR_VPSS_UNEXIST                     FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define FY_ERR_VPSS_NOT_SUPPORT                 FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define FY_ERR_VPSS_NOT_PERM                    FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define FY_ERR_VPSS_NOMEM                       FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define FY_ERR_VPSS_NOBUF                       FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define FY_ERR_VPSS_ILLEGAL_PARAM               FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define FY_ERR_VPSS_BUSY                        FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define FY_ERR_VPSS_BUF_EMPTY                   FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define FY_ERR_VPU_HAS_BINDED		            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_VPU_GRP_HAS_BINDED)
#define FY_ERR_VPU_NOT_BINDED		            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_VPU_GRP_NOT_BINDED)
#define FY_ERR_VPU_CHN_NOT_ENABLE	            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_CHN_NOT_ENABLE)
#define FY_ERR_VPU_CHN_GET_TIMEOUT	            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_CHN_GET_TIMEOUT)
#define FY_ERR_VPU_GRP_NOT_START	            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_GRP_NOT_START)
#define FY_ERR_VPU_GRP_NOT_STOP	                FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_GRP_NOT_STOP)
#define FY_ERR_VPU_GLOBLE_NOT_ENABLE            FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_GLOBLE_NOT_ENABLE)
#define FY_ERR_VPU_SET_SCALER_LEVEL_ERROR       FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_SET_SCALER_LEVEL_ERROR)
#define FY_ERR_VPU_GRP_NOT_DESTORY              FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_GRP_NOT_DESTROY)
#define FY_ERR_VPU_WAIT_FRAME_TIMEOUT           FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_GRP_WAIT_FRAME_TIMEOUT)
#define FY_ERR_VPU_DISABLE_CHN_TIMEOUT          FY_DEF_ERR(FY_ID_VPSS, EN_ERR_LEVEL_ERROR, ERR_VPU_CHN_NOT_DISABLE)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __FY_COMM_VPSS_H__ */


