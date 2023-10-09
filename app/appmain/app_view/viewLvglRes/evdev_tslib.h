
#ifndef __EVDEV_TSLIB_H__
#define __EVDEV_TSLIB_H__


#ifdef __cplusplus
extern "C" {
#endif

int     evdev_tslib_init();
void    evdev_tslib_exit();

void    evdev_tslib_read(lv_indev_drv_t * drv, lv_indev_data_t * data);


#ifdef __cplusplus
}
#endif

#endif