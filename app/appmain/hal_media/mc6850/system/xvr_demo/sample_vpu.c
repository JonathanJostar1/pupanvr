#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include "sample_comm.h"
#include "sample_vpu.h"

#define VPU_TO_VPU_CHN 1
static FY_U32 g_u32ChipID =0; //0:MC6630,1:MC6650

FY_S32 sample_vpu_init_param(VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    SAMPLE_VPU_CHECK_GOTO((NULL == pstVpssGrpAttr), EXIT, "pstVpssGrpAttr null!");

    memset(pstVpssGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
    pstVpssGrpAttr->u32MaxW     = 1920;
    pstVpssGrpAttr->u32MaxH     = 2160;
    pstVpssGrpAttr->enPixFmt    = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVpssGrpAttr->enDieMode   = VPSS_DIE_MODE_AUTO;

 EXIT:
    return FY_SUCCESS;
}

FY_S32 sample_vpu_init_param_420(VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    SAMPLE_VPU_CHECK_GOTO((NULL == pstVpssGrpAttr), EXIT, "pstVpssGrpAttr null!");

    memset(pstVpssGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
    pstVpssGrpAttr->u32MaxW     = 720;
    pstVpssGrpAttr->u32MaxH     = 576;
    pstVpssGrpAttr->enPixFmt    = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

 EXIT:
    return FY_SUCCESS;
}

FY_S32 sample_vpu_start_userchn(FY_S32 s32GrpCnt,VPSS_CHN VpssChn,VPSS_CHN_MODE_S *pstVpssMode)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    VPSS_GRP VpssGrp;
    SAMPLE_VPU_CHECK_GOTO((NULL != pstVpssMode), EXIT, "VPSS_CHN_MODE_S null!");

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = FY_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, pstVpssMode);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_SetChnMode ERR!");

        s32Ret = FY_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_EnableChn ERR!");

    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vpu_disable_preview(FY_S32 s32GrpCnt,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    VPSS_GRP VpssGrp;

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = FY_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_DisableChn ERR!");

    }
EXIT:
    return s32Ret;
}
FY_S32 sample_vpu_check_get_frame(VPSS_GRP VpssGrp,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VIDEO_FRAME_INFO_S stFrame;
    FY_U32 u32Depth = 1;
    FY_U32 u32OrigDepth = 0;
    FY_U32 count = 0;
    VPSS_CHN_MODE_S stOrigVpssMode, stVpssMode={0};
    FY_S32 s32MilliSec = 2000;

    if (FY_MPI_VPSS_GetChnMode(VpssGrp,VpssChn,&stOrigVpssMode) != FY_SUCCESS)
    {
        printf("get mode error!!!\n");
        return -1;
    }

    if (FY_MPI_VPSS_GetDepth(VpssGrp, VpssChn, &u32OrigDepth) != FY_SUCCESS)
    {
        printf("get depth error!!!\n");
        return -1;
    }

    if (FY_MPI_VPSS_DisableChn(VpssGrp,VpssChn) != FY_SUCCESS)
    {
        printf("disable error!!!\n");
        return -1;
    }

    stVpssMode = stOrigVpssMode;
    stVpssMode.u32Width = 720;
    stVpssMode.u32Height = 576;
    stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
    stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    stVpssMode.enCompressMode = COMPRESS_MODE_NONE;
    stVpssMode.bDouble = FY_FALSE;

    if (FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stVpssMode) != FY_SUCCESS)
    {
        printf("set mode error!!!\n");
        return -1;
    }

    if (FY_MPI_VPSS_SetDepth(VpssGrp,VpssChn,u32Depth)!=FY_SUCCESS)
    {
        printf("set depth error!!!\n");
        return -1;
    }

    if (FY_MPI_VPSS_EnableChn(VpssGrp,VpssChn) != FY_SUCCESS)
    {
        printf("enable error!!!\n");
        return -1;
    }

    memset(&stFrame,0,sizeof(stFrame));
    while (count<4)
    {
        s32Ret = FY_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec);

        if(s32Ret == FY_SUCCESS){
            s32Ret = FY_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
            break;
        }
        printf("get frame error!!!\n");
        usleep(2000);
        count++;
    }

    s32Ret |= FY_MPI_VPSS_SetDepth(VpssGrp, VpssChn, u32OrigDepth);
    s32Ret |= FY_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stOrigVpssMode);

    return s32Ret;
}

