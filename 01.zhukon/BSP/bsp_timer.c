#include "bsp_timer.h"


//u16 num_adc1_timer3 = 0;          //ADC1检测检测计时位，用在TIMER3中断程序
//u16 num_adc2_timer3 = 0;          //ADC2检测检测计时位，用在TIMER3中断程序
u16 num_led_time = 0;       //控制LED0调试灯闪烁的节奏，用在定时器TIMER3中断服务程序中

/*************************************************
功能：通用定时器TIM2初始化函数
入口参数：arr为自动重载值   psc分频值
返回值：无
说明：TIM2挂在APB1总线上，一般分频值不为1，定时器时钟为APB1的2倍，即72Mhz
    一般设置arr=4999    psc=7199
    溢出时间计算公式：溢出时间Tout = (arr+1)*(psc+1)/TCLK
    例如：arr=4999 psc=7199    Tout = (4999+1)*(7199+1)/72M=500ms
*************************************************/
void TIM2_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能指定的TIM2中断,允许更新中断
//  TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
}

/*********************************************************************************************
功能：定时器2中断服务函数
入口参数：无
返回值：无
说明：增加升降电机上升保护，在100ms时间内，观察是否达到上升限位。一般情况，升降电机上升过程中
    会触发层反馈，然后控制升降电机停转，所以是不会达到上升限位，如果层反馈失效，托板会一直上升
    必须增加响应检测和保护。
***********************************************************************************************/
void TIM2_IRQHandler(void)   //TIM2中断
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//        YingBiQi_USE();         //时间控制，循环发送0A和0B指令
    }
}

void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}

/**********************************************************************************************************
功能：定时器3中断服务程序
说明：num_chuhuo_timer3为货物出货超时计时位，用在TIMER3中断程序，定时器每次计时100ms，取货计时15s
***********************************************************************************************************/
void TIM3_IRQHandler(void)   //TIM3中断
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);    //清除TIMx的中断待处理位:TIM 中断源
    }

    num_led_time++;

    //实现LED0调试灯的闪烁
    if(num_led_time > 5)         //调试灯500ms亮，500ms灭，指示程序运行
    {
        num_led_time = 0;
        LED = ~LED;       //闪烁
    }
}

/*************************************************
功能：通用定时器TIM4初始化函数
入口参数：arr为自动重载值   psc分频值
返回值：无
说明：TIM2挂在APB1总线上，一般分频值不为1，定时器时钟为APB1的2倍，即72Mhz
    一般设置arr=4999    psc=7199
    溢出时间计算公式：溢出时间Tout = (arr+1)*(psc+1)/TCLK
    例如：arr=4999 psc=7199    Tout = (4999+1)*(7199+1)/72M=500ms
*************************************************/
void TIM4_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
    TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
}

/*********************************************************************************************
功能：定时器4中断服务函数
入口参数：无
返回值：无
说明：增加升降电机上升保护，在100ms时间内，观察是否达到上升限位。一般情况，升降电机上升过程中
    会触发层反馈，然后控制升降电机停转，所以是不会达到上升限位，如果层反馈失效，托板会一直上升
    必须增加响应检测和保护。
    ADC检测暂时没有用，因为有BUG，暂时无法解决。影响层正常反馈
***********************************************************************************************/
void TIM4_IRQHandler(void)   //TIM4中断
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }

    num_led_time++;

    //实现LED0调试灯的闪烁
    if(num_led_time > 1000)         //调试灯1s亮，1s灭，指示程序运行
    {
        num_led_time = 0;

        if(LED)
        {
            LED = 0;
        }
        else
        {
            LED = 1;
        }
    }
}

