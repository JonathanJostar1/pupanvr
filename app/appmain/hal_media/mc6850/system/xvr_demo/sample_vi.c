#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "fy_common.h"
#include "fy_comm_sys.h"
#include "fy_comm_vb.h"
#include "fy_comm_vi.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "sample_comm.h"
#include "sample_vi.h"
#include "list.h"
#include "sample_ad.h"

struct ad_video_format {
    int mode;
    int width;
    int height;
    int fps;
    int cropx;
};


struct vi_logi_chns {
    int count;
    int ad_chips;
    struct chn_info {
        int dev;
        int chn;
        int ad_chip;
        int ad_chn;
    } chns[VIU_MAX_CHN_NUM];
};


static struct ad_video_format gstVideoFormats[] = {
    { .mode = AD_720P60,    .width = 1280, .height = 720,  .fps = 60, .cropx = 0},
    { .mode = AD_720P50,    .width = 1280, .height = 720,  .fps = 50, .cropx = 0},
    { .mode = AD_1080P30,   .width = 1920, .height = 1080, .fps = 30, .cropx = 0},
    { .mode = AD_1080P25,   .width = 1920, .height = 1080, .fps = 25, .cropx = 0},
    { .mode = AD_720P30,    .width = 1280, .height = 720,  .fps = 30, .cropx = 0},
    { .mode = AD_720P25,    .width = 1280, .height = 720,  .fps = 25, .cropx = 0},
    { .mode = AD_SD,        .width = 1280, .height = 720,  .fps =  0, .cropx = 0},
    { .mode = AD_PAL,       .width = 960,  .height = 576,  .fps = 25, .cropx = 0},
    { .mode = AD_NTSC,      .width = 960,  .height = 480,  .fps = 30, .cropx = 0},
    { .mode = AD_720P30V2,  .width = 1280, .height = 720,  .fps = 30, .cropx = 0},
    { .mode = AD_720P25V2,  .width = 1280, .height = 720,  .fps = 25, .cropx = 0},
    { .mode = AD_3M18,      .width = 2048, .height = 1536, .fps = 18, .cropx = 0},
    { .mode = AD_5M12,      .width = 2592, .height = 1944, .fps = 12, .cropx = 32},
    { .mode = AD_4M15,      .width = 2688, .height = 1520, .fps = 15, .cropx = 0},
    { .mode = AD_3M20,      .width = 2048, .height = 1536, .fps = 20, .cropx = 0},
    { .mode = AD_4M12,      .width = 2688, .height = 1520, .fps = 12, .cropx = 0},
    { .mode = AD_6M10,      .width = 3200, .height = 1800, .fps = 10, .cropx = 0},
    { .mode = AD_QHD30,     .width = 2560, .height = 1440, .fps = 30, .cropx = 0},
    { .mode = AD_QHD25,     .width = 2560, .height = 1440, .fps = 25, .cropx = 0},
    { .mode = AD_QHD15,     .width = 2560, .height = 1440, .fps = 15, .cropx = 0},
    { .mode = AD_QXGA18,    .width = 2048, .height = 1536, .fps = 18, .cropx = 0},
    { .mode = AD_QXGA30,    .width = 2048, .height = 1536, .fps = 30, .cropx = 0},
    { .mode = AD_QXGA25,    .width = 2048, .height = 1536, .fps = 25, .cropx = 0},
    { .mode = AD_4M30,      .width = 2688, .height = 1520, .fps = 30, .cropx = 0},
    { .mode = AD_4M25,      .width = 2688, .height = 1520, .fps = 25, .cropx = 0},
    { .mode = AD_5M20,      .width = 2592, .height = 1944, .fps = 20, .cropx = 32},
    { .mode = AD_8M15,      .width = 3840, .height = 2160, .fps = 15, .cropx = 0},
    { .mode = AD_8M12,      .width = 3840, .height = 2160, .fps = 12, .cropx = 0},
    { .mode = AD_1080P15,   .width = 1920, .height = 1080, .fps = 15, .cropx = 0},
    { .mode = AD_1080P60,   .width = 1920, .height = 1080, .fps = 60, .cropx = 0},
    { .mode = AD_960P30,    .width = 1280, .height = 960,  .fps = 30, .cropx = 0},
    { .mode = AD_1080P20,   .width = 1920, .height = 1080, .fps = 20, .cropx = 0},
    { .mode = AD_1080P50,   .width = 1920, .height = 1080, .fps = 50, .cropx = 0},
    { .mode = AD_720P14,    .width = 1280, .height = 720,  .fps = 14, .cropx = 0},
    { .mode = AD_720P30HDR, .width = 1280, .height = 720,  .fps = 30, .cropx = 0},
    { .mode = AD_6M20,      .width = 3200, .height = 1800, .fps = 20, .cropx = 0},
    { .mode = AD_8M15V2,    .width = 3840, .height = 2160, .fps = 15, .cropx = 0},
    { .mode = AD_5M20V2,    .width = 2592, .height = 1944, .fps = 20, .cropx = 32},
    { .mode = AD_8M7,       .width = 3840, .height = 2160, .fps =  7, .cropx = 0},

    { .mode = AD_INVALID_FORMAT,   .width = 0,    .height = 0,    .fps =  0, .cropx = 0},
    { .mode = -1,           .width = 0,    .height = 0,    .fps =  0, .cropx = 0},

};

static ad_device *s_pAdDeivce = NULL;

static int s_vitestcase = -1;
static int s_vi_fps = 30;
static char *s_vi_dir = "/nfs/vidump";
static int s_vi_testmode = 0;
static int s_vi_channel_ns = 4;
static int g_vi_vb_source = 0;
static int g_vi_5m = 0;
static int g_vi_pixelformat =  PIXEL_FORMAT_YUV_SEMIPLANAR_422;

static VI_EC_LEVEL_E s_vi_ec = VI_EC_BYTE_MODE_128byte;

static pthread_t thread_id_vi = 0;
static FY_BOOL bThreadStop = FY_FALSE;
static struct vi_logi_chns g_stViChns = {0};



#define CASE_RETURN_STRING(value) case value: return #value;

