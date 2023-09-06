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
#include "sample_encoding.h"
#include "sample_zoom.h"
#include "sample_rotate.h"


/**
 * sample ansi color code
 *
 */
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RESET  "\x1b[0m"

volatile FY_BOOL bPlayFAV = 0;
volatile FY_BOOL bPlayHDMIAudio = 0;


#define MENU_ITEM_INDEX(idx, content) \
    printf(ANSI_COLOR_GREEN "[%2d]" ANSI_COLOR_RESET "    %s\n", idx, content)

#define MENU_ITEM_CHAR(ch, content) \
    printf(ANSI_COLOR_GREEN "[%2c]" ANSI_COLOR_RESET "    %s\n", ch, content)

static void flush_stdin()
{
	char c = 0;
	while ( (c = getchar()) != '\n' && c != EOF ) ;
}

#if(defined(FY01)	|| defined(FY10) || defined(MC6630) || defined(MC6830) || defined(MC6850))
static int show_encodingmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: H.26x Encoding" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "stop");
    MENU_ITEM_INDEX(++index, "switch");
    MENU_ITEM_INDEX(++index, "pause");
    MENU_ITEM_INDEX(++index, "chn num");
    MENU_ITEM_INDEX(++index, "record path");
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
    char num=0;
    char path[512];
    static char ch_num = 1;
    static FY_BOOL bEnalbeLog=FY_FALSE;

    sample_encode_set_chn_num(ch_num);
    while(1) {
        items = show_encodingmenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q')
            break;
        if((option < '0' || option > ('1' + items)) && option != 'l') {
            continue;
        }

        //Selects the course of action specified by the option
        switch (option)
        {
            case '1':
                //start encode
                if(!sample_encode_is_started() && (FY_SUCCESS != sample_encoding_nvr_init(ch_num)))
                    break;
                result = sample_encode_test_start(FY_FALSE, FY_TRUE);
                break;
            case '2':
                //stop encode
                if(sample_encode_is_started())
                    sample_encoding_nvr_deinit(ch_num); // deinit vpu and unbind vpu with vppu
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
                printf("\tinput channel number[1~2]:");
                scanf("%d", (int*)&num);
                getchar();
                if(num <1 || num > 2)
                {
                    printf("\n\tchn number (%d) is not in range [1,2]\n", num);
                    break;
                }
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
                printf("  input width:  "); scanf("%d", (int*)&w);
                getchar();
                printf("  input height: "); scanf("%d", (int*)&h);
                getchar();
                result = sample_encode_set_resolution(w, h);
                break;
#endif
            default :
                break;
        }
    }
    return result;

}
#endif

