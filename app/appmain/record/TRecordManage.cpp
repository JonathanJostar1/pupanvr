/*
 * TRecordManage.cpp
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#include "TRecordManage.h"
#include "tlog.h"

#include<unistd.h>

TRecordManage*	TRecordManage::m_instance = NULL;

TRecordManage::TRecordManage()
{
	m_registerEnabled = true;
	setServiceName("TRecordManage");
}

TRecordManage* TRecordManage::getInstance()
{
	if(!m_instance)
	{
		m_instance = new TRecordManage();
	}

	return m_instance;
}

TRecordManage::~TRecordManage()
{

}

void TRecordManage::process()
{
	while(!m_stop_flag)
	{
		LOG(INFO) << "hello world.....\n" << endl;
		sleep(1);
	}

}
