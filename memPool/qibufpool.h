/* ============================================================================= 
 *
 *  Copyright © 2019-2024年 QiGan. All rights reserved.
 *  Description: 
 *  FilePath: /utils/qibufpool.h
 *  Author: Ethan.Luo
 *  Date: 2024-03-26 10:55:54
 *  LastEditors: Ethan.Luo
 *  LastEditTime: 2024-03-26 14:44:42
 * ============================================================================= */

#ifndef __QIBUFPOOL_H__
#define __QIBUFPOOL_H__

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
QI_S32 QIMemPoolInit(QI_VOID *pool, QI_U32 poolSize, QI_U32 blkSize);

//销毁指定内存池
QI_S32 QIMemPoolDeInit(QI_VOID *pool);


/*
* 从内存池申请指定大小的空间
* 返回值：失败返回 QI_NULL
*/
QI_VOID *QIMemPoolAlloc(QI_VOID *pool, QI_U32 memSize);

/*
* 清空内存池指定空间的数据
* 参数  pool：内存池起始地址   addrPtr：目标地址  memSize：清空 空间长度
* 返回值  成功0  失败-1
*/
QI_S32 QIMemPoolClr(QI_VOID *pool, QI_VOID* addrPtr, QI_U32 memSize);

/*
* 销毁内存
* 参数 pool：内存池起始地址  blkPtr：待销毁的内存地址  memSize：销毁的内存长度
*/
QI_S32 QIMemPoolFree(QI_VOID *pool, QI_VOID *addrPtr, QI_U32 memSize);

/*
* 获取并打印目标内存池的信息
* 包括内存池大小，内存块大小，内存池blk个数
*
*/
QI_S32 QIMemPoolGetInfo(QI_VOID *pool);

#endif  //__QIBUFPOOL_H__