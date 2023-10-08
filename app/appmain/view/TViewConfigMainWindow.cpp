#include "TViewConfigMainWindow.h"
#include "TViewFontUtils.h"
#include<stdio.h>

typedef struct{
    int     menuItemID;
    char    menuItemText[256];
    char    menuItemImage[256];
    char    menuItemFontAwesome[10];
    lv_obj_t* menuItemObj;
}StSysTopMenuItemInfo;

StSysTopMenuItemInfo gStSysTopMenuItemInfoList[] = {
    {0, "设置",     "",     "\uf2c3", NULL},
    {1, "通道",     "",     "\uf03d", NULL},
    {2, "录相",     "",     "\uf1c8", NULL},
    
    {3, "存储",     "",     "\uf1c0", NULL},
    {4, "报警",     "",     "\uf071", NULL},
    
    {5, "网络",     "",     "\uf26b", NULL},
    {6, "云平台",       "",     "\uf0ee", NULL},

    {7, "设备",     "",     "\uf085", NULL},
};

TViewConfigMainWindow::TViewConfigMainWindow(ViewHandle parentHandle)
    :TViewObject(parentHandle)
{
    lv_style_set_border_width(&m_style, 0);
	lv_style_set_radius(&m_style, 0);
	lv_style_set_pad_all(&m_style, 0);

    lv_obj_set_height(m_viewHandle, lv_obj_get_height(parentHandle));
    lv_obj_set_width(m_viewHandle, lv_obj_get_width(parentHandle));
    lv_obj_center(m_viewHandle);
    lv_obj_refr_size(m_viewHandle);

    lv_obj_set_scrollbar_mode(m_viewHandle, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(m_viewHandle, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_layout(m_viewHandle, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_viewHandle, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(m_viewHandle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(m_viewHandle, 0, 0);

    m_menu_index = -1;

    view_init();
}

TViewConfigMainWindow::~TViewConfigMainWindow()
{

}

void TViewConfigMainWindow::view_init()
{
    m_menu_index = 0;

    lv_obj_t* lvObjTopPanel = lv_obj_create(m_viewHandle);
    lv_obj_set_style_bg_color(lvObjTopPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_set_height(lvObjTopPanel, 80);
    lv_obj_set_width(lvObjTopPanel, lv_obj_get_width(m_viewHandle));
    lv_obj_set_style_border_width(lvObjTopPanel, 0, 0);
    lv_obj_set_style_pad_all(lvObjTopPanel, 0, 0);
    lv_obj_set_style_radius(lvObjTopPanel, 0, 0);
    lv_obj_set_style_pad_gap(lvObjTopPanel, 0, 0);
    lv_obj_center(lvObjTopPanel);

    lv_obj_set_scrollbar_mode(lvObjTopPanel, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(lvObjTopPanel, LV_OBJ_FLAG_SCROLLABLE);

    view_init_top(lvObjTopPanel);

    lv_obj_t*  lvObjbodyPanel = lv_obj_create(m_viewHandle);
    lv_obj_set_style_bg_color(lvObjbodyPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 4), 0);
    lv_obj_set_width(lvObjbodyPanel, lv_obj_get_width(m_viewHandle));
    lv_obj_set_style_border_width(lvObjbodyPanel, 0, 0);
    lv_obj_set_style_pad_all(lvObjbodyPanel, 0, 0);
    lv_obj_set_flex_grow(lvObjbodyPanel, 1);
    lv_obj_set_style_radius(lvObjbodyPanel, 0, 0);
    lv_obj_set_style_pad_gap(lvObjbodyPanel, 0, 0);
    lv_obj_center(lvObjbodyPanel);

    lv_obj_set_scrollbar_mode(lvObjbodyPanel, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(lvObjbodyPanel, LV_OBJ_FLAG_SCROLLABLE);
    
    view_init_bottom(lvObjbodyPanel);
}

void TViewConfigMainWindow::view_init_top(ViewHandle handle)
{
    /*图标或文字*/
   /*设置布局*/
    lv_obj_set_layout(handle, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(handle, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(handle, 10, 0);
    
    lv_obj_set_flex_align(handle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    //logo显示
    lv_obj_t* toplogPanel = lv_obj_create(handle);
    lv_obj_set_style_bg_color(toplogPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 1), 0);
    lv_obj_set_style_border_width(toplogPanel, 0, 0);
    lv_obj_set_style_pad_all(toplogPanel, 0, 0);
    lv_obj_set_style_radius(toplogPanel, 0, 0);
    lv_obj_set_style_width(toplogPanel, 250, 0);
    lv_obj_set_flex_align(toplogPanel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* plabItemImg = lv_label_create(toplogPanel);
    lv_obj_set_style_text_color(plabItemImg, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(plabItemImg, TViewFontUtils::getInstance()->getViewFont(VF_FONT_DEFAULT, 20), 0);
    lv_label_set_text(plabItemImg, "系统管理");
    //lv_obj_set_style_align()

    //顶部菜单
    lv_obj_t* topMenuPanel = lv_obj_create(handle);
    lv_obj_set_flex_grow(topMenuPanel, 1);
    
    lv_obj_set_style_bg_color(topMenuPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_set_style_border_width(topMenuPanel, 0, 0);
    lv_obj_set_style_pad_all(topMenuPanel, 0, 0);
    lv_obj_set_style_pad_left(topMenuPanel, 20, 0);
    lv_obj_set_style_radius(topMenuPanel, 0, 0);

    lv_obj_set_scrollbar_mode(topMenuPanel, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(topMenuPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_align(topMenuPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view_init_top_menu(topMenuPanel);

    lv_obj_t* winButtonPanel = lv_obj_create(handle);
    lv_obj_set_width(winButtonPanel, 150);
    lv_obj_set_flex_grow(winButtonPanel, 0);
    lv_obj_set_style_border_width(winButtonPanel, 0, 0);
    lv_obj_set_style_pad_all(winButtonPanel, 0, 0);
    lv_obj_set_style_radius(winButtonPanel, 0, 0);
    lv_obj_set_style_bg_color(winButtonPanel, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);

}

void TViewConfigMainWindow::view_init_top_menu(ViewHandle topMenuPanel)
{
    int menuItemWidth = 150;

    lv_obj_set_layout(topMenuPanel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(topMenuPanel, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(topMenuPanel, 0, 0);
    lv_obj_set_style_pad_gap(topMenuPanel, 10, 0);

    StSysTopMenuItemInfo* pItem  = NULL;
    for(unsigned i = 0; i < sizeof(gStSysTopMenuItemInfoList) / sizeof(gStSysTopMenuItemInfoList[0]); i++)
    {
        pItem = &gStSysTopMenuItemInfoList[i];

        lv_obj_t* pMenuItemObj = lv_obj_create(topMenuPanel);
        lv_obj_set_style_border_width(pMenuItemObj, 0, 0);
        lv_obj_set_style_pad_all(pMenuItemObj, 0, 0);
        lv_obj_set_style_radius(pMenuItemObj, 0, 0);
        lv_obj_set_width(pMenuItemObj, menuItemWidth);
        if(m_menu_index == (int)i)
        {
            lv_obj_set_style_bg_color(pMenuItemObj, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
        }else{
            lv_obj_set_style_bg_color(pMenuItemObj, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);    
        }
        lv_obj_set_flex_align(pMenuItemObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

          //image
        lv_obj_t* plabItemImg = lv_label_create(pMenuItemObj);
        lv_obj_set_style_text_color(plabItemImg, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_font(plabItemImg, TViewFontUtils::getInstance()->getViewFont(VF_FONT_FontAwesome, 25), 0);
        lv_label_set_text(plabItemImg, pItem->menuItemFontAwesome);

        lv_obj_t* plabItemText = lv_label_create(pMenuItemObj);
        lv_obj_set_style_text_color(plabItemText, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_font(plabItemText, TViewFontUtils::getInstance()->getViewFont(VF_FONT_DEFAULT, 25), 0);
        lv_label_set_text(plabItemText, pItem->menuItemText);
        lv_obj_set_style_text_align(plabItemText, LV_TEXT_ALIGN_CENTER, 0);

        pItem->menuItemObj = pMenuItemObj;
        lv_obj_set_user_data(pMenuItemObj, pItem);
        lv_obj_add_event_cb(pMenuItemObj, _menuitem_event_process, LV_EVENT_CLICKED, this);
    }   
}

void TViewConfigMainWindow::view_init_bottom(ViewHandle handle)
{
    
}

void TViewConfigMainWindow::_menuitem_event_process(lv_event_t *event)
{
    TViewConfigMainWindow* obj = (TViewConfigMainWindow*)event->user_data;    
    obj->__menuitem_event_process(event);        
}

void TViewConfigMainWindow::__menuitem_event_process(lv_event_t *event)
{
     lv_obj_t *obj = event->target;
    if(event->code == LV_EVENT_CLICKED){
        StSysTopMenuItemInfo* pItem = (StSysTopMenuItemInfo*)obj->user_data;
        if(pItem)
        {
            if(m_menu_index >= 0)
            {
                lv_obj_set_style_bg_color(gStSysTopMenuItemInfoList[m_menu_index].menuItemObj, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), 0);             
            }

            m_menu_index = pItem->menuItemID;
            lv_obj_set_style_bg_color(obj, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);  
            _menuitem_clicked_process(pItem->menuItemID);     
        }
    }    
}

void TViewConfigMainWindow::_menuitem_clicked_process(int id)
{
     switch(id)
    {
        case 1:
            break;
        case 2:
            break;
        default:
            break;
    }
}