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
#include "TViewFontUtils.h"

TViewPlayer::TViewPlayer(ViewHandle parentHandle): TViewObject(parentHandle)
{
	m_chn = 0;

	lv_style_set_border_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 2));
	lv_style_set_border_width(&m_style, 1);
	lv_style_set_radius(&m_style, 0);
	lv_style_set_bg_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 4));

	lv_style_set_text_color(&m_style, lv_color_hex(0xFFFFFF));

	m_hitlabel = lv_label_create(m_viewHandle);
	lv_label_set_text(m_hitlabel, "TViewPlayer");
	lv_ft_info_t *ftfont = TViewFontUtils::getInstance()->getFont("SourceHanSansCN-Regular.otf", 20, 0);
	lv_obj_set_style_text_font(m_hitlabel, ftfont->font, 0);
	//lv_obj_set_style_text_font(m_hitlabel, &SourceHanSerifSC_Regular_20, 0);
	lv_obj_center(m_hitlabel);
}

TViewPlayer::~TViewPlayer()
{

}

void TViewPlayer::setChannelValue(int chn)
{
	m_chn = chn;
	char bufferTmp[32];
	snprintf(bufferTmp, sizeof(bufferTmp), "通道%d", m_chn);
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
#if 1
		if(TViewMainWindow::getInstance()->getViewLogin()->visiabled())
		{
			TViewMainWindow::getInstance()->getViewLogin()->viewHide();
		}else{
			TViewMainWindow::getInstance()->getViewLogin()->viewShow();
		}
#endif
	}

}

void TViewPlayer::setViewPlayerActive(bool flag)
{
	if(flag)
	{
		lv_style_set_border_color(&m_style, lv_color_hex(0xFFFFFF));
	}else{
		lv_style_set_border_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 2));
	}

	lv_obj_invalidate(m_viewHandle);
}
