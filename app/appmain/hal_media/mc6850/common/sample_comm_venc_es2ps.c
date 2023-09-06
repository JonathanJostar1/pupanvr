#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <sys/prctl.h>
#include "fy_common.h"
#include "sample_comm.h"

#define PS_HDR_LEN  20
#define SYS_HDR_LEN 18
#define PSM_HDR_LEN 0x54
#define PES_HDR_LEN 17

#define FILE_HEADER_SIZE  40
#define FILE_HEADER_STRMTYPE  0x0A

#define PS_VIDEO_STREAM_ID          0xE0
#define PSM_PIC_STREAMID_OFFSET     (0x2C+4)
#define PSM_PIC_WIDTH_OFFSET          (0x36+4)
#define PSM_PIC_HEIGHT_OFFSET          (0x38+4)
#define PSM_PIC_FRAMERATE_OFFSET    (0x41)

#define PS_PES_PAYLOAD_SIZE (64*1024 - 14)

#define VENC_DEFAULT_FRAMERATE  15

#define VENC_ONE_SECOND_COUNT                   1000000ULL
#define VENC_CALC_UNIFY_FR(frame_rate)          180*1000/(frame_rate)
#define TO_MS(pts)                              (pts)/1000
#define VENC_PTS_1MHZ_TO_90KHZ(pts)             pts*90/1000
#define VENC_PTS_1MHZ_TO_27MHZ(pts)             pts*27


FY_U8 au8FileHeader[] =
{
    0x49, 0x4D, 0x4B, 0x48, //0x0
    0x02, 0x01, 0x00, 0x00, //0x4
    0x02, 0x00, 0x05, /*1:h264, 5:h265*/ 0x00, //0x08
    0x00, 0x00, 0x00, 0x00, //0x0c (audio enable&audio type)
    0x00, 0x00, 0x00, 0x00, //0x10
    0x00, 0x00, 0x00, 0x00, //0x14
    0x00/*smart enbale:0x81*/, 0x00, 0x00, 0x00,//0x18 
    0x00, 0x00, 0x00, 0x00, //0x1c
    0x00, 0x00, 0x00, 0x00, //0x20
    0x00, 0x00, 0x00, 0x00, //0x24
};

FY_U8 au8PsmHeader[] =
{
    0x00, 0x4E/*pes len*/, 0xE0, 0xFF, //0x0

    /*programe stream info start*/
    0x00, 0x24,//0x04
    0x40, 0x0E, /*programe stream info*/
    0x48, 0x4B, 0x01, 0x00, //0x08
    0x13, 0x1F, 0x2C, 0xF6, //0x0c
    0xE8, 0xB8, 0x00, 0xFF, //0x10
    0xFF, 0xFF, 0x41, 0x12, //0x14
    0x48, 0x4B, 0x00, 0x00, //0x18
    0x00, 0x00, 0x00, 0x00, //0x1c
    0x00, 0x00, 0x00, 0x00, //0x20
    0x00, 0x00, 0x00, 0x00, //0x24
    0x00, 0x00, //0x28
    /*programe stream info end*/

    /* elementary stream map start*/
    0x00, 0x20, //0x28, ES info len
    0x24, 0xE0, 0x00, 0x1C, //0x2c
    0x42, 0x0E, 0x00, 0x00, //0x30
    0x60, 0x00, 0x03, 0xc0, //0x34, 0x36-0x37: pic width
    0x04, 0x40, 0x02, 0x0F, //0x38, 0x38-0x39: pic height
    0xFF, 0x00, 0x55, 0x55, //0x3c, 0x3e-0x3f: frame rate
    0x44, 0x0A, 0x00, 0x00, //0x40
    0x80, 0x00, 0x00, 0x00, //0x44
    0x00, 0x00, 0xFF, 0xFF, //0x48
    /* elementary stream map end*/
    
    /* crc */
    0x99, 0xFF, 0xA2, 0xB9, //0x4c
};

typedef struct bits_buffer
{
    int     i_data;
    FY_U8   i_mask;
    FY_U8   *p_data;

} bits_buffer_s;

static inline void bits_write( bits_buffer_s *p_buffer, FY_S32 i_count, FY_U64 i_bits )
{
    while( i_count > 0 )
    {
        i_count--;

        if( ( i_bits >> i_count )&0x01 )
        {
            p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask;
        }
        else
        {
            p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask;
        }
        p_buffer->i_mask >>= 1;
        if( p_buffer->i_mask == 0 )
        {
            p_buffer->i_data++;
            p_buffer->i_mask = 0x80;
        }
    }
}


