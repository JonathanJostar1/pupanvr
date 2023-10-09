#ifndef __TVIEWSTYLE_H__
#define __TVIEWSTYLE_H__

#include "TViewObject.h"

class TViewStyle
{
public:
    static TViewStyle* getInstance();
    void    setDalogModelViewWindowStyle(TViewObject* viewObj);

    ViewHandle frameTitleCreater(ViewHandle parent,  int height, const char* title);
public:
    static lv_color_t  DialogFrameBgColor;
private:
    TViewStyle();
    virtual ~TViewStyle();   
private:
    static TViewStyle*  m_instance;
};

#endif