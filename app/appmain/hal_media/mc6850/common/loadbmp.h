#ifndef     __LOAD_BMP_H__
#define     __LOAD_BMP_H__

#include "fy_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/* the color format OSD supported */
typedef enum fyOSD_COLOR_FMT_E
{
    OSD_COLOR_FMT_RGB444    = 0,
    OSD_COLOR_FMT_RGB4444   = 1,
    OSD_COLOR_FMT_RGB555    = 2,
    OSD_COLOR_FMT_RGB565    = 3,
    OSD_COLOR_FMT_RGB1555   = 4,
    OSD_COLOR_FMT_RGB888    = 6,
    OSD_COLOR_FMT_RGB8888   = 7,
    OSD_COLOR_FMT_BUTT
}OSD_COLOR_FMT_E;

typedef struct fyOSD_RGB_S
{
    FY_U8   u8B;
    FY_U8   u8G;
    FY_U8   u8R;
    FY_U8   u8Reserved;
}OSD_RGB_S;

typedef struct fyOSD_SURFACE_S
{
    OSD_COLOR_FMT_E enColorFmt;         /* color format */
    FY_U8  *pu8PhyAddr;               /* physical address */
    FY_U16  u16Height;                /* operation height */
    FY_U16  u16Width;                 /* operation width */
    FY_U16  u16Stride;                /* surface stride */
    FY_U16  u16Reserved;
}OSD_SURFACE_S;

typedef struct tag_OSD_Logo
{
    FY_U32    width;        /* out */
    FY_U32    height;       /* out */
    FY_U32    stride;       /* in */
    FY_U8 *   pRGBBuffer;   /* in/out */
}OSD_LOGO_T;

typedef struct tag_OSD_BITMAPINFOHEADER{
        FY_U16      biSize;
        FY_U32       biWidth;
        FY_S32       biHeight;
        FY_U16       biPlanes;
        FY_U16       biBitCount;
        FY_U32      biCompression;
        FY_U32      biSizeImage;
        FY_U32       biXPelsPerMeter;
        FY_U32       biYPelsPerMeter;
        FY_U32      biClrUsed;
        FY_U32      biClrImportant;
} OSD_BITMAPINFOHEADER;

typedef struct tag_OSD_BITMAPFILEHEADER {
        FY_U32   bfSize;
        FY_U16    bfReserved1;
        FY_U16    bfReserved2;
        FY_U32   bfOffBits;
} OSD_BITMAPFILEHEADER; 

typedef struct tag_OSD_RGBQUAD {
        FY_U8    rgbBlue;
        FY_U8    rgbGreen;
        FY_U8    rgbRed;
        FY_U8    rgbReserved;
} OSD_RGBQUAD;

typedef struct tag_OSD_BITMAPINFO {
    OSD_BITMAPINFOHEADER    bmiHeader;
    OSD_RGBQUAD                 bmiColors[1];
} OSD_BITMAPINFO;

typedef struct fyOSD_COMPONENT_INFO_S{

    int alen;
    int rlen;
    int glen;
    int blen;
}OSD_COMP_INFO;

FY_S32 LoadImage(const FY_CHAR *filename, OSD_LOGO_T *pVideoLogo);
FY_S32 LoadBitMap2Surface(const FY_CHAR *pszFileName, const OSD_SURFACE_S *pstSurface, FY_U8 *pu8Virt);
FY_S32 CreateSurfaceByBitMap(const FY_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, FY_U8 *pu8Virt);
FY_S32 CreateSurfaceByCanvas(const FY_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, FY_U8 *pu8Virt, FY_U32 u32Width, FY_U32 u32Height, FY_U32 u32Stride);
FY_S32 GetBmpInfo(const FY_CHAR *filename, OSD_BITMAPFILEHEADER  *pBmpFileHeader,OSD_BITMAPINFO *pBmpInfo);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __LOAD_BMP_H__*/

