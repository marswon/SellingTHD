#include "bsp_zhibiqi.h"

//纸币器延时时间，单位为ms
#define TIME_DELAY_ZHI      100

/********************* 纸币器 ********************/
//u16 tmp_TUBE_ZHI = 0;      //纸币器钱管满状态缓存
//u8 tmp1_01_TUBE_ZHI = 0;    //纸币器1元钱数量，当前次检测值
//u8 tmp2_01_TUBE_ZHI = 0;    //纸币器1元钱数量，之前一次检测值
//u8 tmp1_05_TUBE_ZHI = 0;   //纸币器5元钱数量，当前次检测值
//u8 tmp2_05_TUBE_ZHI = 0;   //纸币器5元钱数量，之前一次检测值
//u8 tmp1_10_TUBE_ZHI = 0;    //纸币器10元钱数量，当前次检测值
//u8 tmp2_10_TUBE_ZHI = 0;    //纸币器10元钱数量，之前一次检测值
//u8 tmp1_20_TUBE_ZHI = 0;   //纸币器20元钱数量，当前次检测值
//u8 tmp2_20_TUBE_ZHI = 0;   //纸币器20元钱数量，之前一次检测值
//u8 num_01_ZHI = 0;     //投入1元的总数
//u8 num_05_ZHI = 0;     //投入5元的总数
//u8 num_10_ZHI = 0;    //投入10元的总数
//u8 num_20_ZHI = 0;    //投入20元的总数

//功能：纸币器初始化
//入口参数：
//说明：纸币器初始化函数，按照文档初始化流程
//void ZhiBiQi_Init(void)
//{
//    u8 coin_dat[4] = {0};
//    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_ZHI);
//#endif
//    Send_CMD_BASIC_coin(STATUS_ZHI, NULL);      //发送状态指令0x31
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_ZHI);
//#endif
//    Send_CMD_EXP_coin(IDENTIFICATION_ZHI, NULL);      //发送扩展指令0x3700
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_ZHI);
//#endif
//    Send_CMD_BASIC_coin(STACKER_ZHI, NULL);      //发送指令0x36
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_ZHI);
//#endif
//    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
//    coin_dat[1] = 0x0F;     //纸币器可使用纸币类型B0~B3，1,5,TIME_DELAY_ZHI,20
//    coin_dat[2] = 0x00;
//    coin_dat[3] = 0x0F;     //使用暂保留功能
//    Send_CMD_BASIC_coin(BILL_TYPE_ZHI, coin_dat);      //发送指令0x34
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_ZHI);
//#endif
////    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
////    while(1)
////    {
////        Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
////#if(FLAG_WAIT == 1)
////        delay_ms(TIME_DELAY_ZHI);
////#endif
////        delay_ms(500);
////    }
//}

void ZhiBiQi_Init(void)
{
    Send_POLL_ZHI();      //发送指令0x33
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_ZHI);
#endif
    Send_STATUS_ZHI();      //发送状态指令0x31
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_ZHI);
#endif
    Send_IDENTIFICATION_ZHI();      //发送扩展指令0x3700
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_ZHI);
#endif
    Send_STACKER_ZHI();      //发送指令0x36
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_ZHI);
#endif
    Send_BILL_TYPE_ZHI();   //发送指令0x34000f000f
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_ZHI);
#endif
//    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
//    while(1)
//    {
//        Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
//#if(FLAG_WAIT == 1)
//        delay_ms(TIME_DELAY_ZHI);
//#endif
//        delay_ms(500);
//    }
}


//功能：纸币器工作
//入口参数：
//说明：纸币器要想检测纸币，必须循环发送指定指令。否则，机器不会检测纸币
void ZhiBiQi_USE(void)
{
    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
    delay_ms(500);
}

//功能：发送复位指令30H
void Send_RESET_ZHI(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x30;  //对应模式位置1，表示地址字节
    num = 0x30;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送纸币器固有属性查看指令31H
void Send_STATUS_ZHI(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x31;  //对应模式位置1，表示地址字节
    num = 0x31;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送安全等级指令32H，数据区两字节
void Send_SECURITY_ZHI(void)
{
    u16 cmd = 0;
    u8 coin_dat[2] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x00;
    num = (u8)(0x32 + 0x00 + 0x00);
    cmd = (0x01 << 8) | 0x32;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 2);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送纸币器动作状态指令33H
void Send_POLL_ZHI(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x33;  //对应模式位置1，表示地址字节
    num = 0x33;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送纸币器可接收纸币类型指令34H，数据区4字节
void Send_BILL_TYPE_ZHI(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x0F;
    coin_dat[2] = 0x00;
    coin_dat[3] = 0x0F;
    num = (u8)(0x34 + 0x00 + 0x0f + 0x0f);
    cmd = (0x01 << 8) | 0x34;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 4);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送暂保留状态指令35H，数据区1字节
void Send_ESCROW_ZHI(u8 dat)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = (u8)(0x35 + dat);
    cmd = (0x01 << 8) | 0x35;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, dat);   //发送数据，对应支出硬币的数值
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送复位指令36H
void Send_STACKER_ZHI(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x36;  //对应模式位置1，表示地址字节
    num = 0x36;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X3700
void Send_IDENTIFICATION_ZHI(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x37 + 0x00;
    cmd = (0x01 << 8) | 0x37;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X00);           //发送副指令
    USART_SendByte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X3701和数据区4字节
void Send_FEATURE_ENABLE_ZHI(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x00;
    coin_dat[2] = 0x00;
    coin_dat[3] = 0x00;
    num = 0x37 + 0x01 + 0x00;
    cmd = (0x01 << 8) | 0x37;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X01);           //发送副指令
    USART_SendBytes(USART2, coin_dat, 4);
    USART_SendByte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X3702
void Send_IDENTIFICATION_ZHI2(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x37 + 0x02;
    cmd = (0x01 << 8) | 0x37;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X02);           //发送副指令
    USART_SendByte(USART2, num);           //发送CHK检验和
}





