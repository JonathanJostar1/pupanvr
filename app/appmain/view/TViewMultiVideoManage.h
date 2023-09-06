/*
 * TViewMultiVideoManage.h
 *多视频窗管理器界面
 *  Created on: 2022年1月16日
 *      Author: jhting
 * 
 * 视频预览管理窗口
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

class TViewMultiVideoManage: public TViewObject
{
public:
	TViewMultiVideoManage(ViewHandle parentHandle);
	virtual ~TViewMultiVideoManage();
	/*设置分割模式*/
	bool						setViewSpliteMode(ViewVideoPlayerSplitMode viewSpliteMode);
	/*获取当前的分割模式*/
	ViewVideoPlayerSplitMode 	getViewSpliteMode();
	/*设置当前选中的通道*/
	void						setCurrentSelectPlayView(int chn);
private:
	bool 						_setViewSpliteModeInit(ViewVideoPlayerSplitMode viewSpliteMode);
	bool						_setViewShowMode(ViewVideoPlayerSplitMode viewSpliteMode);

	void	event_process(lv_event_t * event);
private:
	/*显示模式*/
	ViewVideoPlayerSplitMode 	m_viewSpliteMode;
	/*记录各个播放窗口*/
	map<int, TViewPlayer*>		m_viewPlayerMap;
	/*记录当前选中的通道号*/
	int							m_selectChnValue;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWMULTIVIDEOMANAGE_H_ */
