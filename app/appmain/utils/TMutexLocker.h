/*
 * TMutexLocker.h
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#ifndef APPMAIN_UTILS_TMUTEXLOCKER_H_
#define APPMAIN_UTILS_TMUTEXLOCKER_H_

#include "TMutex.h"

class TMutexLocker
{
public:
	TMutexLocker(TMutex* mutex);
	virtual ~TMutexLocker();
private:
	TMutex*	m_mutex;
};

#endif /* APPMAIN_UTILS_TMUTEXLOCKER_H_ */
