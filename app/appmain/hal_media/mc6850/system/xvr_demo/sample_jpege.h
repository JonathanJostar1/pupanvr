#ifndef __SAMPLE_JPEGE_H__
#define __SAMPLE_JPEGE_H__

#include "fy_common.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

FY_S32 sample_jpege_test_start(FY_BOOL bNvrDemo);
FY_S32 sample_jpege_test_stop();
FY_S32 sample_jpege_test_auto_switch();
FY_S32 sample_jpege_test_switch_pause();
FY_S32 sample_jpege_set_chn_num(FY_U32 u32ChnNum);
FY_S32 sample_jpege_get_chn_num(FY_U32 *pu32ChnNum);
FY_S32 sample_jpege_set_snap_num(FY_U32 u32SnapNum);
FY_S32 sample_jpege_manual_test(FY_BOOL bNvrDemo);
FY_S32 sample_jpege_get_default_params(VENC_TEST_PARA_S *pastJpegeTestPara, FY_U32 u32ChanNum);
FY_S32 sample_jpege_set_storage_path(char *path);
FY_BOOL sample_jpege_is_started();
FY_VOID sample_jpege_enable_log(FY_BOOL bEnableLog);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_JPEGE_H__
