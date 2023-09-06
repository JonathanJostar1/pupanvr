#ifndef __FH_6210_H__
#define __FH_6210_H__

enum {
    FH_1080P25      =   0x03,
    FH_1080P30      =   0x02,
    FH_720P25       =   0x05,
    FH_720P30       =   0x04,
    FH_720P50       =   0x01,
    FH_720P60       =   0x00,
    FH_SD           =   0x06,
    FH_INVALID      =   0x07,
    FH_720P25V2     =   0x0D,
    FH_720P30V2     =   0x0C,
    FH_PAL          =   0x08,
    FH_NTSC         =   0x09,
    FH_HALF1080P25  =   0x43,
    FH_HALF1080P30  =   0x42,
    FH_HALF720P25   =   0x45,
    FH_HALF720P30   =   0x44,
    FH_HALF720P50   =   0x41,
    FH_HALF720P60   =   0x40,
    FH_3M18         =   0x20,   //2048x1536@18.75 for FVI
    FH_5M12         =   0x21,   //2592x1944@12.5 for FVI
    FH_4M15         =   0x22,   //2688x1520@15 for FVI
    FH_3M20         =   0x23,   //2048x1536@20 for FVI
    FH_4M12         =   0x24,   //2688x1520@12.5 for FVI
    FH_6M10         =   0x25,   //3200x1800@10 for FVI
    FH_QHD30        =   0x26,   //2560x1440@30 for FVI/HDA/HDC
    FH_QHD25        =   0x27,   //2560x1440@25 for FVI/HDA/HDC
    FH_QHD15        =   0x28,   //2560x1440@15 for HDA
    FH_QXGA18       =   0x29,   //2048x1536@18 for HDA/FVI
    FH_QXGA30       =   0x2A,   //2048x1536@30 for HDA
    FH_QXGA25       =   0x2B,   //2048x1536@25 for HDA
    FH_4M30         =   0x2C,   //2688x1520@30 for FVI
    FH_4M25         =   0x2D,   //2688x1520@25 for FVI
    FH_5M20         =   0x2E,   //2592x1944@20 for FVI/HDA/FCVI
    FH_8M15         =   0x2F,   //3840x2160@15 for FVI/HDA/FCVI
    FH_8M12         =   0x30,   //3840x2160@12.5 for FVI/FCVI
    FH_1080P15      =   0x31,   //1920x1080@15 for FVI
    FH_1080P60      =   0x32,   //1920x1080@60 for FVI
    FH_960P30       =   0x33,   //1280x960@30 for FVI
    FH_1080P20      =   0x34,   //1920x1080@20 for FVI
    FH_1080P50      =   0x35,   //1920x1080@50 for FVI
    FH_720P14       =   0x36,   //1280x720@14 for FVI
    FH_720P30HDR    =   0x37,   //1280x720@30 for FVI
    FH_6M20         =   0x38,   //2960x1920@20 for FCVI
    FH_8M15V2       =   0x39,   //3264x2448@15 for FVI
    FH_5M20V2       =   0x3A,   //2960x1660@20 for FVI
    FH_8M7          =   0x3B,   //3720x2160@7.5 for HDA
	FH_8M12V2       =   0x3C,   //4096x2160@12 for FCVI
	FH_8M15V3       =   0x3D,   //3520x2160@15 for FCVI
};

typedef enum
{
	format_ftvi = 0,
	format_fahd = 1,
	format_fcvi = 2,
	format_pal = 3,
	format_ntsc = 4,
	format_not_support = 5,
	format_support_all = 6
}format_t;

typedef struct _fh_video_mode
{
	unsigned char chip;
	unsigned char chan;
	unsigned char mode;
	unsigned char std;
}fh_video_mode;

typedef struct _fh_video_loss
{
	unsigned char chip;
	unsigned char chan;
	unsigned char is_lost;
}fh_video_loss;

typedef struct
{
    int chip;
    int chan;
    int format;
}fh_fmt_det;

typedef struct _fh_i2c_frame
{
    unsigned char chip;
    unsigned char regaddr;
    unsigned char value;
}fh_i2c_frame;

typedef struct _fh_i2c_update_page
{
	int chip;
	unsigned short page;
	unsigned char pageshift;
	unsigned char len;
	unsigned char buf[128];
}fh_i2c_update_page;

