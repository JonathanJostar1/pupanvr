#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "sample_comm.h"
#include "sample_playback.h"




static VDEC_CHN_ATTR_S stVdecChnAttr[VDEC_MAX_CHN_NUM];
static VdecThreadParam stVdecSend[VDEC_MAX_CHN_NUM];
static pthread_t	VdecThread[VDEC_MAX_CHN_NUM];
static pthread_t  VdecReadThread;
static pthread_t  VdecAutoChansThread;

static VdecGetImageThreadParam stVdecRead;
static int VdecChanNum  =0;
static int OrgVdecChanNum  =0;
static int VdecPlayBackStart = 0;
static int VdecBind = 0;
static int VdecFbCnt = 0;
static FY_S32 VdecLoadUserPic = FY_FAILURE;
static VIDEO_FRAME_INFO_S VdecUserPic;
static volatile FY_BOOL VdecTerminateTaskProc;
static FY_BOOL VdecStartProc = FY_FALSE;
static int  VdecEnMode;
static VO_LAYER VdecVoLayer;
static VO_CHN   VdecStartVoChn;
static FY_BOOL  VdecSHow;
static int task_type = 0; /* 0-auto channels, 1 - auto speed*/
static FY_BOOL  VdecbFav = FY_FALSE;
static int VdecAoDev;

FY_S32 SAMPLE_VDEC_VO_Init_Layer(int enMode, VO_LAYER VoLayer, VO_CHN startVoChn, FY_S32 s32Chn)
{
	int i;
    FY_S32 s32Ret = FY_SUCCESS;

	FY_MPI_VO_SetAttrBegin(VoLayer);
	for(i=0;i<s32Chn;i++)
	{
		s32Ret = SAMPLE_COMM_VO_StartChnOne(VoLayer, startVoChn + i, enMode);
		if(s32Ret != FY_SUCCESS)
			break;
	}
	FY_MPI_VO_SetAttrEnd(VoLayer);
	return s32Ret;
}

FY_S32 SAMPLE_VDEC_VO_DeInit_Layer(int enMode, VO_LAYER VoLayer,VO_CHN startVoChn, FY_S32 s32Chn)
{
	int i;
    FY_S32 s32Ret = FY_SUCCESS;

	for(i=0;i<s32Chn;i++)
	{
		s32Ret = SAMPLE_COMM_VO_StopChnOne(VoLayer, startVoChn + i, enMode);
		if(s32Ret != FY_SUCCESS)
			break;
	}
	return s32Ret;
}

FY_S32 SAMPLE_VDEC_VO_StopChannel_Layer(FY_S32 s32Chn, VO_LAYER VoLayer)
{
	FY_MPI_VO_DisableChn(VoLayer,(VO_CHN)s32Chn);

	return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_VO_StartChannel_Layer(FY_S32 s32Chn,int enMode, VO_LAYER VoLayer)
{
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    VO_CHN_ATTR_S stChnAttr;

	FY_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
	SAMPLE_COMM_VO_GetChnRect(s32Chn, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);

	stChnAttr.u32Priority		= 0;
	stChnAttr.bDeflicker		= FY_FALSE;

	FY_MPI_VO_SetChnAttr(VoLayer, s32Chn, &stChnAttr);
	FY_MPI_VO_SetChnFrameRate(VoLayer, s32Chn, 15);
	FY_MPI_VO_EnableChn(VoLayer,(VO_CHN)s32Chn);

	return FY_SUCCESS;
}

FY_S32 SAMPLE_VDEC_VGS_Init_Layer(FY_S32 s32ChnNum, int enMode,VO_LAYER VoLayer)
{
	int i;
	VGS_CHN VgsChn;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN_ATTR_S stChnAttr;
    FY_S32 s32Ret = FY_SUCCESS;
	VGS_CHN_PARA_S vgsMode = {
		.enChnMode = VGS_CHN_MODE_AUTO,//VGS_CHN_MODE_USER,
		.u32Width = 960,
		.u32Height = 1088,
		.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
	};

 	FY_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);

	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;

	    s32Ret = SAMPLE_COMM_VO_GetChnRect(i, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);
        if (s32Ret != FY_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return FY_FAILURE;
        }

        vgsMode.u32Width = ALIGN_BACK(stChnAttr.stRect.u32Width, 8);
        vgsMode.u32Height = ALIGN_BACK(stChnAttr.stRect.u32Height, 2);
		CHECK_CHN_RET(FY_MPI_VGS_CreateChn(VgsChn),i,"FY_MPI_VGS_CreateChn");
		CHECK_CHN_RET(FY_MPI_VGS_SetChnMode(VgsChn, 0, &vgsMode),i,"FY_MPI_VGS_SetChnMode");

	}
    return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_VGS_Init_Layer_UserMode(FY_S32 s32ChnNum, int square,VO_LAYER VoLayer)
{
	int i;
	VGS_CHN VgsChn;
	VGS_CHN_PARA_S vgsMode = {
		.enChnMode = VGS_CHN_MODE_AUTO,//VGS_CHN_MODE_USER,
		.u32Width = HD_WIDTH/ square,
		.u32Height = HD_HEIGHT/ square,
		.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
	};
	if(VoLayer==SAMPLE_VO_LAYER_VSD0)
	{
		vgsMode.enChnMode = VGS_CHN_MODE_AUTO;
		vgsMode.u32Width = D1_WIDTH_704/square;
		vgsMode.u32Height = D1_HEIGHT/square;
	}

	vgsMode.u32Width = ALIGN_BACK(vgsMode.u32Width, 8);
	vgsMode.u32Height = ALIGN_BACK(vgsMode.u32Height, 2);

	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;

		CHECK_CHN_RET(FY_MPI_VGS_CreateChn(VgsChn),i,"FY_MPI_VGS_CreateChn");
		CHECK_CHN_RET(FY_MPI_VGS_SetChnMode(VgsChn, 0, &vgsMode),i,"FY_MPI_VGS_SetChnMode");
	}

    return FY_SUCCESS;
}



FY_S32 SAMPLE_VDEC_VGS_Bind_VO_Layer(FY_S32 s32ChnNum, VO_LAYER VoLayer,VO_CHN startVoChn)
{
	int i;
	VGS_CHN VgsChn;
	VO_CHN VoChn;

	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;
		VoChn = startVoChn + i;

		CHECK_CHN_RET(SAMPLE_COMM_VGS_BindVo(VgsChn,VoLayer,VoChn),i,"SAMPLE_COMM_VGS_BindVo");
	}

	return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_VGS_UnBind_VO_Layer(FY_S32 s32ChnNum, VO_LAYER VoLayer,VO_CHN startVoChn)
{
	int i;
	VGS_CHN VgsChn;
	VO_CHN VoChn;

	for(i=0;i<s32ChnNum;i++)
	{
		VgsChn = i;
		VoChn = startVoChn + i;

		CHECK_CHN_RET(SAMPLE_COMM_VGS_UnBindVo(VgsChn,VoLayer,VoChn),i,"SAMPLE_COMM_VGS_UnBindVo");
	}

	return FY_SUCCESS;
}


FY_S32 SAMPLE_VDEC_Start_VGS_OneChanne_Layer(FY_S32 s32Chn,int enMode,VO_LAYER VoLayer )
{
	VGS_CHN VgsChn;
    FY_S32 s32Ret = FY_SUCCESS;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	VGS_CHN_PARA_S vgsMode = {
	    .enChnMode = VGS_CHN_MODE_AUTO,//VGS_CHN_MODE_USER,
        .u32Width = 960,
        .u32Height = 1088,
        .enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    };

    FY_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);

    s32Ret = SAMPLE_COMM_VO_GetChnRect(s32Chn, enMode, &stLayerAttr.stImageSize, &stChnAttr.stRect);
    if (s32Ret != FY_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return FY_FAILURE;
    }

    vgsMode.u32Width = ALIGN_BACK(stChnAttr.stRect.u32Width, 8);
    vgsMode.u32Height = ALIGN_BACK(stChnAttr.stRect.u32Height, 2);

	VgsChn = (VGS_CHN)s32Chn;

	CHECK_CHN_RET(FY_MPI_VGS_CreateChn(VgsChn),s32Chn,"FY_MPI_VGS_CreateChn");
	CHECK_CHN_RET(FY_MPI_VGS_SetChnMode(VgsChn, 0, &vgsMode),VgsChn,"FY_MPI_VGS_SetChnMode");

    return FY_SUCCESS;
}



static unsigned long long VdecChannsMaps = 0;
static FY_VOID* task_proc(void* arg)
{
	FY_U32	s32ChnNum;
	int mode = 1;
	int wait_time = 1;
	int i;
	int type = (int)(*((int*)arg));
	int speed = 1;

	prctl(PR_SET_NAME, "play_ctrl");

	while(!VdecTerminateTaskProc)
	{
		for(i=0;i<wait_time;i++)
		{
			sleep(1);
			if(VdecTerminateTaskProc) break;
		}

		if(type==0)
		{
	AUTO_CHANNES:
			s32ChnNum = VdecChanNum;
			if(mode == 1)
			{
				if(s32ChnNum<OrgVdecChanNum)
					Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AJUST_CHANNEL,1);
				else
				{
					mode = -1;
					goto AUTO_CHANNES;
				}
			}
			else
			{
				if(s32ChnNum>=1)
					Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AJUST_CHANNEL,-1);
				else
				{
					mode = 1;
					goto AUTO_CHANNES;
				}
			}
		}
		else if(type==1)
		{
			if(speed==1)
				speed = 4;
			else
				speed = 1;
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15*speed);
		}
        else if(type==2)
        {
            FY_BOOL on;
            s32ChnNum = rand()%OrgVdecChanNum;

            if(s32ChnNum >=OrgVdecChanNum)
                s32ChnNum = 0;
            if(VdecChannsMaps & (i<<s32ChnNum))
                on = FY_FALSE;
            else
                on = FY_TRUE;
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_ONOFF_CHANNEL,s32ChnNum | (on<<16));
            if(on)
               VdecChannsMaps |= (i<<s32ChnNum);
            else
                VdecChannsMaps &= ~(i<<s32ChnNum);
        }
	}

	return NULL;
}



int	Sample_Playback_Start_Mux_StartVoChn_ext(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height, PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer,VO_CHN startVoChn, FY_BOOL bShow)
{
	int	i;
	FY_S32 ret;
	SIZE_S astSize[VDEC_MAX_CHN_NUM];
	int square;
	VB_CONF_S stModVbConf;
	//PAYLOAD_TYPE_E pt_types[VDEC_MAX_CHN_NUM];
	//char* filenames[VDEC_MAX_CHN_NUM];
	VDEC_MOD_PARAM_S mod_parm;
	int bind_vo_channel;

	if(VdecPlayBackStart)
		return 1;

	if(enMode==VO_MODE_1MUX && s32ChnNum>1)
		return -1;
	else if(enMode==VO_MODE_4MUX && ( startVoChn)>3)
		return -1;
	else if(enMode==VO_MODE_9MUX && ( startVoChn)>8)
		return -1;
	else if(enMode==VO_MODE_1B_7S&& ( startVoChn)>7)
		return -1;
	else if(enMode==VO_MODE_16MUX && ( startVoChn)>15)
		return -1;
	else if(enMode==VO_MODE_25MUX && ( startVoChn)>25)
		return -1;
	else if(enMode==VO_MODE_36MUX && ( startVoChn)>36)
		return -1;
	else if(enMode==VO_MODE_49MUX && ( startVoChn)>49)
		return -1;
	else if(enMode==VO_MODE_64MUX && ( startVoChn)>64)
		return -1;
	else if(enMode > VO_MODE_BUTT)
		return -1;

	VdecEnMode = enMode;
	if(enMode == VO_MODE_1MUX)
		square = 1;
	else if(enMode == VO_MODE_4MUX)
		square = 2;
	else if(enMode == VO_MODE_9MUX)
		square = 3;
	else if(enMode == VO_MODE_1B_7S)
		square = 3;
	else if(enMode == VO_MODE_25MUX)
		square = 5;
	else if(enMode == VO_MODE_36MUX)
		square = 6;
	else if(enMode==VO_MODE_49MUX)
		square = 7;
	else if(enMode==VO_MODE_64MUX)
		square = 8;
	else
		square = 4;

	bind_vo_channel = s32ChnNum;
	if(enMode==VO_MODE_4MUX)
	{
		if( ( startVoChn + s32ChnNum)>4)
			bind_vo_channel = 4 - startVoChn;
	}
	else if(enMode==VO_MODE_9MUX)
	{
		if( ( startVoChn + s32ChnNum)>9)
			bind_vo_channel = 9 - startVoChn ;
	}
	else if(enMode == VO_MODE_1B_7S)
	{
		if( ( startVoChn + s32ChnNum)>8)
			bind_vo_channel = 8 - startVoChn ;
	}
	else if(enMode==VO_MODE_16MUX)
	{
		if( ( startVoChn + s32ChnNum)>16)
			bind_vo_channel = 16 - startVoChn;
	}
	else if(enMode==VO_MODE_25MUX)
	{
		if( ( startVoChn + s32ChnNum)>25)
			bind_vo_channel = 25 - startVoChn;
	}
	else if(enMode==VO_MODE_36MUX)
	{
		if( ( startVoChn + s32ChnNum)>36)
			bind_vo_channel = 36 - startVoChn;
	}
	else if(enMode==VO_MODE_49MUX)
	{
		if( ( startVoChn + s32ChnNum)>49)
			bind_vo_channel = 49 - startVoChn;
	}
	else if(enMode==VO_MODE_64MUX)
	{
		if( ( startVoChn + s32ChnNum)>64)
			bind_vo_channel = 64 - startVoChn;
	}

    for(i = 0 ; i < s32ChnNum; i++) {
    	astSize[i].u32Width	= video_width;
	    astSize[i].u32Height = video_height;
    }

	FY_MPI_VDEC_GetModParam(&mod_parm);
	if(mod_parm.u32VBSource==0)
	{
		SAMPLE_COMM_VDEC_ModCommPoolConf_ext(&stModVbConf,pt_types,astSize,s32ChnNum,fb_cnt);
		SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
	}

	if(bShow)
	{
		ret = SAMPLE_VDEC_VO_Init_Layer(enMode,VoLayer,startVoChn,bind_vo_channel);
		if(ret)
			goto FAIL0;
	}

	if(bShow)
		ret = SAMPLE_VDEC_VGS_Init_Layer(s32ChnNum,enMode,VoLayer);
	else
		ret = SAMPLE_VDEC_VGS_Init_Layer_UserMode(s32ChnNum,square,VoLayer);
	if(ret)
		goto FAIL1;

	if(bShow)
	{
		ret = SAMPLE_VDEC_VGS_Bind_VO_Layer(bind_vo_channel,VoLayer,startVoChn);
		if(ret) goto FAIL2;
	}

	SAMPLE_COMM_VDEC_ChnAttr(s32ChnNum, &stVdecChnAttr[0],	pt_types, astSize);
	for(i=0;i<VDEC_MAX_CHN_NUM;i++)
		stVdecChnAttr[i].stVdecVideoAttr.enMode = VIDEO_MODE_FRAME;

	ret = SAMPLE_COMM_VDEC_Start(s32ChnNum,stVdecChnAttr,fb_cnt);
	if(ret)
		goto FAIL3;

	SAMPLE_COMM_VDEC_ThreadParam(s32ChnNum, &stVdecSend[0], &stVdecChnAttr[0], filenames, NULL, FY_TRUE);


	if(!bind){
		stVdecRead.u32ChnCnt = s32ChnNum;
		stVdecRead.s32IntervalTime = 2;
		stVdecRead.bUseSelect =	FY_FALSE;

		pthread_create(&VdecReadThread,	0, VdecVGSGetImages, (FY_VOID *)&stVdecRead);
	}
	else
	{
		VdecReadThread = 0;

		for(i=0;i<s32ChnNum;i++)
		{
			ret = SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)i,(VGS_CHN)i);
			if(ret) goto FAIL4;
		}

	}
	SAMPLE_COMM_VDEC_StartSendStream(s32ChnNum, &stVdecSend[0], &VdecThread[0]);

	VdecVoLayer = VoLayer;
	VdecStartVoChn = startVoChn;
	VdecSHow = bShow;
	OrgVdecChanNum = VdecChanNum = s32ChnNum;
	VdecBind = bind;
	VdecFbCnt = fb_cnt;
	VdecPlayBackStart = 1;

    VdecChannsMaps = 0 ;
	for(i=0;i<OrgVdecChanNum;i++)
		VdecChannsMaps |= (1<<i);

	return 0;

FAIL4:
	SAMPLE_COMM_VDEC_StopSendStream(s32ChnNum, &stVdecSend[0], &VdecThread[0]);

	for(i=0;i<s32ChnNum;i++)
		SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)i,(VGS_CHN)i);
FAIL3:
	SAMPLE_COMM_VDEC_Stop(s32ChnNum);
FAIL2:
	if(bShow)
		SAMPLE_VDEC_VGS_UnBind_VO_Layer(s32ChnNum,VoLayer,startVoChn);
FAIL1:
	SAMPLE_VDEC_VGS_DeInit(s32ChnNum);
	if(bShow)
		SAMPLE_VDEC_VO_DeInit_Layer(enMode,VoLayer,startVoChn,s32ChnNum);
FAIL0:

	FY_MPI_VB_ExitModCommPool(VB_UID_VDEC);

	return ret;
}


int	Sample_Playback_Start_Mux_StartVoChn(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height,PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer,VO_CHN startVoChn)
{
	return Sample_Playback_Start_Mux_StartVoChn_ext(s32ChnNum,enMode,video_width,video_height,pt_types,filenames,fb_cnt,bind,VoLayer,startVoChn,FY_TRUE);
}


int	Sample_Playback_Start_Mux(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height,PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer)
{
	return Sample_Playback_Start_Mux_StartVoChn(s32ChnNum,enMode,video_width,video_height,pt_types,filenames,fb_cnt,bind,VoLayer,0);
}

int	Sample_Playback_Start_Mux_Fav(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height,PAYLOAD_TYPE_E pt_type,char* filename, int fb_cnt, int bind,VO_LAYER VoLayer,int aoDev,int audio_idx,int video_idx)
{
	int i;
	FY_S32 ret;
	SIZE_S astSize[VDEC_MAX_CHN_NUM];
	VB_CONF_S stModVbConf;
	PAYLOAD_TYPE_E pt_types[VDEC_MAX_CHN_NUM];
	char* filenames[VDEC_MAX_CHN_NUM];
	VDEC_MOD_PARAM_S mod_parm;
	int bind_vo_channel;
	int stream_cnt  = 0;
	struct fav_stream  streams[64];
	struct fav_packet_info *pPacktInfo;
	int packet_cnt = 0;
	int	audio_freq = 0;
	int	audio_chan = 0;
	int audio_packet_size = 0;
	AIO_ATTR_S	stAioAttr;
	AUDIO_FRAME_E	frameInfo;
	VO_HDMI_AUDIO_S hdmiAttr;
	FY_U32 reSampleParm;

	if(VdecPlayBackStart)
		return 1;

	if(enMode > VO_MODE_BUTT)
		return -1;

	stream_cnt = SAMPLE_COMM_VDEC_PaserFAV_Header(filename,streams);
	if(stream_cnt<=0)
	{
		printf("File %s is not FAV file\n",filename);
		return -1;
	}
	for(i=0;i<stream_cnt;i++)
		packet_cnt += streams[i].packet_cnt;
	pPacktInfo = malloc(sizeof(struct fav_packet_info)*packet_cnt);
	if(pPacktInfo==NULL)
	{
		printf("Malloc failed!\n");
		return -1;
	}
	ret = SAMPLE_COMM_VDEC_PaserFAV_Packet_Header(filename,stream_cnt,streams,pPacktInfo);
	if(ret != packet_cnt)
	{
		free(pPacktInfo);
		return -1;
	}
	for(i=0;i<stream_cnt;i++)
	{
		if(streams[i].stream_type == 'a' && i==audio_idx)
		{
			audio_freq = streams[i].audio_freq;
			audio_chan = streams[i].audio_chan;
			break;
		}
	}

	audio_packet_size = 0;
	for(i=0;i<packet_cnt;i++)
	{
		if(audio_packet_size==0 && pPacktInfo[i].stream_type=='a' && pPacktInfo[i].stream_idx == audio_idx)
		{
			audio_packet_size = pPacktInfo[i].size;
			break;
		}
	}
	free(pPacktInfo);
	for(i=0;i<s32ChnNum;i++)
	{
		pt_types[i] = pt_type;
		filenames[i] = filename;
	}

	VdecEnMode = enMode;

	bind_vo_channel = s32ChnNum;

	for(i = 0 ; i < s32ChnNum; i++) {
		astSize[i].u32Width = video_width;
		astSize[i].u32Height = video_height;
	}

	FY_MPI_VDEC_GetModParam(&mod_parm);
	if(mod_parm.u32VBSource==0)
	{
		SAMPLE_COMM_VDEC_ModCommPoolConf_ext(&stModVbConf,pt_types,astSize,s32ChnNum,fb_cnt);
		SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
	}
	ret = SAMPLE_VDEC_VO_Init_Layer(enMode,VoLayer,0,bind_vo_channel);
	if(ret)
		goto FAIL0;

	ret = SAMPLE_VDEC_VGS_Init_Layer(s32ChnNum,enMode,VoLayer);
	if(ret)
		goto FAIL1;

	ret = SAMPLE_VDEC_VGS_Bind_VO_Layer(bind_vo_channel,VoLayer,0);
	if(ret) goto FAIL2;

	SAMPLE_COMM_VDEC_ChnAttr(s32ChnNum, &stVdecChnAttr[0],	pt_types, astSize);
	for(i=0;i<VDEC_MAX_CHN_NUM;i++)
		stVdecChnAttr[i].stVdecVideoAttr.enMode = VIDEO_MODE_FRAME;

	ret = SAMPLE_COMM_VDEC_Start(s32ChnNum,stVdecChnAttr,fb_cnt);
	if(ret)
		goto FAIL3;

	SAMPLE_COMM_VDEC_ThreadParam(s32ChnNum, &stVdecSend[0], &stVdecChnAttr[0], filenames, NULL, FY_FALSE);
	stVdecSend[0].bFAV = FY_TRUE;
	stVdecSend[0].s32VdecCnt = s32ChnNum;
	stVdecSend[0].s32AudioDev = aoDev;
	stVdecSend[0].s32VideoIdx = video_idx;
	stVdecSend[0].s32AudioIdx = audio_idx;
	if(audio_packet_size)
	{
		int ao_dev =aoDev;
		if(ao_dev & (1<<I2S_DEV3))
		{
			FY_MPI_VO_GetHdmiAudio(0,&hdmiAttr);
			switch(audio_freq)
			{
			case 48000:
				hdmiAttr.enSampleRate = VO_SAMPLE_RATE_48K;
				break;
			case 44100:
				hdmiAttr.enSampleRate = VO_SAMPLE_RATE_44K;
				break;
			case 32000:
				hdmiAttr.enSampleRate = VO_SAMPLE_RATE_32K;
				break;
			case 16000:
			case 8000:
				hdmiAttr.enSampleRate = VO_SAMPLE_RATE_32K;
				;
				break;
			}
			reSampleParm = (FY_FALSE<<16) | (1<<8) | 16;
			FY_MPI_AO_EnableReSmp(I2S_DEV3,0,audio_freq,reSampleParm);
			FY_MPI_VO_SetHdmiAudio(0,&hdmiAttr);
		}

		while(ao_dev)
		{
			int local_ao_dev = 0;
			char cmd[128];
			/*
			00001 ：C MCLK/1
			00010 ：C MCLK/2
			00011 ：C MCLK/3
			00100 ：C MCLK/4
			00101 ：C MCLK/6
			00110 ：C MCLK/8
			00111 ：C MCLK/9
			01000 ：C MCLK/11
			01001 ：C MCLK/12
			01010 ：C MCLK/16
			01011 ：C MCLK/18
			01100 ：C MCLK/22
			01101 ：C MCLK/24
			01110 ：C MCLK/33
			01111 ：C MCLK/36
			10000 ：C MCLK/44
			10001 ：C MCLK/48 - 8K
			*/

			//reg8
			char bit_clk[] = { 0x11, 0xD,0x9,-1,0x5,0x4}; // 8K, 16K, 32K, 44.1K, 48K 96K
/*
			Register 13  ADCFsRatio 	 Register 24 DACFsRatio

			8 kHz (MCLK/1536) 01010  8 kHz	(MCLK/1536) 01010 MCLK/6
			8 kHz (MCLK/1536) 01010  48 kHz (MCLK/256) 00010 MCLK/4
			12 kHz (MCLK/1024)00111  12 kHz (MCLK/1024)00111 MCLK/4
			16 kHz (MCLK/768) 00110  16 kHz (MCLK/768) 00110 MCLK/6
			24 kHz (MCLK/512) 00100  24 kHz (MCLK/512) 00100 MCLK/4
			32 kHz (MCLK/384) 00011  32 kHz (MCLK/384) 00011 MCLK/6
			48 kHz (MCLK/256) 00010  8 kHz (MCLK/1536) 01010 MCLK/4
			48 kHz (MCLK/256) 00010  48 kHz (MCLK/256)00010 MCLK/4
			96 kHz (MCLK/128) 00000  96 kHz (MCLK/128) 00000 MCLK/2
*/
			//reg13
			char ADCFsRatio[] = {0xA,0x6,0x3,-1,0x2,0x0};
			//reg24
			char DACFsRatio[] = {0xA,0x6,0x3,-1,0x2,0x0};

			if(ao_dev==0)
				break;

			if(ao_dev & (1<<I2S_DEV1))
			{
				int val_idx = 0;
				local_ao_dev = I2S_DEV1;
				ao_dev &= ~(1<<I2S_DEV1);

				if(audio_freq== 8000)
					val_idx = 0;
				else if(audio_freq== 16000)
					val_idx = 1;
				else if(audio_freq== 32000)
					val_idx = 2;
				else if(audio_freq== 44100)
					val_idx = 3;
				else if(audio_freq== 48000)
					val_idx = 4;
				else if(audio_freq== 96000)
					val_idx = 5;
				else
				{
					aoDev &= ~(1<<I2S_DEV1);
					continue;
				}
				if(bit_clk[val_idx] == -1)
				{
					aoDev &= ~(1<<I2S_DEV1);
					continue;
				}
				//mclk
				printf("val_idx:%d\n",val_idx);
				sprintf(cmd,"echo w 8 0x%x 4 > /proc/umap/acw",0x80 | bit_clk[val_idx]);
				printf("cmd:%s\n",cmd);
				system(cmd);
				sprintf(cmd,"echo w 13 0x%x 4 > /proc/umap/acw",ADCFsRatio[val_idx]);
				printf("cmd:%s\n",cmd);
				system(cmd);
				sprintf(cmd,"echo w 24 0x%x 4 > /proc/umap/acw",DACFsRatio[val_idx]);
				printf("cmd:%s\n",cmd);
				system(cmd);

			}
			else if(ao_dev & (1<<I2S_DEV3))
			{
				local_ao_dev = I2S_DEV3;
				ao_dev &= ~(1<<I2S_DEV3);
			}

			memset(&frameInfo,0,sizeof(frameInfo));
			memset(&stAioAttr,0,sizeof(stAioAttr));


			if(local_ao_dev== I2S_DEV3)
				frameInfo.frame_size = audio_packet_size*2 * hdmiAttr.enSampleRate/audio_freq;
			else
				frameInfo.frame_size = audio_packet_size;
			frameInfo.frame_cnt = 16;


			if(local_ao_dev == I2S_DEV3)
				stAioAttr.fs_rate	= hdmiAttr.enSampleRate;
			else
				stAioAttr.fs_rate		= audio_freq;
			stAioAttr.chn_num	= audio_chan;
			if(local_ao_dev == I2S_DEV3)
				stAioAttr.is_slave	= AIO_MODE_I2S_MASTER;
			else
				stAioAttr.is_slave	= AIO_MODE_I2S_SLAVE;
			stAioAttr.set_param = (stAioAttr.is_slave == AIO_MODE_I2S_MASTER)?I2S_CLK_DIV:I2S_CHN_CMD;
			if(local_ao_dev == I2S_DEV3)
				stAioAttr.chn_width = AUDIO_BIT_WIDTH_32;
			else
				stAioAttr.chn_width = AUDIO_BIT_WIDTH_16;
			stAioAttr.i2s_data_format = I2S_MIX_DATA_FORMAT;
			stAioAttr.i2s_rxt_mode	= I2S_TXF_MODE;

			frameInfo.fileInfo.isNeedSave = FY_FALSE;
			stAioAttr.u32DmaSize = frameInfo.frame_size;
			if(local_ao_dev==I2S_DEV1)
				stAioAttr.io_type		= AC_LINE_HPOUT;



			ret = FY_MPI_AO_SetPubAttr(local_ao_dev,IOC_AICMD_BUTT, &stAioAttr);
			ret = FY_MPI_AO_SetChnParam(local_ao_dev,0, &frameInfo);
			ret = FY_MPI_AO_EnableChn(local_ao_dev, 0);
			ret = FY_MPI_AO_Enable(local_ao_dev);
		}
	}
	else
		aoDev = 0;



	if(!bind){
		stVdecRead.u32ChnCnt = s32ChnNum;
		stVdecRead.s32IntervalTime = 2;
		stVdecRead.bUseSelect = FY_FALSE;

		pthread_create(&VdecReadThread, 0, VdecVGSGetImages, (FY_VOID *)&stVdecRead);
	}
	else
	{
		VdecReadThread = 0;

		for(i=0;i<s32ChnNum;i++)
		{
			ret = SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)i,(VGS_CHN)i);
			if(ret) goto FAIL4;
		}

	}
	SAMPLE_COMM_VDEC_StartSendStream(s32ChnNum, &stVdecSend[0], &VdecThread[0]);

	VdecVoLayer = VoLayer;
	VdecStartVoChn = 0;
	VdecSHow = FY_TRUE;
	OrgVdecChanNum = VdecChanNum = s32ChnNum;
	VdecBind = bind;
	VdecFbCnt = fb_cnt;
	VdecPlayBackStart = 1;

	VdecChannsMaps = 0 ;
	for(i=0;i<OrgVdecChanNum;i++)
		VdecChannsMaps |= (1<<i);

	VdecbFav = FY_TRUE;
	VdecAoDev = aoDev;

	return 0;

