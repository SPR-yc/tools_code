

#ifndef __QIBUFPOOL_STATIC_H__
#define  __QIBUFPOOL_STATIC_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "qapi_mmp_typedef.h"


/*
* 初始化内存池，并将内存池按指定的粒度(blkSize)分割成大小相同的块
* 参数 pool：内存池起始地址   poolSize:内存池大小(byte)    blkSize：内存池块大小(byte)
* 返回值  成功 0     失败 -1
* 注： 第三个参数blkSize是用来将内存池分成大小相同的块，由用户自定义的块大小。
*/
QI_S32 QIStaMemPoolInit(QI_VOID *pool, QI_U32 poolSize, QI_U32 blkSize);

//销毁指定内存池
QI_S32 QIStaMemPoolDeInit(QI_VOID *pool);


/*
* 从内存池申请指定大小的空间
* 返回值：失败返回 QI_NULL
*/
QI_VOID *QIStaMemAlloc(QI_VOID *pool);

/*
* 清空内存池指定地址的内存块数据
* 参数 pool：内存池地址   addrPtr：目标内存块的地址
*/
QI_S32 QIStaMemClr(QI_VOID *pool, QI_VOID *addrPtr);

/*
* 销毁内存
* 参数 pool：内存池起始地址  blkPtr：待销毁的内存地址  memSize：销毁的内存长度
*/
QI_S32 QIStaMemFree(QI_VOID *pool, QI_VOID *addrPtr);

//打印内存池信息
QI_S32 QIStaMemGetInfo(QI_VOID *pool);

#endif   //__QIBUFPOOL_STATIC_H__