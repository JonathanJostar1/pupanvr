/*
 * TViewObject.h
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 * 
 * 所有UI界面的父类，定义一些统一、基本的操作
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
	virtual bool		visiabled();


	virtual lv_style_t*	getDefaultStyle();	

	/*事件处理*/
	virtual void		event_process(lv_event_t * event);

protected:
	static void			_event_process(lv_event_t * event);
protected:
	/*LVGL的一个对像句柄*/
	ViewHandle m_viewHandle;
	/*LVGL的一个对像的基本样式*/
	lv_style_t m_style;
};

#endif /* APPMAIN_APPMAIN_VIEW_TVIEWOBJECT_H_ */
