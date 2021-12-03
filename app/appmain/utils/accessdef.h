#ifndef __ASSERTDEF_H__
#define __ASSERTDEF_H__

#include "tlog.h"
#include<assert.h>

#define ASSERT(a) \
    do{\
        if(!(a)) \
        { \
            LOG(INFO) << "ASSERT FAILURE! file: " << __FILE__ << " function:"  << __FUNCTION__ << " LINE:" <<  __LINE__;\
            assert(0); \
        } \
    }while(0)

#endif // __ASSERTDEF_H__