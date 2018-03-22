#include "bsp_usart.h"

//除了串口2，其他串口发送信息的缓存
u8 UsartBuffer[USART_BUFFER_LEN] = {0}; //数据缓冲区
u16 UsartWptr = 0;
u16 UsartRptr = 0;

#if (USE_COIN == 1)
//纸币器和硬币器POLL指令，回复数据专用缓存
u8 BUF_POLL[20] = {0};
//硬币器TUBE STATUS指令，回复数据专用缓存
u8 BUF_0A[30] = {0};
u8 Wptr_YING = 0;     //硬币器写指针
u16 Wptr_mode = 0;      //硬币器发送串口指令模式位，默认为0
u8 price_num = 0;       //货物价格
//纸币器和硬币器某些指令，回复数据共用缓存
u8 BUF_common[40] = {0};
char dat_quehuo[3] = {0};     //缓存取货几行几列，用于硬币器使用
#endif

bool flag_huodao_det = FALSE;       //货道电机单独测试，默认为带有硬币器和纸币器的正常出货
bool flag_chu_fail = FALSE;        //出货失败标志位，电机->主控，默认为0
bool flag_chu_success = FALSE;     //出货成功标志位，电机->主控，默认为0
bool flag_take_huowu = FALSE;       //取货标志位，安卓->主控，取货，对应标志位置一
u8 flag_test = 0;                 //调试标记位，用于PC机调试，根据不同值执行不同动作
u8 start_flash_flag = 0;
bool flag_enable_debug = FALSE;

bool flag_quhuo = TRUE;     //安卓取货标志位，默认可以取货

//printf函数重定向到串口1
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); //循环发送,直到发送完毕

    USART1->DR = (u8) ch;
    return ch;
}
#endif

//功能: USART1中断处理函数
//说明：串口1接GPRS，目前作为PC调试接口
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
        flag_test = nTemp;          //测试标志位,根据不同的串口数值，执行不同的动作
//        printf("flag_test : %d \r\n", flag_test);        //打印标记调试位
        USART_BufferWrite(nTemp);
//        USART_SendByte(USART1, nTemp);
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

//说明：串口2扩展为4路，之前的温度控制接232小板作为PC调试,现在作为纸币器通信
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
#if (USE_COIN == 1)
        rev_data_len++;     //硬币器回复数据计数
        BufWrite_COIN(nTemp);         //纸币器，硬币器回复数据专用缓存
#endif
    }

    if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET) //overflow
    {
        USART_ReceiveData(USART2);    // delete data
        USART_ClearFlag(USART2, USART_FLAG_ORE);
    }

#if SYSTEM_SUPPORT_UCOS
    OSIntExit();
#endif
}
#endif

//说明：串口3接安卓板
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
        USART_BufferWrite(nTemp);
//        USART_SendByte(USART1, nTemp);
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

//说明：串口4接电机板
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
        USART_BufferWrite(nTemp);
//        USART_SendByte(USART1, nTemp);      //硬币器和纸币器回复的信息，串口实时打印到PC
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

//说明：串口5接温控
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

