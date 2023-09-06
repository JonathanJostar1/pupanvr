#ifndef __XVR_SAMPLE_ZOMM_H__
#define __XVR_SAMPLE_ZOOM_H__

#include "fy_type.h"


#define VDEC_CHN_NUM                8
#define STREAM_HD_WIDTH             1920
#define STREAM_HD_HEIGHT            1088
#define STREAM_D1_WIDTH             720
#define STREAM_D1_HEIGHT            576
#define STREAM_400W_WIDTH           2560
#define STREAM_400W_HEIGHT          1440
#define STREAM_800W_WIDTH           3840
#define STREAM_800W_HEIGHT          2160


#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RESET  "\x1b[0m"


#define MENU_ITEM_INDEX(idx, content) \
    printf(ANSI_COLOR_GREEN "[%2d]" ANSI_COLOR_RESET "    %s\n", idx, content)
#define MENU_ITEM_CHAR(ch, content) \
        printf(ANSI_COLOR_GREEN "[%2c]" ANSI_COLOR_RESET "    %s\n", ch, content)
#define MENU_ITEM_CHAR_NUM(ch, content, num) \
        printf(ANSI_COLOR_GREEN "[%2c]" ANSI_COLOR_RESET "    %s:%d\n", ch, content, num)


int Sample_ZOOM_Start(FY_S32 s32ChnNum);

#endif//__XVR_SAMPLE_ZOOM_H__

