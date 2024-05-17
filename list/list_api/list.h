#ifndef  __LIST_H_WYC__
#define __LIST_H_WYC__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


typedef struct list_node{
    void* data;
    uint32_t data_len;
    struct list_node* next;
}list_node_st;

typedef struct list_info
{
    list_node_st* next;
    list_node_st* list_head;
    list_node_st* list_tail;
    uint8_t node_count;
    pthread_mutex_t list_mutex;
}list_info_st;


int spr_list_init(list_info_st* list);
int spr_list_deinit(list_info_st* list);

//注意：添加进链表传进来的data是用户自己malloc的地址，需要用户pop链表处理完数据之后自行 free
int spr_list_add_node_head(list_info_st* list, void* data, uint32_t len);  //data
int spr_list_add_node_tail(list_info_st* list, void* data, uint32_t len);  //data
list_node_st* spr_list_pop_head_node(list_info_st* list);
int spr_list_pop_head_node_data(list_info_st* list, void** data, uint32_t* data_len);  //data
int spr_list_get_node_count(list_info_st* list);



#endif