int VENC_RECFILE_MakePsHeader(FY_S8 *pData, FY_U64 s64Scr, FY_U32 u32SeqNum)
{	
    FY_U64 lScrExt = VENC_PTS_1MHZ_TO_27MHZ(s64Scr);
    s64Scr = VENC_PTS_1MHZ_TO_90KHZ(s64Scr);    
    bits_buffer_s  	bitsBuffer;	
    bitsBuffer.i_data = 0;    
    bitsBuffer.i_mask = 0x80; 
    // 二进制：10000000 这里是为了后面对一个字节的每一位进行操作，避免大小端夸字节字序错乱	
    bitsBuffer.p_data =	(unsigned char *)(pData);	
    memset(bitsBuffer.p_data, 0, PS_HDR_LEN);    
    bits_write(&bitsBuffer, 32, 0x000001BA);     /*start codes*/

    //byte1 ~ byte6
    bits_write(&bitsBuffer, 2, 	1);		 /*marker bits '01b'*/    
    bits_write(&bitsBuffer, 3, 	(s64Scr>>30)&0x07);     /*System clock [32..30]*/	
    bits_write(&bitsBuffer, 1, 	1);						/*marker bit*/    
    bits_write(&bitsBuffer, 15, (s64Scr>>15)&0x7FFF);   /*System clock [29..15]*/	
    bits_write(&bitsBuffer, 1, 	1);						/*marker bit*/    
    bits_write(&bitsBuffer, 15, s64Scr&0x7fff);         /*System clock [29..15]*/	
    bits_write(&bitsBuffer, 1, 	1);						/*marker bit*/	
    bits_write(&bitsBuffer, 9, 	lScrExt&0x01ff);		/*System clock [14..0]*/	
    bits_write(&bitsBuffer, 1, 	1);						/*marker bit*/	
    
    bits_write(&bitsBuffer, 22, (255)&0x3fffff);		/*bit rate(n units of 50 bytes per second.)*/	
    bits_write(&bitsBuffer, 2, 	3);						/*marker bits '11'*/	
    
    bits_write(&bitsBuffer, 5, 	0x1f);					/*reserved(reserved for future use)*/	
    bits_write(&bitsBuffer, 3, 	6);						/*stuffing length*/	

    bits_write(&bitsBuffer, 8, 	0xff);				    /*stuff 0 */	
    bits_write(&bitsBuffer, 8, 	0xff);					/*stuff 1*/	
    bits_write(&bitsBuffer, 32, u32SeqNum);				/*stuffing sequence number*/	
    
    return 0;
}

int VENC_RECFILE_MakeSysHeader(FY_S8 *pData)
{		
    bits_buffer_s   bitsBuffer; 
    bitsBuffer.i_data = 0;  
    bitsBuffer.i_mask = 0x80;   
    bitsBuffer.p_data = (unsigned char *)(pData); 
    
    memset(bitsBuffer.p_data, 0, SYS_HDR_LEN);  /*system header*/   
    bits_write( &bitsBuffer, 32, 0x000001BB);   /*start code*/    
    bits_write( &bitsBuffer, 16, SYS_HDR_LEN-6);  /*header_length 表示此字节后面的长度，后面的相关头也是此意思*/    
    bits_write( &bitsBuffer, 1,    1);            /*marker_bit*/  
    bits_write( &bitsBuffer, 22, 50000);        /*rate_bound*/    
    bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/    
    bits_write( &bitsBuffer, 6,  1);            /*audio_bound*/    
    bits_write( &bitsBuffer, 1,  0);            /*fixed_flag */    
    bits_write( &bitsBuffer, 1,  1);          /*CSPS_flag */    
    bits_write( &bitsBuffer, 1,  1);          /*system_audio_lock_flag*/    
    bits_write( &bitsBuffer, 1,  1);          /*system_video_lock_flag*/    
    bits_write( &bitsBuffer, 1,  1);          /*marker_bit*/    
    bits_write( &bitsBuffer, 5,  1);          /*video_bound*/    
    bits_write( &bitsBuffer, 1,  0);         /*dif from mpeg1*/    
    bits_write( &bitsBuffer, 7,  0x7F);       /*reserver*/    /*audio stream bound*/    
    bits_write( &bitsBuffer, 8,  0xC0);         /*stream_id*/    
    bits_write( &bitsBuffer, 2,  3);         /*marker_bit */    
    bits_write( &bitsBuffer, 1,  0);            /*PSTD_buffer_bound_scale*/    
    bits_write( &bitsBuffer, 13, 512);          /*PSTD_buffer_size_bound*/    /*video stream bound*/    
    bits_write( &bitsBuffer, 8,  PS_VIDEO_STREAM_ID);         /*stream_id*/    
    bits_write( &bitsBuffer, 2,  3);         /*marker_bit */    
    bits_write( &bitsBuffer, 1,  1);         /*PSTD_buffer_bound_scale*/    
    bits_write( &bitsBuffer, 13, 2048);      /*PSTD_buffer_size_bound*/  
    return 0;
}

