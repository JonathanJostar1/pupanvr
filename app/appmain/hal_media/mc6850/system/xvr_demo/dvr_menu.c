#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "menu.h"
#include "sample_comm.h"
#include "sample_playback.h"
#include "sample_ui.h"
#include "sample_veu.h"
#include "sample_jpege.h"
#include "sample_vo.h"
#include "sample_preview.h"
#include "sample_vi.h"
#include "nna_sample.h"

/**
 * sample ansi color code
 *
 */
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RESET  "\x1b[0m"

#define MENU_ITEM_INDEX(idx, content) \
    printf(ANSI_COLOR_GREEN "[%2d]" ANSI_COLOR_RESET "    %s\n", idx, content)

#define MENU_ITEM_CHAR(ch, content) \
    printf(ANSI_COLOR_GREEN "[%2c]" ANSI_COLOR_RESET "    %s\n", ch, content)

static void flush_stdin()
{
    char c = 0;
    while((c = getchar()) != '\n' && c != EOF) ;
}
static int show_viconfigmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE:  Camera Control" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_CHAR('m', "Menu");
    MENU_ITEM_CHAR('u', "up");
    MENU_ITEM_CHAR('d', "down");
    MENU_ITEM_CHAR('l', "left");
    MENU_ITEM_CHAR('r', "right");

    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_viuconfig()
{
    int result = 0;
    char option = 0;

    while(1) {
        int cmd = 0;
        show_viconfigmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }
        if(option == 'm') {
            cmd = 1;
        } else if(option == 'u') {
             cmd = 2;
        } else if(option == 'd') {
             cmd = 3;
        } else if(option == 'l') {
             cmd = 4;
        } else if(option == 'r') {
             cmd = 5;
        } else {
            cmd = 0;
        }

        if(cmd > 0) {
            sample_vi_camera_cmd(cmd);
        }
    }

    return result;

}

static int show_previewmenu_sub()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: Preview Sub" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "1x1 Preview");
    MENU_ITEM_INDEX(++index, "2x2 Preview");
    MENU_ITEM_INDEX(++index, "3x3 Preview");
    MENU_ITEM_INDEX(++index, "1x5 Preview");
    MENU_ITEM_INDEX(++index, "1x7 Preview");
    MENU_ITEM_INDEX(++index, "4x4 Preview");

    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_preview_sub()
{
    int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        items = show_previewmenu_sub();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }
        if(option < '0' || option > ('1' + items)) {
            continue;
        }

        //Selects the course of action specified by the option
        switch(option) {
        case '1':
            sample_preview_start(VO_MODE_1MUX, 0);
            break;
        case '2':
            sample_preview_start(VO_MODE_4MUX, 0);
            break;
        case '3':
            sample_preview_start(VO_MODE_9MUX, 0);
            break;
        case '4':
            sample_preview_start(VO_MODE_1B_5S, 0);
            break;
        case '5':
            sample_preview_start(VO_MODE_1B_7S, 0);
            break;
        case '6':
            sample_preview_start(VO_MODE_16MUX, 0);
            break;
        default :
            break;
        }
    }
    return result;

}

static int show_ele_previewmenu_sub()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: Electronic Zoom Preview Sub" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "1 Zoom up");
    MENU_ITEM_INDEX(++index, "2 Zoom down");

    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}


static int menu_ele_preview_sub()
{
    int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        items = show_ele_previewmenu_sub();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            sample_ele_preview_start(2);
            sample_preview_start(VO_MODE_1MUX, 0);
            break;
        }
        if(option < '0' || option > ('1' + items)) {
            continue;
        }

        //Selects the course of action specified by the option
        switch(option) {
        case '1':
            sample_ele_preview_start(1);
            break;
        case '2':
            sample_ele_preview_start(0);
            break;
        default :
            break;
        }
    }
    return result;

}


static int show_previewmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: Preview" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "HDMI 1024*768/60HZ preview test");
    MENU_ITEM_INDEX(++index, "HDMI 1280*720/60HZ preview test");
    MENU_ITEM_INDEX(++index, "HDMI 1280*1024/60HZ preview test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ preview test");
	MENU_ITEM_INDEX(++index, "HDMI 2560x1440/60HZ preview test");
	MENU_ITEM_INDEX(++index, "HDMI 3840x2160/60HZ preview test");
    MENU_ITEM_INDEX(++index, "Auto Preview");
	MENU_ITEM_INDEX(++index, "Electronic Zoom");
	MENU_ITEM_INDEX(++index, "PIP disable/enable");
    MENU_ITEM_INDEX(++index, "stop Preview");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}

