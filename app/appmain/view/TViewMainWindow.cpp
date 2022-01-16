/*
 * TViewMainWindow.cpp
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */


#include<stdio.h>


#include "view_lvgl.h"

#include "TViewMainWindow.h"
#include "tlog.h"
#include "TViewLogin.h"

#include "TViewPlayer.h"



TViewMainWindow* 	TViewMainWindow::m_instance = NULL;


TViewMainWindow* TViewMainWindow::getInstance()
{
	if(!m_instance)
	{
		m_instance = new TViewMainWindow();
	}

	return m_instance;
}

TViewMainWindow::TViewMainWindow()
{
	static lv_style_t style;
	lv_style_init(&style);

	lv_style_set_border_width(&style, 0);
	lv_style_set_radius(&style, 0);
	lv_style_set_bg_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 1));


	ViewHandle screen = lv_scr_act();
	m_viewHandle = lv_obj_create(screen);

	lv_obj_set_size(m_viewHandle, lv_obj_get_width(screen), lv_obj_get_height(screen));
	lv_obj_refr_size(m_viewHandle);

	lv_obj_add_style(m_viewHandle, &style, 0);

	lv_obj_set_scrollbar_mode(m_viewHandle, LV_SCROLLBAR_MODE_OFF);

	m_viewLabelHnd = lv_label_create(m_viewHandle);
	lv_label_set_text(m_viewLabelHnd, "TViewMainWindow");
	lv_obj_center(m_viewLabelHnd);

	/*创建视频分割管理器*/
	m_viewMultiVideoManage = new TViewMultiVideoManage(m_viewHandle);

	m_viewMultiVideoManage->setViewSize(lv_obj_get_width(m_viewHandle), lv_obj_get_height(m_viewHandle));

	printf("m_viewMultiVideoManage setViewSize %d %d\n", m_viewMultiVideoManage->getViewWidth(), m_viewMultiVideoManage->getViewHeight());
	lv_obj_center(m_viewMultiVideoManage->getViewHandle());

	/*初始化分割模式*/
	m_viewMultiVideoManage->setViewSpliteMode(View_Splite_mode_9);
	//m_viewMultiVideoManage->viewHide();
}


TViewMainWindow::~TViewMainWindow()
{
	if(m_viewMultiVideoManage)
	{
		delete m_viewMultiVideoManage;
		m_viewMultiVideoManage = NULL;
	}
}


bool TViewMainWindow::init()
{
	return true;
}



