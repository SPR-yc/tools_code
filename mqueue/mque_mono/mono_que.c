#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include "mono_que.h"

#define MQ_MSG_CNT 10
#define MQ_MSG_SIZE 128

static mesg_handle_func_t g_msg_handler = NULL;
static sem_t main_sem_handle;
static bool msg_loop_exit = false;
static struct mq_attr msg_attr;
static mqd_t msg_queue_id;
static pthread_t msg_thread_id = 0;
char* mono_msg_mq_name = "/mono_mesg_queue";

//example: mesg_tpe->media
//         event_type-> init, start, take pic, record, stop, deinit;
typedef struct que_mesg_info{
    //unsigned int event_id;    //event id for mesg id
    unsigned int priority;
    unsigned int data_len;
    unsigned char data[];   //mesg data trans
}que_mesg_info;


static que_mesg_info* mono_queue_gen_mesg(unsigned int priority, unsigned int data_len, void* data)
{
	que_mesg_info* que_mesg = NULL;

    if((data_len != 0) && (data != NULL))
    {
        que_mesg = (que_mesg_info *)malloc(sizeof(que_mesg_info) + data_len);
        if(que_mesg == NULL)
        {
            printf("%s-- line:%d, malloc failed!\n", __FUNCTION__, __LINE__);
            return NULL;
        }
        memcpy(que_mesg->data, data, data_len);

    }else{
		que_mesg = (que_mesg_info *)malloc(sizeof(que_mesg_info));
		if(que_mesg == NULL)
		{
			printf("%s-- line:%d   malloc failed!\n", __FUNCTION__, __LINE__);
			return NULL;
		}
	}
	que_mesg->priority = priority;
	que_mesg->data_len = data_len;

    return que_mesg;
    
}

static void mono_dispatch_msg(que_mesg_info *msg)
{
    assert(msg);

    if(g_msg_handler)
    {
        g_msg_handler(msg->data_len, msg->data_len ? msg->data : NULL);
    }else{
        printf("please registe mesg handler first!\n");
    }
		
}

void* mono_handle_msg_thread(void* args)
{
    printf("---enter into %s  \n", __FUNCTION__);
    que_mesg_info* que_mesg = NULL;
    while (!msg_loop_exit) {
        sem_wait(&main_sem_handle);   //发送mesg个数和read一一对应，使用信号量控制
        char que_data[MQ_MSG_SIZE];
        uint32_t data_len = mq_receive(msg_queue_id, que_data, sizeof(que_data), NULL);
        printf("--------------- mesg recv, mesg_p: %p, mesg_size:%d  \n\n", que_data, data_len);
        if (data_len == sizeof(que_mesg_info *)) {
            memcpy(&que_mesg, que_data, sizeof(que_mesg_info *));
            //que_mesg = (que_mesg_info*)que_data;
            printf("mqueue recv data: %p\n", que_mesg);
            if (que_mesg == NULL) {
                printf("mq_receive NULL msg!\n");
                continue;
            }
            mono_dispatch_msg(que_mesg);
            free(que_mesg);
        }
    }

}


int mono_mqueue_init(void)
{
    sem_init(&main_sem_handle, 0, 0);
    msg_thread_id = 0;
    msg_loop_exit = false;

    msg_attr.mq_maxmsg = MQ_MSG_CNT;
    msg_attr.mq_msgsize = MQ_MSG_SIZE;
    mq_unlink(mono_msg_mq_name);
    printf("mque open !\n");
    msg_queue_id = mq_open(mono_msg_mq_name, O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK, 0666, &msg_attr);//O_CREAT | O_RDWR
    if(msg_queue_id == -1){
        printf("mq_open name %s failed\n", mono_msg_mq_name);
		if (errno == EEXIST){
			//O_EXCL
			printf("mq_open name %s already exist, delete first\n", mono_msg_mq_name);
        }
        goto exit;
    }

    pthread_create(&msg_thread_id, NULL, mono_handle_msg_thread, NULL);
    //pthread_setname_np(msg_thread_id, "app_msg");
    return 0;

exit:
    return -1;
}

int mono_mqueue_deinit(void)
{
    msg_loop_exit = true;
    sem_post(&main_sem_handle);
    if(msg_thread_id){
        pthread_join(msg_thread_id, NULL);
    }
    if(msg_queue_id > 0){
        mq_close(msg_queue_id);
    }
    sem_destroy(&main_sem_handle);

    return 0;
}

int mono_mqueue_register_handle(mesg_handle_func_t handler)
{

    g_msg_handler = handler;

    return 0;
    
}

int mono_mqueue_unregister_handle(void)
{
    if(g_msg_handler != NULL)
    {
		g_msg_handler = NULL;
	}

    return 0;
}

int mono_mqueue_mesg_send(unsigned int priority, unsigned int data_len, void* data)
{
    int ret = 0;
    que_mesg_info* mesg_info_p;
    if(msg_queue_id <= 0){
        printf("open msg_queue first\n");
        return -1;
    }

    mesg_info_p = mono_queue_gen_mesg(priority, data_len, data);
    if(!mesg_info_p)
    {
        printf("priority:%d,  gen mesg failed!\n", priority);
        return -1;
    }

    printf("--------------- mesg send, mesg_p: %p, mesg_size:%ld  \n\n", &mesg_info_p, sizeof(mesg_info_p));
    if (mq_send(msg_queue_id, (char *)&mesg_info_p, sizeof(mesg_info_p), priority) == -1) {
        printf("line:%d --app_send_msg, mq_send error\n", __LINE__);
        free(mesg_info_p);
        return -1;
    }

    sem_post(&main_sem_handle);
    return 0;
}