static int show_pipmenu()
{
    int index = 0;

    //show the pip menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: pip" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, " 1 enable pip layer ");
    MENU_ITEM_INDEX(++index, " 2 disable pip layer ");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}

static FY_S32 Sample_preview_PipDeinit(FY_VOID)
{
    FY_S32 s32Ret = FY_SUCCESS;
    VO_LAYER VoLayer = SAMPLE_VO_LAYER_VPIP;

    FY_MPI_VO_DisableChn(VoLayer, 0);
    s32Ret = SAMPLE_COMM_VO_StopLayer(VoLayer);

    return s32Ret;
}


static int menu_pip_enable()
{
	int items = 0;
	char option = 0;
	int result = 0;
	FY_BOOL ENABLE_PIP = FY_FALSE;

	while(1) {
		items = show_pipmenu();
		//handle menu command
		option = getchar();
		flush_stdin();
		if(option == 'q') {
			break;
		}
		if(option < '0' || option > ('1' + items)) {
			continue;
		}
	
		//Selects the course of action specified by the option
		switch(option) {
		case '1':
			ENABLE_PIP = FY_TRUE;
			sample_PIP_enable(ENABLE_PIP);
			sample_PIP_enable_pre(ENABLE_PIP);
			sample_preview_stop();
			sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60); // VO_OUTPUT_3840x2160_30
            sample_preview_start(VO_MODE_1MUX, 0);
			menu_preview_sub();
			break;
		case '2':
			ENABLE_PIP = FY_FALSE;
			sample_PIP_enable(ENABLE_PIP);
			sample_PIP_enable_pre(ENABLE_PIP);
			sample_preview_stop();
			Sample_preview_PipDeinit();
			sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60); // VO_OUTPUT_3840x2160_30
            sample_preview_start(VO_MODE_1MUX, 0);
			menu_preview_sub();
			break;
		}
	}
	return result;

}

static int menu_preview()
{
    int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        items = show_previewmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }
        if(option < '0' || option > ('1' + items)) {
            continue;
        }

        //Selects the course of action specified by the option
        switch(option) {
        case '1':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1024x768_60);
            menu_preview_sub();
            break;
        case '2':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_720P60);
            menu_preview_sub();
            break;
        case '3':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1280x1024_60);
            menu_preview_sub();
            break;
        case '4':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            menu_preview_sub();
            break;
        case '5':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_2560x1440_60);
            menu_preview_sub();
            break;
        case '6':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_3840x2160_30);
            menu_preview_sub();
            break;
		case '7':
            sample_preview_start(VO_MODE_1MUX, 1);
            break;
		case '8':
            sample_preview_stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            sample_preview_start(VO_MODE_1MUX, 2);
            menu_ele_preview_sub();
			break;
		case '9':
			menu_pip_enable();
            break;
        case 'a':
            sample_preview_stop();
            break;
        default :
            break;
        }
    }
    return result;

}

static int show_encodingmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: Encoding" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");      //1
    MENU_ITEM_INDEX(++index, "stop");       //2
    MENU_ITEM_INDEX(++index, "switch");     //3
    MENU_ITEM_INDEX(++index, "pause");      //4
    MENU_ITEM_INDEX(++index, "chn num");    //5
    MENU_ITEM_INDEX(++index, "record path");//6
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}

