#ifndef __SAMPLE_COMM_VENC_H__
#define __SAMPLE_COMM_VENC_H__

#include <sys/sem.h>
#include "fy_common.h"
#include "fy_comm_sys.h"
#include "fy_comm_vb.h"
#include "fy_comm_venc.h"
#include "fy_defines.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_venc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* Begin of #ifdef __cplusplus */

#define MAX_FILE_NAME_LEN 512
#define VENC_CHNID(idx)    (idx+VENC_MAX_CHN_NUM)


/*******************************************************
    macro define
*******************************************************/
#define SAMPLE_MAX_WIDTH              2592
#define SAMPLE_MAX_HEIGHT             1952 //1088
    
#define SAMPLE_MAX_MAIN_CHAN_WIDTH    960
#define SAMPLE_MAX_MAIN_CHAN_HEIGHT   1088
#define SAMPLE_MAX_SUB_CHAN_WIDTH     960
#define SAMPLE_MAX_SUB_CHAN_HEIGHT    1088

typedef enum sample_rc_e
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_QVBR,
    SAMPLE_RC_FIXQP,
    SAMPLE_RC_QPMAP,
    SAMPLE_RC_BUTT
}SAMPLE_RC_E;

typedef enum sample_filetype_e
{
    SAMPLE_FT_MP4 = 0,
    SAMPLE_FT_RAWSTREAM,
}SAMPLE_FILETYPE_E;
    
typedef struct test_venc_para
{
    FY_U32  u32ChanId;         					/*0: baseline; 1:MP; 2:HP; 3: SVC-T ; */
    PAYLOAD_TYPE_E enType;
    SIZE_S stPicSize;
    FY_U32  u32Profile;         					/*0: baseline; 1:MP; 2:HP; 3: SVC-T ; */
    SAMPLE_RC_E enRcMode;
    FY_U32  u32Gop;                                  /*the interval of ISLICE. */
    FY_U32  u32StatTime;            /* the rate statistic time, the unit is senconds(s) */
    FY_U32  u32SrcFrmRate;                           /* the input frame rate of the venc chnnel */
    FY_FR32 fr32DstFrmRate ;                         /* the target frame rate of the venc chnnel */
    FY_U32  u32BitRate;                              /* the average bitrate */
    FY_U32  u32MaxBitRate;                           /* the max bitrate */
    FY_U32  u32MaxQp;                                /* the max qp */
    FY_U32  u32MinQp;
    FY_U32  bIntraRefreshEn;
    FY_U32  u32Base;                          /*Base layer period*/
    FY_U32  u32Enhance;                       /*Enhance layer period*/
    FY_U32  bEnablePred;                      /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is FY_FALSE, all frames at the base layer reference IDR frames.*/
    FY_U32  bEnableSmart;
    FY_U32  u32PixFmt;
	FY_U32  u32Rotate;
    char*   pInputfile;

    //added for vpss+veu
    FY_U32  u32VpuGrpId;
    FY_U32  u32VpuChanId;
    FY_U32  u32RcvNum;
    char    pOutfile[MAX_FILE_NAME_LEN];
}VENC_TEST_PARA_S;

typedef struct sample_venc_getstrm_s
{
    FY_BOOL bThreadStart;
    FY_S32  s32Cnt;
    FY_U32 u32TestItem;
    SAMPLE_FILETYPE_E enFileType; /*0:mp4, 1: raw stream*/
    VENC_TEST_PARA_S *pTestPara;
}SAMPLE_VENC_GETSTRM_PARA_S;

typedef struct venc_rec_stream_info_s
{
    FY_U32 u32ChnId;
    PAYLOAD_TYPE_E enStreamType;     
    FY_U32 u32Width;
    FY_U32 u32Height;
    FY_U32 u32SeqNum;
    FY_BOOL bEnableSmart;     
    FY_U32 u32FrameRate;
}VENC_REC_STREAM_INFO_S;

typedef enum
{
    TEST_THREAD_START,
    TEST_THREAD_AUTOTEST,
    TEST_THREAD_PAUSE,
    TEST_THREAD_STOP,        
}thread_state_e;
    
typedef void (*led_onoff)(int, FY_BOOL);


