#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "sample_comm.h"
#include "fy_comm_vo.h"
#include <sys/prctl.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct tagSAMPLE_AENC_S
{
	FY_BOOL bStart;
	pthread_t stAencPid;
	FY_S32  AeChn;
	FY_S32  AdChn;
	FILE    *pfd;
	FY_BOOL bSaveFile;
	FY_BOOL bSendAdChn;
} SAMPLE_AENC_S;

typedef struct tagSAMPLE_AI_S
{
	volatile FY_BOOL bStart;
	FY_S32  AiDev;
	FY_S32  AiChn;
	FY_S32  AencChn;
	FY_S32  AoDev;
	FY_S32  AoChn;
	FY_S32  AdecChn;
	FY_BOOL bSaveOriFile;
	FY_BOOL bSendAenc;
	FY_BOOL bSaveAencFile;
	FY_BOOL bSendAo;
	FY_BOOL bSendToFifo;
	FY_BOOL bSendAdec;
	FILE *pfd[FILE_TYPE_TOTAL];
	pthread_t stAiPid;
	PAYLOAD_TYPE_E enType;
} SAMPLE_AI_S;

typedef struct tagSAMPLE_ADEC_S
{
	FY_BOOL bStart;
	FY_S32  AdChn;
	FY_S32  AoDev;
	FY_S32  AoChn;
	FY_BOOL bManual;
	FILE	*pfd;
	pthread_t stAdPid;
	FY_U32	uRdFrmLen;
	PAYLOAD_TYPE_E enType;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AO_S
{
	AUDIO_DEV AoDev;
	FY_S32	AoChn;
	volatile FY_BOOL bStart;
	FY_BOOL bRdFromFifo;
	FILE	*pfd;
	pthread_t stAoPid;
	FY_U32 uRdFrmLen;
	AUDIO_FRAME_S	frmInfo;
	FY_BOOL	bAllocMem;
}SAMPLE_AO_S;

typedef struct _AUDIO_FIFO_M
{
	FY_BOOL	bWriteFlg;
	FY_VOID*pAddr;
	FY_U32	uSize;
	FY_U32	u32phyaddr;
}AUDIO_FIFO_M;

static SAMPLE_AI_S	gs_stSampleAi[I2S_DEV_TOTAL*MAX_AUDIO_CHN_NUM];
static SAMPLE_AENC_S	gs_stSampleAenc[AENC_MAX_CHN_NUM];
static SAMPLE_ADEC_S	gs_stSampleAdec[ADEC_MAX_CHN_NUM];
static SAMPLE_AO_S	gs_stSampleAo[I2S_DEV_TOTAL*MAX_AUDIO_CHN_NUM];
static AUDIO_FIFO_M	g_audio_fifo;
static FY_BOOL 		g_bFifOInit=FY_FALSE;
static pthread_mutex_t 	g_fifo_mutex=PTHREAD_MUTEX_INITIALIZER;
static FY_U32		g_u32AudioType = AUDIO_CODEC_NAU88C10;
static FY_S32 		g_comm_ctrl_flag;
FY_U32			g_saveFile=0;

FY_VOID SAMPLE_COMM_AUDIO_Tp28xx_init()
{
	system("cd /sys/kernel/debug/19700000.i2s/\necho r > ctrl\n");
}

void SAMPLE_COMM_AUDIO_SelectType()
{
	FY_BOOL bExit = FY_FALSE;
	FY_CHAR option,end_flag;

	while(1)
	{
		printf("\n\n=============================================================\n\n");
		printf("Option[1 ~ 2]:\n\n");
		printf("    1:    audio sample from nau88c10\n");
		printf("    2:    audio sample from acwrapper\n");
		printf("==============================================================\n\n");
		printf("\033[1;31;44m Enter your choice [1/2]: \033[0m");
		option = getchar();
		if(10 == option)
		{
			continue;
		}
		end_flag = getchar();
		if(10 != end_flag)
		{
			while((end_flag = getchar()) != '\n' && end_flag != EOF);
			{
				;
			}
			continue;
		}
		switch(option)
		{

			case '1':
				g_u32AudioType = AUDIO_CODEC_NAU88C10;
				bExit=FY_TRUE;
				break;

			case '2':
				g_u32AudioType = AUDIO_CODEC_ACWRAPPER;
				bExit=FY_TRUE;
				break;

			case 'q':
				bExit=FY_TRUE;
				break;

			default:
				bExit=FY_FALSE;
			break;
		}
		if(bExit)
		{
			break;
		}
	}
}

FY_S32 SAMPLE_COMM_AUDIO_Get_AcwType()
{
	return g_u32AudioType;
}

FY_S32 SAMPLE_COMM_AUDIO_Get_channel()
{
	FY_S32 chn=0;
	FY_BOOL bExit = FY_FALSE;
	FY_CHAR option,end_flag;

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
		end_flag = getchar();
		if(10 != end_flag)
		{
			while((end_flag = getchar()) != '\n' && end_flag != EOF);
			{
				;
			}
			continue;
		}
		switch(option)
		{
			case '4':
				chn++;
			case '3':
				chn++;
			case '2':
				chn++;
			case '1':
				chn++;
				printf("OK,Chn Option is %d\n",chn);
				bExit=FY_TRUE;
				break;
			case 'q':
				goto exit;
			default:
				bExit=FY_FALSE;
			break;
		}
		if(bExit)
		{
			chn--;
			break;
		}
	}
exit:
	return chn;
}


/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
FY_U32 SAMPLE_COMM_Get_AdecLen(FY_U32 type,FY_U32 frmLen)
{
	FY_U32 len=0;
	switch(type)
	{
		case PT_G711A:
			len=frmLen/2;
		break;

		case PT_G711U:
			len=frmLen/2;
		break;

		case PT_G726:
			len=frmLen/8;
		break;

		case PT_G726_32K:
			len=frmLen/4;
		break;

		case PT_AAC:
			len=170;//frmLen/16;
		break;

		case PT_LPCM:
			len=frmLen;
		break;

		case PT_ADPCMA:
			len=frmLen/4;
		break;

		default:
		break;
	}
	return len;
}

FY_VOID SAMPLE_COMM_AUDIO_fifo_init()
{
	if(!g_bFifOInit)
	{
		pthread_mutex_init(&g_fifo_mutex,FY_NULL);
		g_bFifOInit=FY_TRUE;
	}

	pthread_mutex_lock(&g_fifo_mutex);
	memset(&g_audio_fifo,0,sizeof(AUDIO_FIFO_M));
	pthread_mutex_unlock(&g_fifo_mutex);
}