static char *string_unknown = "AD_UNKNOWN";

static char*  AD_Mod_String(int e)
{
    switch(e) {
        CASE_RETURN_STRING(AD_720P60);
        CASE_RETURN_STRING(AD_720P50);
        CASE_RETURN_STRING(AD_1080P30);
        CASE_RETURN_STRING(AD_1080P25);
        CASE_RETURN_STRING(AD_720P30);
        CASE_RETURN_STRING(AD_720P25);
        CASE_RETURN_STRING(AD_SD);
        CASE_RETURN_STRING(AD_INVALID_FORMAT);
        CASE_RETURN_STRING(AD_PAL);
        CASE_RETURN_STRING(AD_NTSC);
        CASE_RETURN_STRING(AD_720P30V2);
        CASE_RETURN_STRING(AD_720P25V2);
        CASE_RETURN_STRING(AD_3M18);
        CASE_RETURN_STRING(AD_5M12);
        CASE_RETURN_STRING(AD_4M15);
        CASE_RETURN_STRING(AD_3M20);
        CASE_RETURN_STRING(AD_4M12);
        CASE_RETURN_STRING(AD_6M10);
        CASE_RETURN_STRING(AD_QHD30);
        CASE_RETURN_STRING(AD_QHD25);
        CASE_RETURN_STRING(AD_QHD15);
        CASE_RETURN_STRING(AD_QXGA18);
        CASE_RETURN_STRING(AD_QXGA30);
        CASE_RETURN_STRING(AD_QXGA25);
        CASE_RETURN_STRING(AD_4M30);
        CASE_RETURN_STRING(AD_4M25);
        CASE_RETURN_STRING(AD_5M20);
        CASE_RETURN_STRING(AD_8M15);
        CASE_RETURN_STRING(AD_8M12);
        CASE_RETURN_STRING(AD_1080P15);
        CASE_RETURN_STRING(AD_1080P60);
        CASE_RETURN_STRING(AD_960P30);
        CASE_RETURN_STRING(AD_1080P20);
        CASE_RETURN_STRING(AD_1080P50);
        CASE_RETURN_STRING(AD_720P14);
        CASE_RETURN_STRING(AD_720P30HDR);
        CASE_RETURN_STRING(AD_6M20);
        CASE_RETURN_STRING(AD_8M15V2);
        CASE_RETURN_STRING(AD_5M20V2);
        CASE_RETURN_STRING(AD_8M7);
    default:
        return string_unknown;
        break;
    }
}


static char*  AD_Std_String(int e)
{
    switch(e) {
        CASE_RETURN_STRING(AD_STD_TVI);
        CASE_RETURN_STRING(AD_STD_HDA);
        CASE_RETURN_STRING(AD_STD_HDC);
        CASE_RETURN_STRING(AD_STD_HDA_DEFAULT);
        CASE_RETURN_STRING(AD_STD_HDC_DEFAULT);
    default:
        return string_unknown;
        break;
    }
}

static int __CalcPicVbBlkSize(SIZE_S stSize, PIXEL_FORMAT_E enPixFmt, FY_U32 u32AlignWidth)
{

    unsigned int u32Width       = 0;
    unsigned int u32Height      = 0;
    unsigned int u32BlkSize     = 0;

    if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt) {
        printf("pixel format[%d] input failed!\n", enPixFmt);
        return -1;
    }


    if(704 == stSize.u32Width) {
        stSize.u32Width = 720;
    }
    u32Width  = CEILING_2_POWER(stSize.u32Width, u32AlignWidth);
    u32Height = CEILING_2_POWER(stSize.u32Height, u32AlignWidth);

    if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt) {
        u32BlkSize = u32Width * u32Height * 2;
    } else {
        u32BlkSize = u32Width * u32Height * 3 / 2;
    }

    return u32BlkSize;
}

static VB_POOL __create_user_vbpool(VI_CHN channel, int vbcnt,  VI_CHN_ATTR_S *pstChnAttr, int align)
{
    VB_POOL pool_id = VB_INVALID_POOLID;
    char mmb_name[16];
    FY_U32 vb_size = __CalcPicVbBlkSize(pstChnAttr->stDestSize, pstChnAttr->enPixFormat, align);
    sprintf(mmb_name, "viM_%d[%d]", channel, vbcnt);
    pool_id = FY_MPI_VB_CreatePoolEx(vb_size, vbcnt, NULL, mmb_name);
    if(pool_id == VB_INVALID_POOLID) {
        printf("[VI: %d] Create user VB(u32BlkSize=%d,u32BlkCnt=%d) failed!\n",
               channel,  vb_size, vbcnt);


    }
    return pool_id;
}

static int __set_dev_attr(int dev, int bDouble, int mux)
{
    int result = 0;
    VI_DEV_ATTR_S stDevAttr;
    //VI_DLL_CTRL_S dll;

    memset(&stDevAttr, 0, sizeof(stDevAttr));
    stDevAttr.enIntfMode = VI_MODE_BT656;
    stDevAttr.enClkEdge  = (0 == bDouble? VI_CLK_EDGE_SINGLE_UP :  VI_CLK_EDGE_DOUBLE);
    stDevAttr.enWorkMode = mux;
    //memset(&dll, 0, sizeof(dll));
    //dll.u32DelayData = 0x26;
    //FY_MPI_VI_SetDLL(dev, &dll);
    result = FY_MPI_VI_SetDevAttr(dev, &stDevAttr);
    if(0 == result) {
        result = FY_MPI_VI_EnableDev(dev);
    } else {

    }

    return result;

}


