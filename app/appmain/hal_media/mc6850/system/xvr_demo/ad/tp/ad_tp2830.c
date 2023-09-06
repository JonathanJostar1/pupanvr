#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "sample_comm.h"
#include "tp2802.h"
#include "list.h"
#include "../../sample_ad.h"


/* global variable for tp28xx */
static int fd_ad = -1;
static int ad_chips = 0;
static int ad_mux[4] = {0};




static int tp_GetVideLoss(ad_video_loss *pLoss)
{
    int result = 0;
    tp2802_video_loss video_loss;
    if(NULL == pLoss) {
        return EN_ERR_ILLEGAL_PARAM;
    }
    video_loss.chip    = pLoss->chip;
    video_loss.ch      = pLoss->ch;
    video_loss.is_lost = 1;
    result = ioctl(fd_ad, TP2802_GET_VIDEO_LOSS, (void*)&video_loss);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl TP2802_GET_VIDEO_LOSS failed! ret = 0x%x\n", __func__, result);

    }
    pLoss->is_lost = video_loss.is_lost;
    return result;
}


static int tp_GetVideMode(ad_video_mode *pMode)
{
    int result = 0;
    tp2802_video_mode video_mode;
    if(NULL == pMode) {
        return -EN_ERR_ILLEGAL_PARAM;
    }

    memset(&video_mode, 0, sizeof(video_mode));
    video_mode.chip    = pMode->chip;
    video_mode.ch      = pMode->ch;
    result = ioctl(fd_ad, TP2802_GET_VIDEO_MODE, (void*)&video_mode);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl TP2802_GET_VIDEO_MODE failed! ret = 0x%x\n", __func__, result);

    } else {
        pMode->mode = video_mode.mode;
        pMode->std  = video_mode.std;
    }
    return result;
}

static int tp_GetChips()
{
    return ad_chips;
}

static int tp_GetMuxMode(int chip)
{
    if((chip >= ad_chips) || (chip < 0)) {
        return -EN_ERR_ILLEGAL_PARAM;
    }
    return ad_mux[chip];
}

static int tp_SetPTZData(ad_PTZ_data *pPtzData)
{
    int result = 0;
    tp2802_PTZ_data ptz;
    if(NULL == pPtzData) {
        return -EN_ERR_ILLEGAL_PARAM;
    }

    memset(&ptz, 0, sizeof(ptz));
    ptz.chip = pPtzData->chip;
    ptz.ch   = pPtzData->ch;
    ptz.mode = pPtzData->mode;
    memcpy(ptz.data, pPtzData->data, 16);
    result = ioctl(fd_ad, TP2802_SET_PTZ_DATA, (void*)&ptz);
    if(FY_SUCCESS != result) {
        printf("%s: ioctl TP2802_SET_PTZ_DATA failed! ret = 0x%x\n", __func__, result);
    }
    return result;

}

int tp_SetChnBind(int chip, int *chns)
{
    int result = 0;


    return result;

}

static int tp_Init()
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
    result = SAMPLE_COMM_Get_Mod_Param("tp2830", "chips", 1, &ad_chips);
    if(result < 0) {
        return result;
    }

    printf("ad chips = %d\n", ad_chips);
    //get ad output
    SAMPLE_COMM_Get_Mod_Param_Str("tp2830", "output", outputs, 32);
    printf("ad mode = %s\n", outputs);

    i = 0;
    s = outputs;
    while((p = strsep(&s, ",")) != NULL) {
        admode = atoi(p);
        if(admode == 3) {
            ad_mux[i] = AD_4MUX;
        } else if(admode == 2) {
            ad_mux[i] = AD_2MUX;
        } else if(admode == 4) {
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
        fd_ad = open("/dev/tp2830", O_RDWR, 0);
        if(fd_ad < 0) {
            printf("open AD device [/dev/tp2830] failed, ret:%d \n", fd_ad);
            result = -EN_ERR_UNEXIST;
        }
    }

    return result;


}

static int tp_Exit()
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

static ad_device tp28xx_device = {
    .name = "tp2830",
    .fpGetChips = tp_GetChips,
    .fpGetMuxMode = tp_GetMuxMode,
    .fpGetVideLoss = tp_GetVideLoss,
    .fpGetVideMode = tp_GetVideMode,
    .fpSetPTZData = tp_SetPTZData,
    .fpSetChnBind = tp_SetChnBind,
    .fpInit = tp_Init,
    .fpExit = tp_Exit,

};

ad_device *p_tp_dev __attribute__((section("ad_device_section")))  = &tp28xx_device;


