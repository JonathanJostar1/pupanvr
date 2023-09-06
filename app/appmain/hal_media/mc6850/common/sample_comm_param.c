/******************************************************************************
  Copyright (C) 2018, YGTek. Co., Ltd.

 ******************************************************************************
    Modification:  2018-12 Created
******************************************************************************/
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

#include "sample_comm_param.h"

//#define DEBUG_PARSER

#ifdef DEBUG_PARSER
#define PARSER_DEBUG(fmt ...) printf(fmt)
#else
#define PARSER_DEBUG(fmt ...) do{}while(0)
#endif

char stViChnNumName[VI_SYS_NUM][MAX_NAME] =
{
    "u32DevNum",
    "enNorm",
    "u32Chn1080Num",
    "u32Chn720Num",
    "u32ChnD1Num",
    "u32CompRate"
};

char stChnInfoName[VI_CHN_PARA_SIZE][MAX_NAME] =
{
    "u32CapX",
    "u32CapY",
    "u32CapWidth",
    "u32CapHeight",
    "u32DstWidth",
    "u32DstHeight",
    "enCapSel",
    "enScanMode",
    "enPixFormat",
    "bMirror",
    "bFlip",
    "s32SrcFrameRate",
    "s32DstFrameRate",
    "stSensorWidth",
    "stSensorHeight",
    "enDataSeq",
    "bMinor",
    "u32MDstWidth",
    "u32MDstHeight",
    "s32MSrcFrameRate",
    "s32MDstFrameRate",
    "inputFile"
};

char stVpssGlobleInfoName[VPSS_GLOBLE_INFO_NUM][MAX_NAME]=
{
	"glob_idx",
    "benable",
	"glb_pixfmt",
	"frame_rate",
	"u32Width",
	"u32Height"
};

char stVpssGrpInfoName[VPSS_GRP_ATTR_NUM][MAX_NAME]=
{
    "enPixFmt",
    "enCompMode",
    "u32Width",
    "u32Height",
    "HTileNum",
    "VTileNum",
    "bYcnrEn",
    "bYcnrEcDcEn",
    "nrCompRate",
    "bApcEn",
    "bPurpleEn",
    "bYGammaEn",
    "bChromaEn",
    "bLcEn",
    "bYHistEn"
};

char stVpssChnInfoName[VPSS_CHN_ATTR_NUM][MAX_NAME]=
{
    "bEnable",
    "s32X",
    "s32Y",
    "u32Width",
    "u32Height"
};

char stVpssChnModeInfoName[VPSS_CHN_MODE_NUM][MAX_NAME]=
{
    "enChnMode",
    "u32Width",
    "u32Height",
    "enCompressMode",
    "enPixelFormat",
    "CompRate",
    "bUseGloble",
    "glob_idx",
    "s32X",
    "s32Y",
    "bYcMeanEn",
    "ycmeanMode",
    "bBgmYEn",
    "bCpyEn",
    "AiOutSel",
    "RGBOutMode"
};

char stVpssGrpDumpFrameName[VPSS_DUMP_FRAME_NUM][MAX_NAME]=
{
    "bdump",
    "u32Grp",
    "u32Chn",
    "u32FrameCnt",
    "u32CompressMode",
    "u32PixelFormat",
    "u32Comprate",
    "u32Width",
    "u32Height"
};

char stVpssGrpSendFrameInfoName[VPSS_SEND_FRAME_NUM][MAX_NAME]=
{
    "u32Width",
    "u32Height",
    "inputFile"
};

FY_S32 Print_config(Config *cnf)
{
    if(NULL == cnf)
        return -1;
    PARSER_DEBUG("-------------- After Read File --------------\n");
    print_config_info(cnf); // 打印cnf对象
    //sleep(1);
    return 0;
}

