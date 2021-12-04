/*
 * TMutex.h
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#ifndef APPMAIN_UTILS_TMUTEX_H_
#define APPMAIN_UTILS_TMUTEX_H_

#include<pthread.h>

class TMutex
{
public:
	TMutex();
	virtual ~TMutex();

	void lock();
	bool trylock();
	void unlock();
private:
	pthread_mutex_t m_mutex;
};


#endif /* APPMAIN_UTILS_TMUTEX_H_ */
