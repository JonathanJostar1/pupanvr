#ifndef __FY_DEFINES_H__
#define __FY_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define FY_V01 0x46590100

#ifndef FYCHIP
    #define FYCHIP FY_V01
#endif

#if FYCHIP==FY_V01
    #define CHIP_NAME    "FY01"
    #define MPP_VER_PRIX "_MPP_V"
#else
    #error FYCHIP define may be error
#endif


#define DEFAULT_ALIGN     16
#define MAX_MMZ_NAME_LEN 32
#define MMAX_MMB_NAME_LEN 16

#define MAX_NODE_NUM        16

/* For VDA */
#define VDA_CHN_NUM_MAX   128


/* For VENC */
#define VENC_MAX_CHN_NUM  32
#define VENC_MAX_GRP_NUM  128
#define VENC_MAX_ROI_NUM  8
#define H264E_MIN_WIDTH   176
#define H264E_MIN_HEIGHT  144
#define H265E_MIN_WIDTH   176
#define H265E_MIN_HEIGHT  144
#define JPEGE_MAX_WIDTH   4096
#define JPEGE_MAX_HEIGHT  3072
#define JPEGE_MAX_CHN_NUM  32


/*For RC*/
#define RC_TEXTURE_THR_SIZE         12
#define RC_RQRATIO_SIZE             8



/* For VDEC */
#define VDEC_MAX_CHN_NUM        64
#define MAX_VDEC_CHN            VDEC_MAX_CHN_NUM



/* for VDH-H264  */

/* for VDH-HEVC  */


/* for JPEGD  */

/* for VPSS */
#define VPSS_MAX_WIDTH              2048//1920
#define VPSS_MAX_HEIGHT             2160//1920
#define VPSS_MAX_GRP_NUM            256
#define VPSS_MAX_PHY_CHN_NUM        6//5
#define VPSS_MAX_EXT_CHN_NUM        0
#define VPSS_MAX_CHN_NUM            (VPSS_MAX_PHY_CHN_NUM + VPSS_MAX_EXT_CHN_NUM)
#define VPSS_CHN0                   0
#define VPSS_CHN1                   1
#define VPSS_CHN2                   2
#define VPSS_CHN3                   3
#define VPSS_CHN4                   4
#define VPSS_CHN5                   5
#define VPSS_INVALID_CHN            -1
#define VPSS_LUMA_STAT_NUM          64

/* For PCIV */
#define PCIV_MAX_CHN_NUM          128       /* max pciv channel number in each pciv device */

/* for REGION */
#define OVERLAY_MAX_NUM_VPSS      8

/* for VGS */
#define VGS_MAX_CHN_NUM     96
#define VGS_MAX_PTH_NUM     3


#if 0
#define MAX_COVER_NUM           4
#define MAX_VIOVERLAY_NUM       8
#define MAX_COVEREX_REGION_NUM  16
#define MAX_REGION_NUM          8
#define OVERLAY_START_X_ALIGN   8
#define OVERLAY_START_Y_ALIGN   2
#define MAX_VIOVERLAY_ALPHA     255
#endif


/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_ID_MAX_NUM 2   // viu0~1
#define VIU_TYPE_NUM 2 // viu main/lite
#define VIU_PORT_MAX_NUM 2 // viu port0/port1
#define VIU_CHN_NUM 8 // viu chn0~7s

#define VIU_MAX_DEV_NUM               4 /*viu0 port0+1;viu1 port0+1*/
#define VIU_MAX_CHN_NUM_PER_DEV       4
#define VIU_SUB_CHN_START             16
#define VIU_MAX_CHN_NUM               (VIU_MAX_DEV_NUM * VIU_MAX_CHN_NUM_PER_DEV)


/*get the subchn index by main chn */
#define  SUBCHN(ViChn)                (ViChn + VIU_SUB_CHN_START)
#define VIU_MAX_PHYCHN_NUM            (VIU_MAX_DEV_NUM * VIU_MAX_CHN_NUM_PER_DEV) /*total phy chn*/
#define COVER_MAX_NUM_VI          	  4    /*one chn max mask block*/

#define VIU_STR "mc-viu"
#define VIU_0_STR "mc-viu0"
#define VIU_1_STR "mc-viu1"

#define VIU_ID "molchip,mc-viu"

#define VIU_0_ID "molchip,mc-viu0"
#define VIU_1_ID "molchip,mc-viu1"

typedef enum {
    VIU_0 = 0,
    VIU_1,
    VIU_ID_MAX = VIU_ID_MAX_NUM
}VIU_ID_E;

typedef enum {
    VIU_MAIN = 0,
    VIU_LITE,
    VIU_TYPE_MAX = VIU_TYPE_NUM
}VIU_TYPE_E;

typedef enum {
    VIU_PORT_0 = 0,
    VIU_PORT_1,
    VIU_PORT_MAX = VIU_PORT_MAX_NUM
}VIU_PORT_E;

