#ifndef __BSP_YINGBIQI_H
#define __BSP_YINGBIQI_H
#include "stm32f10x_conf.h"
#include "bsp_common.h"

/* 硬币器串口指令 */

//常规指令
#define     RESET_YING       0x08
#define     STATUS_YING      0x09
#define     TUBE_STATUS_YING 0x0A
#define     POLL_YING        0x0B
#define     COIN_TYPE_YING   0x0C
#define     DISPENSE_YING    0x0D
//扩展指令
#define     IDENTIFICATION_YING      0x0F00
#define     FEATURE_ENABLE_YING      0x0F01
#define     PAYOUT_YING              0x0F02
#define     PAYOUT_STATUS_YING       0x0F03
#define     PAYOUT_VALUE_POLL_YING   0x0F04
#define     SEND_DIAGNOSTIC_YING     0x0F05
//常规指令回复数据长度，不包括CHK校验字节
#define     LEN_RESET_YING           1
#define     LEN_STATUS_YING          23
#define     LEN_TUBE_STATUS_YING     18
#define     LEN_POLL_YING            16
#define     LEN_COIN_TYPE_YING       4
#define     LEN_DISPENSE_YING        1
//常规指令发送数据区长度，只有2条指令有数据区
#define     DAT_COIN_TYPE           4
#define     DAT_DISPENSE            1
//扩展指令回复数据长度，不包括CHK校验字节
#define     LEN_IDENTIFICATION_YING       33
#define     LEN_FEATURE_ENABLE_YING      0
#define     LEN_PAYOUT_YING              0
#define     LEN_PAYOUT_STATUS_YING       16
#define     LEN_PAYOUT_VALUE_POLL_YING   1
#define     LEN_SEND_DIAGNOSTIC_YING     16
//扩展指令发送数据区长度，只有2条指令有数据区
#define     DAT_FEATURE_ENABLE_YING      4
#define     DAT_PAYOUT_YING              1

//自定义扩展指令长度处理错误标志位
#define     FLAG_ERROR_EXP      2
//发送常规指令校验，判断硬币器和纸币器
#define     IS_COIN_BASIC_COMMAND(cmd)      ((cmd == RESET_YING) || (cmd == STATUS_YING) || (cmd == TUBE_STATUS_YING) || \
               (cmd == POLL_YING) || (cmd == COIN_TYPE_YING) || (cmd == DISPENSE_YING) || (cmd == RESET_ZHI) || (cmd == STATUS_ZHI) || \
               (cmd == SECURITY_ZHI) || (cmd == POLL_ZHI) || (cmd == BILL_TYPE_ZHI) || (cmd == ESCROW_ZHI) || (cmd == STACKER_ZHI))

//发送扩展指令校验，判断硬币器
#define     IS_COIN_EXP_COMMAND_YING(exp_cmd)   ((exp_cmd == IDENTIFICATION_YING) || (exp_cmd == FEATURE_ENABLE_YING) || (exp_cmd == PAYOUT_YING) || \
                (exp_cmd == PAYOUT_STATUS_YING) || (exp_cmd == PAYOUT_VALUE_POLL_YING) || (exp_cmd == SEND_DIAGNOSTIC_YING))
//发送扩展指令校验，判断纸币器
#define     IS_COIN_EXP_COMMAND_ZHI(exp_cmd)   ((exp_cmd == IDENTIFICATION_ZHI) || (exp_cmd == FEATURE_ENABLE_ZHI) || (exp_cmd == IDENTIFICATION_ZHI2))



//发送地址字节和CHK校验和
u8 Send_CMD_BASIC_coin(u16 basic_cmd, u8 *data);
//发送扩展命令函数，包括数据区
u8 Send_CMD_EXP_coin(u16 exp_cmd, u8 *data);


//硬币器初始化
void YingBiQi_Init(void);
//硬币器使用
void YingBiQi_USE(void);


/***************************************************************************************************/

//单独每条指令使用一个函数，不管发送的结果

//功能：发送复位0X08指令
void Send_RESET_YING(void);
//功能：发送投币器参数指令0x09
void Send_STATUS_YING(void);
//功能：发送投币器参数指令0x0A
void Send_TUBE_STATUS_YING(void);
//功能：发送投币器参数指令0x0B
void Send_POLL_YING(void);
//功能：发送硬币类型0C0003FFFFh，使能收钱
void Send_COIN_ENABLE_YING(void);
//功能：发送硬币类型0C0000FFFFh，禁止收钱
void Send_COIN_DISENABLE_YING(void);
//功能：发送支出的硬币类型指令0DH
void Send_DISPENSE_YING(u8 dat);
//功能：发送扩展指令0X0F00
void Send_IDENTIFICATION_YING(void);
//功能：发送扩展指令0X0F01和数据区
void Send_FEATURE_ENABLE_YING(void);
//功能：发送扩展指令0X0F02和数据区
void Send_PAYOUT_YING(u8 dat);
//功能：发送扩展指令0X0F03
void Send_PAYOUT_STATUS_YING(void);
//功能：发送扩展指令0X0F04
void Send_PAYOUT_VALUE_POLL_YING(void);
//功能：发送扩展指令0X0F05
void Send_SEND_DIAGNOSTIC_YING(void);


/***************************************************************************************************/

//单独每条指令使用一个函数，需要接收到指定的数据

//功能：发送复位0X08指令并校验返回值
void DET_RESET_YING(void);
//功能：发送投币器参数指令0x09并校验返回值
void DET_STATUS_YING(void);
//功能：发送投币器参数指令0x0A并校验返回值
u8 DET_TUBE_STATUS_YING(u8* num_05, u8* num_10);
//功能：发送投币器参数指令0x0B并校验返回值
u8 DET_POLL_YING(void);
//功能：发送硬币类型0C0003FFFFh，使能收钱并校验返回值
void DET_COIN_ENABLE_YING(void);
//功能：发送硬币类型0C0000FFFFh，禁止收钱
void DET_COIN_DISENABLE_YING(void);
//功能：发送支出的硬币类型指令0DH
void DET_DISPENSE_YING(u8 dat);
//功能：发送扩展指令0X0F00
void DET_IDENTIFICATION_YING(void);
//功能：发送扩展指令0X0F01和数据区
void DET_FEATURE_ENABLE_YING(void);
//功能：发送扩展指令0X0F02和数据区
void DET_PAYOUT_YING(u8 dat);
//功能：发送扩展指令0X0F03
void DET_PAYOUT_STATUS_YING(void);
//功能：发送扩展指令0X0F04
void DET_PAYOUT_VALUE_POLL_YING(void);
//功能：发送扩展指令0X0F05
u8 DET_SEND_DIAGNOSTIC_YING(void);
//获取接收数据的CHK
u8 Get_CHK(u8* str, u8 str_len);

extern u8 rev_data_len;        //串口2回复数据长度，用于没有收到数据继续发送

#endif