typedef struct
{
	int chip;
	int chan;
	int df_en;
	int df_status;
	unsigned short * df_addr;
	unsigned short df_num;
}fh_ioctl_df_param;

typedef struct
{
	int chip;
	int chan;
	unsigned short df_num;
	unsigned short df_water_level;
	unsigned short * user_addr;
}fh_ioctl_df_get_value;

typedef struct
{
	int chip;
	int value;
}fh_ioctl_chip_sel;

typedef struct
{
	int chip;
	int chan;
}fh_ioctl_sel;

typedef struct
{
	int chip;
	int chan;
	int en;
}fh_ioctl_sel_en;

typedef struct
{
	int chip;
	int chan;
	int agc_stable_status;//0不稳定，1是稳定,2是未开启
	int acc_stable_status;//0不稳定，1是稳定,2是未开启
}fh_get_agc_acc_status;

typedef union
{
	struct
	{
		unsigned int chip          :4;
		unsigned int chan          :4;
		unsigned int sdMode        :2;
		unsigned int afdMode       :2;
		unsigned int sdStatus      :1;
		unsigned int hlockStatus   :1;
		unsigned int vlockStatus   :1;
		unsigned int slockStatus   :1;
		unsigned int reserved      :16;
	}bf;
	unsigned int dw;
}fh_get_video_status;

typedef union
{
	struct
	{
		unsigned int chip         :4;
		unsigned int chan         :4;
		unsigned int format       :8;
		unsigned int reserved     :16;
	}bf;
	unsigned int dw;
}fh_force_dec;

typedef union
{
	struct
	{
		unsigned int chip         :4;
		unsigned int chan         :4;
		unsigned int mode         :8;
		unsigned int reserved     :16;
	}bf;
	unsigned int dw;
}fh_force_mode;

typedef union
{
	struct
	{
		unsigned int chip         :4;
		unsigned int chan         :4;
		unsigned int ycDelay_low  :3;
		unsigned int ycDelay_high :5;
		unsigned int reserved     :16;
	}bf;
	unsigned int dw;
}fh_ycbcr_delay;

typedef union
{
	struct
	{
		unsigned int chip         :4;
		unsigned int chan         :4;
		unsigned int ygain        :10;
		unsigned int brightness   :10;
		unsigned int reserved     :4;
	}bf;
	unsigned int dw;
}fh_brightness;

typedef union
{
	struct
	{
		unsigned int chip            :4;
		unsigned int chan            :4;
		unsigned int sharpness_gain  :8;
		unsigned int reserved        :16;
	}bf;
	unsigned int dw;
}fh_sharpness;

typedef union
{
	struct
	{
		unsigned int chip                 :4;
		unsigned int chan                 :4;
		unsigned int constract_gain       :5;
		unsigned int reserved             :19;
	}bf;
	unsigned int dw;
}fh_constract;

typedef union
{
	struct
	{
		unsigned int chip       :4;
		unsigned int chan       :4;
		unsigned int hueCos     :10;
		unsigned int hueSin     :10;
		unsigned int reserved   :4;
	}bf;
	unsigned int dw;
}fh_hue;

typedef union
{
	struct
	{
		unsigned int chip       :4;
		unsigned int chan       :4;
		unsigned int saturation :8;
		unsigned int reserved   :16;
	}bf;
	unsigned int dw;
}fh_saturation;

typedef union
{
	struct
	{
		unsigned int chip      : 4;
		unsigned int chan      : 4;
		unsigned int format    : 8;
		unsigned int type      : 8;
		unsigned int reserved  : 8;
	}bf;
	unsigned int dw;
}fh_ioctl_ptz_type;

typedef struct
{
	unsigned char chip;
	unsigned char chan;
	unsigned char format;
	unsigned char len;
	unsigned char buf[12];
}fh_ioctl_ptz_data;

typedef struct
{
	int en;
	fh_ioctl_ptz_type ptz_type;
}fh_ioctl_ptz_tx;

typedef struct
{
	unsigned char chip;
	unsigned char chan;
	unsigned short len;
	unsigned char buf[72];
}fh_ioctl_coax_data;

typedef union
{
	struct
	{
		unsigned int chip   : 8;
		unsigned int chan   : 8;
		unsigned int left   : 16;
	}bf;
	unsigned int dw;
}fh_ioctl_coax_data_left;

typedef struct
{
	int shift;
	int len;
	unsigned char buf[1024];
}fh_ioctl_bd_data;