FY_BOOL sample_vpu_chn_bshow(FY_S32 s32GrpCnt,VPSS_GRP VpssGrp)
{
    FY_BOOL bShow = FY_FALSE;
    if(g_u32ChipID){
        bShow = FY_TRUE;
    }
    else {
		if(0 == VpssGrp%2)
			bShow = FY_TRUE;
    }
    return bShow;
}

FY_S32 sample_vpu_enable_preview(FY_S32 s32GrpCnt,VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    VPSS_GRP VpssGrp;

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = FY_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_EnableChn ERR!");

    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vpu_usermode_preview(VPSS_GRP VpssGrp,    VPSS_CHN VpssChn)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_CHN_MODE_S stChnMode;
    memset(&stChnMode,0,sizeof(VPSS_CHN_MODE_S));
    stChnMode.enChnMode         = VPSS_CHN_MODE_USER;
    if((VPSS_CHN0 == VpssChn) ||(VPSS_CHN4 == VpssChn) || (VPSS_CHN3 == VpssChn))
        return s32Ret;
    if(VPSS_CHN1 == VpssChn){
        stChnMode.enPixelFormat     = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stChnMode.u32Width          = 720;
        stChnMode.u32Height         = 576;
        stChnMode.enCompressMode    = COMPRESS_MODE_NONE;
    }
    else if(VPSS_CHN2 == VpssChn){
        stChnMode.enPixelFormat     = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        stChnMode.u32Width          = 352;
        stChnMode.u32Height         = 288;
        stChnMode.enCompressMode    = COMPRESS_MODE_NONE;
        stChnMode.CompRate          = COMPRESS_RATIO_NONE;
    }
    else if(VPSS_CHN3 == VpssChn){
        stChnMode.enPixelFormat     = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
        stChnMode.u32Width          = 960;
        stChnMode.u32Height         = 540;
        stChnMode.enCompressMode    = COMPRESS_MODE_SLICE;
        stChnMode.CompRate          = COMPRESS_RATIO_60_PER;
    }
    s32Ret = FY_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stChnMode);
    SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_SetChnMode ERR!");

EXIT:
    return s32Ret;
}

