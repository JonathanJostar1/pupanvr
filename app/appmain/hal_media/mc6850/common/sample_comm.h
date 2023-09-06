#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include <sys/sem.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "fy_common.h"
#include "fy_comm_sys.h"
#include "fy_comm_vb.h"
#include "fy_comm_vi.h"
#include "fy_comm_vo.h"
#include "fy_comm_venc.h"
#include "fy_comm_vpss.h"
#include "fy_comm_vdec.h"
#include "fy_comm_region.h"
#include "fy_comm_adec.h"
#include "fy_comm_aenc.h"
#include "fy_comm_aio.h"
#include "fy_comm_vgs.h"
#include "fy_defines.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "mpi_vdec.h"
#include "mpi_vgs.h"
#include "mpi_region.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"


#include "sample_comm_param.h"
#include "sample_comm_venc.h"
#include "sample_comm_vo.h"
#include "sample_comm_vdec.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* Begin of #ifdef __cplusplus */

/*******************************************************
    macro define
*******************************************************/
#define CHECK_CHN_RET(express,Chn,name)\
	do{\
		FY_S32 Ret;\
		Ret = express;\
		if (FY_SUCCESS != Ret)\
		{\
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
			fflush(stdout);\
			return Ret;\
		}\
	}while(0)

#define CHECK_RET(express,name)\
    do{\
        FY_S32 Ret;\
        Ret = express;\
        if (FY_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)

//#define SAMPLE_GLOBAL_NORM	    VIDEO_ENCODING_MODE_PAL
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_422

#define NVP6134_FILE    "/dev/nc_vdec"


#define SAMPLE_AUDIO_PTNUMPERFRM	320
#define SAMPLE_AUDIO_TLV320_DEV		1
#define SAMPLE_AUDIO_TW2865_DEV		0
#define SAMPLE_AUDIO_HDMI_AO_DEV	I2S_DEV3
#if(defined(FY01) || defined(FY10) || defined(MC6630) || defined(MC6830)|| defined(MC6650))
#define SAMPLE_AUDIO_AI_DEV		I2S_DEV0
#define SAMPLE_AUDIO_AO_DEV		I2S_DEV1
#elif defined(MC6850)
#define SAMPLE_AUDIO_AI_DEV		I2S_DEV1
#define SAMPLE_AUDIO_AO_DEV		I2S_DEV1
#else
#define SAMPLE_AUDIO_AI_DEV		DEV_ACW_ID_0
#define SAMPLE_AUDIO_AO_DEV		DEV_ACW_ID_0
#endif
#define SAMPLE_AUDIO_RATE		AUDIO_SAMPLE_RATE_8000
#define SAMPLE_AUDIO_FRM_SIZE		(SAMPLE_AUDIO_RATE*2/25)
#define SAMPLE_AAC_FRM_SIZE		1024*2
#define FRMAE_SIZE(fs,bit,num)		(fs/num)*(bit/8)
#define CNT_PER_S			25

#if (defined(MC6630) || defined(MC6650))
#define AI_CHN_NUM			AUDIO_CHN_NUM_16
#else
#define AI_CHN_NUM			AUDIO_CHN_NUM_2
#endif

#if defined(MC6850)
#define CODEC_MASTER_MODE		FY_FALSE
#else
#define CODEC_MASTER_MODE		FY_FALSE
#endif

#define SAMPLE_CIF_H264_PATH "../common/CIF.h264"
#define SAMPLE_1080P_H264_PATH "../common/1080P.h264"
#define SAMPLE_1080P_H265_PATH "../common/1080P.h265"
#define SAMPLE_4K_H264_PATH "../common/tmp1"
#define SAMPLE_4K_H265_PATH "../common/tmp2"
#define SAMPLE_1080P_MPEG4_PATH "../common/1080P.mpeg4"
#define SAMPLE_FIELD_H264_PATH "../common/D1_field.h264"
#define SAMPLE_1080P_JPEG_PATH "../common/1080P.jpg"
#define SAMPLE_4K_JPEG_PATH "../common/tmp3"

#define SAMPLE_MAX_VDEC_CHN_CNT 8


#define ALIGN_UP(x, a)              ((x+a-1)&(~(a-1)))
#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))

#define SAMPLE_SYS_ALIGN_WIDTH  16
#define VO_BKGRD_BLUE           0x0000FF

#define HD_WIDTH                1920
#define HD_HEIGHT               1088

#define D1_WIDTH                720
#define D1_HEIGHT               576
#define D1_WIDTH_704            704

#define _720P_WIDTH              1280
#define _720P_HEIGHT             720

#define GOSD_DEFAULT_PIXEDEPTH 16

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

typedef struct sample_vi_param_s
{
    FY_S32 s32ViDevCnt;        // VI Dev Total Count
    FY_S32 s32ViDevInterval;   // Vi Dev Interval
    FY_S32 s32ViChnCnt;        // Vi Chn Total Count
    FY_S32 s32ViChnInterval;   // VI Chn Interval
}SAMPLE_VI_PARAM_S;
#define VPSS_GRP_NUM_TEST 4



typedef struct fysample_MEMBUF_S
{
    VB_BLK  hBlock;
    VB_POOL hPool;
    FY_U32  u32PoolId;

    FY_U32  u32PhyAddr;
    FY_U8   *pVirAddr;
    FY_S32  s32Mdev;
} SAMPLE_MEMBUF_S;

typedef enum sample_rgn_change_type_e
{
    RGN_CHANGE_TYPE_FGALPHA = 0,
    RGN_CHANGE_TYPE_BGALPHA,
    RGN_CHANGE_TYPE_LAYER
}SAMPLE_RGN_CHANGE_TYPE_EN;

typedef enum
{
    HIFB_LAYER_0 = 0x0,
    HIFB_LAYER_1,
    HIFB_LAYER_2,
    HIFB_LAYER_CURSOR_0,
    HIFB_LAYER_ID_BUTT
} HIFB_LAYER_ID_E;


typedef enum fyAudioCodecType
{
    AUDIO_CODEC_INNER = 0,
    AUDIO_CODEC_TLV320,
    AUDIO_CODEC_HDMI,
    AUDIO_CODEC_TW2865,
    AUDIO_CODEC_TP28XX,
    AUDIO_CODEC_ACWRAPPER,
    AUDIO_CODEC_NAU88C10,
    AUDIO_CODEC_ALC5616,
    AUDIO_CODEC_BUTT
}AudioCodecType;

typedef enum __AudioFileT
{
	FILE_TYPE_AI,
	FILE_TYPE_AENC,
	FILE_TYPE_TOTAL
}AudioFileT;

/*******************************************************
    function announce
*******************************************************/
FY_S32 SAMPLE_COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);
FY_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, FY_U32 u32AlignWidth,COMPRESS_MODE_E enCompFmt);
FY_S32 SAMPLE_COMM_SYS_MemConfig(FY_VOID);
FY_VOID SAMPLE_COMM_SYS_Exit(void);
FY_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S *pstVbConf);
FY_S32 SAMPLE_COMM_SYS_Payload2FilePostfix(PAYLOAD_TYPE_E enPayload, FY_CHAR* szFilePostfix);
FY_S32 SAMPLE_VI_GetChnInterval(FY_U32 u32ViDevCnt, SAMPLE_VI_PARAM_S *pstViParam);
FY_S32 SAMPLE_COMM_VI_Start(FY_U32 u32ViDevCnt,stViCnfInfo *psViCnfInfo);
FY_S32 SAMPLE_COMM_VI_Stop(FY_U32 u32ViDevCnt);
FY_S32 SAMPLE_COMM_VI_BindVpss(FY_U32 u32ViDevCnt);
FY_S32 SAMPLE_COMM_VI_BindVpss_MixCap(FY_U32 u32ViDevCnt,FY_U32 g_mix_flag,FY_U32 u32GrpNum);
FY_S32 SAMPLE_COMM_VI_UnBindVpss(FY_U32 u32ViDevCnt);
FY_S32 SAMPLE_COMM_VI_UnBindVpss_MixCap(FY_U32 u32ViDevCnt,FY_U32 g_mix_flag,FY_U32 u32GrpNum);
FY_S32 SAMPLE_COMM_VI_GetVFrameFromYUV(FILE *pYUVFile, FY_U32 u32Width, FY_U32 u32Height,FY_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo);
FY_S32 SAMPLE_COMM_VI_Dump(VI_CHN ViChn, FY_U32 u32Cnt,stViCnfInfo *psViCnfInfo);


