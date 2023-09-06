#ifndef __SAMPLE_COMM_PREVIEW_H__
#define __SAMPLE_COMM_PREVIEW_H__

#include "fy_common.h"
#include "sample_vpu.h"
#include "sample_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef void (*led_onoff)(int, FY_BOOL);

FY_S32 sample_preview_init(FY_U32 u32GrpCnt);
//FY_S32 sample_preview_init(FY_VOID);
FY_S32 sample_preview_deinit(FY_VOID);
FY_S32  sample_preview_stop(FY_VOID);
FY_S32 sample_preview_start(SAMPLE_VO_MODE_E enVoMode,FY_BOOL autoFlag);
FY_S32  sample_preview_stop_show(FY_VOID);
FY_S32 sample_preview_set_led_cb(led_onoff pLedOnoff);
FY_S32 sample_preview_check(FY_VOID);
FY_S32 sample_get_grp_number(FY_VOID);
FY_S32 sample_set_vpu_modparam(FY_VOID);
FY_VOID sample_set_osd_showtime(FY_BOOL bFlag);
FY_S32 sample_get_osd_showtime(FY_BOOL bFlag);
FY_VOID sample_preview_set_5M(int bUse5M);
FY_S32 sample_ele_preview_start(FY_BOOL bUp);
FY_VOID sample_PIP_enable_pre(FY_BOOL bEnablePIP);


#define SAMPLE_PREVIEW_PRT(msg, ...)   \
    do {\
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
       }while(0)

#define SAMPLE_PRE_CHECK_GOTO(cond, label, msg, ...)  \
      if(cond) { \
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
          goto label; \
      } \


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_COMM_PLAYBACK_H__
