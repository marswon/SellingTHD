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
//#define     LEN_STATUS_ZHI          27
#define     LEN_STATUS_ZHI          15
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

/***************************************************************************************************/

//单独每条指令使用一个函数

//功能：发送复位指令30H
void Send_RESET_ZHI(void);
//功能：发送纸币器固有属性查看指令31H
void Send_STATUS_ZHI(void);
//功能：发送安全等级指令32H，数据区两字节
void Send_SECURITY_ZHI(void);
//功能：发送纸币器动作状态指令33H
void Send_POLL_ZHI(void);
//功能：发送纸币器可接收纸币类型指令34H
void Send_BILL_TYPE_ZHI(u8 mode);
//功能：发送暂保留状态指令35H，数据区1字节
void Send_ESCROW_ZHI(u8 mode);
//功能：发送复位指令36H
void Send_STACKER_ZHI(void);
//功能：发送扩展指令0X3700
void Send_IDENTIFICATION_ZHI(void);
//功能：发送扩展指令0X3701和数据区4字节
void Send_FEATURE_ENABLE_ZHI(void);
//功能：发送扩展指令0X3702
void Send_IDENTIFICATION_ZHI2(void);


/***************************************************************************************************/

//单独每条指令使用一个函数，需要接收到指定的数据

//功能：发送复位0X30指令并校验返回值
void DET_RESET_ZHI(void);
//功能：发送纸币器参数指令0x31并校验返回值
void DET_STATUS_ZHI(void);
//功能：发送纸币器安全等级指令0x32并校验返回值
void DET_SECURITY_ZHI(void);
//功能：发送纸币器动作指令0x33并校验返回值
u8 DET_POLL_ZHI(void);
//功能：发送纸币器可接收纸币类型指令34H，数据区4字节
void DET_BILL_TYPE_ZHI(u8 mode);
//功能：处理处于暂保留位置的纸币35H
void DET_ESCROW_ZHI(u8 mode);
//功能：发送读取现金盒钱数指令36H
u8 DET_STACKER_ZHI(u16* num_coin);

//功能：发送扩展指令0x3700
void DET_IDENTIFICATION_ZHI(void);

#endif