static void __save_frame(FILE *fp, VIDEO_FRAME_S *pvFrame)
{
    FY_U32 phy_addr, size;
    char* vir_addr = NULL;

    FY_U32 u32Width = 0;
    FY_U32 u32Height = 0;


    if(NULL == fp) {
        printf("fp == NULL!\n");
        return;
    }

    if(NULL == pvFrame) {
        printf("pvFrame == NULL!\n");
        return;
    }

    phy_addr  = pvFrame->u32PhyAddr[0];
    u32Width  = pvFrame->u32Stride[0];
    u32Height = pvFrame->u32Height;

    size  = u32Width * u32Height * 2;

    vir_addr  = FY_MPI_SYS_Mmap(phy_addr, size);
    if(NULL == vir_addr) {
        printf("mmap the phyaddr: 0x%x failed!\n", phy_addr);
        return;
    }
    if(VIDEO_FIELD_FRAME == pvFrame->u32Field) {
        if(pvFrame->enCompressMode == 0) {
            fwrite(vir_addr, 1, size / 2, fp); //Y data
            fwrite(vir_addr + (pvFrame->u32PhyAddr[1] - pvFrame->u32PhyAddr[0]), 1, size / 2, fp); //UV data
        } else {
            fwrite(vir_addr, 1, size, fp); //compress data
        }

    } else {
        int i = 0;
        char* pty = vir_addr;
        char* pby = vir_addr + u32Width * u32Height / 2;
        char* pc  = vir_addr + u32Width * u32Height;
        for(i = 0; i < u32Height / 2; i++) {
            fwrite(pty + i * u32Width, 1, u32Width, fp);
            fwrite(pby + i * u32Width, 1, u32Width, fp);
        }
        fwrite(pc, 1, u32Width * u32Height, fp);
    }
    FY_MPI_SYS_Munmap((void*)vir_addr, size);

}


static int __get_frame(int chn, int ms, char* fname)
{
    int ret    = 0;
    FILE  *fp = NULL;
    VIDEO_FRAME_INFO_S stFrameInfo;


    memset(&stFrameInfo, 0, sizeof(VIDEO_FRAME_INFO_S));
    ret = FY_MPI_VI_GetFrame(chn, &stFrameInfo, ms);
    if(!ret) {
        //dump the video frame

        printf("============= dump the video frame: %d ===============\n", chn);
        printf("  u32Width       = %d\n", stFrameInfo.stVFrame.u32Width);
        printf("  u32Height      = %d\n", stFrameInfo.stVFrame.u32Height);
        printf("  u32Field       = %d\n", (int)stFrameInfo.stVFrame.u32Field);
        printf("  enPixelFormat  = %d\n", (int)stFrameInfo.stVFrame.enPixelFormat);

        printf("  enVideoFormat  = %d\n", (int)stFrameInfo.stVFrame.enVideoFormat);
        printf("  enCompressMode = %d\n", (int)stFrameInfo.stVFrame.enCompressMode);


        printf("  u32PhyAddr[0]  = 0x%x\n", (unsigned int)stFrameInfo.stVFrame.u32PhyAddr[0]);
        printf("  u32PhyAddr[1]  = 0x%x\n", (unsigned int)stFrameInfo.stVFrame.u32PhyAddr[1]);

        printf("  u32Stride[0]   = %d\n", (unsigned int)stFrameInfo.stVFrame.u32Stride[0]);
        printf("  u32Stride[1]   = %d\n", (unsigned int)stFrameInfo.stVFrame.u32Stride[1]);


        printf("---------------------------------------------------\n");


        //save  the frame to a file
        if(fname) {
            printf("Saved frame to file: %s\n", fname);
            fp = fopen(fname, "wb");
            if(fp) {
                __save_frame(fp, &stFrameInfo.stVFrame);
                fclose(fp);
            } else {
                printf("create file: %s failed!\n", fname);
            }
        }

        FY_MPI_VI_ReleaseFrame(chn, &stFrameInfo);

    }

    return ret;
}


static int __vi_dump_channel(int w, int h)
{
    char fname[64];
    time_t t;
    int i = 0;
    int depth = 1;
    int channels = (s_vitestcase < 3 ? s_vi_channel_ns : 2);

    for(i = 0; i < channels; i++) {
        FY_MPI_VI_SetFrameDepth(i, depth);
        t = time(NULL);
        sprintf(fname, "%s/CH%d_%c_%dx%d_%d.yuv", s_vi_dir, i, 'M', w, h, (int)t);
        __get_frame(i, 2000, fname);
        sprintf(fname, "%s/CH%d_%c_%dx%d_%d.yuv", s_vi_dir, i, 'S', w / 2, h / 2, (int)t);
        __get_frame(16 + i, 2000, fname);
        FY_MPI_VI_SetFrameDepth(i, 0);
    }

    return 0;

}

static int __get_viudev(int channel)
{
    int i = 0;
    int dev = -1;
    for(i = 0; i < g_stViChns.count; i++) {
        if(channel == g_stViChns.chns[i].chn) {
            dev = g_stViChns.chns[i].dev;
            break;
        }
    }
    return dev;
}

