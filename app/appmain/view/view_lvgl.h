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
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_demos/lv_demo.h"

int 	view_lvgl_init();
void 	view_lvgl_exit();

void	view_lvgl_process();


#endif /* APPMAIN_APPMAIN_VIEW_VIEW_LVGL_H_ */
