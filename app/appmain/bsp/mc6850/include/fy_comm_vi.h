#ifndef __FY_COMM_VI_H__
#define __FY_COMM_VI_H__

#include "fy_common.h"
#include "fy_errno.h"
#include "fy_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**********************************************************************
    - Macro
**********************************************************************/

#define VI_INVALID_FRMRATE          (-1UL)
#define VIU_MAX_USER_FRAME_DEPTH    8

/**********************************************************************
    - Structure and Enumeration
**********************************************************************/

typedef enum fyEN_VIU_ERR_CODE_E
{
    ERR_VI_FAILED_NOTENABLE = 64,   /* device or channel not enable             */
    ERR_VI_FAILED_NOTDISABLE,       /* device not disable                       */
    ERR_VI_FAILED_CHNOTDISABLE,     /* channel not disable                      */
    ERR_VI_CFG_TIMEOUT,             /* config timeout                           */
    ERR_VI_NORM_UNMATCH,            /* video norm of ADC and VIU is unmatch     */
    ERR_VI_INVALID_WAYID,           /* invlalid way ID                          */
    ERR_VI_INVALID_PHYCHNID,        /* invalid phychn id                        */
    ERR_VI_FAILED_NOTBIND,          /* device or channel not bind               */
    ERR_VI_FAILED_BINDED,           /* device or channel not unbind             */
} EN_VIU_ERR_CODE_E;

/* interface mode of video input */
typedef enum fyVI_INTF_MODE_E
{
    VI_MODE_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2            */
    VI_MODE_BT601,                  /* ITU-R BT.601 YUV4:2:2            */
    VI_MODE_DIGITAL_CAMERA,         /* digatal camera mode              */
    VI_MODE_BT1120_STANDARD,        /* BT.1120 progressive mode         */
    VI_MODE_BT1120_INTERLEAVED,     /* BT.1120 interstage mode          */

    VI_MODE_BUTT
} VI_INTF_MODE_E;


