/*
 * TViewPlayer.h
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWPLAYER_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWPLAYER_H_

#include "TViewObject.h"

class TViewPlayer: public TViewObject
{
public:
	TViewPlayer(ViewHandle parentHandle);
	virtual ~TViewPlayer();
	void	setChannelValue(int chn);
	int		getChannelValue();

	void	setViewPlayerActive(bool flag);
private:
	void	event_process(lv_event_t * event);
private:
	ViewHandle	m_hitlabel;
	int			m_chn;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWPLAYER_H_ */
