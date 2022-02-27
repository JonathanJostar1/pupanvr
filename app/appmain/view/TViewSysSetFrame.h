#include "TViewObject.h"

#ifndef __TVIEWSYSSETFRAME_H_
#define __TVIEWSYSSETFRAME_H_

class TViewSysSetFrame: public TViewObject
{
public:
    TViewSysSetFrame(ViewHandle parentHandle);
    ~TViewSysSetFrame();
public:
    bool viewInit();
private:

};


#endif

