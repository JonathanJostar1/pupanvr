/*
 * TRecordManage.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_RECORD_TRECORDMANAGE_H_
#define APPMAIN_RECORD_TRECORDMANAGE_H_

#include "TServiceBase.h"

#include "TAppComm.h"
#include "sys_defines.h"
#include "TVideoChannel.h"

#include<map>

class TRecordManage: public TServiceBase
{
public:
	static TRecordManage* getInstance();
private:
	TRecordManage();
	virtual ~TRecordManage();
	void process();
private:
	bool	loadVideoChannel();
	void	videoChannelFree();
private:
	static TRecordManage*		m_instance;
	map<int, TVideoChannel*>	m_channelMap;
	TMutex						m_mutex;
};

#endif /* APPMAIN_RECORD_TRECORDMANAGE_H_ */
