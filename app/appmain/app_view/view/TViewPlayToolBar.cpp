#include "TViewPlayToolBar.h"
#include "TViewFontUtils.h"

typedef struct{
    int     menuItemID;
    char    menuItemText[256];
    char    menuItemImage[256];
    char    menuItemFontAwesome[10];
}StToolbarMenuItemInfo;

StToolbarMenuItemInfo gStToolbarMenuItemInfo[] = {
    {1, "功能1",     "",     "\uf2c3"},
    {2, "功能2",     "",     "\uf03d"},
    {4, "功能3",     "",     "\uf1c8"},
    
    {4, "功能4",     "",     "\uf1c0"},
    {5, "功能5",     "",     "\uf071"},
    {6, "功能6",     "",     "\uf071"},
};


TViewPlayToolBar::TViewPlayToolBar(ViewHandle parentHandle)
    :TViewObject(parentHandle)
{
    int itemWidth = 0;
    int itemHeight = 0;

    int viewPlayToolBarHeight = 50;
    int viewPlayToolBardWidth = 350;

    lv_style_set_border_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 1));
	lv_style_set_border_width(&m_style, 1);
	lv_style_set_radius(&m_style, 0);
	lv_style_set_bg_color(&m_style, lv_palette_darken(LV_PALETTE_GREY, 4));    

    
	lv_obj_set_scrollbar_mode(m_viewHandle, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(m_viewHandle, LV_OBJ_FLAG_SCROLLABLE);

    StToolbarMenuItemInfo* pItem = NULL;

    this->setViewSize(viewPlayToolBardWidth, viewPlayToolBarHeight);    

    lv_obj_set_flex_flow(m_viewHandle, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(m_viewHandle, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    unsigned int itemCount = sizeof(gStToolbarMenuItemInfo) / sizeof(gStToolbarMenuItemInfo[0]);

    itemHeight = viewPlayToolBarHeight - 15;
    itemWidth = viewPlayToolBardWidth / itemCount - 15;

    for(unsigned int i = 0; i < itemCount; i++)
    {
        lv_obj_t* lvObjItem = lv_obj_create(m_viewHandle);
        pItem = &gStToolbarMenuItemInfo[i];
 
        lv_obj_set_style_border_width(lvObjItem, 0, 0);
        lv_obj_set_style_pad_all(lvObjItem, 0, 0);
        lv_obj_set_style_bg_color(lvObjItem, lv_palette_darken(LV_PALETTE_GREY, 4), 0); 

        lv_obj_set_size(lvObjItem, itemWidth, itemHeight);
       
        lv_obj_t* plabItemImg = lv_label_create(lvObjItem);
        lv_obj_set_style_text_color(plabItemImg, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_font(plabItemImg, TViewFontUtils::getInstance()->getViewFont(VF_FONT_FontAwesome, 30), 0);
        lv_label_set_text(plabItemImg, pItem->menuItemFontAwesome);

        lv_obj_add_event_cb(lvObjItem, _btn_event_process, LV_EVENT_CLICKED, this);
    }
}


TViewPlayToolBar::~TViewPlayToolBar()
{
    
}

void TViewPlayToolBar::_btn_event_process(lv_event_t * event)
{
    printf("_btn_event_process LV_EVENT_CLICKED!\n");
}
