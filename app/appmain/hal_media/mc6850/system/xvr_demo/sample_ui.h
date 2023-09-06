#ifndef __XVR_SAMPLE_UI_H__
#define __XVR_SAMPLE_UI_H__

#include "fy_type.h"
#include <linux/fb.h>


FY_S32 ui_start();

FY_VOID ui_manualSwitch();

FY_VOID ui_autoSwitch();

FY_VOID ui_stop();

FY_S32 ui_testmode(FY_S32 testmode);

FY_S32 TDE_CreateSurfaceFromFile(const FY_CHAR *pszFileName, TDE2_SURFACE_S *pstSurface, FY_U8 *pu8Virt);

FY_S32 TDE_DrawUiBySize(FY_S32 fd, TDE2_SURFACE_S *pstScreen, TDE2_SURFACE_S *pstBackGround, FY_BOOL bPartail, struct fb_var_screeninfo *pstVarInfo);

FY_VOID ui_stop_pushed();

FY_S32 ui_query();

FY_BOOL ui_getState();

#endif//__XVR_SAMPLE_UI_H__
