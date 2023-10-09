/*
 * onvif_client_interface.h
 *
 *  Created on: 2021年12月9日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_ONVIF_CLIENT_ONVIF_CLIENT_INTERFACE_H_
#define APPMAIN_APPMAIN_ONVIF_CLIENT_ONVIF_CLIENT_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

int 	onvif_client_init();
void 	onvif_clinet_exit();

int		onvif_search_start();
void	onvif_search_stop();


#ifdef __cplusplus
}
#endif

#endif /* APPMAIN_APPMAIN_ONVIF_CLIENT_ONVIF_CLIENT_INTERFACE_H_ */