static int show_jpegemenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: Jpeg Encoding" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "stop");
    MENU_ITEM_INDEX(++index, "switch");
    MENU_ITEM_INDEX(++index, "pause");
    MENU_ITEM_INDEX(++index, "chn num");
    MENU_ITEM_INDEX(++index, "snap num");
    MENU_ITEM_INDEX(++index, "record path");
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
    char num=0;
    char path[512];
    static char ch_num = 1;
    bool bStarted = false;
    static FY_BOOL bEnalbeLog=FY_FALSE;

    sample_jpege_set_chn_num(ch_num);

    while(1) {
        items = show_jpegemenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q')
            break;
        if((option < '0' || option > ('1' + items)) && option != 'l') {
            continue;
        }

        //Selects the course of action specified by the option
        switch (option)
        {
            case '1':
                //start encode
                if(!sample_jpege_is_started() && (FY_SUCCESS != sample_jpege_nvr_init(ch_num)))
                    break;
                result = sample_jpege_test_start(FY_TRUE);
                break;
            case '2':
                //stop encode
                bStarted = sample_jpege_is_started();
                result = sample_jpege_test_stop();
                if(bStarted)
                    sample_jpege_nvr_deinit(ch_num); // deinit vpu and unbind vpu with vppu
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
                printf("\tinput channel number[1~4]:");
                scanf("%d", (int*)&num);
                getchar();
                if(num <1 || num > 4)
                {
                    printf("\n\tchn number (%d) is not in range [1,4]\n", num);
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
                break;
            case '7':
                printf("\tinput record path:");
                scanf("%s", path);
                getchar();
                printf("\n\tget record path: %s\n", path);
                result = sample_jpege_set_storage_path(path);
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
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: Playback" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "HDMI 1024*768/60HZ playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1280*720/60HZ playback test");
	MENU_ITEM_INDEX(++index, "HDMI 1280*1024/60HZ playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback test");
    MENU_ITEM_INDEX(++index, "CVBS playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback 1 channel 4x");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ playback 1 channel 1x / 4x change");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ 2x2 1920x1080p@15fps playback test");
    MENU_ITEM_INDEX(++index, "HDMI 1920*1080/60HZ 2x2 1920x1080p@30fps playback test(performance)");
    MENU_ITEM_CHAR('a', "HDMI 1920*1080/60HZ 4x4 16 channels playback test");
    MENU_ITEM_CHAR('b', "HDMI 1920*1080/60HZ 8M playback test");
	MENU_ITEM_CHAR('c', "HDMI 1600*1200/60HZ playback test");
	MENU_ITEM_CHAR('d', "HDMI 2560*1440/60HZ playback test");
	MENU_ITEM_CHAR('e', "HDMI 3840*2160/30HZ playback test");
    MENU_ITEM_CHAR('f', "HDMI 4096*2160/30HZ playback test");
    MENU_ITEM_CHAR('g', "HDMI 3840*2160/30HZ 8x1920x1080p@25fps playback test");
#if(VDEC_MAX_CHN_NUM>=64)
    MENU_ITEM_CHAR('h', "HDMI 3840*2160/30HZ 64xD1@15fps playback test");
    MENU_ITEM_CHAR('i', "HDMI 3840*2160/30HZ (60xVideo +  4xjpeg/mjpg)D1@15fps playback test");
#endif
    MENU_ITEM_CHAR('k', "HDMI 1920*1080/60HZ 1920x1080@30fs Video(with pts) + Audio playback test");
    MENU_ITEM_CHAR('l', "HDMI 1920*1080/60HZ 4x4 (12xVideo + 4xD1 jpeg/mjpg) channels playback test");
    MENU_ITEM_CHAR('s', "stop playback");

    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, a-f, s, q]:", index);
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
#if(VDEC_MAX_CHN_NUM>=64)
	char* stream_file_h264d1_name = "/nfs/stream/normal/h264/H264_D1_15fps_300bps.h264";
#endif
    //PAYLOAD_TYPE_E pt_type  = PT_H264;

    char *stream_performance_file_name = stream_1080p30_file_name;
	char* stream_file_fav_name  = "/nfs/stream/normal/h264/pcms16lex2ch_48K_32K_16K_8K_h264.fav";
    char* stream_file_jpg = "/nfs/stream/normal/jpg/flower1.jpg";
    char* stream_file_mjpg = "/nfs/stream/normal/mjpg/TimelapseClouds_720x576.mjp";
    //PAYLOAD_TYPE_E performance_pt_type  = PT_H265;

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
        if(option == 'q')
            break;

		//Selects the course of action specified by the option
		result = 0;
		for(i=0;i<VDEC_MAX_CHN_NUM;i++)
		{
			if((i%4) < 2)
			{
				pt_types[i] = PT_H265;
				filenames[i] = stream_file_h265_name;
			}
			else
			{
				pt_types[i] = PT_H264;
				filenames[i] = stream_file_h264_name;
			}
		}
		bPlayFAV = FY_FALSE;
		switch (option)
		{
		case '1':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1024x768_60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
			break;
		case '2':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_720P60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
			break;
		case '3':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1280x1024_60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
			break;
		case '4':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
			break;
		case '5':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_FALSE);
			sample_vo_init(FY_FALSE, VO_OUTPUT_PAL);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VSD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
			break;
		case '6':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(1,VO_MODE_1MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 60);
            sample_encode_set_frame_rate(15);
			break;
		case '7':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(1,VO_MODE_1MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_SPEED, 1);
            sample_encode_set_frame_rate(15);
			break;
		case '8':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
			//Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_AUTO_CHANNELS, 1);
            sample_encode_set_frame_rate(15);
			break;
		case '9':
			pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
			filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            sample_encode_set_frame_rate(30);
			break;
		case 'a':
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(16,VO_MODE_16MUX,960,HD_HEIGHT,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            sample_encode_set_frame_rate(15);
			break;
        case 'b':
            pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H264;
			filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_file_h264_5M_name;
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
			result = Sample_Playback_Start_Mux(1,VO_MODE_1MUX,3840,2160,pt_types,filenames,fb_cnt,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            sample_encode_set_frame_rate(15);
            break;
		case 'c':
			pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
			filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_1600x1200_60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            sample_encode_set_frame_rate(30);
			break;
		case 'd':
			pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
			filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_2560x1440_60);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            sample_encode_set_frame_rate(30);
			break;
		case 'e':
			pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
			filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
			Sample_Playback_Stop();
			sample_vo_deinit(FY_TRUE);
			sample_vo_init(FY_TRUE, VO_OUTPUT_3840x2160_30);
			result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
			Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            sample_encode_set_frame_rate(30);
			break;
        case 'f':
            pt_types[0] = pt_types[1] = pt_types[2] = pt_types[3] = PT_H265;
            filenames[0] = filenames[1] = filenames[2] = filenames[3] = stream_performance_file_name;
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_4096x2160_30);
            result = Sample_Playback_Start_Mux(4,VO_MODE_4MUX,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 30);
            sample_encode_set_frame_rate(30);
            break;
        case 'g':
            for(i=0;i<VDEC_MAX_CHN_NUM;i++)
            {
                pt_types[i] =  PT_H265;
                filenames[i] = stream_performance_file_name;
            }
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_3840x2160_30);
            result = Sample_Playback_Start_Mux(8,VO_MODE_1B_7S,HD_WIDTH,HD_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 25);
            sample_encode_set_frame_rate(25);
            break;
