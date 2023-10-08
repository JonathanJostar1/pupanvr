/*
 * TViewMainWindow.h
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWMAINWINDOW_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWMAINWINDOW_H_

#include "view_def.h"
#include "TViewObject.h"
#include "TViewMultiVideoManage.h"
#include "TViewLogin.h"
#include "TViewSysSetFrame.h"
#include "TViewPlayToolBar.h"
#include "TViewConfigMainWindow.h"

/*
 * 主界面管理类
 * */
class TViewMainWindow: public TViewObject
{
public:
	static TViewMainWindow* getInstance();
	bool 	init();
	TViewMultiVideoManage* 	getViewMultiVideoManage();
	TViewLogin*				getViewLogin();
	TViewPlayToolBar*		getViewPlayToolBar();
	void					showMainSysMenuConfig(bool flag);
	void					playViewClickProcess(TViewPlayer* obj, lv_event_t * event);
private:
	TViewMainWindow(ViewHandle parentHandle);
	virtual ~TViewMainWindow();

	void		event_process(lv_event_t * event);
private:
	static TViewMainWindow* 	m_instance;
	ViewHandle					m_viewLabelHnd;
private:
	TViewMultiVideoManage*		m_viewMultiVideoManage; 
	TViewLogin*					m_viewLogin;
	TViewSysSetFrame*			m_viewSysSetFrame;
	TViewPlayToolBar*			m_viewPlayToolBar;
	TViewConfigMainWindow*		m_viewConfigMainWindow;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMAINWINDOW_H_ */
