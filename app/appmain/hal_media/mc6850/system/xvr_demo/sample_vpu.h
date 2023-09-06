#ifndef __SVR_SAMPLE_VPU_H__
#define __SVR_SAMPLE_VPU_H__

#define VEU_BEGIN_GRP       0
#define JPEG_BEGION_GRP     32
#define WBC_VPU_GRP_BEGION  64

#define WBC_TO_VO   VPSS_CHN1
#define WBC_TO_VENC VPSS_CHN0

FY_S32 sample_vpu_init_param(VPSS_GRP_ATTR_S *pstVpssGrpAttr);
FY_S32 sample_vpu_start(FY_S32 s32BeginGrp,FY_S32 s32GrpCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr,FY_BOOL bRegion);
FY_S32 sample_vpu_stop(FY_S32 s32BeginGrp,FY_S32 s32GrpCnt, FY_S32 s32ChnCnt,FY_BOOL bRegion);
FY_S32 sample_vpu_start_userchn(FY_S32 s32GrpCnt,VPSS_CHN VpssChn,VPSS_CHN_MODE_S *pstVpssMode);
FY_S32 sample_vi_bind_vpu(FY_U32 u32GrpNum);
FY_S32 sample_vi_unbind_vpu(FY_U32 u32GrpNum);
FY_S32 sample_vi_bind_vpu_mix(FY_U32 u32GrpNum);
FY_S32 sample_vi_unbind_vpu_mix(FY_U32 u32GrpNum);
FY_S32 sample_vpu_disable_preview(FY_S32 s32GrpCnt,VPSS_CHN VpssChn);
FY_S32 sample_vpu_enable_preview(FY_S32 s32GrpCnt,VPSS_CHN VpssChn);
// FY_BOOL sample_vpu_chn_bshow(FY_S32 s32GrpCnt);
FY_BOOL sample_vpu_chn_bshow(FY_S32 s32GrpCnt,VPSS_GRP VpssGrp);
FY_S32 sample_vpu_check_get_frame(VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
FY_S32 sample_vgs_bind_vpu(VGS_CHN VgsChn,VGS_PTH  VgsPth,VPSS_GRP VpssGrp);
FY_S32 sample_vgs_unbind_vpu(VGS_CHN VgsChn,VGS_PTH  VgsPth,VPSS_GRP VpssGrp);
FY_VOID sample_vpu_set_chip(FY_U32 u32ChipID);
FY_U32 sample_vpu_get_chip();


#define SAMPLE_VPU_PRT(msg, ...)   \
    do {\
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
       }while(0)


#define SAMPLE_VPU_CHECK(cond, msg, ...)  \
      if(cond) { \
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
      } \

#define SAMPLE_VPU_CHECK_GOTO(cond, label, msg, ...)  \
      if(cond) { \
          printf("[Func]:%s [Line]:%d: " msg "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__); \
          goto label; \
      } \

#endif //ifndef __SVR_SAMPLE_VI_H__

