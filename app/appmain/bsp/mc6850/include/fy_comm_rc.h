#ifndef __FY_COMM_RC_H__
#define __FY_COMM_RC_H__

#include "fy_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/**********************************************************************
    PART I      - Macro
**********************************************************************/

/**********************************************************************
    PART II     - Structure and Enumeration
**********************************************************************/

typedef FY_U32 FY_FR32;

typedef enum fyVENC_RC_MODE_E
{
    VENC_RC_MODE_H264CBR = 1,
    VENC_RC_MODE_H264VBR,
    VENC_RC_MODE_H264AVBR,
    VENC_RC_MODE_H264QVBR,
    VENC_RC_MODE_H264FIXQP,
    VENC_RC_MODE_H264QPMAP,

    VENC_RC_MODE_H265CBR,
    VENC_RC_MODE_H265VBR,
    VENC_RC_MODE_H265AVBR,
    VENC_RC_MODE_H265QVBR,
    VENC_RC_MODE_H265FIXQP,
    VENC_RC_MODE_H265QPMAP,

    VENC_RC_MODE_BUTT,

}VENC_RC_MODE_E;

typedef enum fyRC_SUPERFRM_MODE_E
{
    SUPERFRM_NONE=0,                                /* sdk don't care super frame */
    SUPERFRM_DISCARD,                               /* the super frame is discarded */
    SUPERFRM_REENCODE,                              /* Not Support */
    SUPERFRM_BUTT
}VENC_SUPERFRM_MODE_E;

typedef enum fyVENC_FRAMELOST_MODE_E
{
    FRMLOST_NORMAL=0,                               /*normal mode*/
    FRMLOST_PSKIP,                                  /*pskip  mode*/
    FRMLOST_BUTT,
}VENC_FRAMELOST_MODE_E;

