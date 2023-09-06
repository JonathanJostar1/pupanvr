#ifndef __FY_COMMON_HDMI_H__
#define __FY_COMMON_HDMI_H__

#include "fy_type.h"
#include "fy_errno.h"
#include "fy_common.h"

#define FY_HDMI_MAX_AUDIO_CAP_COUNT 		16

/**the max audio smprate count*/
#define FY_HDMI_MAX_SAMPE_RATE_NUM		8

/**the max audio bit depth count*/
#define FY_HDMI_MAX_BIT_DEPTH_NUM		6


typedef enum {
	FY_HDMI_VIDEO_FMT_1080P_60 = 0,
	FY_HDMI_VIDEO_FMT_1080P_50,
	FY_HDMI_VIDEO_FMT_1080P_30,
	FY_HDMI_VIDEO_FMT_1080P_25,
	FY_HDMI_VIDEO_FMT_1080P_24,

	FY_HDMI_VIDEO_FMT_1080i_60,
	FY_HDMI_VIDEO_FMT_1080i_50,

	FY_HDMI_VIDEO_FMT_720P_60,
	FY_HDMI_VIDEO_FMT_720P_50,

	FY_HDMI_VIDEO_FMT_576P_50,
	FY_HDMI_VIDEO_FMT_480P_60, /*10*/

	FY_HDMI_VIDEO_FMT_PAL,					/* B D G H I PAL */
	FY_HDMI_VIDEO_FMT_PAL_N,				/* (N)PAL		 */
	FY_HDMI_VIDEO_FMT_PAL_Nc,				/* (Nc)PAL		 */

	FY_HDMI_VIDEO_FMT_NTSC, 				/* (M)NTSC		 */
	FY_HDMI_VIDEO_FMT_NTSC_J,				/* NTSC-J		 */
	FY_HDMI_VIDEO_FMT_NTSC_PAL_M,			/* (M)PAL		 */

	FY_HDMI_VIDEO_FMT_SECAM_SIN,			/**< SECAM_SIN*/
	FY_HDMI_VIDEO_FMT_SECAM_COS,			/**< SECAM_COS*/

	FY_HDMI_VIDEO_FMT_861D_640X480_60,
	FY_HDMI_VIDEO_FMT_VESA_800X600_60,	/*20*/
	FY_HDMI_VIDEO_FMT_VESA_1024X768_60,
	FY_HDMI_VIDEO_FMT_VESA_1280X720_60,
	FY_HDMI_VIDEO_FMT_VESA_1280X800_60,
	FY_HDMI_VIDEO_FMT_VESA_1280X1024_60,
	FY_HDMI_VIDEO_FMT_VESA_1366X768_60,
	FY_HDMI_VIDEO_FMT_VESA_1440X900_60,
	FY_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
	FY_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
	FY_HDMI_VIDEO_FMT_VESA_1600X1200_60,
	FY_HDMI_VIDEO_FMT_VESA_1680X1050_60, /*30*/
	FY_HDMI_VIDEO_FMT_VESA_1920X1080_60,
	FY_HDMI_VIDEO_FMT_VESA_1920X1200_60,
	FY_HDMI_VIDEO_FMT_VESA_2048X1152_60,	/* Currently, not support */

	FY_HDMI_VIDEO_FMT_2560x1440_30,
	FY_HDMI_VIDEO_FMT_2560x1440_60,
	FY_HDMI_VIDEO_FMT_2560x1600_60,
	FY_HDMI_VIDEO_FMT_1920x2160_30,

	FY_HDMI_VIDEO_FMT_3840X2160P_24,		/** 93:3840x2160p @ 24Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_3840X2160P_25,		/** 94:3840x2160p @ 25Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_3840X2160P_30,		/** 95:3840x2160p @ 30Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_3840X2160P_50,		/** 96:3840x2160p @ 50Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_3840X2160P_60,		/** 97:3840x2160p @ 60Hz	No Repetition */

	FY_HDMI_VIDEO_FMT_4096X2160P_24,		/** 98:4096x2160p @ 24Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_4096X2160P_25,		/** 99:4096x2160p @ 25Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_4096X2160P_30,		/** 100:4096x2160p @ 30Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_4096X2160P_50,		/** 101:4096x2160p @ 50Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_4096X2160P_60,		/** 102:4096x2160p @ 60Hz	No Repetition */
	FY_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE, /** Currently, not support */

	FY_HDMI_VIDEO_FMT_BUTT
} FY_HDMI_VIDEO_FMT_E;

