/************************************************************************************************
Test_main.c:只用于测试基本功能，配合串口单步测试，单步执行
*************************************************************************************************/

#include "bsp_sys.h"

extern u8 flag_test;                //调试标记位，用于PC机调试，根据不同值执行不同动作
extern bool flag_enable_debug;
char strtemp[100] = {0};       //打印调试信息

int main(void)
{
#if SYS_ENABLE_IAP
    SCB->VTOR = 0x8002000;
    __enable_irq();
#endif
    delay_init();
    LED_Init();
    KEY_Init();
    GPIO_Configure();
    NVIC_Configure();
    USART_Configure();
    EXTIX_Init();       //外部中断初始化
//    TIM3_Int_Init(9, 7199); //通用定时器TIM2,1ms
    TIM3_Int_Init(999, 7199); //通用定时器TIM2,100ms
    HuoDao_Init();
    sprintf((char*)strtemp, "%s.%s%s\r\n", Version_Year, Version_Month, Version_Day);
    USART_SendBytes(USART1, (uint8_t*)strtemp, strlen((char*)strtemp));
#if SYS_ENABLE_IAP

    if(IAP_Read_UpdateFLAG() != 1)
    {
        Send_CMD(USART2, HBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji), LBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji));
        IAP_Write_UpdateFLAG();
    }

#endif

    while(1)        //PC机调试,记得关闭TIMER3
    {
//        printf("PC TEST\r\n");
        if(flag_test == 1)          //调试货道初始化
        {
            flag_test = 0;
            HuoDao_Init();
        }
        else if(flag_test == 2)     //开启PC打印
        {
            flag_test = 0;

            if(!flag_enable_debug)
            {
                flag_enable_debug = TRUE;
            }

            USART_DEBUG("debug\r\n");
        }
        else if(flag_test == 3)     //关闭PC打印
        {
            flag_test = 0;
            flag_enable_debug = FALSE;
        }
        else if(flag_test == 4)
        {
            flag_test = 0;
            Disable_duishe();       //关闭掉货检测，需要取货检测
        }
        else if(flag_test == 5)
        {
            flag_test = 0;
            printf("PUTThing : %d\r\n", PUTThing);
            delay_ms(500);
            printf("PUTThing : %d\r\n", PUTThing);
        }
        else if(flag_test == 6)
        {
            extern bool Enable_EXTI;       //使能掉货检测外部中断
            flag_test = 0;
            Enable_EXTI = TRUE;     //开启外部检测
            //开启掉货检测
            Enable_duishe();
        }
        else if(flag_test == 7)
        {
            flag_test = 0;
            HUOWU_Take(1, 1);
        }
        else if(flag_test == 8)     //测试第1行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(1);
        }
        else if(flag_test == 9)     //测试第2行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(2);
        }
        else if(flag_test == 10)     //测试第3行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(3);
        }
        else if(flag_test == 11)     //测试第4行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(4);
        }
        else if(flag_test == 12)     //测试第5行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(5);
        }
        else if(flag_test == 13)     //测试第6行所有货道
        {
            flag_test = 0;
            HuoDao_line_test(6);
        }
        else if(flag_test == 14)
        {
            //flag_test = 0;
            if(KEY_Scan(1))     //支持连按，调试按键
            {
                LED0 = 1;       //灯亮
                delay_ms(2000);
                LED0 = 0;
                delay_ms(2000);
            }
        }
        else if(flag_test == 15)
        {
            USART_SendBytess(USART1, "flag_test : 15\r\n");
            flag_test = 0;
            //开启掉货检测
            Enable_duishe();
            delay_ms(1000);
            delay_ms(1000);
            delay_ms(1000);
            delay_ms(1000);
            delay_ms(1000);
            Disable_duishe();       //关闭掉货检测，需要取货检测
        }
    }
}