FY_S32 SAMPLE_COMM_VPSS_Start(FY_S32 s32GrpCnt, SIZE_S *pstSize, FY_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr,stVpssInfo *pstVpssInfo);
FY_S32 SAMPLE_COMM_VPSS_Stop(FY_S32 s32GrpCnt, FY_S32 s32ChnCnt);
FY_S32 SAMPLE_COMM_VPSS_StartSendStream();
void region_set_info(FY_U32 chn, char* vi_type);


//FY_S32 SAMPEL_VPSS_OVERLAY_FUNC_PIXDEPTH(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 pixeldepth);
int Region_logov2_osd(unsigned int grp_idx, unsigned int chan, RGN_HANDLE handle,unsigned int pixeldepth,FY_U32 index,FY_U32 invert);
FY_S32 SAMPLE_VPSS_Overlay_Create(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 pixeldepth,FY_U32 chipId);
FY_S32 SAMPEL_VPSS_OVERLAY_Attach(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,FY_U32 pixeldepth,FY_U32 invert);
FY_S32 SAMPEL_VPSS_OVERLAY_Detach(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_VPSS_Overlay_Destory(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_VPSS_SET_OVERLAY_NUM(FY_U32 u32Num);

FY_S32 SAMPLE_COMM_VpssDump(VPSS_GRP Grp,VPSS_CHN Chn,FY_U32 u32FrameCnt,FY_U32 u32Width,FY_U32 u32Height,FY_U32 u32PixelFormat,FY_U32 u32CompMode,FY_U32 buseGlobe,FY_U32 compRate);
void SAMPLE_COMM_YUV_DUMP(VIDEO_FRAME_S * pVBuf, FILE *pfd);
FY_S32 SAMPLE_COMM_VpssDump_ALL(stVpssInfo *pst_VpssInfo);
FY_S32 test_load_pic(FY_VOID);
FY_S32 test_unload_pic(FY_VOID);
FY_S32 SAMPEL_VPSS_OVERLAY_Attach_RGB1555(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);

void SAMPLE_COMM_AUDIO_SelectType();
FY_S32 SAMPLE_COMM_AUDIO_Get_AcwType();
FY_S32 SAMPLE_COMM_AUDIO_Get_channel();
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn, PAYLOAD_TYPE_E enType, FY_VOID *pAiFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn, FY_VOID *pAiFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiToFifo(AUDIO_DEV AiDev, AI_CHN AiChn, FY_VOID *pAiFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn, PAYLOAD_TYPE_E enType, FY_VOID *pAecFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAencFile(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType, FY_U32 readLen, FILE *pAdcFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAO(AUDIO_DEV AoDev, AO_CHN AoChn, FY_U32 readLen, FILE *pAoFd);
FY_S32 SAMPLE_COMM_AUDIO_reatTrdFifoToAO(AUDIO_DEV AoDev, AO_CHN AoChn, FILE *pAoFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAdecAO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn, PAYLOAD_TYPE_E enType, FY_U32 readLen, FILE *pAdcFd);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAo(AUDIO_DEV AoDev, AI_CHN AoChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencFile(AENC_CHN AeChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd();
FY_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
FY_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
FY_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
FY_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
FY_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
FY_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
FY_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, FY_S32 s32AiChn, AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm);
FY_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, FY_S32 s32AiChn);
FY_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, FY_S32 s32AoChn, AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm, AUDIO_SAMPLE_RATE_E enInSampleRate, FY_BOOL bResampleEn);
FY_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, FY_S32 s32AoChn);
FY_S32 SAMPLE_COMM_AUDIO_StartAenc(AENC_CHN AeChn, PAYLOAD_TYPE_E enType);
FY_S32 SAMPLE_COMM_AUDIO_StopAenc(AENC_CHN AeChn, PAYLOAD_TYPE_E enType);
FY_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
FY_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
FY_S32 SAMPLE_COMM_AUDIO_StartHdmi();
FY_VOID SAMPLE_COMM_AUDIO_fifo_init();
FY_VOID SAMPLE_COMM_AUDIO_fifo_deinit();
FY_VOID SAMPLE_COMM_AUDIO_SetBuff(FY_S32 status);
FY_S32 SAMPLE_COMM_AUDIO_enable_vqe(FY_S32 id,FY_U32 anr_level,FY_U32 agc_level);
FY_S32 SAMPLE_COMM_AUDIO_set_volume(FY_S32 id);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdFifoToAO(AUDIO_DEV AoDev, AO_CHN AoChn, FILE *pAoFd);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAdec(AUDIO_DEV AiDev, AI_CHN AiChn, ADEC_CHN AeChn, PAYLOAD_TYPE_E enType, FY_VOID *pAecFd);
FY_S32 SAMPLE_COMM_AUDIO_StartAiAll(AUDIO_DEV AiDevId, FY_S32 MaxChn, AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm);
FY_S32 SAMPLE_COMM_AUDIO_StopAiAll(AUDIO_DEV AiDevId, FY_S32 MaxChn);
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAll(AUDIO_DEV AiDev, AI_CHN MaxChn,AUDIO_DEV AoDev, AO_CHN AoChn);
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAiAll(AUDIO_DEV AiDev);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
