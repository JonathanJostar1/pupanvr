
#ifndef  __FY_COMM_VDEC_H__
#define  __FY_COMM_VDEC_H__
#include "fy_type.h"
#include "fy_common.h"
#include "fy_errno.h"
#include "fy_comm_video.h"
#include "fy_comm_vb.h"
#include "fy_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/**********************************************************************
    - Macro
**********************************************************************/
#define FY_IO_BLOCK   FY_TRUE
#define FY_IO_NOBLOCK FY_FALSE


/**********************************************************************
    - Structure and Enumeration
**********************************************************************/

/*
 * extend command for vdec, used in API: FY_MPI_VDEC_ExtApi
*/
enum {
    VDEC_EXT_CMD_GETFRAME_SYNC = 10,

};

typedef enum fyVIDEO_MODE_E
{
    VIDEO_MODE_STREAM = 0,     /*send by stream*/
    VIDEO_MODE_FRAME     ,     /*send by frame*/
    VIDEO_MODE_BUTT
}VIDEO_MODE_E;

typedef enum fyVDEC_JPG_COLOR_FMT_E
{
    JPG_COLOR_FMT_YCBCR400,        /*YUV400*/
    JPG_COLOR_FMT_YCBCR420,        /*YUV420*/
    JPG_COLOR_FMT_YCBCR422BHP,     /*YUV 422 2x1*/
    JPG_COLOR_FMT_YCBCR422BVP,     /*YUV 422 1x2*/
    JPG_COLOR_FMT_YCBCR444,        /*YUV 444*/
    JPG_COLOR_FMT_BUTT
} VDEC_JPG_COLOR_FMT_E;


typedef enum fyVDEC_EVNT_E
{
    VDEC_EVNT_STREAM_ERR = 1,
    VDEC_EVNT_UNSUPPORT,
    VDEC_EVNT_OVER_REFTHR,
    VDEC_EVNT_REF_NUM_OVER,
    VDEC_EVNT_SLICE_NUM_OVER,
    VDEC_EVNT_SPS_NUM_OVER,
    VDEC_EVNT_PPS_NUM_OVER,
    VDEC_EVNT_PICBUF_SIZE_ERR,
    VDEC_EVNT_SIZE_OVER,
    VDEC_EVNT_IMG_SIZE_CHANGE,
    VDEC_EVNT_VPS_NUM_OVER,
    VDEC_EVNT_BUTT
} VDEC_EVNT_E;


typedef struct fyVDEC_CHN_LUM_S
{
    FY_U64 u64LumPixSum;                /* Luma sum of current frame */
    FY_U32 u32LumPixAverage;            /* Luma average of current frame */
    FY_U64 u64Pts;                      /* PTS of current frame ?? */
} VDEC_CHN_LUM_S;

typedef struct fyVDEC_ATTR_JPEG_S
{
    VIDEO_MODE_E         enMode;        /* video stream mode   select */
    VDEC_JPG_COLOR_FMT_E enJpegFormat;  /* jpeg  format select ,may   be YUV420,YUV400,YUV422,YUV444*/
}VDEC_ATTR_JPEG_S,*PTR_VDEC_ATTR_JPEG_S;


typedef struct fyVDEC_ATTR_VIDEO_S
{
    FY_U32       u32RefFrameNum;         /*ref pic num [1,16]*/
    VIDEO_MODE_E enMode;                 /*send by stream or by frame*/
    FY_BOOL      bTemporalMvpEnable;     /* specifies whether temporal motion vector predictors can be used for inter prediction*/
}VDEC_ATTR_VIDEO_S,*PTR_VDEC_ATTR_VIDEO_S;


typedef struct fyVDEC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E enType;                  /* video type to be decoded */
    FY_U32      u32BufSize;                 /* stream buf size(Byte) */
    FY_U32      u32Priority;                /* priority */
    FY_U32      u32PicWidth;                /* max pic width */
    FY_U32      u32PicHeight;               /* max pic height */
    union
    {
        VDEC_ATTR_JPEG_S  stVdecJpegAttr;    /* structure with jpeg or mjpeg type  */
        VDEC_ATTR_VIDEO_S stVdecVideoAttr;   /* structure with video ( h264/mpeg4) */
    };
}VDEC_CHN_ATTR_S;


typedef struct fyVDEC_STREAM_S
{
    FY_U8*  pu8Addr;            /* stream address */
    FY_U32  u32Len;             /* stream len */
    FY_U64  u64PTS;             /* time stamp */
    FY_BOOL bEndOfFrame;        /* is the end of a frame */
    FY_BOOL bEndOfStream;       /* is the end of all stream */
}VDEC_STREAM_S;

typedef struct fyVDEC_USERDATA_S{
    FY_U8*          pu8Addr;        /* userdata data vir address */
    FY_U32          u32PhyAddr;     /* userdata data phy address */
    FY_U32          u32Len;         /* userdata data len */
    FY_BOOL         bValid;         /* is valid? */
}VDEC_USERDATA_S;