int VENC_RECFILE_MakePsmHeader(FY_S8 *pData, PAYLOAD_TYPE_E enStreamType, FY_U32 u32Width, FY_U32 u32Heigth, FY_U32 u32FrameRate)
{		    
    bits_buffer_s   bitsBuffer; 
    bitsBuffer.i_data = 0;  
    bitsBuffer.i_mask = 0x80;   
    bitsBuffer.p_data = (unsigned char *)(pData);   
    memset(bitsBuffer.p_data, 0, PSM_HDR_LEN);  
    bits_write(&bitsBuffer, 24,0x000001);   /*start code*/  
    bits_write(&bitsBuffer, 8, 0xBC);       /*map stream id*/ 

    memcpy(pData+4, au8PsmHeader, sizeof(au8PsmHeader));

    //set pic width/height
    *(pData+PSM_PIC_WIDTH_OFFSET) = (u32Width>>8)&0xFF;
    *(pData+PSM_PIC_WIDTH_OFFSET+1) = u32Width&0xFF;        
    *(pData+PSM_PIC_HEIGHT_OFFSET) = (u32Heigth>>8)&0xFF;
    *(pData+PSM_PIC_HEIGHT_OFFSET+1) = u32Heigth&0xFF;     
    
    //set frame rate
    if(u32FrameRate != -1)
    {
        *(pData+PSM_PIC_FRAMERATE_OFFSET) = (u32FrameRate>>16)&0xFF;        
        *(pData+PSM_PIC_FRAMERATE_OFFSET+1) = (u32FrameRate>>8)&0xFF;        
        *(pData+PSM_PIC_FRAMERATE_OFFSET+2) = u32FrameRate&0xFF;        
    }
            
    //set stream type
    *(pData+PSM_PIC_STREAMID_OFFSET) = (enStreamType == PT_H264)?0x1B:0x24;
        
    return 0;
}

int VENC_RECFILE_MakePesHeader(FY_S8 *pData, FY_S32 stream_id, FY_S32 payload_len, FY_U64 pts, FY_BOOL bFirstPesPack)
{		
    bits_buffer_s    bitsBuffer; 
    
    pts = VENC_PTS_1MHZ_TO_90KHZ(pts);
    
    bitsBuffer.i_data = 0;  
    bitsBuffer.i_mask = 0x80;   
    bitsBuffer.p_data = (unsigned char *)(pData);  
    memset(bitsBuffer.p_data, 0, PES_HDR_LEN);  /*system header*/  
    bits_write( &bitsBuffer, 24,0x000001);  /*start code*/  
    bits_write( &bitsBuffer, 8, (stream_id));   /*streamID*/    
    bits_write( &bitsBuffer, 16,(payload_len)+11);  /*packet_len*/ 
    
    bits_write( &bitsBuffer, 2, 2 );        /*'10'*/    
    bits_write( &bitsBuffer, 2, 0 );        /*scrambling_control*/  
    bits_write( &bitsBuffer, 1, 1 );        /*priority*/    
    bits_write( &bitsBuffer, 1, bFirstPesPack?1:0 );        /*data_alignment_indicator*/   
    bits_write( &bitsBuffer, 1, 0 );        /*copyright*/  
    bits_write( &bitsBuffer, 1, 0 );        /*original_or_copy*/   
    
    bits_write( &bitsBuffer, 1, 1 );        /*PTS_flag*/   
    bits_write( &bitsBuffer, 1, 0);        /*DTS_flag*/   
    bits_write( &bitsBuffer, 1, 0 );        /*ESCR_flag*/ 
    bits_write( &bitsBuffer, 1, 0 );        /*ES_rate_flag*/      
    bits_write( &bitsBuffer, 1, 0 );        /*DSM_trick_mode_flag*/
    bits_write( &bitsBuffer, 1, 0 );        /*additional_copy_info_flag*/ 
    bits_write( &bitsBuffer, 1, 0 );        /*PES_CRC_flag*/ 
    bits_write( &bitsBuffer, 1, 0 );        /*PES_extension_flag*/ 
    
    bits_write( &bitsBuffer, 8, 8);        /*header_data_length*/  
    bits_write( &bitsBuffer, 4, 3 );                    /*'0011'*/    
    bits_write( &bitsBuffer, 3, ((pts)>>30)&0x07 );     /*PTS[32..30]*/  
    bits_write( &bitsBuffer, 1, 1 );   
    
    bits_write( &bitsBuffer, 15,((pts)>>15)&0x7FFF);    /*PTS[29..15]*/ 
    bits_write( &bitsBuffer, 1, 1 );   
    bits_write( &bitsBuffer, 15,(pts)&0x7FFF);          /*PTS[14..0]*/ 
    bits_write( &bitsBuffer, 1, 1 );   

    bits_write( &bitsBuffer, 8, 0xff ); 
    bits_write( &bitsBuffer, 8, 0xff ); 
    bits_write( &bitsBuffer, 8, 0xf8 ); 
   
    return 0;
}