static int __set_channel_attr(int channel, int mode, int std)
{
    int result = 0;
    VI_CHN_ATTR_S stChnAttr;
    FY_U32 u32Width = 0;
    FY_U32 u32Height = 0;
    FY_U32 u32CropX = 0;
    int fps;
    int vihalf = 0;
    VB_POOL pool_id = VB_INVALID_POOLID;
    VI_WORK_MODE_E mux = VI_WORK_MODE_4Multiplex;
    char osd[64];
    struct ad_video_format *pStVMode = NULL;
    int i = 0;
    VI_DEV_ATTR_S dev_attr;
    int vmode = mode;

    int dev = __get_viudev(channel);
    if(-1 == dev) {
        printf("%s: channel %d get viu dev failed!\n", __func__, channel);
    }
    FY_MPI_VI_GetDevAttr(dev, &dev_attr);
    mux = dev_attr.enWorkMode;

    vmode = mode;
    if(vmode > 0x40) {
        vmode -= 0x40;
    }

    pStVMode = NULL;
    for(i = 0; gstVideoFormats[i].mode >= 0; i++) {
        if(gstVideoFormats[i].mode == vmode) {
            pStVMode = &gstVideoFormats[i];
            break;
        }
    }
    if(NULL == pStVMode) {
        printf("unknow format: 0x%d\n", vmode);
        return -1;
    }


    u32Width  = pStVMode->width;
    u32Height = pStVMode->height;
    u32CropX  = pStVMode->cropx;
    fps       = pStVMode->fps;

    if(0 == fps) {
        return -1;
    }
    if(mode > 0x40) {
        //lite mode
        u32Width /= 2;
        u32CropX /= 2;
    }

    memset(&stChnAttr, 0, sizeof(VI_CHN_ATTR_S));
    stChnAttr.stCapRect.s32X         = u32CropX;
    stChnAttr.stCapRect.s32Y         = 0;
    stChnAttr.stCapRect.u32Width     = u32Width - stChnAttr.stCapRect.s32X;
    stChnAttr.stCapRect.u32Height    = u32Height;
    stChnAttr.stDestSize.u32Width    = stChnAttr.stCapRect.u32Width;
    stChnAttr.stDestSize.u32Height    = u32Height;
    stChnAttr.enCapSel               = VI_CAPSEL_BOTH;
    stChnAttr.enPixFormat            = g_vi_pixelformat;
    stChnAttr.enDataSeq              = VI_INPUT_DATA_UYVY;
    stChnAttr.enScanMode             = VI_SCAN_PROGRESSIVE;
    stChnAttr.s32SrcFrameRate        = fps;
    stChnAttr.s32DstFrameRate        = fps;

    vihalf = 0;
    if((stChnAttr.stDestSize.u32Width > 1920) && (0 == g_vi_5m)) {
        stChnAttr.stDestSize.u32Width = stChnAttr.stCapRect.u32Width / 2;
        vihalf = 1;
    }


    //for CVBS
    if((mode == AD_PAL) || (mode == AD_NTSC)) {
        stChnAttr.enScanMode  = VI_SCAN_INTERLACED;
        if(0 == strncasecmp(s_pAdDeivce->name, "tp", 2)) {
        if(VI_WORK_MODE_2Multiplex == mux) {
            stChnAttr.stCapRect.u32Width *= 4;
        } else if(VI_WORK_MODE_4Multiplex == mux) {
            stChnAttr.stCapRect.u32Width *= 2;
            }
        }
    }
    if(0 == strncasecmp(s_pAdDeivce->name, "tp", 2)) {
    if((u32Width == 1280) && (u32Height == 720)) {
        if(VI_WORK_MODE_2Multiplex == mux) {
            stChnAttr.stCapRect.u32Width *= 2;
            }
        }
    }

    {
        //set gosd format
        char *pmd  = AD_Mod_String(vmode);
        char *pstd = AD_Std_String(std);
        if(mode > 0x40) {
            sprintf(osd, "%s:N,%s", pmd + 3,  pstd + 7);
        } else {
            if(vihalf == 1) {
                sprintf(osd, "%s:Fn,%s", pmd + 3,  pstd + 7);
            } else {
                sprintf(osd, "%s:F,%s", pmd + 3,  pstd + 7);
            }
        }
        region_set_info(channel, osd);
    }
    if(0 == result) {
        int vbcnt = 2;
        FY_MPI_VI_SetChnVBCnt(channel, vbcnt);

        if(2 == g_vi_vb_source) {
            pool_id = __create_user_vbpool(channel, vbcnt + 1, &stChnAttr, 128);
        }

        if(VI_WORK_MODE_1Multiplex == dev_attr.enWorkMode) {
            if((mode == AD_QHD30) || (mode == AD_QHD25) || (mode == AD_4M30) || (mode == AD_4M25)
                || (mode == AD_5M20) || (mode == AD_8M15) || (mode == AD_8M12) || (mode == AD_1080P60)
                || (mode == AD_1080P50) || (mode == AD_6M20) || (mode == AD_8M15V2) || (mode == AD_5M20V2)) {
                if(VI_CLK_EDGE_DOUBLE != dev_attr.enClkEdge) {
                    FY_MPI_VI_DisableDev(dev);
                    __set_dev_attr(dev, FY_TRUE, VI_WORK_MODE_1Multiplex);
                    FY_MPI_VI_EnableDev(dev);
                }
            } else {
                if(VI_CLK_EDGE_DOUBLE == dev_attr.enClkEdge) {
                    FY_MPI_VI_DisableDev(dev);
                    __set_dev_attr(dev, FY_FALSE, VI_WORK_MODE_1Multiplex);
                    FY_MPI_VI_EnableDev(dev);
                }
            }
        }
        result = FY_MPI_VI_SetChnAttr(channel, &stChnAttr);
        if(0 == result) {
            if((2 == g_vi_vb_source) && (VB_INVALID_POOLID != pool_id)) {
                FY_MPI_VI_AttachVbPool(channel, pool_id);
            }
            //set the minor attr
            stChnAttr.s32DstFrameRate = stChnAttr.s32SrcFrameRate;
            stChnAttr.stDestSize.u32Width /= 2;
            stChnAttr.stDestSize.u32Height /= 2;

            if(2 == g_vi_vb_source) {
                pool_id = __create_user_vbpool(channel, vbcnt + 1, &stChnAttr, 128);
            }
            //if(!g_vi_5m)
            //    result = FY_MPI_VI_SetChnMinorAttr(channel, &stChnAttr);
            if((2 == g_vi_vb_source) && (VB_INVALID_POOLID != pool_id)) {
                FY_MPI_VI_AttachVbPool(channel, pool_id);
            }

        }
    }


    return result;
}


static int __start_channel(int channel)
{
    int result = 0;


    result = FY_MPI_VI_EnableChn(channel);

    return result;
}

static int __stop_channel(int channel)
{
    int result = 0;

    result = FY_MPI_VI_DisableChn(channel);

    if(2 == g_vi_vb_source) {
        FY_MPI_VI_DetachVbPool(channel);
    }

    return result;
}


static int __start_channel_nosignal(int chn)
{
    int result = 0;
    int ochn = chn;

    result = __set_channel_attr(chn, AD_720P25, 0);
    __start_channel(chn);
    FY_MPI_VI_DisableChnInterrupt(ochn);
    FY_MPI_VI_EnableUserPic(ochn);

    return result;
}


