/*
 * TViewLogin.h
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_

class TViewLogin
{
public:
	static TViewLogin* getInstance();
	TViewLogin();
	virtual ~TViewLogin();
	void show();
	void hide();
private:
	bool viewInit();

private:

	static TViewLogin* m_instance;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWLOGIN_H_ */
