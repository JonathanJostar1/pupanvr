/*
 * TViewLogin.h
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
private:
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_ */
