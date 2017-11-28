#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "bsp_sys.h"

void Adc_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch, u8 times);

#endif