FAIL4:
	SAMPLE_COMM_VDEC_StopSendStream(s32ChnNum, &stVdecSend[0], &VdecThread[0]);

	for(i=0;i<s32ChnNum;i++)
		SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)i,(VGS_CHN)i);
FAIL3:
	SAMPLE_COMM_VDEC_Stop(s32ChnNum);
FAIL2:
	SAMPLE_VDEC_VGS_UnBind_VO_Layer(s32ChnNum,VoLayer,0);
FAIL1:
	SAMPLE_VDEC_VGS_DeInit(s32ChnNum);
	SAMPLE_VDEC_VO_DeInit_Layer(enMode,VoLayer,0,s32ChnNum);
FAIL0:

	FY_MPI_VB_ExitModCommPool(VB_UID_VDEC);

	return ret;
}

int	Sample_Playback_Start(FY_U32	s32ChnNum, PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer)
{
	int enMode;
	if(s32ChnNum==1)
		enMode = VO_MODE_1MUX;
	else if(s32ChnNum<=4)
		enMode = VO_MODE_4MUX;
	else if(s32ChnNum<=8)
		enMode = VO_MODE_9MUX;
	else if(s32ChnNum<=16)
		enMode = VO_MODE_16MUX;
	else if(s32ChnNum<=25)
		enMode = VO_MODE_25MUX;
	else if(s32ChnNum<=36)
		enMode = VO_MODE_36MUX;
	else if(s32ChnNum<=49)
		enMode = VO_MODE_49MUX;
	else if(s32ChnNum<=64)
		enMode = VO_MODE_64MUX;
	else
		return -1;

	return Sample_Playback_Start_Mux(s32ChnNum,enMode,960,HD_HEIGHT,pt_types,filenames,fb_cnt,bind,VoLayer);
}


