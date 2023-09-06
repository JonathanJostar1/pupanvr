#ifndef __SAMPLE_COMM_VO_H__
#define __SAMPLE_COMM_VO_H__

#include <sys/sem.h>
#include "fy_common.h"
#include "fy_comm_sys.h"
#include "fy_comm_vb.h"
#include "fy_comm_vo.h"
#include "fy_defines.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* Begin of #ifdef __cplusplus */

/*******************************************************
    macro define
*******************************************************/
#define HDMI_SUPPORT                0


#define SAMPLE_VO_DEV_DHD0          0
#define SAMPLE_VO_LAYER_VHD0        0

#if (defined(MC6650)||defined(MC6850))
#define SAMPLE_VO_DEV_DSD0          2
#define SAMPLE_VO_LAYER_VSD0        2
#define SAMPLE_VO_LAYER_VPIP        3
#else
#define SAMPLE_VO_DEV_DSD0          1
#define SAMPLE_VO_LAYER_VSD0        1
#define SAMPLE_VO_LAYER_VPIP        2
#endif

#define VO_LAYER_PIP                2
#define VO_LAYER_PIP_STA	        2
#define VO_LAYER_PIP_END	        2
#define VO_DEV_HD_END	            0

#define SAMPLE_VO_GRAPHIC_GHD0      0       // HD OSD
#define SAMPLE_VO_GRAPHIC_GSD0      1       // SD OSD
#define SAMPLE_VO_GRAPHIC_GHDC      2       // HD HC
#define SAMPLE_VO_GRAPHIC_GSDC      3       // SD HC


#define SAMPLE_VO_WBC_BASE              0
#define SAMPLE_VO_LAYER_PRIORITY_BASE   0
#define SAMPLE_VO_LAYER_PRIORITY_PIP    1
#define GRAPHICS_LAYER_HC0          SAMPLE_VO_GRAPHIC_GHDC

#define SAMPLE_VO_TIMER_INTERVAL       5

typedef enum sample_vo_mode_e
{
    VO_MODE_1MUX    = 0,
    VO_MODE_4MUX    = 1,
    VO_MODE_9MUX    = 2,
    VO_MODE_16MUX   = 3,
    VO_MODE_1B_5S   = 4,
    VO_MODE_1B_7S   = 5,
    VO_MODE_1L_1R   = 6,
    VO_MODE_25MUX   = 7,
    VO_MODE_36MUX   = 8,
    VO_MODE_4T_4B   = 9,
    VO_MODE_49MUX   = 10,
    VO_MODE_64MUX   = 11,
    VO_MODE_BUTT
}SAMPLE_VO_MODE_E;

typedef struct sample_vo_param_s
{
    VO_DEV VoDev;
    FY_CHAR acMmzName[20];
    FY_U32 u32WndNum;
    SAMPLE_VO_MODE_E enVoMode;
    VO_PUB_ATTR_S stVoPubAttr;
    FY_BOOL bVpssBind;
}SAMPLE_VO_PARAM_S;

typedef struct sample_vo_yuv_s
{
    FY_CHAR		chName[64];
    FY_U32   	u32PicWidth;
    FY_U32   	u32PicHeigth;
    FY_U32  	u32PoolId;
    FY_U32   	u32BlkHandle;
    FY_U32   	u32PhyAddrY;
    FY_VOID    *pVirAddrY;

	FY_U32   	u32PhyAddrUV;
    FY_VOID    *pVirAddrUV;

	FY_U32		u32PicSize;
} SAMPLE_VO_YUV_S;

typedef struct sample_vo_rgb_s
{
    FY_CHAR		chName[64];
    FY_U32   	u32PicWidth;
    FY_U32   	u32PicHeigth;
    FY_U32  	u32PoolId;
    FY_U32   	u32BlkHandle;
    FY_U32   	u32PhyAddr;
    FY_VOID    *pVirAddr;
    PIXEL_FORMAT_E  enPixelFormat;
} SAMPLE_VO_RGB_S;

/*******************************************************
    function announce
*******************************************************/

FY_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, FY_CHAR *pcMmzName);
FY_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
FY_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
FY_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
FY_S32 SAMPLE_COMM_VO_StartCompressedLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
FY_S32 SAMPLE_COMM_VO_StartSingleLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr, FY_BOOL isSingle);

FY_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
FY_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
FY_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);

FY_S32 SAMPLE_COMM_VO_StartChnOne(VO_LAYER VoLayer, VO_CHN voChn, SAMPLE_VO_MODE_E enMode);
FY_S32 SAMPLE_COMM_VO_StopChnOne(VO_LAYER VoLayer, VO_CHN voChn, SAMPLE_VO_MODE_E enMode);

FY_S32 SAMPLE_COMM_VO_StartWbc(VO_WBC VoWbc,const VO_WBC_ATTR_S *pstWbcAttr);
FY_S32 SAMPLE_COMM_VO_StopWbc(VO_WBC VoWbc);
FY_S32 SAMPLE_COMM_WBC_BindVo(VO_WBC VoWbc,VO_WBC_SOURCE_S *pstWbcSource);
FY_S32 SAMPLE_COMM_VO_BindVoWbc(VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VO_UnBindVoWbc(VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
FY_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync,FY_U32 *pu32W,FY_U32 *pu32H,FY_U32 *pu32Frm);
FY_S32 SAMPLE_COMM_VO_GetChnRect(VO_CHN voChn, SAMPLE_VO_MODE_E enMode, SIZE_S *pstSize, RECT_S* pstRect);
#if (HDMI_SUPPORT == 1)
FY_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
FY_S32 SAMPLE_COMM_VO_HdmiStop(FY_VOID);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMM_VO_H__ */

