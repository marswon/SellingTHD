#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H
#include "bsp_common.h"


//定时器中断分组配置使能，使用对应中断对应的标志位置1
#define TIM2_CONFIG_ENABLED     (0)
#define TIM3_CONFIG_ENABLED     (1)
#define TIM4_CONFIG_ENABLED     (0)


void TIM4_Int_Init(u16 arr, u16 psc);
void TIM3_Int_Init(u16 arr, u16 psc);
void TIM2_Int_Init(u16 arr, u16 psc);

#endif
