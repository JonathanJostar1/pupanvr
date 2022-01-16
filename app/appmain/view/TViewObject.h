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
protected:
	ViewHandle m_viewHandle;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWOBJECT_H_ */
