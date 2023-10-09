#ifndef EVDEV_LINUX_H
#define EVDEV_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_drv_conf.h"
#include "lvgl.h"
#include "lvgl/lvgl.h"

/**
 * Initialize the evdev
 */
void evdev_linux_init(void);
/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_linux_set_file(char* dev_name);
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 */
void evdev_linux_read(lv_indev_drv_t * drv, lv_indev_data_t * data);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EVDEV_LINUX_H */
