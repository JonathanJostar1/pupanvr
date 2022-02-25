/*
 * TViewFontUtils.h
 *
 *  Created on: 2022年2月17日
 *      Author: jhting
 */

#ifndef APP_APPMAIN_VIEW_TVIEWFONTUTILS_H_
#define APP_APPMAIN_VIEW_TVIEWFONTUTILS_H_

#include<iostream>
#include<string>
#include<map>

#include "tlog.h"
#include "view_lvgl.h"

#include "TMutex.h"
#include "TMutexLocker.h"
#include "lv_freetype.h"

/*
 *
 */
class TViewFontUtils
{
public:
	static TViewFontUtils* getInstance();

	lv_ft_info_t* 		getFont(char* fontName, int size, int style = 0);
	bool				releaseFont(char* fontName, int size, int style);
private:
	TViewFontUtils();
	virtual ~TViewFontUtils();

	string			_getFontMapKeyName(const char* fontName, int size, int style);

	lv_ft_info_t*	_create_lv_ft_info(const char* fontFileName, int size, int style);
	lv_ft_info_t*	_getFontByKeyname(string& mapkeyName);
	bool			_addFontToMap(string& keyName, lv_ft_info_t* obj);
	void			_delFontByKeyName(string& mapkeyName);
private:
	static TViewFontUtils*			m_instance;

	TMutex 							m_mutex;
	map<string, lv_ft_info_t*>		m_mapfontInfo;
};

#endif /* APP_APPMAIN_VIEW_TVIEWFONTUTILS_H_ */
