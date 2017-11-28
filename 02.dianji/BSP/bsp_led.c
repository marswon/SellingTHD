#include "bsp_led.h"

/***************************************************************************************
功能：LED相关引脚初始化
入口参数：无
返回值：无
说明：LED0对应PA15引脚，默认上电后，为JTAG调试用的引脚。指示程序的正常运行，由TIMER3
    控制实现1s亮，1s灭。
****************************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //重映射开启，要使用JTAG相关引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);      //LED0,程序调试灯
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);         //常态下，LED0熄灭
}


