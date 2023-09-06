#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <dirent.h>

#include "fy_comm_aio.h"
#include "fy_comm_sys.h"
#include "mpi_ai.h"
#include "mpi_sys.h"
#include "mpi_ao.h"
#include "mpi_aenc.h"
#include "mpi_adec.h"
#include "mpi_vo.h"
#include "fy_comm_vo.h"
#include "sample_comm.h"

enum
{
	TYPE_DVR_MC6630=0,
	TYPE_NVR_MC6830,
	TYPE_MAX
};

typedef struct _play_param
{
	FY_BOOL bMusic;
	FY_BOOL bSaveFile;
	FY_BOOL bDec;
	FY_U32 uType;
}play_param;


#define RECORD_FILE_NAME	"audio_test.pcm"

//#define IS_AI_RESAMPLE			FY_TRUE
//#define DEBUG_SEND_BUFFER		FY_TRUE
//#define TEST_48K16BIT			FY_TRUE

#define GET_FRMSIZE(rate,bit)	((rate/25)*(bit/8))

#define AO_ORI_SMPRATE		SAMPLE_AUDIO_RATE
#define HDMI_ORI_SMPRATE	AUDIO_SAMPLE_RATE_8000
#define HDMI_DFT_SMPRATE	AUDIO_SAMPLE_RATE_48000

#define AIO_OPEN_FILE(file,filename,flag) \
do{\
	file = fopen(filename,flag); \
	if(file == NULL)\
	{\
	        printf("[%s]open %s fail\n",__FUNCTION__,filename); \
	        goto exit;\
	}\
}while(0)

#define AIO_CLOSE_FILE(file) \
do{\
	if(file != NULL)\
		fclose(file);\
	file=FY_NULL;\
}while(0)

#define AIO_MUTEX_INIT(handle,ptr) \
do{\
	pthread_mutex_init(&handle,ptr);\
}while(0)

#define AIO_MUTEX_LOCK(handle) \
do{\
	pthread_mutex_lock(&handle);\
}while(0)

#define AIO_MUTEX_UNLOCK(handle) \
do{\
	pthread_mutex_unlock(&handle);\
}while(0)

#define AIO_MUTEX_DESTROY(handle) \
do{\
	pthread_mutex_destroy(&handle);\
}while(0)

#define CHECK_NUM_VALID(n) \
do{\
	if(n < AUDIO_CHN_NUM_4)\
	{\
		printf("[chn %d]exit because out of size!!!!\n",n);\
		goto exit;\
	}\
}while(0)

static FY_BOOL g_bInited=FY_FALSE;
static FY_U32 g_hdmi_samplerate=HDMI_DFT_SMPRATE;
static FY_U32 g_Ai_uChnID=0;
static FY_U32 g_AiFdHandle[FILE_TYPE_TOTAL]={FY_NULL,FY_NULL};
static FILE* g_AoFdHandle=FY_NULL;
static FILE* g_HdmiFdHandle=FY_NULL;
static FY_BOOL g_music_ao=FY_FALSE;
static FY_BOOL g_music_hdmi=FY_FALSE;
static FY_U32 g_chn_num=AUDIO_CHN_NUM_2;

static FY_BOOL g_AiVqeEn=FY_FALSE;
static FY_BOOL g_AoVqeEn=FY_TRUE;

extern FY_U32			g_saveFile;

FY_VOID sample_aio_set_channel(FY_S32 type);

FY_S32 sample_codec_test(FY_S32 devId)
{
	FY_U32 vol=0;
	FY_S32 ret;
	FY_MPI_AO_Enable(devId);
	FY_MPI_AO_SetVolume(devId, ACW_VOLUME_AO_ANA, CODEC_TYPE_FLAG|vol);
	ret = FY_MPI_AO_Disable(devId);
	return ret;
}

FY_S32 sample_start_chn(AIO_TYPE type,FY_S32 devId,FY_S32 chn)
{
	FY_S32 ret=FY_SUCCESS;

	switch(type)
	{
		case SELECT_TYPE_AI:
		{
			ret=FY_MPI_AI_EnableChn(devId, chn);
		}
		break;

		case SELECT_TYPE_AO:
		{
			ret=FY_MPI_AO_EnableChn(devId, chn);
		}
		break;

		default:
			SAMPLE_PRT("[devid-%d] is invalid!!!\n",devId);
		return -1;
	}
	return ret;
}

FY_S32 sample_stop_chn(AIO_TYPE type,FY_S32 devId,FY_S32 chn)
{
	FY_S32 ret=FY_SUCCESS;

	switch(type)
	{
		case SELECT_TYPE_AI:
		{
			ret=FY_MPI_AI_DisableChn(devId, chn);
			FY_MPI_AI_DisableReSmp(devId, chn);
		}
		break;

		case SELECT_TYPE_AO:
		{
			ret=FY_MPI_AO_DisableChn(devId, chn);
			FY_MPI_AO_DisableReSmp(devId, chn, 0);
		}
		break;

		default:
			SAMPLE_PRT("[devid-%d] is invalid!!!\n",devId);
		return -1;
	}
	return ret;
}

