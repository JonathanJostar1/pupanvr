#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "sample_comm.h"
#include "sample_playback.h"
#include "sample_vo.h"
#include "sample_preview.h"
#include "fy_common.h"
#include "menu.h"


//parameters from the command line
#define VALUE_BETWEEN(x,min,max) (((x)>=(min)) && ((x) <= (max)))
#define VALUE_VALID(x,a,b) (((x)!=(a)) && ((x)!=(b)))

extern FY_VOID sample_aio_deinit();
extern FY_VOID SAMPLE_VDA_MdStopAll();
extern FY_VOID SAMPLE_VDA_CdStopAll();

static void print_usage()
{
    printf("Usage: ./nvr_sample\n");
    //printf("\t -c --channels     : the numbers of the vi channels\n");
}

static int parsr_args(int argc, char *argv[])
{
    int result = 0;
    int ch;

    const char *string = "h";
    const struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}

    };
    while((ch = getopt_long(argc, argv, string, long_opts, NULL)) != -1) {

        switch(ch) {
        case 'h':
        default:
            print_usage();
            exit(1);

        }
    }

    return result;

}

int nvr_main(int argc, char *argv[])
{
    int result = 0;

    VB_CONF_S stVbConf;
    //SIZE_S stSize;
    FY_U32 u32BlkSize = 0;

    printf("APP build time: [" __DATE__ "-" __TIME__ "]\n\n");

    //parse the command line
    result = parsr_args(argc, argv);
    if(0 != result) {
        printf("The command line parse failed!\n");
        return -1;
    }

    //stSize.u32Width = HD_WIDTH;
    //stSize.u32Height = HD_HEIGHT;

    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    stVbConf.u32MaxPoolCnt = 1;

    u32BlkSize = 1920 * 1440 * 3/2;
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 4;


    SAMPLE_COMM_SYS_Init(&stVbConf);

    sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);

    sample_vo_init(FY_FALSE, VO_OUTPUT_PAL);

    nvr_menu_main();

    Sample_Playback_Stop();

    sample_vo_deinit(FY_FALSE);

    sample_vo_deinit(FY_TRUE);

    sample_aio_deinit();

    SAMPLE_VDA_MdStopAll();
    SAMPLE_VDA_CdStopAll();

    SAMPLE_COMM_SYS_Exit();

    return result;

}
