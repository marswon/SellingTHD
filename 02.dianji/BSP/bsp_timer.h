#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H
#include "bsp_sys.h"

//定义中断优先级分组配置使能
#define TIM2_CONFIG_ENABLED     (0)
#define TIM3_CONFIG_ENABLED     (1)

extern u8 flag_shangxian;              //上限位异常标志
extern u16 num_chuhuo_timer3;       //货物出货超时，计时位，用在TIMER3中断程序

void TIM4_Int_Init(u16 arr, u16 psc);
void TIM3_Int_Init(u16 arr, u16 psc);
void TIM2_Int_Init(u16 arr, u16 psc);

#endif
