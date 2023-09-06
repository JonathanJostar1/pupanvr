#ifndef __SAMPLE_COMM_PLAYBACK_H__
#define __SAMPLE_COMM_PLAYBACK_H__

#include "fy_common.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum PLAYBACK_CMD_CODE_E
{
	PLAYBACK_CMD_CODE_SPEED = 0, /* cmd_parm: frame rate,*/
	PLAYBACK_CMD_CODE_PAUSE_RESUME,  /* cmd_parm: 0-pause, 1-resume */
	PLAYBACK_CMD_CODE_USERPIC,  /* cmd_parm: 0-hide, 1-show */
	PLAYBACK_CMD_CODE_AJUST_CHANNEL, /* cmd_parm: -1: decrase, 1: increase */
	PLAYBACK_CMD_CODE_AUTO_CHANNELS, /* cmd_parm: 0: stop, 1: start */
	PLAYBACK_CMD_CODE_AUTO_CHANNELS_ONOFF, /* cmd_parm: 0: stop, 1: start */
	PLAYBACK_CMD_CODE_ONOFF_CHANNEL,
	PLAYBACK_CMD_CODE_AUTO_SPEED, /* cmd_parm: 0: stop, 1: start */
}PLAYBACK_CMD_CODE;



int	Sample_Playback_Start(FY_U32	s32ChnNum, PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer);
int	Sample_Playback_Start_Mux_StartVoChn_ext(FY_U32	s32ChnNum, int enMode,  int video_width,int video_height, PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer,VO_CHN startVoChn, FY_BOOL bShow);
int	Sample_Playback_Start_Mux_StartVoChn(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height,PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer,VO_CHN startVoChn);
int	Sample_Playback_Start_Mux(FY_U32	s32ChnNum, int enMode, int video_width, int video_height,PAYLOAD_TYPE_E pt_types[],char* filenames[], int fb_cnt, int bind,VO_LAYER VoLayer);
int	Sample_Playback_Start_Mux_Fav(FY_U32	s32ChnNum, int enMode,  int video_width, int video_height,PAYLOAD_TYPE_E pt_types,char* filenames, int fb_cnt, int bind,VO_LAYER VoLayer,int aoDev,int audio_idx,int video_idx);

int	Sample_Playback_Stop(void);
int	Sample_Playback_Ctrl(PLAYBACK_CMD_CODE cmd_code, FY_S32 cmd_parm);
int	Sample_Playback_query(FY_U32 decode_frames[]);
int	Sample_Playback_queryVgs(FY_U32 resize_frames[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__SAMPLE_COMM_PLAYBACK_H__