#if(VDEC_MAX_CHN_NUM>=64)
        case 'h':
            for(i=0;i<VDEC_MAX_CHN_NUM;i++)
            {
                pt_types[i] = PT_H264;
                filenames[i] = stream_file_h264d1_name;
            }
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_3840x2160_30);
            result = Sample_Playback_Start_Mux(64,VO_MODE_64MUX,D1_WIDTH,D1_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            sample_encode_set_frame_rate(15);
            break;
        case 'i':
            for(i=0;i<VDEC_MAX_CHN_NUM;i++)
            {
                pt_types[i] = PT_H264;
                filenames[i] = stream_file_h264d1_name;
            }
            for(i=60;i<VDEC_MAX_CHN_NUM;i++)
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
            sample_vo_init(FY_TRUE, VO_OUTPUT_3840x2160_30);
            result = Sample_Playback_Start_Mux(64,VO_MODE_64MUX,D1_WIDTH,D1_HEIGHT,pt_types,filenames,4,1,SAMPLE_VO_LAYER_VHD0);
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 15);
            sample_encode_set_frame_rate(15);
            break;

#endif
        case 'k':
            if(bPlayHDMIAudio)
            {
                printf("Audio is in tesing, A/V test in not suppored!!!\n");
                break;
            }
            Sample_Playback_Stop();
            sample_vo_deinit(FY_TRUE);
            sample_vo_init(FY_TRUE, VO_OUTPUT_1080P60);
            {
                int i,stream_cnt,audio_cnt = 0,audio_select=-1;
                int video_idx = -1,audio_idx = -1;
                struct fav_stream streams[32];
                stream_cnt = SAMPLE_COMM_VDEC_PaserFAV_Header(stream_file_fav_name,streams);
                for(i=0;i<stream_cnt;i++)
                {
                    if(streams[i].stream_type == 'a')
                        audio_cnt++;
                    else
                    {
                        video_idx = i;
                        break;
                    }
                }

                printf("There %d audio: \n",audio_cnt);
                for(i=0;i<audio_cnt;i++)
                {
                    printf("%d: pcm %s %dx%d\n",i,streams[i].codec,streams[i].audio_freq,streams[i].audio_chan);
                }
                printf("Select audio: ");
                scanf("%d",&audio_select);
                if(audio_select<audio_cnt && audio_select>=0)
                {
                    printf("You select audio: pcm %s %dx%d\n",streams[audio_select].codec,streams[audio_select].audio_freq,streams[audio_select].audio_chan);
                    audio_idx = audio_select;
                    bPlayFAV = FY_TRUE;
                }
                flush_stdin();
                result = Sample_Playback_Start_Mux_Fav(1,VO_MODE_1MUX,HD_WIDTH,HD_HEIGHT,PT_H264,stream_file_fav_name,4,1,SAMPLE_VO_LAYER_VHD0,(1<<I2S_DEV3),audio_idx,video_idx);
            }
            Sample_Playback_Ctrl(PLAYBACK_CMD_CODE_SPEED, 60);
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
		if(result == 1)
		{
			if(option == '1')
			    printf("Playback already started!\n");
			else
			    printf("Playback is not started!\n");
		}
		else if(result != 0)
			printf("Failed!\n");
	}

    return result;

}

