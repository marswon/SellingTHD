#ifndef _BSP_IIC_H
#define _BSP_IIC_H
#include "stm32f10x_conf.h"

#define IIC_ADDR_EEPROM             0xA0

#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7
#define SCL_read      (GPIOB->IDR  & GPIO_Pin_6)
#define SDA_read      (GPIOB->IDR  & GPIO_Pin_7)


#endif  //_BSP_IIC_H
