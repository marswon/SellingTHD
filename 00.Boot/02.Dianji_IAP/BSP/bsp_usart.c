#include "bsp_common.h"

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
u32 JumpAddress; //跳转地址

u8 UsartBuffer[USART_BUFFER_LEN] = {0}; //数据缓冲区
u16 UsartWptr = 0;
u16 UsartRptr = 0;

u8 start_flash_flag = 0;
u8 write_flash_data_bit = 0;
u8 datalow = 0;
u8 datahigh = 0;
u32 flashwptr = 0;

u8 flag_dis_jump = 1; // 禁止处理升级结束指令

u8 USART_BufferRead(u8 *data)
{
    if(UsartRptr == UsartWptr) // empty
    {
        return 0;
    }

    *data = UsartBuffer[UsartRptr];
    //UsartRptr++;
    UsartRptr = (UsartRptr + 1) % USART_BUFFER_LEN; //保证读位置值不溢出
    return 1;
}

void USART_BufferWrite(u8 ntemp)
{
    //if(UsartWptr == (UsartRptr - 1))
    if((UsartWptr + 1) % USART_BUFFER_LEN == UsartRptr) // full
    {
        return;
    }

    UsartBuffer[UsartWptr] = ntemp;

    if(UsartBuffer[UsartWptr] == 0x0A && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 1) % USART_BUFFER_LEN] == 0x0D
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 2) % USART_BUFFER_LEN] == 0x71 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 3) % USART_BUFFER_LEN] == 0xBC
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 4) % USART_BUFFER_LEN] == 0x02 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 5) % USART_BUFFER_LEN] == 0x00
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 6) % USART_BUFFER_LEN] == 0x13 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 7) % USART_BUFFER_LEN] == 0x02)
    {
        // 复位
        flag_dis_jump = 1;
        start_flash_flag = None_Flash_Bod;
        __disable_irq();
        NVIC_SystemReset();
    }

    //UsartWptr++;
    //UsartWptr = UsartWptr % USART_BUFFER_LEN;
    UsartWptr = (UsartWptr + 1) % USART_BUFFER_LEN;

    if(UsartBuffer[UsartRptr] == 0x0A && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 1) % USART_BUFFER_LEN] == 0x0D
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 2) % USART_BUFFER_LEN] == 0xA0 && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 3) % USART_BUFFER_LEN] == 0x6A
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 4) % USART_BUFFER_LEN] == 0x02 && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 5) % USART_BUFFER_LEN] == 0x00
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 6) % USART_BUFFER_LEN] == 0x5A && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 7) % USART_BUFFER_LEN] == 0x02)
    {
        start_flash_flag = Stop_Flash_Bod;  //结束电机板升级
    }
}