static int __set_user_pic(int chn, int width, int height, char* fname)
{
    int ret     = 0;
    FILE  *fp   = NULL;

    if(NULL == fname) {
        printf("ERR: SetUserPic must give a file!\n");
        ret = -1;
    } else {
        fp = fopen(fname, "rb");
        if(NULL == fp) {
            printf("ERR: SetUserPic open the file:%s failed!\n", fname);
            VI_USERPIC_ATTR_S userPic;
            memset(&userPic, 0, sizeof(userPic));
            userPic.bPub = FY_TRUE;
            userPic.enUsrPicMode = VI_USERPIC_MODE_BGC;
            userPic.unUsrPic.stUsrPicBg.u32BgColor = 0x4169e1;
            ret = FY_MPI_VI_SetUserPic(chn, &userPic);

        } else {
            FY_U32 u32Size = width * height * 2;
            VB_BLK VbBlk;
            FY_U32 u32PhyAddr;
            FY_U8 *pVirAddr;
            VI_USERPIC_ATTR_S userPic;
            VIDEO_FRAME_INFO_S *pstVFrameInfo = &userPic.unUsrPic.stUsrPicFrm;

            memset(&userPic, 0, sizeof(userPic));
            userPic.bPub = FY_TRUE;
            userPic.enUsrPicMode = VI_USERPIC_MODE_PIC;

            /* get video buffer block form common pool */
            VbBlk = FY_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
            if(VB_INVALID_HANDLE == VbBlk) {
                printf("ERR: SetUserPic FY_MPI_VB_GetBlock failed!\n");
                return -1;
            }
            /* get physical address*/
            u32PhyAddr = FY_MPI_VB_Handle2PhysAddr(VbBlk);
            if(0 == u32PhyAddr) {
                printf("ERR: SetUserPic FY_MPI_VB_Handle2PhysAddr failed!\n");
                return -1;
            }
            pVirAddr = FY_MPI_SYS_Mmap(u32PhyAddr, u32Size);
            if(NULL == pVirAddr) {
                printf("ERR: SetUserPic FY_MPI_SYS_MmapCache failed!\n");
                FY_MPI_VB_ReleaseBlock(VbBlk);
                return -1;
            }
            //read the yuv data from the file
            fread(pVirAddr, 1, u32Size, fp);
            fclose(fp);
            FY_MPI_SYS_Munmap(pVirAddr, u32Size);

            /* get pool id */
            pstVFrameInfo->u32PoolId = FY_MPI_VB_Handle2PoolId(VbBlk);
            if(VB_INVALID_POOLID == pstVFrameInfo->u32PoolId) {
                FY_MPI_VB_ReleaseBlock(VbBlk);
                return -1;
            }

            pstVFrameInfo->stVFrame.u32PhyAddr[0]   = u32PhyAddr;
            pstVFrameInfo->stVFrame.u32PhyAddr[1]   = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32Size / 2;
            pstVFrameInfo->stVFrame.u32Width        = width;
            pstVFrameInfo->stVFrame.u32Height       = height;
            pstVFrameInfo->stVFrame.u32Stride[0]    = width;
            pstVFrameInfo->stVFrame.u32Stride[1]    = width;
            pstVFrameInfo->stVFrame.enPixelFormat   = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
            pstVFrameInfo->stVFrame.u32Field        = VIDEO_FIELD_FRAME;
            pstVFrameInfo->stVFrame.enVideoFormat   = VIDEO_FORMAT_LINEAR;
            pstVFrameInfo->stVFrame.enCompressMode  = COMPRESS_MODE_NONE;
            pstVFrameInfo->stVFrame.s16OffsetLeft   = 0;
            pstVFrameInfo->stVFrame.s16OffsetTop    = 0;
            pstVFrameInfo->stVFrame.s16OffsetRight  = width ;
            pstVFrameInfo->stVFrame.s16OffsetBottom = height;
            pstVFrameInfo->stVFrame.u32TimeRef      = 0;
            FY_MPI_SYS_GetCurPts(&pstVFrameInfo->stVFrame.u64pts);
            ret = FY_MPI_VI_SetUserPic(chn, &userPic);
        }
    }

    return ret;
}

int sample_vi_block_test(int chn)
{
    int i = 0;
    VI_BLOCK_S block;
    int min_margin = 100;
    int vi_width, vi_height;
    int result = 0;
    VI_CHN_ATTR_S attr;

    result = FY_MPI_VI_GetChnAttr(chn, &attr);

    if(!result) {
        return result;
    }

    vi_width = attr.stDestSize.u32Width;
    vi_height = attr.stDestSize.u32Height;


    memset(&block, 0, sizeof(block));
    for(i = 0; i < 4; i++) {

        block.bBlkByPass[i] = 1;

        int r = rand() % (vi_width / 2 - min_margin);
        block.stBlkRect[i].s32X = r * 2;
        block.stBlkRect[i].u32Width = 2 * (min_margin +  rand() % (vi_width / 2  - 1 - r - min_margin));
        r = rand() % (vi_height / 2 - min_margin);
        block.stBlkRect[i].s32Y = r * 2;
        block.stBlkRect[i].u32Height = 2 * (min_margin + rand() % (vi_height / 2 - 1 - r - min_margin));
        block.stBlkColor[i].s32YColor = rand() %  0xff;
        block.stBlkColor[i].s32CrColor = rand() %  0xff;
        block.stBlkColor[i].s32CbColor = rand() %  0xff;
    }

    FY_MPI_VI_SetBlock(chn, &block);

    return 0;
}

int sample_vi_stop()
{
    int result = 0;
    result = FY_MPI_VI_DisableDev(0);
    result = FY_MPI_VI_DisableDev(1);
    s_vitestcase = -1;
    return result;
}

int sample_vi_dumpframe()
{
    int w = 0;
    int h = 0;
    struct stat st = {0};


    if(stat(s_vi_dir, &st) == -1) {
        if(mkdir(s_vi_dir, 0755)) {
            printf("%s: create dir: %s failed!\n", __func__, s_vi_dir);
            return -1;
        }
    }

    switch(s_vitestcase) {
    case 0:// 4 1080P lite
        w = 960;
        h = 1080;
        break;
    case 1:// 4 720P
        w = 1280;
        h = 720;
        break;
    case 2:// 4 cvbs
        w = 960;
        h = (s_vi_fps == 30 ? 480 : 576);
        break;
    case 3:// 2 1080p
        w = 1920;
        h = 1080;
        break;
    default:
        w = 0;
        break;
    }
    if(w > 0) {
        __vi_dump_channel(w, h);
    }
    return 0;


}

