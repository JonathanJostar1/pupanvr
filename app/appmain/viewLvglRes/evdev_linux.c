#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#include "evdev_linux.h"

#define DEFAULT_MOUSE_DEV "/dev/input/event0"

int g_evdev_linux_flag = 0;

int evdev_fd = 0;
int evdev_root_x = 0;
int evdev_root_y = 0;
int evdev_button = 0;

int evdev_key_val = 0;

int ver_res = 0;
int hor_res = 0;

int _evdev_init()
{
    evdev_fd = open(DEFAULT_MOUSE_DEV, O_RDWR | O_NOCTTY | O_NDELAY);

    if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return -1;
    }

    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_key_val = 0;
    evdev_button = LV_INDEV_STATE_REL;

    printf("evdev_init ok!\n");

    return 0;
}

/**
 * Initialize the evdev
 */
void evdev_linux_init(void)
{
    if(!evdev_fd)
    {
        _evdev_init();
    }
}

/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_linux_set_file(char* dev_name)
{
    return true;
}

/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 */

// REL_X:0, REL_Y:1 相对坐标, EV_ABS:3, 绝对坐标, EV_KEY:1,健盘
void evdev_linux_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    if(!evdev_fd)
    {
        _evdev_init();    
    }

    if(!evdev_fd)
    {
        return;
    }

    struct input_event in;

    while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) 
    {
        //printf("REL_X:%d, EV_ABS:%d, EV_KEY:%d, type:%d code:%d value:%d\n", REL_X, EV_ABS, EV_KEY, in.type, in.code, in.value);
        if(in.type == EV_REL) 
        {
            if(in.code == REL_X)
				evdev_root_x += in.value;
            else if(in.code == REL_Y)
				evdev_root_y += in.value;
        } 
        else if(in.type == EV_ABS) 
        {
            if(in.code == ABS_X)
				evdev_root_x = in.value;
            else if(in.code == ABS_Y)
				evdev_root_y = in.value;
            else if(in.code == ABS_MT_POSITION_X)
                evdev_root_x = in.value;
            else if(in.code == ABS_MT_POSITION_Y)
                evdev_root_y = in.value;
            else if(in.code == ABS_MT_TRACKING_ID)
            {
            	if(in.value == -1)
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 0)
                    evdev_button = LV_INDEV_STATE_PR;
            }
        } 
        else if(in.type == EV_KEY) 
        {
            if(in.code == BTN_MOUSE || in.code == BTN_LEFT || in.code == BTN_RIGHT || in.code == BTN_MIDDLE) 
            {
                if(in.value == 0)
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 1)
                    evdev_button = LV_INDEV_STATE_PR;

                if(in.code == BTN_LEFT)
                {
                    evdev_key_val = LV_KEY_LEFT;     
                }
                else if(in.code == BTN_RIGHT)  
                {
                    evdev_key_val = LV_KEY_RIGHT;     
                }
                else if(in.code == BTN_MIDDLE)  
                {
                    evdev_key_val = LV_KEY_RIGHT;     
                }
            } 
        }
    }

    
    /*Store the collected data*/
    data->key = evdev_key_val;
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;

    if(hor_res == 0)
    {
        if(drv->disp->driver->sw_rotate == 1)
        {
            if(drv->disp->driver->rotated == LV_DISP_ROT_90)
            {
                hor_res = drv->disp->driver->ver_res;
                ver_res = drv->disp->driver->hor_res; 
            }
            else if(drv->disp->driver->rotated == LV_DISP_ROT_270)
            {
                hor_res = drv->disp->driver->ver_res;
                ver_res = drv->disp->driver->hor_res; 
            }
            else if(drv->disp->driver->rotated == LV_DISP_ROT_180)
            {
                hor_res = drv->disp->driver->hor_res;
                ver_res = drv->disp->driver->ver_res; 
            }
        }else{
            hor_res = drv->disp->driver->hor_res;
            ver_res = drv->disp->driver->ver_res; 
        } 

    }
    
    data->state = evdev_button;

    if(data->point.x < 0)
    {
        data->point.x = 0;
    }  
    if(data->point.y < 0)
    {
      data->point.y = 0;
    }

    if(data->point.x >= hor_res)
    {
        data->point.x = hor_res - 1;
    }  
    if(data->point.y >= ver_res)
    {
      data->point.y = ver_res - 1;
    }

    if(drv->disp->driver->sw_rotate == 1)
    {
        if(drv->disp->driver->rotated == LV_DISP_ROT_90)
        {
            int x1 = data->point.x;   
            int y1 = data->point.y;

            data->point.x = y1;
            data->point.y = hor_res - x1 - 1;
        }
        else if(drv->disp->driver->rotated == LV_DISP_ROT_270)
        {
            int x1 = data->point.x;   
            int y1 = data->point.y;

            data->point.x = ver_res - y1 - 1;
            data->point.y = x1;
        }
        else if(drv->disp->driver->rotated == LV_DISP_ROT_180)
        {
            int x1 = data->point.x;   
            int y1 = data->point.y;

            data->point.x = hor_res - x1 - 1;
            data->point.y = ver_res - y1 - 1;
        } 
    }

    //printf("==============[%d, %d]=====\n", data->point.x, data->point.y);

    return;    
}
