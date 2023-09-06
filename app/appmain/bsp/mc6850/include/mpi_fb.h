#ifndef __FB_IOC_H__
#define __FB_IOC_H__

#include "fy_comm_fb.h"
#include <linux/fb.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define IOC_TYPE_FYFB       'F'

/* Standard API */
#define FBIOGET_COLORKEY_FYFB         _IOR(IOC_TYPE_FYFB, 90, FYFB_COLORKEY_S)          /* To obtain the colorkey of an overlay layer */
#define FBIOPUT_COLORKEY_FYFB         _IOW(IOC_TYPE_FYFB, 91, FYFB_COLORKEY_S)          /* To set the colorkey of an overlay layer */
#define FBIOGET_ALPHA_FYFB            _IOR(IOC_TYPE_FYFB, 92, FYFB_ALPHA_S)             /* To get the alpha of an overlay layer */
#define FBIOPUT_ALPHA_FYFB            _IOW(IOC_TYPE_FYFB, 93, FYFB_ALPHA_S)             /* To set the alpha of an overlay layer */
#define FBIOGET_SCREEN_ORIGIN_FYFB    _IOR(IOC_TYPE_FYFB, 94, FYFB_POINT_S)             /* To get the origin of an overlay layer on the screen */
#define FBIOPUT_SCREEN_ORIGIN_FYFB    _IOW(IOC_TYPE_FYFB, 95, FYFB_POINT_S)             /* To set the origin of an overlay layer on the screen */
#define FBIOPUT_SCREENSIZE            _IOW(IOC_TYPE_FYFB, 130, FYFB_SIZE_S*)            /* set the screen output size */
#define FBIOGET_SCREENSIZE            _IOR(IOC_TYPE_FYFB, 131, FYFB_SIZE_S*)            /* get the screen output size */
#define FBIOPUT_SHOW_FYFB             _IOW(IOC_TYPE_FYFB, 101, FY_BOOL)                 /* To set the display state of an overlay layer */
#define FBIOGET_SHOW_FYFB             _IOR(IOC_TYPE_FYFB, 102, FY_BOOL)                 /* To obtain the display state of an overlay layer */
#define FBIOPUT_COMPRESSION_FYFB      _IOW(IOC_TYPE_FYFB, 133, FY_BOOL)                 /*To set the compression status of an overlay layer*/
#define FBIOGET_COMPRESSION_FYFB      _IOR(IOC_TYPE_FYFB, 134, FY_BOOL)                 /*To obtain the compression status of an overlay layer*/
#define FBIOPUT_LAYER_INFO            _IOW(IOC_TYPE_FYFB, 120, FYFB_LAYER_INFO_S*)      /* To set the layer information */
#define FBIOGET_LAYER_INFO            _IOR(IOC_TYPE_FYFB, 121, FYFB_LAYER_INFO_S*)      /* To get the layer information */


/* Unsupport API */
#define FBIOGET_DEFLICKER_FYFB        _IOR(IOC_TYPE_FYFB, 98, FYFB_DEFLICKER_S)         /* To obtain the anti-flicker setting of an overlay layer */
#define FBIOPUT_DEFLICKER_FYFB        _IOW(IOC_TYPE_FYFB, 99, FYFB_DEFLICKER_S)         /* To set the anti-flicker setting of an overlay layer */
#define FBIOGET_VBLANK_FYFB           _IO(IOC_TYPE_FYFB, 100)                           /* To wait for the vertical blanking region of an overlay layer */
#define FBIOGET_CAPABILITY_FYFB       _IOR(IOC_TYPE_FYFB, 103, FYFB_CAPABILITY_S)       /* to obtain the capability of an overlay layer */
#define FBIOPUT_CURSOR_INFO           _IOW(IOC_TYPE_FYFB, 104, FYFB_CURSOR_S *)         /* crusor handle */ /* ATTN:surface in cursor will be released by user*/
#define FBIOGET_CURSOR_INFO           _IOW(IOC_TYPE_FYFB, 105, FYFB_CURSOR_S *)
#define FBIOPUT_CURSOR_STATE          _IOW(IOC_TYPE_FYFB, 106, FY_BOOL *)
#define FBIOGET_CURSOR_STATE          _IOW(IOC_TYPE_FYFB, 107, FY_BOOL *)
#define FBIOPUT_CURSOR_POS            _IOW(IOC_TYPE_FYFB, 108, FYFB_POINT_S *)
#define FBIOGET_CURSOR_POS            _IOR(IOC_TYPE_FYFB, 109, FYFB_POINT_S *)
#define FBIOPUT_CURSOR_COLORKEY       _IOR(IOC_TYPE_FYFB, 110, FYFB_COLORKEY_S *)
#define FBIOGET_CURSOR_COLORKEY       _IOW(IOC_TYPE_FYFB, 111, FYFB_COLORKEY_S *)
#define FBIOPUT_CURSOR_ALPHA          _IOR(IOC_TYPE_FYFB, 112, FYFB_ALPHA_S *)
#define FBIOGET_CURSOR_ALPHA          _IOW(IOC_TYPE_FYFB, 113, FYFB_ALPHA_S *)
#define FBIOPUT_CURSOR_ATTCHCURSOR    _IOW(IOC_TYPE_FYFB, 114, FY_U32 *)
#define FBIOPUT_CURSOR_DETACHCURSOR   _IOW(IOC_TYPE_FYFB, 115, FY_U32 *)
#define FBIOGET_CANVAS_BUFFER         _IOR(IOC_TYPE_FYFB, 123, FYFB_BUFFER_S*)          /* To get canvas buf */
#define FBIO_REFRESH                  _IOW(IOC_TYPE_FYFB, 124, FYFB_BUFFER_S*)          /* To refresh the displayed contents in extended mode */
#define FBIO_WAITFOR_FREFRESH_DONE    _IO(IOC_TYPE_FYFB, 125)                           /* sync refresh*/
#define FBIOFLIP_SURFACE              _IOW(IOC_TYPE_FYFB, 132, FYFB_SURFACEEX_S)        /* To display multiple surfaces in turn & set alpha and colorkey attr */
#define FBIOPUT_MDDRDETECT_FYFB       _IOW(IOC_TYPE_FYFB, 135, FYFB_DDRZONE_S*)         /* To set the DDR detect zone of an overlay layer */
#define FBIOGET_MDDRDETECT_FYFB       _IOW(IOC_TYPE_FYFB, 136, FYFB_DDRZONE_S*)         /* To get the DDR detect zone of an overlay layer */
#define FBIO_RELEASE_FYFB             _IO(IOC_TYPE_FYFB, 140)                           /* To release the layer*/



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FB_IOC_H__ */