FY_U32 SAMPLE_COMM_AUDIO_fifo_push(AUDIO_FRAME_S* p)
{
	FY_U32 ret=FY_SUCCESS;

	if(!g_bFifOInit || !p)
	{
		return ret;
	}
	pthread_mutex_lock(&g_fifo_mutex);
	g_audio_fifo.pAddr=p->pVirAddr;
	g_audio_fifo.uSize=p->u32Len;
	g_audio_fifo.u32phyaddr=p->u32PhyAddr;
	if(!g_audio_fifo.bWriteFlg)
	{
		g_audio_fifo.bWriteFlg=FY_TRUE;
	}
	pthread_mutex_unlock(&g_fifo_mutex);
	return ret;
}

FY_VOID SAMPLE_COMM_AUDIO_fifo_poll(AUDIO_FRAME_S* p)
{
	if(g_bFifOInit)
	{
		pthread_mutex_lock(&g_fifo_mutex);
		if(g_audio_fifo.bWriteFlg && p)
		{
			p->pVirAddr=(FY_CHAR*)g_audio_fifo.pAddr;
			p->u32Len=g_audio_fifo.uSize;
			p->u32PhyAddr=g_audio_fifo.u32phyaddr;
			memset(&g_audio_fifo,0,sizeof(AUDIO_FIFO_M));
		}
		else
		{
			p->pVirAddr=FY_NULL;
			p->u32Len=0;
			p->u32PhyAddr=0;
		}
		pthread_mutex_unlock(&g_fifo_mutex);
	}
	else
	{
		p->pVirAddr=FY_NULL;
		p->u32Len=0;
		p->u32PhyAddr=0;
	}
}

FY_VOID SAMPLE_COMM_AUDIO_fifo_deinit()
{
	if(g_bFifOInit)
	{
		g_bFifOInit=FY_FALSE;
		pthread_mutex_destroy(&g_fifo_mutex);
	}
}

void SAMPLE_COMM_AUDIO_EndianSwap(FY_U8 *pData, FY_U32 startIndex, FY_U32 length)
{
    FY_U32 i,cnt,end,start;
	FY_U8 tmp;

    cnt = length / 2;
    start = startIndex;
    end  = startIndex + length - 1;
    for (i = 0; i < cnt; i++)
    {
        tmp            = pData[start+i];
        pData[start+i] = pData[end-i];
        pData[end-i]   = tmp;
    }
}

void SAMPLE_COMM_AUDIO_EndianSwap_bits(FY_U8 *pData, FY_U32 startIndex, FY_U32 length)
{
	FY_U32 i,cnt,start;
	FY_U16 tmp;
	FY_U32 j;
	FY_U16 out=0;

	cnt = length / 2;
	start = startIndex;
	for (i = 0; i < cnt; i++)
	{
		tmp = (FY_U16)pData[start+i];
		for (j = 0; j < 8; j++) {
			out |= (tmp & (0x0001 << j)) << (15 - 2 *j);//H
			out |= (tmp & (0x0100 << j)) >> (2 * j + 1);//L
		}
		memcpy(&pData[start+i],&out,sizeof(FY_U16));
	}
}


void *SAMPLE_COMM_AUDIO_AiProc(void *parg)
{
	FY_S32 s32Ret;
	SAMPLE_AI_S *pstAiCtl = (SAMPLE_AI_S *)parg;
	AUDIO_FRAME_S stFrame;
	AUDIO_STREAM_S stream;
	static int cnt=1000;

	while (pstAiCtl->bStart)
	{
		/* get frame from ai chn */
		s32Ret = FY_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL, 1000);
		if (FY_SUCCESS != s32Ret )
		{
			//printf("%s: FY_MPI_AI_GetFrame(%d, %d), failed with %#x!\n",__FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
			usleep(10*1000);
			continue;
		}

		if (FY_TRUE == pstAiCtl->bSaveOriFile)
		{
			//printf("[FILE]addr=%p,u32Seq=%d,len=0x%x,data=%x-%x\n",stFrame.pVirAddr,stFrame.u32Seq,stFrame.u32Len,((char*)stFrame.pVirAddr)[0],((char*)stFrame.pVirAddr)[1]);
			/* save audio stream to file */
			s32Ret = fwrite(stFrame.pVirAddr, 1, stFrame.u32Len, pstAiCtl->pfd[FILE_TYPE_AI]);
		}

		/* send frame to encoder */
		if (FY_TRUE == pstAiCtl->bSendAenc)
		{
			s32Ret = FY_MPI_AENC_SendFrame(pstAiCtl->AencChn,&stFrame,FY_FALSE);
			if(s32Ret != FY_SUCCESS)
			{
				continue;
			}
			if (FY_TRUE == pstAiCtl->bSaveAencFile)
			{
				FY_MPI_AENC_GetStream(pstAiCtl->AencChn,&stream,FY_FALSE);
				/* save audio stream to file */
				s32Ret = fwrite(stream.pStream, 1, stream.u32Len, pstAiCtl->pfd[FILE_TYPE_AENC]);
			}
		}

		if(pstAiCtl->bSendToFifo)
		{
			SAMPLE_COMM_AUDIO_fifo_push(&stFrame);
		}

		/* send frame to ao */
		if (FY_TRUE == pstAiCtl->bSendAo && cnt)
		{
			s32Ret = FY_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 0);
			if (FY_SUCCESS != s32Ret )
			{
				printf("%s: FY_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",\
				__FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
				usleep(10*1000);
				continue;
			}
			//cnt--;
		}

		if(pstAiCtl->bSendAdec)
		{
			stream.pStream = stFrame.pVirAddr;
			stream.u32Len = stFrame.u32Len;
			FY_MPI_ADEC_SendStream(pstAiCtl->AdecChn, &stream, FY_TRUE);
		}

		/* finally you must release the stream */
		s32Ret = FY_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL);
		if (FY_SUCCESS != s32Ret )
		{
			printf("%s: FY_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n",\
			__FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
			usleep(10*1000);
			continue;
		}
	}
	pstAiCtl->bStart = FY_FALSE;
	return NULL;
}

