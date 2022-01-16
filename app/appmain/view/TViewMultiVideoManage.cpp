/*
 * TViewMultiVideoManage.cpp
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#include<math.h>
#include<stdio.h>

#include "TViewMultiVideoManage.h"
#include "TViewPlayer.h"

TViewMultiVideoManage::TViewMultiVideoManage(ViewHandle parentHandle)
{
	static lv_style_t style;
	lv_style_init(&style);

	lv_style_set_border_width(&style, 0);
	lv_style_set_radius(&style, 0);
	lv_style_set_pad_top(&style, 0);
	lv_style_set_pad_left(&style, 0);
	lv_style_set_pad_bottom(&style, 0);
	lv_style_set_pad_right(&style, 0);
	lv_style_set_bg_color(&style, lv_palette_darken(LV_PALETTE_RED, 1));


	m_viewHandle = lv_obj_create(parentHandle);
	m_viewSpliteMode = View_Splite_Mode_UNKNOW;
	/*根据产品路数，先初始化所有的通道的播放器*/
	_setViewSpliteModeInit(View_Splite_mode_16);

	lv_obj_set_scrollbar_mode(m_viewHandle, LV_SCROLLBAR_MODE_OFF);
	lv_obj_add_style(m_viewHandle, &style, 0);
}

TViewMultiVideoManage::~TViewMultiVideoManage()
{
	TViewPlayer* viewPlayer = NULL;
	for(unsigned int i = 0; i < m_viewPlayerMap.size(); i++)
	{
		viewPlayer = m_viewPlayerMap[i];
		delete viewPlayer;
	}
}

bool TViewMultiVideoManage::setViewSpliteMode(ViewVideoPlayerSplitMode viewSpliteMode)
{
	return _setViewShowMode(viewSpliteMode);
}

ViewVideoPlayerSplitMode TViewMultiVideoManage::getViewSpliteMode()
{
	return m_viewSpliteMode;
}

bool TViewMultiVideoManage::_setViewSpliteModeInit(
		ViewVideoPlayerSplitMode viewSpliteMode)
{
	int i = 0;

	TViewPlayer* viewPlayer = NULL;
	for(i = 0; i < viewSpliteMode; i++)
	{
		viewPlayer = new TViewPlayer(m_viewHandle);
		viewPlayer->viewHide();
		viewPlayer->setChannelValue(i + 1);
		m_viewPlayerMap[i] = viewPlayer;
	}

	return true;
}

bool TViewMultiVideoManage::_setViewShowMode(
		ViewVideoPlayerSplitMode viewSpliteMode)
{
	int i = 0;
	int ChannelBeginIndex = 0;
	int ChannelEndIndex = 0;

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	int itemCount = (int)viewSpliteMode;
	int cellItemCount = sqrt(itemCount);
	int rowItemCount = cellItemCount;

	/*能开方的布局计算*/
	width = getViewWidth() / cellItemCount;
	height = getViewHeight() / rowItemCount;

	ChannelBeginIndex = 0;
	ChannelEndIndex = itemCount - 1;

	m_viewSpliteMode = viewSpliteMode;

	TViewPlayer* viewPlayer = NULL;
	for(i = 0; i < itemCount; i++)
	{
		viewPlayer = m_viewPlayerMap[i];

		if(i < ChannelBeginIndex)
		{
			viewPlayer->viewHide();
			continue;
		}

		if(i > ChannelEndIndex)
		{
			viewPlayer->viewHide();
			continue;
		}

		x = i % cellItemCount * width;
		y = i / cellItemCount * height;
		printf("chn:%d viewPlayer x:%d y:%d width:%d height:%d\n", i + 1, x, y, width, height);
		viewPlayer->setpos(x, y);
		viewPlayer->setViewSize(width, height);
		viewPlayer->viewShow();
	}

	return true;
}