typedef struct
{
	int chip;
	int chan;
	int len;
	unsigned char buf[24];
}fh_ioctl_bd_head;

typedef struct
{
	int chip;
	int chan;
	int shift;
	int len;
}fh_ioctl_bd_bin;

typedef struct
{
	struct
	{
		unsigned int chip    : 4;
		unsigned int status  : 4;
		unsigned int reserved : 24;
	}bf;
	unsigned int dw;
}fh_ioctl_heart;

typedef struct
{
    unsigned char chip;
    unsigned char protocol;   /* 0:i2s;1:dsp */
    unsigned char mode;   /* 0:slave 1:master*/
    unsigned char bitWidth;/* 0:8bit;1:16bit*/
	unsigned char sampleRate;/* 0:8K,1:16K */
	unsigned char format;/*0:signed 1:unsigned*/
}fh_audio_format;

typedef struct
{
	unsigned char chip;
	unsigned char chan;
	unsigned char input_type;//ain-in audio or coax audio
}fh_audio_coax_channel;

typedef struct
{
	unsigned char chip;
	unsigned char chan;
	unsigned char is_detect;
}fh_audio_coax_detect;

typedef struct
{
	unsigned int chip            : 2;
	unsigned int chan            : 2;
	unsigned int signalTh        : 6;
	unsigned int autoThGain      : 4;
	unsigned int binaryFixTh     : 6;
	unsigned int dataPos         : 10;
	unsigned int binaryThMod     : 1;
	unsigned int reserved1       : 1;
	unsigned int dataPwmWidth    : 24;
	unsigned int reserved2       : 8;
}fh_audio_coax_config;

typedef struct
{
	unsigned int chip           :  2;
	unsigned int chan           :  3;//0:ain0 1:ain1 2:ain2 3:ain3 4:aux
	unsigned int anaGain        :  4;//-6dB~6dB 默认为8
	unsigned int digGain        :  4;//值乘以1/8 0为bypass 默认值为8
	unsigned int reserved       :  19;
}fh_audio_da_volume;

typedef struct
{
	unsigned int chip          :  2;
	unsigned int chan          :  2;
	unsigned int gain          :  4;//值乘以1/8 0为bypass 默认值为8
	unsigned int reserved      :  24;
}fh_audio_coax_volume;

typedef struct
{
	unsigned int chip         :  2;
	unsigned int type         :  4;//0:ain0 1:ain1 2:ain2 3:ain3 4:aux
	unsigned int mute         :  1;//mute
	unsigned int reserved     :  25;
}fh_audio_da_mute;

typedef struct
{
	unsigned int chip;
	unsigned int chan;
	unsigned int times;
}fh_sd_loss_time;

typedef struct
{
	unsigned int chip         :  2;
	unsigned int threshold    :  16;
	unsigned int detnum       :  7;
	unsigned int ain0_en      :  1;
	unsigned int ain1_en      :  1;
	unsigned int ain2_en      :  1;
	unsigned int ain3_en      :  1;
	unsigned int ain4_en      :  1;
	unsigned int reserved     :  2;
}fh_audio_detect;

typedef struct
{
	unsigned int chip        :  2;
	unsigned int detStatus0  :  1;
	unsigned int detStatus1  :  1;
	unsigned int detStatus2  :  1;
	unsigned int detStatus3  :  1;
	unsigned int detStatus4  :  1;
	unsigned int reserved    :  25;
}fh_audio_status;

typedef struct
{
	unsigned  int chip       : 2;
	unsigned  int source0    : 5;
	unsigned  int source1    : 5;
	unsigned  int source2    : 5;
	unsigned  int source3    : 5;
	unsigned  int source4    : 5;
	unsigned  int source5    : 5;
	unsigned  int source6    : 5;
	unsigned  int source7    : 5;
	unsigned  int source8    : 5;
	unsigned  int source9    : 5;
	unsigned  int source10   : 5;
	unsigned  int source11   : 5;
	unsigned  int source12   : 5;
	unsigned  int source13   : 5;
	unsigned  int source14   : 5;
	unsigned  int source15   : 5;
	unsigned  int source16   : 5;
	unsigned  int source17   : 5;
	unsigned  int source18   : 5;
	unsigned  int source19   : 5;
}fh_audio_i2s_tx_sel;

typedef struct
{
	unsigned int chip        : 2;
	unsigned int vd          : 2;
	unsigned int delay_line  : 8;
	unsigned int reserved    : 20;
}fh_vd_delay_line;