void *SAMPLE_COMM_AUDIO_AiAllProc(void *parg)
{
	FY_S32 s32Ret;
	FY_U32 i;
	SAMPLE_AI_S *pstAiCtl = (SAMPLE_AI_S *)parg;
	AUDIO_FRAME_S stFrame;
	FY_CHAR buff[1024]="\0";
	FY_U32 dataLen=0;

	while (pstAiCtl->bStart)
	{
		for(i=0;i<pstAiCtl->AiChn;i++)
		{
			/* get frame from ai chn */
			s32Ret = FY_MPI_AI_GetFrame(pstAiCtl->AiDev, i, &stFrame, NULL, 1000);
			if (FY_SUCCESS != s32Ret )
			{
				continue;
			}

			if (FY_TRUE == pstAiCtl->bSaveOriFile)
			{
				if(stFrame.pVirAddr)
				{
					memcpy(buff,stFrame.pVirAddr,stFrame.u32Len);
				}
				#if 0
				printf("[FILE%d]addr=%p,u32Seq=%d,len=0x%x,pts=%lld,data=%x-%x\n",
					i,stFrame.pVirAddr,stFrame.u32Seq,stFrame.u32Len,stFrame.u64TimeStamp,
					((char*)stFrame.pVirAddr)[0],((char*)stFrame.pVirAddr)[1]);
				#endif
				if(i==0)
				{
					static FY_U32 cnt=0;

					if(cnt >= 250)
					{
						//printf("[pts] %lld,data %d\n",stFrame.u64TimeStamp,dataLen);
						dataLen=0;
						cnt = 0;
					}
					cnt++;
					dataLen += stFrame.u32Len;
					s32Ret = FY_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 0);
					if (FY_SUCCESS != s32Ret )
					{
						printf("%s: FY_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",\
						__FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
						continue;
					}
				}
				/* save audio stream to file */
				//s32Ret = fwrite(stFrame.pVirAddr, 1, stFrame.u32Len, pstAiCtl->pfd[FILE_TYPE_AI]);
			}

			/* finally you must release the stream */
			s32Ret = FY_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, i, &stFrame, NULL);
			if (FY_SUCCESS != s32Ret )
			{
				printf("%s: FY_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n",\
				__FUNCTION__, pstAiCtl->AiDev, i, s32Ret);
			}
		}
		//usleep(38000);
	}
	pstAiCtl->bStart = FY_FALSE;
	return NULL;
}

/******************************************************************************
* function : get stream from Aenc, send it  to Adec & save it to file
******************************************************************************/
void *SAMPLE_COMM_AUDIO_AencProc(void *parg)
{
	FY_S32 s32Ret;
	SAMPLE_AENC_S *pstAencCtl = (SAMPLE_AENC_S *)parg;
	AUDIO_STREAM_S stStream;

	while (pstAencCtl->bStart)
	{
		/* get stream from aenc chn */
		s32Ret = FY_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, FY_TRUE);
		if(s32Ret != FY_SUCCESS)
		{
			usleep(1*1000);
			printf("[SAMP]aenc get stream err,one more again!\n");
			continue;
		}
		/* save audio stream to file */
		if(pstAencCtl->bSaveFile)
		{
			fwrite(stStream.pStream, 1, stStream.u32Len, pstAencCtl->pfd);

			/* finally you must release the stream */
		}

		if (FY_TRUE == pstAencCtl->bSendAdChn)
		{
			s32Ret = FY_MPI_ADEC_SendStream(pstAencCtl->AdChn, &stStream, FY_TRUE);
			if (FY_SUCCESS != s32Ret )
			{
				printf("%s: FY_MPI_ADEC_SendStream(%d), failed with %#x!\n",\
				__FUNCTION__, pstAencCtl->AdChn, s32Ret);
				pstAencCtl->bStart = FY_FALSE;
				return NULL;
			}
		}

	}

	if(pstAencCtl->bSaveFile)
		fclose(pstAencCtl->pfd);
	pstAencCtl->bStart = FY_FALSE;
	return NULL;
}

