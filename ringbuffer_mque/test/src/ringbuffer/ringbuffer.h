#ifndef __SPR_RINGBUFFER_H__
#define __SPR_RINGBUFFER_H__

#include<stdint.h>
#include<pthread.h>

typedef struct ringBuffer_st{
    char* data_source;  //数据指针
    uint32_t buff_size;  //ringbuffer大小
    uint32_t pos_read;   //buffer读指针位置
    uint32_t pos_write;  //buffer写指针位置
    pthread_mutex_t buff_mutex;   //同步锁
}ringBuffer_st;

//出错返回值全为-1
int spr_ringBuffer_init(ringBuffer_st* p_buff, uint32_t len);   //创建ringbuffer，len:创建buffer的大小
int spr_ringBuffer_deinit(ringBuffer_st* p_buff);   //销毁指定ringbuffer
int spr_ringBuffer_clear(ringBuffer_st* p_buff);    //清除buffer
uint32_t spr_ringBuffer_get_buffer_size(ringBuffer_st* p_buff);   //获取buffer大小
uint32_t spr_ringBuffer_get_buffer_writeabled_size(ringBuffer_st* p_buff);   //获取buffer可写空间
uint32_t spr_ringBuffer_get_buffer_readable_size(ringBuffer_st* p_buff);   //获取buffer可读空间
int spr_ringBuffer_write_data(ringBuffer_st* p_buff, char* data, uint32_t data_len);   //向buffer写入指定长度数据
int spr_ringBuffer_read_data(ringBuffer_st* p_buff, char* data, uint32_t data_len);    //向buffer读出指定长度数据


#endif   //__SPR_RINGBUFFER_H__