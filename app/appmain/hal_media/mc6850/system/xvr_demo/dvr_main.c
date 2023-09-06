#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "sample_comm.h"
#include "sample_playback.h"
#include "sample_vo.h"
#include "sample_vi.h"
#include "sample_preview.h"
#include "nna_sample.h"
#include "fy_common.h"
#include "menu.h"


//parameters from the command line
#define VALUE_BETWEEN(x,min,max) (((x)>=(min)) && ((x) <= (max)))
#define VALUE_VALID(x,a,b) (((x)!=(a)) && ((x)!=(b)))

//the numbers of vi channels, default t0 4
static int s_param_channels = 8;
static int s_vi_ec = VI_EC_BYTE_MODE_128byte;
static int s_vi_lite = 1;
static int s_vi_5m = 0;
static int s_is_fy02 = FY_FALSE;


extern FY_VOID sample_aio_deinit();
extern FY_VOID SAMPLE_VDA_MdStopAll();
extern FY_VOID SAMPLE_VDA_CdStopAll();

static void print_usage()
{
    printf("Usage: ./xvr_sample -c numbers_of_vi_channels \n");
    printf("\t -c --channels     : the numbers of the vi channels\n");
    printf("\t -t --type     : the numbers of the vi channels\n");
}

static int parsr_args(int argc, char *argv[])
{
    int result = 0;
    int ch;

    const char *string = "c:e:l:t:h";
    const struct option long_opts[] = {
        {"channels", required_argument, NULL, 'c'},
        {"ec", required_argument, NULL, 'e'},
        {"lite", required_argument, NULL, 'l'},
        {"type", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}

    };
    while((ch = getopt_long(argc, argv, string, long_opts, NULL)) != -1) {

        switch(ch) {
        case 'c':
            s_param_channels = atoi(optarg);
            if(VALUE_VALID(s_param_channels, 4, 8)) {
                printf("the numbers of vi channels must be 4 or 8!\n\n");
                return -1;
            }
            break;

        case 'e':
            s_vi_ec = atoi(optarg);
            if(!VALUE_BETWEEN(s_vi_ec, VI_EC_LEVEL_NONE, VI_EC_BYTE_MODE_192byte)) {
                printf("the ec of vi channels must be 0 ~ 3!\n\n");
                return -1;
            }
            break;
        case 't':
            s_vi_5m = atoi(optarg);
            if(!VALUE_BETWEEN(s_vi_5m, 0, 1)) {
                printf("the use 5M must be 0 ~ 1!\n\n");
                return -1;
            }
            break;
         case 'l':
            s_vi_lite = atoi(optarg);

            break;
        case 'h':
        default:
            print_usage();
            exit(1);

        }
    }

    return result;

}

static FY_BOOL chip_is_fy02()
{
    char *chip_name = SAMPLE_COMM_Get_Chip_Name();

    if(NULL == chip_name) {
        return FY_FALSE;
    }
    printf("Current CHIP is %s\n", chip_name);
    if(0 == strncasecmp(chip_name, "fy02", 4))  {
        return FY_TRUE;
    } else if(0 == strncasecmp(chip_name, "mc6650", 6))  {
        return FY_TRUE;
    }

    return FY_FALSE;

}