FY_S32 sample_start_dev(AIO_TYPE type,FY_S32 devId)
{
	FY_S32 ret=FY_SUCCESS;

	switch(type)
	{
		case SELECT_TYPE_AI:
		{
			ret=FY_MPI_AI_Enable(devId);
		}
		break;

		case SELECT_TYPE_AO:
		{
			ret=FY_MPI_AO_Enable(devId);
		}
		break;

		default:
			SAMPLE_PRT("[devid-%d] is invalid!!!\n",devId);
		return -1;
	}
	return ret;
}


FY_S32 sample_stop_dev(AIO_TYPE type,FY_S32 devId)
{
	FY_S32 ret=FY_SUCCESS;

	//printf("%s:enter devId-%d\n",__FUNCTION__,devId);
	switch(type)
	{
		case SELECT_TYPE_AI:
		{
			if(g_AiVqeEn)
			{
				FY_MPI_AI_DisableVqe(devId,g_Ai_uChnID);
			}

			ret=FY_MPI_AI_Disable(devId);
		}
		break;

		case SELECT_TYPE_AO:
		{
			if(g_AoVqeEn)
			{
				FY_MPI_AO_DisableVqe(devId,0);
			}
			ret=FY_MPI_AO_Disable(devId);
		}
		break;

		default:
			SAMPLE_PRT("[devid-%d] is invalid!!!\n",devId);
		return -1;
	}
	//printf("%s:exit devId-%d\n",__FUNCTION__,devId);
	return ret;
}


