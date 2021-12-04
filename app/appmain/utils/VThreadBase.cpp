/*
 * VThreadBase.cpp
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 */

#include "VThreadBase.h"
#include "tlog.h"

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

VThreadBase::VThreadBase()
{
	m_runningFlag = false;
	m_stop_flag = false;
	memset(m_thread_name, 0, sizeof(m_thread_name));
}

VThreadBase::~VThreadBase()
{

}

bool VThreadBase::start()
{
	int ret = 0;

	pthread_t pid = 0;
	pthread_attr_t attr;

	ret = pthread_attr_init(&attr);
	if(ret != 0)
	{
		LOG(ERROR) << "start pthread_attr_init failure!!!\n";
		return false;
	}

	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(ret != 0)
	{
		pthread_attr_destroy(&attr);
		LOG(ERROR) << "pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED failure!!!\n";
		return false;
	}

	ret = pthread_create(&pid, &attr, run_process, this);
	if(ret == 0)
	{
		LOG(INFO) << m_thread_name << " pthread_create OK! " << endl;
	}else{
		LOG(ERROR) << m_thread_name << " pthread_create failure!! \n";
	}

	pthread_attr_destroy(&attr);
	return (ret == 0);
}

bool VThreadBase::stop(bool block)
{
	m_stop_flag = false;
	return true;
}

bool VThreadBase::status()
{
	return m_runningFlag;
}

void* VThreadBase::run_process(void* data)
{
	VThreadBase* obj = (VThreadBase*)data;
	obj->_process();
	return NULL;
}

void VThreadBase::_process()
{
	m_runningFlag = true;
	process();
	m_runningFlag = false;
}

void VThreadBase::setThreadName(const char *name)
{
	snprintf(m_thread_name, sizeof(m_thread_name), "%s", name);
}

const char* VThreadBase::getThreadName()
{
	return m_thread_name;
}
