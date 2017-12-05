#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H
#include "stm32f10x_conf.h"

#define powercheck   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)

//LED调试灯
#define LED     PBout(14)
//LED灯亮
#define LED_ON()        (LED = 1)
//LED灯灭
#define LED_OFF()       (LED = 0)
//串口2选择,总共4种模式
#define IS_USART2_SELECT ((mode == 0) || (mode == 1) || (mode == 2) || (mode == 3))
//串口2模式配置引脚
#define MODE0_USART2    PAout(0)
#define MODE1_USART2    PAout(1)
//蜂鸣器输出
#define BEEP        PBout(4)

//串口2扩展功能选择
void USART2_select(u8 mode);
void GPIO_Configure(void);
void NVIC_Configure(void);
void USART_Configure(void);
void RUN_Init(void);

#endif  //_BSP_GPIO_H
