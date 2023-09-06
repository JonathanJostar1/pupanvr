#ifndef __TVIEWSYSSETFRAME_H_
#define __TVIEWSYSSETFRAME_H_

#include "TViewObject.h"

/*
 * 系统配置界面
 * */
class TViewSysSetFrame: public TViewObject
{
public:
    TViewSysSetFrame(ViewHandle parentHandle);
    ~TViewSysSetFrame();
public:
    bool viewInit();
    void viewMenuItemInit();
private:
    static void _menuitem_event_process(lv_event_t *event);
    void __menuitem_event_process(lv_event_t *event); 
    void _menuitem_clicked_process(int id);
private:

};


#endif

