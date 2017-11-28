#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H
#include "stm32f10x_conf.h"


//掉货检测
#define PUT_THING  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
//掉货检测使能，值为0开启掉货检测，值为1关闭掉货检测
#define PUT_ST PCout(6)

//霍尔层反馈检测，1到10层，用于升降托板高度检测
#define LED_RE_L1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define LED_RE_L2   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)
#define LED_RE_L3   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)
#define LED_RE_L4   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)
#define LED_RE_L5   GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)
#define LED_RE_L6   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define LED_RE_L7   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define LED_RE_L8   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
#define LED_RE_L9   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define LED_RE_L10  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)


//LED指示灯，输出低电平灯亮
#define LEDR PCout(0)
#define LEDB PCout(1)
#define LEDG PCout(2)

//void GPIO_Configure(void);
void NVIC_Configure(void);
//void RUN_Init(void);
void LED_Init(void);
void KEY_Init(void);

#endif  //_BSP_GPIO_H