FY_S32 sample_set_i2s_param(AIO_TYPE type,AUDIO_DEV devID,FY_S32 chn,FY_U32 fs,FY_U32 chn_num,FY_U32 slave,FY_U32 dFmt)
{
	AIO_ATTR_S stAioAttr;
	AUDIO_FRAME_E frameInfo;
	VO_HDMI_AUDIO_S resampleRate;
	FY_U32 uRate=0;
	FY_BOOL bConvert=FY_FALSE;
	FY_U32 flag=AUDIO_BIT_WIDTH_16;

	sample_stop_dev(type,devID);
	switch(type)
	{
		case SELECT_TYPE_AI:
		{
			switch(devID)
			{
				case SAMPLE_AUDIO_AI_DEV:
				{
					stAioAttr.fs_rate	= fs;
					stAioAttr.chn_num	= chn_num;
					stAioAttr.is_slave	= slave;
					stAioAttr.set_param	= (stAioAttr.is_slave == AIO_MODE_I2S_MASTER)?I2S_CLK_DIV:I2S_CHN_CMD;;
					stAioAttr.chn_width     = AUDIO_BIT_WIDTH_16;
					stAioAttr.i2s_data_format = dFmt;
					stAioAttr.i2s_rxt_mode	= I2S_RXF_MODE;
					stAioAttr.io_type	= AC_MIC_IN;

					frameInfo.frame_cnt	= AI_CACHE_FRM_NUM;
					frameInfo.fileInfo.isNeedSave = FY_FALSE;

					if(stAioAttr.i2s_data_format == I2S_MIX_DATA_FORMAT)
					{
						frameInfo.frame_size	= GET_FRMSIZE(fs,stAioAttr.chn_width)*stAioAttr.chn_num;
						stAioAttr.u32DmaSize	= frameInfo.frame_size;
					}
					else
					{
						frameInfo.frame_size	= GET_FRMSIZE(fs,stAioAttr.chn_width);
						stAioAttr.u32DmaSize	= frameInfo.frame_size*stAioAttr.chn_num;
					}

					FY_MPI_AI_SetPubAttr(devID,IOC_AICMD_BUTT, &stAioAttr);
					FY_MPI_AI_SetChnParam(devID, chn,&frameInfo);
					FY_MPI_AI_DisableChn(devID, chn);

					FY_MPI_AI_SetTrackMode(devID, AUDIO_TRACK_LEFT);

					#ifdef IS_AI_RESAMPLE
					uRate = g_hdmi_samplerate;
					FY_MPI_AI_EnableReSmp(devID,chn,uRate,AUDIO_BIT_WIDTH_32);
					#endif
				}
				break;
				default:
					SAMPLE_PRT("[devid-%d] is invalid!!!\n",devID);
					break;
			}
		}
		break;

		case SELECT_TYPE_AO:
		{
			switch(devID)
			{
				case SAMPLE_AUDIO_AO_DEV:
				case DEV_I2S1_WITH_AC:
				{
					stAioAttr.fs_rate	= fs;
					stAioAttr.chn_num	= chn_num;
					stAioAttr.is_slave	= slave;
					stAioAttr.set_param	= (stAioAttr.is_slave == AIO_MODE_I2S_MASTER)?I2S_CLK_DIV:I2S_CHN_CMD;;
					stAioAttr.chn_width	= AUDIO_BIT_WIDTH_16;
					stAioAttr.i2s_data_format = dFmt;
					stAioAttr.i2s_rxt_mode	= I2S_TXF_MODE;

					frameInfo.frame_cnt	= AO_CACHE_FRM_NUMBER;
					frameInfo.frame_size	= GET_FRMSIZE(fs,stAioAttr.chn_width);
					if(!IS_ACW_ID(devID))
						frameInfo.frame_size	*= stAioAttr.chn_num;
					frameInfo.fileInfo.isNeedSave = FY_FALSE;
					stAioAttr.u32DmaSize	= frameInfo.frame_size;
					stAioAttr.io_type	= AC_LINE_HPOUT;
					//printf("[test-ao]%d - num %d - %d -- %d\n",frameInfo.frame_size,stAioAttr.chn_num,SAMPLE_AUDIO_FRM_SIZE,SAMPLE_AUDIO_FRM_SIZE*stAioAttr.chn_num);

					frameInfo.fileInfo.isNeedSave = FY_FALSE;
					if(frameInfo.fileInfo.isNeedSave)
					{
						sprintf(frameInfo.fileInfo.fileName,"/nfs/xvr_ao_%d.pcm",chn);
					}

					FY_MPI_AO_SetPubAttr(devID,IOC_AICMD_BUTT, &stAioAttr);
					FY_MPI_AO_SetChnParam(devID, chn,&frameInfo);
					FY_MPI_AO_DisableChn(devID, chn);

					FY_MPI_AO_SetTrackMode(devID, AUDIO_TRACK_LEFT);

					if(!IS_ACW_ID(devID))
					{
						#ifndef IS_AI_RESAMPLE
						uRate = AO_ORI_SMPRATE;
						FY_MPI_AO_EnableReSmp(devID,chn,uRate,flag);
						#endif
					}
				}
				break;

				case SAMPLE_AUDIO_HDMI_AO_DEV:
				{
					resampleRate.enSampleRate=fs;
					FY_MPI_VO_SetHdmiAudio(0,&resampleRate);

					stAioAttr.fs_rate	= fs;
					stAioAttr.chn_num	= chn_num;
					stAioAttr.is_slave	= slave;
					stAioAttr.set_param	= (stAioAttr.is_slave == AIO_MODE_I2S_MASTER)?I2S_CLK_DIV:I2S_CHN_CMD;;
					stAioAttr.chn_width	= AUDIO_BIT_WIDTH_32;
					stAioAttr.i2s_data_format = dFmt;
					stAioAttr.i2s_rxt_mode	= I2S_TXF_MODE;

					frameInfo.frame_cnt	= AO_CACHE_FRM_NUMBER;
					frameInfo.frame_size	= GET_FRMSIZE(fs,stAioAttr.chn_width);
					frameInfo.frame_size	*= stAioAttr.chn_num;//left-right-track
					frameInfo.fileInfo.isNeedSave = FY_FALSE;
					stAioAttr.u32DmaSize = frameInfo.frame_size;

					//frameInfo.fileInfo.isNeedSave = FY_TRUE;
					if(frameInfo.fileInfo.isNeedSave)
					{
						sprintf(frameInfo.fileInfo.fileName,"/nfs/xvr_hdmi_%d.pcm",chn);
					}

					//printf("[AIO_SET_RESAMPLE]fs=%d,num=%d,framsize=%d\n",stAioAttr.fs_rate,stAioAttr.chn_num,frameInfo.frame_size);
					FY_MPI_AO_SetPubAttr(devID,IOC_AICMD_BUTT, &stAioAttr);
					FY_MPI_AO_SetChnParam(devID, chn,&frameInfo);
					FY_MPI_AO_DisableChn(devID, chn);

					#ifdef IS_AI_RESAMPLE
					uRate = g_hdmi_samplerate;
					FY_MPI_AO_EnableReSmp(devID,chn,uRate,AUDIO_BIT_WIDTH_32);
					#else

					#ifdef TEST_48K16BIT
					{
						bConvert = FY_TRUE;
						flag |= bConvert << 16;
						flag |= AUDIO_SOUND_MODE_STEREO << 8 ;


						//FY_MPI_AO_DisableReSmp(devID, chn, flag);
						uRate = AUDIO_SAMPLE_RATE_48000;
						FY_MPI_AO_EnableReSmp(devID,chn,uRate,flag);
					}
					#else
					uRate = HDMI_ORI_SMPRATE;
					flag |= bConvert << 16;
					FY_MPI_AO_EnableReSmp(devID,chn,uRate,flag);
					#endif

					#endif
				}
				break;
				default:
				SAMPLE_PRT("[devid-%d] is invalid!!!\n",devID);
				break;
			}
		}
		break;

		default:
			SAMPLE_PRT("[devid-%d] is invalid!!!\n",devID);
		return -1;
	}

	return FY_SUCCESS;
}

