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
	return false;
	return m_userloginName.length() > 0;
}

bool THciBLLProcess::login(const char *username, const char *password)
{
	//check user,password

	m_userloginName = username;
	return true;
}

bool THciBLLProcess::init()
{
	return true;
}
