#ifndef __SAMPLE_COMM_PARAM_H__
#define __SAMPLE_COMM_PARAM_H__

#include <sys/sem.h>
#include "fy_comm_vi.h"
#include "fy_comm_vpss.h"

#include "param_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* Begin of #ifdef __cplusplus */


#define VI_SYS_NMAE "VI_SYS_ATTR_S"
#define VI_NVP_CNF  "NVP6134_CFG"
#define VI_DEV_NMAE "VI_DEV_ATTR_S"
#define VI_CHN_CNF  "VI_CHN_ATTR_S"


#define VPSS_GRP_CNF "VPSS_GRP_ATTR_S"
#define VPSS_CHN_CNF "VPSS_GRP_CHN_ATTR_S"
#define VPSS_CHN_MODE_CNF "VPSS_GRP_CHN_MODE_S"
#define VPSS_SEND_FRAME_CNF "SAMPLE_VPSS_SENDSTRM_PARA_S"
#define VPSS_DUMP_FRAME_CNF "SAMPLE_VPSS_DUMPSTRM_PARA_S"
#define VPSS_GLOBLE_DISPLAY_CNF "VPSS_GLOBLE_DISPLAY"

#define MAX_NAME 32
#define VPSS_CHN_NUM 5
#define VPSS_GRP_NUM 16
#define VPSS_DUMP_CNT 50

#define MAX_FILE_NAME_LEN 512

typedef struct fyViSysInfo
{
    FY_U32 u32DevNum;
    VIDEO_NORM_E enNorm;
    FY_U32 u32Chn1080Num;
    FY_U32 u32Chn720Num;
    FY_U32 u32ChnD1Num;
    FY_U32 u32CompRate;
}stViSysInfo;

typedef struct fyViChnInfo
{
    VI_CHN_ATTR_S stChnAttr;
    FY_BOOL  bMinor;
    FY_U32 u32MDstWidth;
    FY_U32 u32MDstHeight;
    FY_U32 s32MSrcFrameRate;
    FY_U32 s32MDstFrameRate;
    char *pInputFile;
	FY_BOOL bConfig;
}stViChnInfo;

#define VI_CHN_PARA_SIZE (sizeof(stViChnInfo)/4-1)
#define VI_SYS_NUM sizeof(stViSysInfo)/4  //the count  params of stViChnNumName

typedef struct fyViCnfInfo
{
    stViSysInfo stViInfo;
    VI_DEV_ATTR_S stViDevAttr;
    stViChnInfo ViChnInfo;
}stViCnfInfo;


typedef struct fyVpssGrpAttr
{
    FY_U32 enPixFmt;  /*VI Pixel format*/
    FY_U32 enCompMode;

    FY_U32 u32Width;
    FY_U32 u32Height;
    FY_U32 HTileNum;    /*水平拼接图像数*/
    FY_U32 VTileNum;    /*垂直拼接图像数*/

    /*各图像效果模块使能后，使用驱动默认参数进行配置*/
    FY_U32 bYcnrEn;       /*去噪使能*/
    FY_U32 bYcnrEcDcEn;   /*去噪参考帧压缩解压是否使能*/ //默认不开就行
    FY_U32 nrCompRate;
    FY_U32 bApcEn;        /*APC 使能*/
    FY_U32 bPurpleEn;     /*去紫边使能*/
    FY_U32 bYGammaEn;    /*YGmamma校正使能*/
    FY_U32 bChromaEn;     /*色度饱和度校正使能*/
    FY_U32 bLcEn;         /*局部色度饱和度校正使能*/
    FY_U32 bYHistEn;      /*亮度直方图统计使能*/
}stVpssGrpAttr;

typedef struct fyVpssChnAttr
{
    FY_BOOL bEnable;
    FY_S32 s32X;
    FY_S32 s32Y;
    FY_U32 u32Width;
    FY_U32 u32Height;
}stVpssChnAttr;