/*
*********************************************************************************************************
*   函 数 名: USART1_IRQHandler
*   功能说明: USART1中断处理函数.
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/

#if USART1_CONFIG_ENABLED > 0
void USART1_IRQHandler(void)
{
    uint8_t nTemp;
#if SYSTEM_SUPPORT_UCOS
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        nTemp = USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); //clear flag
        /************************************************/
        USART_BufferWrite(nTemp);
        //USART_SendByte(USART1,nTemp);
    }

    if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(USART1);    // delete data
        USART_ClearFlag(USART1, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

#if USART2_CONFIG_ENABLED > 0
void USART2_IRQHandler(void)
{
    uint8_t nTemp;
#if SYSTEM_SUPPORT_UCOS
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        nTemp = USART_ReceiveData(USART2);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); //clear flag
        /************************************************/
        USART_BufferWrite(nTemp);
    }

    if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(USART2);    // delete data
        USART_ClearFlag(USART1, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

#if USART3_CONFIG_ENABLED > 0
void USART3_IRQHandler(void)
{
    uint8_t nTemp;
#if SYSTEM_SUPPORT_UCOS
    OSIntEnter();
#endif

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        nTemp = USART_ReceiveData(USART3);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); //clear flag
        /************************************************/
        //USART_BufferWrite(nTemp);
    }

    if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(USART3);    // delete data
        USART_ClearFlag(USART3, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

#if UART4_CONFIG_ENABLED > 0
void UART4_IRQHandler(void)
{
    uint8_t nTemp;
#if SYSTEM_SUPPORT_UCOS
    OSIntEnter();
#endif

    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        nTemp = USART_ReceiveData(UART4);
        USART_ClearITPendingBit(UART4, USART_IT_RXNE); //clear flag
        /************************************************/
        //USART_BufferWrite(nTemp);
    }

    if(USART_GetFlagStatus(UART4, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(UART4);    // delete data
        USART_ClearFlag(UART4, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

#if UART5_CONFIG_ENABLED > 0
void UART5_IRQHandler(void)
{
    uint8_t nTemp;
#if SYSTEM_SUPPORT_UCOS
    OSIntEnter();
#endif

    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        nTemp = USART_ReceiveData(UART5);
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); //clear flag
        /************************************************/
        //USART_BufferWrite(nTemp);
    }

    if(USART_GetFlagStatus(UART5, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(UART5);    // delete data
        USART_ClearFlag(UART5, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

//USART发送单字节
void USART_SendByte(USART_TypeDef* USARTx, uint8_t byte)
{
    /* 发送一个字节数据到USART */
    USART_SendData(USARTx, byte);

    /* 等待发送数据寄存器为空 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

//USART发送字符串
void USART_SendBytes(USART_TypeDef* USARTx, uint8_t *str, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < len; i++)
    {
        USART_SendByte(USARTx, str[i]);
    }
}

void USART_SendBytess(USART_TypeDef* USARTx, char *str)
{
    while(*str != '\0')
    {
        USART_SendByte(USARTx, *str);
        str++;
    }
}

void Send_CMD(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD)
{
    u8 str[8] = {0};
    u8 cnt = 0;
    u16 ncrc = 0;
    str[0] = HCMD;
    str[1] = LCMD;
    str[2] = 0;
    str[3] = 2;
    ncrc = CRC16_isr(&str[0], 4);
    str[4] = HBYTE(ncrc);
    str[5] = LBYTE(ncrc);
    str[6] = 0x0D;
    str[7] = 0x0A;

    for(cnt = 0; cnt < 8; cnt++)
    {
        USART_SendByte(USARTx, str[cnt]);
    }
}

void Send_CMD_DAT(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD, char *dat, u16 dat_len)
{
    u8 cnt = 0;
    u16 ncrc = 0;
    u8 str[255] = {0};
    u16 datalen = 0;
    str[0] = HCMD;
    str[1] = LCMD;
    memcpy(&str[2], &dat[0], dat_len);
    datalen = dat_len + 2;
    str[dat_len + 2] = HBYTE(datalen);
    str[dat_len + 3] = LBYTE(datalen);
    ncrc = CRC16_isr(&str[0], 4 + dat_len);
    str[dat_len + 4] = HBYTE(ncrc);
    str[dat_len + 5] = LBYTE(ncrc);
    str[dat_len + 6] = 0x0D;
    str[dat_len + 7] = 0x0A;

    for(cnt = 0; cnt < 8 + dat_len; cnt++)
    {
        USART_SendByte(USARTx, str[cnt]);
    }
}

void IAP_Write_UpdateFLAG(void)
{
    uint32_t Address = 0x00;
    uint32_t Data = UPDATE_FLAG_FLASH_DATA;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    Address = UPDATE_FLAG_FLASH_ADDR;
    FLASH_ErasePage(Address);
    FLASH_ProgramWord(Address, Data);
    FLASH_Lock();
}

u8 IAP_Read_UpdateFLAG(void)
{
    uint32_t Address = 0x00;
    uint32_t Data = UPDATE_FLAG_FLASH_DATA;
    uint32_t Data1 = UPDATE_FLAG_FLASH_DATA1;
    Address = UPDATE_FLAG_FLASH_ADDR;

    if(((*(__IO uint32_t*) Address) == Data) || ((*(__IO uint32_t*) Address) == Data1))
    {
        return 1;
    }
    else
    {
        return 0;
    }

    //if((*(__IO uint32_t*) Address) != Data)
    //{
    //return 0;
    //}
    //else
    //{
    //return 1;
    //}
}

void IAP_Reset_UpdateFLAG(void)
{
    uint32_t Address = 0x00;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    Address = UPDATE_FLAG_FLASH_ADDR;
    FLASH_ErasePage(Address);
    FLASH_Lock();
}

void IAP_Erase_APP_Flash(void)
{
    u16 eraseCounter = 0;
    u16 nbrOfPage = 0;
    nbrOfPage = (u16)(APP_FLASH_SIZE) / PAGE_SIZE; //计算page数
    FLASH_Unlock(); //解除flash擦写锁定
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位

    for(eraseCounter = 0; eraseCounter < nbrOfPage; eraseCounter++)//开始擦除
    {
        FLASH_ErasePage(FLASH_APP_ADDRESS + (eraseCounter * PAGE_SIZE));//擦除
    }

    FLASH_Lock();//flash擦写锁定
}
void IAP_Program_FlashData(u16 data)
{
    FLASH_Unlock();//flash上锁
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
    FLASH_ProgramHalfWord(flashwptr, data); //烧写u16数据
    flashwptr = flashwptr + 2;//移动烧写地址
    FLASH_Lock();//flash解锁
}

void IAP_Start_Program_Flash_Init(void)
{
    u8 blockNum = (u8)(FLASH_APP_ADDRESS - FLASH_IAP_ADDRESS) >> 12;   //计算flash块
    u32 UserMemoryMask = ((u32)(~((1 << blockNum) - 1)));//计算掩码
    IAP_Erase_APP_Flash();
    write_flash_data_bit = 0;
    datalow = 0;
    datahigh = 0;
    flashwptr = FLASH_APP_ADDRESS;

    //查看块所在区域是否写保护
    if((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
    {
        FLASH_EraseOptionBytes();  //关闭写保护
    }
}

void IAP_Start_Program_Flash(u8 data)
{
    switch(write_flash_data_bit)
    {
        case 0:
            datalow = data; //接收低字节数据
            write_flash_data_bit = 1;
            break;

        case 1:
            datahigh = data; //接收高字节数据
            write_flash_data_bit = 0;
            IAP_Program_FlashData(((u16)(datalow)) | ((u16)(datahigh << 8)));

            if(start_flash_flag == None_Flash_Bod)
            {
                datahigh = 0xff;
                write_flash_data_bit = 0;
                IAP_Program_FlashData(((u16)(datalow)) | ((u16)(datahigh << 8)));
            }

            break;

        default:
            break;
    }
}

void IAP_JumpToApplication(void)
{
    if(((*(__IO u32 *)FLASH_APP_ADDRESS) & 0x2FFE0000) == 0x20000000)//有升级代码，IAP_ADDR地址处理应指向主堆栈区，即0x20000000
    {
        JumpAddress = *(__IO u32 *)(FLASH_APP_ADDRESS + 4);//获取复位地址
        Jump_To_Application = (pFunction)JumpAddress;//函数指针指向复位地址
        __set_MSP(*(__IO u32*)FLASH_APP_ADDRESS);  //设置主堆栈指针MSP指向升级机制APP_ADDRESS
        Jump_To_Application();  //跳转到升级代码处
    }
}

void IAP_JumpToIAP(void)
{
    JumpAddress = *(__IO u32 *)(FLASH_IAP_ADDRESS + 4);
    Jump_To_Application = (pFunction)JumpAddress;
    __set_MSP(*(__IO u32*)FLASH_IAP_ADDRESS);
    Jump_To_Application(); //跳转回IAP代码处
}

void Handle_USART_CMD(u16 Data, char *Dat, u16 dat_len)
{
    if(dat_len != 0) // 处理数据区域
    {
    }
    else  // 处理指令
    {
        if(Data == USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji) // will升级电机
        {
            flag_dis_jump = 1;
            Send_CMD(USART2, 0x02, LBYTE(USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji));
            USART_SendBytess(USART1, "will update\r\n");
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji) // 开始升级电机
        {
            flag_dis_jump = 0;
            start_flash_flag = Start_Flash_Bod;
            IAP_Start_Program_Flash_Init();
            Send_CMD(USART2, 0x02, LBYTE(USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji));
            USART_SendBytess(USART1, "start update\r\n");
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_GetDianjiVer) // 获取电机版本
        {
            char strstr[10] = {0};
            sprintf(strstr, "%s.%s%s", Version_Year, Version_Month, Version_Day);
            Send_CMD_DAT(USART2, 0x02, LBYTE(USARTCMD_ZHUKONG_DIANJI_GetDianjiVer), strstr, 7);
            USART_SendBytess(USART1, strstr);
        }
    }

    Data = 0;
}

