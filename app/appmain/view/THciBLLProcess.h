/*
 * THciBLLProcess.h
 *用户界面业务处理类
 *  Created on: 2022年2月26日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_THCIBLLPROCESS_H_
#define APPMAIN_APPMAIN_VIEW_THCIBLLPROCESS_H_

#include<string>

using namespace std;

class THciBLLProcess
{
public:
	static 	THciBLLProcess* getInstance();
	bool	getLoginStatus();
	bool	login(const char* username, const char* password);
private:
	THciBLLProcess();
	virtual ~THciBLLProcess();
	bool	init();
private:
	static THciBLLProcess*	m_instance;
private:
	string	m_userloginName;
};

#endif /* APPMAIN_APPMAIN_VIEW_THCIBLLPROCESS_H_ */