FY_S32 sample_start_record(AIO_TYPE type,FY_S32 AiDevId,FY_S32 AiChn,FY_S32 bSaveFile,FY_BOOL bMusic)
{
	FILE* pFile=FY_NULL;
	FY_U32	dataFmt;
	FY_U32 slave=AIO_MODE_I2S_SLAVE;

	if(bSaveFile)
	{
		if(AiChn == 0)
		{
			AIO_OPEN_FILE(pFile,RECORD_FILE_NAME,"w+");
		}
		else
		{
			FY_CHAR buff[32];
			memset(buff,0,sizeof(buff));
			sprintf(buff,"single_chn%d.pcm",AiChn);
			AIO_OPEN_FILE(pFile,buff,"w+");
		}
	}
	g_AiFdHandle[FILE_TYPE_AI]=(FY_U32)pFile;
	dataFmt = AiChn==0?I2S_MIX_DATA_FORMAT:I2S_CHN_DATA_FORMAT;
	slave = CODEC_MASTER_MODE?AIO_MODE_I2S_MASTER:AIO_MODE_I2S_SLAVE;
	g_Ai_uChnID = AiChn==0?0:(AiChn-1);
	sample_set_i2s_param(type,AiDevId,g_Ai_uChnID,SAMPLE_AUDIO_RATE,g_chn_num,slave,dataFmt);
	sample_start_chn(type,AiDevId,g_Ai_uChnID);
	sample_start_dev(type,AiDevId);
	if(bMusic)
	{
		SAMPLE_COMM_AUDIO_fifo_init();
		SAMPLE_COMM_AUDIO_CreatTrdAiToFifo(AiDevId,g_Ai_uChnID,g_AiFdHandle);
	}
	else
	{
		SAMPLE_COMM_AUDIO_CreatTrdAi(AiDevId,g_Ai_uChnID,g_AiFdHandle);
	}
exit:
	return 0;
}

FY_S32 sample_stop_record(AIO_TYPE type,FY_S32 AiDevId,FY_S32 AiChn)
{
	int i;
	printf("%s:type %d devid %d,chn %d\n",__FUNCTION__,type,AiDevId,AiChn);
	SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDevId, AiChn);
	sample_stop_chn(type,AiDevId, AiChn);
	sample_stop_dev(type,AiDevId);
	for(i=0;i<FILE_TYPE_TOTAL;i++)
	{
		FILE* pFile=(FILE*)g_AiFdHandle[i];
		AIO_CLOSE_FILE(pFile);
		g_AiFdHandle[i]=FY_NULL;
	}
	printf("%s:exit\n",__FUNCTION__);
	return 0;
}

FY_S32 sample_start_play(AIO_TYPE type,FY_S32 AoDevId,FY_S32 AoChn,play_param* pATR)
{
	FILE* pFile=FY_NULL;
	FY_CHAR buff[32];
	FY_U32 sampleRate,is_slave;
	FY_S32 ret;
	FY_U32 readSize=SAMPLE_AUDIO_FRM_SIZE;

	if(!pATR->bMusic)
	{
		switch(AoChn)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			{
				memset(buff,0,sizeof(buff));
				sprintf(buff,"single_chn%d.pcm",AoChn);
				AIO_OPEN_FILE(pFile,buff,"r");
			}
			break;
			default:
				printf("no input files\n");
			return -1;
		}
	}

	switch(AoDevId)
	{
		case SAMPLE_AUDIO_AO_DEV:
		case DEV_I2S1_WITH_AC:
			sampleRate=SAMPLE_AUDIO_RATE;
			is_slave=CODEC_MASTER_MODE?AIO_MODE_I2S_MASTER:AIO_MODE_I2S_SLAVE;
			g_AoFdHandle=pFile;
			g_music_ao=pATR->bMusic;

			#ifndef IS_AI_RESAMPLE
	 		readSize = GET_FRMSIZE(AO_ORI_SMPRATE, AUDIO_BIT_WIDTH_16);
			#endif

			break;
		case SAMPLE_AUDIO_HDMI_AO_DEV:
			sampleRate=g_hdmi_samplerate;
			is_slave=AIO_MODE_I2S_MASTER;
			g_HdmiFdHandle=pFile;
			g_music_hdmi=pATR->bMusic;

			#ifndef IS_AI_RESAMPLE
	 		readSize = GET_FRMSIZE(HDMI_ORI_SMPRATE, AUDIO_BIT_WIDTH_16);
			#endif
			break;
		default:
			printf("no support ao devid\n");
		return -1;
	}
	sample_set_i2s_param(SELECT_TYPE_AO,AoDevId,0,sampleRate,AUDIO_CHN_NUM_2,is_slave,I2S_MIX_DATA_FORMAT);
	sample_start_chn(type,AoDevId,0);
	sample_start_chn(type,AoDevId,1);
	sample_start_dev(type,AoDevId);

	if(AoDevId == DEV_I2S1_WITH_AC)
	{
		ret = FY_MPI_AO_SetVolume(AoDevId, ACW_VOLUME_AO_DIG, 0x18);
		printf("[set-vol]ret = 0x%x\n",ret);
	}

	if(pATR->bMusic)
	{
		SAMPLE_COMM_AUDIO_CreatTrdFifoToAO(AoDevId,0,FY_NULL);
	}
	else
	{
		#ifdef TEST_48K16BIT
		readSize = 7680;
		//readSize = 3840;
		#endif

		SAMPLE_COMM_AUDIO_CreatTrdAO(AoDevId,0,readSize,pFile);
	}
