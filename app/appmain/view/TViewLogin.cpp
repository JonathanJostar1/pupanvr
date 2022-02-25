/*
 * TViewLogin.cpp
 *
 *  Created on: 2021年12月26日
 *      Author: jhting
 */


#include "TViewLogin.h"
#include "TViewMainWindow.h"
#include "tlog.h"
#include "TViewFontUtils.h"

TViewLogin::TViewLogin(ViewHandle parentHandle): TViewObject(parentHandle)
{
	viewInit();
}

TViewLogin::~TViewLogin()
{

}

bool TViewLogin::viewInit()
{
	setViewSize(500, 400);
	lv_obj_center(m_viewHandle);

	lv_style_set_bg_color(&m_style, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2));
	lv_style_set_text_color(&m_style, lv_color_hex(0xffffff));

	lv_obj_set_flex_flow(m_viewHandle, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(m_viewHandle, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_ft_info_t *ftfont20 = TViewFontUtils::getInstance()->getFont("SourceHanSansCN-Regular.otf", 20, 0);
	/*title*/
	lv_obj_t* lvlabTitle = lv_label_create(m_viewHandle);
	lv_obj_set_style_text_font(lvlabTitle, ftfont20->font, 0);
	lv_obj_set_style_text_align(lvlabTitle, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lvlabTitle, "用户登录");
	lv_obj_set_width(lvlabTitle, lv_pct(100));
	lv_obj_set_height(lvlabTitle, 50);

	lv_ft_info_t *ftfont16 = TViewFontUtils::getInstance()->getFont("SourceHanSansCN-Regular.otf", 16, 0);
	/*用户名*/
	lv_obj_t* lvlabelName = lv_label_create(m_viewHandle);
	lv_label_set_text(lvlabelName, "用户名");
	lv_obj_set_style_text_align(lvlabelName, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_font(lvlabelName, ftfont16->font, 0);
	lv_obj_set_width(lvlabelName, lv_pct(60));


	lv_obj_t* lvUserName = lv_textarea_create(m_viewHandle);
	lv_obj_set_style_text_font(lvUserName, ftfont16->font, 0);
	lv_textarea_set_placeholder_text(lvUserName, "用户名");
	lv_textarea_set_text(lvUserName, "");
	lv_textarea_set_password_mode(lvUserName, true);
	lv_textarea_set_one_line(lvUserName, true);
	lv_obj_set_width(lvUserName, lv_pct(60));

	/*密码*/
	lv_obj_t* lvlabelPwd = lv_label_create(m_viewHandle);
	lv_label_set_text(lvlabelPwd, "密码");
	lv_obj_set_style_text_align(lvlabelPwd, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_font(lvlabelPwd, ftfont16->font, 0);
	lv_obj_set_width(lvlabelPwd, lv_pct(60));

	lv_obj_t* lvUserPwd = lv_textarea_create(m_viewHandle);
	lv_obj_set_style_text_font(lvUserPwd, ftfont16->font, 0);
	lv_textarea_set_placeholder_text(lvUserPwd, "用户密码");
	lv_textarea_set_text(lvUserPwd, "");
	lv_textarea_set_password_mode(lvUserPwd, true);
	lv_textarea_set_one_line(lvUserPwd, true);
	lv_obj_set_width(lvUserPwd, lv_pct(60));

	lv_ft_info_t *ftfont14 = TViewFontUtils::getInstance()->getFont("SourceHanSansCN-Regular.otf", 14, 0);
	/*忘记密码*/
	lv_obj_t* lvlab2 = lv_label_create(m_viewHandle);
	lv_label_set_text(lvlab2, "忘记密码\n\n");
	lv_obj_set_style_text_align(lvlab2, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_text_font(lvlab2, ftfont14->font, 0);
	lv_obj_set_style_text_color(lvlab2, lv_palette_darken(LV_PALETTE_BLUE, 4), 0);
	lv_obj_set_width(lvlab2, lv_pct(60));

	/*button 登 录*/
	lv_obj_t* lvBtnOk = lv_btn_create(m_viewHandle);
	lv_obj_set_style_text_font(lvBtnOk, ftfont16->font, 0);
	lv_obj_t* btnlabel = lv_label_create(lvBtnOk);
	lv_label_set_text(btnlabel, "登 录");
	lv_obj_center(btnlabel);
	lv_obj_set_width(lvBtnOk, lv_pct(60));

	return true;
}
