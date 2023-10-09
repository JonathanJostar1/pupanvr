/*
 * TViewLogin.h
 *
 * 用户登录界面
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_

#include "TViewObject.h"

class TViewLogin: public TViewObject
{
public:
	TViewLogin(ViewHandle parentHandle);
	virtual ~TViewLogin();
private:
	bool viewInit();
	void		event_process(lv_event_t * event);
	static void event_loginBtnOK_handler(lv_event_t* e);
private:
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_ */