int	Sample_Playback_Stop(void)
{
	int i;
	FY_U32	s32ChnNum;
	int enMode;
	VO_LAYER VoLayer;

	int bothBind;
	int proc_type = 0;

	if(!VdecPlayBackStart)
		return 1;

	if(VdecStartProc)
	{
		VdecTerminateTaskProc = FY_TRUE;
		pthread_join(VdecAutoChansThread, FY_NULL);
		VdecStartProc = FY_FALSE;

		proc_type = task_type;
	}

	VoLayer = VdecVoLayer;
	bothBind = VdecBind;
	s32ChnNum = VdecChanNum;
	enMode = VdecEnMode;


    if(!bothBind){
    	stVdecRead.eCtrlSinal =	VDEC_CTRL_STOP;
	    pthread_join(VdecReadThread, FY_NULL);
    }

	SAMPLE_COMM_VDEC_StopSendStream(s32ChnNum, &stVdecSend[0], &VdecThread[0]);
	sleep(1);
	for(i=0;i<s32ChnNum;i++)
	{
		VDEC_CHN_STAT_S	stStat;
		FY_MPI_VDEC_Query(stVdecSend[i].s32ChnId, &stStat);
		PRINTF_VDEC_CHN_STATE(stVdecSend[i].s32ChnId, stStat);
	}

	if(proc_type != 2)
	{
		SAMPLE_COMM_VDEC_Stop(s32ChnNum);
		for(i=0;i<s32ChnNum;i++)
			SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)i,(VGS_CHN)i);
		SAMPLE_VDEC_VGS_UnBind_VO_Layer(s32ChnNum,VoLayer,VdecStartVoChn);
		SAMPLE_VDEC_VGS_DeInit(s32ChnNum);
		SAMPLE_VDEC_VO_DeInit_Layer(enMode,VoLayer,VdecStartVoChn,s32ChnNum);
	}
	else
	{
	    for(i=0;i<OrgVdecChanNum;i++)
	    {
            FY_S32 s32Chn;
            s32Chn = i;
            if( !( VdecChannsMaps & (1<<s32Chn)))
                continue;

            SAMPLE_COMM_VDEC_StopSendStreamChannel(s32Chn,stVdecSend,VdecThread);
            SAMPLE_COMM_VDEC_Stop_Channel(s32Chn);
            if(VdecBind)
                SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)s32Chn,(VGS_CHN)s32Chn);
            else
                stVdecRead.u32ChnCnt --;
            SAMPLE_COMM_VGS_UnBindVo((VGS_CHN)s32Chn,VdecVoLayer,(VO_CHN)(VdecStartVoChn+ s32Chn));
            SAMPLE_VDEC_Stop_VGS_OneChanne(s32Chn);
            SAMPLE_VDEC_VO_StopChannel_Layer(VdecStartVoChn + s32Chn,VdecVoLayer);
	    }
	}


	FY_MPI_VB_ExitModCommPool(VB_UID_VDEC);
	if(VdecbFav)
	{
		int ao_dev;
		ao_dev =VdecAoDev;

		while(ao_dev)
		{
			int local_ao_dev = 0;

			if(ao_dev==0)
				break;

			if(ao_dev & (1<<I2S_DEV1))
			{
				local_ao_dev = I2S_DEV1;
				ao_dev &= ~(1<<I2S_DEV1);
			}
			else if(ao_dev & (1<<I2S_DEV3))
			{
				local_ao_dev = I2S_DEV3;
				ao_dev &= ~(1<<I2S_DEV3);
			}
			FY_MPI_AO_DisableChn(local_ao_dev, 0);
			FY_MPI_AO_Disable(local_ao_dev);
		}

	}

	VdecPlayBackStart = 0;
	VdecbFav = FY_FALSE;

	return 0;
}

