#include "bsp_gpio.h"


//功能：STM32管脚的初始化配置
//说明：主要配置货道电机行驱动，列驱动，掉货检测，行反馈，红外对射检测相关的引脚，其他用途的引脚配置在对应文件中
void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);      //行反馈引脚1~4，常态为高电平，检测到低电平时即为反馈信号
//    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);      //层反馈引脚，常态为高电平
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);      //掉货检测引脚，常态（接收到光）检测到低电平；掉货：检测到高电平
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);      //行反馈引脚5~10，常态为低电平，检测到高电平时即为反馈信号
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);      //行驱动引脚，常态为高电平
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);      //红外对射使能引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);      //列驱动引脚，常态为低电平
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
    GPIO_ResetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);       //常态下，红外低电平不使能；高电平使能红外发射
}

//功能：嵌套中断优先级设置
//说明：程序中标用到的所有中断优先级定义在这里
void NVIC_Configure(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if USART1_CONFIG_ENABLED > 0
    /* 使能串口1中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;      //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if USART2_CONFIG_ENABLED > 0
    /* 使能串口2中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
#if USART3_CONFIG_ENABLED > 0
    /* 使能串口3中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器
#endif
//定时器中断分组
#if TIM2_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
#endif
#if TIM3_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;             //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;          //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
#endif
//外部中断线中断分组
#if EXTI9_5_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;          //使能掉货检测所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);
#endif
#if EXTI15_10_CONFIG_ENABLED > 0
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;        //使能调试按键KEY0所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);
#endif
}

//串口基本配置函数
void USART_Configure(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
#if USART1_CONFIG_ENABLED > 0       /* 串口配置使能判断，串口1 TX = PA9   RX = PA10 */
    /* 第1步：打开GPIO和USART部件的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /* 第2步：将USART Tx的GPIO配置为推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* 第3步：将USART Rx的GPIO配置为浮空输入模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 第4步：USART 初始化设置
    USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;//头文件宏定义，方便移植;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口
    USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清除发送完成标志 */
#endif
#if USART2_CONFIG_ENABLED > 0                  /*串口配置使能判断， 串口2 TX = PA2， RX = PA3  */
    //使能GPIOA,复用功能以及USART2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART2_RX   PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = USART2_BAUDRATE;       //见宏定义;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清除发送完成标志 */
#endif
#if USART3_CONFIG_ENABLED > 0                  /*串口配置使能判断， 串口3 TX = PB10， RX = PB11  */
    //使能GPIOB,复用功能以及USART3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //USART3_RX   PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = USART3_BAUDRATE;       //见宏定义;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART3, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清除发送完成标志 */
#endif
}

