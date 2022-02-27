#include "TViewStyle.h"

TViewStyle*  TViewStyle::m_instance = NULL;

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

    lv_style_set_bg_color(style, lv_color_hex(0x293450));
	lv_style_set_text_color(style, lv_color_hex(0xffffff));
}