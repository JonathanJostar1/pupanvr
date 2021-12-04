/*
 * TMutex.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "TMutex.h"

TMutex::TMutex()
{
	pthread_mutex_init(&m_mutex, NULL);

}

TMutex::~TMutex()
{

}

void TMutex::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void TMutex::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

bool TMutex::trylock()
{
	return pthread_mutex_trylock(&m_mutex) == 0;
}
