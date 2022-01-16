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


#define DISP_BUF_SIZE (128 * 1024)


static lv_indev_t* g_indev_mouse;


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
	static lv_indev_drv_t indev_drv;
	evdev_init();

    /*Register a mouse input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    g_indev_mouse = lv_indev_drv_register(&indev_drv);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
    lv_img_set_src(mouse_cursor, LV_SYMBOL_SETTINGS);
    lv_indev_set_cursor(g_indev_mouse, mouse_cursor);

	return 0;
}


int view_lvgl_init()
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 1920;
    disp_drv.ver_res    = 1080;
    lv_disp_drv_register(&disp_drv);

    lv_indev_init();

    /*Create a Demo*/
    //lv_demo_widgets();
    //lv_demo_benchmark();
    //lv_demo_keypad_encoder();
    //lv_demo_music();
    //lv_demo_stress();

    //lv_example_arc_1();

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

