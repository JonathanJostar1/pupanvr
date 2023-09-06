#ifndef __SVR_SAMPLE_VI_H__
#define __SVR_SAMPLE_VI_H__

struct vi_channels_info {
    int ad_chips;
    int channels;
    int channels_by_mux;
};



/**
 test cases
	 0: 1080P30, 2: 1080P30, 4: 1080P30, 6: 1080P30
	 0: 1080P30, 2: 720P30,  4: 1080P30, 6: CVBSP30
	 1: 1080P30, 3: 720P30,  5: 720P30,  6: CVBSP30
	 1: 1080P30, 3: CVBSP30, 5: 720P30,  6: CVBSP30
*/
int sample_vi_start(int test_case, int fps);
int sample_vi_stop();
int sample_vi_dumpframe();
int sample_vi_reset();
int sample_vi_testmode(int testmode);
int sample_vi_query();
int sample_vi_set_chn_ns(int ns);
int sample_vi_init(FY_BOOL bFY02, VI_EC_LEVEL_E ec, PIXEL_FORMAT_E pixformat);
int sample_vi_exit();
void sample_vi_set_5M(int bUse5M);
int sample_vi_camera_cmd(int cmd);
int sample_vi_set_buff(int max_size);
int sample_vi_init_channels(FY_BOOL bFY02, struct vi_channels_info *pstViChn);



#endif //ifndef __SVR_SAMPLE_VI_H__
