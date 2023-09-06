#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/prctl.h>

#include "sample_comm.h"
#include "sample_encoding.h"
#include "sample_veu.h"
#include "sample_preview.h"

#define VgsPthToVpu    1

#define VGS_PTH1_WIDTH  960
#define VGS_PTH1_HEIGHT 1080

FY_S32 sample_encoding_set_vppu_param(FY_U32 u32ChnIndex)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VGS_CHN_PARA_S stVgsMode;

	s32Ret = FY_MPI_VGS_GetChnMode(u32ChnIndex, VgsPthToVpu, &stVgsMode);
    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get VGS chn%d mode failed, ret=%#x!!\n", u32ChnIndex, s32Ret);
        return FY_FAILURE;
    }

    //SAMPLE_PRT("Get VGS ch%d u32Width=%d, u32Height=%d, enChnMode=%d\n",
    //    u32ChnIndex, stVgsMode.u32Width, stVgsMode.u32Height, stVgsMode.enChnMode);
	stVgsMode.u32Width  = VGS_PTH1_WIDTH;
	stVgsMode.u32Height = VGS_PTH1_HEIGHT;
    stVgsMode.enChnMode = VGS_CHN_MODE_USER;
    stVgsMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    s32Ret = FY_MPI_VGS_SetChnMode(u32ChnIndex, VgsPthToVpu, &stVgsMode);

    if (FY_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get VGS chn%d mode failed, ret=%#x!\n", u32ChnIndex, s32Ret);
        return FY_FAILURE;
    }
    //SAMPLE_PRT("Set VGS ch%d path1 u32Width=%d, u32Height=%d, enChnMode=%d\n",
    //    u32ChnIndex, stVgsMode.u32Width, stVgsMode.u32Height, stVgsMode.enChnMode);

    return s32Ret;
}

FY_S32 sample_encoding_vpu_init(FY_U32 u32EncType,FY_U32 u32GrpCnt)
{
    VPSS_GRP_ATTR_S stGrpAttr;
    FY_U32 i = 0,u32BeginGrp =0;
    FY_S32 s32VpssGrpId = 0;
    FY_S32 s32Ret = FY_SUCCESS;

    if(0 == u32EncType)
        u32BeginGrp = VEU_BEGIN_GRP;
    else if(1 == u32EncType)
        u32BeginGrp = JPEG_BEGION_GRP;
    else
        u32BeginGrp = VEU_BEGIN_GRP;

    /*3.1 start vpss and vgs bind vpss*/
    s32Ret = sample_vpu_init_param(&stGrpAttr);
    if (FY_SUCCESS != s32Ret) {
        SAMPLE_PRT("sample_vpu_init_param failed!\n");
        return s32Ret;
    }

    s32Ret = sample_set_vpu_modparam();
    if (FY_SUCCESS != s32Ret) {
        SAMPLE_PRT("sample_set_vpu_modparam failed!\n");
        goto ERR1;
    }
    
    s32Ret = sample_vpu_start(u32BeginGrp,u32GrpCnt, &stGrpAttr,0);
    if (FY_SUCCESS != s32Ret) {
        SAMPLE_PRT("sample_vpu_start grp:%d  failed!\n",u32GrpCnt);
        goto ERR1;
    }

    for(i=0;i<u32GrpCnt;i++){
        s32VpssGrpId = i+u32BeginGrp;

        s32Ret = sample_encoding_set_vppu_param(i);
        if (FY_SUCCESS != s32Ret) {
            SAMPLE_PRT("sample_vpu_start failed!\n");
            continue;
        }
        s32Ret = sample_vgs_bind_vpu(i,VgsPthToVpu,s32VpssGrpId);
        if (FY_SUCCESS != s32Ret) {
            SAMPLE_PRT("sample_vgs_bind_vpu vgs chn:%d, vpu grp:%d failed!\n",i,i);
            goto ERR1;
        }
    }

    return s32Ret;

ERR1:
    sample_vpu_stop(u32BeginGrp,u32GrpCnt,VPSS_MAX_CHN_NUM,0);
    return s32Ret;
}

FY_S32 sample_encoding_vpu_deinit(FY_U32 u32EncType,FY_U32 u32GrpCnt)
{
    FY_U32 i = 0,u32BeginGrp =0;
    FY_S32 s32VpssGrpId = 0;
    FY_S32 s32Ret = FY_SUCCESS;

    if(0 == u32EncType)
        u32BeginGrp = VEU_BEGIN_GRP;
    else if(1 == u32EncType)
        u32BeginGrp = JPEG_BEGION_GRP;
    else
        u32BeginGrp = VEU_BEGIN_GRP;

    for(i=0;i<u32GrpCnt;i++){
        s32VpssGrpId = i+u32BeginGrp;

        s32Ret = sample_vgs_unbind_vpu(i,VgsPthToVpu,s32VpssGrpId);
        if (FY_SUCCESS != s32Ret) {
            SAMPLE_PRT("sample_vgs_bind_vpu vgs chn:%d, vpu grp:%d failed!\n",i,i);
        }
    }
    s32Ret = sample_vpu_stop(u32BeginGrp,u32GrpCnt,VPSS_MAX_CHN_NUM,0);

    return s32Ret;
}


