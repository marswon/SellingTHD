#ifndef __BSP_COIN_H
#define __BSP_COIN_H
#include "stm32f10x_conf.h"
#include "bsp_common.h"


#include "bsp_yingbiqi.h"
#include "bsp_zhibiqi.h"


//硬币器
extern u8 pre_05_TUBE;   //硬币器5角钱数量，上次取货后5角数量
extern u8 pre_10_TUBE;   //硬币器1元钱数量，上次取货后1元数量

//功能：纸币器和硬币器联合工作
void COIN_use(void);

#endif

