#include "stm32f10x.h"
#include "bsp_common.h"


u8 KEY_Scan_Hall(u8 mode);      //霍尔层反馈检测，用于检测升降托板高度
u8 KEY_Scaning(u8 mode);        //掉货检测

extern u8 start_flash_flag;
extern u8 falg_ledr;

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
    NVIC_Configure();
    USART_Configure();
    delay_init();
    LED_Init();
    KEY_Init();
    sprintf((char*)ndat, "%s.%s%s\r\n", Version_Year, Version_Month, Version_Day);
    USART_SendBytess(USART1, (char*)ndat);
#if SYS_ENABLE_IAP

    if(IAP_Read_UpdateFLAG() != 1)
    {
        Send_CMD(USART2, HBYTE(USARTCMD_DIANJI_DUISHE_StopUpdateDuishe), LBYTE(USARTCMD_DIANJI_DUISHE_StopUpdateDuishe));
        IAP_Write_UpdateFLAG();
    }

#endif

    while(1)
    {
        if(USART_BufferRead(&data) == 1)
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

                        i = 0;
                    }
                }
            }
        }

        if(KEY_Scaning(1))
        {
            LEDG = 0;
        }
        else
        {
            LEDG = 1;
        }

        //层反馈检测，一直检测是否达到某层
        switch(KEY_Scan_Hall(0))
        {
            case 1:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb1), LBYTE(DUISHE_DIANJI_NUMb1));
                break;

            case 2:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb2), LBYTE(DUISHE_DIANJI_NUMb2));
                break;

            case 3:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb3), LBYTE(DUISHE_DIANJI_NUMb3));
                break;

            case 4:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb4), LBYTE(DUISHE_DIANJI_NUMb4));
                break;

            case 5:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb5), LBYTE(DUISHE_DIANJI_NUMb5));
                break;

            case 6:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb6), LBYTE(DUISHE_DIANJI_NUMb6));
                break;
            case 7:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb7), LBYTE(DUISHE_DIANJI_NUMb7));
                break;

            case 8:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb8), LBYTE(DUISHE_DIANJI_NUMb8));
                break;

            case 9:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb9), LBYTE(DUISHE_DIANJI_NUMb9));
                break;

            case 10:
                Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb10), LBYTE(DUISHE_DIANJI_NUMb10));
                break;

            default:
                break;
        }

        /*掉货检测*/
        if(PUT_THING == 1)
        {
            LEDB = 0;
            Send_CMD(USART2, HBYTE(DUISHE_DIANJI_NUMb15), LBYTE(DUISHE_DIANJI_NUMb15));
        }
        else
        {
            LEDB = 1;
        }

        if(falg_ledr == 1)
        {
            LEDR = 0;
        }
        else
        {
            LEDR = 1;
        }
    }
}

u8 KEY_Scan_Hall(u8 mode)
{
    static u8 key_up = 1;

    if(mode)
        key_up = 1;

    if(key_up && (LED_RE_L1 == 0 || LED_RE_L2 == 0 || LED_RE_L3 == 0 || LED_RE_L4 == 0 || LED_RE_L5 == 0 || 
        LED_RE_L6 == 0 || LED_RE_L7 == 0 || LED_RE_L8 == 0 || LED_RE_L9 == 0 || LED_RE_L10 == 0))
    {
        delay_ms(10);
        key_up = 0;

        if(LED_RE_L1 == 0)
            return 1;
        else if(LED_RE_L2 == 0)
            return 2;
        else if(LED_RE_L3 == 0)
            return 3;
        else if(LED_RE_L4 == 0)
            return 4;
        else if(LED_RE_L5 == 0)
            return 5;
        else if(LED_RE_L6 == 0)
            return 6;
        else if(LED_RE_L7 == 0)
            return 7;
        else if(LED_RE_L8 == 0)
            return 8;
        else if(LED_RE_L9 == 0)
            return 9;
        else if(LED_RE_L10 == 0)
            return 10;
    }
    else if(LED_RE_L1 == 1 && LED_RE_L2 == 1 && LED_RE_L3 == 1 && LED_RE_L4 == 1 && LED_RE_L5 == 1 && 
        LED_RE_L6 == 1 && LED_RE_L7 == 1 && LED_RE_L8 == 1 && LED_RE_L9 == 1 && LED_RE_L10 == 1)
        key_up = 1;

    return 0;
}


u8 KEY_Scaning(u8 mode)
{
    static u8 key_up = 1;

    if(mode)
        key_up = 1;

    if(key_up && (LED_RE_L1 == 0 || LED_RE_L2 == 0 || LED_RE_L3 == 0 || LED_RE_L4 == 0 || LED_RE_L5 == 0 || 
        LED_RE_L6 == 0 || LED_RE_L7 == 0 || LED_RE_L8 == 0 || LED_RE_L9 == 0 || LED_RE_L10 == 0))
    {
        delay_ms(10);
        key_up = 0;

        if(LED_RE_L1 == 0)
            return 1;
        else if(LED_RE_L2 == 0)
            return 2;
        else if(LED_RE_L3 == 0)
            return 3;
        else if(LED_RE_L4 == 0)
            return 4;
        else if(LED_RE_L5 == 0)
            return 5;
        else if(LED_RE_L6 == 0)
            return 6;
    }
    else if(LED_RE_L1 == 1 && LED_RE_L2 == 1 && LED_RE_L3 == 1 && LED_RE_L4 == 1 && LED_RE_L5 == 1 && 
        LED_RE_L6 == 1 && LED_RE_L7 == 1 && LED_RE_L8 == 1 && LED_RE_L9 == 1 && LED_RE_L10 == 1)
        key_up = 1;

    return 0;
}

