#include <stdio.h>
#include <unistd.h>
#include "que.h"
#include "test_1.h"


static int test_1_mesg_handle(unsigned int event_id, unsigned int data_len, void* data)
{
    printf("enter into %s \n", __FUNCTION__);
    int ret = 0;

    switch(event_id)
    {
        case TEST_1_EVENT_1:
			if(data)
			{
				printf("%s, event_1 mesg data: %s \n", __FUNCTION__, (char *)data);
			}
        break;

        case TEST_1_EVENT_2:
			if(data)
			{
				printf("%s, event_2 mesg data: %s \n", __FUNCTION__, (char *)data);
			}
        break;

        default:
            printf("%s, event_id[%d] error!\n", __FUNCTION__, event_id);
        break;
    }

    return 0;

}

int test_1_init(void)
{
    printf("enter into %s  \n", __FUNCTION__);
    int ret = 0;
    ret = mqueue_register_handle(MESG_TYPE_TEST_1, test_1_mesg_handle);

    return 0;
}

int test_1_deinit(void)
{
    int ret = 0;
    ret = mqueue_unregister_handle(MESG_TYPE_TEST_1);

    return 0;
}
