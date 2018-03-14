#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H
#include "stm32f10x_conf.h"

//#define powercheck   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)

void GPIO_Configure(void);
void NVIC_Configure(void);
void USART_Configure(void);
void RUN_Init(void);

#endif  //_BSP_GPIO_H