exit:
	return 0;
}

FY_S32 sample_stop_play(AIO_TYPE type,FY_S32 AoDevId,FY_S32 AoChn)
{
	printf("%s:type %d devid %d,chn %d\n",__FUNCTION__,type,AoDevId,AoChn);
	SAMPLE_COMM_AUDIO_DestoryTrdAo(AoDevId,AoChn);
	sample_stop_chn(type,AoDevId, AoChn);
	sample_stop_dev(type,AoDevId);
	if(AoDevId == SAMPLE_AUDIO_AO_DEV)
	{
		AIO_CLOSE_FILE(g_AoFdHandle);
	}
	else
	{
		AIO_CLOSE_FILE(g_HdmiFdHandle);
	}
	printf("%s:exit\n",__FUNCTION__);
	return 0;
}

FY_VOID sample_clr_status(int type,FY_S32 DevId)
{
	switch(DevId)
	{
		case I2S_DEV_TOTAL:
			sample_stop_play(SELECT_TYPE_AO,SAMPLE_AUDIO_AO_DEV,0);
			sample_stop_play(SELECT_TYPE_AO,SAMPLE_AUDIO_HDMI_AO_DEV,0);
			sample_stop_record(SELECT_TYPE_AI,SAMPLE_AUDIO_AI_DEV,g_Ai_uChnID);
			break;
		case SAMPLE_AUDIO_AI_DEV:
			if(type == SELECT_TYPE_AI)
			{
				sample_stop_record(SELECT_TYPE_AI,SAMPLE_AUDIO_AI_DEV,g_Ai_uChnID);
			}
			else
			{
				sample_stop_play(SELECT_TYPE_AO,DevId,0);
			}
			break;
		default:
			sample_stop_play(SELECT_TYPE_AO,DevId,0);
			break;
	}
}

FY_VOID sample_aio_deinit()
{
	if(g_bInited)
	{
		sample_clr_status(I2S_DEV_TOTAL,I2S_DEV_TOTAL);
		sample_stop_dev(SELECT_TYPE_AI,SAMPLE_AUDIO_AI_DEV);
		sample_stop_dev(SELECT_TYPE_AO,SAMPLE_AUDIO_AO_DEV);
		sample_stop_dev(SELECT_TYPE_AO,SAMPLE_AUDIO_HDMI_AO_DEV);
		g_bInited=FY_FALSE;
	}
}


static FY_VOID exception_signal_handler(int sig)
{
	sample_aio_deinit();
	exit(0);
}