FY_S32 SAMPLE_COMM_VI_Parser_Cfg(char *filename,stViCnfInfo *psViCnfInfo)
{
    FY_S32 s32Ret = FY_SUCCESS;
    FY_U32 i = 0;
    FY_U32 *param;
    stViCnfInfo sViCnfInfo;

    if((NULL == filename) || (NULL == psViCnfInfo))
        return -1;

    Config *cnf = cnf_load_config(filename);
    if (NULL == cnf) {
        return -1; /* 创建对象失败 */
    }

    Print_config(cnf);

	memset(&sViCnfInfo,0,sizeof(stViCnfInfo));
    /***get vi Sys info**/
    param = (FY_U32 *)&(sViCnfInfo.stViInfo);

    for(i=0;i<VI_SYS_NUM;i++)
    {
        s32Ret = cnf_get_value(cnf,VI_SYS_NMAE, stViChnNumName[i]);
        if(s32Ret)
            *param = cnf->re_int;
        PARSER_DEBUG("the value of name is %s, Value is %d \n",stViChnNumName[i],*param);
        param++;
    }

     //get dev attr info

    s32Ret = cnf_get_value(cnf,VI_DEV_NMAE, "enIntfMode");
    if(s32Ret)
        sViCnfInfo.stViDevAttr.enIntfMode = cnf->re_int;

     s32Ret = cnf_get_value(cnf,VI_DEV_NMAE, "enWorkMode");
    if(s32Ret)
        sViCnfInfo.stViDevAttr.enWorkMode= cnf->re_int;

    s32Ret = cnf_get_value(cnf,VI_DEV_NMAE, "enClkEdge");
    if(s32Ret)
        sViCnfInfo.stViDevAttr.enClkEdge = cnf->re_int;

    s32Ret = cnf_get_value(cnf,VI_DEV_NMAE, "enDataSeq");
    if(s32Ret)
        sViCnfInfo.stViDevAttr.enDataSeq = cnf->re_int;

#if 0
   /***get vi NVP info**/
    param = (FY_U32 *)&(sViCnfInfo.ViNvpInfo);
    for(i=0;i<VI_NP_NUM;i++)
    {
        s32Ret = cnf_get_value(cnf,VI_NVP_CNF, stNvpCfgName[i]);
        if(s32Ret)
            *param = cnf->re_int;
        PARSER_DEBUG("NvpInfo[%s] = %d \n",stNvpCfgName[i],*param);
        param++;
    }
#endif

    //get chn attr info
    param = (FY_U32 *)&(sViCnfInfo.ViChnInfo);
    /***get Dev Sys info**/
    for(i=0;i<VI_CHN_PARA_SIZE;i++)
    {
        s32Ret = cnf_get_value(cnf,VI_CHN_CNF, stChnInfoName[i]);

        if(s32Ret)
        {
            if(!strcmp(stChnInfoName[i], "inputFile"))
            {
                param = (FY_U32 *)malloc(MAX_FILE_NAME_LEN);
                strcpy((void *)*param, cnf->re_string);
                PARSER_DEBUG("the value of name is %s, Value is %s\n",stChnInfoName[i],(char *)*param);
            }
            else
                *param = cnf->re_int;
        }

        PARSER_DEBUG("ChnInfo[%s] = %d \n",stChnInfoName[i],*param);
        param++;
    }

//    printf("%s, w-h=%dx%d\n", __func__, sViCnfInfo.ViChnInfo.stChnAttr.stSensorSize.u32Width, sViCnfInfo.ViChnInfo.stChnAttr.stSensorSize.u32Height);
    memcpy(psViCnfInfo,&sViCnfInfo,sizeof(stViCnfInfo));
    cnf_release(cnf);

    return FY_SUCCESS;
}