/*******************************************************
    function announce
*******************************************************/
FY_S32 SAMPLE_COMM_VENC_MemConfig(FY_VOID);
FY_S32 SAMPLE_COMM_VENC_Init(VENC_PARAM_MOD_EXT_S *pstModParamExt);
FY_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, VENC_TEST_PARA_S *pTestParams);
FY_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
FY_S32 SAMPLE_COMM_VENC_StopGetStream();
FY_S32 SAMPLE_COMM_VENC_BindVpss(VENC_CHN VencChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_BOOL bEnableBgm);
FY_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_CHN VencChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn, FY_BOOL bEnableBgm);
FY_VOID SAMPLE_COMM_VENC_ReadOneFrame( FILE * fp, FY_U8 * pY, FY_U8 * pU, FY_U8 * pV,
                                              FY_U32 width, FY_U32 height, FY_U32 stride, FY_U32 stride2);
FY_S32 SAMPLE_COMM_VENC_PlanToSemi(FY_U8 *pY, FY_S32 yStride,
                       FY_U8 *pU, FY_S32 uStride,
					   FY_U8 *pV, FY_S32 vStride,
					   FY_S32 picWidth, FY_S32 picHeight);
FY_S32 TEST_VENC_GetTestParams(Config *pstTestCfg, VENC_TEST_PARA_S *pstTestParams, FY_U32 u32TestItem);
FY_S32 SAMPLE_COMM_VENC_StartGetStream(FY_S32 s32Cnt, VENC_TEST_PARA_S *pTestParams, SAMPLE_FILETYPE_E enFileType);
char *TEST_VENC_GetRCString(SAMPLE_RC_E enRCMode);
FY_S32 TEST_VENC_ConstrChanAttr(VENC_CHN VencChn, VENC_TEST_PARA_S *pTestParams, VENC_CHN_ATTR_S *pstVencChnAttr);
FY_S32 SAMPLE_COMM_VENC_AgingTest(FY_BOOL bAgingTest);

FY_S32 SAMPLE_COMM_JPEGE_BindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_COMM_JPEGE_UnBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 SAMPLE_COMM_JPEGE_BindVgs(VENC_CHN VeChn,VGS_CHN VgsChn);
FY_S32 SAMPLE_COMM_JPEGE_UnBindVgs(VENC_CHN VeChn,VGS_CHN VgsChn);
FY_S32 SAMPLE_COMM_JPEGE_BindSrc(VENC_CHN VeChn,MOD_ID_E enBindSrc, FY_S32 u32SrcGrp,FY_S32 u32SrcChan);
FY_S32 SAMPLE_COMM_JPEGE_UnBindSrc(VENC_CHN VeChn,MOD_ID_E enBindSrc, FY_S32 u32SrcGrp,FY_S32 u32SrcChan);
FY_S32 SAMPLE_COMM_JPEGE_Start(VENC_CHN s32Chn, VENC_TEST_PARA_S *pTestParams);
FY_S32 SAMPLE_COMM_JPEGE_Stop(VENC_CHN VencChn);
//FY_S32 SAMPLE_COMM_JPEGE_SaveStream(JPEGE_STREAM_S* pstStream);
FY_S32 SAMPLE_COMM_JPEGE_SnapProcess(FY_U32 u32ChanNum, VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32SnapNum, MOD_ID_E enBindSrc, thread_state_e *bStop);
    FY_S32 SAMPLE_COMM_JPEGE_StartGetStream(FY_S32 s32Cnt, VENC_TEST_PARA_S *pTestParams);
FY_S32 SAMPLE_COMM_JPEGE_StopGetStream();
FY_VOID* SAMPLE_COMM_JPEGE_GetJpegStreamProc(FY_VOID* p);
FY_S32 SAMPLE_COMM_JPEGE_SaveStream(char *path, VENC_STREAM_S * pstStream);
FY_S32 SAMPLE_COMM_JPEGE_AgingTest(FY_BOOL bAgingTest);
int VENC_RECFILE_VidoeEs2Ps(VENC_REC_STREAM_INFO_S *pstRecStrInfo, FILE* pFd, VENC_STREAM_S* pstStream);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
