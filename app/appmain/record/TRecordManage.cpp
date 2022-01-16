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
	loadVideoChannel();

	while(!m_stop_flag)
	{
		sleep(1);
	}

}

bool TRecordManage::loadVideoChannel()
{
	int videoCount = 5;
	TMutexLocker locker(&m_mutex);
	videoChannelFree();

	TVideoChannel* videoChannel;
	for(int i = 1; i < videoCount + 1; i++)
	{
		videoChannel = new TVideoChannel();
		videoChannel->setChannelValue(i);
		m_channelMap[i] = videoChannel;
	}

	return true;
}

void TRecordManage::videoChannelFree()
{
	map<int, TVideoChannel*>::iterator iter;
	iter = m_channelMap.begin();
	TVideoChannel* pVideoChannel = NULL;
	while(iter != m_channelMap.end())
	{
		pVideoChannel = iter->second;
		delete pVideoChannel;
		m_channelMap.erase(iter);
		iter++;
	}
}
