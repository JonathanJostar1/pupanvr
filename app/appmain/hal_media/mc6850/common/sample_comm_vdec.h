#ifndef __SAMPLE_COMMON_VDEC_H_
#define __SAMPLE_COMMON_VDEC_H_

#include "fy_common.h"
#include "mpi_vb.h"
#include "mpi_vdec.h"


#define PRINTF_VDEC_CHN_STATE(Chn, stChnStat) \
		do{\
				printf(" chn:%2d,  bStart:%2d,	DecodeFrames:%4d,  LeftPics:%3d,  LeftBytes:%10d,  LeftFrames:%4d,	RecvFrames:%6d\n",\
					Chn,\
					stChnStat.bStartRecvStream,\
					stChnStat.u32DecodeStreamFrames,\
					stChnStat.u32LeftPics,\
					stChnStat.u32LeftStreamBytes,\
					stChnStat.u32LeftStreamFrames,\
					stChnStat.u32RecvStreamFrames);\
		}while(0)

typedef enum fyVdecThreadCtrlSignal_E
{
	VDEC_CTRL_START,
	VDEC_CTRL_PAUSE,
	VDEC_CTRL_STOP,
}VdecThreadCtrlSignal_E;

typedef struct fyVdecThreadParam
{
	FY_S32 s32ChnId;
	PAYLOAD_TYPE_E enType;
	FY_CHAR cFileName[100];
	FY_S32 s32StreamMode;
	FY_S32 s32MilliSec;
	FY_S32 s32MinBufSize;
	FY_S32 s32IntervalTime;
	VdecThreadCtrlSignal_E eCtrlSinal;
	FY_U64	u64PtsInit;
	FY_U64	u64PtsIncrease;
	FY_BOOL bLoopSend;
	FY_BOOL bManuSend;
	FY_CHAR cUserCmd;
	FY_S32 send_times;
    FY_U32  *pFramePos;

	FY_BOOL bFAV;
	FY_S32	s32VdecCnt;
	FY_S32	s32AudioChn;
	FY_S32	s32AudioIdx;
	FY_S32	s32VideoIdx;
	FY_S32	s32AudioDev;

}VdecThreadParam;

typedef	struct fyVdecGetImageThreadParam
{
	FY_U32 u32ChnCnt;
	FY_S32 s32IntervalTime;
	VdecThreadCtrlSignal_E eCtrlSinal;
	FY_BOOL	bUseSelect;

	FY_CHAR	cUserCmd;
}VdecGetImageThreadParam;

typedef struct fySendThreadParam
{
    FY_S32 s32ChnNum;
    VDEC_CHN_ATTR_S	*pstVdecChnAttr;
    char **pStreamFileNames;
    FY_U32 **pFramePos;
    FY_BOOL loop;
    FY_S32 framerate;
    FY_S32 ptsinit;
}VdecSendThreadParam;

struct fav_stream{
	char stream_type;
	char codec[11];
	int  packet_cnt;
	int  audio_freq;
	int  audio_chan;
	int  video_width;
	int  video_height;
};


struct fav_packet_info
{
	char stream_type; /* 'a' or 'v' */
	unsigned char stream_idx;
	int  pos;
	int size;
	unsigned int pts;
};


FY_VOID	* VdecGetImages(FY_VOID	*pArgs);
FY_VOID	* VdecVGSGetImages(FY_VOID	*pArgs);
FY_VOID	* VdecVOGetImages(FY_VOID	*pArgs);