typedef enum {
    VIU_CHN_0 = 0,
    VIU_CHN_1,
    VIU_CHN_2,
    VIU_CHN_3,
    VIU_CHN_4,
    VIU_CHN_5,
    VIU_CHN_6,
    VIU_CHN_7,
    VIU_CHN_NUM_MAX = VIU_CHN_NUM
}VIU_CHN_E;
/* for VOU  */
#define VO_MAX_DEV_NUM                  3                   /* max dev num */
#define VO_MAX_LAYER_NUM                5                   /* max layer num */
#define VHD_MAX_CHN_NUM                 64                  /* max VHD chn num */
#define VO_MAX_CHN_NUM                  VHD_MAX_CHN_NUM     /* max chn num */
#define VO_MAX_GRAPHICS_LAYER_NUM       5                   /* max gfx layer num */
#define VO_MAX_WBC_NUM                  2                   /* max wbc dev num */

#define AENC_MAX_CHN_NUM     32
#define ADEC_MAX_CHN_NUM     32

/* VB size calculate for compressed frame.
	[param input]
		w: 	width
		h: 	height
		fmt:	pixel format, 0: SP420, 1: SP422
		z:	compress mode, 0: no compress, 1: default compress
	[param output]
		size: vb blk size

 */
#define VB_W_ALIGN		16
#define VB_H_ALIGN		32
#define VB_ALIGN(x, a)	((a) * (((x) + (a) - 1) / (a)))


#define VDU_ALIGN4(_x)              (((_x)+0x03)&~0x03)
#define VDU_ALIGN8(_x)              (((_x)+0x07)&~0x07)
#define VDU_ALIGN16(_x)             (((_x)+0x0f)&~0x0f)
#define VDU_ALIGN32(_x)             (((_x)+0x1f)&~0x1f)
#define VDU_ALIGN64(_x)             (((_x)+0x3f)&~0x3f)
#define VDU_ALIGN128(_x)            (((_x)+0x7f)&~0x7f)
#define VDU_ALIGN256(_x)            (((_x)+0xff)&~0xff)
#define VDU_ALIGN512(_x)            (((_x)+0x1ff)&~0x1ff)
#define VDU_ALIGN1024(_x)           (((_x)+0x3ff)&~0x3ff)
#define VDU_ALIGN2048(_x)           (((_x)+0x7ff)&~0x7ff)
#define VDU_ALIGN4096(_x)           (((_x)+0xfff)&~0xfff)
#define VDU_ALIGN16384(_x)          (((_x)+0x3fff)&~0x3fff)

/* VB size calculate for compressed frame.
	[param input]
		w: 	width
		h: 	height
		fmt:	pixel format, 0: SP420, 1: SP422
		z:	compress mode, 0: no compress, 1: default compress
	[param output]
		size: vb blk size
 */
#define VB_HEADER_STRIDE    16

#define VB_PIC_Y_HEADER_SIZE(Width, Height, size)\
	do{\
            size = VB_HEADER_STRIDE * (Height);\
	}while(0)

#define VB_PIC_HEADER_SIZE(Width, Height, Type, size)\
	do{\
            if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == Type)\
            {\
		        size = VB_HEADER_STRIDE * (Height) * 2;\
            }\
            else\
            {\
                size = (VB_HEADER_STRIDE * (Height) * 3) >> 1;\
            }\
	}while(0)


#define VB_PIC_BLK_SIZE(Width, Height, Type, size)\
	do{\
			unsigned int u32AlignWidth;\
			unsigned int u32AlignHeight;\
			unsigned int __mvSize = 0;\
			unsigned int __fbcYHeaderSize = 0;\
			unsigned int __fbcCHeaderSize = 0,__fbcCHeaderSize2;\
			if (Type==PT_H264 || Type==PT_H265) \
			{\
				u32AlignWidth = VDU_ALIGN64(Width);\
				u32AlignHeight= VDU_ALIGN64(Height);\
				size = (u32AlignWidth * u32AlignHeight * 3 )>> 1;\
				if(Type==PT_H264) \
					__mvSize  =	0; \
				else if(Type==PT_H265) \
					__mvSize  =	((u32AlignWidth*u32AlignHeight*5)>>6); \
				__mvSize = VDU_ALIGN64(__mvSize); \
				__fbcCHeaderSize  = (VDU_ALIGN1024(u32AlignWidth)>>10)*( (VDU_ALIGN4((u32AlignHeight)>>1))>>2)*64; \
				__fbcCHeaderSize2 = (VDU_ALIGN1024(u32AlignHeight)>>10)*( (VDU_ALIGN4((u32AlignWidth)>>1))>>2)*64; \
				if(__fbcCHeaderSize<__fbcCHeaderSize2) __fbcCHeaderSize = __fbcCHeaderSize2; \
				__fbcYHeaderSize  = (__fbcCHeaderSize<<1); \
				size +=  (__mvSize + __fbcYHeaderSize + __fbcCHeaderSize);\
			}\
			else\
			{\
				u32AlignWidth = VDU_ALIGN16(Width);\
				u32AlignHeight= VDU_ALIGN16(Height);\
				size = (u32AlignWidth * u32AlignHeight * 3) >> 1;\
			}\
			size = VDU_ALIGN4096(size); \
	}while(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_DEFINES_H__ */

