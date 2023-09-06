#ifndef __FY_TYPE_H__
#define __FY_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           FY_U8;
typedef unsigned short          FY_U16;
typedef unsigned int            FY_U32;
typedef double                  FY_DOUBLE;
typedef signed char             FY_S8;
typedef short                   FY_S16;
typedef int                     FY_S32;

#ifndef _M_IX86
    typedef unsigned long long  FY_U64;
    typedef long long           FY_S64;
#else
    typedef __int64             FY_U64;
    typedef __int64             FY_S64;
#endif

typedef char                    FY_CHAR;
#define FY_VOID                 void

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    FY_FALSE = 0,
    FY_TRUE  = 1,
} FY_BOOL;

#ifndef NULL
    #define NULL    0L
#endif

#define FY_NULL     0L
#define FY_SUCCESS  0
#define FY_FAILURE  (-1)


#define FY_U8_MAX		((FY_U8)~0U)
#define FY_S8_MAX		((FY_S8)(FY_U8_MAX>>1))
#define FY_S8_MIN		((FY_S8)(-FY_S8_MAX - 1))
#define FY_U16_MAX		((FY_U16)~0U)
#define FY_S16_MAX		((FY_S16)(FY_U16_MAX>>1))
#define FY_S16_MIN		((FY_S16)(-FY_S16_MAX - 1))
#define FY_U32_MAX		((FY_U32)~0U)
#define FY_S32_MAX		((FY_S32)(FY_U32_MAX>>1))
#define FY_S32_MIN		((FY_S32)(-FY_S32_MAX - 1))
#define FY_U64_MAX		((FY_U64)~0ULL)
#define FY_S64_MAX		((FY_S64)(FY_U64_MAX>>1))
#define FY_S64_MIN		((FY_S64)(-FY_S64_MAX - 1))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FY_TYPE_H__ */