static int menu_encoding()
{
    int items = 0;
    int result = 0;
    char option = 0;
    char num = 0;
    char path[512];
    static char ch_num = 8;
    static FY_BOOL bEnalbeLog=FY_FALSE;

    sample_encode_set_chn_num(ch_num);

    while(1) {
        items = show_encodingmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }
        if((option < '0' || option > ('1' + items)) && option != 'l') {
            continue;
        }

        //Selects the course of action specified by the option
        switch(option) {
        case '1':
            //start encode
            result = sample_encode_test_start(FY_FALSE, FY_FALSE);
            break;
        case '2':
            //stop encode
            result = sample_encode_test_stop();
            break;
        case '3':
            //start auto switch
            result = sample_encode_test_auto_switch();
            break;
        case '4':
            //pause auto switch
            result = sample_encode_test_switch_pause();
            break;
        case '5':
            //pause auto switch
            printf("\tinput channel number[1~%d]:", VENC_MAX_TEST_CHAN_NUM);
            scanf("%d", (int*)&num);
            getchar();
            result = sample_encode_set_chn_num(num);
            ch_num = num;
            break;
        case '6':
            printf("\tinput record path:");
            scanf("%s", path);
            getchar();
            printf("\n\tget record path::%s\n", path);
            result = sample_encode_set_storage_path(path);
            break;
        case 'l': //enable log
            bEnalbeLog = !bEnalbeLog;
            sample_encode_enable_log(bEnalbeLog);
            break;
#if 0
        case '7':
            printf("\t  1: 5M(2592x1952);\n");
            printf("\t  2: 5M-N(1280x1952);\n");
            printf("\t  3: 1920x1088;\n");
            printf("\t  4: 1280x720;\n");
            printf("\t  5: 960x1088;\n");
            printf("\t  6: User defined;\n");
            printf("\n  Select encoding stream res: ");
            scanf("%d", (int*)&num);
            getchar();
            printf("  Get res setting: %d\n", num);
            if(num > 6)
            {
                printf("\n  item [%d] is invalid\n", num);
                break;
            }
            switch(num)
            {
                case ENC_RES_5M_2592X1952:
                    w=2592; h=1952;
                    break;
                case ENC_RES_5MN_1280X1952:
                    w=1280; h=1952;
                    break;
                case ENC_RES_1920X1088:
                    w=1920; h=1088;
                    break;
                case ENC_RES_1280X720:
                    w=1280; h=720;
                    break;
                case ENC_RES_960X1088:
                    w=960; h=1088;
                    break;
                default:
                {
                    printf("  input width:  "); scanf("%d", (int*)&w);
                    getchar();
                    printf("  input height: "); scanf("%d", (int*)&h);
                    getchar();
                    break;
                }
            }

            result = sample_encode_set_resolution(w, h);
            break;
#endif
        default :
            break;
        }
    }
    return result;

}

static int show_jpegemenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: jpege" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "stop");
    MENU_ITEM_INDEX(++index, "switch");
    MENU_ITEM_INDEX(++index, "pause");
    MENU_ITEM_INDEX(++index, "chn num");
    MENU_ITEM_INDEX(++index, "snap num");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_jpege()
{
    int items = 0;
    int result = 0;
    char option = 0;
    char num = 0;
    static char ch_num = 8;
    static FY_BOOL bEnalbeLog=FY_FALSE;

    sample_jpege_set_chn_num(ch_num);

    while(1) {
        items = show_jpegemenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }
        if((option < '0' || option > ('1' + items)) && option != 'l') {
            continue;
        }

        //Selects the course of action specified by the option
        switch(option) {
        case '1':
            //start encode
            result = sample_jpege_test_start(FY_FALSE);
            break;
        case '2':
            //stop encode
            result = sample_jpege_test_stop();
            break;
        case '3':
            //start auto switch
            result = sample_jpege_test_auto_switch();
            break;
        case '4':
            //pause auto switch
            result = sample_jpege_test_switch_pause();
            break;
        case '5':
            //pause auto switch
            printf("\tinput channel number[1~8]:");
            scanf("%d", (int*)&num);
            getchar();
            if(num < 1 || num > 8) {
                printf("\n\tchannel number %d should be [1,8]\n", num);
                break;
            }
            result = sample_jpege_set_chn_num(num);
            ch_num = num;
            break;
        case '6':
            //pause auto switch
            printf("\tinput snap number[1~100]:");
            scanf("%d", (int*)&num);
            getchar();
            printf("\n\tget snap number:%d\n", num);
            result = sample_jpege_set_snap_num(num);
            ch_num = num;
            break;
        case 'l': //enable log
            bEnalbeLog = !bEnalbeLog;
            sample_jpege_enable_log(bEnalbeLog);
            break;
        default :
            break;
        }
    }
    return result;

}

