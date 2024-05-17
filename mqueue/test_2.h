#ifndef __TEST_2_H__
#define __TEST_2_H__

typedef enum {  //单独模块事件处理，例如：media -> take picture, record, preview  由每个模块单独定义
    TEST_2_EVENT_1,
    TEST_2_EVENT_2,

}test2_event_id;


int test_2_init(void);
int test_2_deinit(void);

#endif