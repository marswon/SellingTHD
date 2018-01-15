#include "bsp_sys.h"

extern bool flag_PUTTHING;      //掉货检测标志位

//功能：外部中断初始化程序
void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //使能复用功能时钟
    //掉货检测GPIOC.9 中断线以及中断初始化配置   上升沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;  //掉货检测引脚PUTThing
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;      //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    //GPIOA.11      中断线以及中断初始化配置 下降沿触发 //调试按键KEY0
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //下降沿触发
//    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//    //GPIOE.4      中断线以及中断初始化配置  下降沿触发 //KEY0
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
//    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//    //GPIOA.0      中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;            //使能掉货检测所在的外部中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    //抢占优先级2，
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;                   //子优先级3
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;            //使能调试按键KEY0所在的外部中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    //抢占优先级2，
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;                   //子优先级2
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;            //使能按键KEY1所在的外部中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    //抢占优先级2
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;                   //子优先级1
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;            //使能按键KEY0所在的外部中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    //抢占优先级2
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;                   //子优先级0
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//功能：外部中断9服务程序
//说明：掉货检测，常态（接收到光）检测到低电平；掉货：检测到高电平
void EXTI9_5_IRQHandler(void)
{
    flag_PUTTHING = TRUE;       //有货物掉落，对应标志位置1
    USART_SendBytess(USART1, "PUTThing\r\n");       //打印调试信息
    EXTI_ClearITPendingBit(EXTI_Line9); //清除LINE9上的中断标志位
}

//外部中断11服务程序
//说明：测试按键KEY0，常态为高电平，按键按下为低电平
//void EXTI15_10_IRQHandler(void)
//{
//    USART_SendBytess(USART1, "KEY0\r\n");       //打印调试信息
//    EXTI_ClearITPendingBit(EXTI_Line11);  //清除LINE11上的中断标志位
//}