FY_VOID sample_aio_init(FY_S32 type)
{
	//printf("sample_aio_init %d\n",open("/dev/acw", O_RDWR, 0));
	if(!g_bInited)
	{
		#if 0
		/**** step.1 set AI param****/
		sample_set_i2s_param(SAMPLE_AUDIO_AI_DEV,0,SAMPLE_AUDIO_RATE,AUDIO_CHN_NUM_2,
				AIO_MODE_I2S_SLAVE,I2S_CHN_DATA_FORMAT);
		/**** step.2 set AO param****/
		sample_set_i2s_param(SAMPLE_AUDIO_AO_DEV,0,SAMPLE_AUDIO_RATE,AUDIO_CHN_NUM_2,
				AIO_MODE_I2S_SLAVE,I2S_MIX_DATA_FORMAT);
		/**** step.3 set HDMI param****/
		sample_set_i2s_param(SAMPLE_AUDIO_HDMI_AO_DEV,0,g_hdmi_samplerate,AUDIO_CHN_NUM_2,
				AIO_MODE_I2S_MASTER,I2S_MIX_DATA_FORMAT);
		#endif
		if(access("/proc/umap/ad",F_OK)!=-1)
		{
			//printf("path exist!\n");
			system("echo p=9,r=0x18,v=0x30 >/proc/umap/ad");
		}
		sample_aio_set_channel(type);
		g_bInited=FY_TRUE;
		signal(SIGINT,  exception_signal_handler);
	}
}
int ctrl_flag=0;
FY_S32 sample_aio_test(FY_U32 cmd)
{
	FY_S32 AiChn=0,AoChn=0;
	play_param param;
	FY_S32 AiDevID=SAMPLE_AUDIO_AI_DEV;
	FY_S32 AoDevID=SAMPLE_AUDIO_AO_DEV;
	FY_S32 HdmiDevID=SAMPLE_AUDIO_HDMI_AO_DEV;

	g_hdmi_samplerate=HDMI_DFT_SMPRATE;
	switch(cmd)
	{
		case 1:
		{
			char option,ch1;
			FY_BOOL isExit=FY_FALSE;

			sample_clr_status(SELECT_TYPE_AI,AiDevID);
			sample_clr_status(SELECT_TYPE_AO,AoDevID);
			sample_clr_status(SELECT_TYPE_AO,HdmiDevID);
			while(1)
			{
				printf("\n\n=============================================================\n\n");
				printf("Option[0 ~ 4]:\n\n");
				printf("    0:          mixData \n    1/2/3/4:    single channel number\n\n");
				printf("==============================================================\n\n");
				printf("\033[1;31;44m Enter your choice [0/1/2/3/4]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
				ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
							;
					}
					continue;
				}
				printf("option %c\n",option);
				switch(option)
				{
					case '4':
						AiChn++;
					case '3':
						AiChn++;
						CHECK_NUM_VALID(g_chn_num);
					case '2':
						AiChn++;
					case '1':
						AiChn++;
					case '0':
						printf("ok,AiChn=%d\n",AiChn);
						isExit=FY_TRUE;
						break;
					case 'q':
						goto exit;
					default:
						isExit=FY_FALSE;
					break;
				}
				if(isExit)
				{
					break;
				}
			}

			sample_start_record(SELECT_TYPE_AI,AiDevID,AiChn,FY_TRUE,FY_FALSE);
			printf("\n\033[31m ########  [AUDIO]START RECORD FILES!  ########\033[0m\n");
		}
		break;

		case 2:
		{
			char option,ch1;
			FY_BOOL isExit=FY_FALSE;

			//sample_clr_status(SELECT_TYPE_AI,AiDevID);
			sample_clr_status(SELECT_TYPE_AO,AoDevID);
			while(1)
			{
				printf("\n\n=============================================================\n\n");
				printf("Option[1 ~ 4]:\n\n");
				printf("    1/2/3/4:    single channel number\n\n");
				printf("==============================================================\n\n");
				printf("\033[1;31;44m Enter your choice [1/2/3/4]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
		        	ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
					    ;
					}
					continue;
				}
				switch(option)
				{
					case '4':
						AoChn++;
					case '3':
						AoChn++;
					case '2':
						AoChn++;
					case '1':
						AoChn++;
					//case '0':
						printf("ok,AoChn=%d\n",AoChn);
						isExit=FY_TRUE;
						break;
					case 'q':
						goto exit;
					default:
						isExit=FY_FALSE;
					break;
				}
				if(isExit)
				{
					break;
				}
			}

			param.bDec=FY_FALSE;
			param.bMusic=FY_FALSE;
			param.bSaveFile=FY_FALSE;
			param.uType=0;
			//AoDevID = DEV_I2S1_WITH_AC;
			sample_start_play(SELECT_TYPE_AO,AoDevID,AoChn,&param);
			printf("\n\033[31m ########  [AUDIO]START PLAY FILES!  ########\033[0m\n");
		}
		break;

		case 3:
		{
			char option,ch1;
			FY_BOOL isExit=FY_FALSE;

			if(g_music_hdmi)
			{
				sample_clr_status(SELECT_TYPE_AO,HdmiDevID);
				g_music_hdmi=FY_FALSE;
			}
			sample_clr_status(SELECT_TYPE_AO,AoDevID);
			sample_clr_status(SELECT_TYPE_AI,AiDevID);
			SAMPLE_COMM_AUDIO_fifo_deinit();
			while(1)
			{
				printf("\n\n=============================================================\n\n");
				printf("Option[1 ~ 4]:\n\n");
				printf("	  1/2/3/4:	  channel number\n\n");
				printf("==============================================================\n\n");
				printf("\033[1;31;44m Enter your choice [1/2/3/4]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
				ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
							;
					}
					continue;
				}
				switch(option)
				{
					case '4':
						AiChn++;
					case '3':
						AiChn++;
						CHECK_NUM_VALID(g_chn_num);
					case '2':
						AiChn++;
					case '1':
						AiChn++;
						printf("ok,musicChn=%d\n",AiChn);
						isExit=FY_TRUE;
						break;
					case 'q':
						goto exit;
					default:
						isExit=FY_FALSE;
					break;
				}
				if(isExit)
				{
					break;
				}
			}

			sample_start_record(SELECT_TYPE_AI,AiDevID,AiChn,FY_FALSE,FY_TRUE);
			param.bDec=FY_FALSE;
			param.bMusic=FY_TRUE;
			param.bSaveFile=FY_FALSE;
			param.uType=0;
			sample_start_play(SELECT_TYPE_AO,AoDevID,AoChn,&param);
			printf("\n\033[31m ########  [AUDIO]START PLAY MUSIC!  ########\033[0m\n");
		}
		break;

		case 4:
		{
			#ifdef DEBUG_SEND_BUFFER
			{
				if(ctrl_flag)
					ctrl_flag=0;
				else
					ctrl_flag=1;
				SAMPLE_COMM_AUDIO_SetBuff(ctrl_flag);
				break;
			}
			#endif

			if(0)
			{
				static FY_BOOL flag=FY_FALSE;
				AUDIO_SAVE_FILE_INFO_S file;

				flag = !flag;

				file.bCfg = flag;
				file.u32FileSize = 0;
				//file.u32FileSize = 1024*1024;

				sprintf(file.aFilePath,"/nfs/");
				sprintf(file.aFileName,"ai_%d.pcm",AiChn);
				FY_MPI_AI_SaveFile(AiDevID, AiChn, &file);

				sprintf(file.aFileName,"ao_%d.pcm",AoChn);
				FY_MPI_AO_SaveFile(AoDevID, AoChn, &file);
				break;
			}
			//sample_codec_test(DEV_ONLY_AC);
			//g_saveFile = (g_saveFile+1)%2;
			//break;
			sample_clr_status(I2S_DEV_TOTAL,I2S_DEV_TOTAL);
			printf("\n\n\033[31m ########  [AUDIO]STATUS HAS BEEN CLEANED!  ########\033[0m\n");
		}
		break;

		case 5:
		{
			char option,ch1;
			FY_BOOL isExit=FY_FALSE;

			//sample_clr_status(SELECT_TYPE_AI,AiDevID);
			sample_clr_status(SELECT_TYPE_AO,HdmiDevID);
			while(1)
			{
				printf("\n\n=============================================================\n\n");
				printf("Option[1 ~ 4]:\n\n");
				printf("    1/2/3/4:    single channel number 48k\n\n");
				printf("    q:          exit\n\n");
				printf("==============================================================\n\n");
				printf("\n\n\033[1;31;44m Enter your choice [1/2/3/4/5,q]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
		        	ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
						;
					}
					continue;
				}
				switch(option)
				{
					case '4':
						AoChn++;
					case '3':
						AoChn++;
					case '2':
						AoChn++;
					case '1':
						AoChn++;
					case '0':
						isExit=FY_TRUE;
						break;
					case 'q':
						goto exit;
					default:
						isExit=FY_FALSE;
					break;
				}
				if(isExit)
				{
					break;
				}
			}

			param.bDec=FY_FALSE;
			param.bMusic=FY_FALSE;
			param.bSaveFile=FY_FALSE;
			param.uType=0;
			sample_start_play(SELECT_TYPE_AO,HdmiDevID,AoChn,&param);
			printf("\n\033[31m ########  [AUDIO]START PLAY FILES TO HDMI!  ########\033[0m\n");
		}
		break;
		case 6:
		{
			char option,ch1;
			FY_BOOL isExit=FY_FALSE;

			if(g_music_ao)
			{
				sample_clr_status(SELECT_TYPE_AO,AoDevID);
				g_music_ao=FY_FALSE;
			}
			sample_clr_status(SELECT_TYPE_AO,HdmiDevID);
			sample_clr_status(SELECT_TYPE_AI,AiDevID);
			SAMPLE_COMM_AUDIO_fifo_deinit();
			while(1)
			{
				printf("\n\n=============================================================\n\n");
				printf("Option[1 ~ 4]:\n\n");
				printf("    1/2/3/4:	  channel number\n\n");
				printf("    q:          exit\n\n");
				printf("==============================================================\n\n");
				printf("\033[1;31;44m Enter your choice [1/2/3/4/5,q]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
				ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
							;
					}
					continue;
				}
				switch(option)
				{
					case '4':
						AiChn++;
					case '3':
						AiChn++;
						CHECK_NUM_VALID(g_chn_num);
					case '2':
						AiChn++;
					case '1':
						AiChn++;
						printf("ok,hdmiChn=%d\n",AiChn);
						isExit=FY_TRUE;
						break;
					case 'q':
						goto exit;
					default:
						isExit=FY_FALSE;
					break;
				}
				if(isExit)
				{
					break;
				}
			}

			sample_start_record(SELECT_TYPE_AI,AiDevID,AiChn,FY_FALSE,FY_TRUE);
			param.bDec=FY_FALSE;
			param.bMusic=FY_TRUE;
			param.bSaveFile=FY_FALSE;
			param.uType=0;
			sample_start_play(SELECT_TYPE_AO,HdmiDevID,AoChn,&param);
			printf("\n\033[31m ########  [AUDIO]START PLAY MUSIC TO HDMI!  ########\033[0m\n");
		}
		break;
		case 7:
		{
			char option,ch1;
			FY_U32 anr_level,agc_level;

			while(1)
			{
				printf("\n\n==================   VQE TEST MENU   ====================\n\n");
				printf("Option[1 ~ 9]:\n\n");
				printf("    1:		AI-VQE[ANR-LEVEL:1 AGC-LEVEL:1]\n\n");
				printf("    2:		AI-VQE[ANR-LEVEL:2 AGC-LEVEL:2]\n\n");
				printf("    3:		AI-VQE[ANR-LEVEL:3 AGC-LEVEL:3]\n\n");
				printf("    4:		AI-VQE[VOL-range(0~255):step+30]\n\n");
				printf("    5:		AO-VQE[ANR-LEVEL:1 AGC-LEVEL:1]\n\n");
				printf("    6:		AO-VQE[ANR-LEVEL:2 AGC-LEVEL:2]\n\n");
				printf("    7:		AO-VQE[ANR-LEVEL:3 AGC-LEVEL:3]\n\n");
				printf("    8:		AO-VQE[VOL-range(0~255):step+30]\n\n");
				printf("    9:		stop vqe\n\n");
				printf("    q:          exit\n\n");
				printf("==============================================================\n\n");
				printf("\033[1;31;44m Enter your choice [1-9,q]: \033[0m");
				option = getchar();
				if(10 == option)
				{
					continue;
				}
				ch1 = getchar();
				if(10 != ch1)
				{
					while((ch1 = getchar()) != '\n' && ch1 != EOF);
					{
						;
					}
					continue;
				}
				switch(option)
				{
					case '1':
						anr_level = 1;
						agc_level = 1;
						goto ai_label;
					case '2':
						anr_level = 2;
						agc_level = 2;
						goto ai_label;
					case '3':
						anr_level = 3;
						agc_level = 3;
						goto ai_label;
					case '4':
						SAMPLE_COMM_AUDIO_set_volume(AiDevID);
						break;

					case '5':
						anr_level = 1;
						agc_level = 1;
						goto ao_label;
					case '6':
						anr_level = 2;
						agc_level = 2;
						goto ao_label;
					case '7':
						anr_level = 3;
						agc_level = 3;
						goto ao_label;
					case '8':
						SAMPLE_COMM_AUDIO_set_volume(AoDevID);
						break;

					case '9':
					{
						if(g_AiVqeEn)
						{
							FY_MPI_AI_DisableVqe(AiDevID,g_Ai_uChnID);
							g_AiVqeEn=FY_FALSE;
						}

						if(g_AoVqeEn)
						{
							FY_MPI_AO_DisableVqe(AoDevID,0);
							g_AoVqeEn=FY_FALSE;
						}
					}
					break;

					case 'q':
						goto exit;
					default:
					break;
				}

				continue;

			ai_label:
				SAMPLE_COMM_AUDIO_enable_vqe(AiDevID,anr_level,agc_level);
				g_AiVqeEn=FY_TRUE;
				continue;
			ao_label:
				SAMPLE_COMM_AUDIO_enable_vqe(AoDevID,anr_level,agc_level);
				g_AoVqeEn=FY_TRUE;
				continue;
			}
		}
		break;
		default:
		break;
	}
