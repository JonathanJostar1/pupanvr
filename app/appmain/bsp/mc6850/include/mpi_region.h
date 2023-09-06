#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#include "fy_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*Standard API*/
FY_S32 FY_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
FY_S32 FY_MPI_RGN_Destroy(RGN_HANDLE Handle);

FY_S32 FY_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
FY_S32 FY_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

FY_S32 FY_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

FY_S32 FY_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
FY_S32 FY_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn);

FY_S32 FY_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
FY_S32 FY_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

FY_S32 FY_MPI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);
FY_S32 FY_MPI_RGN_UpdateCanvas(RGN_HANDLE Handle);

/*Unsupport API*/
FY_S32 FY_MPI_RGN_SetAttachField(RGN_HANDLE Handle, VIDEO_FIELD_E enAttachField);
FY_S32 FY_MPI_RGN_GetAttachField(RGN_HANDLE Handle, VIDEO_FIELD_E *penAttachField);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_REGION_H__ */

