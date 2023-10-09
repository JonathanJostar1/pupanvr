/*
 * TViewConfigWinBaseFrame.cpp
 *
 * 设置窗体的一个基本框架
 *
 *  Created on: 2023年09月17日
 *      Author: jhting
 */


#include "TViewConfigWinBaseFrame.h"
#include "TViewStyle.h"
#include "TViewFontUtils.h"

#define TOPPANNEL_HEIGHT    50

TViewConfigWinBaseFrame::TViewConfigWinBaseFrame()
{
    lv_obj_set_width(m_viewHandle, 800);
    lv_obj_set_height(m_viewHandle, 600);
    
    m_view_titleObj = NULL;
    m_viewMenuItemList = NULL;
    m_lastListItemBtnObj = NULL;
    m_lvObjBodyRightPanel = NULL;
    
    m_title = "";
    m_lastListItemBtnObj = NULL;
    m_stViewConfigWinBaseFrameParams = NULL;

    lv_obj_center(m_viewHandle);
    lv_obj_refr_size(m_viewHandle);

    lv_obj_set_scrollbar_mode(m_viewHandle, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(m_viewHandle, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_set_style_pad_all(m_viewHandle, 0, 0);
    TViewStyle::getInstance()->setDalogModelViewWindowStyle(this);

    lv_obj_set_layout(m_viewHandle, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_viewHandle, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(m_viewHandle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(m_viewHandle, 0, 0);

/*top panel*/
    lv_obj_t* lvObjTopPanel = lv_obj_create(m_viewHandle);

    lv_obj_set_width(lvObjTopPanel, this->getViewWidth());
    lv_obj_set_style_bg_color(lvObjTopPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_set_height(lvObjTopPanel, TOPPANNEL_HEIGHT);

    lv_obj_set_width(lvObjTopPanel, lv_obj_get_width(m_viewHandle));
    lv_obj_set_style_border_width(lvObjTopPanel, 0, 0);
    lv_obj_set_style_pad_all(lvObjTopPanel, 0, 0);
    lv_obj_set_style_radius(lvObjTopPanel, 0, 0);

    m_view_titleObj = lv_label_create(lvObjTopPanel);
    lv_obj_set_style_text_color(m_view_titleObj, lv_color_hex(0xffffff), 0);    
    lv_obj_set_style_text_font(m_view_titleObj, TViewFontUtils::getInstance()->getDefaultFont(20, 0), 0);
    lv_label_set_text(m_view_titleObj, "基本框架");
    lv_obj_set_style_text_align(m_view_titleObj, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(m_view_titleObj);

    /*关闭按钮*/
    lv_obj_t* btnObj = lv_btn_create(lvObjTopPanel);
    lv_obj_set_width(btnObj, TOPPANNEL_HEIGHT - 10);
    lv_obj_set_height(btnObj, TOPPANNEL_HEIGHT - 10);
    lv_obj_set_pos(btnObj, this->getViewWidth() - TOPPANNEL_HEIGHT + 10, 5);
    lv_obj_set_style_bg_color(btnObj, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_set_style_border_width(btnObj, 0, 0);
    lv_obj_set_style_pad_all(btnObj, 0, 0);
    lv_obj_set_style_radius(btnObj, 0, 0);

    lv_obj_add_event_cb(btnObj, _closeBtn_event_handler, LV_EVENT_CLICKED, this);

    lv_obj_t * labelbtn = lv_label_create(btnObj);
    lv_obj_set_style_text_font(labelbtn, TViewFontUtils::getInstance()->getDefaultFont(20, 0), 0);
    lv_label_set_text(labelbtn, " X ");
    lv_obj_center(labelbtn);

/*body*/
    lv_obj_t* lvObjBodyPanel = lv_obj_create(m_viewHandle);
    lv_obj_set_width(lvObjBodyPanel, this->getViewWidth());
    lv_obj_set_flex_grow(lvObjBodyPanel, 1);
    lv_obj_set_style_bg_color(lvObjBodyPanel, TViewStyle::DialogFrameBgColor, 0);
    lv_obj_set_style_border_width(lvObjBodyPanel, 0, 0);
    lv_obj_set_style_pad_all(lvObjBodyPanel, 0, 0);
    lv_obj_set_style_radius(lvObjBodyPanel, 0, 0);

    view_bodyPanelInit(lvObjBodyPanel);
}

TViewConfigWinBaseFrame::~TViewConfigWinBaseFrame()
{
    LOG(INFO) << "TViewConfigWinBaseFrame:" << m_title  << ", close!" << endl;
}

void TViewConfigWinBaseFrame::setViewConfigWinTitle(const char* titleText)
{
    m_title = titleText;
    lv_label_set_text(m_view_titleObj, titleText);
}

void TViewConfigWinBaseFrame::setViewConfigWinBaseFrameParams(StViewConfigWinBaseFrameParamsItemInfo params[])
{
    m_stViewConfigWinBaseFrameParams = params;
    view_configLoad();
}

void TViewConfigWinBaseFrame::view_configLoad()
{
    int i = 0;
    lv_obj_t* btn = NULL;

    if(!m_stViewConfigWinBaseFrameParams)
    {
        return;
    }

    if(!m_viewMenuItemList)
    {
        return;
    }

    StViewConfigWinBaseFrameParamsItemInfo* item = &m_stViewConfigWinBaseFrameParams[i++];
    while(item)
    {
        if(!item->itemText)
        {
            break;
        }

        if(strlen(item->itemText) == 0)
        {
            break;
        }

        printf("add lv_list_add_btn:%s\n", item->itemText);
        btn = lv_list_add_btn(m_viewMenuItemList, NULL, item->itemText);
        lv_obj_set_style_border_width(btn, 0, 0);
        lv_obj_set_style_pad_all(btn, 20, 0);
        lv_obj_set_style_text_font(btn, TViewFontUtils::getInstance()->getDefaultFont(16, 0), 0);
        lv_obj_set_style_bg_color(btn, lv_palette_darken(LV_PALETTE_BLUE_GREY, 3), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        /*set item to button userdata, when button click can get item config!*/
        lv_obj_set_user_data(btn, item);

        lv_obj_add_event_cb(btn, _menuItem_event_handler, LV_EVENT_CLICKED, this);

        item = &m_stViewConfigWinBaseFrameParams[i++];
    }
}

void TViewConfigWinBaseFrame::view_bodyPanelInit(lv_obj_t* bodyPanel)
{
    lv_obj_set_layout(bodyPanel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(bodyPanel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bodyPanel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(bodyPanel, 0, 0);
    
    lv_obj_refr_size(m_viewHandle);
    lv_obj_refr_size(bodyPanel);
    //left
    lv_obj_t* lvObjBodyLeftPanel = lv_obj_create(bodyPanel);    
    lv_obj_set_width(lvObjBodyLeftPanel, 150);
    lv_obj_set_height(lvObjBodyLeftPanel, lv_obj_get_height(m_viewHandle) - TOPPANNEL_HEIGHT - 5); 
    lv_obj_set_style_border_width(lvObjBodyLeftPanel, 0, 0);
    lv_obj_set_style_pad_all(lvObjBodyLeftPanel, 0, 0);
    lv_obj_set_style_radius(lvObjBodyLeftPanel, 0, 0);  
    lv_obj_set_style_bg_color(lvObjBodyLeftPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 3), 0);

    //right
    m_lvObjBodyRightPanel = lv_obj_create(bodyPanel); 
    lv_obj_set_flex_grow(m_lvObjBodyRightPanel, 1);   
    lv_obj_set_height(m_lvObjBodyRightPanel, lv_obj_get_height(m_viewHandle) - TOPPANNEL_HEIGHT - 5); 
    lv_obj_set_style_border_width(m_lvObjBodyRightPanel, 0, 0);
    lv_obj_set_style_pad_all(m_lvObjBodyRightPanel, 0, 0);
    lv_obj_set_style_radius(m_lvObjBodyRightPanel, 0, 0); 

    lv_obj_set_style_bg_color(m_lvObjBodyRightPanel, TViewStyle::DialogFrameBgColor, 0);

    //left list
    m_viewMenuItemList = lv_list_create(lvObjBodyLeftPanel);
    lv_obj_set_width(m_viewMenuItemList, 150);
    lv_obj_set_style_border_width(m_viewMenuItemList, 0, 0);
    lv_obj_set_style_pad_all(m_viewMenuItemList, 0, 0);
    lv_obj_set_style_radius(m_viewMenuItemList, 0, 0); 
    //lv_obj_set_size(list2, lv_pct(40), lv_pct(100));
    //lv_obj_align(list, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(m_viewMenuItemList, lv_palette_darken(LV_PALETTE_BLUE_GREY, 3), 0);
    lv_obj_set_flex_flow(m_viewMenuItemList, LV_FLEX_FLOW_COLUMN);

    view_configLoad();
}

void TViewConfigWinBaseFrame::_closeBtn_event_handler(lv_event_t* event)
{
    TViewConfigWinBaseFrame* obj = (TViewConfigWinBaseFrame*)event->user_data;    
    obj->viewHide();
    delete obj;   /*delete instance of TViewConfigWinBaseFrame*/
}

void TViewConfigWinBaseFrame::_menuItem_event_handler(lv_event_t* event)
{
    TViewConfigWinBaseFrame* obj =  (TViewConfigWinBaseFrame*)event->user_data;
    if(obj->m_lastListItemBtnObj)
    {
        lv_obj_set_style_bg_color(obj->m_lastListItemBtnObj, lv_palette_darken(LV_PALETTE_BLUE_GREY, 3), 0);
    }

    obj->m_lastListItemBtnObj =  event->target;

    printf("_menuItem_event_handler click!\n");
    lv_obj_set_style_bg_color(obj->m_lastListItemBtnObj, TViewStyle::DialogFrameBgColor, 0);

    StViewConfigWinBaseFrameParamsItemInfo* item = (StViewConfigWinBaseFrameParamsItemInfo*)lv_obj_get_user_data(obj->m_lastListItemBtnObj);
    if(!item)
    {
        return;
    }

    if(item && item->funGetFrame)
    {
        item->funGetFrame(obj->m_lvObjBodyRightPanel);
    }

}