#define TEST_CNT 10
#define TEST_CLR_CNT 10
//#define DEBUG1	1
/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void *SAMPLE_COMM_AUDIO_AdecProc(void *parg)
{
	FY_S32 s32Ret;
	AUDIO_FRAME_S stFrame;
	FY_U32 u32Len = 160;
	FY_U32 u32ReadLen;
	FY_U8 *pu8AudioStream = NULL;
	SAMPLE_ADEC_S *pstAdecCtl = (SAMPLE_ADEC_S *)parg;
	FILE *pfd = pstAdecCtl->pfd;
	AUDIO_STREAM_S adecStream;
	static int test_cnt=0;
	#ifdef DEBUG1
	int sleeptime=0;
	struct timespec requested_time;
	#endif

	u32Len=SAMPLE_COMM_Get_AdecLen(pstAdecCtl->enType,pstAdecCtl->uRdFrmLen);
	pu8AudioStream = (FY_U8*)malloc(sizeof(FY_U8)*u32Len);
	printf("u32Len %d\n",u32Len);
	if (NULL == pu8AudioStream)
	{
		printf("%s: malloc failed!\n", __FUNCTION__);
		return NULL;
	}
	memset(pu8AudioStream,0,u32Len);
	adecStream.u32Seq=0;
	prctl(PR_SET_NAME,"ADEC-AO");
	while (FY_TRUE == pstAdecCtl->bStart)
	{
		/* read from file */
		u32ReadLen = fread(pu8AudioStream, 1, u32Len, pfd);
		if (u32ReadLen <= 0)
		{
			fseek(pfd, 0, SEEK_SET);/*read file again*/
			//printf("\033[32m[read-end]u32Len %d\033[0m\n",u32ReadLen);
			continue;
		}

		/* if ao not bind to adec, should do following steps */
		if(FY_TRUE == pstAdecCtl->bManual)
		{
			adecStream.pStream=pu8AudioStream;
			adecStream.u32Len=u32ReadLen;
			adecStream.u32Seq++;
			s32Ret = FY_MPI_ADEC_SendStream(pstAdecCtl->AdChn,&adecStream,FY_FALSE);
			s32Ret = FY_MPI_ADEC_GetFrame(pstAdecCtl->AdChn,&stFrame, FY_FALSE);
			//printf("[%s] s32Ret %x\n",__func__,s32Ret);
			//printf("seq %d-%d\n",adecStream.u32Seq,stFrame.u32Seq);
			if(s32Ret == FY_SUCCESS)//(test_cnt < TEST_CLR_CNT)
			{
				s32Ret = FY_MPI_AO_SendFrame(pstAdecCtl->AoDev, pstAdecCtl->AoChn, &stFrame, 0);
				if(FY_SUCCESS != s32Ret)
				{
					printf("%s: FY_MPI_AO_SendFrame(%d, %d) failed with %#x!\n",\
					__FUNCTION__, pstAdecCtl->AoDev, pstAdecCtl->AoChn, s32Ret);
					break;
				}
			}
			else
			{
				//if(test_cnt == TEST_CNT)
					//FY_MPI_AO_ClearChnBuf(pstAdecCtl->AoDev,pstAdecCtl->AoChn);
			//	usleep(40000);
			}
			test_cnt++;
		}
		else
		{
			static FY_U64 last_time=0;
			FY_U64 cur_time;

			FY_MPI_SYS_GetCurPts(&cur_time);
			if(last_time)
			{
				//printf("[ADEC]time intval=%lld\n",cur_time-last_time);
			}
			last_time=cur_time;

			adecStream.pStream=pu8AudioStream;
			adecStream.u32Len=u32ReadLen;
			#ifdef DEBUG1
			s32Ret = FY_MPI_ADEC_SendStream(pstAdecCtl->AdChn,&adecStream,FY_FALSE);
			#else
			s32Ret = FY_MPI_ADEC_SendStream(pstAdecCtl->AdChn,&adecStream,FY_TRUE);
			#endif
			if(FY_SUCCESS != s32Ret)
			{
				#if DEBUG1
				printf("%s: FY_MPI_ADEC_SendStream(%d, %d) failed with %#x!\n",\
				__FUNCTION__, pstAdecCtl->AoDev, pstAdecCtl->AoChn, s32Ret);
				#endif
				continue;
			}
			#ifdef DEBUG1
			if(test_cnt%TEST_CNT==0)
			{
				sleeptime=35;
			}
			else if(test_cnt%TEST_CNT==1)
			{
				sleeptime=24;
			}
			else if(test_cnt%TEST_CNT==2)
			{
				sleeptime=50;
			}
			else if(test_cnt%TEST_CNT==3)
			{
				sleeptime=35;
			}
			else if(test_cnt%TEST_CNT==4)
			{
				sleeptime=50;
			}
			else if(test_cnt%TEST_CNT==5)
			{
				sleeptime=39;
			}
			else if(test_cnt%TEST_CNT==6)
			{
				sleeptime=40;
			}
			else if(test_cnt%TEST_CNT==7)
			{
				sleeptime=39;
			}
			else if(test_cnt%TEST_CNT==8)
			{
				sleeptime=30;
			}
			else if(test_cnt%TEST_CNT==9)
			{
				sleeptime=27;
			}
			if(pstAdecCtl->enType == PT_AAC)
			{
				switch(SAMPLE_AUDIO_RATE)
				{
					case AUDIO_SAMPLE_RATE_8000:
						requested_time.tv_nsec = 125000000;
						requested_time.tv_sec = 0;
						break;

					case AUDIO_SAMPLE_RATE_16000:
						requested_time.tv_nsec = 58450000;
						requested_time.tv_sec = 0;
						break;

					default:
					break;
				}

				nanosleep(&requested_time, 0);
			}
			else
			{
				sleeptime=35;
				usleep(sleeptime*1000);
			}

			if(test_cnt == TEST_CNT)
			{
			//	FY_MPI_ADEC_DestroyChn(pstAdecCtl->AdChn);
				test_cnt=0;
			}
			if(sleeptime)
			{
				sleeptime=0;
			}
			test_cnt++;
			#endif
		}
	}

	free(pu8AudioStream);
	pu8AudioStream = NULL;
	fclose(pfd);
	pstAdecCtl->bStart = FY_FALSE;
	return NULL;
}

/******************************************************************************
* function : get stream from file, and send it  to Ao
******************************************************************************/
void SAMPLE_COMM_AUDIO_SetBuff(FY_S32 status)
{
	g_comm_ctrl_flag = status;
}

void *SAMPLE_COMM_AUDIO_AoProc(void *parg)
{
	FY_S32 s32Ret;
	FY_U32 u32ReadLen;
	FY_U32 toReadLen=4,i;
	SAMPLE_AO_S *pstAoCtl = (SAMPLE_AO_S *)parg;
	FILE *pfd = pstAoCtl->pfd;
	FY_CHAR* pAtr=FY_NULL;
	static FY_U64 last_time=0;
	FY_U64 cur_time,end_time;
	static FY_U32 cnt=0;

	printf("pstAoCtl->bRdFromFifo %d len %d -- %d\n",pstAoCtl->bRdFromFifo,pstAoCtl->uRdFrmLen,pstAoCtl->frmInfo.u32Len);
	if(!pstAoCtl->bRdFromFifo)
	{
		pstAoCtl->frmInfo.u32Len = pstAoCtl->uRdFrmLen;
		s32Ret = FY_MPI_SYS_MmzAlloc(&pstAoCtl->frmInfo.u32PhyAddr,&pstAoCtl->frmInfo.pVirAddr, "file-ao", FY_NULL, pstAoCtl->frmInfo.u32Len);
		if (s32Ret != FY_SUCCESS)
		{
			printf("%s: malloc failed!\n", __FUNCTION__);
			return NULL;
		}
		memset(pstAoCtl->frmInfo.pVirAddr,0,pstAoCtl->frmInfo.u32Len);
		pstAoCtl->bAllocMem=FY_TRUE;
	}
	else
	{
		pstAoCtl->bAllocMem=FY_FALSE;
	}

	while (FY_TRUE == pstAoCtl->bStart)
	{
		/* read from file */
		if(IS_ACW_ID(pstAoCtl->AoDev))
		{
			//toReadLen /= 2;
		}

		if(pstAoCtl->bRdFromFifo)
		{
			SAMPLE_COMM_AUDIO_fifo_poll(&pstAoCtl->frmInfo);
			if(pstAoCtl->frmInfo.pVirAddr == FY_NULL && pstAoCtl->frmInfo.u32Len == 0)
			{
				usleep(5*1000);
				continue;
			}
		}
		else
		{
			for(i=0;i<pstAoCtl->frmInfo.u32Len;i += 4)
			{
				u32ReadLen = fread(pstAoCtl->frmInfo.pVirAddr+i, 1, toReadLen, pfd);
				if (u32ReadLen <= 0)
				{
					fseek(pfd, 0, SEEK_SET);/*read file again*/
					continue;
				}
			}
		}
		pAtr = (FY_CHAR*)pstAoCtl->frmInfo.pVirAddr;

		if(!g_comm_ctrl_flag)
		{
			static FILE* pWr=FY_NULL;

			if(g_saveFile)
			{
				if(!pWr)
				{
					pWr = fopen("ai-ao.pcm", "w+");
					printf("open file ##################\n");
				}
				if(pWr)
					fwrite(pstAoCtl->frmInfo.pVirAddr,1,pstAoCtl->frmInfo.u32Len,pWr);
			}
			else
			{
				if(pWr)
				{
					fclose(pWr);
					pWr = FY_NULL;
				}
			}
			s32Ret = FY_MPI_AO_SendFrame(pstAoCtl->AoDev, pstAoCtl->AoChn, &pstAoCtl->frmInfo, 0);
			if(FY_SUCCESS != s32Ret)
			{
				#if 0
				printf("%s: FY_MPI_AO_SendFrame(%d, %d) failed with %#x!\n",\
				__FUNCTION__, pstAoCtl->AoDev, pstAoCtl->AoChn, s32Ret);
				break;
				#endif
			}
		}
		else
		{
			usleep(40*1000);
		}

		FY_MPI_SYS_GetCurPts(&cur_time);
		if(last_time && pAtr)
		{
			end_time = cur_time-last_time;
			if(end_time > 40000)
			{
				//printf("[time:%lld]pAtr=%p,data:0x%02x-%02x-%02x-%02x\n",end_time,pAtr,pAtr[0],pAtr[1],pAtr[2],pAtr[3]);
				//printf("[time:%lld]cnt=%d,s32Ret=%d\n",end_time,cnt,s32Ret);
				cnt=0;
			}
		}
		last_time=cur_time;
		cnt++;
	}

	if(!pstAoCtl->bRdFromFifo)
	{
		if(pstAoCtl->frmInfo.pVirAddr != FY_NULL && pstAoCtl->bAllocMem)
		{
			FY_MPI_SYS_MmzFree(pstAoCtl->frmInfo.u32PhyAddr,pstAoCtl->frmInfo.pVirAddr);
			pstAoCtl->frmInfo.pVirAddr = NULL;
			pstAoCtl->bAllocMem=FY_FALSE;
		}
	}

	pstAoCtl->bStart = FY_FALSE;
	return NULL;
}


