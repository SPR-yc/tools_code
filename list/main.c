#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include "list_api/list.h"


#define  LIST_TEST  1


int main()
{
    int ret = 0;

    printf("queue test start ........\n");

#if LIST_TEST

    printf("test for list  start!!\n");
    list_info_st *main_list = NULL;
    spr_list_init(&main_list);

    char* test_data_1 = (char*)malloc(512);
    memset(test_data_1, 0, 512);
    sprintf(test_data_1, "%s", "hello, test for list! [1]");
    spr_list_add_node_head(main_list, (void*)test_data_1, strlen(test_data_1));

    char* test_data_2 = (char*)malloc(512);
    memset(test_data_2, 0, 512);
    sprintf(test_data_2, "%s", "hello, test for list! [2]");
    spr_list_add_node_head(main_list, (void*)test_data_2, strlen(test_data_2));

    char* test_data_3 = (char*)malloc(512);   //user free mem after using
    memset(test_data_3, 0, 512);
    sprintf(test_data_3, "%s", "hello, test for list! [3]");
    spr_list_add_node_head(main_list, (void*)test_data_3, strlen(test_data_3));



    int count = main_list->node_count;
    int i = 0;
    for(i = 0; i < count; i++)
    {
        void* pop_data = NULL;
        uint32_t data_len = 0;
        spr_list_pop_head_node_data(main_list, (void**)&pop_data, &data_len);
        printf("node[%d]   pop_data:%s  data_len:%d \n", i, (char*)pop_data, data_len);
        free(pop_data);   //free node data
    }

    spr_list_deinit(main_list);

#endif

    while(1)
    {
        sleep(1);
    }


    return ret;
}
