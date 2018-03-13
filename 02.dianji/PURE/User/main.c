#include "bsp_sys.h"

extern bool flag_enable_debug;
char strtemp[100] = {0};       //打印调试信息

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
}