void IAP_Reset_UpdateFLAG(void)
{
    uint32_t Address = 0x00;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    Address = UPDATE_FLAG_FLASH_ADDR;
    FLASH_ErasePage(Address);
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

//USART发送单字节
void USART_SendByte(USART_TypeDef* USARTx, uint8_t byte)
{
    /* 发送一个字节数据到USART */
    USART_SendData(USARTx, byte);

    /* 等待发送数据寄存器为空 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

//USART发送9位数据
void USART_Send2Byte(USART_TypeDef* USARTx, uint16_t byte)
{
    /* 发送一个字节数据到USART */
    USART_SendData(USARTx, byte);

    /* 等待发送数据寄存器为空 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

//USART发送长度字符串
void USART_SendBytes(USART_TypeDef* USARTx, uint8_t *str, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < len; i++)
    {
        USART_SendByte(USARTx, str[i]);
    }
}

//USART发送字符串
void USART_SendBytess(USART_TypeDef* USARTx, char *str)
{
    while(*str != '\0')
    {
        USART_SendByte(USARTx, *str);
        str++;
    }
}

//打印调试信息
void USART_DEBUG(char *str)
{
    if(flag_enable_debug)
    {
        while(*str != '\0')
        {
            USART_SendByte(USART1, *str);   //串口2改为串口1作为PC调试,串口2作为投币器和纸币器通信
            str++;
        }
    }
}

void Send_CMD(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD)//无数据区数据包
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

void Send_CMD_DAT(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD, char *dat, u16 dat_len)//完整数据包
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

u8 USART_BufferRead(u8 *data)
{
    if(UsartRptr == UsartWptr) // empty
    {
        return 0;
    }

    *data = UsartBuffer[UsartRptr];
    UsartRptr = (UsartRptr + 1) % USART_BUFFER_LEN; //保证读位置值不溢出
    return 1;
}

u16 USART_BufferLength(void)
{
    return (UsartWptr - UsartRptr + USART_BUFFER_LEN) % USART_BUFFER_LEN;
}


void USART_BufferWrite(u8 ntemp)
{
    //char strtemp[32] = {0};
    if((UsartWptr + 1) % USART_BUFFER_LEN == UsartRptr) // full
    {
        return;
    }

    UsartBuffer[UsartWptr] = ntemp;

    if(UsartBuffer[UsartWptr] == 0x0A && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 1) % USART_BUFFER_LEN] == 0x0D
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 2) % USART_BUFFER_LEN] == 0x20 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 3) % USART_BUFFER_LEN] == 0x38
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 4) % USART_BUFFER_LEN] == 0x02 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 5) % USART_BUFFER_LEN] == 0x00
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 6) % USART_BUFFER_LEN] == 0x12 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 7) % USART_BUFFER_LEN] == 0x01)
    {
        //复位主控
        USART_DEBUG("zhukong reset \r\n");
        __disable_irq();
        NVIC_SystemReset();
    }
    else if(UsartBuffer[UsartWptr] == 0x0A && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 1) % USART_BUFFER_LEN] == 0x0D
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 2) % USART_BUFFER_LEN] == 0x71 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 3) % USART_BUFFER_LEN] == 0xF8
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 4) % USART_BUFFER_LEN] == 0x02 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 5) % USART_BUFFER_LEN] == 0x00
            && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 6) % USART_BUFFER_LEN] == 0x13 && UsartBuffer[(USART_BUFFER_LEN + UsartWptr - 7) % USART_BUFFER_LEN] == 0x01)
    {
        // 复位电机
        Send_CMD(UART4, 0x02, 0x13);
        USART_DEBUG("dianji reset: 0213\r\n");
    }

    if(UsartBuffer[UsartRptr] == 0x0A && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 1) % USART_BUFFER_LEN] == 0x0D
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 2) % USART_BUFFER_LEN] == 0xA0 && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 3) % USART_BUFFER_LEN] == 0x2E
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 4) % USART_BUFFER_LEN] == 0x02 && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 5) % USART_BUFFER_LEN] == 0x00
            && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 6) % USART_BUFFER_LEN] == 0x5A && UsartBuffer[(USART_BUFFER_LEN + UsartRptr - 7) % USART_BUFFER_LEN] == 0x01)
    {
        // 结束电机升级
        start_flash_flag = 0;
        UsartWptr = 0;
        UsartRptr = 0;
        Send_CMD(UART4, 0x02, 0x5A);
        USART_DEBUG("stop update dianji: 025A\r\n");
    }

    UsartWptr = (UsartWptr + 1) % USART_BUFFER_LEN;
}

#if (USE_COIN == 1)
//功能：缓存串口2接收到的硬币器回复的信息
//说明：硬币器收到命令后，会回复信息给我们，我们每条指令用一个单独的BUF来接收。
//发送串口指令时，我们会指定mode位
void BufWrite_COIN(u8 ntemp)
{
    switch(Wptr_mode)       //Wptr_mode为发送指令模式位
    {
        case 0x0036 :
        case 0x000A :
            BUF_0A[Wptr_YING++] = ntemp;
            break;

        case 0x0033 :
        case 0x000B :
            BUF_POLL[Wptr_YING++] = ntemp;      //纸币器和硬币器POLL指令，回复共用缓存
            break;

        case 0x0009 :
        case 0x0F00 :
        case 0x0F03 :
            BUF_common[Wptr_YING++] = ntemp;        //回复信息写入公共缓存区
            break;

        case 0x0F04 :
            BUF_common[Wptr_YING++] = ntemp;        //回复信息写入缓存区
            break;

        case 0x0F05 :
            BUF_common[Wptr_YING++] = ntemp;
            break;

        default :
            break;
    }
}

#endif

//功能：串口协议命令处理
void Handle_USART_CMD(u16 Data, char *Dat, u16 dat_len)
{
    char str_dat[10] = {0};
    sprintf(strtemp, "Data: %04X\r\n", Data);
    USART_DEBUG(strtemp);
    memset(strtemp, 0, sizeof(strtemp));

    if(dat_len != 0) // 处理数据区域
    {
        if(Data == DIANJI_ZHUKON_NUMb6)//货道检测异常
        {
            Send_CMD_DAT(USART3, HBYTE(ZHUKON_ANZHUO_NUMb6), LBYTE(ZHUKON_ANZHUO_NUMb6), Dat, dat_len);
            sprintf(strtemp, "ZHUKON_ANZHUO_NUMb6: %04X\r\n", ZHUKON_ANZHUO_NUMb6);
            USART_DEBUG(strtemp);
        }

#if USE_COIN
        else if(Data == ANZHUO_ZHUKON_HANGLIE)  // 取"x行y列"货,发送到电机板，需要校验投入的钱数
        {
            flag_take_huowu = TRUE;    //用于纸币器和硬币器检测取货命令
            dat_quehuo[0] = *Dat;       //取货行号
            dat_quehuo[1] = *(Dat + 1); //取货列号
            price_num = *(Dat + 2);     //货物价格
//            sprintf(strtemp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d %d\r\n", ZHUKON_DIANJI_HANGLIE, Dat[0], Dat[1], Dat[2]);
//            USART_DEBUG(strtemp);

            if(flag_huodao_det == TRUE)     //单独测试货道电机运行
            {
                Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), dat_quehuo, 2);     //主控->电机，取货
                sprintf((char*)strtemp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d %d\r\n", ZHUKON_DIANJI_HANGLIE, dat_quehuo[0], dat_quehuo[1], price_num);
                USART_DEBUG((char*)strtemp);
            }
        }

#endif
        else if(Data == ANZHUO_ZHUKON_QUHUO)  // 取"x行y列"货,发送到电机板，只管出货
        {
//            flag_take_huowu = TRUE;    //用于纸币器和硬币器检测取货命令
            str_dat[0] = *Dat;       //取货行号
            str_dat[1] = *(Dat + 1); //取货列号
            Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), str_dat, 2);     //主控->电机，取货
            sprintf((char*)strtemp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d\r\n", ZHUKON_DIANJI_HANGLIE, str_dat[0], str_dat[1]);
            USART_DEBUG((char*)strtemp);
//            flag_quhuo = FALSE;         //本次取货开始，本次取货没有结束，没法进行下次取货
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_GetDianjiVer) // 获取电机版本
        {
            sprintf(str_dat, "%s", Dat);
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_GetDianjiVer), LBYTE(USARTCMD_ANDROID_ZHUKONG_GetDianjiVer), str_dat, 7);
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_GetDianjiVer:%04X, %s\r\n", USARTCMD_ANDROID_ZHUKONG_GetDianjiVer, str_dat);
            USART_DEBUG(strtemp);
        }

#if USE_COIN
        else if(Data == USARTCMD_ANDROID_ZHUKONG_HUODAO) // 单独测试货道电机，不需要纸币器和硬币器参与
        {
            if(*Dat == 0xAA)
            {
                flag_huodao_det = TRUE;     //关闭硬币器和纸币器的使用，用于单独检测货道电机运行
                USART_SendBytess(USART1, "MOTOR huodao use\r\n");     //提示信息
            }
            else if(*Dat == 0x55)
            {
                flag_huodao_det = FALSE;    //开启硬币器和纸币器的使用，不检测货道电机运行，正常取货
                flag_take_huowu = FALSE;    //用于纸币器和硬币器检测取货命令,保证下次开启纸币器和硬币器检测后不会立即出货
                USART_SendBytess(USART1, "MOTOR huodao nouse\r\n");     //提示信息
            }
        }

#endif
    }
    else
    {
        if(Data == DIANJI_ZHUKON_NUMb1)//出货成功
        {
            flag_chu_success = TRUE;       //电机->主控，出货成功，用于硬币器
            flag_quhuo = TRUE;      //上次取货完成，可以进行下次取货，用于多次取货的场景
            Send_CMD(USART3, HBYTE(ZHUKON_ANZHUO_NUMb1), LBYTE(ZHUKON_ANZHUO_NUMb1));
            sprintf(strtemp, "ZHUKON_ANZHUO_NUMb1:%04X\r\n", ZHUKON_ANZHUO_NUMb1);
            USART_DEBUG(strtemp);
        }
        else if(Data == DIANJI_ZHUKON_NUMb2)//出货失败
        {
            flag_chu_fail = TRUE;         //电机->主控，出货失败，用于硬币器
            flag_quhuo = TRUE;      //上次取货完成，可以进行下次取货，用于多次取货的场景
            Send_CMD(USART3, HBYTE(ZHUKON_ANZHUO_NUMb2), LBYTE(ZHUKON_ANZHUO_NUMb2));
            sprintf(strtemp, "ZHUKON_ANZHUO_NUMb2:%04X\r\n", ZHUKON_ANZHUO_NUMb2);
            USART_DEBUG(strtemp);
        }
        else if(Data == DIANJI_ZHUKON_NUMb5)//层反馈异常
        {
            Send_CMD(USART3, HBYTE(ZHUKON_ANZHUO_NUMb5), LBYTE(ZHUKON_ANZHUO_NUMb5));
            sprintf(strtemp, "ZHUKON_ANZHUO_NUMb5:%04X\r\n", ZHUKON_ANZHUO_NUMb5);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong) //准备升级主控
        {
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong), LBYTE(USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong: %04X\r\n", USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong);
            USART_DEBUG(strtemp);
            IAP_Reset_UpdateFLAG();
            __disable_irq();
            NVIC_SystemReset();
        }
        else if(Data == USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji) //准备升级电机
        {
            Send_CMD(UART4, HBYTE(USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji), LBYTE(USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji));
            sprintf(strtemp, "USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji: %04X\r\n", USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji) //返回准备升级电机
        {
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji), LBYTE(USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji: %04X\r\n", USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji) //开始升级电机
        {
            Send_CMD(UART4, HBYTE(USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji), LBYTE(USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji));
            sprintf(strtemp, "USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji: %04X\r\n", USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji) //返回开始升级电机
        {
            start_flash_flag = 1;
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji), LBYTE(USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji: %04X\r\n", USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji) //返回结束升级电机
        {
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_StopUpdateDianji), LBYTE(USARTCMD_ANDROID_ZHUKONG_StopUpdateDianji));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_StopUpdateDianji: %04X\r\n", USARTCMD_ANDROID_ZHUKONG_StopUpdateDianji);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ANDROID_ZHUKONG_GetZhukongVer) // 主控版本
        {
            sprintf(str_dat, "%s.%s%s", Version_Year, Version_Month, Version_Day);
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_GetZhukongVer), LBYTE(USARTCMD_ANDROID_ZHUKONG_GetZhukongVer), str_dat, 7);
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_GetZhukongVer: %04X, %s\r\n", USARTCMD_ANDROID_ZHUKONG_GetZhukongVer, str_dat);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ANDROID_ZHUKONG_GetDianjiVer)  // 获取电机版本
        {
            Send_CMD(UART4, HBYTE(USARTCMD_ZHUKONG_DIANJI_GetDianjiVer), LBYTE(USARTCMD_ZHUKONG_DIANJI_GetDianjiVer));
            sprintf(strtemp, "USARTCMD_ZHUKONG_DIANJI_GetDianjiVer: %04X\r\n", USARTCMD_ZHUKONG_DIANJI_GetDianjiVer);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT) // 升降电机+门电机电压超压
        {
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_DIANJI1VOLT), LBYTE(USARTCMD_ANDROID_ZHUKONG_DIANJI1VOLT));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_DIANJI1VOLT:%04X\r\n", USARTCMD_ANDROID_ZHUKONG_DIANJI1VOLT);
            USART_DEBUG(strtemp);
        }
        else if(Data == USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT) // 货道电机电压超压
        {
            Send_CMD(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_DIANJI2VOLT), LBYTE(USARTCMD_ANDROID_ZHUKONG_DIANJI2VOLT));
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_DIANJI2VOLT:%04X\r\n", USARTCMD_ANDROID_ZHUKONG_DIANJI2VOLT);
            USART_DEBUG(strtemp);
        }

#if USE_COIN
        else if(Data == USARTCMD_ANDROID_ZHUKONG_GetBalance)    //安卓查询余额指令，回复安卓上次取货后，投入的金额
        {
            u16 coin_num = 0;
            coin_num = GetBalance();        //查询余额
            str_dat[0] = LBYTE(coin_num);    //余额低字节
            str_dat[1] = HBYTE(coin_num);    //余额高字节
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ANDROID_ZHUKONG_GetBalance), LBYTE(USARTCMD_ANDROID_ZHUKONG_GetBalance), str_dat, 2);
            sprintf(strtemp, "USARTCMD_ANDROID_ZHUKONG_GetBalance: %04X, %d\r\n", USARTCMD_ANDROID_ZHUKONG_GetBalance, coin_num);
            USART_DEBUG(strtemp);
        }

#endif
        else if(Data == 0x01FE) // 开启debug打印
        {
            if(!flag_enable_debug)
            {
                flag_enable_debug = TRUE;
            }

            USART_DEBUG("debug\r\n");
        }
        else if(Data == 0x01FD) // 关闭debug打印
        {
            flag_enable_debug = FALSE;
        }
        else if(Data == 0x01FF) // 软件复位
        {
            SoftwareRESET();
        }
    }
}

//功能：软件复位操作
void SoftwareRESET(void)
{
    __set_FAULTMASK(1);     // 关闭所有中断
    NVIC_SystemReset();     //软件复位，类似于按下RESET按键
}


