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

/*定义字体类型*/
typedef enum{
	VF_FONT_UNKNOW = 0,
	VF_FONT_DEFAULT = 1,
}ViewFontDef;

/*
 *
 */
class TViewFontUtils
{
public:
	static TViewFontUtils* getInstance();
	/*获取指定的字体对像*/
	lv_font_t*			getViewFont(ViewFontDef type, int size, int style = 0);
	/*获取默认的字体*/
	lv_font_t*			getDefaultFont(int size, int style = 0);
	/*释放字体*/
	void				releaseViewFont(ViewFontDef type, int size, int style);
private:
	TViewFontUtils();
	virtual ~TViewFontUtils();

	lv_ft_info_t* 	_getFont(const char* fontName, int size, int style = 0);
	bool			_releaseFont(const char* fontName, int size, int style);

	string			_getFontMapKeyName(const char* fontName, int size, int style);

	lv_ft_info_t*	_create_lv_ft_info(const char* fontFileName, int size, int style);
	lv_ft_info_t*	_getFontByKeyname(string& mapkeyName);
	bool			_addFontToMap(string& keyName, lv_ft_info_t* obj);
	void			_delFontByKeyName(string& mapkeyName);

	const char*		_getFontNameByViewFontDef(ViewFontDef type);
private:
	static TViewFontUtils*			m_instance;

	TMutex 							m_mutex;
	/*记录加载的字体信息*/
	map<string, lv_ft_info_t*>		m_mapfontInfo;
};

#endif /* APP_APPMAIN_VIEW_TVIEWFONTUTILS_H_ */
