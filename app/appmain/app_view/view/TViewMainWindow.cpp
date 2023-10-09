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
#include "THciBLLProcess.h"


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
	lv_obj_clear_flag(m_viewHandle, LV_OBJ_FLAG_SCROLLABLE);

	m_viewLabelHnd = lv_label_create(m_viewHandle);
	lv_label_set_text(m_viewLabelHnd, "TViewMainWindow");
	lv_obj_center(m_viewLabelHnd);

	/*创建视频分割管理器*/
	m_viewMultiVideoManage = new TViewMultiVideoManage(m_viewHandle);

	m_viewMultiVideoManage->setViewSize(lv_obj_get_width(m_viewHandle), lv_obj_get_height(m_viewHandle));


	printf("m_viewMultiVideoManage setViewSize %d %d\n", m_viewMultiVideoManage->getViewWidth(), m_viewMultiVideoManage->getViewHeight());
	lv_obj_center(m_viewMultiVideoManage->getViewHandle());

	/*初始化分割模式*/
	m_viewMultiVideoManage->setViewSpliteMode(View_Splite_mode_4);

	m_viewMultiVideoManage->viewShow();

	/*创建登录界面*/
	m_viewLogin = new TViewLogin(m_viewHandle);
	m_viewLogin->viewHide();

	/*创建配置菜单界面*/
	m_viewSysSetFrame = NULL;
	m_viewConfigMainWindow = NULL;
#if 1
	m_viewSysSetFrame = new TViewSysSetFrame(m_viewHandle);
	m_viewSysSetFrame->viewHide();
#else
	m_viewConfigMainWindow = new TViewConfigMainWindow(m_viewHandle);
	m_viewConfigMainWindow->setpos(0, 0);
	m_viewConfigMainWindow->viewHide();
#endif
	m_viewPlayToolBar = new TViewPlayToolBar(m_viewHandle);
	m_viewPlayToolBar->viewHide();

	lv_obj_refr_size(m_viewHandle);


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

void TViewMainWindow::event_process(lv_event_t * event)
{
	
}


bool TViewMainWindow::init()
{
	return true;
}

TViewLogin* TViewMainWindow::getViewLogin()
{
	return m_viewLogin;
}

TViewPlayToolBar* TViewMainWindow::getViewPlayToolBar()
{
	return m_viewPlayToolBar;
}


void TViewMainWindow::showMainSysMenuConfig(bool trigger)
{
	if(m_viewSysSetFrame)
	{
		if(trigger)
		{
			if(m_viewSysSetFrame->visiabled())
			{
				m_viewSysSetFrame->viewHide();	
			}else{
				m_viewSysSetFrame->viewShow();	
			}
		}else{
			m_viewSysSetFrame->viewShow();	
		}
	}

	if(m_viewConfigMainWindow)
	{
		if(trigger)
		{
			if(m_viewConfigMainWindow->visiabled())
			{
				m_viewConfigMainWindow->viewHide();	
			}else{
				m_viewConfigMainWindow->viewShow();	
			}
		}else{
			m_viewConfigMainWindow->viewShow();	
		}
	}
}

void TViewMainWindow::playViewClickProcess(TViewPlayer* obj, lv_event_t * event)
{
	if(event->code != LV_EVENT_RELEASED)
	{
		return;
	}

	if(m_viewConfigMainWindow)
	{
		if(m_viewConfigMainWindow->visiabled())
		{
			m_viewConfigMainWindow->viewHide();	
			THciBLLProcess::getInstance()->loginout();
			return;
		}
	}

	if(m_viewSysSetFrame)
	{
		if(m_viewSysSetFrame->visiabled())
		{
			m_viewSysSetFrame->viewHide();
			THciBLLProcess::getInstance()->loginout();
			return;
		}
	}

	//printf("TViewPlayer chn[%d] clicked!!! event->code:%d\n", m_chn, event->code);

	if(!THciBLLProcess::getInstance()->getLoginStatus())
	{
		if(!TViewMainWindow::getInstance()->getViewLogin()->visiabled())
		{
			TViewMainWindow::getInstance()->getViewLogin()->viewShow();
		}else{
			TViewMainWindow::getInstance()->getViewLogin()->viewHide();
		}

		return;
	}
	
	TViewMainWindow::getInstance()->getViewMultiVideoManage()->setCurrentSelectPlayView(obj->getChannelValue());
	obj->showViewPlayToolBar();

}

#include "TViewScreenSnap.h"
void    view_lvgl_snap()
{
	if(!TViewScreenSnap::screenSnap("/nfsroot/hi3536c/snap_test1.png"))
	{
		printf("screen snap failure!\n");
	}else{
		printf("screen snap ok!\n");	
	}
}