/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn, PAYLOAD_TYPE_E enType, FY_VOID *pAiFd)
{
	SAMPLE_AI_S *pstAi = NULL;
	int i;
	FY_U32* pFd=(FY_U32*)pAiFd;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];
	pstAi->bSendAenc = FY_FALSE;
	pstAi->bSendAo = FY_TRUE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;
	pstAi->enType = enType;
	if(pFd)
	{
		for(i=0;i<FILE_TYPE_TOTAL;i++)
		{
			pstAi->pfd[i] = (FILE*)*(pFd+i);
			if(pstAi->pfd[FILE_TYPE_AI])
			{
				pstAi->bSaveOriFile = FY_TRUE;
			}
			if(pstAi->pfd[FILE_TYPE_AENC])
			{
				pstAi->bSaveAencFile = FY_TRUE;
			}
		}
	}
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and save  data to file
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn, FY_VOID *pAiFd)
{
	SAMPLE_AI_S *pstAi = NULL;
	int i;
	FY_U32* pFd=(FY_U32*)pAiFd;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];
	pstAi->bSendAenc = FY_FALSE;
	pstAi->bSendAo = FY_FALSE;
	pstAi->bSaveOriFile = FY_TRUE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	if(pFd)
	{
		for(i=0;i<FILE_TYPE_TOTAL;i++)
		{
			pstAi->pfd[i] = (FILE*)*(pFd+i);
		}
	}
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and save  data to file
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAll(AUDIO_DEV AiDev, AI_CHN MaxChn,AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S *pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM];
	pstAi->bSendAenc = FY_FALSE;
	pstAi->bSendAo = FY_FALSE;
	pstAi->bSaveOriFile = FY_TRUE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = MaxChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiAllProc, pstAi);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send  data to fifo
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiToFifo(AUDIO_DEV AiDev, AI_CHN AiChn, FY_VOID *pAiFd)
{
	SAMPLE_AI_S *pstAi = NULL;
	int i;
	FY_U32* pFd=(FY_U32*)pAiFd;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];
	pstAi->bSendAenc = FY_FALSE;
	pstAi->bSendAo = FY_FALSE;
	pstAi->bSaveOriFile = FY_FALSE;
	pstAi->bSendToFifo = FY_TRUE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	if(pFd)
	{
		for(i=0;i<FILE_TYPE_TOTAL;i++)
		{
			pstAi->pfd[i] = (FILE*)*(pFd+i);
		}
	}
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to aenc,
*            and save encoded data to file if need
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn, PAYLOAD_TYPE_E enType, FY_VOID *pAecFd)
{
	SAMPLE_AI_S *pstAi = NULL;
	int i;
	FY_U32* pFd=(FY_U32*)pAecFd;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];
	pstAi->bSendAenc = FY_TRUE;
	pstAi->bSaveAencFile = FY_TRUE;
	pstAi->bSaveOriFile = FY_TRUE;
	pstAi->bSendAo = FY_FALSE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AencChn = AeChn;
	if(pFd)
	{
		for(i=0;i<FILE_TYPE_TOTAL;i++)
		{
			pstAi->pfd[i] = (FILE*)*(pFd+i);
		}
	}
	pstAi->enType = enType;
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAdec(AUDIO_DEV AiDev, AI_CHN AiChn, ADEC_CHN AeChn, PAYLOAD_TYPE_E enType, FY_VOID *pAecFd)
{
	SAMPLE_AI_S *pstAi = NULL;
	int i;
	FY_U32* pFd=(FY_U32*)pAecFd;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];
	pstAi->bSendAdec = FY_TRUE;
	pstAi->bSaveOriFile = FY_FALSE;
	pstAi->bSendAo = FY_FALSE;
	pstAi->bStart= FY_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AdecChn = AeChn;
	if(pFd)
	{
		for(i=0;i<FILE_TYPE_TOTAL;i++)
		{
			pstAi->pfd[i] = (FILE*)*(pFd+i);
		}
	}
	pstAi->enType = enType;
	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return FY_SUCCESS;
}


