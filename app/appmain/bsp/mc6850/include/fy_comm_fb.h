#ifndef __FY_COMM_FB_H__
#define __FY_COMM_FB_H__

#include "fy_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


typedef enum
{
    FYFB_FMT_RGB565 = 0,
    FYFB_FMT_RGB888,                           /* RGB888 24bpp */
    FYFB_FMT_KRGB444,                          /* RGB444 16bpp */
    FYFB_FMT_KRGB555,                          /* RGB555 16bpp */
    FYFB_FMT_KRGB888,                          /* RGB888 32bpp */
    FYFB_FMT_ARGB4444,                         /* ARGB4444 */
    FYFB_FMT_ARGB1555,                         /* ARGB1555 */
    FYFB_FMT_ARGB8888,                         /* ARGB8888 */
    FYFB_FMT_ARGB8565,                         /* ARGB8565 */
    FYFB_FMT_RGBA4444,                         /* ARGB4444 */
    FYFB_FMT_RGBA5551,                         /* RGBA5551 */
    FYFB_FMT_RGBA5658,                         /* RGBA5658 */
    FYFB_FMT_RGBA8888,                         /* RGBA8888 */
    FYFB_FMT_BGR565,                           /* BGR565 */
    FYFB_FMT_BGR888,                           /* BGR888 */
    FYFB_FMT_ABGR4444,                         /* ABGR4444 */
    FYFB_FMT_ABGR1555,                         /* ABGR1555 */
    FYFB_FMT_ABGR8888,                         /* ABGR8888 */
    FYFB_FMT_ABGR8565,                         /* ABGR8565 */
    FYFB_FMT_KBGR444,                          /* BGR444 16bpp */
    FYFB_FMT_KBGR555,                          /* BGR555 16bpp */
    FYFB_FMT_KBGR888,                          /* BGR888 32bpp */
    FYFB_FMT_1BPP,                             /* clut1 */
    FYFB_FMT_2BPP,                             /* clut2 */
    FYFB_FMT_4BPP,                             /* clut4 */
    FYFB_FMT_8BPP,                             /* clut8 */
    FYFB_FMT_ACLUT44,                          /* AClUT44*/
    FYFB_FMT_ACLUT88,                          /* ACLUT88 */
    FYFB_FMT_PUYVY,                            /* UYVY */
    FYFB_FMT_PYUYV,                            /* YUYV */
    FYFB_FMT_PYVYU,                            /* YVYU */
    FYFB_FMT_YUV888,                           /* YUV888 */
    FYFB_FMT_AYUV8888,                         /* AYUV8888 */
    FYFB_FMT_YUVA8888,                         /* YUVA8888 */
    FYFB_FMT_BUTT
}FYFB_COLOR_FMT_E;

/* refresh mode */
typedef enum
{
    FYFB_LAYER_BUF_DOUBLE = 0x0,               /* 2 display buf in fb */
    FYFB_LAYER_BUF_ONE    = 0x1,               /* 1 display buf in fb */
    FYFB_LAYER_BUF_NONE   = 0x2,               /* no display buf in fb,the buf user refreshed will be directly set to VO */
    FYFB_LAYER_BUF_DOUBLE_IMMEDIATE=0x3,       /* 2 display buf in fb, each refresh will be displayed */
    FYFB_LAYER_BUF_BUTT
} FYFB_LAYER_BUF_E;

/* antiflicker level */
/* Auto means fb will choose a appropriate antiflicker level automatically according to the color info of map */
typedef enum
{
    FYFB_LAYER_ANTIFLICKER_NONE = 0x0,         /* no antiflicker*/
    FYFB_LAYER_ANTIFLICKER_LOW = 0x1,          /* low level*/
    FYFB_LAYER_ANTIFLICKER_MIDDLE = 0x2,       /* middle level*/
    FYFB_LAYER_ANTIFLICKER_HIGH = 0x3,         /* fygh level*/
    FYFB_LAYER_ANTIFLICKER_AUTO = 0x4,         /* auto*/
    FYFB_LAYER_ANTIFLICKER_BUTT
}FYFB_LAYER_ANTIFLICKER_LEVEL_E;

/* layer info maskbit */
typedef enum
{
    FYFB_LAYERMASK_BUFMODE = 0x1,              /* BUFMODE bitmask */
    FYFB_LAYERMASK_ANTIFLICKER_MODE = 0x2,     /* ANTIFLICKER_MODE bitmask */
    FYFB_LAYERMASK_POS = 0x4,                  /* the position bitmask */
    FYFB_LAYERMASK_CANVASSIZE = 0x8,           /* canvassize bitmask */
    FYFB_LAYERMASK_DISPSIZE = 0x10,            /* displaysize bitmask */
    FYFB_LAYERMASK_SCREENSIZE = 0x20,          /* screensize bitmask */
    FYFB_LAYERMASK_BMUL = 0x40,                /* pre-mult bitmask */
    FYFB_LAYERMASK_BUTT
}FYFB_LAYER_INFO_MASKBIT;