typedef enum {
	FY_HDMI_AUDIO_FORMAT_CODE_RESERVED	= 0x00,  /**<Audio coding type, refer stream,default type*/
	FY_HDMI_AUDIO_FORMAT_CODE_PCM,				 /**<Audio coding PCM type*/
	FY_HDMI_AUDIO_FORMAT_CODE_AC3,				 /**<Audio coding AC3 type*/
	FY_HDMI_AUDIO_FORMAT_CODE_MPEG1,			 /**<Audio coding MPEG1 type*/
	FY_HDMI_AUDIO_FORMAT_CODE_MP3,				 /**<Audio coding MP3 type*/
	FY_HDMI_AUDIO_FORMAT_CODE_MPEG2,			 /**<Audio coding MPEG2 type*/
	FY_HDMI_AUDIO_FORMAT_CODE_AAC,				 /**<Audio coding AAC type*/
	FY_HDMI_AUDIO_FORMAT_CODE_DTS,				 /**<Audio coding DTS type*/
	FY_HDMI_AUDIO_FORMAT_CODE_ATRAC,			 /**<Audio coding ATRAC type*/
	FY_HDMI_AUDIO_FORMAT_CODE_ONE_BIT,			 /**<Audio coding ONE BIT AUDIO type*/
	FY_HDMI_AUDIO_FORMAT_CODE_DDP,				 /**<Audio coding DDPLUS type*/
	FY_HDMI_AUDIO_FORMAT_CODE_DTS_HD,			 /**<Audio coding DTS HD type*/
	FY_HDMI_AUDIO_FORMAT_CODE_MAT,				 /**<Audio coding MAT type*/
	FY_HDMI_HDMI_AUDIO_FORMAT_CODE_DST, 		 /**<Audio coding DST type*/
	FY_HDMI_AUDIO_FORMAT_CODE_WMA_PRO,			 /**<Audio coding WMA PRO type*/
	FY_HDMI_AUDIO_FORMAT_CODE_BUTT,
}FY_HDMI_AUDIO_FORMAT_CODE_E;

typedef enum {
	FY_HDMI_SAMPLE_RATE_UNKNOWN=0,			/**< unknown sample rate	*/
	FY_HDMI_SAMPLE_RATE_8K	  = 8000,		/**< 8K sample rate 	 */
	FY_HDMI_SAMPLE_RATE_11K   = 11025,		/**< 11.025K sample rate */
	FY_HDMI_SAMPLE_RATE_12K   = 12000,		/**< 12K sample rate	 */
	FY_HDMI_SAMPLE_RATE_16K   = 16000,		/**< 16K sample rate	 */
	FY_HDMI_SAMPLE_RATE_22K   = 22050,		/**< 22.050K sample rate */
	FY_HDMI_SAMPLE_RATE_24K   = 24000,		/**< 24K sample rate	 */
	FY_HDMI_SAMPLE_RATE_32K   = 32000,		/**< 32K sample rate	 */
	FY_HDMI_SAMPLE_RATE_44K   = 44100,		/**< 44.1K sample rate	 */
	FY_HDMI_SAMPLE_RATE_48K   = 48000,		/**< 48K sample rate	 */
	FY_HDMI_SAMPLE_RATE_88K   = 88200,		/**< 88.2K sample rate	 */
	FY_HDMI_SAMPLE_RATE_96K   = 96000,		/**< 96K sample rate	 */
	FY_HDMI_SAMPLE_RATE_176K  = 176400,		/**< 176K sample rate	 */
	FY_HDMI_SAMPLE_RATE_192K  = 192000, 	/**< 192K sample rate	 */
	FY_HDMI_SAMPLE_RATE_BUTT		,
}FY_HDMI_SAMPLE_RATE_E;

