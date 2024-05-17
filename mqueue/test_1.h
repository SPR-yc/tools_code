#ifndef __TEST_1_H__
#define __TEST_1_H__

typedef enum {  //单独模块事件处理，例如：media -> take picture, record, preview  由每个模块单独定义
    TEST_1_EVENT_1,
    TEST_1_EVENT_2,

}test1_event_id;

int test_1_init(void);
int test_1_deinit(void);

#endif




