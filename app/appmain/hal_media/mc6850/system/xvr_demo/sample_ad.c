#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "sample_comm.h"
#include "list.h"

#include "sample_ad.h"


LIST_HEAD(ad_device_list);


ad_device* ad_device_find(const char *name)
{
    ad_device *pdev = NULL;

    list_for_each_entry(pdev, &ad_device_list, list) {
		if (0 == strcmp(name, pdev->name)) {
			return pdev;
		}
	}
    return NULL;
}


static int ad_device_register(ad_device *pDevice)
{
    ad_device *pdev = NULL;
    if(NULL == pDevice) {
        return -EN_ERR_ILLEGAL_PARAM;
    }
    printf("%s: register the ad device: %s\n", __func__, pDevice->name);
    pdev = ad_device_find(pDevice->name);
    if(NULL == pdev) {
        printf("%s: register the ad device: %s to the AD device list!\n", __func__, pDevice->name);
        list_add(&pDevice->list, &ad_device_list);
    }
    return 0;
}


extern ad_device *__start_ad_device_section;
extern ad_device *__stop_ad_device_section;


int ad_device_init()
{
    int result = 0;
    INIT_LIST_HEAD(&ad_device_list);

    ad_device **iter = &__start_ad_device_section;
    for(;iter < &__stop_ad_device_section; ++iter) {
        printf("Init the ad device: %s\n", (*iter)->name);
        if((*iter)->fpInit) {
            result = (*iter)->fpInit();
            if(0 == result) {
                result = ad_device_register((*iter));
            } else {
                printf("%s: ad %s->fpInit failed, ret = 0x%x!\n", __func__, (*iter)->name, result);
            }
        }
    }
    return result;

}

int ad_device_cleanup()
{
     ad_device *p, *n;

    list_for_each_entry_safe(p, n, &ad_device_list, list) {
		p->fpExit();
        list_del(&p->list);
	}
    return 0;
}


static ad_device dummy_device = {
    .name = "dummy",
    .fpGetChips = NULL,
    .fpGetMuxMode = NULL,
    .fpGetVideLoss = NULL,
    .fpGetVideMode = NULL,
    .fpSetPTZData = NULL,
    .fpInit = NULL,
    .fpExit = NULL,

};

ad_device *p_dummy_dev __attribute__((section("ad_device_section")))  = &dummy_device;
