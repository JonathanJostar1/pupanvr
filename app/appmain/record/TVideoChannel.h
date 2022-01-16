/*
 * TVideoChannel.h
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 *
 *  视频通道对像，一个通道对像应该包含一个视频源
 */

#ifndef APPMAIN_RECORD_TVIDEOCHANNEL_H_
#define APPMAIN_RECORD_TVIDEOCHANNEL_H_

#include "TVideoSource.h"

class TVideoChannel
{
public:
	TVideoChannel();
	virtual ~TVideoChannel();

	int		getChannelValue();
	void 	setChannelValue(int value);
private:
	int		m_channelValue;

	TVideoSource*	m_videoSource;
};

#endif /* APPMAIN_RECORD_TVIDEOCHANNEL_H_ */