/******************************************************************************
* function : Create the thread to get stream from aenc and save encoded data
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAencFile(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd)
{
	SAMPLE_AENC_S *pstAenc = NULL;

	if (NULL == pAecFd)
	{
		return FY_FAILURE;
	}

	pstAenc = &gs_stSampleAenc[AeChn];
	pstAenc->AeChn = AeChn;
	pstAenc->AdChn = AdChn;
	pstAenc->bSendAdChn = FY_FALSE;
	pstAenc->bSaveFile = FY_TRUE;
	pstAenc->pfd = pAecFd;
	pstAenc->bStart = FY_TRUE;
	pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType, FY_U32 readLen, FILE *pAdcFd)
{
	SAMPLE_ADEC_S *pstAdec = NULL;

	if (NULL == pAdcFd)
	{
		return FY_FAILURE;
	}

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->AdChn = AdChn;
	pstAdec->pfd = pAdcFd;
	pstAdec->uRdFrmLen = readLen;
	pstAdec->enType = enType;
	pstAdec->bStart = FY_TRUE;
	pstAdec->bManual = FY_FALSE;
	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAO(AUDIO_DEV AoDev, AO_CHN AoChn, FY_U32 readLen, FILE *pAoFd)
{
	SAMPLE_AO_S *pstAo = NULL;

	if (NULL == pAoFd)
	{
		return FY_FAILURE;
	}

	pstAo = &gs_stSampleAo[AoDev*MAX_AUDIO_CHN_NUM + AoChn];
	pstAo->pfd = pAoFd;
	pstAo->AoDev = AoDev;
	pstAo->AoChn = AoChn;
	pstAo->bStart = FY_TRUE;
	pstAo->uRdFrmLen = readLen;
	pstAo->bRdFromFifo = FY_FALSE;
	pthread_create(&pstAo->stAoPid, 0, SAMPLE_COMM_AUDIO_AoProc, pstAo);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from fifo and send to ao
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdFifoToAO(AUDIO_DEV AoDev, AO_CHN AoChn, FILE *pAoFd)
{
	SAMPLE_AO_S *pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev*MAX_AUDIO_CHN_NUM + AoChn];
	pstAo->pfd = pAoFd;
	pstAo->AoDev = AoDev;
	pstAo->AoChn = AoChn;
	pstAo->bStart = FY_TRUE;
	pstAo->uRdFrmLen = 0;
	pstAo->bRdFromFifo = FY_TRUE;
	pthread_create(&pstAo->stAoPid, 0, SAMPLE_COMM_AUDIO_AoProc, pstAo);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_CreatTrdAdecAO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn, PAYLOAD_TYPE_E enType, FY_U32 readLen, FILE *pAdcFd)
{
	SAMPLE_ADEC_S *pstAdec = NULL;

	if (NULL == pAdcFd)
	{
		return FY_FAILURE;
	}

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->AdChn = AdChn;
	pstAdec->pfd = pAdcFd;
	pstAdec->AoDev = AoDev;
	pstAdec->AoChn = AoChn;
	pstAdec->enType = enType;
	pstAdec->bStart = FY_TRUE;
	pstAdec->bManual = FY_TRUE;
	pstAdec->uRdFrmLen = readLen;
	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get frame from ai and send to ao or aenc
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn)
{
	SAMPLE_AI_S *pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM + AiChn];

	if (pstAi->bStart)
	{
		pstAi->bStart = FY_FALSE;
	//	pthread_cancel(pstAi->stAiPid);
		if(pstAi->bSendToFifo)
		{
			pstAi->bSendToFifo = FY_FALSE;
		}
		pthread_join(pstAi->stAiPid, 0);
	}

	return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAiAll(AUDIO_DEV AiDev)
{
	SAMPLE_AI_S *pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev*MAX_AUDIO_CHN_NUM];

	if (pstAi->bStart)
	{
		pstAi->bStart = FY_FALSE;
		pthread_cancel(pstAi->stAiPid);
		pthread_join(pstAi->stAiPid, 0);
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to ao
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAo(AUDIO_DEV AoDev, AI_CHN AoChn)
{
	SAMPLE_AO_S *pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev*MAX_AUDIO_CHN_NUM + AoChn];
	if (pstAo->bStart)
	{
		pstAo->bStart = FY_FALSE;
		//pthread_cancel(pstAo->stAoPid);
		if(pstAo->bRdFromFifo)
		{
			pstAo->bRdFromFifo = FY_FALSE;
		}
		pthread_join(pstAo->stAoPid, 0);
		if(!pstAo->bRdFromFifo)
		{
			if(pstAo->frmInfo.pVirAddr != FY_NULL && pstAo->bAllocMem)
			{
				FY_MPI_SYS_MmzFree(pstAo->frmInfo.u32PhyAddr,pstAo->frmInfo.pVirAddr);
				pstAo->frmInfo.pVirAddr = NULL;
				pstAo->bAllocMem=FY_FALSE;
			}
		}
	}
	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from aenc and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencFile(AENC_CHN AeChn)
{
	SAMPLE_AENC_S *pstAenc = NULL;

	pstAenc = &gs_stSampleAenc[AeChn];

	if (pstAenc->bStart)
	{
		pstAenc->bStart = FY_FALSE;
		pthread_join(pstAenc->stAencPid, 0);
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from aenc and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn)
{
	SAMPLE_AENC_S *pstAenc = NULL;

	pstAenc = &gs_stSampleAenc[AeChn];

	if (pstAenc->bStart)
	{
		pstAenc->bStart = FY_FALSE;
		pthread_join(pstAenc->stAencPid, 0);
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from file and send to adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn)
{
	SAMPLE_ADEC_S *pstAdec = NULL;

	pstAdec = &gs_stSampleAdec[AdChn];

	if (pstAdec->bStart)
	{
		pstAdec->bStart = FY_FALSE;
		pthread_join(pstAdec->stAdPid, 0);
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Destory the all thread
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd()
{
	FY_U32 u32DevId, u32ChnId;

	for (u32DevId = 0; u32DevId < I2S_DEV_TOTAL; u32DevId ++)
	{
		for (u32ChnId = 0; u32ChnId < MAX_AUDIO_CHN_NUM; u32ChnId ++)
		{
			SAMPLE_COMM_AUDIO_DestoryTrdAi(u32DevId, u32ChnId);
			SAMPLE_COMM_AUDIO_DestoryTrdAo(u32DevId, u32ChnId);
		}
	}

	for (u32ChnId = 0; u32ChnId < AENC_MAX_CHN_NUM; u32ChnId ++)
	{
		SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(u32ChnId);
	}

	for (u32ChnId = 0; u32ChnId < ADEC_MAX_CHN_NUM; u32ChnId ++)
	{
		SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(u32ChnId);
	}

	return FY_SUCCESS;
}


/******************************************************************************
* function : Ao bind Adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_ADEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = AdChn;
	stDestChn.enModId = FY_ID_AO;
	stDestChn.s32DevId = AoDev;
	stDestChn.s32ChnId = AoChn;

	return FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_ADEC;
	stSrcChn.s32ChnId = AdChn;
	stSrcChn.s32DevId = 0;
	stDestChn.enModId = FY_ID_AO;
	stDestChn.s32DevId = AoDev;
	stDestChn.s32ChnId = AoChn;

	return FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao bind Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_AI;
	stSrcChn.s32ChnId = AiChn;
	stSrcChn.s32DevId = AiDev;
	stDestChn.enModId = FY_ID_AO;
	stDestChn.s32DevId = AoDev;
	stDestChn.s32ChnId = AoChn;

	return FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_AI;
	stSrcChn.s32ChnId = AiChn;
	stSrcChn.s32DevId = AiDev;
	stDestChn.enModId = FY_ID_AO;
	stDestChn.s32DevId = AoDev;
	stDestChn.s32ChnId = AoChn;

	return FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Aenc bind Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;
	stDestChn.enModId = FY_ID_AENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AeChn;

	return FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Aenc unbind Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
	MPP_CHN_S stSrcChn,stDestChn;

	stSrcChn.enModId = FY_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;
	stDestChn.enModId = FY_ID_AENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AeChn;

	return FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Start Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, FY_S32 s32AiChn, AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm)
{
	FY_S32 s32Ret;

	s32Ret = FY_MPI_AI_SetPubAttr(AiDevId,IOC_AICMD_BUTT, pstAioAttr);
	if (s32Ret)
	{
		printf("%s: FY_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	s32Ret = FY_MPI_AI_SetChnParam(AiDevId,s32AiChn, pFrm);
	s32Ret = FY_MPI_AI_DisableChn(AiDevId, s32AiChn);
	if (s32Ret)
	{
		printf("%s: FY_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, s32AiChn, s32Ret);
		return s32Ret;
	}
	s32Ret = FY_MPI_AI_Disable(AiDevId);
	s32Ret = FY_MPI_AI_Enable(AiDevId);
	s32Ret = FY_MPI_AI_EnableChn(AiDevId, s32AiChn);
	if (s32Ret)
	{
		printf("%s: FY_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	return FY_SUCCESS;
}


/******************************************************************************
* function : Stop Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, FY_S32 s32AiChn)
{
	FY_S32 s32Ret;

	s32Ret = FY_MPI_AI_DisableChn(AiDevId, s32AiChn);
	if (FY_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	s32Ret = FY_MPI_AI_Disable(AiDevId);
	if (FY_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Start Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StartAiAll(AUDIO_DEV AiDevId, FY_S32 MaxChn, AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm)
{
	FY_S32 s32Ret;
	FY_S32 s32AiChn;

	s32Ret = FY_MPI_AI_SetPubAttr(AiDevId,IOC_AICMD_BUTT, pstAioAttr);
	if (s32Ret)
	{
		printf("%s: FY_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for(s32AiChn=0;s32AiChn<MaxChn;s32AiChn++)
	{
		s32Ret = FY_MPI_AI_SetChnParam(AiDevId,s32AiChn, pFrm);
		s32Ret = FY_MPI_AI_DisableChn(AiDevId, s32AiChn);
		if (s32Ret)
		{
			printf("%s: FY_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, s32AiChn, s32Ret);
		}
	}
	s32Ret = FY_MPI_AI_Disable(AiDevId);
	s32Ret = FY_MPI_AI_Enable(AiDevId);

	for(s32AiChn=0;s32AiChn<MaxChn;s32AiChn++)
	{
		s32Ret = FY_MPI_AI_EnableChn(AiDevId, s32AiChn);
		if (s32Ret)
		{
			printf("%s: FY_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		}
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Stop Ai
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StopAiAll(AUDIO_DEV AiDevId, FY_S32 MaxChn)
{
	FY_S32 s32Ret;
	FY_S32 s32AiChn;

	for(s32AiChn=0;s32AiChn<MaxChn;s32AiChn++)
	{
		s32Ret = FY_MPI_AI_DisableChn(AiDevId, s32AiChn);
		if (FY_SUCCESS != s32Ret)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		}
	}

	s32Ret = FY_MPI_AI_Disable(AiDevId);
	if (FY_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Start Ao
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, FY_S32 s32AoChn,
                                AIO_ATTR_S* pstAioAttr, AUDIO_FRAME_E *pFrm, AUDIO_SAMPLE_RATE_E enInSampleRate, FY_BOOL bResampleEn)
{
	FY_S32 s32Ret;
	VO_HDMI_AUDIO_S resampleRate;
	FY_U32 flag;

	if (FY_TRUE == bResampleEn)
	{
		resampleRate.enSampleRate=enInSampleRate;
		FY_MPI_VO_SetHdmiAudio(0,&resampleRate);
	}

	s32Ret = FY_MPI_AO_SetPubAttr(AoDevId,IOC_AICMD_BUTT, pstAioAttr);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
		return FY_FAILURE;
	}

	s32Ret = FY_MPI_AO_SetChnParam(AoDevId,s32AoChn, pFrm);
	s32Ret = FY_MPI_AO_EnableChn(AoDevId, s32AoChn);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, s32AoChn, s32Ret);
		return FY_FAILURE;
	}

	s32Ret = FY_MPI_AO_Enable(AoDevId);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
		return FY_FAILURE;
	}

	flag = 75;
	//flag |= 1<<7;
	//flag |= 1<<8;
	printf("[%s]flag=%d -- %x\n",__func__,flag,flag);
	//FY_MPI_AO_SetHpfParam(AoDevId,s32AoChn,flag);

	return FY_SUCCESS;
}

/******************************************************************************
* function : Stop Ao
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, FY_S32 s32AoChn)
{
	FY_S32 s32Ret;

	s32Ret = FY_MPI_AO_DisableChn(AoDevId, s32AoChn);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
		return s32Ret;
	}

	s32Ret = FY_MPI_AO_Disable(AoDevId);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
		return s32Ret;
	}

	FY_MPI_AO_DisableReSmp(AoDevId, s32AoChn, 0);
	return FY_SUCCESS;
}


/******************************************************************************
* function : Start Aenc
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StartAenc(AENC_CHN AeChn, PAYLOAD_TYPE_E enType)
{
	FY_S32 s32Ret;
	AENC_CHN_ATTR_S adecAttr;

	switch (enType)
	{
		case PT_G711A:
		case PT_G711U:
		case PT_G726:
		case PT_G726_32K:
		case PT_AAC:
		case PT_LPCM:
		case PT_ADPCMA:
		break;

		default:
			printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, enType);
		return FY_FAILURE;
	}

	adecAttr.enType=enType;
	adecAttr.u32FrmCnt=1;
	adecAttr.u32FrmSize=SAMPLE_AUDIO_FRM_SIZE;
	adecAttr.fs_rate=SAMPLE_AUDIO_RATE;
	adecAttr.chn_width=AUDIO_BIT_WIDTH_16;
	adecAttr.track_mode=AUDIO_SOUND_MODE_MONO;

	/* create adec chn*/
	s32Ret = FY_MPI_AENC_CreateChn(AeChn,&adecAttr);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, AeChn,s32Ret);
		return s32Ret;
	}

	return 0;
}


