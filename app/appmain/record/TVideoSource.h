/*
 * TVideoSource.h
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 *
 *  视频源，可以是相机、文件
 */

#ifndef APPMAIN_RECORD_TVIDEOSOURCE_H_
#define APPMAIN_RECORD_TVIDEOSOURCE_H_

#include "sys_defines.h"

class TVideoSource
{
public:
	TVideoSource();
	VideoSourceDevType getVideoSourceType();
	virtual ~TVideoSource();
protected:
	VideoSourceDevType	m_videoSourceDevType;
};

#endif /* APPMAIN_RECORD_TVIDEOSOURCE_H_ */
