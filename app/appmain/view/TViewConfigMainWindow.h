#ifndef TVIEWCONFIGMAINWINDOW_H
#define TVIEWCONFIGMAINWINDOW_H

#include "TViewObject.h"

/*
*一种设备窗体界面
*/

class TViewConfigMainWindow: public TViewObject
{
public:
    TViewConfigMainWindow(ViewHandle parentHandle);
    ~TViewConfigMainWindow();
private:
    void view_init();
    void view_init_top(ViewHandle handle);
    void view_init_top_menu(ViewHandle topMenuPanel);
    void view_init_bottom(ViewHandle handle);

    static void _menuitem_event_process(lv_event_t *event);
    void __menuitem_event_process(lv_event_t *event); 
    void _menuitem_clicked_process(int id);
private:
    int     m_menu_index;
};

#endif