/*
 * TServiceBase.cpp
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#include "TServiceBase.h"

#include<string.h>
#include<stdio.h>

#include "TSysServicesManage.h"

TServiceBase::TServiceBase()
{


}

TServiceBase::~TServiceBase()
{

}

void TServiceBase::setServiceName(const char *name)
{
	setThreadName(name);
}

const char* TServiceBase::getServiceName()
{
	return getThreadName();
}

void TServiceBase::_process()
{
	if(m_registerEnabled)
	{
		if(!m_registerFlag)
		{
			TSysServicesManage::getInstance()->serviceRegister(this);
		}
	}

	VThreadBase::_process();
}
