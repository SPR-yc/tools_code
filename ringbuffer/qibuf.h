/* ============================================================================= 
 *
 *  Copyright © 2019-2024年 QiGan. All rights reserved.
 *  Description: 
 *  FilePath: /utils/qibuf.h
 *  Author: Ethan.Luo
 *  Date: 2024-03-26 10:55:51
 *  LastEditors: Ethan.Luo
 *  LastEditTime: 2024-03-26 14:44:32
 * ============================================================================= */

#ifndef  __QIBUF_H__
#define __QIBUF_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include "qapi_mmp_typedef.h"


typedef struct _ringBuffer{
    QI_CHAR *data_source;   //buffer起始地址
    QI_U32 buff_size;  //ringbuffer 大小
    QI_U32 pos_read;   //读位置
    QI_U32 pos_write;  //写位置
    pthread_mutex_t buff_mutex;   
    QI_CHAR is_ring;   //ringbuffer标志位  1：本buffer用作ringbuffer   0：本buffer用作普通buffer使用
}QIBuffer;


/*
* 创建并初始化一个长度为len的ringbuffer
* 参数 p_buff:buffer结构体地址   len:buffer长度 
* 返回值  成功0   失败 -1
*/
QI_S32 QIBufInit(QIBuffer **p_buff, QI_U32 len);   

/*
* 销毁一个ringbuffer
* 参数 p_buff：buffer结构体地址
*/
QI_S32 QIBufDeInit(QIBuffer *p_buff);

/*
* 清空ringbuffer
* 参数  p_buff: buffer结构体地址
* 返回值  成功0  失败-1
*/
QI_S32 QIBufClr(QIBuffer *p_buff);

/*
* 获取buffer长度
*
*/
//QI_U32 QIBufGetBufSize(QIBuffer *p_buff);

/*
* 获取buffer可写空间大小
* 
*/
//QI_U32 QIBufGetWriteableSize(QIBuffer *p_buff);

/*
* 获取buffer可写空间和具体信息
* 参数：  p_buffer:目标buffer指针  ptr1:接收第一段可读空间起始地址  size_1:接收第一段可读空间长度
                                  ptr2:接收第二段可读空间起始地址  size_2:接收第二段可读空间长度
* 返回值： 返回buffer可读空间长度  0包含出错情况和实际可读长度为0 的情况
* 注意：用作ringbuffer时，若可写长度没有经过buffer尾部回环到头部，这时第二段空间不存在，ptr2=NULL, size_2=0
* 用户调用此接口获取可读空间长度，保证传参正确，只关注返回值即可
*/
QI_U32 QIBufGetWriteableSize(QIBuffer *p_buff, char **ptr1, QI_U32 *size_1, char **ptr2, QI_U32 *size_2);


/*
*   获取buffer可读大小
*/
//QI_U32 QIBufGetReadableSize(QIBuffer *p_buff);

/*
* 获取buffer可读空间和具体信息 (如果用作ringbuffer，可读空间包含buffer尾部以及回环的buffer头部的话，这时会返回两部分空间的位置信息和长度信息)
* 参数 p_buff:目标buffer指针  ptr1:用来接收第一段空间起始位置  size_1:用来接收第一段空间长度
*                            ptr2:用来接收第二段空间起始位置  size_2:用来接收第二段空间长度
* 返回值  返回buffer可读区域长度  0包含出错情况，也包括实际可读长度为0 的情况
* 注意：用作ringbuffer的时候，若可读区域没有经过buffer回环，整个可读区间为一段，这时 ptr2=NULL, size_2=0;
*       若可读区域经过回环，则两段长度信息分别传回。  返回值等于 size_1 + size_2
* 用户调用此接口简单获取可读数值，保证传参正确，只关注返回值即可。
*/
QI_U32 QIBufGetReadableSize(QIBuffer *p_buff, char **ptr1, QI_U32 *size_1, char **ptr2, QI_U32 *size_2);

/*
* 向buffer中写入数据
* 参数 p_buff：buffer结构体指针  data:要写入的数据   data_len:写入数据长度
* 返回值  成功：返回写入的data长度   失败：-1
*/
QI_S32 QIBufWriteData(QIBuffer *p_buff, QI_CHAR *data, QI_U32 data_len);


/*
* 从buffer中读出数据
* 参数  p_buff:buffer结构体指针   data:接收数据的指针   data_len:要读出的数据长度
* 返回值  成功：返回读出的数据长度  失败：-1
*/
QI_S32 QIBufReadData(QIBuffer *p_buff, QI_CHAR *data, QI_U32 data_len);


#endif  //__QIBUF_H__
