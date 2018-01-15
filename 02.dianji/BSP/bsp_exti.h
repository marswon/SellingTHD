#ifndef _BSP__EXTI_H
#define _BSP__EXIT_H
#include "bsp_sys.h"

//外部中断优先级分组配置
#define     EXTI9_5_CONFIG_ENABLED      (1)
#define     EXTI15_10_CONFIG_ENABLED    (0)

void EXTIX_Init(void);//外部中断初始化
#endif

