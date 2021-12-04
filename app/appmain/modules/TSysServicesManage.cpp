/*
 * TSysServicesManage.cpp
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#include "TSysServicesManage.h"
#include "GetClassInstance.h"

TSysServicesManage::TSysServicesManage()
{

}

bool TSysServicesManage::serviceRegister(TServiceBase* pService)
{
	return true;
}

TSysServicesManage::~TSysServicesManage()
{

}

TSysServicesManage* TSysServicesManage::getInstance()
{
	return GetClassInstance<TSysServicesManage>::getInstance();
}