typedef struct
{
    FY_U32              u32Width;
    FY_U32              u32Height;
}FYFB_SIZE_S;

typedef struct
{
    FY_BOOL             bKeyEnable;            /* colorkey enable flag */
    FY_U32              u32Key;                /* colorkey value, maybe contains alpha */
}FYFB_COLORKEY_S;

typedef struct
{
    FY_S32              x;
    FY_S32              y;
    FY_S32              w;
    FY_S32              h;
} FYFB_RECT;

typedef struct
{
    FY_S32              s32XPos;               /* horizontal position */
    FY_S32              s32YPos;               /* vertical position */
}FYFB_POINT_S;

typedef struct fyFB_DEFLICKER_S
{
    FY_U32              u32HDfLevel;           /* horizontal deflicker level */
    FY_U32              u32VDfLevel;           /* vertical deflicker level */
    FY_U8*              pu8HDfCoef;            /* horizontal deflicker coefficient */
    FY_U8*              pu8VDfCoef;            /* vertical deflicker coefficient */
}FYFB_DEFLICKER_S;

/* Alpha info */
typedef struct
{
    FY_BOOL             bAlphaEnable;          /* alpha enable flag */
    FY_BOOL             bAlphaChannel;         /* alpha channel enable flag */
    FY_U8               u8Alpha0;              /* alpha0 value, used in ARGB1555 */
    FY_U8               u8Alpha1;              /* alpha1 value, used in ARGB1555 */
    FY_U8               u8GlobalAlpha;         /* global alpha value */
    FY_U8               u8Reserved;
}FYFB_ALPHA_S;

typedef struct
{
    FY_BOOL             bKeyRgb;
    FY_BOOL             bKeyAlpha;             /* whether support colorkey alpha */
    FY_BOOL             bGlobalAlpha;          /* whether support global alpha */
    FY_BOOL             bCmap;                 /* whether support color map */
    FY_BOOL             bHasCmapReg;           /* whether has color map register*/
    FY_BOOL             bColFmt[FYFB_FMT_BUTT]; /* support which color format */
    FY_BOOL             bVoScale;              /* support vo scale*/
    FY_BOOL             bLayerSupported;       /* whether support a certain layer, for example:x5 HD support FYFB_SD_0 not support FYFB_SD_1 */
    FY_U32              u32MaxWidth;           /* the max pixels per line */
    FY_U32              u32MaxHeight;          /* the max lines */
    FY_U32              u32MinWidth;           /* the min pixels per line */
    FY_U32              u32MinHeight;          /* the min lines */
    FY_U32              u32VDefLevel;          /* vertical deflicker level, 0 means vertical deflicker is unsupported */
    FY_U32              u32HDefLevel;          /* horizontal deflicker level, 0 means horizontal deflicker is unsupported  */
    FY_BOOL             bDcmp;
    FY_BOOL             bPreMul;
}FYFB_CAPABILITY_S;

/* surface info */
typedef struct
{
    FY_U32              u32PhyAddr;            /* start physical address */
    FY_U32              u32Width;              /* width pixels */
    FY_U32              u32Height;             /* height pixels */
    FY_U32              u32Pitch;              /* line pixels */
    FYFB_COLOR_FMT_E    enFmt;                 /* color format */
}FYFB_SURFACE_S;

typedef struct
{
    FY_U32              u32PhyAddr;
    FYFB_ALPHA_S        stAlpha;
    FYFB_COLORKEY_S     stColorkey;
}FYFB_SURFACEEX_S;

/* refresh surface info */
typedef struct
{
    FYFB_SURFACE_S      stCanvas;
    FYFB_RECT           UpdateRect;            /* refresh region */
}FYFB_BUFFER_S;

/* cursor info */
typedef struct
{
    FYFB_SURFACE_S      stCursor;
    FYFB_POINT_S        stHotPos;
} FYFB_CURSOR_S;

/* DDR detect zone info */
typedef struct
{
    FY_U32              u32StartSection;
    FY_U32              u32ZoneNums;
} FYFB_DDRZONE_S;

/* layer info */
typedef struct
{
    FYFB_LAYER_BUF_E    BufMode;
    FYFB_LAYER_ANTIFLICKER_LEVEL_E eAntiflickerLevel;
    FY_S32              s32XPos;               /* the x pos of origion point in screen */
    FY_S32              s32YPos;               /* the y pos of origion point in screen */
    FY_S32              u32CanvasWidth;        /* the width of canvas buffer */
    FY_S32              u32CanvasHeight;       /* the height of canvas buffer */
    FY_U32              u32DisplayWidth;       /* the width of display buf in fb.for 0 buf ,there is no display buf in fb, so it's effectless*/
    FY_U32              u32DisplayHeight;      /* the height of display buf in fb. */
    FY_U32              u32ScreenWidth;        /* the width of screen */
    FY_U32              u32ScreenHeight;       /* the height of screen */
    FY_BOOL             bPreMul;               /* The data drawed in buf is premul data or not*/
    FY_U32              u32Mask;               /* param modify mask bit*/
}FYFB_LAYER_INFO_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_COMM_FB_H__ */

