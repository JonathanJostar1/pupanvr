#ifndef __TVIEWPLAYTOOLBAR_H__
#define __TVIEWPLAYTOOLBAR_H__

#include "TViewObject.h"

/*播放视窗的一个快捷操作菜单*/

class TViewPlayToolBar: public TViewObject
{
public:
    TViewPlayToolBar(ViewHandle parentHandle);
    virtual ~TViewPlayToolBar();
private:
    static void	_btn_event_process(lv_event_t * event);
private:

};

#endif