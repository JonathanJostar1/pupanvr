/*
 * TViewLogin.cpp
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */


#include "TViewLogin.h"
#include "TViewMainWindow.h"
#include "tlog.h"

TViewLogin* TViewLogin::m_instance = NULL;

TViewLogin::TViewLogin()
{

	viewInit();
}

TViewLogin::~TViewLogin()
{

}

bool TViewLogin::viewInit()
{

	return true;
}

TViewLogin* TViewLogin::getInstance()
{
	if(!m_instance)
	{
		m_instance = new TViewLogin();
	}

	return m_instance;
}

void TViewLogin::show()
{

}

void TViewLogin::hide()
{

}
