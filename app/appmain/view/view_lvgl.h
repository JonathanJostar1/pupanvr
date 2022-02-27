/*
 * view_lvgl.h
 *
 *  Created on: 2022年1月14日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_VIEW_LVGL_H_
#define APPMAIN_APPMAIN_VIEW_VIEW_LVGL_H_

#include "lvgl/examples/lv_examples.h"
#include "lvgl/lvgl.h"
#include "lvgl/lv_port_indev.h"
#include "lvgl/src/extra/libs/freetype/lv_freetype.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_demos/lv_demo.h"

#include "view_lvgl_res.h"

#if 1
LV_FONT_DECLARE(SourceHanSerifSC_Regular_12)
LV_FONT_DECLARE(SourceHanSerifSC_Regular_14)
LV_FONT_DECLARE(SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(SourceHanSerifSC_Regular_20)

LV_FONT_DECLARE(fontawesome_webfont_12)
LV_FONT_DECLARE(fontawesome_webfont_14)
LV_FONT_DECLARE(fontawesome_webfont_16)
LV_FONT_DECLARE(fontawesome_webfont_20)
#endif

/*
 * lvgl相关的初始调用
 * */
int 	view_lvgl_init();
/*
 * 退出lvgl
 * */
void 	view_lvgl_exit();

/*
 * 进入lvgl的UI处理
 * */
void	view_lvgl_process();


#endif /* APPMAIN_APPMAIN_VIEW_VIEW_LVGL_H_ */
