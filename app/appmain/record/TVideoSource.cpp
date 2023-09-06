/*
 * TVideoSource.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "TVideoSource.h"

TVideoSource::TVideoSource()
{
	m_videoSourceDevType = VS_TYPE_UNKNOW;
}

VideoSourceDevType TVideoSource::getVideoSourceType()
{
	return m_videoSourceDevType;
}

TVideoSource::~TVideoSource()
{

}

