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

class TViewMainWindow: public TViewObject
{
public:
	static TViewMainWindow* getInstance();
	bool 	init();

private:
	TViewMainWindow();
	virtual ~TViewMainWindow();

private:
	static TViewMainWindow* 	m_instance;
	TViewMultiVideoManage*		m_viewMultiVideoManage;
	ViewHandle					m_viewLabelHnd;

};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMAINWINDOW_H_ */