/******************************************************************************
* function : Stop Aenc
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StopAenc(AENC_CHN AeChn, PAYLOAD_TYPE_E enType)
{
	FY_S32 s32Ret;

	s32Ret = FY_MPI_AENC_DestroyChn(AeChn);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AENC_DestroyChn(AdChn:%d) failed with %#x!\n", __FUNCTION__, AeChn, s32Ret);
		return s32Ret;
	}

	return FY_SUCCESS;
}

/******************************************************************************
* function : Start Adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	FY_S32 s32Ret;
	ADEC_CHN_ATTR_S adecAttr;
	FY_U32 uFrmSize = SAMPLE_AUDIO_FRM_SIZE;

	switch (enType)
	{
		case PT_G711A:
		case PT_G711U:
		case PT_G726:
		case PT_G726_32K:
		case PT_AAC:
		case PT_LPCM:
		case PT_ADPCMA:
		break;

		default:
			printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, enType);
		return FY_FAILURE;
	}

	//uFrmSize = 2048;
	adecAttr.enType=enType;
	adecAttr.u32FrmCnt=AI_CACHE_FRM_NUM;
	adecAttr.u32FrmSize=SAMPLE_COMM_Get_AdecLen(enType,uFrmSize);
	adecAttr.enMode=ADEC_MODE_PACK;
	adecAttr.fs_rate=SAMPLE_AUDIO_RATE;
	adecAttr.chn_width=AUDIO_BIT_WIDTH_16;

	FY_MPI_ADEC_SetPlySize(AdChn,SAMPLE_AUDIO_FRM_SIZE);
	/* create adec chn*/
	s32Ret = FY_MPI_ADEC_CreateChn(AdChn,&adecAttr);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_AENC_Init(%d) failed with %#x!\n", __FUNCTION__, AdChn,s32Ret);
		return s32Ret;
	}

	return 0;
}