int dvr_main(int argc, char *argv[])
{
    int result = 0;
    srand(time(NULL));
    VB_CONF_S stVbConf;
    //SIZE_S stSize;
    FY_U32 u32BlkSize = 0;
    FY_U32 u32MaxVIBlkSize = 0;
    struct vi_channels_info vi_info;
    int vbpoolcnt = 0;
    int cnt = 0;

    printf("APP build time: [" __DATE__ "-" __TIME__ "]\n\n");

    //parse the command line
    result = parsr_args(argc, argv);
    if(0 != result) {
        printf("The command line parse failed!\n");
        return -1;
    }

    //stSize.u32Width = HD_WIDTH;
    //stSize.u32Height = HD_HEIGHT;

    s_is_fy02 = chip_is_fy02();
    if(FY_TRUE == s_is_fy02) {
        //for mc6650, force to support 5M
        s_vi_5m = 1;
    }

    memset(&vi_info, 0, sizeof(vi_info));
    sample_vi_init_channels(s_is_fy02, &vi_info);

    printf("%s: vi info: \n  ad chips = %d\n  channel = %d\n  chn by mux = 0x%08x\n",
            __func__, vi_info.ad_chips, vi_info.channels, vi_info.channels_by_mux);


    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    vbpoolcnt = 0;

    //number of 4mux channel
    cnt = (vi_info.channels_by_mux) & 0xff;
    if(cnt > 0) {
        u32BlkSize = 1280 * 1944 * 3 / 2;
        stVbConf.astCommPool[vbpoolcnt].u32BlkSize = u32BlkSize;
        stVbConf.astCommPool[vbpoolcnt].u32BlkCnt = cnt * 4;
        ++vbpoolcnt;
        u32MaxVIBlkSize = u32BlkSize;
    }

    //number of 2mux channel
    cnt = (vi_info.channels_by_mux >> 8) & 0xff;
    if(cnt > 0) {
        u32BlkSize = 2688 * 2048 * 3 / 2;
        stVbConf.astCommPool[vbpoolcnt].u32BlkSize = u32BlkSize;
        stVbConf.astCommPool[vbpoolcnt].u32BlkCnt = cnt * 4;
        ++vbpoolcnt;
        u32MaxVIBlkSize = u32BlkSize;
    }

    //number of 1mux channel
    cnt = (vi_info.channels_by_mux >> 16) & 0xff;
    if(cnt > 0) {
        u32BlkSize = 3840 * 2160 * 3 / 2;
        stVbConf.astCommPool[vbpoolcnt].u32BlkSize = u32BlkSize;
        stVbConf.astCommPool[vbpoolcnt].u32BlkCnt = cnt * 4;
        ++vbpoolcnt;
        u32MaxVIBlkSize = u32BlkSize;

    }

    u32BlkSize = 3840 * 2160 * 2 + 2048;
    stVbConf.astCommPool[vbpoolcnt].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[vbpoolcnt].u32BlkCnt = 2;
    ++vbpoolcnt;


    stVbConf.astCommPool[vbpoolcnt].u32BlkSize = 352 * 288 * 3 / 2 * 70 / 100;
    stVbConf.astCommPool[vbpoolcnt].u32BlkCnt   = vi_info.channels * 2;
    ++vbpoolcnt;

    stVbConf.astCommPool[vbpoolcnt].u32BlkSize = 720 * 576 * 2;
    stVbConf.astCommPool[vbpoolcnt].u32BlkCnt  = vi_info.channels * 2;
    ++vbpoolcnt;

    stVbConf.u32MaxPoolCnt = vbpoolcnt;



    SAMPLE_COMM_SYS_Init(&stVbConf);

    if(s_vi_5m){
        sample_vo_set_compress(FY_FALSE);
        SAMPLE_VPSS_SET_OVERLAY_NUM(1);
        sample_vi_set_5M(s_vi_5m);
        sample_preview_set_5M(s_vi_5m);
    }
    else{
        SAMPLE_VPSS_SET_OVERLAY_NUM(1);
    }
    sample_vi_set_buff(u32MaxVIBlkSize);
    //sample_vo_set_compress(FY_FALSE);
    sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);

    sample_vo_init(FY_FALSE, VO_OUTPUT_PAL);
    //viu init
    {
        PIXEL_FORMAT_E pf = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
        if((FY_FALSE == s_is_fy02) && (FY_TRUE == s_vi_5m)) {
            pf = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
            s_vi_ec = VI_EC_LEVEL_NONE;
        }
        sample_vi_init(s_is_fy02, s_vi_ec, pf);
    }

    sample_preview_init(vi_info.channels * 2);

    dvr_menu_main();

    sample_preview_deinit();

    Sample_Playback_Stop();

    sample_vi_exit();

    sample_vo_deinit(FY_FALSE);

    sample_vo_deinit(FY_TRUE);

    sample_aio_deinit();

    SAMPLE_VDA_MdStopAll();
    SAMPLE_VDA_CdStopAll();

    SAMPLE_COMM_SYS_Exit();



    return result;

}
