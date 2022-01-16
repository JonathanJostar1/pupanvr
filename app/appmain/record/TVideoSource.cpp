/*
 * TVideoSource.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "TVideoSource.h"

TVideoSource::TVideoSource(VideoSourceDevType type)
	:m_videoSourceDevType(type)
{

}

VideoSourceDevType TVideoSource::getVideoSourceType()
{
	return m_videoSourceDevType;
}

TVideoSource::~TVideoSource()
{

}