typedef struct fy_VDEC_DECODE_ERROR_S
{
    FY_S32 s32FormatErr;            /* format error. eg: do not support filed */
    FY_S32 s32PicSizeErrSet;        /* picture width or height is larger than chnnel width or height*/
    FY_S32 s32StreamUnsprt;         /* unsupport the stream specification */
    FY_S32 s32PackErr;              /* stream package error */
    FY_S32 s32PrtclNumErrSet;       /* protocol num is not enough. eg: slice, pps, sps */
    FY_S32 s32RefErrSet;            /* refrence num is not enough */
    FY_S32 s32PicBufSizeErrSet;     /* the buffer size of picture is not enough */
    FY_S32 s32VdecStreamNotRelease; /* the stream not released for too long time */
}VDEC_DECODE_ERROR_S;


typedef struct fyVDEC_CHN_STAT_S
{
    PAYLOAD_TYPE_E enType;               /* video type to be decoded */
    FY_U32  u32LeftStreamBytes;          /* left stream bytes waiting for decode */
    FY_U32  u32LeftStreamFrames;         /* left frames waiting for decode,only valid for H264D_MODE_FRAME */
    FY_U32  u32LeftPics;                 /* pics waiting for output */
    FY_BOOL bStartRecvStream;            /* had started recv stream? */
    FY_U32  u32RecvStreamFrames;         /* how many frames of stream has been received. valid when send by frame. */
    FY_U32  u32DecodeStreamFrames;       /* how many frames of stream has been decoded. valid when send by frame. */
    VDEC_DECODE_ERROR_S stVdecDecErr;    /* information about decode error */
}VDEC_CHN_STAT_S;

typedef struct fyVDEC_CHN_STAT_EX_S
{
	VDEC_CHN_STAT_S stat;
	FY_U32      u32PicWidth;               /* decoded pic width */
	FY_U32      u32PicHeight;              /* decoded pic height */
}VDEC_CHN_STAT_EX_S;


/*
 * static parameter: must set after stop sending stream and all stream is decoded.
 * dynamic parameter: can be set at any time.
 */
typedef struct fyVDEC_CHN_PARAM_S
{
    FY_S32  s32DisplayFrameNum; /* display frame num */
    FY_S32  s32ChanErrThr;      /* threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    FY_S32  s32ChanStrmOFThr;   /* threshold for stream overflow, 0~ , 0: nothing to do when stream is overflow */
    FY_S32  s32DecMode;         /* decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    FY_S32  s32DecOrderOutput;  /* frames output order ,0: the same with display order , 1: the same width decoder order */
    VIDEO_FORMAT_E  enVideoFormat;
    COMPRESS_MODE_E enCompressMode;
}VDEC_CHN_PARAM_S;


typedef struct fyH264_PRTCL_PARAM_S
{
    FY_S32  s32MaxSliceNum;           /* max  slice num support */
    FY_S32  s32MaxSpsNum;             /* max sps num support */
    FY_S32  s32MaxPpsNum;             /* max pps num support */
}H264_PRTCL_PARAM_S;

typedef struct fyH265_PRTCL_PARAM_S
{
    FY_S32  s32MaxSliceSegmentNum;    /* max slice segmnet num support */
    FY_S32  s32MaxVpsNum;             /* max vps num support */
    FY_S32  s32MaxSpsNum;             /* max sps num support */
    FY_S32  s32MaxPpsNum;             /* max pps num support */
}H265_PRTCL_PARAM_S;

typedef struct fyVDEC_PRTCL_PARAM_S
{
    PAYLOAD_TYPE_E enType;                      /* video type to be decoded ,only h264 and HEVC supported*/
    union
    {
        H264_PRTCL_PARAM_S stH264PrtclParam;    /* protocol param structure for h264*/
        H265_PRTCL_PARAM_S stH265PrtclParam;    /* protocol param structure for HEVC*/
    };
}VDEC_PRTCL_PARAM_S;


typedef struct fyVDEC_CHN_POOL_S
{
    VB_POOL hPicVbPool;
    VB_POOL hPmvVbPool;
}VDEC_CHN_POOL_S;

typedef struct fyVDEC_MOD_PARAM_S
{
  FY_U32  u32MiniBufMode;
  FY_U32  u32VBSource;
} VDEC_MOD_PARAM_S;

/**********************************************************************
    - Error Code
**********************************************************************/

//vdec privite error number
#define VDEC_ERR_NUM_SLICE       (EN_ERR_BUTT + 1)

/* invlalid channel ID */
#define FY_ERR_VDEC_INVALID_CHNID     FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define FY_ERR_VDEC_ILLEGAL_PARAM     FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define FY_ERR_VDEC_EXIST             FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define FY_ERR_VDEC_NULL_PTR          FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define FY_ERR_VDEC_NOT_CONFIG        FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define FY_ERR_VDEC_NOT_SURPPORT      FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define FY_ERR_VDEC_NOT_PERM          FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define FY_ERR_VDEC_UNEXIST           FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define FY_ERR_VDEC_NOMEM             FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define FY_ERR_VDEC_NOBUF             FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define FY_ERR_VDEC_BUF_EMPTY         FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define FY_ERR_VDEC_BUF_FULL          FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define FY_ERR_VDEC_SYS_NOTREADY      FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define FY_ERR_VDEC_BUSY              FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define FY_ERR_VDEC_BADADDR           FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/*number of slices is more than max*/
#define FY_ERR_VDEC_NUM_SLICE             FY_DEF_ERR(FY_ID_VDEC, EN_ERR_LEVEL_ERROR, VDEC_ERR_NUM_SLICE)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __FY_COMM_VDEC_H__ */

