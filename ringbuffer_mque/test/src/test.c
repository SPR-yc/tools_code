#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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
#include "pcm_vir.h"
#include <mqueue.h>
#include <pthread.h>
#include "que.h"

#define PERIOD_SIZE     1024
#define SAMPLE_RATE     16000
#define BIT_PER_SAMPLE  16
#define CHN_NUM         2

#define  PCM_SAVE_FILE  "/mnt/extsd/test_capture.wav"

int save_fd = 0;
int total_pcm_size = 0;
int test_sec = 0;
int sample_rate = 0;
int sample_bit = 0;
int chn_num = 0;
//int period_size = 0;
unsigned long pcm_read_len = 0;
static mqd_t msg_mque_id;
pthread_t mque_thread_id;
char exit_flg = 0;


void printf_usage(char* argv[])
{
	printf("test for read pcm data,  usage:%s sample_rate sample_bit chn_num second \n",argv[0]);
	printf("-----sample_rate: 16000 44100 48000 96000\n");
	printf("-----sample_bit: 16 24 32 \n");
	printf("-----chn_num: 1 2 \n\n\n");
}

int mq_data_handle_func(unsigned int event_id, unsigned int data_len, void* data)
{
	int ret = 0;

	if(data)
	{
		ret = write(save_fd, (char *)data, data_len);
		if(ret < 0)
		{
			printf("%s--line:%d   write failed!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		pcm_read_len += ret;
	}

	return ret;
}

int main(int argc, char* argv[])
{

	printf_usage(argv);
	int ret;

	if(argc < 5)
	{
		sample_rate = SAMPLE_RATE;
		sample_bit = BIT_PER_SAMPLE;
		chn_num = CHN_NUM;
		test_sec = 5;  //default
	}else if(argc == 5)
	{
		sample_rate = atoi(argv[1]);
		sample_bit = atoi(argv[2]);
		chn_num = atoi(argv[3]);
		test_sec = atoi(argv[4]);
	}else{
		printf("usage is not right!\n");
		return -1;
	}

	save_fd = open(PCM_SAVE_FILE, O_RDWR | O_CREAT);
	if(save_fd < 0)
	{
		printf("open pcm save file failed!\n");
		ret = -1;
		goto err0;
	}
	

	total_pcm_size = sample_rate * chn_num * sample_bit / 8 * test_sec;
	
	printf("total_pcm_size = %d \n", total_pcm_size);
	


	ret = pcm_vir_set_config(sample_rate, sample_bit, chn_num);
	if(ret < 0)
	{
		goto err1;
	}
	ret = pcm_vir_prepare();
	if(ret < 0)
	{
		goto err2;
	}

	ret = mqueue_init();
	if(ret < 0)
	{
		printf("mque init failed!\n");
		goto err2;
	}
	mqueue_register_handle(0, mq_data_handle_func);

	ret = pcm_vir_start();
	if(ret < 0)
	{
		goto err3;
	}

	while(1)
	{
		if(pcm_read_len >= total_pcm_size)
		{
			printf("read pcm finish!\n");
			break;
		}
		usleep(1000);

	}

err3:
	mqueue_deinit();
	mqueue_unregister_handle(0);
err2:
	pcm_vir_stop();
err1:
	close(save_fd);
err0:
	return ret;
}





