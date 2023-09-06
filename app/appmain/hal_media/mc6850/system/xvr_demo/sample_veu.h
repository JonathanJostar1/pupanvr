#ifndef __SAMPLE_VEU_H__
#define __SAMPLE_VEU_H__

#include "fy_common.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VENC_MAX_WIDTH  2592
#define VENC_MAX_HEIGHT 1952
#define VENC_MIN_WIDTH  176
#define VENC_MIN_HEIGHT 144

#define VENC_MAX_TEST_CHAN_NUM VENC_MAX_CHN_NUM

typedef enum{
    ENC_RES_5M_2592X1952=1,
    ENC_RES_5MN_1280X1952,
    ENC_RES_1920X1088,
    ENC_RES_1280X720,
    ENC_RES_960X1088,
}ENC_RES_E;

FY_S32 sample_encode_test_start(FY_BOOL bAgingTest, FY_BOOL bNvrDemo);
FY_S32 sample_encode_test_stop();
FY_S32 sample_encode_test_auto_switch();
FY_S32 sample_encode_test_switch_pause();
FY_S32 sample_encode_set_chn_num(FY_U32 u32ChnNum);
FY_S32 sample_encode_get_chn_num(FY_U32 *pu32ChnNum);
FY_S32 sample_encode_set_storage_path(char *path);
FY_S32 sample_encode_set_frame_rate(FY_U32 u32FrameRate);
FY_S32 sample_encode_config_chan(VENC_TEST_PARA_S *pChanConfig, FY_U32 u32ChnNum);
FY_BOOL sample_encode_is_started();
FY_S32 sample_encode_set_resolution(FY_U32 w, FY_U32 h);
FY_VOID sample_encode_enable_log(FY_BOOL bEnableLog);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_COMM_PLAYBACK_H__
