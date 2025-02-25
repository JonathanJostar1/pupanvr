/*
 * view_lvgl.cpp
 *
 *  Created on: 2022年1月14日
 *      Author: jhting
 */

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "view_lvgl.h"
#include "view_lvgl_res.h"
#include "tlog.h"

#include "evdev_linux.h"
#include "evdev_tslib.h"
#include "halmedia_fbdev.h"


#define DISP_BUF_SIZE (128 * 1024)


static lv_indev_t* g_indev_mouse;

//extern lv_img_dsc_t aero_arrow;
//extern lv_img_dsc_t arrow_m;
extern lv_img_dsc_t arrow_48;


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

static int lv_indev_init()
{
	static lv_style_t style;
	static lv_indev_drv_t indev_drv;
    static lv_indev_drv_t indev_drv_tslib;

    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_color_hex(0xFFFFFF));

	evdev_linux_init();
    /*Register a mouse input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_linux_read;
    g_indev_mouse = lv_indev_drv_register(&indev_drv);

#if 0
    evdev_tslib_init();

    lv_indev_drv_init(&indev_drv_tslib);
    indev_drv_tslib.type = LV_INDEV_TYPE_POINTER;
    indev_drv_tslib.read_cb = evdev_tslib_read;
    lv_indev_drv_register(&indev_drv_tslib);
#endif

#if 1
    /*Set cursor. For simplicity set a HOME symbol now.*/
    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
    //lv_obj_add_style(mouse_cursor, &style, 0);
    lv_img_set_src(mouse_cursor, &arrow_48);
    
#else
    lv_obj_t* mouse_cursor = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(mouse_cursor, &fontawesome_webfont_16, 0);
    lv_obj_add_style(mouse_cursor, &style, 0);
    lv_label_set_text(mouse_cursor, "\x80");
#endif

    lv_indev_set_cursor(g_indev_mouse, mouse_cursor);

	return 0;
}

static void lv_log_print_g_cb_(const char * buf)
{
	LOG(INFO) << buf << endl;
}

int view_lvgl_init()
{
    /*LittlevGL init*/
    lv_init();

    //lv_log_register_print_cb(lv_log_print_g_cb_);

    lv_extra_init();

    view_lvgl_res_init();

    /*Linux frame buffer device init*/
#ifdef FBDEV_USE_HALMEDIA
    halmedia_fbdev_init();
#else
    fbdev_init();
#endif

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
#ifdef FBDEV_USE_HALMEDIA
    disp_drv.flush_cb   = halmedia_fbdev_flush;
#else
    disp_drv.flush_cb   = fbdev_flush;
#endif
    disp_drv.hor_res    = 1920;
    disp_drv.ver_res    = 1080;
    lv_disp_drv_register(&disp_drv);

    lv_indev_init();

    return 0;
}

void view_lvgl_exit()
{

    return;
}

void	view_lvgl_process()
{
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1)
    {
        lv_task_handler();
        usleep(5000);
    }
}
