/*
 * TViewObject.h
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#ifndef APPMAIN_APPMAIN_VIEW_TVIEWOBJECT_H_
#define APPMAIN_APPMAIN_VIEW_TVIEWOBJECT_H_

#include "view_def.h"

class TViewObject
{
public:
	TViewObject();
	TViewObject(ViewHandle parentHandle);
	virtual ~TViewObject();
	/**
	 * 获取窗口对像句柄
	 * */
	virtual ViewHandle 	getViewHandle();
	/**
	 * 设置显示位置
	 * */
	virtual bool		setpos(int x, int y);
	/**
	 * 设置显示大小
	 * */
	virtual bool		setViewSize(int width, int height);
	virtual int			getViewWidth();
	virtual int 		getViewHeight();

	virtual bool 		viewShow();
	virtual bool		viewHide();

	virtual void		event_process(lv_event_t * event);
private:
	static void			_event_process(lv_event_t * event);
protected:
	ViewHandle m_viewHandle;
	lv_style_t m_style;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWOBJECT_H_ */
