#include <stdio.h>
#include <unistd.h>
#include "que.h"
#include "test_2.h"



static int test_2_mesg_handle(unsigned int event_id, unsigned int data_len, void* data)
{
	//这个event_id 也可不用，视情况而定，若模块内需要多种场景处理数据，可以使用此参数
    printf("enter into %s \n", __FUNCTION__);
    int ret = 0;

	//需要先判断数据的有效性，有可能没有传递数据，这里是空的
    switch(event_id)
    {
        case TEST_2_EVENT_1:
			if(data)
			{
				printf("%s, event_1 mesg data: %s \n", __FUNCTION__, (char *)data);
			}
        break;

        case TEST_2_EVENT_2:
			if(data)
			{
				printf("%s, event_2 mesg data:%s \n", __FUNCTION__, (char *)data);
			}
        break;

        default:
            printf("%s, event_id[%d] error!\n", __FUNCTION__, event_id);
        break;
    }

    return 0;

}

int test_2_init(void)
{
    int ret = 0;
    ret = mqueue_register_handle(MESG_TYPE_TEST_2, test_2_mesg_handle);

    return 0;
}

int test_2_deinit(void)
{
    int ret = 0;
    ret = mqueue_unregister_handle(MESG_TYPE_TEST_2);

    return 0;
}
