#ifndef __QUE_H__
#define __QUE_H__

/*
* 使用说明：
* 1、根据项目包含模块，自定义mesg_type
* 2、根据各个模块内部具体功能细分，自定义event id
* 3、主应用程序调用 mqueue_init  mqueue_deinit 创建和销毁队列
* 4、各个分模块init 函数分别实现各自的消息处理函数，其中包含各种对应event id的处理，并调用 mqueue_register_handle 函数进行注册；在 mqueue_unregister_handle 函数中注销
* 5、最后在各个工程项目中，可随时调用 mqueue_mesg_send 函数分发相应模块的具体事件进行处理。各个模块在内部处理事件，通过队列联系，模块与模块之间独立
* 
* 适用场景：适合多模块之间互相通信，各模块中，又分多种信号类型。适合做多模块的状态机处理机制
*
* 不足之处：针对不同项目实例，需要每次修改 que.h 中mesg_type 枚举项目中的模块，并且要在各模块中，定义event_id 枚举
*/

//改良：mesg_type=0: 单生产者消费者模式； mesg_type!=0:多模块模式，对应多个模块的处理函数

typedef enum mesg_type{  //这个东西不灵活，不同工程都要修改h文件，在init函数中初始化这部分（数值起始从非0开始，0代表单生产者消费者模式）
    MESG_TYPE_APP_USER = 0,   //自定义mesg type，对应各个功能模块，比如uart，media，USB，bt
    MSG_TYPE_MAX,
}mesg_type;

/*
typedef enum event_id{  //单独模块事件处理，例如：media -> take picture, record, preview  由每个模块单独定义

}event_id;
*/


typedef int (*mesg_handle_func_t)(unsigned int event_id, unsigned int data_len, void* data);

int mqueue_init(void);   //创建消息队列
int mqueue_deinit(void);  //销毁消息队列
int mqueue_register_handle(unsigned int mesg_type, mesg_handle_func_t handler);  //注册对应消息类型 mesg_type 的处理函数（单模式此值为0）
int mqueue_unregister_handle(unsigned int mesg_type);    //注销消息 mesg_type 的处理函数（单模式此值为0）

/*
* 消息发送函数
* mesg_type: 要通知的模块类型 (如果是单模式，此值传0即可)
* event_id:  发送的具体事务类型
* priority:  优先级
* data_len:  传递的数据长度  无数据传递时为0
* data: 传递的数据  无数据传递时为NULL
*
*/
int mqueue_mesg_send(unsigned int mesg_type, unsigned int event_id, unsigned int priority, unsigned int data_len, void* data);

#endif