static int show_playbackmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: Playback" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "HDMI 1024*768/60HZ playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1280*720/60HZ playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1280*1024/60HZ playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback test");
    MENU_ITEM_INDEX(++index, "CVBS playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback 1 channel 4x");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback 1 channel 1x / 4x change");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ 2x2 4 channels playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ 2x2 1920x1080p@30fps 2 channels playback test(performance)");
    MENU_ITEM_CHAR('a', "HDMI 1920*1080/60HZ 4x4 16 channels playback test");
    MENU_ITEM_CHAR('b', "HDMI 1920*1080/60HZ  8M playback test");
    MENU_ITEM_CHAR('l', "HDMI 1920*1080/60HZ 4x4 (12xVideo + 4xD1 jpeg/mjpg) channels playback test");
    MENU_ITEM_CHAR('s', "stop playback");

    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_playback()
{
    //int items = 0;
    int result = 0;
    char option = 0;
    char* stream_file_h264_name = "/nfs/stream/normal/h264/h264_960x1080_15fps_real_ip_15gop_800kbps.h264";
    char* stream_file_h264_5M_name = "/nfs/stream/normal/h264/3840x2160_25fps_4M.h264";
    char* stream_file_h265_name = "/nfs/stream/normal/h265/h265_960x1080_15fps_real_ip_15gop_800bps.265";
    char* stream_1080p30_file_name = "/nfs/stream/normal/h265/h265_1920x1080P_30fps_8Mbps.265";
    //PAYLOAD_TYPE_E pt_type  = PT_H264;

    char *stream_performance_file_name = stream_1080p30_file_name;
    //PAYLOAD_TYPE_E performance_pt_type  = PT_H265;
    char* stream_file_jpg = "/nfs/stream/normal/jpg/flower1.jpg";
    char* stream_file_mjpg = "/nfs/stream/normal/mjpg/TimelapseClouds_720x576.mjp";

    int i;
    PAYLOAD_TYPE_E pt_types[VDEC_MAX_CHN_NUM];
    char *filenames[VDEC_MAX_CHN_NUM];

    int fb_cnt = 3;
    sample_preview_stop();


    while(1) {
        show_playbackmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        //Selects the course of action specified by the option
        result = 0;
        for(i = 0; i < VDEC_MAX_CHN_NUM; i++) {
            if((i % 4) < 2) {
                pt_types[i] = PT_H265;
                filenames[i] = stream_file_h265_name;
            } else {
                pt_types[i] = PT_H264;
                filenames[i] = stream_file_h264_name;
            }
        }
        switch(option) {
        case '1':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1024x768_60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '2':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_720P60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '3':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1280x1024_60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '4':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '5':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_FALSE);
            sample_vo_init(FY_FALSE, VO_OUTPUT_PAL);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VSD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '6':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(1, VO_MODE_1MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 60);
            break;
        case '7':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(1, VO_MODE_1MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_SPEED, 1);
            break;
        case '8':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            //Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            break;
        case '9':
            pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
            filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(4, VO_MODE_4MUX, HD_WIDTH, HD_HEIGHT, pt_types, filenames, 4, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            break;
        case 'a':
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(16, VO_MODE_16MUX, 960, HD_HEIGHT, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            break;
        case 'b':
            pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H264;
            filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_file_h264_5M_name;
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(1, VO_MODE_1MUX, 3840, 2160, pt_types, filenames, fb_cnt, 1, SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            break;
        case 'l':
            for(i=12;i<16;i++)
            {
                if(i%2==0)
                {
                    pt_types[i] = PT_JPEG;
                    filenames[i] = stream_file_jpg;
                }
                else
                {
                    pt_types[i] = PT_MJPEG;
                    filenames[i] = stream_file_mjpg;
                }
            }
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            result = Sample_Playback_Start_Mux(16,VO_MODE_16MUX,960,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            sample_encode_set_frame_rate(15);
            break;
        case 's':
            Sample_Playback_Stop();
            break;
        default :
            break;
        }
        if(result == 1) {
            if(option == '1') {
                printf("Playback already started!\n");
            } else {
                printf("Playback is not started!\n");
            }
        } else if(result != 0) {
            printf("Failed!\n");
        }
    }

    return result;

}

static int show_uimenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: UI Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "stop");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_uitest()
{
    int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        items = show_uimenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        //Selects the course of action specified by the option

        switch(option) {
        case '1':
            if(!ui_getState()) {
                ui_start();
            } else {
                printf("UI stil running! Please stop first!\n");
            }
            break;
        case '2':
            if(ui_getState()) {
                ui_stop();
            } else {
                printf("UI already stoped!\n");
            }
            break;
        default :
            break;
        }
    }
    return result;

}


static int show_audiomenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: AUDIO Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start record file");
    MENU_ITEM_INDEX(++index, "start play file");
    MENU_ITEM_INDEX(++index, "start music");
    //MENU_ITEM_INDEX(++index, "start ai to aenc");
    //MENU_ITEM_INDEX(++index, "start adec to ao");
    MENU_ITEM_INDEX(++index, "stop and clr status");
    MENU_ITEM_INDEX(++index, "start play file to hdmi");
    MENU_ITEM_INDEX(++index, "start play music to hdmi");
    MENU_ITEM_INDEX(++index, "VQE Set");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}

FY_VOID sample_aio_init(FY_S32 type);
FY_S32 sample_aio_test(FY_U32 cmd);
static int menu_audiotest()
{
    int items = 0;
    int result = 0;
    char option = 0;
    //char ch1;

    printf("Welcome to Audio Test,please enjoy it!\n");
    sample_aio_init(0);
    while(1) {
        items = show_audiomenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        //Selects the course of action specified by the option
        sample_aio_test(option - '0');
    }
    return result;

}

