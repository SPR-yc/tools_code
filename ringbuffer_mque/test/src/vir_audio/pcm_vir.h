/* ============================================================================= 
 *
 *  Copyright © 2019-2024年 QiGan. All rights reserved.
 *  Description: test for get audio data
 *  FilePath: /src/vir_audio/pcm_vir.h
 *  Author: WangYuChun
 *  Date: 2024-04-09 11:14:38
 *  LastEditors: error: git config user.name & please set dead value or install git
 *  LastEditTime: 2024-04-09 11:19:32
 * ============================================================================= */

#ifndef __PCM_VIR_H__
#define  __PCM_VIR_H__


//上层设置采样音频参数数据类型
typedef struct __pcm_config{
    int sample_bit;          //采样深度
    int sample_rate;         //采样率
    int chn_num;             //通道数
}PCM_CONFIG_ST;

//回传底层pcm数据
typedef struct __pcm_data_info{
    unsigned int data_len;
    char *data;  
}PCM_INFO_S;


int pcm_vir_set_config(int rate, int bit_width, int chn_num);     //设置采样参数
int pcm_vir_prepare();        //申请buffer等一些资源  注册消息队列
int pcm_vir_start();          //创建线程，读pcm数据进入buffer
// int pcm_vir_get_data(char *p_dist, uint32_t len);       //获取音频数据

//用户回调函数，处理底层获取返回数据 或消息队列
typedef int (*data_handle_callback)(PCM_INFO_S *arg); //callback type
int pcm_vir_register_callback(data_handle_callback callback);



int pcm_vir_stop();           //停止获取数据，释放一些资源 注销消息队列

#endif  //__PCM_VIR_H__
