/*
 * mpp_def.h
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_
#define APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_

#include<stdio.h>
#include<string.h>

#include "sample_comm.h"

#define FY_MPI_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

typedef struct{

}StHalMediaContextInfo;

StHalMediaContextInfo*	mpp_sys_getSthalMediaContextInfo();

#endif /* APPMAIN_APPMAIN_HAL_MEDIA_HI3536C_MPP_DEF_H_ */
