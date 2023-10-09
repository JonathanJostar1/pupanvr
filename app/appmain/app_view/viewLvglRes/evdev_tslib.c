#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "lv_drv_conf.h"
#include "lvgl.h"
#include "lvgl/lvgl.h"

#include "evdev_tslib.h"
#include "tslib.h"



int evdev_root_x;
int evdev_root_y;
int evdev_button;

static struct tsdev* g_tsdev = NULL;
struct ts_sample samp;

int evdev_tslib_init()
{
    g_tsdev = ts_setup(NULL, O_RDONLY | O_NONBLOCK);
    if(!g_tsdev)
    {
        printf("ts_setup failure!\n");
        return -1;
    }

    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_button = LV_INDEV_STATE_REL;

    return 0;   
}

void evdev_tslib_exit()
{
    
}

void evdev_tslib_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    if(!g_tsdev)
    {
        //printf("ts is not init!\n");
        return;
    }

    while(ts_read(g_tsdev, &samp, 1) > 0)
    {
        evdev_root_x = samp.x ;
        evdev_root_y = samp.y ;
    }

    if(0 == samp.pressure)
    {
         evdev_button = LV_INDEV_STATE_REL ;
    }   
    else{
        evdev_button = LV_INDEV_STATE_PR ;
    }

    
    data->point.x = evdev_root_x ;
    data->point.y = evdev_root_y ;
    data->state = evdev_button ;
    
    if(data->point.x < 0)
    {
        data->point.x = 0;
    }    

    if(data->point.y < 0)
    {    
        data->point.y = 0;
    }

    if(data->point.x >= drv->disp->driver->hor_res)
    {
        data->point.x = drv->disp->driver->hor_res - 1;
    }

    if(data->point.y >= drv->disp->driver->ver_res)
    {    
        data->point.y = drv->disp->driver->ver_res - 1;
    }

    return ;
}
