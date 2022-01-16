/*
 * VThreadBase.h
 *
 *  Created on: 2021年12月4日
 *      Author: jhting
 *
 *  对线程的一个包装，方便使用线程
 */

#ifndef APPMAIN_UTILS_VTHREADBASE_H_
#define APPMAIN_UTILS_VTHREADBASE_H_

#include "TMutex.h"
#include "TMutexLocker.h"

class VThreadBase
{
public:
	VThreadBase();
	virtual ~VThreadBase();
	virtual void setThreadName(const char* name);
	virtual const char* getThreadName();
	virtual bool start();
	virtual bool stop(bool block = false);
	virtual bool status();
	virtual void process() = 0;
protected:
	void	_process();
private:
	static void* run_process(void* data);
protected:
	bool	m_stop_flag;
	char 	m_thread_name[64];
private:
	bool	m_runningFlag;
};

#endif /* APPMAIN_UTILS_VTHREADBASE_H_ */
