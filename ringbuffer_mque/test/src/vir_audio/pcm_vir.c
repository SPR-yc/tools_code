
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pcm_vir.h"
#include "que.h"


#define  PCM_PERIOD_SIZE   1024
#define  PCM_PERIOD_CNT    4

int pcm_fd;
PCM_CONFIG_ST g_pcm_config;
unsigned long period_len = 0;      //周期长度  byte
unsigned int frame_time = 0;      //产生一帧数据所需时长，模拟录音时长 (一帧时长= 一帧样本个数 / 采样率)
pthread_t pcm_thread_id;
bool thread_exit = false;
char *pcm_data_r = NULL;


//线程处理函数，读取pcm数据，存入buffer
void* pcm_read_thread(void* args)
{
    int ret = 0;
    int r_len = 0;
    //printf("pcm_read_thread start !!! \n");

    while(!thread_exit)
    {
        memset(pcm_data_r, 0, period_len);  //每次清空buffer

        r_len = read(pcm_fd, pcm_data_r, period_len);    //注意文件读到末尾情况
        if(r_len < period_len && r_len != -1){
            printf("pcm_read_thread: read pcm file not right, want:%ld, fact: %d \n", period_len, r_len);
            //lseek(pcm_fd, 44, SEEK_SET);   //文件读到末尾，之后，重新从头开始读，循环读(wav 文件头信息44字节)
            read(pcm_fd, pcm_data_r + r_len, period_len - r_len);
        }else if(r_len == -1)
        {
            printf("%s: read pcm_fd error!\n", __FUNCTION__);
            break;
        }
        
        //msg mque 发送消息
        mqueue_mesg_send(0, 0, 0, period_len, (void *)pcm_data_r);

        usleep(PCM_PERIOD_SIZE * frame_time);  //延时等待消费者读取消息队列 处理数据 模拟采样时间
    }
}

//设置并保存采样信息，计算buffer长度
int pcm_vir_set_config(int rate, int bit_width, int chn_num)
{
    int ret = 0;

    g_pcm_config.sample_rate = rate;
    g_pcm_config.sample_bit = bit_width;
    g_pcm_config.chn_num = chn_num;

    char pcm_file[128] = {0};    //根据配置信息找对应pcm文件 格式：pcm_[采样率]_[采样位宽]_[通道数].wav
    sprintf(pcm_file, "/mnt/extsd/pcm_file/pcm_%d_%d_%d.wav", g_pcm_config.sample_rate, g_pcm_config.sample_bit, g_pcm_config.chn_num);
    printf("pcm file str:%s \n", pcm_file);
    pcm_fd = open(pcm_file, O_RDWR);
    if(pcm_fd < 0)
    {
        printf("open pcm file[%s] failed!\n", pcm_file);
        return -1;
    }
    //lseek(pcm_fd, 44, SEEK_SET); //获取pcm数据时 避开wav头信息

    period_len = PCM_PERIOD_SIZE * g_pcm_config.sample_bit * g_pcm_config.chn_num /8;  //byte
    frame_time = 1 * 1000000 / g_pcm_config.sample_rate;   //us

    return ret;
}

//创建buffer，创建消息队列
int pcm_vir_prepare()
{
    int ret = 0;

    //申请一个周期大小空间，读取pcm文件
    pcm_data_r = malloc(period_len);
    if(pcm_data_r == NULL)
    {
        printf("malloc pcm_data_r failed!\n");
        ret = -1;
    }

    return ret;
}


//创建线程，读取数据送入buffer
int pcm_vir_start()         //准备就绪，可以获取数据
{
    int ret = 0;
    thread_exit = false;

    ret = pthread_create(&pcm_thread_id, NULL, pcm_read_thread, NULL);
    if(ret != 0)
    {
        printf("pcm_vir_star create thread failed!\n");
        return -1;
    }

    return 0;
}

//停止线程，释放buffer
int pcm_vir_stop()           //停止获取数据，释放一些资源
{
    int ret = 0;

    thread_exit = true;

    if(pcm_thread_id)
    {
        pthread_join(pcm_thread_id, NULL);
    }

    if(pcm_data_r != NULL)
    {
        free(pcm_data_r);
    }  

    if(pcm_fd > 0)
    {
        close(pcm_fd);
    }
    


    return ret;
}