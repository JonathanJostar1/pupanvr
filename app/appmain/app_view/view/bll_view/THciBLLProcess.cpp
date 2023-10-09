/*
 * THciBLLProcess.cpp
 *
 *  Created on: 2022年2月26日
 *      Author: jhting
 */

#include "THciBLLProcess.h"

THciBLLProcess*	THciBLLProcess::m_instance = NULL;

THciBLLProcess* THciBLLProcess::getInstance()
{
	if(!m_instance)
	{
		m_instance = new THciBLLProcess();
	}

	return m_instance;
}

THciBLLProcess::THciBLLProcess()
{
	m_userloginName = "";

	init();
}

THciBLLProcess::~THciBLLProcess()
{

}

bool THciBLLProcess::getLoginStatus()
{
	return m_login_status;
}

bool THciBLLProcess::login(const char *username, const char *password)
{
	//check user,password

	m_userloginName = username;

	m_login_status = true;
	return true;
}

void THciBLLProcess::loginout()
{
	m_login_status = false;	
}

bool THciBLLProcess::init()
{
	m_login_status = false;
	return true;
}
