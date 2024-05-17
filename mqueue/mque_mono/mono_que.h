#ifndef __MONO_QUE_H__
#define __MONO_QUE_H__




/*
* 使用说明：
* 此消息队列应用于单生产者消费者模式
* 
* 1、主应用程序调用 mono_mqueue_init  mono_mqueue_deinit 创建和销毁队列
* 2、生产者模块实现消息处理函数，并调用 mono_mqueue_register_handle 函数进行注册；在 mono_mqueue_unregister_handle 函数中注销
* 3、生产者调用 mono_mqueue_mesg_send 函数发送消息。消费者接收消息进行处理
*/


typedef int (*mesg_handle_func_t)(unsigned int data_len, void* data);

int mono_mqueue_init(void);
int mono_mqueue_deinit(void);
int mono_mqueue_register_handle(mesg_handle_func_t handler);  //注册对应mesg_type处理函数 
int mono_mqueue_unregister_handle(void);
int mono_mqueue_mesg_send(unsigned int priority, unsigned int data_len, void* data);  

#endif