/******************************************************************************
* function : Stop Adec
******************************************************************************/
FY_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	FY_S32 s32Ret;

	s32Ret = FY_MPI_ADEC_DestroyChn(AdChn);
	if (FY_SUCCESS != s32Ret)
	{
		printf("%s: FY_MPI_ADEC_DestroyChn(AdChn:%d) failed with %#x!\n", __FUNCTION__, AdChn, s32Ret);
		return s32Ret;
	}

	return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_AUDIO_StartHdmi()
{
	VO_HDMI_AUDIO_S hdmiAttr;

	FY_MPI_VO_GetHdmiAudio(0,&hdmiAttr);
	switch(hdmiAttr.enSampleRate)
	{
		case VO_SAMPLE_RATE_32K:
		case VO_SAMPLE_RATE_48K:
		{
			FY_MPI_VO_SetHdmiAudio(0,&hdmiAttr);
		}
		break;
		default:
		break;
	}
	printf("[AUDIO] current hdmi type %d\n",hdmiAttr.enSampleRate);
	return  FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_AUDIO_enable_vqe(FY_S32 id,FY_U32 anr_level,FY_U32 agc_level)
{
	AO_VQE_CONFIG_S vqe;
	AI_VQE_CONFIG_S vqe_ai;

	memset(&vqe,0,sizeof(vqe));
	memset(&vqe_ai,0,sizeof(vqe_ai));
	if(id == SAMPLE_AUDIO_AI_DEV)
	{
		vqe_ai.bAnrOpen = FY_TRUE;
		vqe_ai.s32WorkSampleRate = SAMPLE_AUDIO_RATE;
		vqe_ai.stAnrCfg.s16NrIntensity = anr_level;

		vqe_ai.bAgcOpen = FY_TRUE;
		vqe_ai.stAgcCfg.s8MaxGain = 127;
		vqe_ai.stAgcCfg.s8OutputMode = agc_level;
		vqe_ai.stAgcCfg.s8TargetLevel = 3;
		vqe_ai.stAgcCfg.s8AdjustSpeed = 30;

		vqe_ai.bAecOpen = FY_TRUE;
		vqe_ai.stAecCfg.s8CngMode = 1;
		vqe_ai.stAecCfg.s8NearAllPassEnergy = 59;
		vqe_ai.stAecCfg.s32Reserved = 0;
		FY_MPI_AI_SetVqeAttr(id,0,0,0,&vqe_ai);
		FY_MPI_AI_EnableVqe(id,0);
	}

	if(id == SAMPLE_AUDIO_AO_DEV || id == SAMPLE_AUDIO_HDMI_AO_DEV)
	{
		vqe.bAnrOpen = FY_FALSE;
		vqe.s32WorkSampleRate = SAMPLE_AUDIO_RATE;
		vqe.stAnrCfg.s16NrIntensity = anr_level;

		vqe.bAgcOpen = FY_TRUE;
		vqe.stAgcCfg.s8MaxGain = 127;
		vqe.stAgcCfg.s8OutputMode = agc_level;
		vqe.stAgcCfg.s8TargetLevel = 3;
		vqe.stAgcCfg.s8AdjustSpeed = 9;
		FY_MPI_AO_SetVqeAttr(id,0,&vqe);
		FY_MPI_AO_EnableVqe(id,0);
	}
	return FY_SUCCESS;
}
FY_S32 SAMPLE_COMM_AUDIO_set_volume(FY_S32 id)
{
	static int value=0;
	//static FY_BOOL mute=FY_FALSE;

	value += 30;
	if(value > 255)
	{
		value=0;
	}

	if(id == SAMPLE_AUDIO_AI_DEV)
	{
		FY_MPI_AI_SetVqeVolume(id,0,0,value);
	}
	else
	{
		FY_MPI_AO_SetVolume(id,0,value);
		//FY_MPI_AO_SetVolume(HdmiDevID,0,value);
	}
	//mute = mute?FY_FALSE:FY_TRUE;
	//FY_MPI_AO_SetMute(HdmiDevID, mute, FY_NULL);

	return FY_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