FY_S32 SAMPLE_COMM_VPSS_Parser_Cfg(char *filename,stVpssInfo *pstVpssInfo)
{
    FY_S32 s32Ret = FY_SUCCESS;

    FY_U32 i = 0, index =0,u32SecLen=0,grp_id=0,glob_id=0;
    FY_U32 *param=NULL;
    stVpssInfo st_VpssInfo;
    char section[256];
    char *buf;
	Data *has_section = NULL;

    if(NULL == filename || NULL == pstVpssInfo)
    {
        PARSER_DEBUG("the filename is NULL \n");
        return -1;
    }
	memset(&st_VpssInfo,0,sizeof(stVpssInfo));
    Config *cnf = cnf_load_config(filename);
    if (NULL == cnf) {
        return -1; /* 创建对象失败 */
    }

    Print_config(cnf);
#if 1
	/***parser globle config******/
	 s32Ret = cnf_get_value(cnf,VPSS_GLOBLE_DISPLAY_CNF, "u32GlobNum");
    if(s32Ret)
        st_VpssInfo.st_VpssGlobleInfo.u32GlobNum = cnf->re_int;

	PARSER_DEBUG("*****the globle count is %d\n",st_VpssInfo.st_VpssGlobleInfo.u32GlobNum);

	for(glob_id=0;glob_id<(st_VpssInfo.st_VpssGlobleInfo.u32GlobNum);glob_id++)
	{
		memset(section,0,sizeof(section));
	    sprintf(section, VPSS_GLOBLE_DISPLAY_CNF);
	    u32SecLen = strlen(section);
		sprintf(section+u32SecLen, "_%d", glob_id); //VPSS_GLOBLE_DISPLAY_%d

	    /**Get Globle infomation**/
		has_section = cnf_has_section(cnf,section);
		if(has_section)
		{
		    param = (FY_U32 *)&(st_VpssInfo.st_VpssGlobleInfo.initcfg[glob_id]);
			PARSER_DEBUG("/**********the globle%d params:******************/\n",glob_id);
		    for(i=0;i<VPSS_GLOBLE_INFO_NUM;i++)
		    {
		        s32Ret = cnf_get_value(cnf,section, stVpssGlobleInfoName[i]);
		        if(s32Ret)
		            *param = cnf->re_int;
		        PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssGlobleInfoName[i],*param);
		        param ++;
		    }
		}
	}


    /*********/
    s32Ret = cnf_get_value(cnf,VPSS_GRP_CNF, "s32VpssGrpCnt");
    if(s32Ret)
        st_VpssInfo.u32GrpNum = cnf->re_int;

    s32Ret = cnf_get_value(cnf,VPSS_GRP_CNF, "u32ShowMode");
    if(s32Ret)
        st_VpssInfo.u32ShowMode = cnf->re_int;

	PARSER_DEBUG("*****the group count is %d, the show mode is %d\n",st_VpssInfo.u32GrpNum,st_VpssInfo.u32ShowMode);

	for(grp_id=0;grp_id<(st_VpssInfo.u32GrpNum);grp_id++)
	{
		memset(section,0,sizeof(section));
	    sprintf(section, VPSS_GRP_CNF);
	    u32SecLen = strlen(section);
		sprintf(section+u32SecLen, "_%d", grp_id); //VPSS_GRP_ATTR_S_%d

		/*
	    s32Ret = cnf_get_value(cnf,section, "u32PixelFormat");
	    if(s32Ret)
	        pstVpssInfo->u32PixelFormat = cnf->re_int;
		*/

	    /**Get Grp attr infomation**/
		has_section = cnf_has_section(cnf,section);
		if(has_section)
		{
		    param = (FY_U32 *)&(st_VpssInfo.st_VpssGrpInfo[grp_id].VpssGrpInfo);
			PARSER_DEBUG("/**********the Group%d params:******************/\n",grp_id);
		    for(i=0;i<VPSS_GRP_ATTR_NUM;i++)
		    {
		        s32Ret = cnf_get_value(cnf,section, stVpssGrpInfoName[i]);
		        if(s32Ret)
		            *param = cnf->re_int;
		        PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssGrpInfoName[i],*param);
		        param ++;
		    }
		}

	    /*Get Chn attr information*/
	    memset(section,0,sizeof(section));
	    sprintf(section, VPSS_CHN_CNF);
	    u32SecLen = strlen(section);
		sprintf(section+u32SecLen, "_%d", grp_id); //VPSS_CHN_ATTR_S_%d
		u32SecLen = strlen(section);

		PARSER_DEBUG("/**********the Group%d chn attr params:******************/\n",grp_id);
		for(i=0;i<VPSS_CHN_NUM;i++)
		{
    	    param = (FY_U32 *)&(st_VpssInfo.st_VpssGrpInfo[grp_id].VpssChnAttr[i]);
			sprintf(section+u32SecLen, "_%d", i); //VPSS_CHN_ATTR_S_%d
			PARSER_DEBUG("/**********the chn %d attr params:******************/\n",i);
			has_section = cnf_has_section(cnf,section);
			if(has_section)
			{
				for(index=0;index<VPSS_CHN_ATTR_NUM;index++)
				{
					s32Ret = cnf_get_value(cnf,section, stVpssChnInfoName[index]);
					if(s32Ret)
						*param = cnf->re_int;
					PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssChnInfoName[index],*param);
					param ++;
				}
			}
		}




	    /****Get Chn Mode information*****/
	    memset(section,0,sizeof(section));
	    sprintf(section, VPSS_CHN_MODE_CNF);
	    u32SecLen = strlen(section);
		sprintf(section+u32SecLen, "_%d", grp_id); //VPSS_CHN_ATTR_S_%d
		u32SecLen = strlen(section);

		PARSER_DEBUG("/**********the Group%d chn mode params:******************/\n",grp_id);
	    for(i=0;i<VPSS_CHN_NUM;i++)
	    {
    	    param = (FY_U32 *)&(st_VpssInfo.st_VpssGrpInfo[grp_id].VpssChnMode[i]);
	        sprintf(section+u32SecLen, "_%d", i); //VPSS_CHN_MODE_S_%d
	        PARSER_DEBUG("/**********the chn %d mode params:******************/\n",i);
			has_section = cnf_has_section(cnf,section);
			if(has_section)
			{
		        for(index=0;index<VPSS_CHN_MODE_NUM;index++)
		        {
		            s32Ret = cnf_get_value(cnf,section, stVpssChnModeInfoName[index]);
		            if(s32Ret)
		                *param = cnf->re_int;
		            else
		            {
		                PARSER_DEBUG("get value %s failed\n", stVpssChnModeInfoName[index]);
		            }
		            PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssChnModeInfoName[index],*param);
		            param ++;
		        }
				*param = 1; // the chn is configed
				param++;
			}
	    }

	}

    /******Get SendFrame information*****/
    memset(section,0,sizeof(section));
    sprintf(section, VPSS_SEND_FRAME_CNF);
    u32SecLen = strlen(section);

	has_section = cnf_has_section(cnf,section);
	if(has_section)
	{
	    s32Ret = cnf_get_value(cnf,section, "s32GrpCnt");
	    if(s32Ret)
	        st_VpssInfo.stVpssSendParam.s32Cnt = cnf->re_int;
		else
			st_VpssInfo.stVpssSendParam.s32Cnt = 0;

	    s32Ret = cnf_get_value(cnf,section, "u32PixelFormat");
	    if(s32Ret)
	        st_VpssInfo.stVpssSendParam.u32PixelFormat = cnf->re_int;



		PARSER_DEBUG("/**********the sendframe params:******************/\n");
	    for(i=0;i<st_VpssInfo.stVpssSendParam.s32Cnt;i++)
	    {
	    	param =(FY_U32 *)&(st_VpssInfo.stVpssSendParam.SendFrameInfo[i]);
	        sprintf(section+u32SecLen, "_%d", i); //
	        PARSER_DEBUG("/**********the group %d sendframe params:******************/\n",i);
	        for(index=0;index<VPSS_SEND_FRAME_NUM;index++)
	        {
	            s32Ret = cnf_get_value(cnf,section, stVpssGrpSendFrameInfoName[index]);
	            if(!s32Ret)
	            {
	                PARSER_DEBUG("get value %s failed\n", stVpssGrpSendFrameInfoName[index]);
	            }

	            if(!strcmp(stVpssGrpSendFrameInfoName[index], "inputFile"))
	            {
	                buf = malloc(512);
	                *param = (FY_U32)buf;
	                strcpy((void *)*param, cnf->re_string);
	                PARSER_DEBUG("the value of name is %s, Value is %s\n",stVpssGrpSendFrameInfoName[index],(char *)*param);
	            }
	            else
	            {
	                *param = cnf->re_int;
	                PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssGrpSendFrameInfoName[index],*param);
	            }
	            param++;
	        }
	    }
	}
    /******Get DumpFrame information*****/
    memset(section,0,sizeof(section));
    sprintf(section, VPSS_DUMP_FRAME_CNF);
    u32SecLen = strlen(section);

	has_section = cnf_has_section(cnf,section);
	if(has_section)
	{
	    s32Ret = cnf_get_value(cnf,section, "s32Cnt");
	    if(s32Ret)
	        st_VpssInfo.stDumpFramInfo.s32Cnt= cnf->re_int;
		else
			st_VpssInfo.stDumpFramInfo.s32Cnt= 0;

		PARSER_DEBUG("/**********the dumpframe params:******************/\n");
	    for(i=0;i<st_VpssInfo.stDumpFramInfo.s32Cnt;i++)
	    {
	    	param =(FY_U32 *)&(st_VpssInfo.stDumpFramInfo.DumpFrameInfo[i]);
	        sprintf(section+u32SecLen, "_%d", i); //
	        PARSER_DEBUG("/**********the chn %d dump frame params:******************/\n",i);
	        for(index=0;index<VPSS_DUMP_FRAME_NUM;index++)
	        {
	            s32Ret = cnf_get_value(cnf,section, stVpssGrpDumpFrameName[index]);
	            if(!s32Ret)
	            {
	                PARSER_DEBUG("get value %s failed\n", stVpssGrpDumpFrameName[index]);
	            }

	            *param = cnf->re_int;
	            PARSER_DEBUG("the value of name is %s, Value is %d \n",stVpssGrpDumpFrameName[index],*param);

	            param++;
	        }
	    }
	}