int sample_vi_reset()
{
    int result = 0;
    int chn = 0;
    char* fpic = "./userframe.yuv";

    sample_vi_stop();

    FY_MPI_VI_SetEC(s_vi_ec);


    //set the user pic
    result = __set_user_pic(chn, 960, 540, fpic);
    if(result) {
        printf("user frame: %s failed!\n", fpic);
        return -1;
    }

    result = __set_dev_attr(0, FY_TRUE, VI_WORK_MODE_4Multiplex);
    if(s_vi_channel_ns > VIU_MAX_CHN_NUM_PER_DEV) {
        result = __set_dev_attr(1, FY_TRUE, VI_WORK_MODE_4Multiplex);
    }

    for(chn = 0; chn < s_vi_channel_ns; chn++) {
        result = __set_channel_attr(chn, AD_720P25, 0);
        __start_channel(chn);
        FY_MPI_VI_DisableChnInterrupt(chn);
        FY_MPI_VI_EnableUserPic(chn);

    }


    return result;


}


int sample_vi_testmode(int testmode)
{
    s_vi_testmode = testmode;
    if(testmode > 0) {
        s_vi_channel_ns = 8;
    } else {
        s_vi_channel_ns = 4;
    }
    return 0;
}

int sample_vi_set_chn_ns(int ns)
{
    if(ns == 4 || ns == 8) {
        s_vi_channel_ns = ns;
    } else {
        printf("%s: the numbers of channels must be 4 or 8, not %d\n", __func__, ns);
    }
    return 0;

}
int sample_vi_camera_cmd(int cmd)
{
    int result = 0;
    static FY_U8 ptz_cmds[][8] = {
        {0xb5, 0x00, 0x17, 0x00, 0x5f, 0x00, 0x00, 0x2b}, //menu
        {0xb5, 0x00, 0x06, 0x00, 0x3f, 0x00, 0x00, 0xfa}, //up
        {0xb5, 0x00, 0x07, 0x00, 0x3f, 0x00, 0x00, 0xfb}, //down
        {0xb5, 0x00, 0x09, 0x00, 0x3f, 0x00, 0x00, 0xfd}, //left
        {0xb5, 0x00, 0x08, 0x00, 0x3f, 0x00, 0x00, 0xfc}, //right
    };
    ad_PTZ_data ptz;
    if(NULL == s_pAdDeivce) {
        printf("%s: ad device not open\n", __func__);
        return -1;
    }
    if(cmd < 1 || cmd > 4) {
        printf("%s: error command: %d\n", __func__, cmd);
        return -1;
    }
    memset(&ptz, 0, sizeof(ptz));
    ptz.chip = 0;
    ptz.ch   = 0;
    ptz.mode = AD_PTZ_TVI;
    memcpy(ptz.data, ptz_cmds[cmd - 1], 8);
    result = s_pAdDeivce->fpSetPTZData(&ptz);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl AD_SET_PTZ_DATA failed! ret = 0x%x\n", __func__, result);
    }
    return result;
}

int sample_vi_query()
{
    int result = 0;
    VI_CHN_STAT_S st;

    int chn = 0;
    //master
    for(chn = 0; chn < s_vi_channel_ns; chn++) {
        memset(&st, 0, sizeof(VI_CHN_STAT_S));
        FY_MPI_VI_Query(chn, &st);
        if(FY_TRUE == st.bEnable) {
            if(st.u32FrmRate < 12) {
                printf("%s: VIU CH[%d:M]: fps = %d\n", __func__, chn, st.u32FrmRate);
                result += -1;
            }
        }
    }
    //slave
    for(chn = 0; chn < s_vi_channel_ns; chn++) {
        memset(&st, 0, sizeof(VI_CHN_STAT_S));
        FY_MPI_VI_Query(chn + 16, &st);
        if(FY_TRUE == st.bEnable) {
            if(st.u32FrmRate < 25) {
                printf("%s: VIU CH[%d:S]: fps = %d\n", __func__, chn, st.u32FrmRate);
                result += -1;
            }
        }
    }
    return result;
}

void sample_vi_set_5M(int bUse5M)
{
    g_vi_5m = bUse5M;

}

int sample_vi_set_buff(int size)
{
    int ret = 0;
    VI_MOD_PARAM_S mpara;

    FY_MPI_VI_GetModParam(&mpara);
    mpara.buff_size = size;
    ret = FY_MPI_VI_SetModParam(&mpara);
    return ret;

}


#define HALF_MODE(x) ((x) | 0x40)

