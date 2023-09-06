#ifndef __FY_COMM_VENC_H__
#define __FY_COMM_VENC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "fy_type.h"
#include "fy_common.h"
#include "fy_errno.h"
#include "fy_comm_video.h"
#include "fy_comm_rc.h"

/**********************************************************************
    PART I      - Macro
**********************************************************************/
#define VENC_MAX_CHAN_SIZE_MODE_CNT  8

/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/

/*the nalu type of H264E*/
typedef enum fyH264E_NALU_TYPE_E
{
     H264E_NALU_PSLICE = 1,                         /*PSLICE types*/
     H264E_NALU_ISLICE = 5,                         /*ISLICE types*/
     H264E_NALU_SEI    = 6,                         /*SEI types*/
     H264E_NALU_SPS    = 7,                         /*SPS types*/
     H264E_NALU_PPS    = 8,                         /*PPS types*/
     H264E_NALU_IPSLICE = 9,
     H264E_NALU_BUTT
} H264E_NALU_TYPE_E;

/*the nalu type of H265E*/
typedef enum fyH265E_NALU_TYPE_E
{
     H265E_NALU_PSLICE = 1,                         /*P SLICE types*/
     H265E_NALU_ISLICE = 19,                        /*I SLICE types*/
     H265E_NALU_VPS    = 32,                        /*VPS types*/
     H265E_NALU_SPS    = 33,                        /*SPS types*/
     H265E_NALU_PPS    = 34,                        /*PPS types*/
     H265E_NALU_SEI    = 39,                        /*SEI types*/

     H265E_NALU_BUTT
} H265E_NALU_TYPE_E;

/*the reference type of H264E slice*/
typedef enum fyH264E_REFSLICE_TYPE_E
{
     H264E_REFSLICE_FOR_1X = 1,                     /*Reference slice for H264E_REF_MODE_1X*/
     H264E_REFSLICE_FOR_2X = 2,                     /*Reference slice for H264E_REF_MODE_2X*/
     H264E_REFSLICE_FOR_4X = 5,                     /*Reference slice for H264E_REF_MODE_4X*/
     H264E_REFSLICE_FOR_BUTT                        /* slice not for reference*/
} H264E_REFSLICE_TYPE_E;

