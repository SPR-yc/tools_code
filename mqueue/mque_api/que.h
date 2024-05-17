#ifndef __QUE_H__
#define __QUE_H__




/*
* 使用说明：
* 1、根据项目包含模块，自定义mesg_type
* 2、根据各个模块内部具体功能细分，自定义event id
* 3、主应用程序调用 mqueue_init  mqueue_deinit 创建和销毁队列
* 4、各个分模块init 函数分别实现各自的消息处理函数，其中包含各种对应event id的处理，并调用 mqueue_register_handle 函数进行注册；在 mqueue_unregister_handle 函数中注销
* 5、最后在各个工程项目中，可随时调用 mqueue_mesg_send 函数分发相应模块的具体事件进行处理。各个模块在内部处理事件，通过队列联系，模块与模块之间独立
*/

typedef enum mesg_type{   //若是单生产者消费者模式，可不用此变量(此值从非0值开始，0代表多模块模式)
    MESG_TYPE_TEST_1 = 1,   //自定义mesg type，对应各个功能模块，比如uart，media，USB，bt
    MESG_TYPE_TEST_2,
    MSG_TYPE_MAX,
}mesg_type;

/*
typedef enum event_id{  //单独模块事件处理，(也可以不定义，处理函数中只有一种场景的情况)例如：media -> take picture, record, preview  由每个模块单独定义

}event_id;
*/


typedef int (*mesg_handle_func_t)(unsigned int event_id, unsigned int data_len, void* data);

int mqueue_init(void);
int mqueue_deinit(void);
int mqueue_register_handle(unsigned int mesg_type, mesg_handle_func_t handler);  //注册对应mesg_type处理函数 (单模式此值为0)
int mqueue_unregister_handle(unsigned int mesg_type);  //单模式此值为0即可
int mqueue_mesg_send(unsigned int mesg_type, unsigned int event_id, unsigned int priority, unsigned int data_len, void* data);   //单模式mesg_type此值为0

#endif
