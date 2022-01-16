/*
 * TViewObject.cpp
 *
 *  Created on: 2022年1月16日
 *      Author: jhting
 */

#include "TViewObject.h"

TViewObject::TViewObject()
{
	m_viewHandle = NULL;
}

TViewObject::~TViewObject()
{
	if(m_viewHandle)
	{
		lv_obj_del(m_viewHandle);
		m_viewHandle = NULL;
	}
}

ViewHandle TViewObject::getViewHandle()
{
	return m_viewHandle;
}

bool TViewObject::setViewSize(int width, int height)
{
	if(!m_viewHandle)
	{
		return false;
	}


	lv_obj_set_size(m_viewHandle, width, height);

	lv_obj_refr_size(m_viewHandle);
	return true;
}

bool TViewObject::setpos(int x, int y)
{
	if(!m_viewHandle)
	{
		return false;
	}

	lv_obj_set_pos(m_viewHandle, x, y);
	return true;
}

bool TViewObject::viewShow()
{
	lv_obj_clear_flag(m_viewHandle, LV_OBJ_FLAG_HIDDEN);
	return true;
}

bool TViewObject::viewHide()
{
	lv_obj_add_flag(m_viewHandle, LV_OBJ_FLAG_HIDDEN);
	return true;
}

int TViewObject::getViewWidth()
{
	return lv_obj_get_width(m_viewHandle);
}

int TViewObject::getViewHeight()
{
	return lv_obj_get_height(m_viewHandle);
}