#endif

    memcpy(pstVpssInfo,&st_VpssInfo,sizeof(stVpssInfo));
    cnf_release(cnf);
    return s32Ret;
}

int SAMPLE_COMM_Get_Mod_Param(const char* modname, const char* paraname, int defval, int *param)
{
	int fd = -1;
    int ret = 0;
    int value = 0;
    char buf[8];
    char fname[128];
    sprintf(fname, "/sys/module/%s/parameters/%s", modname, paraname);

    *param = defval;
    ret = access(fname, R_OK);
    if(ret < 0) {
        printf("[%s] couldn't access: %s, errno = %d (%s)\n", __func__,  fname, errno, strerror(errno));
        return -1;
    }

    fd = open(fname, O_RDONLY, 0);
    if(fd < 0) {
    	printf("[%s] fail to open %s\n", __func__, fname);
        printf("[%s]   errno = %d (%s)\n", __func__, errno, strerror(errno));
    	return -1;
	}
    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 4);
    if(-1 == ret) {
        printf("[%s] read faid. errno = %d (%s)\n",  __func__, errno, strerror(errno));
    } else {
        value = atoi(buf);
    }
    close(fd);
    *param = value;

	return ret;

}

int SAMPLE_COMM_Get_Mod_Param_Str(const char* modname, const char* paraname, char *param, int size)
{
    FILE *fp = NULL;
    int ret = 0;
    char fname[128];
    sprintf(fname, "/sys/module/%s/parameters/%s", modname, paraname);

    fp = fopen(fname, "r");
    if(fp == NULL) {
        printf("[%s] fail to open %s\n", __func__, fname);
        printf("[%s]   errno = %d (%s)\n", __func__, errno, strerror(errno));
        return -1;
    }
    memset(param, 0, size);
    ret = fread(param, 1, size, fp);
    fclose(fp);


    return ret;

}


char *SAMPLE_COMM_Get_Chip_Name()
{
    static char chip_name[32] = {0};

    int fd = -1;
    int ret = 0;
    char fname[128];

    strcpy(fname, "/proc/device-tree/model");

    if(0 == chip_name[0]) {
        ret = access(fname, R_OK);
        if(ret < 0) {
            printf("[%s] couldn't access: %s, errno = %d (%s)\n", __func__,  fname, errno, strerror(errno));
            return NULL;
        }

        fd = open(fname, O_RDONLY, 0);
        if(fd < 0) {
        	printf("[%s] fail to open %s\n", __func__, fname);
            printf("[%s]   errno = %d (%s)\n", __func__, errno, strerror(errno));
        	return NULL;
    	}
        memset(chip_name, 0, sizeof(chip_name));
        ret = read(fd, chip_name, sizeof(chip_name) - 1);
        if(-1 == ret) {
            printf("[%s] read faid. errno = %d (%s)\n",  __func__, errno, strerror(errno));
            chip_name[0] = 0;
        } else {
            chip_name[ret] = 0;
        }
        close(fd);

    }

    return chip_name;

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