static int show_uimenu()
{
    int index = 0;

    //show the ui menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: UI Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "start");
    MENU_ITEM_INDEX(++index, "manual");
    MENU_ITEM_INDEX(++index, "auto");
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
        if(option == 'q')
            break;

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        //Selects the course of action specified by the option

        switch (option)
        {
        case '1':
            if(!ui_getState()){
                ui_start();
            }else{
                printf("UI stil running! Please stop first!\n");
            }
            break;
        case '2':
            if(ui_getState()){
                ui_manualSwitch();
            }else{
                printf("UI already stoped!\n");
            }
            break;
        case '3':
            if(ui_getState()){
                ui_autoSwitch();
            }else{
                printf("UI already stoped!\n");
            }
            break;
        case '4':
            if(ui_getState()){
                ui_stop();
            }else{
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
	printf(ANSI_COLOR_BLUE "  NVR SAMPLE: AUDIO Test" ANSI_COLOR_RESET "\n");
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
	sample_aio_init(1);
	while(1) {
	items = show_audiomenu();
	//handle menu command
	option = getchar();
	flush_stdin();
	if(option == 'q')
	    break;

	if(option < '0' || option > ('1' + items)) {
	    continue;
	}
	//Selects the course of action specified by the option
		if(bPlayFAV)
		{
			printf("A/V is in tesing. Audio testing is disabled!!!!\n");
			continue;
		}
		bPlayHDMIAudio = FY_TRUE;
		sample_aio_test(option-'0');

		if(option=='4')
			bPlayHDMIAudio = FY_FALSE;

	}
	return result;

}

static int show_zoommenu()
{
    int index = 0;

    //show the zoom menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: ZOOM Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "playback 1 channel in 1080p");
    MENU_ITEM_INDEX(++index, "playback 4 channel in 1080p");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;
}
static int menu_zoomtest()
{
    int result = 0;
    char option = 0;

    while(1) {
        show_zoommenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q')
            break;

        switch (option)
        {
        case '1':
            Sample_Playback_Stop();
            Sample_ZOOM_Start(1);
            break;
        case '2':
            Sample_Playback_Stop();
            Sample_ZOOM_Start(4);
            break;
        default :
            break;
        }
    }
    return result;
}

#if(defined(MC6810) || defined(MC6810E) || defined(MC3312))
static int show_rotatemenu()
{
    int index = 0;

    //show the zoom menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: ROTATE Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "rotate in 2 channel");
    MENU_ITEM_INDEX(++index, "rotate in 8 channel");
    MENU_ITEM_CHAR('q', "Quit to main menu");
    printf("\n");
    printf("================================================================\n");
    printf("Enter your choice [1 - %d, q]:", index);
    return index;
}
static int menu_rotatetest()
{
    int result = 0;
    char option = 0;

    while(1) {
        show_rotatemenu();
        //handle menu command
        option = getchar();
        flush_stdin();
        if(option == 'q'){
            break;
        }

        switch (option)
        {
        case '1':
            Sample_Playback_Stop();
            Sample_Rotate_Start(2, VO_OUTPUT_1080P60/*VO_OUTPUT_800x1280_60*/);
            break;
        case '2':
            Sample_Playback_Stop();
            Sample_Rotate_Start(8, VO_OUTPUT_1080P60);
            break;
        default :
            break;
        }
    }
    return result;
}
#endif

#if 0
static int show_mdmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: MD Test" ANSI_COLOR_RESET "\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    MENU_ITEM_INDEX(++index, "md test one channel ");
	MENU_ITEM_INDEX(++index, "md test more[one~eight] channel");
	//MENU_ITEM_INDEX(++index, "cd test one channel");
	//MENU_ITEM_INDEX(++index, "cd test eight channel");
	MENU_ITEM_INDEX(++index, "stop all md test");
	MENU_ITEM_INDEX(++index, "open /close log");
	MENU_ITEM_INDEX(++index, "change print type");
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
		if(option == 'q')
			break;

		if(option < '0' || option > ('1' + items)) {
			continue;
		}
		sample_md_start(option-'0');
		//Selects the course of action specified by the option
	}
	return result;

}

