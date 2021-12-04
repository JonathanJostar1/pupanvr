/*
 * TRecordManage.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_RECORD_TRECORDMANAGE_H_
#define APPMAIN_RECORD_TRECORDMANAGE_H_

#include "TServiceBase.h"

class TRecordManage: public TServiceBase
{
public:
	static TRecordManage* getInstance();
private:
	TRecordManage();
	virtual ~TRecordManage();
	void process();
private:

private:
	static TRecordManage*	m_instance;
};

#endif /* APPMAIN_RECORD_TRECORDMANAGE_H_ */
