/*
 * TViewFontUtils.cpp
 *
 *  Created on: 2022年2月17日
 *      Author: jhting
 */



#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include<map>

#include<iostream>

#include "TViewFontUtils.h"
#include "accessdef.h"



#define FONT_PATH  "/nfsroot/hi3536c/app/font"

TViewFontUtils* TViewFontUtils::m_instance = NULL;

TViewFontUtils* TViewFontUtils::getInstance()
{
	if(!m_instance)
	{
		m_instance = new TViewFontUtils();
	}

	return m_instance;
}

TViewFontUtils::TViewFontUtils()
{
	if(!lv_freetype_init(64, 1, 0))
	{
		LOG(ERROR) << "lv_freetype_init failure!" << endl;
	}
}

TViewFontUtils::~TViewFontUtils()
{
	lv_ft_info_t* plvFtInfoObj = NULL;
	map<string, lv_ft_info_t*>::iterator iter;

	m_mutex.lock();
	for(iter = m_mapfontInfo.begin(); iter != m_mapfontInfo.end(); iter++)
	{
		plvFtInfoObj = iter->second;
		m_mapfontInfo.erase(iter);
		delete plvFtInfoObj;
	}
	m_mutex.unlock();
}

lv_ft_info_t* TViewFontUtils::getFont(char *fontName, int size, int style)
{
	string fontKeyName = "";
	lv_ft_info_t* plvFtInfoObj = NULL;

	TMutexLocker locker(&m_mutex);

	fontKeyName = _getFontMapKeyName(fontName, size, style);
	/*获取字体是否已加载，已加载时直接返回*/
	plvFtInfoObj = _getFontByKeyname(fontKeyName);
	if(plvFtInfoObj)
	{
		printf("_getFontByKeyname find:[%s]\n", fontKeyName.c_str());
		return plvFtInfoObj;
	}

	printf("_getFontByKeyname not find:[%s]\n", fontKeyName.c_str());
	/*不存在时加载新的配置*/
	char font_file[256];
	snprintf(font_file, sizeof(font_file), "%s/%s", FONT_PATH, fontName);
	if(access(font_file, F_OK) != 0)
	{
		LOG(ERROR) << "没有发现指定的字体文件:" << fontName << endl;
		return NULL;
	}

	plvFtInfoObj = _create_lv_ft_info(font_file, size, style);
	if(!plvFtInfoObj)
	{
		return NULL;
	}

	if(!_addFontToMap(fontKeyName, plvFtInfoObj))
	{
		free(plvFtInfoObj);
		return NULL;
	}

	LOG(INFO) << "创建新的字体缓存对像:" << fontKeyName.c_str() << endl;
	return plvFtInfoObj;
}

bool TViewFontUtils::releaseFont(char *fontName, int size, int style)
{
	string fontKeyName = "";

	TMutexLocker locker(&m_mutex);
	fontKeyName = _getFontMapKeyName(fontName, size, style);

	if(fontKeyName.length() == 0)
	{
		return false;
	}

	_delFontByKeyName(fontKeyName);

	return true;
}

string TViewFontUtils::_getFontMapKeyName(const char *fontName, int size, int style)
{
	char fontNameBuffer[128];
	snprintf(fontNameBuffer, sizeof(fontNameBuffer), "%s_%d_%d", fontName, size, style);

	return fontNameBuffer;
}

lv_ft_info_t* TViewFontUtils::_getFontByKeyname(string& keyName)
{
	map<string, lv_ft_info_t*>::const_iterator iter;

	iter = m_mapfontInfo.find(keyName);
	if(iter == m_mapfontInfo.end())
	{
		return NULL;
	}

	return iter->second;
}

bool TViewFontUtils::_addFontToMap(string& keyName, lv_ft_info_t* obj)
{
	m_mapfontInfo[keyName] = obj;
	return true;
}

lv_ft_info_t* TViewFontUtils::_create_lv_ft_info(const char *fontFileName, int size,
		int style)
{
	lv_ft_info_t* obj = (lv_ft_info_t*)malloc(sizeof(lv_ft_info_t));
	obj->name = fontFileName;
	obj->weight = size;
	obj->style = style;

	if(!lv_ft_font_init(obj))
	{
		free(obj);
		return NULL;
	}

	return obj;
}


void TViewFontUtils::_delFontByKeyName(string &mapkeyName)
{
	lv_ft_info_t* obj = _getFontByKeyname(mapkeyName);
	if(obj)
	{
		m_mapfontInfo.erase(mapkeyName);
		lv_ft_font_destroy(obj->font);
		free(obj);
	}
}
