#ifndef __SAMPLE_NNA_H__
#define __SAMPLE_NNA_H__

#include "fy_common.h"
#include "fy_comm_nna.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define SAMPLE_NNA_PRT(fmt,...)   \
    do {\
        printf("[%s]-%d: "fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__);\
       }while(0)

#define SAMPLE_NNA_CHECK_EXPR_GOTO(Expr, Label, fmt, ...)                    \
do{																				  \
	if(Expr)                                                                      \
	{                                                                             \
		SAMPLE_NNA_PRT(fmt,## __VA_ARGS__);                               \
		goto Label;                                                               \
	}                                                                             \
}while(0)
/****
*Expr is true,return void
*/
#define SAMPLE_NNA_CHECK_EXPR_RET_VOID(Expr,Level,Msg, ...)					     \
do{                                                                              \
	if(Expr)                                                                     \
	{                                                                            \
		SAMPLE_NNA_PRT(Level,Msg, ##__VA_ARGS__);                              \
		return;                                                                  \
	}                                                                            \
}while(0)
/****
*Expr is true,return Ret
*/
#define SAMPLE_NNA_CHECK_EXPR_RET(Expr,Ret,Msg, ...)					     \
do{                                                                              \
	if(Expr)                                                                     \
	{                                                                            \
		SAMPLE_NNA_PRT(Msg, ##__VA_ARGS__);                              \
		return Ret;                                                              \
	}                                                                            \
}while(0)

FY_S32 sample_nna_test_start(FY_U8 num, FY_TYPE_E eDetType);
FY_S32 sample_nna_test_stop();
FY_S32 sample_nna_set_chn_num(FY_U32 u32ChnNum);
FY_S32 sample_nna_get_chn_num(FY_U32 *pu32ChnNum);
FY_S32 sample_nna_disable_draw_box(FY_BOOL bDisable);

//FY_S32 sample_encode_set_chn_num(FY_U32 u32ChnNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_NNA_H__
