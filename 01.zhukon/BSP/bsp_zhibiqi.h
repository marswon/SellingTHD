#ifndef     __BSP_ZHIBIQI_H
#define     __BSP_ZHIBIQI_H
#include "stm32f10x_conf.h"
#include "bsp_common.h"

/* 纸币器串口指令 */

//常规指令
#define     RESET_ZHI           0x30
#define     STATUS_ZHI          0x31
#define     SECURITY_ZHI        0x32
#define     POLL_ZHI            0x33
#define     BILL_TYPE_ZHI       0x34
#define     ESCROW_ZHI          0X35
#define     STACKER_ZHI         0X36
//扩展指令
#define     IDENTIFICATION_ZHI  0x3700
#define     FEATURE_ENABLE_ZHI  0x3701
#define     IDENTIFICATION_ZHI2 0x3702
//常规指令回复数据长度，不包括CHK校验字节
#define     LEN_RESET_ZHI           1
#define     LEN_STATUS_ZHI          27
#define     LEN_SECURITY_ZHI        0
#define     LEN_POLL_ZHI            16
#define     LEN_BILL_TYPE_ZHI       0
#define     LEN_ESCROW_ZHI          0
#define     LEN_STACKER_ZHI         2
//常规指令发送数据区长度，只有2条指令有数据区
#define     DAT_SECURITY_ZHI        2
#define     DAT_BILL_TYPE_ZHI       4
#define     DAT_ESCROW_ZHI          1
//扩展指令回复数据长度，不包括CHK校验字节
#define     LEN_IDENTIFICATION_ZHI      29
#define     LEN_FEATURE_ENABLE_ZHI      0
#define     LEN_IDENTIFICATION_ZHI2     33
//扩展指令发送数据区长度，只有1条指令有数据区
#define     DAT_FEATURE_ENABLE_ZHI      4

//纸币器初始化
void ZhiBiQi_Init(void);
//纸币器使用
void ZhiBiQi_USE(void);

#endif
