#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "sample_comm.h"
#include "fh6210.h"

#include "list.h"
#include "../../sample_ad.h"


/* global variable for tp28xx */
static int fd_ad = -1;
static int ad_chips = 0;
static int ad_mux[4] = {0};


static int yj_GetVideLoss(ad_video_loss *pLoss)
{
    int result = 0;
    fh_video_loss video_loss;
    if(NULL == pLoss) {
        return EN_ERR_ILLEGAL_PARAM;
    }
    video_loss.chip    = pLoss->chip;
    video_loss.chan    = pLoss->ch;
    video_loss.is_lost = 1;
    result = ioctl(fd_ad, FH_GET_VIDEO_STATUS, (void*)&video_loss);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl FH_GET_VIDEO_STATUS failed! ret = 0x%x\n", __func__, result);

    }
    pLoss->is_lost = video_loss.is_lost;
    return result;
}


static int yj_GetVideMode(ad_video_mode *pMode)
{
    int result = 0;
    fh_video_mode video_mode;
    if(NULL == pMode) {
        return -EN_ERR_ILLEGAL_PARAM;
    }

    memset(&video_mode, 0, sizeof(video_mode));
    video_mode.chip    = pMode->chip;
    video_mode.chan    = pMode->ch;
    result = ioctl(fd_ad, FH_GET_VIDEO_MODE, (void*)&video_mode);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl FH_GET_VIDEO_MODE failed! ret = 0x%x\n", __func__, result);

    } else {
        int half = 0;
        int vmode = 0;
        pMode->mode = video_mode.mode;
        pMode->std  = video_mode.std;

        vmode = video_mode.mode;
        if(video_mode.mode > 0x40) {
            half = 0x40;
            vmode -= 0x40;
        }
        if(vmode == AD_4M30) {
            pMode->mode = AD_QHD30;
        } else if(vmode == AD_4M25) {
            pMode->mode = AD_QHD25;
        } else if(vmode == AD_4M15) {
            pMode->mode = AD_QHD15;
        }
        pMode->mode |= half;
    }
    return result;
}

static int yj_GetChips()
{
    return ad_chips;
}

static int yj_GetMuxMode(int chip)
{
    if((chip >= ad_chips) || (chip < 0)) {
        return -EN_ERR_ILLEGAL_PARAM;
    }
    return ad_mux[chip];
}

static int yj_SetPTZData(ad_PTZ_data *pPtzData)
{
    int result = 0;
    fh_ioctl_ptz_type ptz;
    fh_fmt_det fmt;

    if(NULL == pPtzData) {
        return -EN_ERR_ILLEGAL_PARAM;
    }

    fmt.chip = pPtzData->chip;
    fmt.chan = pPtzData->ch;
    fmt.format = -1;

    result =  ioctl(fd_ad, FH_GET_VIDEO_MODE_DETAIL, (void*)&fmt);
    if(result) {
        printf("%s: [%d][%d] FH_GET_VIDEO_MODE_DETAIL. ret = 0x%x\n", __func__, fmt.chip, fmt.chan, result);
        return -1;
    }

    memset(&ptz, 0, sizeof(ptz));
    ptz.bf.chip = pPtzData->chip;
    ptz.bf.chan = pPtzData->ch;
    ptz.bf.format = fmt.format;
    ptz.bf.type = pPtzData->mode;

    result =  ioctl(fd_ad, FH_PTZ_TX_SET, (void*)&ptz);

    return result;

}

