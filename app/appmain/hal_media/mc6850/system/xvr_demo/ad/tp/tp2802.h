#ifndef __TP2802_H__
#define __TP2802_H__

//#include <linux/ioctl.h>

#define TP2802_VERSION_CODE KERNEL_VERSION(0, 7, 4)

enum {
    TP2802C = 0x0200,
    TP2802D = 0x0201,
    TP2804  = 0x0400,
    TP2806  = 0x0401,
    TP2822  = 0x2200,
    TP2823  = 0x2300,
    TP2834  = 0x2400,
    TP2833  = 0x2301,
    TP2853C = 0x2600,
    TP2833C = 0x2610,
    TP2823C = 0x2618,
    TP2826  = 0x2601,
    TP2816  = 0x2619,
    TP2827  = 0x2611,
    TP2827C = 0x270C
};
enum {
    TP2802_1080P25     = 0x03,
    TP2802_1080P30     = 0x02,
    TP2802_720P25      = 0x05,
    TP2802_720P30      = 0x04,
    TP2802_720P50      = 0x01,
    TP2802_720P60      = 0x00,
    TP2802_SD          = 0x06,
    INVALID_FORMAT     = 0x07,
    TP2802_720P25V2    = 0x0D,
    TP2802_720P30V2    = 0x0C,
    TP2802_PAL         = 0x08,
    TP2802_NTSC        = 0x09,
    TP2802_HALF1080P25 = 0x43,
    TP2802_HALF1080P30 = 0x42,
    TP2802_HALF720P25  = 0x45,
    TP2802_HALF720P30  = 0x44,
    TP2802_HALF720P50  = 0x41,
    TP2802_HALF720P60  = 0x40,
    TP2802_3M18        = 0x20,   //2048x1536@18.75 for TVI
    TP2802_5M12        = 0x21,   //2592x1944@12.5 for TVI
    TP2802_4M15        = 0x22,   //2688x1520@15 for TVI
    TP2802_3M20        = 0x23,   //2048x1536@20 for TVI
    TP2802_4M12        = 0x24,   //2688x1520@12.5 for TVI
    TP2802_6M10        = 0x25,   //3200x1800@10 for TVI
    TP2802_QHD30       = 0x26,   //2560x1440@30 for TVI/HDA/HDC
    TP2802_QHD25       = 0x27,   //2560x1440@25 for TVI/HDA/HDC
    TP2802_QHD15       = 0x28,   //2560x1440@15 for HDA
    TP2802_QXGA18      = 0x29,   //2048x1536@18 for HDA/TVI
    TP2802_QXGA30      = 0x2A,   //2048x1536@30 for HDA
    TP2802_QXGA25      = 0x2B,   //2048x1536@25 for HDA
    TP2802_4M30        = 0x2C,   //2688x1520@30 for TVI(for future)
    TP2802_4M25        = 0x2D,   //2688x1520@25 for TVI(for future)
    TP2802_5M20        = 0x2E,   //2592x1944@20 for TVI/HDA/CVI
    TP2802_8M15        = 0x2f,   //3840x2160@15 for TVI/HDA/CVI
    TP2802_8M12        = 0x30,   //3840x2160@12.5 for TVI/CVI
    TP2802_1080P15     = 0x31,   //1920x1080@15 for TVI
    TP2802_1080P60     = 0x32,   //1920x1080@60 for TVI
    TP2802_960P30      = 0x33,   //1280x960@30 for TVI
    TP2802_1080P20     = 0x34,   //1920x1080@20 for TVI
    TP2802_1080P50     = 0x35,   //1920x1080@50 for TVI
    TP2802_720P14      = 0x36,   //1280x720@14 for TVI
    TP2802_720P30HDR   = 0x37,   //1280x720@30 for TVI
    TP2802_6M20        = 0x38,   //2960x1920@20 for CVI
    TP2802_8M15V2      = 0x39,   //3264x2448@15 for TVI
    TP2802_5M20V2      = 0x3a,   //2960x1660@20 for TVI
    TP2802_8M7         = 0x3b,   //3720x2160@7.5 for AHD
};
enum {
    VIDEO_UNPLUG,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};
enum {
    SDR_1CH,    //148.5M mode
    SDR_2CH,    //148.5M mode
    DDR_2CH,    //297M mode, support from TP2822/23
    DDR_4CH,    //297M mode, support from TP2824/33
    DDR_1CH     //297M mode, support from TP2827
};
enum {
    CH_1       = 0, //
    CH_2       = 1,  //
    CH_3       = 2,  //
    CH_4       = 3,   //
    CH_ALL     = 4,
    DATA_PAGE  = 5,
    AUDIO_PAGE = 9
};
enum {
    SCAN_DISABLE = 0,
    SCAN_AUTO,
    SCAN_TVI,
    SCAN_HDA,
    SCAN_HDC,
    SCAN_MANUAL,
    SCAN_TEST
};
enum {
    STD_TVI,
    STD_HDA,
    STD_HDC,
    STD_HDA_DEFAULT,
    STD_HDC_DEFAULT
};
enum {
    PTZ_TVI       = 0,
    PTZ_HDA_1080P = 1,
    PTZ_HDA_720P  = 2,
    PTZ_HDA_CVBS  = 3,
    PTZ_HDC       = 4,
    PTZ_HDA_3M18  = 5, //HDA QXGA18
    PTZ_HDA_3M25  = 6, //HDA QXGA25,QXGA30
    PTZ_HDA_4M25  = 7, //HDA QHD25,QHD30,5M20
    PTZ_HDA_4M15  = 8, //HDA QHD15,5M12.5
    PTZ_HDC_QHD   = 9 //HDC QHD25,QHD30
};
enum {
    PTZ_RX_TVI_CMD,
    PTZ_RX_TVI_BURST,
    PTZ_RX_ACP1,
    PTZ_RX_ACP2,
    PTZ_RX_ACP3
};
#define FLAG_LOSS           0x80
#define FLAG_H_LOCKED     0x20
#define FLAG_HV_LOCKED    0x60

