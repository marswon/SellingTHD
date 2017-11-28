#ifndef __bsp_key_h
#define __bsp_key_h

#include "bsp_sys.h"

////上限位 常态为高电平，到达上限位置时检测为低电平，检测到低电平切断电机运行
//#define PKEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
////下限位 常态为高电平，到达下限位置时检测为低电平，检测到低电平切断电机运行
//#define PKEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)

////出货门电机开门限位开关 常态为高电平，到达位置时检测为低电平，切断出货门电机运行
//#define PKEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)
////出货门电机关门限位开关 常态为高电平，到达位置时检测为低电平，切断出货门电机运行
//#define PKEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)

//拨码开关,实现一板多用和电机驱动板级联
#define DKEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)  //拨码开关第一位 当输入为高软件为电机驱动板软件;当输入为低时软件为电磁锁软件 (ON端为0)
#define DKEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)  //拨码开关第二位 当输入为高软件为升降梯售货机;当输入为低时为弹簧道售货机 (ON端为0）
#define DKEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)  //拨码开关第三位 RS485地址配置 用于电机驱动板级联   (ON端为0)
#define DKEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10) //拨码开关第四位 RS485地址配置 用于电机驱动板级联   (ON端为0)

////用于掉货检测，常态下低电平，掉货时，检测到高电平
//#define PUTThing  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)

//按键
#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)

#define KEY0_PRES   1   //KEY0按下
#define KEY1_PRES   2   //KEY1按下
#define KEY2_PRES   3   //KEY2按下
#define KEY3_PRES   4   //KEY_UP按下(即WK_UP/KEY_UP)

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
u8 DKEY_Scan(void);

#endif