#endif

#if 0
static int show_wbcmenu()
{
    int index = 0;

    //show the main menu
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf(ANSI_COLOR_BLUE "  NVR SAMPLE: WBC Test" ANSI_COLOR_RESET "\n");
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
        if(option == 'q')
            break;

        if(option < '0' || option > ('1' + items)) {
            continue;
        }
        //Selects the course of action specified by the option

        switch (option)
        {
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
#endif

static int show_mainmenu()
{
	int index = 0;

	//show the main menu
	printf("\n");
	printf("----------------------------------------------------------------\n");
	printf(ANSI_COLOR_BLUE "  NVR SAMPLE: main menu " ANSI_COLOR_RESET "\n");
	printf("----------------------------------------------------------------\n");
	printf("\n");
	MENU_ITEM_INDEX(++index, "Playback");
#if(defined(FY01)	|| defined(FY10) || defined(MC6630) || defined(MC6830) || defined(MC6850))
	MENU_ITEM_INDEX(++index, "H.26x Encoding");
#else
	MENU_ITEM_INDEX(++index, "N/A");
#endif
	MENU_ITEM_INDEX(++index, "Jpeg Encoding");
	MENU_ITEM_INDEX(++index, "UI");
	MENU_ITEM_INDEX(++index, "Audio");
	//MENU_ITEM_INDEX(++index, "MD test");
	//MENU_ITEM_INDEX(++index, "wbc test");
	MENU_ITEM_INDEX(++index, "Zoom");
#if(defined(MC6810) || defined(MC6810E) || defined(MC3312))
        MENU_ITEM_INDEX(++index, "Rotate");
#else
        MENU_ITEM_INDEX(++index, "N/A");
#endif
	MENU_ITEM_CHAR('q', "Quit");
	printf("\n");
	printf("================================================================\n");
	printf("Enter your choice [1 - %d, q]:", index);

	return index;
}
int nvr_menu_main()
{
	//int items = 0;
	int result = 0;
	char option = 0;
	unsigned int ch_num;

	while(1) {
		show_mainmenu();

		//handle menu command
		option = getchar();
		flush_stdin();
		switch (option)
		{
			case '1':
			    menu_playback();
			    break;
			case '2':
#if(defined(FY01)	|| defined(FY10) || defined(MC6630) || defined(MC6830) || defined(MC6850))
			    menu_encoding();
#endif
			    break;
			case '3':
			    menu_jpege();
			    break;
			case '4':
			    menu_uitest();
			    break;
			case '5':
			    menu_audiotest();
			    break;
			case '6':
			    ui_stop();
			    menu_zoomtest();
			    break;
			case '7':
#if(defined(MC6810) || defined(MC6810E) || defined(MC3312))
			    ui_stop();
			    menu_rotatetest();
#endif
			    break;
			case 'q':
			    if(sample_encode_is_started())
			    {
			        sample_encode_get_chn_num(&ch_num);
			        sample_encoding_nvr_deinit(ch_num); // deinit vpu and unbind vpu with vppu
			    }
			    sample_encode_test_stop();
			    sample_jpege_test_stop();
			    ui_stop();
			    return result;
			default :
			    break;
		}
	}
	//Selects the course of action specified by the option

	return result;

}
