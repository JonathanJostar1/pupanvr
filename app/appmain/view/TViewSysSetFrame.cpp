#include "TViewSysSetFrame.h"
#include "TViewStyle.h"

TViewSysSetFrame::TViewSysSetFrame(ViewHandle parentHandle): TViewObject(parentHandle)
{
    viewInit();
    
}

TViewSysSetFrame::~TViewSysSetFrame()
{

}

bool TViewSysSetFrame::viewInit()
{
    setViewSize(800, 600);
	lv_obj_center(m_viewHandle);

    TViewStyle::getInstance()->setDalogModelViewWindowStyle(this);
    return true;
}