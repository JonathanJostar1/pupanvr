#ifndef __XVR_DEMO_AD_H__
#define __XVR_DEMO_AD_H__


enum {
    AD_1080P25     = 0x03,
    AD_1080P30     = 0x02,
    AD_720P25      = 0x05,
    AD_720P30      = 0x04,
    AD_720P50      = 0x01,
    AD_720P60      = 0x00,
    AD_SD          = 0x06,
    AD_INVALID_FORMAT     = 0x07,
    AD_720P25V2    = 0x0D,
    AD_720P30V2    = 0x0C,
    AD_PAL         = 0x08,
    AD_NTSC        = 0x09,
    AD_HALF1080P25 = 0x43,
    AD_HALF1080P30 = 0x42,
    AD_HALF720P25  = 0x45,
    AD_HALF720P30  = 0x44,
    AD_HALF720P50  = 0x41,
    AD_HALF720P60  = 0x40,
    AD_3M18        = 0x20,   //2048x1536@18.75 for TVI
    AD_5M12        = 0x21,   //2592x1944@12.5 for TVI
    AD_4M15        = 0x22,   //2688x1520@15 for TVI
    AD_3M20        = 0x23,   //2048x1536@20 for TVI
    AD_4M12        = 0x24,   //2688x1520@12.5 for TVI
    AD_6M10        = 0x25,   //3200x1800@10 for TVI
    AD_QHD30       = 0x26,   //2560x1440@30 for TVI/HDA/HDC
    AD_QHD25       = 0x27,   //2560x1440@25 for TVI/HDA/HDC
    AD_QHD15       = 0x28,   //2560x1440@15 for HDA
    AD_QXGA18      = 0x29,   //2048x1536@18 for HDA/TVI
    AD_QXGA30      = 0x2A,   //2048x1536@30 for HDA
    AD_QXGA25      = 0x2B,   //2048x1536@25 for HDA
    AD_4M30        = 0x2C,   //2688x1520@30 for TVI(for future)
    AD_4M25        = 0x2D,   //2688x1520@25 for TVI(for future)
    AD_5M20        = 0x2E,   //2592x1944@20 for TVI/HDA/CVI
    AD_8M15        = 0x2f,   //3840x2160@15 for TVI/HDA/CVI
    AD_8M12        = 0x30,   //3840x2160@12.5 for TVI/CVI
    AD_1080P15     = 0x31,   //1920x1080@15 for TVI
    AD_1080P60     = 0x32,   //1920x1080@60 for TVI
    AD_960P30      = 0x33,   //1280x960@30 for TVI
    AD_1080P20     = 0x34,   //1920x1080@20 for TVI
    AD_1080P50     = 0x35,   //1920x1080@50 for TVI
    AD_720P14      = 0x36,   //1280x720@14 for TVI
    AD_720P30HDR   = 0x37,   //1280x720@30 for TVI
    AD_6M20        = 0x38,   //2960x1920@20 for CVI
    AD_8M15V2      = 0x39,   //3264x2448@15 for TVI
    AD_5M20V2      = 0x3a,   //2960x1660@20 for TVI
    AD_8M7         = 0x3b,   //3720x2160@7.5 for AHD
    AD_BUTT,
};

enum {
    AD_STD_TVI,
    AD_STD_HDA,
    AD_STD_HDC,
    AD_STD_HDA_DEFAULT,
    AD_STD_HDC_DEFAULT
};

enum {
    AD_0MUX = 0,
    AD_1MUX,
    AD_2MUX,
    AD_4MUX,
};

enum {
    AD_PTZ_TVI       = 0,
    AD_PTZ_HDA_1080P = 1,
    AD_PTZ_HDA_720P  = 2,
    AD_PTZ_HDA_CVBS  = 3,
    AD_PTZ_HDC       = 4,
    AD_PTZ_HDA_3M18  = 5, //HDA QXGA18
    AD_PTZ_HDA_3M25  = 6, //HDA QXGA25,QXGA30
    AD_PTZ_HDA_4M25  = 7, //HDA QHD25,QHD30,5M20
    AD_PTZ_HDA_4M15  = 8, //HDA QHD15,5M12.5
    AD_PTZ_HDC_QHD   = 9 //HDC QHD25,QHD30
};

typedef struct _ad_video_mode {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char std;
} ad_video_mode;

typedef struct _ad_video_loss {
    unsigned char chip;
    unsigned char ch;
    unsigned char is_lost;
} ad_video_loss;


typedef struct _ad_PTZ_data {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char data[16];
} ad_PTZ_data;


typedef struct _ad_device {
    //attribute
    struct list_head list;
    char* name;

    //functions
    int (*fpGetChips)();
    int (*fpGetVideLoss)(ad_video_loss *pLoss);
    int (*fpGetVideMode)(ad_video_mode *pMode);
    int (*fpGetMuxMode)(int chip);
    int (*fpSetPTZData)(ad_PTZ_data *pPtzData);
    int (*fpSetChnBind)(int chip, int *chns);
    int (*fpInit)();
    int (*fpExit)();

}ad_device;

ad_device* ad_device_find(const char *name);
int ad_device_init();
int ad_device_cleanup();



#endif // __XVR_DEMO_AD_H__

