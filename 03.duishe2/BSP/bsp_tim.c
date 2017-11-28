#include "bsp_common.h"

extern u16 LED_RE_L1_xx;
extern u16 LED_RE_L2_xx;
extern u16 LED_RE_L3_xx;
extern u16 LED_RE_L4_xx;
extern u16 LED_RE_L5_xx;
extern u16 LED_RE_L6_xx;

extern u8 LED_RE_L1_bit;   // 当为1时，说明该层被触发（遮挡）
extern u8 LED_RE_L2_bit;   // 当为1时，说明该层被触发（遮挡）
extern u8 LED_RE_L3_bit;   // 当为1时，说明该层被触发（遮挡）
extern u8 LED_RE_L4_bit;   // 当为1时，说明该层被触发（遮挡）
extern u8 LED_RE_L5_bit;   // 当为1时，说明该层被触发（遮挡）
extern u8 LED_RE_L6_bit;   // 当为1时，说明该层被触发（遮挡）

extern u16 wave1_n1;

u8 flag_start_zaibo_L1 = 0;
u8 flag_start_zaibo_L2 = 0;
u8 flag_start_zaibo_L3 = 0;
u8 flag_start_zaibo_L4 = 0;
u8 flag_start_zaibo_L5 = 0;
u8 flag_start_zaibo_L6 = 0;

bool flag_enable_wave_L1 = FALSE;
bool flag_enable_wave_L2 = FALSE;
bool flag_enable_wave_L3 = FALSE;
bool flag_enable_wave_L4 = FALSE;
bool flag_enable_wave_L5 = FALSE;
bool flag_enable_wave_L6 = FALSE;

u16 cnt20ms = 0;

void TIM2_Configure(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
    TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
    //NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    //NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void TIM3_Configure(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
    //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    //NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void TIM2_IRQHandler(void)   //TIM2中断
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
    }

    wave1_n1++;
    cnt20ms++;
    LED_RE_L1_xx++;
    LED_RE_L2_xx++;
    LED_RE_L3_xx++;
    LED_RE_L4_xx++;
    LED_RE_L5_xx++;
    LED_RE_L6_xx++;

    // 发送波
    if(wave1_n1 == 40) //8ms一个周期
    {
        wave1_n1 = 0;
    }

    if(wave1_n1 == 5) // 1ms
    {
        if(flag_enable_wave_L1)
        {
            flag_start_zaibo_L1 = 1;
        }
        else
        {
            LED_SEND_L1 = 0;
            flag_start_zaibo_L1 = 0;
        }

        if(flag_enable_wave_L2)
        {
            flag_start_zaibo_L2 = 1;
        }
        else
        {
            LED_SEND_L2 = 0;
            flag_start_zaibo_L2 = 0;
        }

        if(flag_enable_wave_L3)
        {
            flag_start_zaibo_L3 = 1;
        }
        else
        {
            LED_SEND_L3 = 0;
            flag_start_zaibo_L3 = 0;
        }

        if(flag_enable_wave_L4)
        {
            flag_start_zaibo_L4 = 1;
        }
        else
        {
            LED_SEND_L4 = 0;
            flag_start_zaibo_L4 = 0;
        }

        if(flag_enable_wave_L5)
        {
            flag_start_zaibo_L5 = 1;
        }
        else
        {
            LED_SEND_L6 = 0;
            flag_start_zaibo_L5 = 0;
        }

        if(flag_enable_wave_L6)
        {
            flag_start_zaibo_L6 = 1;
        }
        else
        {
            LED_SEND_L6 = 0;
            flag_start_zaibo_L6 = 0;
        }
    }
    else if(wave1_n1 == 10) // 2ms-8ms
    {
        flag_start_zaibo_L1 = 0;
        LED_SEND_L1 = 0;
        flag_start_zaibo_L2 = 0;
        LED_SEND_L2 = 0;
        flag_start_zaibo_L3 = 0;
        LED_SEND_L3 = 0;
        flag_start_zaibo_L4 = 0;
        LED_SEND_L4 = 0;
        flag_start_zaibo_L5 = 0;
        LED_SEND_L5 = 0;
        flag_start_zaibo_L6 = 0;
        LED_SEND_L6 = 0;
    }

///////////////////////////////////
    if(LED_RCV_L1 == 0)
    {
        LED_RE_L1_xx = 0;
    }

    if(LED_RCV_L2 == 0)
    {
        LED_RE_L2_xx = 0;
    }

    if(LED_RCV_L3 == 0)
    {
        LED_RE_L3_xx = 0;
    }

    if(LED_RCV_L4 == 0)
    {
        LED_RE_L4_xx = 0;
    }

    if(LED_RCV_L5 == 0)
    {
        LED_RE_L5_xx = 0;
    }

    if(LED_RCV_L6 == 0)
    {
        LED_RE_L6_xx = 0;
    }

    if(cnt20ms > 50) // 10ms
    {
        cnt20ms = 0;

        if(flag_enable_wave_L1)
        {
            if(LED_RE_L1_xx > 100)
            {
                LED_RE_L1_bit = 1;
            }
        }

        if(flag_enable_wave_L2)
        {
            if(LED_RE_L2_xx > 100)
            {
                LED_RE_L2_bit = 1;
            }
        }

        if(flag_enable_wave_L3)
        {
            if(LED_RE_L3_xx > 100)
            {
                LED_RE_L3_bit = 1;
            }
        }

        if(flag_enable_wave_L4)
        {
            if(LED_RE_L4_xx > 100)
            {
                LED_RE_L4_bit = 1;
            }
        }

        if(flag_enable_wave_L5)
        {
            if(LED_RE_L5_xx > 100)
            {
                LED_RE_L5_bit = 1;
            }
        }

        if(flag_enable_wave_L6)
        {
            if(LED_RE_L6_xx > 100)
            {
                LED_RE_L6_bit = 1;
            }
        }
    }
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
    }

    if(flag_start_zaibo_L1 == 1)
    {
        LED_SEND_L1 = !LED_SEND_L1;
    }

    if(flag_start_zaibo_L2 == 1)
    {
        LED_SEND_L2 = !LED_SEND_L2;
    }

    if(flag_start_zaibo_L3 == 1)
    {
        LED_SEND_L3 = !LED_SEND_L3;
    }

    if(flag_start_zaibo_L4 == 1)
    {
        LED_SEND_L4 = !LED_SEND_L4;
    }

    if(flag_start_zaibo_L5 == 1)
    {
        LED_SEND_L5 = !LED_SEND_L5;
    }

    if(flag_start_zaibo_L6 == 1)
    {
        LED_SEND_L6 = !LED_SEND_L6;
    }
}
