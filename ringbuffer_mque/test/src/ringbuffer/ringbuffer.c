#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ringbuffer.h"


int spr_ringBuffer_init(ringBuffer_st* p_buff, uint32_t len)
{
    int ret = -1;

    //printf("%s: buffer size: %d \n", __FUNCTION__, len);

    p_buff->data_source = malloc(len);
    if(p_buff->data_source == NULL)
    {
        printf("%s-- line:%d,  malloc mem failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    p_buff->pos_read = 0;
    p_buff->pos_write = 0;
    p_buff->buff_size = len;
    pthread_mutex_init(&p_buff->buff_mutex, NULL);

    return 0;
}

int spr_ringBuffer_deinit(ringBuffer_st* p_buff)
{
    int ret = 0;
    if(p_buff->data_source == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);

    free(p_buff->data_source);
    p_buff->pos_read = 0;
    p_buff->pos_write = 0;
    p_buff->buff_size = 0;

    pthread_mutex_unlock(&p_buff->buff_mutex);
    pthread_mutex_destroy(&p_buff->buff_mutex);

    return 0;
}

int spr_ringBuffer_clear(ringBuffer_st* p_buff)
{
    int ret = 0;
    if(p_buff->data_source == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);

    p_buff->pos_read = 0;
    p_buff->pos_write = 0;
    pthread_mutex_unlock(&p_buff->buff_mutex);

    return 0;
}

uint32_t spr_ringBuffer_get_buffer_size(ringBuffer_st* p_buff)
{
    uint32_t ret = 0;
    if(p_buff->data_source == NULL)
    {
        return 0;
    }

    return p_buff->buff_size;
}

uint32_t spr_ringBuffer_get_buffer_writeabled_size(ringBuffer_st* p_buff)
{
    if(p_buff->data_source == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);


    uint32_t pos_read = p_buff->pos_read;
    uint32_t pos_write = p_buff->pos_write;
    uint32_t tmp_len = 0;
    //printf("%s,   pos_read:%d, pos_write:%d  \n", __FUNCTION__, pos_read, pos_write);

    if(pos_read > pos_write)
    {
        tmp_len = pos_read - pos_write - 1;
    }else{
        tmp_len = pos_read + p_buff->buff_size - pos_write - 1;
    }

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return tmp_len;

}

uint32_t spr_ringBuffer_get_buffer_readable_size(ringBuffer_st* p_buff)
{
    if(p_buff->data_source == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);

    uint32_t pos_read = p_buff->pos_read;
    uint32_t pos_write = p_buff->pos_write;
    uint32_t tmp_len = 0;
    //printf("%s,   pos_read:%d, pos_write:%d  \n", __FUNCTION__, pos_read, pos_write);

    if(pos_read > pos_write)
    {
        tmp_len = pos_write + p_buff->buff_size - pos_read;
    }else{
        tmp_len = pos_write - pos_read;
    }

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return tmp_len;
}

int spr_ringBuffer_write_data(ringBuffer_st* p_buff, char* data, uint32_t data_len)
{
    int ret = 0;
    if(p_buff->data_source == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    uint32_t writeable_len = spr_ringBuffer_get_buffer_writeabled_size(p_buff);
    if(data_len > writeable_len)
    {
        printf("%s-- line:%d   writeabled_len: %d, but user want to write: %d \n", __FUNCTION__, __LINE__, writeable_len, data_len);
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);

    if(p_buff->buff_size - p_buff->pos_write >= data_len)
    {
        memcpy(p_buff->data_source + p_buff->pos_write, data, data_len);
    }else{
        uint32_t tmp_len = p_buff->buff_size - p_buff->pos_write;
        memcpy(p_buff->data_source + p_buff->pos_write, data, tmp_len);
        memcpy(p_buff->data_source, data + tmp_len, data_len - tmp_len);
    }

    p_buff->pos_write = (data_len + p_buff->pos_write)%p_buff->buff_size;

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return data_len;

}

int spr_ringBuffer_read_data(ringBuffer_st* p_buff, char* data, uint32_t data_len)
{
    int ret = 0;
    if(p_buff->data_source == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    uint32_t readable_len = spr_ringBuffer_get_buffer_readable_size(p_buff);
    if(data_len > readable_len)
    {
        printf("%s-- line:%d  readable_len: %d, but user want to read: %d \n", __FUNCTION__, __LINE__, readable_len, data_len);
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);

    if(p_buff->buff_size - p_buff->pos_read >= data_len)
    {
        memcpy(data, p_buff->data_source + p_buff->pos_read, data_len);
    }else{
        uint32_t tmp_len = p_buff->buff_size - p_buff->pos_read;
        memcpy(data, p_buff->data_source + p_buff->pos_read, tmp_len);
        memcpy(data + tmp_len, p_buff->data_source, data_len - tmp_len);
    }

    p_buff->pos_read = (data_len + p_buff->pos_read)%p_buff->buff_size;

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return data_len;
}