typedef struct
{
	unsigned int chip        : 2;
	unsigned int vd          : 2;
	unsigned int vdSel1      : 2;
	unsigned int vdSel2      : 2;
	unsigned int vdSel3      : 2;
	unsigned int vdSel4      : 2;
	unsigned int reserved    : 20;
}fh_vd_sel;

typedef struct
{
	unsigned int all_en      : 1;
	unsigned int tvi_en      : 1;
	unsigned int cvi_en      : 1;
	unsigned int ahd_en      : 1;
	unsigned int ntsc_en     : 1;
	unsigned int pal_en      : 1;
	unsigned int reserved    : 26;
}fh_protocol_cfg;

typedef struct
{
	int cmd;
	void * addr;
}fh_custom_cmd;

#define FH6210_IOC_MAGIC                     'f'

#define FH_AGC_AUTO_EN           _IOWR(FH6210_IOC_MAGIC,80,fh_ioctl_sel_en)
#define FH_ACC_AUTO_EN           _IOWR(FH6210_IOC_MAGIC,81,fh_ioctl_sel_en)

#define FH_GET_VIDEO_MODE        _IOWR(FH6210_IOC_MAGIC,90,fh_video_mode)
#define FH_GET_VIDEO_STATUS      _IOWR(FH6210_IOC_MAGIC,91,fh_video_loss)
#define FH_GET_SD_LOSS_TIMES     _IOWR(FH6210_IOC_MAGIC,92,fh_sd_loss_time)
#define FH_GET_VIDEO_STATUS_DETAIL _IOWR(FH6210_IOC_MAGIC,93,fh_get_video_status)
#define FH_GET_AGC_ACC_STATUS     _IOWR(FH6210_IOC_MAGIC,94,fh_get_agc_acc_status)
#define FH_SET_PROTOCOL           _IOWR(FH6210_IOC_MAGIC,95,fh_protocol_cfg)

#define FH_GET_VIDEO_MODE_DETAIL _IOWR(FH6210_IOC_MAGIC,104,fh_fmt_det)
#define FH_CHIP_STATUS           _IOWR(FH6210_IOC_MAGIC,105,fh_ioctl_chip_sel)
#define FH_UPDATE_RAM            _IOW(FH6210_IOC_MAGIC,106,fh_i2c_update_page)
#define FH_RAM_UPDATE_DONE       _IOW(FH6210_IOC_MAGIC,107,fh_ioctl_chip_sel)
#define FH_CHECK_OK              _IOWR(FH6210_IOC_MAGIC,109,fh_ioctl_chip_sel)
#define FH_RAM_NOUPDATE          _IOWR(FH6210_IOC_MAGIC,110,fh_ioctl_chip_sel)
#define FH_CHIP_RESET            _IOWR(FH6210_IOC_MAGIC,111,fh_ioctl_chip_sel)
#define FH_CHIP_RESET_WITH_PARAM _IOWR(FH6210_IOC_MAGIC,112,fh_ioctl_chip_sel)

#define FH_DF_EN                 _IOWR(FH6210_IOC_MAGIC,113,fh_ioctl_sel)
#define FH_DF_GETSTATUS          _IOWR(FH6210_IOC_MAGIC,114,fh_ioctl_df_param)
#define FH_DF_GETVALUE           _IOWR(FH6210_IOC_MAGIC,115,fh_ioctl_df_get_value)

#define FH_YCBCR_DELAY_SET _IOWR(FH6210_IOC_MAGIC,120,fh_ycbcr_delay)
#define FH_YCBCR_DELAY_GET _IOWR(FH6210_IOC_MAGIC,121,fh_ycbcr_delay)
#define FH_BRIGHTNESS_SET  _IOWR(FH6210_IOC_MAGIC,122,fh_brightness)
#define FH_BRIGHTNESS_GET  _IOWR(FH6210_IOC_MAGIC,123,fh_brightness)
#define FH_SHARPNESS_SET   _IOWR(FH6210_IOC_MAGIC,124,fh_sharpness)
#define FH_SHARPNESS_GET   _IOWR(FH6210_IOC_MAGIC,125,fh_sharpness)
#define FH_CONSTRACT_SET   _IOWR(FH6210_IOC_MAGIC,126,fh_constract)
#define FH_CONSTRACT_GET   _IOWR(FH6210_IOC_MAGIC,127,fh_constract)
#define FH_HUE_SET         _IOWR(FH6210_IOC_MAGIC,128,fh_hue)
#define FH_HUE_GET         _IOWR(FH6210_IOC_MAGIC,129,fh_hue)
#define FH_SATURATION_SET  _IOWR(FH6210_IOC_MAGIC,130,fh_saturation)
#define FH_SATURATION_GET  _IOWR(FH6210_IOC_MAGIC,131,fh_saturation)

