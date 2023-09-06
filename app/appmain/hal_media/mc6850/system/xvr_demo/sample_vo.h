#ifndef __SVR_SAMPLE_VO_H__
#define __SVR_SAMPLE_VO_H__

//#define SAMPLE_VO_DEBUG

/**
 test cases
	 0: 1024x768P60, 1: 720P60,  2: 1280x1024P60, 3: 1080P60
*/
FY_S32 sample_vo_enable_lcd(FY_BOOL bEnable);

FY_S32 sample_vo_init(FY_BOOL bIsHD, VO_INTF_SYNC_E enIntfSync);

FY_S32 sample_vo_set_compress(FY_BOOL bEnable);

FY_S32 sample_vo_start_all(FY_BOOL bIsHD, SAMPLE_VO_MODE_E mode);

FY_S32 sample_vo_stop_all(FY_BOOL bIsHD, SAMPLE_VO_MODE_E mode);

FY_S32 sample_vo_start_one(FY_BOOL bIsHD, SAMPLE_VO_MODE_E mode, VO_CHN voChn);

FY_S32 sample_vo_stop_one(FY_BOOL bIsHD, SAMPLE_VO_MODE_E mode, VO_CHN voChn);

FY_S32 sample_vo_start_wbc(FY_BOOL bBind);

FY_S32 sample_vo_stop_wbc(FY_BOOL bBind);

FY_VOID sample_PIP_enable(FY_BOOL bEnablePIP);

FY_S32 sample_vo_deinit(FY_BOOL bIsHD);

#ifdef SAMPLE_VO_DEBUG
#define SAMPLE_VO_PRT(msg, ...)   \
    do {\
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
       }while(0)
#else
#define SAMPLE_VO_PRT(msg, ...) do{}while(0)
#endif

#define SAMPLE_VO_CHECK(cond, msg, ...)  \
      if(cond) { \
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
      } \

#define SAMPLE_VO_CHECK_GOTO(cond, label, msg, ...)  \
      if(cond) { \
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
          goto label; \
      } \

#endif //ifndef __SVR_SAMPLE_VI_H__