static void* thread_vi(void *arg)
{
    int result = 0;
    int chn;
    ad_video_loss video_loss;
    ad_video_mode video_mode;
    static FY_U32 sa_videomode[VIU_MAX_CHN_NUM] = {0};
    FY_U32 u32Mode;
    int i;

    memset(sa_videomode, 0xff, sizeof(sa_videomode));

    while(!bThreadStop) {
        for(i = 0; i < g_stViChns.count; i++) {
            video_loss.chip    = g_stViChns.chns[i].ad_chip;
            video_loss.ch      = g_stViChns.chns[i].ad_chn;
            video_loss.is_lost = 1;
            chn = g_stViChns.chns[i].chn;
            result = s_pAdDeivce->fpGetVideLoss(&video_loss);
            if(FY_SUCCESS != result) {
                printf("%s: ioctl AD_GET_VIDEO_LOSS failed! ret = 0x%x\n", __func__, result);
                goto thread_exit;
            }
            if(0 == video_loss.is_lost) {
                //signal locked
                memset(&video_mode, 0, sizeof(video_mode));
                video_mode.chip    = g_stViChns.chns[i].ad_chip;
                video_mode.ch      = g_stViChns.chns[i].ad_chn;
                result = s_pAdDeivce->fpGetVideMode(&video_mode);
                if(FY_SUCCESS != result) {
                    printf("%s: ioctl AD_GET_VIDEO_LOSS failed! ret = 0x%x\n", __func__, result);
                    goto thread_exit;
                }
                //playback the channel by the video mode
                u32Mode = (video_mode.std << 16) | video_mode.mode;
                if(u32Mode != sa_videomode[chn]) {

                    printf("Detect CHN[%d:%d] = %s:%c(0x%x), mod = %s(0x%x)\n",
                           g_stViChns.chns[i].ad_chip, g_stViChns.chns[i].ad_chn,
                           AD_Mod_String(video_mode.mode & (~0x40)),
                           (video_mode.mode & 0x40) > 0 ? 'N' : 'F',
                           video_mode.mode,
                           AD_Std_String(video_mode.std), video_mode.std);


                    sa_videomode[chn] = u32Mode;
                    __stop_channel(chn);
                    result = __set_channel_attr(chn, video_mode.mode, video_mode.std);
                    if(0 == result) {
                        __start_channel(chn);
                    } else {
                        __start_channel_nosignal(chn);
                        region_set_info(chn, "UserPic:720P25");
                    }
                    //sample_vi_block_test(chn, type);

                }
            } else {
                //no signal
                if(sa_videomode[chn] != 0xff) {

                    __stop_channel(chn);
                    __start_channel_nosignal(chn);
                    sa_videomode[chn] = 0xff;
                    region_set_info(chn, "UserPic:720P25");
                }

            }
            usleep(10000);
        }
        usleep(1000000);
    }

thread_exit:
    bThreadStop = FY_TRUE;
    printf("exit the %s\n", __func__);

    return NULL;
}



static int sample_vi_ad_chn_set_fy01(int ad_chip, int admode)
{
    int result = 0;
    VI_WORK_MODE_E mux = VI_WORK_MODE_4Multiplex;
    printf("[VI]: ad chip = %d, output = %d\n", ad_chip, admode);
    switch(admode) {
    case AD_2MUX:
        mux = VI_WORK_MODE_2Multiplex;
        printf("[VI]: set to 2MUX\n");
        break;
    case AD_4MUX:
        mux = VI_WORK_MODE_4Multiplex;
        printf("[VI]: set to 4MUX\n");
        break;
    case AD_1MUX:
        mux = VI_WORK_MODE_1Multiplex;
        printf("[VI]: set to 1MUX\n");
        break;
    default:
        printf("[VI]: unsupport ad mode: %d!\n", admode);
        break;
    }

    result = __set_dev_attr(ad_chip, FY_TRUE, mux);
    if(mux == VI_WORK_MODE_2Multiplex) {
        result = __set_dev_attr(ad_chip * 2 + 1, FY_TRUE, mux);
    } else if(mux == VI_WORK_MODE_1Multiplex) {
        result = __set_dev_attr(ad_chip * 2 + 1, FY_FALSE, mux);
    }

    if(mux == VI_WORK_MODE_2Multiplex) {

        VI_CHN_BIND_ATTR_S bind;
        int chn;

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 0;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 1;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip;
        bind.ViWay = 1;
        FY_MPI_VI_BindChn(chn, &bind);

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 2;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip + 1;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip + 1;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 3;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip + 1;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip + 1;
        bind.ViWay = 1;
        FY_MPI_VI_BindChn(chn, &bind);

    } else if(mux == VI_WORK_MODE_4Multiplex) {

        VI_CHN_BIND_ATTR_S bind;
        int i = 0;

        for(i = 0; i < 4; i++) {
            int chn = g_stViChns.count;
            g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
            g_stViChns.chns[g_stViChns.count].ad_chn  = i;
            g_stViChns.chns[g_stViChns.count].dev     = ad_chip;
            g_stViChns.chns[g_stViChns.count].chn     = chn;
            ++g_stViChns.count;

            FY_MPI_VI_UnBindChn(chn);
            bind.ViDev = ad_chip;
            bind.ViWay = i;
            FY_MPI_VI_BindChn(chn, &bind);
        }

    } else if(mux == VI_WORK_MODE_1Multiplex) {

        VI_CHN_BIND_ATTR_S bind;

        int chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 0;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 1;
        g_stViChns.chns[g_stViChns.count].dev     = ad_chip + 1;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;
        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = ad_chip + 1;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);

    }

    return result;


}


static int sample_vi_ad_chn_set_fy02(int ad_chip, int admode)
{
    int result = 0;
    int dev_id = ad_chip;
    VI_WORK_MODE_E mux = VI_WORK_MODE_4Multiplex;
    printf("[VI]: ad chip = %d, output = %d\n", ad_chip, admode);
    switch(admode) {
    case AD_2MUX:
        mux = VI_WORK_MODE_2Multiplex;
        printf("[VI]: set to 2MUX\n");
        break;
    case AD_4MUX:
        mux = VI_WORK_MODE_4Multiplex;
        printf("[VI]: set to 4MUX\n");
        break;
    case AD_1MUX:
        mux = VI_WORK_MODE_1Multiplex;
        printf("[VI]: set to 1MUX\n");
        break;
    default:
        printf("[VI]: unsupport ad mode: %d!\n", admode);
        break;
    }

    dev_id = ad_chip;
    if(0 == strcmp("fh6210", s_pAdDeivce->name)) {
        if(AD_2MUX == admode) {
            int channels[] = {2, 3, 0, 1};
            s_pAdDeivce->fpSetChnBind(ad_chip, channels);
        }
    }

    result = __set_dev_attr(dev_id, FY_TRUE, mux);

    if(mux == VI_WORK_MODE_2Multiplex) {

        VI_CHN_BIND_ATTR_S bind;
        int chn;

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 0;
        g_stViChns.chns[g_stViChns.count].dev     = dev_id;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = dev_id;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 1;
        g_stViChns.chns[g_stViChns.count].dev     = dev_id;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = dev_id;
        bind.ViWay = 1;
        FY_MPI_VI_BindChn(chn, &bind);

    } else if(mux == VI_WORK_MODE_4Multiplex) {

        VI_CHN_BIND_ATTR_S bind;
        int i = 0;

        for(i = 0; i < 4; i++) {
            int chn = g_stViChns.count;
            g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
            g_stViChns.chns[g_stViChns.count].ad_chn  = i;
            g_stViChns.chns[g_stViChns.count].dev     = dev_id;
            g_stViChns.chns[g_stViChns.count].chn     = chn;
            ++g_stViChns.count;

            FY_MPI_VI_UnBindChn(chn);
            bind.ViDev = dev_id;
            bind.ViWay = i;
            FY_MPI_VI_BindChn(chn, &bind);
        }

    } else if(mux == VI_WORK_MODE_1Multiplex) {

        VI_CHN_BIND_ATTR_S bind;
        int chn;

        chn = g_stViChns.count;
        g_stViChns.chns[g_stViChns.count].ad_chip = ad_chip;
        g_stViChns.chns[g_stViChns.count].ad_chn  = 0;
        g_stViChns.chns[g_stViChns.count].dev     = dev_id;
        g_stViChns.chns[g_stViChns.count].chn     = chn;
        ++g_stViChns.count;

        FY_MPI_VI_UnBindChn(chn);
        bind.ViDev = dev_id;
        bind.ViWay = 0;
        FY_MPI_VI_BindChn(chn, &bind);
    }

    return result;


}
static int sample_vi_ad_chn_set(FY_BOOL bFY02, int ad_chip, int admode)
{
    int result = 0;
    if(FY_TRUE == bFY02) {
        result = sample_vi_ad_chn_set_fy02(ad_chip, admode);
    } else {
        result = sample_vi_ad_chn_set_fy01(ad_chip, admode);
    }

    return result;
}

