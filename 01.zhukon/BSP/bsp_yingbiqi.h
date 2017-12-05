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
#define     EXPENSION           0x0F
#define     IDETIFICATION_EXP       0x00
#define     FEATURE_ENABLE_EXP      0x01
#define     PAYOUT_EXP              0x02
#define     PAYOUT_STATUS_EXP       0x03
#define     PAYOUT_VALUE_POLL_EXP   0x04
#define     SEND_DIAGNOSTIC_EXP     0x05

//发送地址字节校验
#define     IS_COIN_COMMAND(addr)   ((addr == RESET_COMMAND) || (addr == STATUS_COMMAND) || (addr == TUBE_STATUS_COMMAND) || \
                (addr == POLL_COMMAND) || (addr == COIN_TYPE_COMMAND) || (addr == DISPENSE_COMMAND) || (addr == EXPENSION))

//发送地址字节和CHK校验和
void Send_ADDR_coin(u8 addr);

#endif

