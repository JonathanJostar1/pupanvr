/*
 * TServiceBase.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_MODULES_TSERVICEBASE_H_
#define APPMAIN_MODULES_TSERVICEBASE_H_

#include "VThreadBase.h"

class TServiceBase: public VThreadBase
{
public:
	TServiceBase();
	virtual ~TServiceBase();
	virtual void setServiceName(const char* name);
	virtual const char* getServiceName();
private:
	void _process();
protected:
	bool    m_registerFlag;			 	/*标识是否已经注册到管理类里*/
	bool    m_registerEnabled;			/*标识是否启用注册到管理类里*/
};

#endif /* APPMAIN_MODULES_TSERVICEBASE_H_ */