int	Sample_Playback_query(FY_U32 decode_frames[])
{
	int i;
	VDEC_CHN_STAT_S	stStat;
	FY_S32 ret;
	if(!VdecPlayBackStart)
		return 1;

	for(i=0;i<VdecChanNum;i++)
	{
		ret = FY_MPI_VDEC_Query(stVdecSend[i].s32ChnId, &stStat);
		if(ret == FY_SUCCESS)
			decode_frames[i] = stStat.u32DecodeStreamFrames;
		else
			decode_frames[i]  = 0;
	}

	return 0;
}

int	Sample_Playback_queryVgs(FY_U32 resize_frames[])
{
	int i;
    VGS_CHN_STS_S stchnSts;
	if(!VdecPlayBackStart)
		return 1;

	for(i=0;i<VdecChanNum;i++)
	{
		FY_MPI_VGS_ChnQuery(stVdecSend[i].s32ChnId, &stchnSts);

		resize_frames[i] = stchnSts.u32SendFrmOk + stchnSts.u32SendFrmFail;
	}

	return 0;
}

int	Sample_Playback_Ctrl(PLAYBACK_CMD_CODE cmd_code, FY_S32 cmd_parm)
{
	int i;
	FY_S32 ret = 0;
	FY_S32 s32FrameRate;

	if(!VdecPlayBackStart)
		return 1;

	switch(cmd_code)
	{
	case PLAYBACK_CMD_CODE_SPEED:
		{
			s32FrameRate = cmd_parm;
			for(i=0;i<VdecChanNum;i++)
				ret |= FY_MPI_VO_SetChnFrameRate(VdecVoLayer, (VO_CHN)(VdecStartVoChn + i), s32FrameRate);
		}
		break;
	case PLAYBACK_CMD_CODE_PAUSE_RESUME:
		{
			for(i=0;i<VdecChanNum;i++)
			{
				if(cmd_parm==0)
					ret |= FY_MPI_VO_PauseChn(VdecVoLayer, (VO_CHN)(VdecStartVoChn + i));
				else
					ret |= FY_MPI_VO_ResumeChn(VdecVoLayer, (VO_CHN)(VdecStartVoChn + i));
			}
		}
		break;
	case PLAYBACK_CMD_CODE_USERPIC:
		{
			if(cmd_parm==0)
			{
				for(i=0;i<VdecChanNum;i++)
					FY_MPI_VDEC_DisableUserPic((VDEC_CHN)i);

				if(VdecLoadUserPic== FY_SUCCESS)
				{
					SAMPLE_COMM_VDEC_Release_UserPic(&VdecUserPic);
					VdecLoadUserPic = FY_FAILURE;
				}
				ret = 0;
			}
			else
			{
				if(VdecLoadUserPic!= FY_SUCCESS)
					VdecLoadUserPic = SAMPLE_COMM_VDEC_Load_UserPic("novideo.yuv",704,576,PIXEL_FORMAT_YUV_SEMIPLANAR_420,&VdecUserPic);
				if(VdecLoadUserPic== FY_SUCCESS)
				{
					for(i=0;i<VdecChanNum;i++)
						ret |= FY_MPI_VDEC_SetUserPic((VDEC_CHN)i,&VdecUserPic);
					for(i=0;i<VdecChanNum;i++)
						ret |= FY_MPI_VDEC_EnableUserPic((VDEC_CHN)i,FY_TRUE);
				}
			}
		}
		break;
	case PLAYBACK_CMD_CODE_AJUST_CHANNEL:
		{
			FY_U32	s32ChnNum;
			FY_U32 s32Chn;
			s32ChnNum = VdecChanNum;

			if(cmd_parm!=-1 && cmd_parm!=1)
				return -1;
			else if(cmd_parm==-1 && s32ChnNum==0)
				return -1;
			else if(cmd_parm==1 && s32ChnNum==OrgVdecChanNum)
				return -1;

			if(cmd_parm==-1)
				s32Chn = s32ChnNum - 1;
			else
				s32Chn = s32ChnNum;
			if(cmd_parm==-1)
			{
				SAMPLE_COMM_VDEC_StopSendStreamChannel(s32Chn,stVdecSend,VdecThread);
				ret |= SAMPLE_COMM_VDEC_Stop_Channel(s32Chn);
				if(VdecBind)
					ret |= SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)s32Chn,(VGS_CHN)s32Chn);
				else
					stVdecRead.u32ChnCnt --;
				ret |= SAMPLE_COMM_VGS_UnBindVo((VGS_CHN)s32Chn,VdecVoLayer,(VO_CHN)(VdecStartVoChn+ s32Chn));
				SAMPLE_VDEC_Stop_VGS_OneChanne(s32Chn);
				SAMPLE_VDEC_VO_StopChannel_Layer(VdecStartVoChn + s32Chn,VdecVoLayer);
				VdecChanNum--;
			}
			else
			{

				SAMPLE_VDEC_VO_StartChannel_Layer(VdecStartVoChn + s32Chn,VdecEnMode,VdecVoLayer);
				SAMPLE_VDEC_Start_VGS_OneChanne_Layer(s32Chn,VdecEnMode,VdecVoLayer);
				ret |= SAMPLE_COMM_VDEC_Start_Channel(s32Chn,stVdecChnAttr,VdecFbCnt);
				if(VdecBind)
					ret |= SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)s32Chn,(VGS_CHN)s32Chn);
				else
					stVdecRead.u32ChnCnt ++;
				ret |= SAMPLE_COMM_VGS_BindVo((VGS_CHN)s32Chn,VdecVoLayer,(VO_CHN)(VdecStartVoChn + s32Chn));
				SAMPLE_COMM_VDEC_StartSendStreamChannel(s32Chn,stVdecSend,VdecThread);
				VdecChanNum++;
			}
		}
		break;
    case PLAYBACK_CMD_CODE_ONOFF_CHANNEL:
        {
            FY_U32  s32Chn = cmd_parm & 0xFFFF;
            FY_BOOL on = (FY_BOOL)(cmd_parm>>16);
            static int times = 0;

            if(s32Chn>= OrgVdecChanNum )
                return -1;

            printf("\n\n==============s32Chn:%d,on:%d, times:%d======================\n\n",s32Chn,on,times++);
            if(on)
            {
				SAMPLE_VDEC_VO_StartChannel_Layer(VdecStartVoChn + s32Chn,VdecEnMode,VdecVoLayer);
				SAMPLE_VDEC_Start_VGS_OneChanne_Layer(s32Chn,VdecEnMode,VdecVoLayer);
				ret |= SAMPLE_COMM_VDEC_Start_Channel(s32Chn,stVdecChnAttr,VdecFbCnt);
				if(VdecBind)
					ret |= SAMPLE_COMM_VDEC_BindVgs((VDEC_CHN)s32Chn,(VGS_CHN)s32Chn);
				ret |= SAMPLE_COMM_VGS_BindVo((VGS_CHN)s32Chn,VdecVoLayer,(VO_CHN)(VdecStartVoChn + s32Chn));
				SAMPLE_COMM_VDEC_StartSendStreamChannel(s32Chn,stVdecSend,VdecThread);
            }
            else
            {
				SAMPLE_COMM_VDEC_StopSendStreamChannel(s32Chn,stVdecSend,VdecThread);
				ret |= SAMPLE_COMM_VDEC_Stop_Channel(s32Chn);
				if(VdecBind)
					ret |= SAMPLE_COMM_VDEC_UnBindVgs((VDEC_CHN)s32Chn,(VGS_CHN)s32Chn);
				ret |= SAMPLE_COMM_VGS_UnBindVo((VGS_CHN)s32Chn,VdecVoLayer,(VO_CHN)(VdecStartVoChn+ s32Chn));
				SAMPLE_VDEC_Stop_VGS_OneChanne(s32Chn);
				SAMPLE_VDEC_VO_StopChannel_Layer(VdecStartVoChn + s32Chn,VdecVoLayer);
            }
        }
        break;
	case PLAYBACK_CMD_CODE_AUTO_CHANNELS:
		{
			FY_BOOL bstart = (FY_BOOL)cmd_parm;

			if(bstart==VdecStartProc)
				return -1;
			if(bstart==FY_FALSE)
			{
				VdecTerminateTaskProc = FY_TRUE;
				pthread_join(VdecAutoChansThread, FY_NULL);
				VdecStartProc = FY_FALSE;
			}
			else
			{
				VdecTerminateTaskProc = FY_FALSE;
				task_type = 0;
				pthread_create(&VdecAutoChansThread,	0, task_proc, (FY_VOID *)&task_type);
				VdecStartProc = FY_TRUE;
			}
		}
		break;
    case PLAYBACK_CMD_CODE_AUTO_CHANNELS_ONOFF:
        {
			FY_BOOL bstart = (FY_BOOL)cmd_parm;

			if(bstart==VdecStartProc)
				return -1;
			if(bstart==FY_FALSE)
			{
				VdecTerminateTaskProc = FY_TRUE;
				pthread_join(VdecAutoChansThread, FY_NULL);
				VdecStartProc = FY_FALSE;
			}
			else
			{
				VdecTerminateTaskProc = FY_FALSE;
				task_type = 2;
				pthread_create(&VdecAutoChansThread,	0, task_proc, (FY_VOID *)&task_type);
				VdecStartProc = FY_TRUE;
			}
        }
        break;
	case PLAYBACK_CMD_CODE_AUTO_SPEED:
		{
			FY_BOOL bstart = (FY_BOOL)cmd_parm;

			if(bstart==VdecStartProc)
				return -1;
			if(bstart==FY_FALSE)
			{
				VdecTerminateTaskProc = FY_TRUE;
				pthread_join(VdecAutoChansThread, FY_NULL);
				VdecStartProc = FY_FALSE;
			}
			else
			{
				VdecTerminateTaskProc = FY_FALSE;
				task_type = 1;
				pthread_create(&VdecAutoChansThread,	0, task_proc, (FY_VOID *)&task_type);
				VdecStartProc = FY_TRUE;
			}
		}
	default:
		return -1;
	}

	return ret;
}

int	Sample_Playback_queryVppu(FY_U32 resize_frames[])
{
	int i;
    VGS_CHN_STS_S stChnSts;

    if(!VdecPlayBackStart)
		return 1;

	for(i=0;i<VdecChanNum;i++)
	{
		FY_MPI_VGS_ChnQuery(stVdecSend[i].s32ChnId, &stChnSts);
		resize_frames[i] = stChnSts.u32SendFrmOk;
	}

	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