#define FH_FORCE_DEC       _IOWR(FH6210_IOC_MAGIC,135,fh_force_dec)
#define FH_FORCE_MODE      _IOWR(FH6210_IOC_MAGIC,136,fh_force_mode)

#define FH_PTZ_TX_SET      _IOW(FH6210_IOC_MAGIC,140,fh_ioctl_ptz_type)
#define FH_PTZ_TX_DATA_SET _IOW(FH6210_IOC_MAGIC,141,fh_ioctl_ptz_data)

#define FH_COAX_TX_DATA_LEFT   _IOWR(FH6210_IOC_MAGIC,150,fh_ioctl_coax_data_left)
#define FH_COAX_TX_DATA_WRITE  _IOWR(FH6210_IOC_MAGIC,151,fh_ioctl_coax_data)
#define FH_COAX_RX_DATA_LEFT   _IOWR(FH6210_IOC_MAGIC,152,fh_ioctl_coax_data_left)
#define FH_COAX_RX_DATA_READ   _IOWR(FH6210_IOC_MAGIC,153,fh_ioctl_coax_data)

#define FH_BD_BUFF_MALLOC       _IOW(FH6210_IOC_MAGIC,160,int)
#define FH_BD_BUFF_FILL         _IOW(FH6210_IOC_MAGIC,161,fh_ioctl_bd_data)
#define FH_BD_BUFF_FREE         _IOW(FH6210_IOC_MAGIC,162,NULL)
#define FH_BD_RAMUPDATE_SET     _IOW(FH6210_IOC_MAGIC,163,fh_ioctl_sel_en)
#define FH_BD_HEADER_SEND       _IOW(FH6210_IOC_MAGIC,164,fh_ioctl_bd_head)
#define FH_BD_BIN_SEND          _IOW(FH6210_IOC_MAGIC,165,fh_ioctl_bd_bin)
#define FH_BD_BIN_FLUSH         _IOW(FH6210_IOC_MAGIC,166,fh_ioctl_bd_bin)

#define FH_HEART_STATUS         _IOWR(FH6210_IOC_MAGIC,170,fh_ioctl_heart)
#define FH_HEART_STATUS_SET     _IOWR(FH6210_IOC_MAGIC,171,fh_ioctl_heart)
#define FH_IO_DRIVER_SET        _IOWR(FH6210_IOC_MAGIC,172,fh_ioctl_chip_sel)
#define FH_VD_DELAY_LINE_SET    _IOWR(FH6210_IOC_MAGIC,173,fh_vd_delay_line)
#define FH_VD_SEL_SET           _IOWR(FH6210_IOC_MAGIC,174,fh_vd_sel)