FY_VOID	SAMPLE_COMM_VDEC_Sysconf(VB_CONF_S *pstVbConf, SIZE_S *pstSize);
FY_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf(VB_CONF_S *pstModVbConf,PAYLOAD_TYPE_E enType, SIZE_S *pstSize, FY_S32 s32ChnNum);
FY_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf_ext(VB_CONF_S *pstModVbConf,PAYLOAD_TYPE_E* enTypes, SIZE_S *pstSize, FY_S32 s32ChnNum,FY_S32  fbCnt);
FY_S32	SAMPLE_COMM_VDEC_InitModCommVb(VB_CONF_S *pstModVbConf);
FY_VOID	SAMPLE_COMM_VDEC_ChnAttr(FY_S32 s32ChnNum, VDEC_CHN_ATTR_S	*pstVdecChnAttr, PAYLOAD_TYPE_E	enTypes[],	SIZE_S *pstSize);
FY_VOID	SAMPLE_COMM_VDEC_VoAttr(FY_S32 s32ChnNum, VO_DEV VoDev ,VO_PUB_ATTR_S *pstVoPubAttr, VO_VIDEO_LAYER_ATTR_S *pstVoLayerAttr);
FY_VOID SAMPLE_COMM_VDEC_ThreadParam(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend, VDEC_CHN_ATTR_S	*pstVdecChnAttr,	char *pStreamFileNames[], FY_U32 **pFramePos, FY_BOOL loop);
FY_VOID SAMPLE_COMM_VDEC_ThreadParam_ext(VdecSendThreadParam *pThreadParam, VdecThreadParam *pstVdecSend);
FY_S32 Sample_COMM_VPSS_StartCover(FY_S32 VpssGrp);
FY_VOID SAMPLE_COMM_VDEC_CmdCtrl(FY_S32 s32ChnNum,VdecThreadParam *pstVdecSend);
FY_VOID SAMPLE_COMM_VDEC_StartSendStream(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
FY_VOID SAMPLE_COMM_VDEC_StopSendStream(FY_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
FY_VOID SAMPLE_COMM_VDEC_StartSendStreamChannel(FY_S32 s32Chn, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
FY_VOID SAMPLE_COMM_VDEC_StopSendStreamChannel(FY_S32 s32Chn, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
FY_VOID* SAMPLE_COMM_VDEC_SendStream(FY_VOID *pArgs);
FY_S32 SAMPLE_COMM_VDEC_Start(FY_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstAttr,FY_U32 u32BlkCnt);
FY_S32 SAMPLE_COMM_VDEC_Stop(FY_S32 s32ChnNum);
FY_S32 SAMPLE_COMM_VDEC_Start_Channel(FY_S32 s32Chn, VDEC_CHN_ATTR_S *pstAttr, FY_U32 u32BlkCnt);
FY_S32 SAMPLE_COMM_VDEC_Stop_Channel(FY_S32 s32Chn);
FY_S32 SAMPLE_COMM_VDEC_BindVgs(VDEC_CHN VdChn, VGS_CHN VgsChn);
FY_S32 SAMPLE_COMM_VDEC_UnBindVgs(VDEC_CHN VdChn, VGS_CHN VgsChn);
FY_S32 SAMPLE_COMM_VGS_BindVo(VGS_CHN VgsChn,VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VGS_UnBindVo(VGS_CHN VgsChn,VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VDEC_BindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn);
FY_S32 SAMPLE_COMM_VDEC_UnBindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn);

FY_S32 SAMPLE_VDEC_VGS_Init(FY_S32 s32ChnNum, int square);
FY_S32 SAMPLE_VDEC_VGS_DeInit(FY_S32 s32ChnNum);
FY_S32 SAMPLE_VDEC_Stop_VGS_OneChanne(FY_S32 s32Chn );
FY_S32 SAMPLE_VDEC_Start_VGS_OneChanne(FY_S32 s32Chn,int square );

FY_S32 SAMPLE_VDEC_VO_Init(int enMode);
FY_S32 SAMPLE_VDEC_VO_DeInit(int enMode);
FY_S32 SAMPLE_VDEC_VO_StopChannel(FY_S32 s32Chn);
FY_S32 SAMPLE_VDEC_VO_StartChannel(FY_S32 s32Chn,int enMode);


FY_S32 SAMPLE_VDEC_VGS_Bind_VO(FY_S32 s32ChnNum);
FY_S32 SAMPLE_VDEC_VGS_UnBind_VO(FY_S32 s32ChnNum);

FY_S32 SAMPLE_COMM_VDEC_MemConfig(FY_VOID);
FY_S32 SAMPLE_COMM_VDEC_Load_UserPic(char* fname, FY_U32 w,FY_U32 h, PIXEL_FORMAT_E format,VIDEO_FRAME_INFO_S* pUserFrame);
FY_S32 SAMPLE_COMM_VDEC_Release_UserPic(VIDEO_FRAME_INFO_S* pUserFrame);

FY_S32 SAMPLE_VDEC_ParseStream(char *fname, PAYLOAD_TYPE_E type, FY_U32 *pFrame, FY_U32 size);

int SAMPLE_COMM_VDEC_PaserFAV_Header(char* fileName,struct fav_stream* streams );
int SAMPLE_COMM_VDEC_PaserFAV_Packet_Header(char* fileName,int stream_cnt, struct fav_stream* streams,struct fav_packet_info* pPackets );
int SAMPLE_COMM_VDEC_PaserFAV_Packet(const VdecThreadParam *pstVdecThreadParam,const int stream_cnt, const struct fav_stream* streams,const int packet_cnt,const struct fav_packet_info* pPackets, const int audio_idx, const int video_idx);


#endif//__SAMPLE_COMMON_VDEC_H_

