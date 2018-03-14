#include "stm32f10x.h"
#include "bsp_common.h"

extern u8 UsartBuffer[USART_BUFFER_LEN];
extern u16 UsartWptr;
extern u16 UsartRptr;

extern u8 start_flash_flag;
extern u8 flag_dis_jump;

int main(void)
{
    u8 data = 0;
    u16 i = 0;
    u8 ntmp[255] = {0};
    u8 ndat[255] = {0}; // 协议数据
    u16 nlen = 0; // 协议数据包长度
    u16 ncrc = 0; // 协议crc16
    u16 ncmd = 0; // 协议指令
    __enable_irq();
    GPIO_Configure();
    NVIC_Configure();
    USART_Configure();
    //if(FLASH_GetReadOutProtectionStatus() != SET)
    //{
    //FLASH_Unlock();
    //FLASH_ReadOutProtection(ENABLE);
    //}
    sprintf((char*)ndat, "%s.%s%s\r\n", Version_Year, Version_Month, Version_Day);
    USART_SendBytes(USART1, ndat, strlen((char*)ndat));
    start_flash_flag = None_Flash_Bod;
    //IAP_Reset_UpdateFLAG();

    if(IAP_Read_UpdateFLAG() == 1)
    {
        __disable_irq();
        IAP_JumpToApplication();
    }
    else
    {
        while(1)
        {
            if(USART_BufferRead(&data) == 1)
            {
                if(start_flash_flag == None_Flash_Bod)
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
                else if(start_flash_flag == Start_Flash_Bod) // 开始升级
                {
                    IAP_Start_Program_Flash(data);
                    //USART_SendByte(USART1,data);
                }
                else if(start_flash_flag == Stop_Flash_Bod) // 结束升级
                {
                    if(flag_dis_jump == 0)
                    {
                        flag_dis_jump = 1;
                        start_flash_flag = None_Flash_Bod;
                        //Send_CMD(USART2, 0x02, LBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji));
                        USART_SendBytess(USART1, "stop update\r\n");
                        //Send_CMD(USART1, 0x02, LBYTE(USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji));
                        //for(i = 0; i < 30 * 1024; i = i + 4)
                        //{
                        //memset(strtemp, 0, sizeof(strtemp));
                        //sprintf((char *)strtemp, "%08x ", *(__IO uint32_t*)(FLASH_APP_ADDRESS + i));
                        //USART_SendBytess(USART1, (char *)strtemp);
                        //}
                        //IAP_Write_UpdateFLAG();
                        __disable_irq();
                        IAP_JumpToApplication();
                    }
                }
            }
        }
    }
}




