/*
 * TViewPlayer.cpp
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include "TViewPlayer.h"

#include "view_lvgl.h"
#include "TViewMainWindow.h"

TViewPlayer::TViewPlayer(ViewHandle parentHandle)
{
	m_chn = 0;

	lv_style_set_border_color(&m_style, lv_palette_lighten(LV_PALETTE_GREY, 3));
	lv_style_set_border_width(&m_style, 1);
	lv_style_set_radius(&m_style, 0);
	lv_style_set_bg_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 1));


	m_hitlabel = lv_label_create(m_viewHandle);
	lv_label_set_text(m_hitlabel, "TViewPlayer");
	lv_obj_center(m_hitlabel);
}

TViewPlayer::~TViewPlayer()
{

}

void TViewPlayer::setChannelValue(int chn)
{
	m_chn = chn;
	char bufferTmp[32];
	snprintf(bufferTmp, sizeof(bufferTmp), "TViewPlayer%d", m_chn);
	lv_label_set_text(m_hitlabel, bufferTmp);
}

int TViewPlayer::getChannelValue()
{
	return m_chn;
}

void TViewPlayer::event_process(lv_event_t *event)
{
	if(event->code == LV_EVENT_CLICKED)
	{
		printf("TViewPlayer chn[%d] clicked!!!\n", m_chn);
		TViewMainWindow::getInstance()->getViewMultiVideoManage()->setCurrentSelectPlayView(m_chn);
	}
}

void TViewPlayer::setViewPlayerActive(bool flag)
{
	if(flag)
	{
		lv_style_set_border_color(&m_style, lv_palette_darken(LV_PALETTE_RED, 1));
	}else{
		lv_style_set_border_color(&m_style, lv_palette_lighten(LV_PALETTE_GREY, 3));
	}

	lv_obj_invalidate(m_viewHandle);
}