FY_S32 VENC_RECFILE_MakeFileHeader(PAYLOAD_TYPE_E enStreamType, FY_BOOL bEnableSmart, FILE* pFd)
{
    FY_U8 au8Header[FILE_HEADER_SIZE];

    memcpy(au8Header, au8FileHeader, FILE_HEADER_SIZE);

    if(enStreamType == PT_H264)
        au8Header[FILE_HEADER_STRMTYPE] = 0x1;
    else if(enStreamType == PT_H265)
        au8Header[FILE_HEADER_STRMTYPE] = 0x5;
    else
        return FY_FAILURE;

    if(bEnableSmart)
        au8Header[0x18] = 0x81; //smart mode

    fwrite(au8Header, FILE_HEADER_SIZE, 1, pFd);    

    return FY_SUCCESS;
}

FY_S32 VENC_RECFILE_VidoeEs2Ps(VENC_REC_STREAM_INFO_S *pstRecStrInfo, FILE* pFd, VENC_STREAM_S* pstStream)
{
    FY_S8 szTempPacketHead[256];
    int i;
    FY_U32 u32RemainPackLen=0, u32WriteLen=0;
    FY_U8 *pWriteAddr=NULL;
    FY_BOOL bFistPesPack;
       
    FY_U32 u32SeqNum = pstRecStrInfo->u32SeqNum;
    FY_BOOL bEnableSmart = pstRecStrInfo->bEnableSmart;
    FY_U64 u64CurPtsUs = pstStream->pstPack[0].u64PTS;
    FY_BOOL bIFrame = (pstStream->u32PackCount > 1)?FY_TRUE:FY_FALSE;

    if(pstRecStrInfo == NULL || pstStream == NULL || pFd == NULL)
    {
        SAMPLE_PRT("input pointer is null\n");
        return FY_FAILURE;        
    }
    
    if(pstRecStrInfo->enStreamType != PT_H264 && pstRecStrInfo->enStreamType != PT_H265)
    {
        SAMPLE_PRT("not support type %d\n", pstRecStrInfo->enStreamType);
        return FY_FAILURE;
    }
    
    if(u32SeqNum == 0)
    {   //new file start
        VENC_RECFILE_MakeFileHeader(pstRecStrInfo->enStreamType, bEnableSmart, pFd); 
    }
    else
    {
        if(u64CurPtsUs == 0)
        {  // for playback, pts is always 0
            u64CurPtsUs = VENC_ONE_SECOND_COUNT*u32SeqNum/pstRecStrInfo->u32FrameRate;
        }
#if 0  // frame rate set by app
        if(u32SeqNum == 1)
        {   /* ====================================
               caculate frame rate and 
               update the value in first psm header 
               when the second frame is received
              =====================================*/
            u32AvrPtsDiffMs[chn] = TO_MS(u64CurPtsUs - u64FirstPtsUs[chn]);
            if(u32AvrPtsDiffMs[chn] == 0 || u32AvrPtsDiffMs[chn] > VENC_ONE_SECOND_COUNT)
                SAMPLE_PRT("Error!!! chn%d, u64FirstPtsUs=%lld(ms), u64CurPtsUs=%lld(ms), pts_diff=%d(ms) exceed 1s\n", 
                chn, TO_MS(u64FirstPtsUs[chn]), TO_MS(u64CurPtsUs), u32AvrPtsDiffMs[chn]);

            u32FrameRate = VENC_CALC_UNIFY_FR(u32AvrPtsDiffMs[chn]);
            u32CurPos = ftell(pFd);
            fseek(pFd, FILE_HEADER_SIZE+PS_HDR_LEN+PSM_PIC_FRAMERATE_OFFSET, SEEK_SET);
            fwrite((char *)&u32FrameRate+2, 1, 1, pFd);
            fwrite((char *)&u32FrameRate+1, 1, 1, pFd);
            fwrite((char *)&u32FrameRate, 1, 1, pFd);
            fseek(pFd, u32CurPos, SEEK_SET);            
            SAMPLE_PRT("set frame rate: chn%d, first-cur=(%lld-%lld)(ms), diff=%d(ms),u32FrameRate=0x%x\n", 
                chn, TO_MS(u64FirstPtsUs[chn]), TO_MS(u64CurPtsUs), 
                u32AvrPtsDiffMs[chn], u32FrameRate);
        }
        else if(!bFrameRateAdjusted[chn] && (u64CurPtsUs-u64FirstPtsUs[chn])>VENC_ONE_SECOND_COUNT*0.9)
        {  // adjust frame rate at 900ms
            u32PtsDiffMs = TO_MS(u64CurPtsUs -u64FirstPtsUs[chn])/u32SeqNum;
            if(u32PtsDiffMs != u32AvrPtsDiffMs[chn] && 
                u32PtsDiffMs >= u32AvrPtsDiffMs[chn]/3 && u32PtsDiffMs <= u32AvrPtsDiffMs[chn]*3) 
            { 
                u32AvrPtsDiffMs[chn] = u32PtsDiffMs;

                
                u32FrameRate = VENC_CALC_UNIFY_FR(u32AvrPtsDiffMs[chn]);
                u32CurPos = ftell(pFd);
                fseek(pFd, FILE_HEADER_SIZE+PS_HDR_LEN+PSM_PIC_FRAMERATE_OFFSET, SEEK_SET);
                fwrite((char *)&u32FrameRate+2, 1, 1, pFd);
                fwrite((char *)&u32FrameRate+1, 1, 1, pFd);
                fwrite((char *)&u32FrameRate, 1, 1, pFd);
                fseek(pFd, u32CurPos, SEEK_SET);            
                SAMPLE_PRT("adjust frame rate: ch%d, diff=%d(ms),u32FrameRate=0x%x\n", 
                    chn, u32AvrPtsDiffMs[chn], u32FrameRate);

                bFrameRateAdjusted[chn] = FY_TRUE;
            }
        }
#endif        
    }

  	memset(szTempPacketHead, 0, 256);	
  	// 1 package for ps header 	
  	VENC_RECFILE_MakePsHeader(szTempPacketHead, u64CurPtsUs, u32SeqNum);	
  	u32WriteLen += PS_HDR_LEN;		
  	//2 system header 	
  	if( bIFrame )	
  	{   /* ps header + ps psm + pes for I frame*/
		//VENC_RECFILE_MakeSysHeader(szTempPacketHead + u32WriteLen);		
		//u32WriteLen += SYS_HDR_LEN;  
    	VENC_RECFILE_MakePsmHeader(szTempPacketHead + u32WriteLen, pstRecStrInfo->enStreamType, 
    	pstRecStrInfo->u32Width, pstRecStrInfo->u32Height, VENC_CALC_UNIFY_FR(pstRecStrInfo->u32FrameRate));    
    	u32WriteLen += PSM_HDR_LEN;   
  	}
    
    fwrite(szTempPacketHead, u32WriteLen, 1, pFd);
    
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        u32RemainPackLen = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
        pWriteAddr = pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset;
        
        bFistPesPack = FY_TRUE;
        do
        {            
            if(u32RemainPackLen > PS_PES_PAYLOAD_SIZE)
            {
                u32WriteLen = PS_PES_PAYLOAD_SIZE;
            }
            else
            {
                u32WriteLen = u32RemainPackLen;
            }

        	VENC_RECFILE_MakePesHeader(szTempPacketHead, PS_VIDEO_STREAM_ID, u32WriteLen, u64CurPtsUs, bFistPesPack);  
            fwrite(szTempPacketHead, PES_HDR_LEN, 1, pFd);
            //split into multi pes packets
            fwrite(pWriteAddr, u32WriteLen, 1, pFd);               

            u32RemainPackLen -= u32WriteLen;
            pWriteAddr += u32WriteLen;
            bFistPesPack = FY_FALSE;
        }while(u32RemainPackLen > 0);

    }   

    return FY_SUCCESS;
}
