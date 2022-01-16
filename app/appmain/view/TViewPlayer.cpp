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
	m_viewHandle = lv_obj_create(parentHandle);

	static lv_style_t style;
	lv_style_init(&style);

	lv_style_set_border_width(&style, 1);
	lv_style_set_radius(&style, 0);
	lv_style_set_bg_color(&style, lv_palette_darken(LV_PALETTE_GREY, 1));

	lv_obj_add_style(m_viewHandle, &style, 0);

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
