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

class TViewMainWindow: public TViewObject
{
public:
	static TViewMainWindow* getInstance();
	bool 	init();
	TViewMultiVideoManage* 	getViewMultiVideoManage();
	TViewLogin*				getViewLogin();
	
	void					showMainSysMenuConfig();
private:
	TViewMainWindow(ViewHandle parentHandle);
	virtual ~TViewMainWindow();

private:
	static TViewMainWindow* 	m_instance;
	ViewHandle					m_viewLabelHnd;
private:
	TViewMultiVideoManage*		m_viewMultiVideoManage;
	TViewLogin*					m_viewLogin;
	TViewSysSetFrame*			m_viewSysSetFrame;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMAINWINDOW_H_ */
