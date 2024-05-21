/* ============================================================================= 
 *
 *  Copyright © 2019-2024年 QiGan. All rights reserved.
 *  Description: 
 *  FilePath: /qapi_mmp_typedef.h
 *  Author: Ethan.Luo
 *  Date: 2024-03-26 13:32:08
 *  LastEditors: Ethan.Luo
 *  LastEditTime: 2024-03-26 14:48:15
 * ============================================================================= */
#ifndef __QAPI_MMP_TYPEDEF_H__
#define __QAPI_MMP_TYPEDEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char       QI_U8;      /* 8 bit unsigned integer  */
typedef unsigned short      QI_U16;     /* 16 bit unsigned integer */
typedef unsigned int        QI_U32;     /* 32 bit unsigned integer */
typedef unsigned long long  QI_U64;     /* 64 bit unsigned integer */
typedef float               QI_FLOAT32; /* 32 bit floating point   */

typedef signed char         QI_S8;      /* 8 bit signed integer    */
typedef short               QI_S16;     /* 16 bit signed integer   */
typedef int                 QI_S32;     /* 32 bit signed integer   */
typedef long long           QI_S64;     /* 64 bit signed integer   */
typedef double              QI_FLOAT64; /* 64 bit floating point   */

typedef char                QI_CHAR;

#define	QBIT(x)				(QI_U32)(1<<(x))
#define	QBIT64(x)			(((QI_U64)1)<<(x))

#define QBIT_SET(v, b)      (v) = (v) | QBIT(b)
#define QBIT_SET64(v, b)    (v) = (v) | QBIT64(b)

#define	QBIT_CLR(v, b)      (v) = (v) & (~QBIT(b) )
#define	QBIT_CLR64(v, b)	(v) = (v) & (~QBIT64(b) )

#define	QBIT_ISSET(v, b)    ((v)&QBIT(b))
#define QBIT_ISSET64(v, b)  ((v)&QBIT64(b))

#define	QBIT_ISCLR(v, b)    (!QBIT_ISSET(v, b))
#define	QBIT_ISCLR64(v, b)  (!QBIT_ISSET64(v, b))

#ifndef QIMAX
#define QIMAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef QIMIN
#define QIMIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef QIABS
#define QIABS(x) ((x)>=0?(x):-(x))
#endif

#define	QIMalloc                malloc 
#define	QICalloc                calloc
#define	QIFree(_p_)             do{ if(_p_){ free(_p_); (_p_)=NULL;}}while(0)
#define	QIMallocT(_T_)          (_T_*)QIMalloc(sizeof(_T_))
#define	QICallocT(_T_, _num_)   (_T_*)QICalloc( _num_, sizeof(_T_))
#define	QIDel(_obj_)            do{ if(_obj_) { delete (_obj_); (_obj_) = NULL;} } while(0)

typedef enum {
    QI_FALSE = 0,
    QI_TRUE  = 1,
} QI_BOOL;

#ifndef NULL
    #define NULL    0L
#endif
#define QI_VOID     void
#define QI_NULL     0L
#define QI_SUCCESS  0
#define QI_FAILURE  (-1)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif