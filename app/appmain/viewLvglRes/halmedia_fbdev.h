
#ifndef MSTARFB_FBDEV_H
#define MSTARFB_FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"

#include "lvgl.h"

//#include "lvgl/lvgl.h"


void halmedia_fbdev_init(void);
void halmedia_fbdev_exit(void);
void halmedia_fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
void halmedia_fbdev_get_sizes(uint32_t *width, uint32_t *height, uint32_t *line_length);
char* halmedia_fbdev_get_framebufferMapAddr();
/**
 * Set the X and Y offset in the variable framebuffer info.
 * @param xoffset horizontal offset
 * @param yoffset vertical offset
 */
void halmedia_fbdev_set_offset(uint32_t xoffset, uint32_t yoffset);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*MSTARFB_FBDEV_H*/