typedef enum {
	FY_HDMI_BIT_DEPTH_UNKNOWN =0,			/**< unknown bit width	*/
	FY_HDMI_BIT_DEPTH_8  = 8,				/**< 8 bits width  */
	FY_HDMI_BIT_DEPTH_16 = 16,				/**< 16 bits width */
	FY_HDMI_BIT_DEPTH_18 = 18,				/**< 18 bits width */
	FY_HDMI_BIT_DEPTH_20 = 20,				/**< 20 bits width */
	FY_HDMI_BIT_DEPTH_24 = 24,				/**< 24 bits width */
	FY_HDMI_BIT_DEPTH_32 = 32,				/**< 32 bits width */
	FY_HDMI_BIT_DEPTH_BUTT,
}FY_HDMI_BIT_DEPTH_E;

typedef enum {
	FY_HDMI_AUDIO_SPEAKER_FL_FR   ,
	FY_HDMI_AUDIO_SPEAKER_LFE	  ,
	FY_HDMI_AUDIO_SPEAKER_FC	  ,
	FY_HDMI_AUDIO_SPEAKER_RL_RR   ,
	FY_HDMI_AUDIO_SPEAKER_RC	  ,
	FY_HDMI_AUDIO_SPEAKER_FLC_FRC ,
	FY_HDMI_AUDIO_SPEAKER_RLC_RRC ,
	FY_HDMI_AUDIO_SPEAKER_FLW_FRW ,
	FY_HDMI_AUDIO_SPEAKER_FLH_FRH ,
	FY_HDMI_AUDIO_SPEAKER_TC	  ,
	FY_HDMI_AUDIO_SPEAKER_FCH	  ,
	FY_HDMI_AUDIO_SPEAKER_BUTT	  ,
} FY_HDMI_AUDIO_SPEAKER_E;

typedef struct {
	FY_U32 u32VFB;
	FY_U32 u32VBB;
	FY_U32 u32VACT;
	FY_U32 u32HFB;
	FY_U32 u32HBB;
	FY_U32 u32HACT;
	FY_U32 u32VPW;
	FY_U32 u32HPW;
	FY_U32 bIDV;
	FY_U32 bIHS;
	FY_U32 bIVS;
	FY_U32 u32ImageWidth;
	FY_U32 u32ImageHeight;
	FY_U32 u32AspectRationW;
	FY_U32 u32AspectRationH;
	FY_U32 u32Interlace;
	FY_U32 u32PixelClk;
} FY_HDMI_TIMING_INFO_S;

typedef struct {
	FY_U32 u32DetTimingNum;
	FY_HDMI_TIMING_INFO_S astDetTiming[10];
}FY_HDMI_DET_TIMING_S;

typedef struct{
	FY_HDMI_AUDIO_FORMAT_CODE_E 	enAudFmtCode;										/**<Audio coding type*/
	FY_HDMI_SAMPLE_RATE_E			enSupportSampleRate[FY_HDMI_MAX_SAMPE_RATE_NUM];	/**<Audio sample rate*/
	FY_U8							u8AudChannel;										/**<Aud Channel of the coding type*/
	FY_HDMI_BIT_DEPTH_E 			bSupportBitDepth[FY_HDMI_MAX_BIT_DEPTH_NUM];		/**< sample bit depth,when audio format code is FY_UNF_EDID_AUDIO_FORMAT_CODE_PCM*/
	FY_U32							u32SupportBitDepthNum;								/**< sample bit depth Num,when audio format code is FY_UNF_EDID_AUDIO_FORMAT_CODE_PCM*/
	FY_U32							u32MaxBitRate;										/**< enter max bit rate,when audio format code is FY_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - FY_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC**/
}FY_HDMI_AUDIO_INFO_S;

