/*
 * TMutexLocker.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "TMutexLocker.h"

TMutexLocker::TMutexLocker(TMutex* mutex)
{
	m_mutex = mutex;
	m_mutex->lock();
}

TMutexLocker::~TMutexLocker()
{
	if(m_mutex)
	{
		m_mutex->unlock();
	}
}