static int show_mdmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: MD Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "md test one channel ");
    MENU_ITEM_INDEX(++index, "md test more[one~eight] channel");
    MENU_ITEM_INDEX(++index, "stop all md test");
    MENU_ITEM_INDEX(++index, "open /close log");
    MENU_ITEM_INDEX(++index, "change print type");
	MENU_ITEM_INDEX(++index, "cd test one channel");
    MENU_ITEM_INDEX(++index, "cd test eight channel");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}

FY_S32 sample_md_start(FY_U32 cmd);
static int menu_motion_detection()
{
    int items = 0;
    int result = 0;
    char option = 0;

    printf("\033[31mWelcome to MD Test,please select it!\033[0m\n");
    while(1) {
        items = show_mdmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        sample_md_start(option - '0');
        //Selects the course of action specified by the option
    }
    return result;

}

static int show_wbcmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: WBC Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "stop");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_wbctest()
{
    int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        items = show_wbcmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        //Selects the course of action specified by the option

        switch(option) {
        case '1':
            sample_vo_start_wbc(FY_TRUE);
            break;
        case '2':
            sample_vo_stop_wbc(FY_TRUE);
            break;
        default :
            break;
        }
    }
    return result;

}

#ifdef MC6650
static int show_nnamenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: AI detect" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "pedestrian detect start");
    MENU_ITEM_INDEX(++index, "pedestrian&vehicle detect start");
    MENU_ITEM_INDEX(++index, "face detect start");
    MENU_ITEM_INDEX(++index, "detect stop");
    MENU_ITEM_INDEX(++index, "chn num");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;

}
static int menu_nna_test()
{
    int items = 0;
    int result = 0;
    char option = 0;
    static char ch_num = 1, num;

    while(1) {
        items = show_nnamenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q') {
            break;
        }

        if(option < '0' || option > ('1' + items)) {
            continue;
        }

        //Selects the course of action specified by the option
        switch (option)
        {
            case '1':
                //start encode
                result = sample_nna_test_start(ch_num, FN_DET_TYPE_PERSON);
                break;
            case '2':
                //stop encode
                result = sample_nna_test_start(ch_num, FN_DET_TYPE_C2);
                break;

            case '3':
                //start encode
                result = sample_nna_test_start(ch_num, FN_DET_TYPE_FACE);
                break;
            case '4':
                //stop encode
                result = sample_nna_test_stop();
                break;
            case '5':
                printf("\tinput channel number[1~%d]:", 16);
                scanf("%d", (int*)&num);
                getchar();
                result = sample_nna_set_chn_num(ch_num);
                ch_num = num;
                break;
            default :
                break;
        }
    }
    return result;

}
#endif

static int show_mainmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  DVR SAMPLE: main menu " ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    //MENU_ITEM_INDEX(++index, "VI config");
    MENU_ITEM_INDEX(++index, "Preview");
    MENU_ITEM_INDEX(++index, "Video Encoding");
    MENU_ITEM_INDEX(++index, "Jpeg Encoding");
    MENU_ITEM_INDEX(++index, "Playback");
    MENU_ITEM_INDEX(++index, "UI");
    MENU_ITEM_INDEX(++index, "Audio");

#ifdef MC6650
	MENU_ITEM_INDEX(++index, "ai detect");
#endif

    MENU_ITEM_CHAR('q', "Quit");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);

    return index;
}
int dvr_menu_main()
{
    //int items = 0;
    int result = 0;
    char option = 0;

    while(1) {
        show_mainmenu();
        //handle menu command
#if 1
        option = getchar();
        flush_stdin();
#else
        option = getchar();
        if(10 == option) {
            continue;
        }
        ch1 = getchar();
        printf("first=%c,second=%c\n", option, ch1);
        if(10 != ch1) {
            flush_stdin();
        }
        printf("option %c\n", option);
#endif
        switch(option) {
        //case '1':
        //    menu_viuconfig();
        //    break;
        case '1':
            menu_preview();
            break;
        case '2':
            menu_encoding();
            break;
        case '3':
            menu_jpege();
            break;
        case '4':
            menu_playback();
            break;
        case '5':
            menu_uitest();
            break;
        case '6':
            menu_audiotest();
            break;
#ifdef MC6650
        case '7':
            menu_nna_test();
            break;
#endif
        case 'a':
            menu_motion_detection();
            break;
        case 'b':
            menu_wbctest();
            break;
        case 'c':
            menu_viuconfig();
            break;
        case 'q':
            sample_encode_test_stop();
            sample_jpege_test_stop();
#ifdef MC6650
            sample_nna_test_stop();
#endif
            return result;
        default :
            break;
        }
    }
    //Selects the course of action specified by the option

    return result;

}
