#include "list.h"


int spr_list_init(list_info_st* list)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }

    list->list_head = NULL;
    list->list_tail = NULL;
    list->next = NULL;
    list->node_count = 0;
    pthread_mutex_init(&list->list_mutex, NULL);


    return ret;
}


int spr_list_deinit(list_info_st* list)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&list->list_mutex);

    if(list->node_count > 0)
    {
        list_node_st * node_p = list->list_head;
        while(node_p)
        {
            list->list_head = list->list_head->next;
            node_p->data = NULL;
            free(node_p);

            node_p = list->list_head;
        }

        list->list_head = NULL;
        list->list_tail = NULL;
        list->node_count = 0;
    }

    pthread_mutex_unlock(&list->list_mutex);
    pthread_mutex_destroy(&list->list_mutex);

    return ret;
}

int spr_list_add_node_tail(list_info_st* list, void* data, uint32_t len)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }

    list_node_st* new_node = (list_node_st*)malloc(sizeof(list_node_st));
    if(new_node == NULL)
    {
        printf("%s, malloc failed!\n", __FUNCTION__);
        return -1;
    }
    new_node->data = data;
    new_node->data_len = len;
    new_node->next = NULL;

    pthread_mutex_lock(&list->list_mutex);

    list_node_st* node_p = list->list_head;
    if(node_p == NULL)
    {
        list->next = new_node;
        list->list_head = new_node;
    }else{
        while(node_p)
        {
            node_p = node_p->next;
        }
        node_p = new_node;
    }

    list->list_tail = new_node;
    list->node_count++;
    pthread_mutex_unlock(&list->list_mutex);


    return ret;
}

int spr_list_add_node_head(list_info_st* list, void* data, uint32_t len)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }

    list_node_st* new_node = (list_node_st*)malloc(sizeof(list_node_st));
    if(new_node == NULL)
    {
        printf("%s, malloc failed!\n", __FUNCTION__);
        return -1;
    }
    new_node->data = data;
    new_node->data_len = len;
    new_node->next = NULL;

    pthread_mutex_lock(&list->list_mutex);

    list_node_st* node_p = list->list_head;
    if(node_p == NULL)
    {
        list->list_head = new_node;
        list->next = new_node;
    }else{
        new_node->next = list->list_head;
        list->list_head = new_node;
        list->next = new_node;
    }

    list->node_count++;
    pthread_mutex_unlock(&list->list_mutex);


    return ret;
}

list_node_st* spr_list_pop_head_node(list_info_st* list)
{
    if(list == NULL || list->list_head == NULL || list->node_count == 0)
    {
        return NULL;
    }

    pthread_mutex_lock(&list->list_mutex);
    list_node_st* node_p = list->list_head;
    list->list_head = list->list_head->next;
    list->node_count--;

    pthread_mutex_unlock(&list->list_mutex);

    return node_p;
}

int spr_list_pop_head_node_data(list_info_st* list, void** data, uint32_t* data_len)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }
    printf("node_count = %d\n", list->node_count);

    if(list->list_head == NULL || list->node_count == 0)
    {
        if(data)
        {
            *data = NULL;
        }
        if(data_len)
        {
            *data_len = 0;
        }

        return -1;
    }

    pthread_mutex_lock(&list->list_mutex);

    list_node_st* node_p = list->list_head;
    list->list_head = list->list_head->next;
    if(list->list_head == NULL)
    {
        list->list_tail = NULL;
    }


    if(data)
    {
        *data = node_p->data;
    }
    if(data_len)
    {
        *data_len = node_p->data_len;
    }

    node_p->data = NULL;
    node_p->next = NULL;
    node_p->data_len = 0;
    free(node_p);

    if(list->node_count)
    {
        list->node_count--;
    }
    pthread_mutex_unlock(&list->list_mutex);

    return 0;
}


int spr_list_get_node_count(list_info_st* list)
{
    int ret = 0;
    if(list == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&list->list_mutex);
    ret = list->node_count;
    pthread_mutex_unlock(&list->list_mutex);

    return ret;
}
