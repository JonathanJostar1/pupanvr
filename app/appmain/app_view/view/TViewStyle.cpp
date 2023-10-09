#include "TViewStyle.h"
#include "TViewFontUtils.h"

TViewStyle*  TViewStyle::m_instance = NULL;

 lv_color_t TViewStyle::DialogFrameBgColor = lv_color_hex(0x293450);

 static void _titleCloseBtn_event_process(lv_event_t* event)
 {
    lv_obj_t *obj = (lv_obj_t*)event->user_data;  
    if(event->code == LV_EVENT_CLICKED)
    {  
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);    
    }
 }

TViewStyle* TViewStyle::getInstance()
{
    if(!m_instance)
    {
        m_instance = new TViewStyle();
    }  

    return m_instance; 
}

TViewStyle::TViewStyle()
{

    DialogFrameBgColor = lv_color_hex(0x293450);
    
}

TViewStyle::~TViewStyle()
{

}

void    TViewStyle::setDalogModelViewWindowStyle(TViewObject* viewObj)
{
    lv_style_t* style = viewObj->getDefaultStyle();   
    
    lv_style_set_border_color(style, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_set_border_width(style, 1);
	lv_style_set_radius(style, 0);

    lv_style_set_bg_color(style, DialogFrameBgColor);
	lv_style_set_text_color(style, lv_color_hex(0xffffff));
}

ViewHandle TViewStyle::frameTitleCreater(ViewHandle parent,  int height, const char* title)
{
    int width = lv_obj_get_width(parent) - 2;
    
    lv_obj_t* pObjTitle = lv_obj_create(parent);
    lv_obj_set_style_pad_all(pObjTitle, 0, 0);
    lv_obj_set_style_radius(pObjTitle, 0, 0);
    lv_obj_set_style_border_opa(pObjTitle, 0, 0);
    lv_obj_set_style_border_width(pObjTitle, 0, 0);
    lv_obj_set_style_bg_color(pObjTitle, DialogFrameBgColor, 0);

    lv_obj_set_size(pObjTitle, width, height);
    lv_obj_set_pos(pObjTitle, 0, 0);

    //title
    lv_obj_t* plabTitle = lv_label_create(pObjTitle);
    lv_obj_set_size(plabTitle, width - 10, height - 2);
    lv_obj_center(plabTitle);
    lv_obj_set_style_text_align(plabTitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(plabTitle, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(plabTitle, TViewFontUtils::getInstance()->getDefaultFont(20, 0), 0);
    lv_label_set_text(plabTitle, title);

    lv_obj_t* pBtnClose = lv_btn_create(pObjTitle);
    lv_obj_set_size(pBtnClose, 40, height - 2);
    lv_obj_set_style_pad_all(pBtnClose, 0, 0);
    lv_obj_set_style_radius(pBtnClose, 0, 0);
    lv_obj_set_style_border_opa(pBtnClose, 0, 0);
    lv_obj_set_style_border_width(pBtnClose, 0, 0);
    lv_obj_set_style_shadow_width(pBtnClose, 0, 0);
    lv_obj_set_style_bg_color(pBtnClose, DialogFrameBgColor, 0);
    lv_obj_set_style_bg_color_filtered(pBtnClose, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);

    lv_obj_align(pBtnClose, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_add_event_cb(pBtnClose, _titleCloseBtn_event_process, LV_EVENT_CLICKED, parent);
    

    lv_obj_t* pBtnCloseTxt = lv_label_create(pBtnClose);
    lv_obj_set_style_text_font(pBtnCloseTxt, TViewFontUtils::getInstance()->getViewFont(VF_FONT_FontAwesome, 25, 0), 0);
    lv_obj_set_style_text_color(pBtnCloseTxt, lv_color_hex(0xffffff), 0);
    lv_label_set_text(pBtnCloseTxt, " \uf00d ");
    lv_obj_center(pBtnCloseTxt);

    return pObjTitle;
}