typedef enum fyVENC_RC_PRIORITY_E
{
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,             /* BitRate is prior to super frame */
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,               /* Super frame  is prior to bitRate */

    VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;

typedef enum fyVENC_RC_QPMAP_MODE_E
{
    VENC_RC_QPMAP_MODE_MEANQP= 0,
    VENC_RC_QPMAP_MODE_MINQP ,
    VENC_RC_QPMAP_MODE_MAXQP,

    VENC_RC_QPMAP_MODE_BUTT,
}VENC_RC_QPMAP_MODE_E;

typedef struct fyVENC_ATTR_H264_FIXQP_S
{
    FY_U32      u32Gop;                             /*the interval of ISLICE. */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    FY_U32      u32IQp;                             /* qp of the i frame */
    FY_U32      u32PQp;                             /* qp of the p frame */
    FY_U32      u32BQp;                             /* qp of the b frame */
} VENC_ATTR_H264_FIXQP_S;

typedef struct fyVENC_ATTR_H264_CBR_S
{
    FY_U32      u32Gop;                             /*the interval of ISLICE. */
    FY_U32      u32StatTime;                        /* Not Support */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    FY_U32      u32BitRate;                         /* average bitrate */
    FY_U32      u32FluctuateLevel;                  /* level [0..6].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */

    /* Extension Members */
    FY_U32      u32InitQP;                          /* initial QP value */
    FY_U32      u32MaxRatePercent;                  /* the max percent of target bitrate for one frame*/
    FY_U32      u32IFrmMaxBits;                     /* the max bit rate of I frame */
    FY_S32      s32IPQpDelta;                       /* the qp difference between the i frame and p frame, pqp >= iqp + IP_QPDelta*/
    FY_S32      s32IBitProp;                        /* the ratio of i frame */
    FY_S32      s32PBitProp;                        /* the ratio of p frame */
} VENC_ATTR_H264_CBR_S;


typedef struct fyVENC_ATTR_H264_VBR_S
{
    FY_U32      u32Gop;                             /*the interval of ISLICE. */
    FY_U32      u32StatTime;                        /* Not Support */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    FY_U32      u32MaxBitRate;                      /* the max bitrate */
    FY_U32      u32MaxQp;                           /* the max qp */
    FY_U32      u32MinQp;                           /* the min qp */
    FY_U32      u32MinIQp;                          /* min qp for i frame */

    /* Extension Members */
    FY_U32      u32MaxIQp;                          /* the max QP value of I frame */
    FY_U32      u32InitQp;                          /* initial QP value */
    FY_U32      u32MaxRatePercent;                  /* the max percent of target bitrate for one frame*/
    FY_U32      u32IFrmMaxBits;                     /* the max bit rate of I frame */
    FY_S32      s32IPQpDelta;                       /* the qp difference between the i frame and p frame, pqp >= iqp + IP_QPDelta*/
    FY_S32      s32IBitProp;                        /* the ratio of i frame */
    FY_S32      s32PBitProp;                        /* the ratio of p frame */
    FY_U32      u32FluctuateLevel;                  /* level [0..6].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
}VENC_ATTR_H264_VBR_S;

typedef struct fyVENC_ATTR_H264_AVBR_S
{
    FY_U32      u32Gop;                             /*the interval of ISLICE. */
    FY_U32      u32StatTime;                        /* Not Support */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    FY_U32      u32MaxBitRate;                      /* the max bitrate */

    /* Extension Members */
    FY_U32      u32MaxQp;                           /* the max QP value of P frame */
    FY_U32      u32MinQp;                           /* the min QP value of P frame */
    FY_U32      u32MinIQp;                          /* the min QP value of I frame */
    FY_U32      u32MaxIQp;                          /* the max QP value of I frame */
    FY_U32      u32InitQp;                          /* initial QP value */
    FY_U32      u32MaxRatePercent;                  /* the max percent of target bitrate for one frame*/
    FY_U32      u32IFrmMaxBits;                     /* the max bit rate of I frame */
    FY_S32      s32IPQpDelta;                       /* the qp difference between the i frame and p frame, pqp >= iqp + IP_QPDelta*/
    FY_S32      s32IBitProp;                        /* the ratio of i frame */
    FY_S32      s32PBitProp;                        /* the ratio of p frame */
    FY_U32      u32FluctuateLevel;                  /* level [0..6].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
    FY_U32      u32StillRatePercent;                /* the min percent of target bitrate for still scene */
    FY_U32      u32MaxStillQp;                      /* the max QP value for still scene*/
}VENC_ATTR_H264_AVBR_S;

typedef struct fyVENC_ATTR_H264_CBR_S   VENC_ATTR_H265_CBR_S;
typedef struct fyVENC_ATTR_H264_VBR_S   VENC_ATTR_H265_VBR_S;
typedef struct fyVENC_ATTR_H264_AVBR_S   VENC_ATTR_H265_AVBR_S;
typedef struct fyVENC_ATTR_H264_FIXQP_S VENC_ATTR_H265_FIXQP_S;


typedef struct fyVENC_ATTR_MJPEG_FIXQP_S
{
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate;                     /* the target frame rate of the venc chnnel */
    FY_U32      u32Qfactor;                         /* image quality :[1,99]*/
}VENC_ATTR_MJPEG_FIXQP_S;

typedef struct fyVENC_ATTR_MJPEG_CBR_S
{
    FY_U32      u32StatTime;                        /* the rate statistic time, the unit is senconds(s) */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    FY_U32      u32BitRate;                         /* average bitrate */
    FY_U32      u32FluctuateLevel;                  /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
} VENC_ATTR_MJPEG_CBR_S;

typedef struct fyVENC_ATTR_MJPEG_VBR_S
{
     FY_U32      u32StatTime;                       /* the rate statistic time, the unit is senconds(s) */
     FY_U32      u32SrcFrmRate;                     /* the input frame rate of the venc chnnel */
     FY_FR32     fr32DstFrmRate;                    /* the target frame rate of the venc chnnel */
     FY_U32      u32MaxBitRate;                     /* max bitrate */
     FY_U32      u32MaxQfactor;                     /* max image quailty allowed */
     FY_U32      u32MinQfactor;                     /* min image quality allowed */
 }VENC_ATTR_MJPEG_VBR_S;

typedef struct fyVENC_ATTR_H264_QPMAP_S{
    FY_U32      u32Gop;                             /*the interval of ISLICE. */
    FY_U32      u32StatTime;                        /* the rate statistic time, the unit is senconds(s) */
    FY_U32      u32SrcFrmRate;                      /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                    /* the target frame rate of the venc chnnel */
    VENC_RC_QPMAP_MODE_E enQpMapMode;
    FY_BOOL     bQpMapAbsQp;                        /* QP mode of an QpMap.FY_FALSE: relative QP.FY_TURE: absolute QP.*/
    FY_U32      u32Reserved;                        /* reserved */
  }VENC_ATTR_H264_QPMAP_S;

typedef struct fyVENC_ATTR_H264_QVBR_S
{
    FY_U32      u32Gop;                            /*the interval of ISLICE. */
    FY_U32      u32StatTime;                       /* the rate statistic time, the unit is senconds(s) */
    FY_U32      u32SrcFrmRate;                     /* the input frame rate of the venc chnnel */
    FY_FR32     fr32DstFrmRate ;                   /* the target frame rate of the venc chnnel */
    FY_U32      u32TargetBitRate;                  /* the target bitrate */

    /* Extension Members */
    FY_U32      u32InitQp;
    FY_U32      u32MaxRatePercent;                  /* the max percent of target bitrate for one frame*/
    FY_U32      u32IFrmMaxBits;                     /* the max bit rate of I frame */
    FY_S32      s32IPQpDelta;                       /* the qp difference between the i frame and p frame, pqp >= iqp + IP_QPDelta*/
    FY_S32      s32IBitProp;                        /* the ratio of i frame */
    FY_S32      s32PBitProp;                        /* the ratio of p frame */
    FY_U32      u32FluctuateLevel;                  /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
    FY_S32      s32BitPercentUL;
    FY_S32      s32BitPercentLL;
    FY_S32      s32PsnrFluctuateUL;
    FY_S32      s32PsnrFluctuateLL;
}VENC_ATTR_H264_QVBR_S;

typedef struct fyVENC_ATTR_H264_QVBR_S  VENC_ATTR_H265_QVBR_S;
typedef struct fyVENC_ATTR_H264_QPMAP_S VENC_ATTR_H265_QPMAP_S;

typedef struct fyVENC_RC_ATTR_S
{
    VENC_RC_MODE_E enRcMode;            /*the type of rc*/
    union
    {
        VENC_ATTR_H264_CBR_S    stAttrH264Cbr;
        VENC_ATTR_H264_VBR_S    stAttrH264Vbr;
		VENC_ATTR_H264_AVBR_S   stAttrH264AVbr;
		VENC_ATTR_H264_QVBR_S   stAttrH264QVbr;     /*Not Support*/
        VENC_ATTR_H264_FIXQP_S  stAttrH264FixQp;
		VENC_ATTR_H264_QPMAP_S  stAttrH264QpMap;    /*Not Support*/

        VENC_ATTR_MJPEG_CBR_S   stAttrMjpegeCbr;    /*Not Support*/
        VENC_ATTR_MJPEG_VBR_S   stAttrMjpegeVbr;    /*Not Support*/
        VENC_ATTR_MJPEG_FIXQP_S stAttrMjpegeFixQp;  /*Not Support*/

        VENC_ATTR_H265_CBR_S    stAttrH265Cbr;
        VENC_ATTR_H265_VBR_S    stAttrH265Vbr;
        VENC_ATTR_H265_AVBR_S   stAttrH265AVbr;
        VENC_ATTR_H265_QVBR_S   stAttrH265QVbr;     /*Not Support*/
        VENC_ATTR_H265_FIXQP_S  stAttrH265FixQp;
		VENC_ATTR_H265_QPMAP_S  stAttrH265QpMap;    /*Not Support*/
    };
    FY_VOID*       pRcAttr ;                        /*the rc attribute which could be specified by user*/
}VENC_RC_ATTR_S;

typedef struct fyVENC_PARAM_H264_CBR_S
{
    FY_U32  u32MinIprop;                            /* the min ratio of i frame and p frame */
    FY_U32  u32MaxIprop;                            /* the max ratio of i frame and p frame */
    FY_U32  u32MaxQp;                               /* the max QP value */
    FY_U32  u32MinQp;                               /* the min QP value */
    FY_S32  s32IPQPDelta;                           /* the qp difference between the i frame and the before gop avarage qp; == Qp(P) - Qp(I) */
    FY_S32  s32QualityLevel;                        /* quality of picture [1, 5] */
    FY_S32  s32MaxReEncodeTimes;                    /* max number of re-encode times [0, 3]*/
    FY_U32  u32MinIQp;                              /* min qp for I frame */
    FY_U32  u32MaxIQp;                              /* max qp for I frame */
}VENC_PARAM_H264_CBR_S;

typedef struct fyVENC_PARAM_H264_VBR_S
{
    FY_S32 s32IPQPDelta;                            /* the qp difference between the i frame and the before gop avarage qp; == Qp(P) - Qp(I) */
    FY_S32 s32ChangePos;                            /* Indicates the ratio of the current bit rate to the maximum
                                                        bit rate when the QP value starts to be adjusted */
    FY_U32  u32MinIprop;                            /* the min ratio of i frame and p frame */
    FY_U32  u32MaxIprop;                            /* the max ratio of i frame and p frame */
    FY_U32  u32MinIQP;                              /* min qp for i frame */
}VENC_PARAM_H264_VBR_S;

typedef struct fyVENC_PARAM_H264_AVBR_S
{
    FY_S32  s32IPQPDelta;                           /* the qp difference between I frame and P frame; s32IPQPDelta == Qp(P) - Qp(I) */
    FY_S32  s32ChangePos;                           /* when bitrate >= (maxbitrate * s32ChangePos / 100), the QP value starts to be adjusted*/
    FY_U32  u32MinIprop;                            /* the min ratio of i frame and p frame */
    FY_U32  u32MaxIprop;                            /* the max ratio of i frame and p frame */

    FY_U32  u32MinIQp;                              /* the min QP value of I frame */
    FY_U32  u32MaxIQp;                              /* the max QP value of I frame */
    FY_U32  u32MinQp;                               /* the min QP value of P frame */
    FY_U32  u32MaxQp;                               /* the max QP value of P frame */

    FY_S32  s32MinStillPercent;                     /* the min percent of target bitrate for still scene */
    FY_U32  u32MaxStillQP;                          /* the max QP value of I frame for still scene*/
    FY_U32  u32MinStillPSNR;                        /* reserved */
    FY_S32  s32MaxReEncodeTimes;                    /* max number of re-encode times [0, 3]*/
} VENC_PARAM_H264_AVBR_S;

typedef struct fyVENC_PARAM_MJPEG_CBR_S
{
    FY_U32  u32MaxQfactor;                          /* the max Qfactor value*/
    FY_U32  u32MinQfactor;                          /* the min Qfactor value */
    FY_U32  u32RQRatio[RC_RQRATIO_SIZE];            /* the rate stabilization weight,
                                                        100-u32RQRatio[i] is the sequence quality stabilization weight */
}VENC_PARAM_MJPEG_CBR_S;

typedef struct fyVENC_PARAM_MJPEG_VBR_S
{
    FY_S32 s32DeltaQfactor;                         /* Indicates the maximum change of Qfactor values of frames
                                                         when the picture quality changes */
    FY_S32 s32ChangePos;                            /* Indicates the ratio of the current bit rate to the maximum
                                                         bit rate when the Qfactor value starts to be adjusted */
}VENC_PARAM_MJPEG_VBR_S;


typedef struct fyVENC_PARAM_H264_CBR_S   VENC_PARAM_H265_CBR_S;
typedef struct fyVENC_PARAM_H264_VBR_S   VENC_PARAM_H265_VBR_S;
typedef struct fyVENC_PARAM_H264_AVBR_S   VENC_PARAM_H265_AVBR_S;

/*Not Support*/
typedef struct fyVENC_RC_PARAM_S
{
    FY_U32 u32ThrdI[RC_TEXTURE_THR_SIZE];             /* just useful for h264/h265 for now */
    FY_U32 u32ThrdP[RC_TEXTURE_THR_SIZE];
    FY_U32 u32RowQpDelta;
    FY_S32 s32FirstFrameStartQp;
    union
    {
        VENC_PARAM_H264_CBR_S     stParamH264Cbr;
        VENC_PARAM_H264_VBR_S     stParamH264VBR;
        VENC_PARAM_H264_AVBR_S    stParamH264AVbr;
        VENC_PARAM_MJPEG_CBR_S    stParamMjpegCbr;
        VENC_PARAM_MJPEG_VBR_S    stParamMjpegVbr;
        VENC_PARAM_H265_CBR_S     stParamH265Cbr;
        VENC_PARAM_H265_VBR_S     stParamH265Vbr;
        VENC_PARAM_H265_AVBR_S    stParamH265AVbr;
    };

    FY_VOID* pRcParam;                              /*RC parameter which could be specified by usrer*/
}VENC_RC_PARAM_S;

typedef struct fyVENC_PARAM_FRAMELOST_S
{
    FY_BOOL bFrmLostOpen;                           /* Indicates whether to discard frames to ensure
                                                        stable bit rate when the instant bit rate is exceeded */
    FY_U32  u32FrmLostBpsThr;                       /* the instant bit rate threshold */
    VENC_FRAMELOST_MODE_E enFrmLostMode;            /*frame lost strategy*/
    FY_U32  u32EncFrmGaps;                          /*the gap of frame lost*/
}VENC_PARAM_FRAMELOST_S;

typedef struct fyVENC_SUPERFRAME_CFG_S
{
      VENC_SUPERFRM_MODE_E  enSuperFrmMode;         /* Indicates the mode of processing the super frame,[SUPERFRM_NONE,SUPERFRM_DISCARD,SUPERFRM_REENCODE]*/
      FY_U32  u32SuperIFrmBitsThr;                  /* Indicate the threshold of the super I frame
                                                           for enabling the super frame processing mode */
      FY_U32  u32SuperPFrmBitsThr;                  /* Indicate the threshold of the super P frame */
      FY_U32  u32SuperBFrmBitsThr;                  /* Indicate the threshold of the super B frame */
      VENC_RC_PRIORITY_E    enRcPriority;
}VENC_SUPERFRAME_CFG_S;

typedef struct fyVENC_RC_ADVPARAM_S
{
 	FY_U32 u32ClearStatAfterSetAttr;
}VENC_RC_ADVPARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_COMM_RC_H__ */