/* Input mode */
typedef enum fyVI_INPUT_MODE_E
{
    VI_INPUT_MODE_BT656 = 0,        /* ITU-R BT.656 YUV4:2:2 */
    VI_INPUT_MODE_BT601,            /* ITU-R BT.601 YUV4:2:2 */
    VI_INPUT_MODE_DIGITAL_CAMERA,   /* digatal camera mode   */
    VI_INPUT_MODE_INTERLEAVED,

    VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;

typedef enum fyVI_WORK_MODE_E
{
    VI_WORK_MODE_1Multiplex=0,      /* 1 Multiplex mode */
    VI_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    VI_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;



/* whether an input picture is interlaced or progressive */
typedef enum fyVI_SCAN_MODE_E
{
    VI_SCAN_INTERLACED  = 0,
    VI_SCAN_PROGRESSIVE,

    VI_SCAN_BUTT,
} VI_SCAN_MODE_E;

typedef enum fyVI_DATA_YUV_SEQ_E
{
   /*The input sequence of the second component(only contains u and v) in BT.1120 mode */
    VI_INPUT_DATA_VUVU = 0,             /* Not Support                      */
    VI_INPUT_DATA_UVUV,                 /* Not Support                      */

   /* The input sequence for yuv */
    VI_INPUT_DATA_UYVY = 0,
    VI_INPUT_DATA_VYUY,                 /* Not Support                       */
    VI_INPUT_DATA_YUYV,
    VI_INPUT_DATA_YVYU,                 /* Not Support                       */
    VI_INPUT_DATA_Y,                    /* Extension Member: only Y sequence */

    VI_DATA_YUV_BUTT
} VI_DATA_YUV_SEQ_E;

typedef enum fyVI_CLK_EDGE_E
{
    VI_CLK_EDGE_SINGLE_UP = 0,          /* single-edge mode and in rising edge  */
    VI_CLK_EDGE_SINGLE_DOWN,            /* single-edge mode and in falling edge */
    VI_CLK_EDGE_DOUBLE ,                /* Double edge mode                     */

    VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

typedef enum fyVI_COMP_MODE_E
{
    VI_COMP_MODE_SINGLE = 0,           /* in single component mode */
    VI_COMP_MODE_DOUBLE = 1,           /* in double component mode */
    VI_COMP_MODE_BUTT,
}VI_COMP_MODE_E;

/* Y/C composite or separation mode */
typedef enum fyVI_COMBINE_MODE_E
{
    VI_COMBINE_COMPOSITE = 0,           /* Composite mode */
    VI_COMBINE_SEPARATE,                /* Separate mode */
    VI_COMBINE_BUTT,
} VI_COMBINE_MODE_E;

/* Attribute of the vertical synchronization signal */
typedef enum fyVI_VSYNC_E
{
    VI_VSYNC_FIELD = 0,           /* Field/toggle mode:a signal reversal means a new frame or a field */
    VI_VSYNC_PULSE,               /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */ //not support
} VI_VSYNC_E;

/* Polarity of the vertical synchronization signal */
typedef enum fyVI_VSYNC_NEG_E
{
    VI_VSYNC_NEG_HIGH = 0,        /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is high-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is positive pulse.*/
    VI_VSYNC_NEG_LOW              /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is low-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is negative pulse.*/
} VI_VSYNC_NEG_E;

/* Attribute of the horizontal synchronization signal */
typedef enum fyVI_HSYNC_E
{
    VI_HSYNC_VALID_SINGNAL = 0,   /* the horizontal synchronization is valid signal mode */
    VI_HSYNC_PULSE,               /* the horizontal synchronization is pulse mode, a new pulse means the beginning of a new line */ //Not support
} VI_HSYNC_E;

/* Polarity of the horizontal synchronization signal */
typedef enum fyVI_HSYNC_NEG_E
{
    VI_HSYNC_NEG_HIGH = 0,        /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is high-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is positive pulse */
    VI_HSYNC_NEG_LOW              /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is low-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is negative pulse */
} VI_HSYNC_NEG_E;

/* Attribute of the valid vertical synchronization signal */
typedef enum fyVI_VSYNC_VALID_E
{
    VI_VSYNC_NORM_PULSE = 0,      /* the vertical synchronization is pusle mode, a pusle means a new frame or field  */ //Not Support
    VI_VSYNC_VALID_SINGAL,        /* the vertical synchronization is effective mode, a effective signal means a new frame or field */
} VI_VSYNC_VALID_E;

/* Polarity of the valid vertical synchronization signal */
typedef enum fyVI_VSYNC_VALID_NEG_E
{
    VI_VSYNC_VALID_NEG_HIGH = 0,  /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a positive pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is high-level */
    VI_VSYNC_VALID_NEG_LOW        /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a negative pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is low-level */
} VI_VSYNC_VALID_NEG_E;


typedef enum fyVI_EC_LEVEL_E
{
    VI_EC_LEVEL_NONE = 0,      /* Non-compress                       */
    VI_EC_BYTE_MODE_128byte,   /* Compress ratio: 0.5   (128/256)    */
    VI_EC_BYTE_MODE_160byte,   /* Compress ratio: 0.625 (160/256)    */
    VI_EC_BYTE_MODE_192byte,   /* Compress ratio: 0.75  (192/256)    */
}VI_EC_LEVEL_E;


/* the highest bit of the BT.656 timing reference code*/
typedef enum fyBT656_FIXCODE_E
{
    BT656_FIXCODE_1 = 0,       /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 1.*/
    BT656_FIXCODE_0            /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 0.*/
}BT656_FIXCODE_E;

/* Polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum fyBT656_FIELD_POLAR_E
{
    BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1*/
    BT656_FIELD_POLAR_NSTD     /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0*/
}BT656_FIELD_POLAR_E;


typedef enum fyVI_VBI_LOCAL_E
{
    VI_VBI_LOCAL_ODD_FRONT = 0,
    VI_VBI_LOCAL_ODD_END,
    VI_VBI_LOCAL_EVEN_FRONT,
    VI_VBI_LOCAL_EVEN_END,
    VI_VBI_LOCAL_BUTT
} VI_VBI_LOCAL_E;

typedef enum fyVI_VBI_MODE_E
{
    VI_VBI_MODE_8BIT  = 0,
    VI_VBI_MODE_6BIT ,

    VI_VBI_MODE_BUTT
} VI_VBI_MODE_E;

typedef enum fyVI_DATA_TYPE_E
{
    VI_DATA_TYPE_YUV = 0,
    VI_DATA_TYPE_RGB = 1,
    VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

typedef enum fyVI_DATA_PATH_E
{
    VI_PATH_BYPASS    = 0,        /* ISP bypass */
    VI_PATH_ISP       = 1,        /* ISP enable */
    VI_PATH_RAW       = 2,        /* Capture raw data, for debug */
    VI_PATH_BUTT
}VI_DATA_PATH_E;

typedef enum fyVI_CH2MODE_E
{
    VI_CH2MODE_2CH_PIX_INTERLEAVE_AUTO = 0,
    VI_CH2MODE_2835,
    VI_CH2MODE_2816_2864_5158,
    VI_CH2MODE_FORCE_SEP,
    VI_CH2MODE_BUTT,
} VI_CH2MODE_E;

typedef enum fyVI_CH4MODE_E
{
    VI_CH4MODE_4CH_PIX_INTERLEAVE_AUTO = 0,
    VI_CH4MODE_CHNID_IN_SAV_EAV,
    VI_CH4MODE_CHNID_IN_BLANKING,
    VI_CH4MODE_FORCE_SEP,
    VI_CH4MODE_BUTT,
} VI_CH4MODE_E;

typedef enum fy_VI_USERPIC_MODE_E
{
    VI_USERPIC_MODE_PIC = 0,            /* YUV picture */
    VI_USERPIC_MODE_BGC,                /* Background picture only with a color */
    VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

/* captrue selection of video input */
typedef enum fyVI_CAPSEL_E
{
    VI_CAPSEL_TOP = 0,                  /* top field */
    VI_CAPSEL_BOTTOM,                   /* bottom field */
    VI_CAPSEL_BOTH,                     /* top and bottom field */
    VI_CAPSEL_BUTT
} VI_CAPSEL_E;


typedef enum fyVI_SKIP_MODE_E
{
    VI_SKIP_NONE = 0,                   /* default mode, no skip      */
    VI_SKIP_YES,                        /* default skip mode,1/2 skip or 3/4 skip*/
    VI_SKIP_4TO2,                       /* Not Support: 2/4 skip mode*/
    VI_SKIP_BUTT,
}VI_SKIP_MODE_E;


/* Blank information of the input timing */
typedef struct fyVI_TIMING_BLANK_S
{
    FY_U32 u32HsyncHfb ;    /* Horizontal front blanking width */
    FY_U32 u32HsyncAct ;    /* Horizontal effetive width */
    FY_U32 u32HsyncHbb ;    /* Horizontal back blanking width */
    FY_U32 u32VsyncVfb ;    /* Vertical front blanking height of one frame or odd-field frame picture */
    FY_U32 u32VsyncVact ;   /* Vertical effetive width of one frame or odd-field frame picture */
    FY_U32 u32VsyncVbb ;    /* Vertical back blanking height of one frame or odd-field frame picture */
    FY_U32 u32VsyncVbfb ;   /* Even-field vertical front blanking height when input mode is interlace (invalid when progressive input mode) */
    FY_U32 u32VsyncVbact ;  /* Even-field vertical effetive width when input mode is interlace (invalid when progressive input mode) */
    FY_U32 u32VsyncVbbb ;   /* Even-field vertical back blanking height when input mode is interlace (invalid when progressive input mode) */
}VI_TIMING_BLANK_S;

/* synchronization information about the BT.601 or DC timing */
typedef struct fyVI_SYNC_CFG_S
{
    VI_VSYNC_E             enVsync;
    VI_VSYNC_NEG_E         enVsyncNeg;
    VI_HSYNC_E             enHsync;
    VI_HSYNC_NEG_E         enHsyncNeg;
    VI_VSYNC_VALID_E       enVsyncValid;
    VI_VSYNC_VALID_NEG_E   enVsyncValidNeg;
    VI_TIMING_BLANK_S      stTimingBlank;
} VI_SYNC_CFG_S;

typedef struct fyVI_BT656_SYNC_CFG_S
{
    BT656_FIXCODE_E         enFixCode;
    BT656_FIELD_POLAR_E     enFieldPolar;
}VI_BT656_SYNC_CFG_S;

typedef struct fyVI_VBI_ATTR_S
{
    VI_VBI_LOCAL_E enLocal; /* location of VBI */
    FY_S32 s32X;            /* horizontal original position of the VBI data */
    FY_S32 s32Y;            /* vertical original position of the VBI data */
    FY_U32 u32Len;          /* length of VBI data, by word(4 Bytes) */
} VI_VBI_ATTR_S;

typedef struct fyVI_VBI_ARG_S
{
    VI_VBI_ATTR_S stVbiAttr[2];
}VI_VBI_ARG_S;


/* the extended attributes of VI device */
typedef struct fyVI_DEV_ATTR_EX_S
{
    VI_INPUT_MODE_E     enInputMode;        /* Input mode */
    VI_WORK_MODE_E      enWorkMode;         /* 1-, 2-, or 4-channel multiplexed work mode */

    VI_COMBINE_MODE_E   enCombineMode;      /* Not Support: Y/C composite or separation mode */
    VI_COMP_MODE_E      enCompMode;         /* Not Support: Component mode (single-component or dual-component) */
    VI_CLK_EDGE_E       enClkEdge;          /* Clock edge mode (sampling on the rising or falling edge) */

    FY_U32              au32CompMask[2];    /* Not Support: Component mask */

    FY_S32              s32AdChnId[4];      /* Not Support: AD channel ID. Typically, the default value -1 is recommended */

    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Not Support: Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Not Support: Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_BT656_SYNC_CFG_S stBT656SynCfg;      /* Not Support: Sync timing. This member must be configured in BT.656 mode */

    VI_DATA_PATH_E      enDataPath;         /* Not Support: ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* Not Support: RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */

    FY_BOOL             bDataRev;           /* Data reverse */


} VI_DEV_ATTR_EX_S;

/* the attributes of a VI device */
typedef struct fyVI_DEV_ATTR_S
{
    VI_INTF_MODE_E      enIntfMode;         /* Interface mode */
    VI_WORK_MODE_E      enWorkMode;         /* 1-, 2-, or 4-channel multiplexed work mode */

    FY_U32              au32CompMask[2];    /* Not Support: Component mask */

    VI_CLK_EDGE_E       enClkEdge;          /* Clock edge mode (sampling on the rising or falling,double edge) */
    FY_S32              s32AdChnId[4];      /* Not Support: AD channel ID. Typically, the default value -1 is recommended */

    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Not Support: Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Not Support: Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_DATA_PATH_E      enDataPath;         /* Not Support: ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* Not Support: RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */ //just support YUV, delete

    FY_BOOL             bDataRev;           /* Data reverse */

} VI_DEV_ATTR_S;


typedef struct fyVI_CHN_BIND_ATTR_S
{
    VI_DEV ViDev;
    VI_WAY ViWay;
} VI_CHN_BIND_ATTR_S;

/* the attributes of a VI way */
typedef struct fyVI_WAY_ATTR_S
{
   FY_S32 s32AdChnId;
} VI_WAY_ATTR_S;


/* the attributes of a VI channel */
typedef struct fyVI_CHN_ATTR_S
{
    RECT_S          stCapRect;          /* the capture rect (corresponding to the size of the picture captured by a VI device).
                                                For primary channels, the stCapRect's u32Width and u32Height are static attributes. That is,
                                                the value of them can be changed only after primary and secondary channels are disabled.
                                                For secondary channels, stCapRect is an invalid attribute */
    SIZE_S          stDestSize;         /* Target picture size.
                                                For primary channels, stDestSize must be equal to stCapRect's u32Width and u32Height,
                                                because primary channel doesn't have scale capability. Additionally, it is a static
                                                attribute, That is, the value of stDestSize can be changed only after primary and
                                                secondary channels are disabled.
                                                For secondary channels, stDestSize is a dynamic attribute */

    VI_CAPSEL_E     enCapSel;           /* Frame/field select. It is used only in interlaced mode.
                                                For primary channels, enCapSel is a static attribute */
    VI_SCAN_MODE_E  enScanMode;         /* Input scanning mode (progressive or interlaced) */
    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
    FY_BOOL         bMirror;            /* Whether to mirror */
    FY_BOOL         bFlip;              /* Whether to flip */
    FY_S32          s32SrcFrameRate;    /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    FY_S32          s32DstFrameRate;    /* Target frame rate. The value -1 indicates that the frame rate is not controlled */

    /* Extension Members */
    SIZE_S          stSensorSize;       /* Sensor picture size. */
    VI_DATA_YUV_SEQ_E enDataSeq;        /* Input data sequence (only the YUV format is supported) */

} VI_CHN_ATTR_S;

typedef struct fyVI_CHN_STAT_S
{
    FY_BOOL bEnable;                    /* Whether this channel is enabled */
    FY_U32 u32IntCnt;                   /* The video frame interrupt count */
    FY_U32 u32FrmRate;                  /* current frame rate */
    FY_U32 u32LostInt;                  /* The interrupt is received but nobody care */
    FY_U32 u32VbFail;                   /* Video buffer malloc failure */
    FY_U32 u32PicWidth;                 /* curren pic width */
    FY_U32 u32PicHeight;                /* current pic height */
    FY_U32 u32AutoDisInt;               /* auto disable interrupt count, when VIU detected too many interrupts */

    /* Extension Members */
    FY_U32 u32Sync;                  /* current sync state*/
    FY_U32 u32VDet;                  /* video check state*/
    FY_U32 u32ScanMode;              /* check input scan mode:VI_SCAN_INTERLACED or VI_SCAN_PROGRESSIVE*/
} VI_CHN_STAT_S;


typedef struct fyVI_USERPIC_BGC_S
{
    FY_U32          u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct fyVI_USERPIC_ATTR_S
{
    FY_BOOL                 bPub;          /* Whether the user picture information is shared by all VI devices and channels*/
    VI_USERPIC_MODE_E       enUsrPicMode;  /* User picture mode */
    union
    {
        VIDEO_FRAME_INFO_S  stUsrPicFrm;   /* Information about a YUV picture */
        VI_USERPIC_BGC_S    stUsrPicBg;    /* Information about a background picture only with a color */
    }unUsrPic;
} VI_USERPIC_ATTR_S;

typedef struct fyVI_USR_GET_FRM_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S  stVFrame;
    FY_S32              s32MilliSec;
} VI_USR_GET_FRM_TIMEOUT_S;

typedef struct fyVI_EXT_CHN_ATTR_S
{
    VI_CHN    s32BindChn;               /* The channel num which extend channel will bind to*/
    SIZE_S    stDestSize;               /* Target size*/

    FY_S32    s32SrcFrameRate;          /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    FY_S32    s32DstFrameRate;          /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
}VI_EXT_CHN_ATTR_S;

typedef struct fyVI_CHN_LUM_S
{
    FY_U32 u32Lum;                      /* Luma sum of current frame */
    FY_U64 u64Pts;                      /* PTS of current frame */
} VI_CHN_LUM_S;

typedef struct fyVI_SKIP_MODE_EX_S
{
    FY_BOOL bEnable;
    FY_U32  u32YMask;                   /* 0b0000 ~ 0b1111 in binary, horizontal skip mask*/
    FY_U32  u32CMask;                   /* cmask,0000-1111 in binary*/
}VI_SKIP_MODE_EX_S;

typedef struct fyVI_MOD_PARAM_S
{
    FY_S32  detect_err_frame;
    FY_U32  u32ViVbSource;
    //Extension Parameter
    FY_U32 buff_size;                   /* viu lld buffer size */
    FY_U8 userpic_mode;                /* 0: dynamic size by the channel; 1: fixed size */
    FY_U8 vb_optimize;                 /* 0: disable; 1: enable */
    FY_U8 resv[2];
} VI_MOD_PARAM_S;



typedef struct fyVI_BLOCK_COLOR_S
{
    FY_S32  s32YColor;
    FY_S32  s32CbColor;
    FY_S32  s32CrColor;
}VI_BLOCK_COLOR_S;

typedef struct fyVI_BLOCK_S
{
    FY_U32  u32ChnNum;
    FY_BOOL bBlkByPass[4];
    RECT_S  stBlkRect[4];
    VI_BLOCK_COLOR_S stBlkColor[4];
}VI_BLOCK_S;

typedef struct fyVI_DLL_CTRL_S
{
    FY_U32  u32DevNum;
    FY_U32  u32MasterStartP;
    FY_U32  u32PhaseStartP;
    FY_U32  u32ByPassMode;

    FY_U32  u32RstDll;
    FY_U32  u32ReSyncDll;
    FY_U32  u32PhyClkEn;
    FY_U32  u32DelayData;
}VI_DLL_CTRL_S;


typedef struct fyVI_DLL_STATUS_S
{
    VI_DLL_CTRL_S stDllCtrl;
    FY_U32  u32DllLock;
    FY_U32  u32DllLockMode;
    FY_U32  u32DllLockValue;
    FY_U32  u32DecOut;
}VI_DLL_STATUS_S;

/**********************************************************************
    - Extend command
**********************************************************************/
enum {
    VIU_EXTCMD_SET_CHN_FIELDPOL = 0x6900, /* param: int */
    VIU_EXTCMD_BUTT,
};

/**********************************************************************
    - Error Code
**********************************************************************/

/* VIU Error Code */
#define FY_ERR_VI_INVALID_PARA          FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define FY_ERR_VI_INVALID_DEVID         FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define FY_ERR_VI_INVALID_CHNID         FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define FY_ERR_VI_INVALID_NULL_PTR      FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define FY_ERR_VI_FAILED_NOTCONFIG      FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define FY_ERR_VI_SYS_NOTREADY          FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define FY_ERR_VI_BUF_EMPTY             FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define FY_ERR_VI_BUF_FULL              FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define FY_ERR_VI_NOMEM                 FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define FY_ERR_VI_NOT_SUPPORT           FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define FY_ERR_VI_BUSY                  FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define FY_ERR_VI_NOT_PERM              FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define FY_ERR_VI_FAILED_NOTENABLE      FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)
#define FY_ERR_VI_FAILED_NOTDISABLE     FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)
#define FY_ERR_VI_FAILED_CHNOTDISABLE   FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)
#define FY_ERR_VI_CFG_TIMEOUT           FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)
#define FY_ERR_VI_NORM_UNMATCH          FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)
#define FY_ERR_VI_INVALID_WAYID         FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)
#define FY_ERR_VI_INVALID_PHYCHNID      FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)
#define FY_ERR_VI_FAILED_NOTBIND        FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)
#define FY_ERR_VI_FAILED_BINDED         FY_DEF_ERR(FY_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__FY_COMM_VIDEO_IN_H__ */