//#define FLAG_HDC_MODE    0x80 //video standard is in std[]
#define FLAG_HALF_MODE    0x40
#define FLAG_MEGA_MODE    0x20
//#define FLAG_HDA_MODE    0x10 //video standard is in std[]

#define CHANNELS_PER_CHIP   4
#define MAX_CHIPS   4
#define SUCCESS             0
#define FAILURE             -1

#define BRIGHTNESS          0x10
#define CONTRAST            0x11
#define SATURATION          0x12
#define HUE                 0X13
#define SHARPNESS           0X14


#define     MAX_COUNT       0xffff

typedef struct _tp2802_register {
    unsigned char chip;
    unsigned char ch;
    unsigned int reg_addr;
    unsigned int value;
} tp2802_register;

typedef struct _tp2802_work_mode {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
} tp2802_work_mode;

typedef struct _tp2802_video_mode {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char std;
} tp2802_video_mode;

typedef struct _tp2802_video_loss {
    unsigned char chip;
    unsigned char ch;
    unsigned char is_lost;
} tp2802_video_loss;

typedef struct _tp2802_image_adjust {
    unsigned char chip;
    unsigned char ch;
    unsigned int hue;
    unsigned int contrast;
    unsigned int brightness;
    unsigned int saturation;
    unsigned int sharpness;
} tp2802_image_adjust;

typedef struct _tp2802_PTZ_data {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char data[16];
} tp2802_PTZ_data;

typedef enum _tp2802_audio_samplerate {
    SAMPLE_RATE_8000,
    SAMPLE_RATE_16000,
} tp2802_audio_samplerate;

typedef struct _tp2802_audio_playback {
    unsigned int chip;
    unsigned int chn;
} tp2802_audio_playback;

typedef struct _tp2802_audio_da_volume {
    unsigned int chip;
    unsigned int volume;
} tp2802_audio_da_volume;

typedef struct _tp2802_audio_da_mute {
    unsigned int chip;
    unsigned int flag;
} tp2802_audio_da_mute;

typedef struct _tp2833_audio_format {
    unsigned int chip;
    unsigned int chn;
    unsigned int format;   /* 0:i2s; 1:dsp */
    unsigned int mode;   /* 0:slave 1:master*/
    unsigned int clkdir;  /*0:inverted;1:non-inverted*/
    unsigned int bitrate; /*0:256fs 1:320fs*/
    unsigned int precision;/*0:16bit;1:8bit*/
} tp2833_audio_format;
// IOCTL Definitions
#define TP2802_IOC_MAGIC            'v'

#define TP2802_READ_REG            _IOWR(TP2802_IOC_MAGIC, 1,  tp2802_register)
#define TP2802_WRITE_REG           _IOW(TP2802_IOC_MAGIC,  2,  tp2802_register)
#define TP2802_SET_VIDEO_MODE      _IOW(TP2802_IOC_MAGIC,  3,  tp2802_video_mode)
#define TP2802_GET_VIDEO_MODE      _IOWR(TP2802_IOC_MAGIC, 4,  tp2802_video_mode)
#define TP2802_GET_VIDEO_LOSS      _IOWR(TP2802_IOC_MAGIC, 5,  tp2802_video_loss)
#define TP2802_SET_IMAGE_ADJUST    _IOW(TP2802_IOC_MAGIC,  6,  tp2802_image_adjust)
#define TP2802_GET_IMAGE_ADJUST    _IOWR(TP2802_IOC_MAGIC, 7,  tp2802_image_adjust)
#define TP2802_SET_PTZ_DATA        _IOW(TP2802_IOC_MAGIC,  8,  tp2802_PTZ_data)
#define TP2802_GET_PTZ_DATA        _IOWR(TP2802_IOC_MAGIC, 9,  tp2802_PTZ_data)
#define TP2802_SET_SCAN_MODE       _IOW(TP2802_IOC_MAGIC,  10, tp2802_work_mode)
#define TP2802_DUMP_REG            _IOW(TP2802_IOC_MAGIC,  11, tp2802_register)
#define TP2802_FORCE_DETECT        _IOW(TP2802_IOC_MAGIC,  12, tp2802_work_mode)
#define TP2802_SET_SAMPLE_RATE     _IOW(TP2802_IOC_MAGIC,  13, tp2802_audio_samplerate)
#define TP2802_SET_AUDIO_PLAYBACK  _IOW(TP2802_IOC_MAGIC,  14, tp2802_audio_playback)
#define TP2802_SET_AUDIO_DA_VOLUME _IOW(TP2802_IOC_MAGIC,  15, tp2802_audio_da_volume)
#define TP2802_SET_AUDIO_DA_MUTE   _IOW(TP2802_IOC_MAGIC,  16, tp2802_audio_da_mute)
#define TP2802_SET_BURST_DATA      _IOW(TP2802_IOC_MAGIC,  17, tp2802_PTZ_data)
#define TP2802_SET_PTZ_NEW         _IOW(TP2802_IOC_MAGIC,  18, tp2802_PTZ_data)

#endif


