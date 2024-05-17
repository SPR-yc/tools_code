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
#include "que.h"

#define MQ_MSG_CNT 10
#define MQ_MSG_SIZE 128

static mesg_handle_func_t msg_handlers[MSG_TYPE_MAX-1] = {0};  //多模式各模块对应数据处理函数
static mesg_handle_func_t g_msg_handler = NULL;   //单模式对应消费者数据处理函数
static sem_t main_sem_handle;
static bool msg_loop_exit = false;
static struct mq_attr msg_attr;
static mqd_t msg_queue_id;
static pthread_t msg_thread_id = 0;
char* msg_mq_name = "/mesg_queue";

//example: mesg_tpe->media
//         event_type-> init, start, take pic, record, stop, deinit;
typedef struct que_mesg_info{
    unsigned int mesg_type;   //mesg id
    unsigned int event_id;    //event id for mesg id
    unsigned int priority;
    unsigned int data_len;
    unsigned char data[];   //mesg data trans
}que_mesg_info;

que_mesg_info queue_info_t = {0};


static que_mesg_info* queue_gen_mesg(unsigned int mesg_type, unsigned int event_id, unsigned int priority, unsigned int data_len, void* data)
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
	que_mesg->mesg_type = mesg_type;
	que_mesg->event_id = event_id;
	que_mesg->priority = priority;
	que_mesg->data_len = data_len;

    return que_mesg;
    
}

static void dispatch_msg(que_mesg_info *msg)
{
    assert(msg);

    //printf("dispatch_msg type:%d id:%d\n", msg->mesg_type, msg->event_id);

    if (msg->mesg_type >= MSG_TYPE_MAX) {
        printf("dispatch_msg type:%d error!\n", msg->mesg_type);
    }else if(msg->mesg_type == 0){
		if(g_msg_handler)
		{
			g_msg_handler(msg->event_id, msg->data_len, msg->data_len ? msg->data : NULL);
		}else{
			printf("line:%d  mesg_type:%d  handler not found!\n", __LINE__, msg->mesg_type);
		}
	} else {
        if (msg_handlers[msg->mesg_type]) {
            msg_handlers[msg->mesg_type](msg->event_id, msg->data_len, msg->data_len ? msg->data : NULL);
        } else {
            printf("dispatch_msg type:%d handler not found!\n", msg->mesg_type);
        }
    }
}

void* handle_msg_thread(void* args)
{
    //printf("---enter into %s  \n", __FUNCTION__);
    que_mesg_info* que_mesg = NULL;
    while (!msg_loop_exit) {
        sem_wait(&main_sem_handle);   //发送mesg个数和read一一对应，使用信号量控制
        char que_data[MQ_MSG_SIZE];
        uint32_t data_len = mq_receive(msg_queue_id, que_data, sizeof(que_data), NULL);
        //printf("--------------- mesg recv, mesg_p: %p, mesg_size:%d  \n\n", que_data, data_len);
        if (data_len == sizeof(que_mesg_info *)) {
            memcpy(&que_mesg, que_data, sizeof(que_mesg_info *));
            if (que_mesg == NULL) {
                printf("mq_receive NULL msg!\n");
                continue;
            }
            dispatch_msg(que_mesg);
            free(que_mesg);
        }
    }

}


int mqueue_init(void)
{
    sem_init(&main_sem_handle, 0, 0);
    msg_thread_id = 0;
    msg_loop_exit = false;

    msg_attr.mq_maxmsg = MQ_MSG_CNT;
    msg_attr.mq_msgsize = MQ_MSG_SIZE;
    mq_unlink(msg_mq_name);
    printf("mque open !\n");
    msg_queue_id = mq_open(msg_mq_name, O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK, 0666, &msg_attr);//O_CREAT | O_RDWR
    if(msg_queue_id == -1){
        printf("mq_open name %s failed\n", msg_mq_name);
		if (errno == EEXIST){
			//O_EXCL
			printf("mq_open name %s already exist, delete first\n", msg_mq_name);
        }
        goto exit;
    }

    pthread_create(&msg_thread_id, NULL, handle_msg_thread, NULL);
    //pthread_setname_np(msg_thread_id, "app_msg");
    return 0;

exit:
    return -1;
}

int mqueue_deinit(void)
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

int mqueue_register_handle(unsigned int mesg_type, mesg_handle_func_t handler)
{
    //printf("%s: mesg_type:%d \n", __FUNCTION__, mesg_type);
    if(mesg_type < MSG_TYPE_MAX && mesg_type > 0)
    {
        msg_handlers[mesg_type] = handler;
    }else if(mesg_type == 0){
		g_msg_handler = handler;
	}else{
        printf("%s:mesg_type error \n", __FUNCTION__);
        return -1;
    }

    return 0;
    
}

int mqueue_unregister_handle(unsigned int mesg_type)
{
    if(mesg_type < MSG_TYPE_MAX && mesg_type > 0)
    {
        msg_handlers[mesg_type] = NULL;
    }else if(mesg_type == 0){
		g_msg_handler = NULL;
	}else{
        printf("%s: mesg_type error \n", __FUNCTION__);
        return -1;
    }

    return 0;
}

int mqueue_mesg_send(unsigned int mesg_type, unsigned int event_id, unsigned int priority, unsigned int data_len, void* data)
{
    int ret = 0;
    que_mesg_info* mesg_info_p;
    if(msg_queue_id <= 0){
        printf("open msg_queue first\n");
        return -1;
    }

    mesg_info_p = queue_gen_mesg(mesg_type, event_id, priority, data_len, data);
    if(!mesg_info_p)
    {
        printf("mesg_type:%d, event_id:%d, priority:%d,  gen mesg failed!\n", mesg_type, event_id, priority);
        return -1;
    }

   // printf("--------------- mesg send, mesg_p: %p, mesg_size:%lu  \n\n", &mesg_info_p, sizeof(mesg_info_p));
    if (mq_send(msg_queue_id, (char *)&mesg_info_p, sizeof(mesg_info_p), priority) == -1) {
        printf("app_send_msg type:%d id:%d mq_send error\n", mesg_type, event_id);
        free(mesg_info_p);
        return -1;
    }

    sem_post(&main_sem_handle);
    return 0;
}
