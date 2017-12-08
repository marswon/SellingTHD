#ifndef __BSP_YINGBIQI_H
#define __BSP_YINGBIQI_H
#include "stm32f10x_conf.h"
#include "bsp_common.h"


//常规指令
#define     RESET_COMMAND       0x08
#define     STATUS_COMMAND      0x09
#define     TUBE_STATUS_COMMAND 0x0A
#define     POLL_COMMAND        0x0B
#define     COIN_TYPE_COMMAND   0x0C
#define     DISPENSE_COMMAND    0x0D
//扩展指令
#define     IDETIFICATION_EXP       0x0F00
#define     FEATURE_ENABLE_EXP      0x0F01
#define     PAYOUT_EXP              0x0F02
#define     PAYOUT_STATUS_EXP       0x0F03
#define     PAYOUT_VALUE_POLL_EXP   0x0F04
#define     SEND_DIAGNOSTIC_EXP     0x0F05
//常规指令回复数据长度，不包括CHK校验字节
#define     LEN_RESET           0
#define     LEN_STATUS          23
#define     LEN_TUBE_STATUS     18
#define     LEN_POLL            16
#define     LEN_COIN_TYPE       4
#define     LEN_DISPENSE        1
//常规指令发送数据区长度，只有2条指令有数据区
#define     DAT_COIN_TYPE           4
#define     DAT_DISPENSE            1
//扩展指令回复数据长度，不包括CHK校验字节
#define     LEN_IDETIFICATION       33
#define     LEN_FEATURE_ENABLE      0
#define     LEN_PAYOUT              0
#define     LEN_PAYOUT_STATUS       16
#define     LEN_PAYOUT_VALUE_POLL   1
#define     LEN_SEND_DIAGNOSTIC     16
//扩展指令发送数据区长度，只有2条指令有数据区
#define     DAT_FEATURE_ENABLE      4
#define     DAT_PAYOUT              1

//自定义扩展指令长度处理错误标志位
#define     FLAG_ERROR_EXP      2
//发送地址字节校验
#define     IS_COIN_BASIC_COMMAND(cmd)      ((cmd == RESET_COMMAND) || (cmd == STATUS_COMMAND) || (cmd == TUBE_STATUS_COMMAND) || \
               (cmd == POLL_COMMAND) || (cmd == COIN_TYPE_COMMAND) || (cmd == DISPENSE_COMMAND))

//发送扩展命令字节
#define     IS_COIN_EXP_COMMAND(exp_cmd)   ((exp_cmd == IDETIFICATION_EXP) || (exp_cmd == FEATURE_ENABLE_EXP) || (exp_cmd == PAYOUT_EXP) || \
                (exp_cmd == PAYOUT_STATUS_EXP) || (exp_cmd == PAYOUT_VALUE_POLL_EXP) || (exp_cmd == SEND_DIAGNOSTIC_EXP))

//发送地址字节和CHK校验和
u8 Send_CMD_BASIC_coin(u8 basic_cmd, u8 *data);
//发送扩展命令函数，包括数据区
u8 Send_CMD_EXP_coin(u16 exp_cmd, u8 *data);

#endif

