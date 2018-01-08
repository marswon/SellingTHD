#include "bsp_sys.h"
//测试和正式运行程序标志位，值为1为正式运行程序，值为0为测试的程序
#define FLAG_RUN    1

extern u8 flag_test;                //调试标记位，用于PC机调试，根据不同值执行不同动作
extern bool flag_enable_debug;

int main(void)
{
    u8 data = 0;
    u16 i = 0;
    u8 ntmp[255] = {0};
    u8 ndat[255] = {0}; // 协议数据
    u16 nlen = 0; // 协议数据包长度
    u16 ncrc = 0; // 协议crc16
    u16 ncmd = 0; // 协议指令
#if SYS_ENABLE_IAP
    SCB->VTOR = 0x8002000;
    __enable_irq();
#endif
    delay_init();
    LED_Init();
    KEY_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_Configure();
    uart1_init(9600);
    uart3_init(9600);
    uart2_init(9600);
//    TIM3_Int_Init(9, 7199); //通用定时器TIM2,1ms
    TIM3_Int_Init(999, 7199); //通用定时器TIM2,100ms
    HuoDao_Init();
    sprintf((char*)ndat, "%s.%s%s\r\n", Version_Year, Version_Month, Version_Day);
    USART_SendBytes(USART1, ndat, strlen((char*)ndat));
#if SYS_ENABLE_IAP

    if(IAP_Read_UpdateFLAG() != 1)
    {
        Send_CMD(USART2, HBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji), LBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji));
        IAP_Write_UpdateFLAG();
    }

#endif
#if OFFICIAL_VERSION
    //测试版本，输出对应版本内容
    sprintf((char*)ndat, "FLAG_RUN: %d\r\n", FLAG_RUN);      //打印正式程序还是测试程序，实际发布不需要
    USART_SendBytes(USART1, ndat, sizeof(ndat));
#endif
#if FLAG_RUN

    while(1)
    {
        if(USART_BufferRead(&data) == 1)
        {
            // 远程升级关闭
            if(flag_start_flash == 0)
            {
                i++;
                i = i % 255;
                ntmp[i] = data;

                if(ntmp[i - 1] == 0x0D && ntmp[i] == 0x0A) // 判断包尾
                {
                    nlen = MAKEWORD(ntmp[i - 4], ntmp[i - 5]); // 获取数据包长度

                    if(i > nlen)
                    {
                        ncrc = CRC16_isr(&ntmp[i - (nlen + 5)], nlen + 2); // crc计算

                        if(ncrc == MAKEWORD(ntmp[i - 2], ntmp[i - 3])) // crc判断
                        {
                            ncmd = MAKEWORD(ntmp[i - (nlen + 5 - 1)], ntmp[i - (nlen + 5)]); // 解析出串口协议命令,cmd1+cmd2

                            if(nlen > 2) // 获取数据区域
                            {
                                memset(ndat, 0, sizeof(ndat));
                                memcpy(ndat, &ntmp[i - (nlen + 5 - 2)], nlen - 2);
                                Handle_USART_CMD(ncmd, (char *)ndat, nlen - 2); // 处理指令+数据
                            }
                            else
                            {
                                Handle_USART_CMD(ncmd, "", 0); // 处理指令
                            }

                            i = 0;      // 命令缓冲区在处理一次命令后，指针归零
                        }
                    } /* end of if(i > nlen) */
                } /* end of if(ntmp[i - 1] == 0x0D && ntmp[i] == 0x0A) */
            } /* end of if(flag_start_flash == 0) */
            else
            {
                USART_SendByte(USART3, data);
                //USART_SendByte(USART1, data);
            }
        } /* end of if(USART_BufferRead(&data)) */
    } /* end of while(1)*/

#else
    /***********************************************************************************************************/

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
            flag_test = 0;
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
    }

#endif
}


