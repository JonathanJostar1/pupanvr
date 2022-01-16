/*
 * TVideoChannel.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "TVideoChannel.h"
#include "tlog.h"

TVideoChannel::TVideoChannel()
{
	m_channelValue = 0;

	m_videoSource = NULL;
}

TVideoChannel::~TVideoChannel()
{
	if(m_videoSource)
	{
		delete m_videoSource;
		m_videoSource = NULL;
	}

	LOG(INFO) << "video channel free! " << m_channelValue << endl;
}

int TVideoChannel::getChannelValue()
{
	return m_channelValue;
}

void TVideoChannel::setChannelValue(int value)
{
	m_channelValue = value;
}
