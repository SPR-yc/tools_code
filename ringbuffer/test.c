
#include "qibuf.h"


#define  BUF_TEST          1


#if  BUF_TEST

int main(int argc, char* argv[])
{
    QI_S32 ret = 0;
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    QI_U32 size_1;
    QI_U32 size_2;

    QIBuffer *ringbuffer_test = NULL;

    ret = QIBufInit(&ringbuffer_test, 40);
    if(ret < 0)
    {
        printf("buf test: init buf failed!\n");
        return -1;
    }

    ringbuffer_test->is_ring = 0; //用作普通buffer

    ret = QIBufGetWriteableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("buf writeable size:%d \n", ret);

    ret = QIBufGetReadableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("buf readable size:%d \n", ret);

    char test_str[512];
    sprintf(test_str, "hello everyone, welconme to shanghai!\n");
    ret = QIBufWriteData(ringbuffer_test, test_str, strlen(test_str));
    if(ret < 0)
    {
        printf("write buf error!\n");
        QIBufDeInit(ringbuffer_test);
        return -1;
    }

    ret = QIBufGetWriteableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("after write-----------------buf writeable size:%d \n", ret);
    ret = QIBufGetReadableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("buf readable size:%d \n", ret);

    //char test_read[512];
    char *test_read = (char *)malloc(512);
    ret = QIBufReadData(ringbuffer_test, test_read, strlen(test_str));
    if(ret < 0)
    {
        printf("read buf failed!\n");
        QIBufDeInit(ringbuffer_test);
        return -1;
    }
    printf("read:%d---- want read: %lu  read_data:%s\n", ret, strlen(test_str), test_read);

    ret = QIBufGetReadableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf(" after read------------buf readable size:%d \n", ret);
    ret = QIBufGetWriteableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("buf writeable size:%d \n", ret);

    QIBufWriteData(ringbuffer_test, test_str, strlen(test_str));
    QIBufReadData(ringbuffer_test, test_read, strlen(test_str));
    printf("read 2 === data:%s\n", test_read);

    ret = QIBufClr(ringbuffer_test);
    if(ret < 0)
    {
        printf("clear buf failed!\n");
    }

    ret = QIBufGetWriteableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("last  ===buf writeable size:%d \n", ret);

    ret = QIBufGetReadableSize(ringbuffer_test, &ptr1, &size_1, &ptr2, &size_2);
    printf("last ====buf readable size:%d \n", ret);

    QIBufDeInit(ringbuffer_test);

    return 0;
}

#endif

