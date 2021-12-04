/*
 * TSysServicesManage.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_MODULES_TSYSSERVICESMANAGE_H_
#define APPMAIN_MODULES_TSYSSERVICESMANAGE_H_

#include "tlog.h"
#include<string>
#include<map>

#include "TServiceBase.h"

using namespace std;

class TSysServicesManage
{
public:
	static TSysServicesManage* getInstance();

	bool	serviceRegister(TServiceBase* pService);
public:
	TSysServicesManage();
	virtual ~TSysServicesManage();
private:
	map<string, TServiceBase*>	m_sysServiceMap;
};

#endif /* APPMAIN_MODULES_TSYSSERVICESMANAGE_H_ */
