#ifndef __MPI_TDE_H__
#define __MPI_TDE_H__

#include "fy_type.h"
#include "fy_comm_tde.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif /* __cplusplus */
#endif  /* __cplusplus */


#define FY_TDE_Open FY_TDE2_Open
#define FY_TDE_Close FY_TDE2_Close
#define FY_TDE_BeginJob FY_TDE2_BeginJob


/* Standard API */
FY_S32 FY_TDE2_Open(FY_VOID);

FY_VOID FY_TDE2_Close(FY_VOID);

TDE_HANDLE FY_TDE2_BeginJob(FY_VOID);

FY_S32 FY_TDE2_EndJob(TDE_HANDLE s32Handle, FY_BOOL bSync, FY_BOOL bBlock, FY_U32 u32TimeOut);

FY_S32 FY_TDE2_CancelJob(TDE_HANDLE s32Handle);

FY_S32 FY_TDE2_WaitForDone(TDE_HANDLE s32Handle);

FY_S32 FY_TDE2_WaitAllDone(FY_VOID);

FY_S32 FY_TDE2_Reset(FY_VOID);

FY_S32 FY_TDE2_SetDeflickerLevel(TDE_DEFLICKER_LEVEL_E enDeflickerLevel);

FY_S32 FY_TDE2_GetDeflickerLevel(TDE_DEFLICKER_LEVEL_E *pDeflickerLevel);

FY_S32 FY_TDE2_SetAlphaThresholdValue(FY_U8 u8ThresholdValue);

FY_S32 FY_TDE2_GetAlphaThresholdValue(FY_U8 *pu8ThresholdValue);

FY_S32 FY_TDE2_SetAlphaThresholdState(FY_BOOL bEnAlphaThreshold);

FY_S32 FY_TDE2_GetAlphaThresholdState(FY_BOOL * p_bEnAlphaThreshold);

FY_S32 FY_TDE2_QuickCopy(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S *pstDstRect);

FY_S32 FY_TDE2_QuickFill(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S *pstDstRect,
                                FY_U32 u32FillData);

FY_S32 FY_TDE2_QuickResize(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect);

FY_S32 FY_TDE2_QuickDeflicker(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect);

FY_S32 FY_TDE2_Bitblit(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                TDE2_OPT_S* pstOpt);

FY_S32 FY_TDE2_SolidDraw(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                TDE2_SURFACE_S *pstDst, TDE2_RECT_S  *pstDstRect,
                                TDE2_FILLCOLOR_S *pstFillColor, TDE2_OPT_S *pstOpt);


/* Extension API */
FY_S32 FY_TDE2_DoCompress(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_SURFACE_S* pstDst);

FY_S32 FY_TDE2_DeCompress(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_SURFACE_S* pstDst);


/* Unsupport API */
FY_S32 FY_TDE2_MbBlit(TDE_HANDLE s32Handle,
                                TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                TDE2_MBOPT_S* pstMbOpt);

FY_S32 FY_TDE2_BitmapMaskRop(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                TDE2_SURFACE_S* pstMask, TDE2_RECT_S  *pstMaskRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                TDE2_ROP_CODE_E enRopCode_Color, TDE2_ROP_CODE_E enRopCode_Alpha);

FY_S32 FY_TDE2_BitmapMaskBlend(TDE_HANDLE s32Handle,
                                TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                TDE2_SURFACE_S* pstMask, TDE2_RECT_S  *pstMaskRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                FY_U8 u8Alpha, TDE2_ALUCMD_E enBlendMode);

FY_S32 FY_TDE2_PatternFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstBackGround,
                                TDE2_RECT_S *pstBackGroundRect, TDE2_SURFACE_S *pstForeGround,
                                TDE2_RECT_S *pstForeGroundRect, TDE2_SURFACE_S *pstDst,
                                TDE2_RECT_S *pstDstRect, TDE2_PATTERN_FILL_OPT_S *pstOpt);

FY_S32 FY_TDE2_EnableRegionDeflicker(FY_BOOL bRegionDeflicker);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* __MPI_TDE_H__ */