/*the pack type of JPEGE*/
typedef enum fyJPEGE_PACK_TYPE_E
{
     JPEGE_PACK_ECS      = 5,                       /*ECS types*/
     JPEGE_PACK_APP      = 6,                       /*APP types*/
     JPEGE_PACK_VDO      = 7,                       /*VDO types*/
     JPEGE_PACK_PIC      = 8,                       /*PIC types*/
     JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

typedef enum
{
    AVC_PROFILE_BASELINE = 0,
    AVC_PROFILE_MAIN     = 1,
    AVC_PROFILE_HIGH     = 2,
    AVC_PROFILE_SVC_T    = 3,
}AVC_PROFILE_IDC;

typedef enum fyH264E_REF_TYPE_E
{
    BASE_IDRSLICE = 0,                              /*the Idr frame at Base layer*/
    BASE_REFTOIDR = 1,
    BASE_PSLICE_REFBYBASE,                          /*the P frame at Base layer, referenced by other frames at Base layer*/
    BASE_PSLICE_REFBYENHANCE,                       /*the P frame at Base layer, referenced by other frames at Enhance layer*/
    ENHANCE_PSLICE_REFBYENHANCE,                    /*the P frame at Enhance layer, referenced by other frames at Enhance layer*/
    ENHANCE_PSLICE_NOTFORREF,                       /*the P frame at Enhance layer ,not referenced*/
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum fyVENC_FRAME_TYPE_E
{
    VENC_FRAME_TYPE_NONE = 1,
    VENC_FRAME_TYPE_IDR,
    VENC_FRAME_TYPE_BUTT
} VENC_FRAME_TYPE_E;

typedef enum fyH264E_IDR_PIC_ID_MODE_E
{
    H264E_IDR_PIC_ID_MODE_AUTO = 0,                 /*auto mode */
    H264E_IDR_PIC_ID_MODE_USR,                      /*user mode */
    H264E_IDR_PIC_ID_MODE_BUTT,
}H264E_IDR_PIC_ID_MODE_E;

typedef enum fyVENC_MODTYPE_E
{
    MODTYPE_VENC = 1,
    MODTYPE_H264E,
    MODTYPE_H265E,
    MODTYPE_JPEGE,
    MODTYPE_BUTT
} VENC_MODTYPE_E;

/* Externsion */
typedef enum fyVENC_STRM_BUF_MODE_E
{
    STRMBUF_MODE_STATIC_BUF = 0,                   /* static mode, stream buffer is allocated when first channel is created */
    STRMBUF_MODE_DYNAMIC_BUF = 1,                  /* dynamic mode, alloc/free stream buffer when create/destroy chan */
} VENC_STRM_BUF_MODE_E;

/* Externsion */
typedef struct fyVENC_CHAN_SIZE_MODE_S
{
    FY_U32 u32Width;                                /* channel pic width */
    FY_U32 u32Height;                               /* channel pic height */
    FY_U32 u32ChnNum;                               /* channel number */
    FY_U32 u32BufSize;                              /* channel stream buffer size */    
} VENC_CHAN_SIZE_MODE_S;

/* Externsion */
typedef struct fyVENC_MODPARAM_EXT_S
{
    FY_U32               u32SizeModeCnt;            /* number of channel size mode */
    VENC_CHAN_SIZE_MODE_S stSizeMode[VENC_MAX_CHAN_SIZE_MODE_CNT]; /* channel size mode info */
    VENC_STRM_BUF_MODE_E  enStrmBufMode;            /* stream buffer allocation mode */
    FY_BOOL              bOnlyH265;                 /* only h265 stream type */
    FY_U32               u32MaxPicWidth;            /* max pic width of all channels */
    FY_U32               u32MaxPicHeight;           /* max pic height of all channels */
}VENC_PARAM_MOD_EXT_S;


/*the data type of VENC*/
typedef union fyVENC_DATA_TYPE_U
{
    H264E_NALU_TYPE_E    enH264EType;               /*H264E NALU types*/
    JPEGE_PACK_TYPE_E    enJPEGEType;               /*JPEGE pack types*/
    H265E_NALU_TYPE_E    enH265EType;               /*H264E NALU types*/
}VENC_DATA_TYPE_U;

typedef struct fyVENC_PACK_INFO_S
{
    VENC_DATA_TYPE_U  u32PackType;
    FY_U32 u32PackOffset;
    FY_U32 u32PackLength;
}VENC_PACK_INFO_S;

typedef struct fyVENC_PACK_S
{
    FY_U32            u32PhyAddr;                   /*the physics address of stream*/
    FY_U8             *pu8Addr;                     /*the virtual address of stream*/
    FY_U32            u32Len;                       /*the length of stream*/

    FY_U64            u64PTS;                       /*PTS*/
    FY_BOOL           bFrameEnd;                    /*frame end*/

    VENC_DATA_TYPE_U  DataType;                     /*the type of stream*/
    FY_U32            u32Offset;

    FY_U32 u32DataNum;
    VENC_PACK_INFO_S  stPackInfo[8];
}VENC_PACK_S;

typedef enum fyH264E_REF_TYPE_E H265E_REF_TYPE_E;

typedef struct fyVENC_STREAM_INFO_H264_S
{
    FY_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    FY_U32 u32PSkipMbNum;                           /* the skip macroblock num */
    FY_U32 u32IpcmMbNum;                            /* the ipcm macroblock num */
    FY_U32 u32Inter16x8MbNum;                       /* the inter16x8 macroblock num */
    FY_U32 u32Inter16x16MbNum;                      /* the inter16x16 macroblock num */
    FY_U32 u32Inter8x16MbNum;                       /* the inter8x16 macroblock num */
    FY_U32 u32Inter8x8MbNum;                        /* the inter8x8 macroblock num */
    FY_U32 u32Intra16MbNum;                         /* the intra16x16 macroblock num */
    FY_U32 u32Intra8MbNum;                          /* the intra8x8 macroblock num */
    FY_U32 u32Intra4MbNum;                          /* the inter4x4 macroblock num */

    H264E_REFSLICE_TYPE_E enRefSliceType;           /*the reference type of H264E slice*/
    H264E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    FY_U32 u32UpdateAttrCnt;                        /*Not Support*/
    FY_U32 u32StartQp;                              /*StartQP Value*/
}VENC_STREAM_INFO_H264_S;

typedef struct fyVENC_STREAM_INFO_H265_S
{
    FY_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    FY_U32 u32Inter64x64CuNum;                      /* the Inter64x64 Cu num */
    FY_U32 u32Inter32x32CuNum;                      /* the Inter32x32 Cu num */
    FY_U32 u32Inter16x16CuNum;                      /* the Inter16x16 Cu num */
    FY_U32 u32Inter8x8CuNum;                        /* the Inter8x8 Cu num */
    FY_U32 u32Intra32x32CuNum;                      /* the Intra32x32 Cu num */
    FY_U32 u32Intra16x16CuNum;                      /* the Intra16x16 Cu num */
    FY_U32 u32Intra8x8CuNum;                        /* the Intra8x8 Cu num */
    FY_U32 u32Intra4x4CuNum;                        /* the Intra4x4 Cu num */

    H265E_REF_TYPE_E  enRefType;                    /*Type of encoded frames in advanced frame skipping reference mode*/
    FY_U32 u32UpdateAttrCnt;                        /*Not Support*/
    FY_U32 u32StartQp;                              /*StartQP Value*/
}VENC_STREAM_INFO_H265_S;

typedef struct fyVENC_STREAM_INFO_JPEG_S
{
    FY_U32 u32PicBytesNum;                          /* the coded picture stream byte number */
    FY_U32 u32UpdateAttrCnt;                        /* Not Support*/
    FY_U32 u32Qfactor;                              /* image quality */
}VENC_STREAM_INFO_JPEG_S;

typedef struct fyVENC_SSE_INFO_S
{
    FY_BOOL bSSEEn;
    FY_U32  u32SSEVal;  //SSE
} VENC_SSE_INFO_S;

typedef struct fyVENC_STREAM_ADVANCE_INFO_H264_S
{
    FY_U32          u32ResidualBitNum;              /* residual */
    FY_U32          u32HeadBitNum;                  /* head information */
    FY_U32          u32MadiVal;                     /*  madi */
    FY_U32          u32MadpVal;                     /* madp */
    FY_U64          u64PSNRVal;                     /* PSNR */
    FY_U32          u32MseLcuCnt;
    FY_U32          u32MseSum;
    VENC_SSE_INFO_S stSSEInfo[8];                   /*Not Support*/
    FY_U32          u32QpHstgrm[52];                /*Not Support*/
} VENC_STREAM_ADVANCE_INFO_H264_S;

typedef struct fyVENC_STREAM_ADVANCE_INFO_JPEG_S
{
    FY_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_JPEG_S;

typedef struct fyVENC_STREAM_ADVANCE_INFO_H265_S
{
    FY_U32          u32ResidualBitNum;              /* residual */
    FY_U32          u32HeadBitNum;                  /* head information */
    FY_U32          u32MadiVal;                     /*  madi */
    FY_U32          u32MadpVal;                     /* madp */
    FY_U64          u64PSNRVal;                     /* PSNR */

    FY_U32          u32MseLcuCnt;
    FY_U32          u32MseSum;

    VENC_SSE_INFO_S stSSEInfo[8];                   /*Not Support*/
    FY_U32          u32QpHstgrm[52];                /*Not Support*/
} VENC_STREAM_ADVANCE_INFO_H265_S;

typedef struct fyVENC_STREAM_S
{
    VENC_PACK_S *pstPack;                                           /*stream pack attribute*/
    FY_U32      u32PackCount;                                       /*the pack number of one frame stream*/
    FY_U32      u32Seq;                                             /*Not Support*/

    union
    {
        VENC_STREAM_INFO_H264_S  stH264Info;                        /*the stream info of h264*/
        VENC_STREAM_INFO_JPEG_S  stJpegInfo;                        /*the stream info of jpeg*/
        VENC_STREAM_INFO_H265_S  stH265Info;                        /*the stream info of h265*/
    };

    union
    {
        VENC_STREAM_ADVANCE_INFO_H264_S  stAdvanceH264Info;         /*the stream info of h264*/
        VENC_STREAM_ADVANCE_INFO_JPEG_S  stAdvanceJpegInfo;         /*the stream info of jpeg*/
        VENC_STREAM_ADVANCE_INFO_H265_S  stAdvanceH265Info;        /*the stream info of h265*/
    };
} VENC_STREAM_S;

typedef struct fyVENC_ATTR_H264_S
{
    FY_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    FY_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/

    FY_U32  u32BufSize;                             /*stream buffer size*/
    FY_U32  u32Profile;                             /*0: baseline; 1:MP; 2:HP;*/
    FY_BOOL bByFrame;                               /*only support frame mode*/

    FY_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    FY_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
}VENC_ATTR_H264_S;

typedef struct fyVENC_ATTR_H265_S
{
    FY_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    FY_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/

    FY_U32  u32BufSize;                             /*stream buffer size*/
    FY_U32  u32Profile;                             /*0: MP */
    FY_BOOL bByFrame;                               /*only support frame mode*/

    FY_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    FY_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
}VENC_ATTR_H265_S;

typedef struct fyVENC_ATTR_MJPEG_S
{
    FY_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    FY_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    FY_U32  u32BufSize;                             /*stream buffer size*/
    FY_BOOL bByFrame;                               /*only support frame mode*/

    FY_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    FY_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/

}VENC_ATTR_MJPEG_S;

typedef struct fyVENC_ATTR_JPEG_S
{
    FY_U32  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    FY_U32  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    FY_U32  u32BufSize;                             /*stream buffer size*/
    FY_BOOL bByFrame;                               /*only support frame mode*/

    FY_U32  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    FY_U32  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
    FY_BOOL bSupportDCF;                            /*Not Support*/
}VENC_ATTR_JPEG_S;

typedef struct fyVENC_ATTR_S
{
    PAYLOAD_TYPE_E  enType;                         /*the type of payload*/

    union
    {
        VENC_ATTR_H264_S  stAttrH264e;              /*attributes of h264*/
        VENC_ATTR_MJPEG_S stAttrMjpeg;              /*Not Support*/
        VENC_ATTR_JPEG_S  stAttrJpeg;               /*attributes of jpeg*/
        VENC_ATTR_H265_S  stAttrH265e;              /*attributes of h265*/
    };

    /* Extension Members */
    FY_BOOL  bEnableSmart;                          /* enable smart encode*/
    FY_U32   u32SkipMode;                           /* 0: no skip; 1: single layer skip; 2: double layer skip */
}VENC_ATTR_S;

typedef struct fyVENC_CHN_ATTR_S
{
    VENC_ATTR_S stVeAttr;                           /*the attribute of video encoder*/
    VENC_RC_ATTR_S stRcAttr;                        /*the attribute of rate  ctrl*/
}VENC_CHN_ATTR_S;

typedef struct fyVENC_CHN_STAT_S
{
    FY_U32 u32LeftPics;                             /*left picture number */
    FY_U32 u32LeftStreamBytes;                      /*left stream bytes*/
    FY_U32 u32LeftStreamFrames;                     /*left stream frames*/
    FY_U32 u32CurPacks;                             /*pack number of current frame*/
    FY_U32 u32LeftRecvPics;                         /*Number of frames to be received. This member is valid after FY_MPI_VENC_StartRecvPicEx is called.*/
    FY_U32 u32LeftEncPics;                          /*Number of frames to be encoded. This member is valid after FY_MPI_VENC_StartRecvPicEx is called.*/

    /* Extension Members */
    FY_U32 u32EncPics;                              /*encoded picture number*/
    FY_U32 u32LostPics;                             /*discarded picture number*/
    FY_U32 u32LastQp;                               /*the qp value of latest encoded frame */
    FY_U32 u32LastIQp;                              /*the qp value of latest encoded i frame */
    FY_U32 u32Bps;                                  /*the average bitrate of encoded frames */
}VENC_CHN_STAT_S;

typedef struct fyVENC_PARAM_H264_SLICE_SPLIT_S
{
    FY_BOOL bSplitEnable;                           /*slice split enable, FY_TRUE:enable, FY_FALSE:diable, default value:FY_FALSE*/
    FY_U32  u32SplitMode;                           /*0:bit number, 1:mb line number, >=2:no meaning*/
    FY_U32  u32SliceSize;                           /*when the splitmode is 0, this value presents the bitsnum of one slice average,
                                                      when the splitmode is 1, this value presents the mb num of one slice*/
} VENC_PARAM_H264_SLICE_SPLIT_S;

typedef struct fyVENC_PARAM_H264_INTER_PRED_S
{
    FY_U32 u32HWSize;                              /* size of horizontal search window.
                                                      default value: differ with the picture size */
    FY_U32 u32VWSize;                              /* size of vertical search window.
                                                    default value: differ with the picture size */

    FY_BOOL bInter16x16PredEn;                     /*default: FY_TRUE, enable 16x16 prediction*/
    FY_BOOL bInter16x8PredEn;                      /*default: FY_TRUE*/
    FY_BOOL bInter8x16PredEn;                      /*default: FY_TRUE*/
    FY_BOOL bInter8x8PredEn;                       /*default: FY_TRUE*/
    FY_BOOL bExtedgeEn;                            /*default: FY_TRUE*/
} VENC_PARAM_H264_INTER_PRED_S;

typedef struct fyVENC_PARAM_H264_INTRA_PRED_S
{
    FY_BOOL bIntra16x16PredEn;                     /*default: FY_TRUE, enable 16x16 intra prediction*/
    FY_BOOL bIntraNxNPredEn;                       /*default: FY_TRUE, enable 4x4 and/or 8x8 prediction
                                                     Notes: this value must work with IntraTransMode*/
    FY_U32 constrained_intra_pred_flag;            /*default: FY_FALSE, see the H.264 protocol*/
    FY_BOOL bIpcmEn;                               /*default: FY_TRUE, enable ipcm*/
}VENC_PARAM_H264_INTRA_PRED_S;

typedef struct fyVENC_PARAM_H264_TRANS_S
{
    FY_U32 u32IntraTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    FY_U32 u32InterTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */

    FY_BOOL bScalingListValid;                     /* default: FY_FALSE */
                                                   /* Notes: Enable IntraScalingList8X8 and InterScalingList8X8 while bScalingListValid is FY_TRUE */
    FY_U8  InterScalingList8X8[64];
    FY_U8  IntraScalingList8X8[64];

    FY_S32 chroma_qp_index_offset;                 /* [-12,12],default value: 0*/
}VENC_PARAM_H264_TRANS_S;

typedef struct fyVENC_PARAM_H264_ENTROPY_S
{
    FY_U32 u32EntropyEncModeI;                     /* 0:cavlc, 1:cabac */
    FY_U32 u32EntropyEncModeP;                     /* 0:cavlc, 1:cabac */
    FY_U32 cabac_stuff_en;                         /* default: 0*/
    FY_U32 Cabac_init_idc;                         /* 0~2 */
}VENC_PARAM_H264_ENTROPY_S;

typedef struct fyVENC_PARAM_H264_POC_S
{
    FY_U32 pic_order_cnt_type;                     /* default value: 2. {0,1,2} */

}VENC_PARAM_H264_POC_S;

typedef struct fyVENC_PARAM_H264_DBLK_S
{
    FY_U32 disable_deblocking_filter_idc;          /* default value: 0. {0,1,2} */
    FY_S32 slice_alpha_c0_offset_div2;             /* default value: 5. [-6,+6] */
    FY_S32 slice_beta_offset_div2;                 /* default value: 5. [-6,+6] */
}VENC_PARAM_H264_DBLK_S;

typedef struct fyVENC_PARAM_H264_VUI_TIME_INFO_S
{
    FY_U8  timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    FY_U8  fixed_frame_rate_flag;                  /* default value: n/a. {0,1} */
    FY_U32 num_units_in_tick;                      /* default value: n/a. > 0. */
    FY_U32 time_scale;                             /* default value: n/a. > 0. */
}VENC_PARAM_VUI_H264_TIME_INFO_S;

typedef struct fyVENC_PARAM_VUI_ASPECT_RATIO_S
{
    FY_U8  aspect_ratio_info_present_flag;        /* default value: 0. If 1, aspectratio info belows will be encoded into vui. {0,1} */
    FY_U8  aspect_ratio_idc;                      /* default value: n/a. [0,255],17~254 is reserved. */
    FY_U8  overscan_info_present_flag;            /* default value: 0,just be 0.If 1, oversacan info belows will be encoded into vui. {0,1} */
    FY_U8  overscan_appropriate_flag;             /* default value: n/a. */
    FY_U16 sar_width;                             /* default value: n/a. sar_width  and  sar_height  shall  be  relatively  prime. */
    FY_U16 sar_height ;                           /* default value: n/a.  */
}VENC_PARAM_VUI_ASPECT_RATIO_S;

typedef struct fyVENC_PARAM_VUI_VIDEO_SIGNAL_S
{
    FY_U8  video_signal_type_present_flag ;        /* default value: 0. If 1, video singnal info will be encoded into vui. {0,1} */
    FY_U8  video_format ;                          /* default value: n/a. >= 0. */
    FY_U8  video_full_range_flag;                  /* default value: n/a. {0,1}. */
    FY_U8  colour_description_present_flag ;       /* default value: n/a. {0,1} */
    FY_U8  colour_primaries ;                      /* default value: n/a. [0,255],0/9~255 is reserved */
    FY_U8  transfer_characteristics;               /* default value: n/a. [0,255],0/13~255 is reserved */
    FY_U8  matrix_coefficients;                    /* default value: n/a. [0,255],9~255 is reserved */
}VENC_PARAM_VUI_VIDEO_SIGNAL_S;

typedef struct fyVENC_PARAM_VUI_BITSTREAM_RESTRIC_S
{
    FY_U8  bitstream_restriction_flag ;
}VENC_PARAM_VUI_BITSTREAM_RESTRIC_S;

typedef struct fyVENC_PARAM_H264_VUI_S
{
    VENC_PARAM_VUI_ASPECT_RATIO_S       stVuiAspectRatio;
    VENC_PARAM_VUI_H264_TIME_INFO_S     stVuiTimeInfo;
    VENC_PARAM_VUI_VIDEO_SIGNAL_S       stVuiVideoSignal;
    VENC_PARAM_VUI_BITSTREAM_RESTRIC_S  stVuiBitstreamRestric;
}VENC_PARAM_H264_VUI_S;

typedef struct fyVENC_PARAM_H265_TIME_INFO_S
{
    FY_U32 timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    FY_U32 num_units_in_tick;                      /* default value: 1. > 0. */
    FY_U32 time_scale;                             /* default value: 60. > 0. */
    FY_U32 num_ticks_poc_diff_one_minus1;          /* default value: 1. */
}VENC_PARAM_VUI_H265_TIME_INFO_S;

typedef struct fyVENC_PARAM_JPEG_S
{
    FY_U32 u32Qfactor;                             /*image quality :[1,99]*/

    FY_U8  u8YQt[64];                              /* y qt value */
    FY_U8  u8CbQt[64];                             /* cb qt value */
    FY_U8  u8CrQt[64];                             /* cr qt value */

    FY_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_JPEG_S;

typedef struct fyVENC_PARAM_MJPEG_S
{
    FY_U8 u8YQt[64];                               /* y qt value */
    FY_U8 u8CbQt[64];                              /* cb qt value */
    FY_U8 u8CrQt[64];                              /* cr qt value */

    FY_U32 u32MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_MJPEG_S;

/* ROI struct */
typedef struct fyVENC_ROI_CFG_S
{
    FY_U32  u32Index;                              /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
    FY_BOOL bEnable;                               /* Whether to enable this ROI */
    FY_BOOL bAbsQp;                                /* QP mode of an ROI.FY_FALSE: relative QP.FY_TURE: absolute QP.*/
    FY_S32  s32Qp;                                 /* QP value. */
    RECT_S  stRect;                                /* Region of an ROI*/
}VENC_ROI_CFG_S;

/* ROI struct */
typedef struct fyVENC_ROI_CFG_EX_S
{
    FY_U32  u32Index;                              /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
    FY_BOOL bEnable[3];                            /* Subscript of array   0: I Frame; 1: P/B Frame; 2: VI Frame; other params are the same. */
    FY_BOOL bAbsQp[3];                             /* QP mode of an ROI.FY_FALSE: relative QP.FY_TURE: absolute QP.*/
    FY_BOOL  s32Qp[3];                              /* QP value. */
    RECT_S  stRect[3];                             /* Region of an ROI*/
}VENC_ROI_CFG_EX_S;

typedef struct fyVENC_ROIBG_FRAME_RATE_S
{
    FY_S32 s32SrcFrmRate;                          /* Input frame rate of   Roi backgroud*/
    FY_S32 s32DstFrmRate;                          /* Output frame rate of  Roi backgroud */
}VENC_ROIBG_FRAME_RATE_S;

typedef struct fyVENC_PARAM_H265_VUI_S
{
    VENC_PARAM_VUI_ASPECT_RATIO_S       stVuiAspectRatio;
    VENC_PARAM_VUI_VIDEO_SIGNAL_S       stVuiVideoSignal;
    VENC_PARAM_VUI_H265_TIME_INFO_S     stVuiTimeInfo;
    VENC_PARAM_VUI_BITSTREAM_RESTRIC_S  stVuiBitstreamRestric;
}VENC_PARAM_H265_VUI_S;

typedef struct fyVENC_COLOR2GREY_S
{
    FY_BOOL bColor2Grey;                           /* Whether to enable Color2Grey.*/
}VENC_COLOR2GREY_S;

typedef struct fyVENC_CROP_CFG_S
{
    FY_BOOL bEnable;                               /* Crop region enable */
    RECT_S  stRect;                                /* Crop region, note: s32X must be multi of 16 */
}VENC_CROP_CFG_S;


typedef struct fyVENC_FRAME_RATE_S
{
    FY_S32 s32SrcFrmRate;                          /* Input frame rate of a  channel*/
    FY_S32 s32DstFrmRate;                          /* Output frame rate of a channel*/
} VENC_FRAME_RATE_S;

typedef struct fyVENC_PARAM_REF_S
{
    FY_U32       u32Base;                          /* Not Support */
    FY_U32       u32Enhance;                       /* Not Support */
    FY_BOOL      bEnablePred;                      /* Not Support */

    /* Extension Members */
    FY_U32       u32Mode;                          /*skip mode*/
    FY_U32       u32SvcM;                          /*skip frame number of first layer*/
    FY_U32       u32SvcN;                          /*skip frame number of second layer*/
} VENC_PARAM_REF_S;

typedef struct fyVENC_PARAM_INTRA_REFRESH_S
{
    FY_BOOL bRefreshEnable;
    FY_BOOL bISliceEnable;
    FY_U32  u32RefreshLineNum;
    FY_U32  u32ReqIQp;
    FY_U32  u32RefreshStep;                        /*Number of refreshed step in the I slice*/
}VENC_PARAM_INTRA_REFRESH_S;

typedef struct fyVENC_PARAM_REF_EX_S
{
    FY_U32 u32Base;                               /*Base layer period*/
    FY_U32 u32Enhance;                            /*Enhance layer period*/
    FY_BOOL bEnablePred;                          /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is FY_FALSE, all frames at the base layer reference IDR frames.*/

    FY_BOOL bVirtualIEnable;
    FY_U32 u32VirtualIInterval;
    FY_S32 s32VirtualIQpDelta;
} VENC_PARAM_REF_EX_S;

typedef struct fyUSER_RC_INFO_S
{
    FY_U32 u32BlkStartQp;
    FY_U32 u32QpMapPhyAddr;
    VENC_FRAME_TYPE_E enFrameType;
} USER_RC_INFO_S;

typedef struct fyUSER_FRAME_INFO_S
{
    VIDEO_FRAME_INFO_S stUserFrame;
    USER_RC_INFO_S     stUserRcInfo;
} USER_FRAME_INFO_S;

typedef struct fyVENC_H264_IDRPICID_CFG_S
{
    H264E_IDR_PIC_ID_MODE_E enH264eIdrPicIdMode;   /*the mode of idr_pic_id that is set*/
    FY_U32 u32H264eIdrPicId;                       /*idr_pic_id value*/
} VENC_H264_IDRPICID_CFG_S;

typedef struct fyVENC_RECV_PIC_PARAM_S
{
    FY_S32 s32RecvPicNum;                          /*Number of frames received and encoded by the encoding channel*/
} VENC_RECV_PIC_PARAM_S;

typedef struct fyVENC_STREAM_BUF_INFO_S
{
    FY_U32   u32PhyAddr;
    FY_VOID *pUserAddr;
    FY_U32   u32BufSize;
} VENC_STREAM_BUF_INFO_S;

typedef struct fyVENC_PARAM_H265_SLICE_SPLIT_S
{
    FY_BOOL bSplitEnable;                          /* default value: FY_FALSE, means no slice split*/
    FY_U32  u32SplitMode;                          /* 0: by bits number; 1: by lcu line number*/
    FY_U32  u32SliceSize;                          /* when splitmode is 0, this value presents bits number,
                                                      when splitmode is 1, this value presents lcu line number */
    FY_U32  loop_filter_across_slices_enabled_flag;
} VENC_PARAM_H265_SLICE_SPLIT_S;

typedef struct fyVENC_PARAM_H265_PU_S
{
    FY_BOOL bPu32x32En;                            /*default: FY_TRUE */
    FY_BOOL bPu16x16En;                            /*default: FY_TRUE */
    FY_BOOL bPu8x8En;                              /*default: FY_TRUE */
    FY_BOOL bPu4x4En;                              /*default: FY_TRUE */

    FY_U32  constrained_intra_pred_flag;           /*default: FY_FALSE*/
    FY_U32  strong_intra_smoothing_enabled_flag;   /*INTRA_SMOOTH*/
    FY_U32  pcm_enabled_flag;                      /*default: FY_FALSE, enable ipcm*/
    FY_U32  pcm_loop_filter_disabled_flag;

    FY_U32  u32MaxNumMergeCand;
}VENC_PARAM_H265_PU_S;

typedef struct fyVENC_PARAM_H265_TRANS_S
{
    FY_U32 transquant_bypass_enabled_flag;
    FY_U32 transform_skip_enabled_flag;
    FY_S32 cb_qp_offset;
    FY_S32 cr_qp_offset;

    FY_BOOL bScalingListTu4Valid;                  /* default: FY_FALSE */
    FY_U8   InterScalingList4X4[2][16];
    FY_U8   IntraScalingList4X4[2][16];

    FY_BOOL bScalingListTu8Valid;                  /* default: FY_FALSE */
    FY_U8  InterScalingList8X8[2][64];
    FY_U8  IntraScalingList8X8[2][64];

    FY_BOOL bScalingListTu16Valid;
    FY_U8 IntraTu16Dc[2];
    FY_U8 InterTu16Dc[2];
    FY_U8  InterScalingList16X16[2][64];
    FY_U8  IntraScalingList16X16[2][64];

    FY_BOOL bScalingListTu32Valid;
    FY_U8   IntraTu32Dc;
    FY_U8   InterTu32Dc;
    FY_U8   InterScalingList32X32[64];
    FY_U8   IntraScalingList32X32[64];
} VENC_PARAM_H265_TRANS_S;

typedef struct fyVENC_PARAM_H265_ENTROPY_S
{
    FY_U32 cabac_init_flag;                         /* default: 1*/
}VENC_PARAM_H265_ENTROPY_S;

typedef struct fyVENC_PARAM_H265_DBLK_S
{
    FY_U32 slice_deblocking_filter_disabled_flag;  /* default value: 0. {0,1} */
    FY_S32 slice_beta_offset_div2;                 /* default value: 0. [-6,+6] */
    FY_S32 slice_tc_offset_div2;                   /* default value: 0. [-6,+6] */
}VENC_PARAM_H265_DBLK_S;

typedef struct fyVENC_PARAM_H265_SAO_S
{
    FY_U32  slice_sao_luma_flag;
    FY_U32  slice_sao_chroma_flag;
}VENC_PARAM_H265_SAO_S;

typedef struct fyVENC_PARAM_H265_TIMING_S
{
    FY_S32 timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    FY_S32 num_units_in_tick;                      /* default value: 1. > 0. */
    FY_S32 time_scale;                             /* default value: 60. > 0. */
    FY_U32 num_ticks_poc_diff_one;                 /* default value: 1.  */
}VENC_PARAM_H265_TIMING_S;

typedef struct fyVENC_PARAM_MOD_H264E_S
{
    FY_U32  u32OneStreamBuffer;
    FY_U32  u32H264eVBSource;
    FY_U32  u32H264eRcnEqualRef;
    FY_U32  u32H264eMiniBufMode;
} VENC_PARAM_MOD_H264E_S;

typedef struct fyVENC_PARAM_MOD_H265E_S
{
    FY_U32  u32OneStreamBuffer;
    FY_U32  u32H265eMiniBufMode;
} VENC_PARAM_MOD_H265E_S;

typedef struct fyVENC_PARAM_MOD_JPEGE_S
{
    FY_U32  u32OneStreamBuffer;
    FY_U32  u32JpegeMiniBufMode;
} VENC_PARAM_MOD_JPEGE_S;


typedef struct fyVENC_PARAM_MOD_VENC_S
{
    FY_U32 u32VencBufferCache;
} VENC_PARAM_MOD_VENC_S;

typedef struct fyVENC_MODPARAM_S
{
    VENC_MODTYPE_E enVencModType;
    union
    {
       VENC_PARAM_MOD_VENC_S stVencModParam;
       VENC_PARAM_MOD_H264E_S stH264eModParam;
       VENC_PARAM_MOD_H265E_S stH265eModParam;
       VENC_PARAM_MOD_JPEGE_S stJpegeModParam;
    };
} VENC_PARAM_MOD_S;

typedef struct fhVENC_VB_POOL_S
{
    FY_S32 s32VbPoolID;
} VB_POOL_S;

typedef struct fyVENC_SSE_CFG_S
{
    FY_U32  u32Index;
    FY_BOOL bEnable;
    RECT_S  stRect;
} VENC_SSE_CFG_S;


/************************************************************************************************
    PART III    - Error Code
*************************************************************************************************/
#define FY_ERR_VENC_INVALID_CHNID               FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define FY_ERR_VENC_ILLEGAL_PARAM               FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define FY_ERR_VENC_EXIST                       FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define FY_ERR_VENC_UNEXIST                     FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define FY_ERR_VENC_NULL_PTR                    FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define FY_ERR_VENC_NOT_CONFIG                  FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define FY_ERR_VENC_NOT_SUPPORT                 FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define FY_ERR_VENC_NOT_PERM                    FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define FY_ERR_VENC_NOMEM                       FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define FY_ERR_VENC_NOBUF                       FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define FY_ERR_VENC_BUF_EMPTY                   FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define FY_ERR_VENC_BUF_FULL                    FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define FY_ERR_VENC_SYS_NOTREADY                FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define FY_ERR_VENC_BUSY                        FY_DEF_ERR(FY_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_COMM_VENC_H__ */
