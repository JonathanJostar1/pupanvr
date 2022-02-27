#include "TViewObject.h"

#ifndef __TVIEWSTYLE_H__
#define __TVIEWSTYLE_H__

class TViewStyle
{
public:
    static TViewStyle* getInstance();
    void    setDalogModelViewWindowStyle(TViewObject* viewObj);
private:
    TViewStyle();
    virtual ~TViewStyle();   
private:
    static TViewStyle*  m_instance;
};

#endif