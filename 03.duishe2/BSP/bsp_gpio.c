#include "bsp_common.h"

//void GPIO_Configure(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE); //使能PB端口时钟
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //关闭JTAG因为要使用PB3和4
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_2 | GPIO_Pin_6;                //LED0-->PB.14 beep->PB4 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO口速度为50MHz
//    GPIO_Init(GPIOC, &GPIO_InitStructure);                   //根据设定参数初始化GPIOB.14
//    GPIO_SetBits(GPIOC, GPIO_Pin_0);                     //PB.4 输出1
//    GPIO_SetBits(GPIOC, GPIO_Pin_1);                     //PB.4 输出1
//    GPIO_SetBits(GPIOC, GPIO_Pin_2);                     //PB.4 输出1
//    GPIO_SetBits(GPIOC, GPIO_Pin_6);                     //PB.4 输出1
//    //红外线发送功能 功率设置引脚 初始化
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9;                 //LED0-->PB.14 beep->PB4 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO口速度为50MHz
//    GPIO_Init(GPIOC, &GPIO_InitStructure);                   //根据设定参数初始化GPIOB.14
//    GPIO_SetBits(GPIOC, GPIO_Pin_7);                     //PB.4 输出1
//    GPIO_SetBits(GPIOC, GPIO_Pin_8);                     //PB.4 输出1
//    GPIO_SetBits(GPIOC, GPIO_Pin_9);                     //PB.4 输出1
//    //红外线发送功能 层发送引脚 初始化 目前只用到六层
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;               //LED0-->PB.14 beep->PB4 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO口速度为50MHz
//    GPIO_Init(GPIOA, &GPIO_InitStructure);                   //根据设定参数初始化GPIOB.14
//    GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7);                        //PB.4 输出1
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;               //LED0-->PB.14 beep->PB4 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO口速度为50MHz
//    GPIO_Init(GPIOC, &GPIO_InitStructure);                   //根据设定参数初始化GPIOB.14
//    GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);                        //PB.4 输出1
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;               //LED0-->PB.14 beep->PB4 端口配置
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO口速度为50MHz
//    GPIO_Init(GPIOB, &GPIO_InitStructure);                   //根据设定参数初始化GPIOB.14
//    GPIO_SetBits(GPIOB, GPIO_Pin_0);                     //PB.4 输出1
//    GPIO_SetBits(GPIOB, GPIO_Pin_1);                     //PB.4 输出1

//    // key init
//  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//KEY0-KEY2
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOB12,13
//
//  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认上拉
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.8
//
//  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认上拉
//  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA.8
//
//  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//KEY0-KEY2
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB12,13
//}

void NVIC_Configure(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
#if USART1_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if USART2_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;      //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if USART3_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if UART4_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;      //子优先级4
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if UART5_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;      //子优先级5
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
}


void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);   //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);    //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //开启AFIO时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //关闭JTAG因为要使用PB3和4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_2 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_0);    //PC.0 输出1，红灯灭
    GPIO_SetBits(GPIOC, GPIO_Pin_1);    //PC.1 输出1，蓝灯灭
    GPIO_SetBits(GPIOC, GPIO_Pin_2);    //PC.2 输出1，绿灯灭
    GPIO_SetBits(GPIOC, GPIO_Pin_6);    //PC.6 输出1，红外掉货检测停止检测
    //红外线发送功能 功率设置引脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_7);                     //PC.7 输出1
    GPIO_SetBits(GPIOC, GPIO_Pin_8);                     //PC.8 输出1
    GPIO_SetBits(GPIOC, GPIO_Pin_9);                     //PC.9 输出1
    //红外线发送功能 层发送引脚，初始化到10层，常态为高电平，达到某一层检测为低电平
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7);           //PA.6，PA.7输出1，对应第一层，第二层
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);           //PC.4，PC.5输出1，对应第三层，第四层
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
}

void KEY_Init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); //使能PORTA,PORTB,PORTC时钟
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);      //第5层反馈
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);      //PA.8为掉货检测输入，PA.15第1层反馈
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);      //PC.10为第2层反馈，PC.11为第3层反馈，PC.12为第4层反馈
    //PB.3为第6层反馈，PB.4为第7层反馈，PB.5为第8层反馈，PB.6为第9层反馈，PB.7为第10层反馈
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