typedef struct fyVpssChnModeAttr
{
    FY_U32 enChnMode;
    FY_U32 u32Width;              /*Width of target image*/
    FY_U32 u32Height;
	FY_U32 enCompressMode;
    FY_U32  enPixelFormat;/*FH Add, VPU Pixel format of target image*/
    FY_U32 CompRate; //FH add
    /*for globle chn*/
	FY_U32 bUseGloble;
	FY_S32 glob_idx;
    FY_S32 s32X;
    FY_S32 s32Y;

    FY_U32 bYcMeanEn; //YcMean 统计OSD反色
    FY_U32 ycmeanMode;
    FY_U32 bBgmYEn; // 要不要打开bgm统计数据， 1/16 1/8采样。
    FY_U32 bCpyEn; //纹理 编码物体细节， 统计纹理细节的开关

    FY_U32 AiOutSel;  /* AI码流的输出格式 0：YUV输出 1：RGB输出 */
    FY_U32 RGBOutMode;/*0 分三通道输出，1 单通道输出*/
	FY_BOOL bConfig;
}stVpssChnModeAttr;

typedef struct fyVpssGrpInfo
{
    stVpssGrpAttr VpssGrpInfo;
    stVpssChnAttr VpssChnAttr[VPSS_CHN_NUM];
    stVpssChnModeAttr VpssChnMode[VPSS_CHN_NUM];
}stVpssGrpInfo;

typedef struct sample_vpss_grp_s
{
    FY_U32 u32Width;
    FY_U32 u32Height;
    FY_CHAR *inputFile;
}stVpssSendFrameInfo;

typedef struct st_vpss_dump_s
{
    //for dump config
    FY_U32 bdump;
    FY_U32 u32Grp;
    FY_U32 u32Chn;
    FY_U32 u32FrameCnt;
	FY_U32 u32CompressMode;
	FY_U32 u32PixelFormat;
	FY_U32 u32Comprate;
    FY_U32 u32Width;
    FY_U32 u32Height;
}stVpssDumpFrameInfo;

typedef struct sample_vpss_dump_s
{
    FY_U32 s32Cnt;
    stVpssDumpFrameInfo DumpFrameInfo[VPSS_DUMP_CNT];
}SAMPLE_VPSS_DUMPSTRM_PARA_S;

typedef struct sample_vpss_sendstrm_s
{
    FY_BOOL bThreadStart;
    FY_U32  s32Cnt;
    FY_U32 u32PixelFormat;
    stVpssSendFrameInfo SendFrameInfo[VPSS_MAX_GRP_NUM];
}SAMPLE_VPSS_SENDSTRM_PARA_S;

typedef struct
{
	FY_S32 glob_idx;
    FY_U32 benable;
	FY_U32 glb_pixfmt;
	FY_U32 frame_rate;
    FY_U32 u32Width;
    FY_U32 u32Height;
}stVpssGlobleInfo;

typedef struct fystVpssGlobleInfo{
	FY_U32 u32GlobNum;
	stVpssGlobleInfo initcfg[VPSS_CHN_NUM];
}SAMPLE_VPSS_GLOBLE_DISPLAY_INIT_CFG;

#define VPSS_GRP_ATTR_NUM sizeof(stVpssGrpAttr)/4
#define VPSS_CHN_ATTR_NUM sizeof(stVpssChnAttr)/4 //the count  params of stViChnNumName
#define VPSS_CHN_MODE_NUM (sizeof(stVpssChnModeAttr)/4 - 1)  // the count params of stNvpCfgName
#define VPSS_SEND_FRAME_NUM sizeof(stVpssSendFrameInfo)/4
#define VPSS_DUMP_FRAME_NUM sizeof(stVpssDumpFrameInfo)/4
#define VPSS_GLOBLE_INFO_NUM sizeof(stVpssGlobleInfo)/4

typedef struct fyVpssInfo
{
    FY_U32 u32GrpNum;
	FY_U32 u32ShowMode;
	SAMPLE_VPSS_GLOBLE_DISPLAY_INIT_CFG st_VpssGlobleInfo;
    stVpssGrpInfo st_VpssGrpInfo[VPSS_GRP_NUM];
    SAMPLE_VPSS_SENDSTRM_PARA_S stVpssSendParam;
    SAMPLE_VPSS_DUMPSTRM_PARA_S stDumpFramInfo;
}stVpssInfo;


FY_S32 SAMPLE_COMM_VI_Parser_Cfg(char *filename,stViCnfInfo *psViCnfInfo);
FY_S32 SAMPLE_COMM_VPSS_Parser_Cfg(char *filename,stVpssInfo *pstVpssInfo);
int SAMPLE_COMM_Get_Mod_Param(const char* modname, const char* paraname, int defval, int *param);
int SAMPLE_COMM_Get_Mod_Param_Str(const char* modname, const char* paraname, char *param, int size);

char *SAMPLE_COMM_Get_Chip_Name();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