FY_S32 sample_vpu_start(FY_S32 s32BeginGrp,FY_S32 s32GrpCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr,FY_BOOL bRegion)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    FY_S32 i, j;
    //VPSS_CHN_ATTR_S stChnAttr = {0};
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_GRP_ATTR_S *pstGrpTemp = NULL;

    memset(&stVpssGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
    /*** Set Vpss Grp Attr ***/
    SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "pstVpssGrpAttr null!");

    memcpy(&stVpssGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = s32BeginGrp+i;
        if((0 != i%2)&&bRegion && (0 == g_u32ChipID)){
            sample_vpu_init_param_420(&stVpssGrpAttr);
            pstGrpTemp = &stVpssGrpAttr;
        }
        else
        {
            pstGrpTemp = pstVpssGrpAttr;
        }


        if(i<4 && (0 == i%2)) // enable nr for group0,2
        {
          pstGrpTemp->bNrEn = 1;
        }
        else
        {
          pstGrpTemp->bNrEn = 0;
        }

        /*** create vpss group ***/
        s32Ret = FY_MPI_VPSS_CreateGrp(VpssGrp, pstGrpTemp);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_CreateGrp grp id:%d ERR!",VpssGrp);

        /*** enable vpss chn ***/
        for(j=0;j<VPSS_MAX_CHN_NUM;j++){
            VpssChn = j;
            s32Ret = FY_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_EnableChn ERR!");
#if(defined(MC6650)||defined(MC6850))
            if(bRegion && (sample_vpu_chn_bshow(s32GrpCnt,VpssGrp)&&(VPSS_CHN5 != VpssChn))){
                if((0 == i%2)&&(VPU_TO_VPU_CHN==j)&&(i<JPEG_BEGION_GRP)){
                    sample_vpu_usermode_preview(i,j);
                }
                SAMPLE_VPSS_Overlay_Create(VpssGrp,VpssChn,GOSD_DEFAULT_PIXEDEPTH,1);
            }
#else
            if(bRegion && ((sample_vpu_chn_bshow(s32GrpCnt,VpssGrp)&&(VPSS_CHN3 == VpssChn))
                ||(VPSS_CHN0 == VpssChn)||(VPSS_CHN1 == VpssChn))){
                if((0 == i%2)&&(VPU_TO_VPU_CHN==j)&&(i<JPEG_BEGION_GRP)){
                    sample_vpu_usermode_preview(i,j);
                }
                SAMPLE_VPSS_Overlay_Create(VpssGrp,VpssChn,GOSD_DEFAULT_PIXEDEPTH,0);
            }
#endif
        }

        /*** start vpss group ***/
        s32Ret = FY_MPI_VPSS_StartGrp(VpssGrp);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_StartGrp ERR!");

    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vpu_stop(FY_S32 s32BeginGrp,FY_S32 s32GrpCnt, FY_S32 s32ChnCnt,FY_BOOL bRegion)
{
    FY_S32 i, j;
    FY_S32 s32Ret = FY_SUCCESS;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;

    for(i=0; i<s32GrpCnt; i++) {
        VpssGrp = s32BeginGrp+i;
        s32Ret = FY_MPI_VPSS_StopGrp(VpssGrp);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_StopGrp ERR!");
        for(j=0; j<s32ChnCnt; j++) {
            VpssChn = j;
#if(defined(MC6650)||defined(MC6850))
            if(bRegion && (sample_vpu_chn_bshow(s32GrpCnt,VpssGrp)&&(VPSS_CHN5 != VpssChn))){
                SAMPLE_VPSS_Overlay_Destory(VpssGrp,VpssChn);
            }
#else
			if(bRegion && ((sample_vpu_chn_bshow(s32GrpCnt,VpssGrp) && (VPSS_CHN3 == VpssChn))
                ||(VPSS_CHN0 == VpssChn)||(VPSS_CHN1 == VpssChn)))
				SAMPLE_VPSS_Overlay_Destory(VpssGrp,VpssChn);
#endif
            s32Ret = FY_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_DisableChn ERR!");

        }
        s32Ret = FY_MPI_VPSS_DestroyGrp(VpssGrp);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_VPSS_DestroyGrp ERR!");
    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vi_bind_vpu(FY_U32 u32GrpNum)
{
    FY_S32 i, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    VI_CHN ViChn;
    FY_U32 u32MaxGrp = 0;

    VpssGrp = 0;
    u32MaxGrp = u32GrpNum/2;
    if(g_u32ChipID){
        u32MaxGrp = u32GrpNum;
    }

    for (i=0; i<u32MaxGrp; i++) {
        ViChn = i;

        stSrcChn.enModId = FY_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;

        stDestChn.enModId = FY_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

        //master chn
        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_Bind ERR!");

        if(g_u32ChipID){
            VpssGrp ++;
        }
        else {
        //mater vpu bind to second vpu
            stSrcChn.enModId = FY_ID_VPSS;
            stSrcChn.s32DevId = VpssGrp;
            stSrcChn.s32ChnId = VPU_TO_VPU_CHN;

            stDestChn.enModId = FY_ID_VPSS;
            stDestChn.s32DevId = VpssGrp+1;
            stDestChn.s32ChnId = 0;

            s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
            SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_Bind ERR!");

            VpssGrp += 2;
        }
    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vi_unbind_vpu(FY_U32 u32GrpNum)
{
	FY_S32 i, s32Ret;
	VPSS_GRP VpssGrp;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;
	VI_CHN ViChn;
    FY_U32 u32MaxGrp = 0;

    VpssGrp = 0;
    u32MaxGrp = u32GrpNum/2;
    if(g_u32ChipID){
        u32MaxGrp = u32GrpNum;
    }

	for (i=0; i< u32MaxGrp; i++) {
		ViChn = i;
		stSrcChn.enModId = FY_ID_VIU;
		stSrcChn.s32DevId = 0;
		stSrcChn.s32ChnId = ViChn;

		stDestChn.enModId = FY_ID_VPSS;
		stDestChn.s32DevId = VpssGrp;
		stDestChn.s32ChnId = 0;

        //master chn
        s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_UnBind ERR!");

        //mater vpu unbind to second vpu
        if(g_u32ChipID){
            VpssGrp ++;
        }
        else {
        //mater vpu bind to second vpu
            stSrcChn.enModId = FY_ID_VPSS;
            stSrcChn.s32DevId = VpssGrp;
            stSrcChn.s32ChnId = VPU_TO_VPU_CHN;

            stDestChn.enModId = FY_ID_VPSS;
            stDestChn.s32DevId = VpssGrp+1;
            stDestChn.s32ChnId = 0;

            s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
            SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_Bind ERR!");

            VpssGrp += 2;
        }
	}
EXIT:
	return FY_SUCCESS;
}

FY_S32 sample_vi_bind_vpu_mix(FY_U32 u32GrpNum)
{
    FY_S32 i, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    VI_CHN ViChn;

    VpssGrp = 0;
    for (i=0; i<u32GrpNum/2; i++) {
        ViChn = i;

        stSrcChn.enModId = FY_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;

        stDestChn.enModId = FY_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

        //master chn
        stDestChn.s32DevId = VpssGrp;
        //printf("Bind the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_Bind ERR!");

        //slave chn
		stSrcChn.s32ChnId = SUBCHN(ViChn);
        stDestChn.s32DevId = VpssGrp+1;
        //printf("Bind the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
        s32Ret = FY_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_Bind ERR!");

        VpssGrp += 2;
    }
EXIT:
    return s32Ret;
}

FY_S32 sample_vi_unbind_vpu_mix(FY_U32 u32GrpNum)
{
	FY_S32 i, s32Ret;
	VPSS_GRP VpssGrp;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;
	VI_CHN ViChn;

	VpssGrp = 0;
	for (i=0; i< u32GrpNum/2; i++) {
		ViChn = i;
		stSrcChn.enModId = FY_ID_VIU;
		stSrcChn.s32DevId = 0;
		stSrcChn.s32ChnId = ViChn;

		stDestChn.enModId = FY_ID_VPSS;
		stDestChn.s32DevId = VpssGrp;
		stDestChn.s32ChnId = 0;

        //master chn
        stDestChn.s32DevId = VpssGrp;
        //printf("UnBind the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
        s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_UnBind ERR!");

        //slave chn
		stSrcChn.s32ChnId = SUBCHN(ViChn);
        stDestChn.s32DevId = VpssGrp+1;
        //printf("UnBind the ViDev is %d,ViChn is %d, the VpssGrp is %d\n",stSrcChn.s32DevId,stSrcChn.s32ChnId,stDestChn.s32DevId);
        s32Ret = FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        SAMPLE_VPU_CHECK_GOTO((FY_SUCCESS != s32Ret), EXIT, "FY_MPI_SYS_UnBind ERR!");

        VpssGrp += 2;
	}
EXIT:
	return FY_SUCCESS;
}

FY_S32 sample_vgs_bind_vpu(VGS_CHN VgsChn,VGS_PTH  VgsPth,VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = VgsPth;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    SAMPLE_VPU_CHECK(FY_MPI_SYS_Bind(&stSrcChn, &stDestChn), "FY_MPI_SYS_Bind");

    return FY_SUCCESS;
}

FY_S32 sample_vgs_unbind_vpu(VGS_CHN VgsChn,VGS_PTH  VgsPth,VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = FY_ID_VGS;
    stSrcChn.s32DevId = VgsPth;
    stSrcChn.s32ChnId = VgsChn;

    stDestChn.enModId = FY_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    SAMPLE_VPU_CHECK(FY_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "FY_MPI_SYS_UnBind");

    return FY_SUCCESS;
}

FY_VOID sample_vpu_set_chip(FY_U32 u32ChipID)
{
    g_u32ChipID = u32ChipID;
}

FY_U32 sample_vpu_get_chip()
{
    return g_u32ChipID;
}


