#ifndef _BSP_COMMON_H
#define _BSP_COMMON_H
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "stm32f10x_conf.h"

#include "user.h"

#if SYSTEM_SUPPORT_UCOS
#include "includes.h"
#endif

#include "bsp_usart.h"
#include "bsp_gpio.h"
//#include "bsp_adc.h"
//#include "bsp_dma.h"
//#include "bsp_iic.h"
//#include "bsp_systick.h"
#include "bsp_delay.h"

#define   Version_Year       "27"       // year
#define   Version_Month      "12"      // month
#define   Version_Day        "01"      // day

#define HBYTE(a)               ((unsigned char)(((unsigned short)(a) >> 8) & 0xFF))
#define LBYTE(a)               ((unsigned char)((unsigned short)(a)& 0xff))
#define MAKEWORD(bLow,bHigh)   ((unsigned short)(((unsigned char)(bLow))|(((unsigned short)((unsigned char)(bHigh)))<<8)))

typedef enum
{
    FALSE = 0, TRUE = 1
} bool;

unsigned short CRC16_isr(unsigned char *Dat, unsigned int len);
bool strLenCmp(char *cmd1, char *cmd2, u8 len);
bool strLenCpy(char *dst, char *src, u8 len);
u8 strLen(char *cmd);

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);



#endif  //_BSP_COMMON_H
