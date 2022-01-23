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
		m_instance = new TViewMainWindow(lv_scr_act());
	}

	return m_instance;
}

TViewMainWindow::TViewMainWindow(ViewHandle parentHandle): TViewObject(parentHandle)
{
	lv_style_set_border_width(&m_style, 0);
	lv_style_set_radius(&m_style, 0);
	lv_style_set_bg_color(&m_style, lv_palette_darken(LV_PALETTE_BLUE, 1));

	lv_obj_set_size(m_viewHandle, lv_obj_get_width(parentHandle), lv_obj_get_height(parentHandle));
	lv_obj_refr_size(m_viewHandle);

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

	m_viewMultiVideoManage->viewShow();

	m_viewLogin = new TViewLogin(m_viewHandle);
	//m_viewLogin->viewHide();
}

TViewMultiVideoManage* TViewMainWindow::getViewMultiVideoManage()
{
	return m_viewMultiVideoManage;
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

TViewLogin* TViewMainWindow::getViewLogin()
{
	return m_viewLogin;
}
