/*
 * TViewMultiVideoManage.h
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWMULTIVIDEOMANAGE_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWMULTIVIDEOMANAGE_H_

#include<map>

using namespace std;

#include "TViewObject.h"
#include "TViewPlayer.h"

typedef enum{
	View_Splite_Mode_UNKNOW = 0,
	View_Splite_mode_1 = 1,
	View_Splite_mode_4 = 4,
	View_Splite_mode_9 = 9,
	View_Splite_mode_16 = 16
}ViewVideoPlayerSplitMode;

/*
 *多视频窗管理器
 */
class TViewMultiVideoManage: public TViewObject
{
public:
	TViewMultiVideoManage(ViewHandle parentHandle);
	virtual ~TViewMultiVideoManage();

	bool						setViewSpliteMode(ViewVideoPlayerSplitMode viewSpliteMode);
	ViewVideoPlayerSplitMode 	getViewSpliteMode();
private:
	bool 						_setViewSpliteModeInit(ViewVideoPlayerSplitMode viewSpliteMode);
	bool						_setViewShowMode(ViewVideoPlayerSplitMode viewSpliteMode);
private:
	/*显示模式*/
	ViewVideoPlayerSplitMode 	m_viewSpliteMode;

	map<int, TViewPlayer*>			m_viewPlayerMap;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMULTIVIDEOMANAGE_H_ */