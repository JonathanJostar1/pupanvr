/*
 * TViewConfigWinBaseFrame.h
 *
 * 设置窗体的一个基本框架
 *
 *  Created on: 2023年09月17日
 *      Author: jhting
 */

#ifndef TVIEWCONFIGWINBASEFRAME_H
#define TVIEWCONFIGWINBASEFRAME_H

#pragma once


#include "TViewObject.h"

typedef void (*funViewCofnigWinBaseFrameGetItemFrame)(lv_obj_t* parent);

typedef struct{
    char itemText[128];
    funViewCofnigWinBaseFrameGetItemFrame  funGetFrame;
}StViewConfigWinBaseFrameParamsItemInfo;

class TViewConfigWinBaseFrame: public TViewObject
{
public:
    TViewConfigWinBaseFrame();
    ~TViewConfigWinBaseFrame();
    void    setViewConfigWinTitle(const char* titleText);
    void    setViewConfigWinBaseFrameParams(StViewConfigWinBaseFrameParamsItemInfo params[]);
private:
    void    view_bodyPanelInit(lv_obj_t* bodyPanel);
    void    view_configLoad();

    static void _closeBtn_event_handler(lv_event_t* event);
    static void _menuItem_event_handler(lv_event_t* event);
    
private:
    string       m_title;                /*window title*/
    lv_obj_t*    m_view_titleObj;
    lv_obj_t*    m_viewMenuItemList;     /*for display left list of items*/
    lv_obj_t*    m_lastListItemBtnObj;   /*record last clicked item*/
    lv_obj_t*    m_lvObjBodyRightPanel;  /*for display item config window(as parent)*/
    StViewConfigWinBaseFrameParamsItemInfo*  m_stViewConfigWinBaseFrameParams;
};

#endif