static int sample_vi_init_ad()
{
    int result = 0;
    if(s_pAdDeivce) {
        return 0;
    }
    //init ad devices
    result = ad_device_init();

    //get the ad device
    s_pAdDeivce = NULL;
    result = access("/sys/module/tp2830", R_OK);
    if(0 == result) {
        s_pAdDeivce = ad_device_find("tp2830");
    } else {
        result = access("/sys/module/fh6210", R_OK);
        if(0 == result) {
            s_pAdDeivce = ad_device_find("fh6210");
        }
    }
    if(NULL == s_pAdDeivce) {
        printf("No AD device found!\n");
        return -1;
    }
    return result;

}


int sample_vi_init_channels(FY_BOOL bFY02, struct vi_channels_info *pstViChn)
{
    int result =0;
    int ad_chips = 0;
    int admode, i;
    int chns_by_mux = 0;

    sample_vi_init_ad();
    if(NULL == s_pAdDeivce) {
        printf("%s: s_pAdDeivce == NULL !\n", __func__);
        return -1;
    }

    memset(&g_stViChns, 0xff, sizeof(g_stViChns));
    g_stViChns.count = 0;

    //get chips for ad
    ad_chips = s_pAdDeivce->fpGetChips();

    i = 0;
    s_vi_channel_ns = 0;
    chns_by_mux = 0;
    for(i = 0; i < ad_chips; i++) {
        admode = s_pAdDeivce->fpGetMuxMode(i);
        printf("%s: Mux in Chip[%d] = %d\n", __func__, i, admode);
        if(pstViChn == NULL) {
            sample_vi_ad_chn_set(bFY02, i, admode);
        }
        if(admode == AD_2MUX) { //2 mux
            if(bFY02) {
                chns_by_mux += (2 << 8);
                s_vi_channel_ns += 2;
            } else {
                chns_by_mux += (4 << 8);
                s_vi_channel_ns += 4;
            }
        } else if(admode == AD_4MUX) { //4 mux
            chns_by_mux += 4;
            s_vi_channel_ns += 4;
        } else if(admode == AD_1MUX) { //1 mux
            if(bFY02) {
                chns_by_mux += (1 << 16);
                s_vi_channel_ns += 1;
            } else {
                chns_by_mux += (4 << 16);
                s_vi_channel_ns += 4;
            }
        }
    }
    g_stViChns.ad_chips = ad_chips;

    if(pstViChn) {
        pstViChn->ad_chips = ad_chips;
        pstViChn->channels = s_vi_channel_ns;
        pstViChn->channels_by_mux = chns_by_mux;
    }

    result =  s_vi_channel_ns;
    return result;

}

int sample_vi_init(FY_BOOL bFY02, VI_EC_LEVEL_E ec, PIXEL_FORMAT_E pixelformat)
{
    int result = 0;
    int chn = 0;
    char* fpic = "./userframe.yuv";

    s_vi_ec = ec;
   //set compress factor
    FY_MPI_VI_SetEC(s_vi_ec);


    sample_vi_init_channels(bFY02, NULL);

    //get vi_vb_source
    SAMPLE_COMM_Get_Mod_Param("viu", "vi_vb_source", 0, &g_vi_vb_source);
    printf("[VI]: vb source = %s\n", g_vi_vb_source == 2 ? "UserVB" : "CommonVB");

    g_vi_pixelformat = pixelformat;



    //set the user pic
    result = __set_user_pic(chn, 960, 540, fpic);
    if(result) {
        printf("user frame: %s failed!\n", fpic);
        goto init_exit;
    }

    if(s_pAdDeivce) {
        //start the vi thread
        bThreadStop = FY_FALSE;
        result = pthread_create(&thread_id_vi, 0, &thread_vi, NULL);
        if(result != 0) {
            printf("%s: create vi thread failed! result = %d\n", __func__, result);
            goto init_exit;
        }
    } else {
        for(chn = 0; chn < s_vi_channel_ns; chn++) {
            result = __set_channel_attr(chn, AD_720P25, 0);
            __start_channel(chn);
            FY_MPI_VI_DisableChnInterrupt(chn);
            FY_MPI_VI_EnableUserPic(chn);

        }
    }
    result = s_vi_channel_ns;

init_exit:

    return result;

}

int sample_vi_exit()
{
    sample_vi_stop();
    //stop the viu thread
    bThreadStop = FY_TRUE;
    if(thread_id_vi) {
        pthread_join(thread_id_vi, NULL);
    }

    ad_device_cleanup();

    return 0;
}