#define FH_SET_AUDIO_CONFIG       _IOW(FH6210_IOC_MAGIC,180,fh_audio_format)
#define FH_SET_AUDIO_COAX_CHANNEL _IOW(FH6210_IOC_MAGIC,181,fh_audio_coax_channel)
#define FH_GET_AUDIO_COAX_DETECT  _IOWR(FH6210_IOC_MAGIC,182,fh_audio_coax_detect)
#define FH_SET_AUDIO_START        _IOW(FH6210_IOC_MAGIC,183,fh_ioctl_chip_sel)
#define FH_SET_AUDIO_COAX_CONFIG  _IOW(FH6210_IOC_MAGIC,184,fh_audio_coax_config)
#define FH_GET_AUDIO_COAX_CONFIG  _IOWR(FH6210_IOC_MAGIC,185,fh_audio_coax_config)
#define FH_SET_AUDIO_DA_VOLUME    _IOW(FH6210_IOC_MAGIC,186,fh_audio_da_volume)
#define FH_GET_AUDIO_DA_VOLUME    _IOWR(FH6210_IOC_MAGIC,187,fh_audio_da_volume)
#define FH_SET_AUDIO_DA_MUTE      _IOW(FH6210_IOC_MAGIC,188,fh_audio_da_mute)
#define FH_SET_AUDIO_COAX_VOLUME  _IOW(FH6210_IOC_MAGIC,189,fh_audio_coax_volume)
#define FH_GET_AUDIO_COAX_VOLUME  _IOWR(FH6210_IOC_MAGIC,190,fh_audio_coax_volume)
#define FH_SET_AUDIO_AIN_DETECT   _IOWR(FH6210_IOC_MAGIC,191,fh_audio_detect)
#define FH_GET_AUDIO_AIN_DETECT   _IOWR(FH6210_IOC_MAGIC,192,fh_audio_detect)
#define FH_GET_AUDIO_AIN_STATUS   _IOWR(FH6210_IOC_MAGIC,193,fh_audio_status)
#define FH_SET_AUDIO_ANA_OUT_VOLUME _IOW(FH6210_IOC_MAGIC,194,fh_audio_da_volume)
#define FH_GET_AUDIO_ANA_OUT_VOLUME _IOW(FH6210_IOC_MAGIC,195,fh_audio_da_volume)
#define FH_AUDIO_ANA_OUT_SEL      _IOWR(FH6210_IOC_MAGIC,196,fh_ioctl_chip_sel)
#define FH_AUDIO_I2S_TX_SEL       _IOWR(FH6210_IOC_MAGIC,197,fh_audio_i2s_tx_sel)

#define FH_CUSTOM_CMD             _IOWR(FH6210_IOC_MAGIC,230,fh_custom_cmd)

#if 0 //force dec set
#define FH_FORMAT_NTSC_960H                   (0x11)

#define FH_FORMAT_PAL_960H                    (0x21)

#define FH_FORMAT_CVI_720P25                  (0x30)
#define FH_FORMAT_CVI_720P30                  (0x31)
#define FH_FORMAT_CVI_720P50                  (0x32)
#define FH_FORMAT_CVI_720P60                  (0x33)
#define FH_FORMAT_CVI_1080P25                 (0x34)
#define FH_FORMAT_CVI_1080P30                 (0x35)
#define FH_FORMAT_CVI_4M25                    (0x36)
#define FH_FORMAT_CVI_4M30                    (0x37)
#define FH_FORMAT_CVI_8M15                    (0x38)
#define FH_FORMAT_CVI_5M20                    (0x39)
#define FH_FORMAT_CVI_8M12                    (0x3a)

#define FH_FORMAT_TVI_720P50                  (0x40)
#define FH_FORMAT_TVI_720P60                  (0x41)
#define FH_FORMAT_TVI_1080P25                 (0x42)
#define FH_FORMAT_TVI_1080P30                 (0x43)
#define FH_FORMAT_TVI_4M25                    (0x44)
#define FH_FORMAT_TVI_4M30                    (0x45)
#define FH_FORMAT_TVI_5M12                    (0x46)
#define FH_FORMAT_TVI_5M20                    (0x47)
#define FH_FORMAT_TVI_8M12                    (0x48)
#define FH_FORMAT_TVI_8M15                    (0x49)
#define FH_FORMAT_TVI1_720P25                 (0x4a)
#define FH_FORMAT_TVI1_720P30                 (0x4b)
#define FH_FORMAT_TVI2_720P25                 (0x4c)
#define FH_FORMAT_TVI2_720P30                 (0x4d)
#define FH_FORMAT_TVI_3M18                    (0x4e)
#define FH_FORMAT_TVI_4M15                    (0x4f)
#define FH_FORMAT_TVI_4M15_BH2                (0x60)
#define FH_FORMAT_TVI_960P30                  (0x61)

#define FH_FORMAT_AHD_720P25                  (0x50)
#define FH_FORMAT_AHD_720P30                  (0x51)
#define FH_FORMAT_AHD_1080P25                 (0x52)
#define FH_FORMAT_AHD_1080P30                 (0x53)
#define FH_FORMAT_AHD_4M25                    (0x54)
#define FH_FORMAT_AHD_5M20                    (0x55)
#define FH_FORMAT_AHD_8M15                    (0x56)
#define FH_FORMAT_AHD_4M15                    (0x57)
#define FH_FORMAT_AHD_5M12                    (0x58)
#define FH_FORMAT_AHD_4M30                    (0x59)

#define FH_FORMAT_UNKNOWN						(0x0)
#endif

#endif