exit:
	return FY_SUCCESS;
}
FY_S32 sample_hdmi_play_files()
{
	printf("\n\nSLT5 AUDIO: enter\n");

	printf("SLT5 AUDIO:OK\n\n");
	return FY_SUCCESS;
}
FY_VOID sample_aio_set_channel(FY_S32 type)
{
	switch(type)
	{
		case TYPE_DVR_MC6630:
			g_chn_num=AUDIO_CHN_NUM_16;
		break;
		case TYPE_NVR_MC6830:
			g_chn_num=AUDIO_CHN_NUM_2;
		break;
		default:
		break;
	}
}


FY_S32 sample_aio_stress_test()
{
	play_param param;

	if(g_music_hdmi)
	{
		sample_clr_status(SELECT_TYPE_AO,SAMPLE_AUDIO_HDMI_AO_DEV);
		g_music_hdmi=FY_FALSE;
	}
	sample_clr_status(SELECT_TYPE_AO,SAMPLE_AUDIO_AO_DEV);
	sample_clr_status(SELECT_TYPE_AI,SAMPLE_AUDIO_AI_DEV);
	SAMPLE_COMM_AUDIO_fifo_deinit();

	sample_start_record(SELECT_TYPE_AI,SAMPLE_AUDIO_AI_DEV,1,FY_FALSE,FY_TRUE);
	param.bDec=FY_FALSE;
	param.bMusic=FY_TRUE;
	param.bSaveFile=FY_FALSE;
	param.uType=0;
	sample_start_play(SELECT_TYPE_AO,SAMPLE_AUDIO_AO_DEV,1,&param);
	printf("\n\033[31m ########  [AUDIO]START PLAY MUSIC!  ########\033[0m\n");

	return FY_SUCCESS;
}