typedef struct {
	FY_BOOL bConnected; // Whether the devices are connected
	FY_BOOL bSupportHdmi; //Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI
	FY_BOOL bIsSinkPowerOn; // Whether the sink device is powered on. */
	FY_BOOL bIsRealEDID; //Whether the EDID obtains the flag from the sink device.FY_TRUE: The EDID information is correctly read.FY_FASE: default settings
	FY_HDMI_VIDEO_FMT_E enNativeVideoFormat; // Physical resolution of the display device
	FY_BOOL bVideoFmtSupported[FY_HDMI_VIDEO_FMT_BUTT]; // Video capability set.FY_TRUE: This display format is supported.FY_FALSE: This display format is not supported
	FY_BOOL bSupportYCbCr; // Whether the YCBCR display is supported.FY_TRUE: The YCBCR display is supported.FY_FALSE: Only red-green-blue (RGB) is supported
	FY_BOOL bSupportxvYCC601; // Whether the xvYCC601 color format is supported
	FY_BOOL bSupportxvYCC709; // Whether the xvYCC709 color format is supported
	FY_U8 u8MDBit; // Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2
	FY_U32 u32AudioInfoNum; // Num of audio Info
	FY_HDMI_AUDIO_INFO_S stAudioInfo[FY_HDMI_MAX_AUDIO_CAP_COUNT]; // Audio Info.For details, see Table 37 in EIA-CEA-861-D
	FY_BOOL bSpeaker[FY_HDMI_AUDIO_SPEAKER_BUTT]; // Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D
	FY_U8 u8IDManufactureName[4]; // Device vendor flag
	FY_U32 u32IDProductCode; // Device ID.
	FY_U32 u32IDSerialNumber; // Device sequence number
	FY_U32 u32WeekOfManufacture; // Device production data (week)
	FY_U32 u32YearOfManufacture; //Set the production data (year)
	FY_U8 u8Version; // Device version number
	FY_U8 u8Revision; // Device sub version number
	FY_U8 u8EDIDExternBlockNum; // EDID extended block number
	FY_BOOL bIsPhyAddrValid; // Valid flag of the consumer electronics control (CEC) physical address
	FY_U8 u8PhyAddr_A; // CEC physical address A
	FY_U8 u8PhyAddr_B; // CEC physical address B
	FY_U8 u8PhyAddr_C; // CEC physical address C
	FY_U8 u8PhyAddr_D; // CEC physical address D
	FY_BOOL bSupportDVIDual; // Whether to support the DVI dual-link operation
	FY_BOOL bSupportDeepColorYCBCR444; // Whether to support the YCBCR 4:4:4 deep-color mode
	FY_BOOL bSupportDeepColor30Bit; // Whether to support the deep-color 30-bit mode
	FY_BOOL bSupportDeepColor36Bit; // Whether to support the deep-color 36-bit mode
	FY_BOOL bSupportDeepColor48Bit; // Whether to support the deep-color 48-bit mode
	FY_BOOL bSupportAI; // Whether to support the Supports_AI mode
	FY_U32 u32MaxTMDSClock; // Maximum TMDS clock
	FY_BOOL bI_Latency_Fields_Present; // Delay flag bit
	FY_BOOL bLatency_Fields_Present; // whether Video_Latency and Audio_Latency fields are present
	FY_BOOL bHDMI_Video_Present; // Special video format
	FY_U8 u8Video_Latency; // Video delay
	FY_U8 u8Audio_Latency; // Audio delay
	FY_U8 u8Interlaced_Video_Latency; // Video delay in interlaced video mode
	FY_U8 u8Interlaced_Audio_Latency; // Audio delay in interlaced video mode
	FY_BOOL bSupportY420DC30Bit; // Whether to support the YUV420 deep-color 30-bit mode
	FY_BOOL bSupportY420DC36Bit; // Whether to support the YUV420 deep-color 36-bit mode
	FY_BOOL bSupportY420DC48Bit; // Whether to support the YUV420 deep-color 48-bit mode
	FY_BOOL bSupportHdmi_2_0; // Whether to support HDMI2.0
	FY_BOOL bSupportY420Format[FY_HDMI_VIDEO_FMT_BUTT]; // the format support YUV420
	FY_BOOL bOnlySupportY420Format[FY_HDMI_VIDEO_FMT_BUTT]; // the format only support YUV420
	FY_HDMI_DET_TIMING_S stDetailedTiming; //Detailed timing infomation
} VO_HDMI_SINK_S;


#endif /* End of #ifndef __FY_COMMON_HDMI_H__ */