int yj_SetChnBind(int chip, int *chns)
{
    int result = 0;
    fh_vd_sel vd;

    if((unsigned)chip >= ad_chips) {
        printf("%s: the chip[%d] should less than the chips[%d]\n", __func__, chip, ad_chips);
        return -1;
    }

    memset(&vd, 0, sizeof(vd));
    vd.chip = chip;
    if(2 == ad_mux[chip]) {
        vd.vd = 0;
        vd.vdSel1 = chns[0];
        vd.vdSel2 = chns[1];
        vd.vdSel3 = 0;
        vd.vdSel4 = 0;
        result = ioctl(fd_ad, FH_VD_SEL_SET, (void*)&vd);
        if(result) {
            printf("%s: the chip[%d] FH_VD_SEL_SET faied! ret = 0x%x\n", __func__, chip, result);
        }

        vd.vd = 1;
        vd.vdSel1 = chns[2];
        vd.vdSel2 = chns[3];
        vd.vdSel3 = 0;
        vd.vdSel4 = 0;
        result = ioctl(fd_ad, FH_VD_SEL_SET, (void*)&vd);
        if(result) {
            printf("%s: the chip[%d],vd[%d] FH_VD_SEL_SET faied! ret = 0x%x\n", __func__, vd.vd, chip, result);
        }
    } else if(4 == ad_mux[chip]) {

        vd.vd = 0;
        vd.vdSel1 = chns[0];
        vd.vdSel2 = chns[1];
        vd.vdSel3 = chns[2];
        vd.vdSel4 = chns[3];
        result = ioctl(fd_ad, FH_VD_SEL_SET, (void*)&vd);
        if(result) {
            printf("%s: the chip[%d],vd[%d] FH_VD_SEL_SET faied! ret = 0x%x\n", __func__, vd.vd, chip, result);
        }

        vd.vd = 1;
        vd.vdSel1 = chns[0];
        vd.vdSel2 = chns[1];
        vd.vdSel3 = chns[2];
        vd.vdSel4 = chns[3];
        result = ioctl(fd_ad, FH_VD_SEL_SET, (void*)&vd);
        if(result) {
            printf("%s: the chip[%d],vd[%d] FH_VD_SEL_SET faied! ret = 0x%x\n", __func__, vd.vd, chip, result);
        }
    }

    return result;

}


static int yj_start()
{
    int ret = 0;

    return ret;
}


static int yj_Init()
{
    int result = 0;
    char outputs[32];
    char *p = NULL, *s = NULL;
    int admode = 0;
    int i = 0;

    //init global variable
    ad_chips = 0;
    memset(ad_mux, 0, sizeof(ad_mux));


    //get chips for ad
    result = SAMPLE_COMM_Get_Mod_Param("fh6210", "chips", 1, &ad_chips);
    if(result < 0) {
        return result;
    }
    printf("ad chips = %d\n", ad_chips);
    //get ad output
    SAMPLE_COMM_Get_Mod_Param_Str("fh6210", "mux", outputs, 32);
    printf("ad mode = %s\n", outputs);

    i = 0;
    s = outputs;
    while((p = strsep(&s, ",")) != NULL) {
        admode = atoi(p);
        if(admode == 4) {
            ad_mux[i] = AD_4MUX;
        } else if(admode == 2) {
            ad_mux[i] = AD_2MUX;
        } else if(admode == 1) {
            ad_mux[i] = AD_1MUX;
        } else {
            ad_mux[i] = AD_0MUX;
        }
        ++i;
        if(i > 3) {
            //max 4 chip
            break;
        }
    }
    result = 0;
    //open the AD device
    if(-1 == fd_ad) {
        fd_ad = open("/dev/fh6210", O_RDWR, 0);
        if(fd_ad < 0) {
            printf("open AD device [/dev/fh6210] failed, ret:%d \n", fd_ad);
            result = -EN_ERR_UNEXIST;
        }
    }

    yj_start();
    return result;


}

static int yj_Exit()
{
    int result = 0;
    if(fd_ad > -1) {
        close(fd_ad);
        fd_ad = -1;
    }
    ad_chips = 0;
    memset(ad_mux, 0, sizeof(ad_mux));

    return result;
}

static ad_device yj_device = {
    .name = "fh6210",
    .fpGetChips = yj_GetChips,
    .fpGetMuxMode = yj_GetMuxMode,
    .fpGetVideLoss = yj_GetVideLoss,
    .fpGetVideMode = yj_GetVideMode,
    .fpSetPTZData = yj_SetPTZData,
    .fpSetChnBind = yj_SetChnBind,
    .fpInit = yj_Init,
    .fpExit = yj_Exit,

};

ad_device *p_yj_dev __attribute__((section("ad_device_section")))  = &yj_device;


