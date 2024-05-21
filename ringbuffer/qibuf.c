
#include "qibuf.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



QI_S32 QIBufInit(QIBuffer **p_buff, QI_U32 len)
{
    int ret = -1;

    if(p_buff == NULL || len == 0)
    {
        return -1;
    }
    //QIBuffer *tmp_buff = *p_buff;

    *p_buff = (QIBuffer *)malloc(sizeof(QIBuffer));
    if(*p_buff == NULL)
    {
        printf("%s: malloc faile!\n", __FUNCTION__);
        return -1;
    }

    (*p_buff)->data_source = (QI_CHAR *)malloc(len);
    if((*p_buff)->data_source == NULL)
    {
        printf("%s-- line:%d,  malloc mem failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset((*p_buff)->data_source, 0, len);
    (*p_buff)->pos_read = 0;
    (*p_buff)->pos_write = 0;
    (*p_buff)->buff_size = len;
    (*p_buff)->is_ring = 1;
    pthread_mutex_init(&(*p_buff)->buff_mutex, NULL);
    printf("%s  exit \n", __FUNCTION__);

    return 0;
}

QI_S32 QIBufDeInit(QIBuffer *p_buff)
{
    int ret = 0;
    if(p_buff == NULL || p_buff->data_source == NULL)
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

    free(p_buff);

    return 0;
}

QI_S32 QIBufClr(QIBuffer *p_buff)
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

QI_U32 QIBufGetBufSize(QIBuffer *p_buff)
{
    QI_U32 ret = 0;
    if(p_buff->data_source == NULL)
    {
        return 0;
    }

    return p_buff->buff_size;
}

// QI_U32 QIBufGetWriteableSize(QIBuffer *p_buff)
// {

//     pthread_mutex_lock(&p_buff->buff_mutex);


//     QI_U32 pos_read = p_buff->pos_read;
//     QI_U32 pos_write = p_buff->pos_write;
//     QI_U32 tmp_len = 0;

//     if(p_buff->is_ring)
//     {
//         if(pos_read > pos_write)
//         {
//             tmp_len = pos_read - pos_write -1;
//         }else{
//             tmp_len = pos_read + p_buff->buff_size - pos_write -1;
//         }
//     }else{
//         tmp_len = p_buff->buff_size - pos_write;
//     }

//     pthread_mutex_unlock(&p_buff->buff_mutex);

//     return tmp_len;

// }

QI_U32 QIBufGetWriteableSize(QIBuffer *p_buff, char **ptr1, QI_U32 *size_1, char **ptr2, QI_U32 *size_2)
{
    if(p_buff == NULL || p_buff->data_source == NULL || ptr1 == NULL || size_1 == NULL || ptr2 == NULL || size_2 == NULL)
    {
        printf("%s,  error!\n", __FUNCTION__);
        return 0;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);
    QI_U32 pos_read = p_buff->pos_read;
    QI_U32 pos_write = p_buff->pos_write;
    QI_U32 tmp_len = 0;

    if(p_buff->is_ring)
    {
        if(pos_read > pos_write)
        {
            *size_1 = pos_read - pos_write ;
            *ptr1 = p_buff->data_source + pos_write;
            *ptr2 = NULL;
            *size_2 = 0;
        }else{
            *size_1 = p_buff->buff_size - pos_write ;
            *ptr1 = p_buff->data_source + pos_write;
            *size_2 = pos_read;  //防止pos_read = pos_write,这样创建buf之后第一次read会出错
            *ptr2 = p_buff->data_source;
            //tmp_len = pos_read + p_buff->buff_size - pos_write -1;
        }
    }else{
        //tmp_len = p_buff->buff_size - pos_write;
        *ptr1 = p_buff->data_source + pos_write;
        *size_1 = p_buff->buff_size - pos_write;
        *ptr2 = NULL;
        *size_2 = 0;
    }

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return *size_1 + *size_2;
}

// QI_U32 QIBufGetReadableSize(QIBuffer *p_buff)
// {
//     if(p_buff == NULL || p_buff->data_source == NULL)
//     {
//         return 0;
//     }

//     pthread_mutex_lock(&p_buff->buff_mutex);

//     QI_U32 pos_read = p_buff->pos_read;
//     QI_U32 pos_write = p_buff->pos_write;
//     QI_U32 tmp_len = 0;

//     if(p_buff->is_ring)
//     {
//         if(pos_read > pos_write)
//         {
//             tmp_len = pos_write + p_buff->buff_size - pos_read;
//         }else{
//             tmp_len = pos_write - pos_read;
//         }
//     }else{
//         tmp_len = pos_write - pos_read;
//     }
    

//     pthread_mutex_unlock(&p_buff->buff_mutex);

//     return tmp_len;
// }

QI_U32 QIBufGetReadableSize(QIBuffer *p_buff, char **ptr1, QI_U32 *size_1, char **ptr2, QI_U32 *size_2)
{

    if(p_buff == NULL || p_buff->data_source == NULL || ptr1 == NULL || size_1 == NULL || ptr2 == NULL || size_2 == NULL)
    {
        printf("%s,  error!\n", __FUNCTION__);
        return 0;
    }
    pthread_mutex_lock(&p_buff->buff_mutex);

    QI_U32 pos_read = p_buff->pos_read;
    QI_U32 pos_write = p_buff->pos_write;
    QI_U32 tmp_len = 0;

    if(p_buff->is_ring)
    {
        if(pos_read > pos_write)
        {
            *ptr1 = p_buff->data_source + pos_read;
            *size_1 = p_buff->buff_size - pos_read;
            *ptr2 = p_buff->data_source;
            *size_2 = pos_write;
            //tmp_len = pos_write + p_buff->buff_size - pos_read;
        }else{
            //tmp_len = pos_write - pos_read;
            *ptr1 = p_buff->data_source + pos_read;
            *size_1 = pos_write - pos_read;
            *ptr2 = NULL;
            *size_2 = 0;
        }
    }else{
        //tmp_len = pos_write - pos_read;
        *ptr1 = p_buff->data_source + pos_read;
        *size_1 = pos_write - pos_read;
        *ptr2 = NULL;
        *size_2 = 0;
    }
    

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return *size_1 + *size_2;
}

QI_S32 QIBufWriteData(QIBuffer *p_buff, QI_CHAR *data, QI_U32 data_len)
{
    int ret = 0;
    if(p_buff == NULL || p_buff->data_source == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    // QI_U32 writeable_len = QIBufGetWriteableSize(p_buff);
    // if(data_len > writeable_len)
    // {
    //     printf("%s-- line:%d   writeabled_len: %d, but user want to write: %d \n", __FUNCTION__, __LINE__, writeable_len, data_len);
    //     return -1;
    // }

    // pthread_mutex_lock(&p_buff->buff_mutex);

    // if(p_buff->buff_size - p_buff->pos_write >= data_len)  //修改
    // {
    //     memcpy(p_buff->data_source + p_buff->pos_write, data, data_len);
    // }else{
    //     QI_U32 tmp_len = p_buff->buff_size - p_buff->pos_write;
    //     memcpy(p_buff->data_source + p_buff->pos_write, data, tmp_len);
    //     memcpy(p_buff->data_source, data + tmp_len, data_len - tmp_len);
    // }

    char *ptr1 = NULL;
    char *ptr2 = NULL;
    QI_U32 size_1;
    QI_U32 size_2;
    QI_U32 writeable_len = QIBufGetWriteableSize(p_buff, &ptr1, &size_1, &ptr2, &size_2);
    if(data_len > writeable_len)
    {
        printf("%s-- line:%d   writeabled_len: %d, but user want to write: %d \n", __FUNCTION__, __LINE__, writeable_len, data_len);
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);
    if(ptr2 == NULL) //说明可以直接copy
    {
        memcpy(ptr1, data, data_len);
    }else{
        memcpy(ptr1, data, size_1);
        memcpy(ptr2, data+size_1, size_2);
    }

    p_buff->pos_write = (data_len + p_buff->pos_write)%p_buff->buff_size;

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return data_len;

}

QI_S32 QIBufReadData(QIBuffer *p_buff, QI_CHAR *data, QI_U32 data_len)
{
    int ret = 0;
    if(p_buff->data_source == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    // QI_U32 readable_len = QIBufGetReadableSize(p_buff);
    // if(data_len > readable_len)
    // {
    //     printf("%s-- line:%d  readable_len: %d, but user want to read: %d \n", __FUNCTION__, __LINE__, readable_len, data_len);
    //     return -1;
    // }

    // pthread_mutex_lock(&p_buff->buff_mutex);

    // if(p_buff->buff_size - p_buff->pos_read >= data_len)
    // {
    //     memcpy(data, p_buff->data_source + p_buff->pos_read, data_len);
    // }else{
    //     QI_U32 tmp_len = p_buff->buff_size - p_buff->pos_read;
    //     memcpy(data, p_buff->data_source + p_buff->pos_read, tmp_len);
    //     memcpy(data + tmp_len, p_buff->data_source, data_len - tmp_len);
    // }

    char *ptr1 = NULL;
    char *ptr2 = NULL;
    QI_U32 size_1;
    QI_U32 size_2;
    QI_U32 readable_len = QIBufGetReadableSize(p_buff, &ptr1, &size_1, &ptr2, &size_2);
    if(data_len > readable_len)
    {
        printf("%s-- line:%d  readable_len: %d, but user want to read: %d \n", __FUNCTION__, __LINE__, readable_len, data_len);
        return -1;
    }

    pthread_mutex_lock(&p_buff->buff_mutex);
    if(ptr2 == NULL)
    {
        memcpy(data, ptr1, data_len);
    }else{
        memcpy(data, ptr1, size_1);
        memcpy(data + size_1, ptr2, size_2);
    }

    p_buff->pos_read = (data_len + p_buff->pos_read)%p_buff->buff_size;

    pthread_mutex_unlock(&p_buff->buff_mutex);

    return data_len;
}