FY_S32 sample_encoding_perf_init()
{
    FY_U32 u32ChnNum = 1;
    VENC_TEST_PARA_S stVencTestPara[2];
    VENC_TEST_PARA_S *pstChnPara = stVencTestPara;
    FY_S32 s32Ret = FY_SUCCESS;

    //init vpu and bind vpu with vppu, one channel
    s32Ret = sample_encoding_vpu_init(0/*veu*/, 1);
    if(s32Ret != FY_SUCCESS)
    {
         sample_encoding_vpu_deinit(0,1);
         return FY_FAILURE;
    }
    //configure veu channel params   
    memset(stVencTestPara, 0, sizeof(VENC_TEST_PARA_S)*u32ChnNum);
#if 0    
    // ch0: for reverse playback, H.264 base, 1280*720, gop=1, VBR 
    pstChnPara->u32ChanId           = 0;
    pstChnPara->enType              = PT_H264;
    pstChnPara->u32Profile          = 0; //base
    pstChnPara->stPicSize.u32Width  = 1280; 
    pstChnPara->stPicSize.u32Height = 720; 
    pstChnPara->u32Gop              = 1;   
    pstChnPara->enRcMode            = SAMPLE_RC_VBR;  
    pstChnPara->u32StatTime         = 1;
    pstChnPara->u32SrcFrmRate       = 30;  
    pstChnPara->fr32DstFrmRate      = 30;  
    pstChnPara->u32BitRate          = 10240;
    pstChnPara->u32MaxBitRate       = 10240;
    pstChnPara->u32MinQp            = 15;
    pstChnPara->u32MaxQp            = 40;
    pstChnPara->bEnableSmart    = 0;
    pstChnPara->u32Base         = 0;
    pstChnPara->u32Enhance      = 0;
    pstChnPara->bEnablePred     = 0;
    
    pstChnPara->u32VpuGrpId     = VEU_BEGIN_GRP;
    pstChnPara->u32VpuChanId    = 0;    
    pstChnPara++;
#endif    
    // ch1: for wbc, H.264 base, 704*576, gop=1, CBR 
    pstChnPara->u32ChanId           = 0;
    pstChnPara->enType              = PT_H264;
    pstChnPara->u32Profile          = 0; //base
    pstChnPara->stPicSize.u32Width  = 704; 
    pstChnPara->stPicSize.u32Height = 576; 
    pstChnPara->u32Gop              = 25;  
    pstChnPara->enRcMode            = SAMPLE_RC_CBR;  
    pstChnPara->u32StatTime         = 1;
    pstChnPara->u32SrcFrmRate       = 25;  
    pstChnPara->fr32DstFrmRate      = 25;  
    pstChnPara->u32BitRate          = 1792;
    pstChnPara->u32MaxBitRate       = 1792;
    pstChnPara->u32MinQp            = 15;
    pstChnPara->u32MaxQp            = 40;
    pstChnPara->bEnableSmart    = 0;
    pstChnPara->u32Base         = 0;
    pstChnPara->u32Enhance      = 0;
    pstChnPara->bEnablePred     = 0;
    pstChnPara->u32VpuGrpId     = WBC_VPU_GRP_BEGION;
    pstChnPara->u32VpuChanId    = WBC_TO_VENC;    

    return sample_encode_config_chan(&stVencTestPara[0], u32ChnNum);
}

FY_VOID sample_encoding_perf_deinit()
{
   sample_encoding_vpu_deinit(0,1); 
}

FY_S32 sample_encoding_nvr_init(          FY_U32 u32ChnNum)
{
    FY_S32 s32Ret = FY_SUCCESS;

    //init vpu and bind vpu with vppu, one channel
    s32Ret = sample_encoding_vpu_init(0/*veu*/, u32ChnNum);
    if(s32Ret != FY_SUCCESS)
    {
         sample_encoding_vpu_deinit(0,u32ChnNum);
         return FY_FAILURE;
    } 

    return FY_SUCCESS;
}

FY_S32 sample_encoding_nvr_deinit(FY_U32 u32ChnNum)
{
    FY_S32 s32Ret = FY_SUCCESS;

    //init vpu and bind vpu with vppu, one channel
    s32Ret = sample_encoding_vpu_deinit(0/*veu*/, u32ChnNum);
    return s32Ret;
}

FY_S32 sample_jpege_nvr_init(         FY_U32 u32ChnNum)
{
#if 0
    FY_S32 s32Ret = FY_SUCCESS;

    //init vpu and bind vpu with vppu, one channel
    s32Ret = sample_encoding_vpu_init(1/*jpege*/, u32ChnNum);
    if(s32Ret != FY_SUCCESS)
    {
         sample_encoding_vpu_deinit(1,u32ChnNum);
         return FY_FAILURE;
    }
#endif    
    return FY_SUCCESS;
}

FY_S32 sample_jpege_nvr_deinit(FY_U32 u32ChnNum)
{
    FY_S32 s32Ret = FY_SUCCESS;
#if 0
    //init vpu and bind vpu with vppu, one channel
    s32Ret = sample_encoding_vpu_deinit(1/*jpege*/, u32ChnNum);
#endif
    return s32Ret;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
