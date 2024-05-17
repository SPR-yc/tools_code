#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include "test_1.h"
#include "test_2.h"
#include "mque_api/que.h"


#define  MQUE_TEST  1


int main()
{
    int ret = 0;

    printf("queue test start ........\n");

#if MQUE_TEST
    //init
    mqueue_init();
    ret = test_1_init();
    if(ret < 0)
    {
        printf("init test_1 failed!!! \n");
        return -1;
    }

    ret = test_2_init();
    if(ret < 0)
    {
        printf("init test_2 failed!!! \n");
        return -1;
    }

    //test
    char data_str_1[128] = {0};
    sprintf(data_str_1, "%s", "test for module_1, event_1");
    mqueue_mesg_send(MESG_TYPE_TEST_1, TEST_1_EVENT_1, 0, sizeof(data_str_1), data_str_1);

    char data_str_2[128] = {0};
    sprintf(data_str_2, "%s", "test for module_1, event_2");
    mqueue_mesg_send(MESG_TYPE_TEST_1, TEST_1_EVENT_2, 0, sizeof(data_str_2), data_str_2);

    char data_str_3[128] = {0};
    sprintf(data_str_3, "%s", "test for module_2, event_1");
    mqueue_mesg_send(MESG_TYPE_TEST_2, TEST_1_EVENT_1, 0, sizeof(data_str_3), data_str_3);

    char data_str_4[128] = {0};
    sprintf(data_str_4, "%s", "test for module_2, event_2");
    mqueue_mesg_send(MESG_TYPE_TEST_2, TEST_1_EVENT_2, 0, sizeof(data_str_4), data_str_4);
#endif


    while(1)
    {
        sleep(1);
    }

    //deinit
    test_2_deinit();
    test_1_deinit();
    mqueue_deinit();

    return ret;
}
