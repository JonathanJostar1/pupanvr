/*
 * media_hal.h
 *
 *  Created on: 2021年12月10日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_MEDIA_HAL_INCLUDE_MEDIA_HAL_H_
#define APPMAIN_APPMAIN_MEDIA_HAL_INCLUDE_MEDIA_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif

int 	hal_media_init();
void	hal_media_exit();


int		hal_media_fbInit();
void 	hal_media_fbexit();

int     hal_media_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo);

#ifdef __cplusplus
}
#endif

#endif /* APPMAIN_APPMAIN_MEDIA_HAL_INCLUDE_MEDIA_HAL_H_ */
