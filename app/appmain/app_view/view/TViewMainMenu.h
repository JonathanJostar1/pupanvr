/*
 * TViewMainMenu.h
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWMAINMENU_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWMAINMENU_H_

#include "TViewObject.h"

class TViewMainMenu:public TViewObject
{
public:
	TViewMainMenu(ViewHandle parentHandle);
	virtual ~TViewMainMenu();
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMAINMENU_H_ */
