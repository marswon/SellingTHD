#include "bsp_zhibiqi.h"

/********************* 纸币器 ********************/
u16 tmp_TUBE_ZHI = 0;      //纸币器钱管满状态缓存
u8 tmp1_01_TUBE_ZHI = 0;    //纸币器1元钱数量，当前次检测值
u8 tmp2_01_TUBE_ZHI = 0;    //纸币器1元钱数量，之前一次检测值
u8 tmp1_05_TUBE_ZHI = 0;   //纸币器5元钱数量，当前次检测值
u8 tmp2_05_TUBE_ZHI = 0;   //纸币器5元钱数量，之前一次检测值
u8 tmp1_10_TUBE_ZHI = 0;    //纸币器10元钱数量，当前次检测值
u8 tmp2_10_TUBE_ZHI = 0;    //纸币器10元钱数量，之前一次检测值
u8 tmp1_20_TUBE_ZHI = 0;   //纸币器20元钱数量，当前次检测值
u8 tmp2_20_TUBE_ZHI = 0;   //纸币器20元钱数量，之前一次检测值
u8 num_01_ZHI = 0;     //投入1元的总数
u8 num_05_ZHI = 0;     //投入5元的总数
u8 num_10_ZHI = 0;    //投入10元的总数
u8 num_20_ZHI = 0;    //投入20元的总数

//功能：纸币器初始化
//入口参数：
//说明：纸币器初始化函数，按照文档初始化流程
void ZhiBiQi_Init(void)
{
    u8 coin_dat[4] = {0};
    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
//    USART2_COIN_BufCopy(USART2_dat, LEN_POLL_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
//    USART2_dat[LEN_POLL_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
//    USART2_dat[LEN_POLL_ZHI + 2] = 0X0A;
//    //打印POLL指令的回复，回复16个字节
//    USART_SendBytes(USART1, USART2_dat, LEN_POLL_ZHI + 3);   //打印串口2接受的纸币器和投币器回复信息
//    memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
#endif
    Send_CMD_BASIC_coin(STATUS_ZHI, NULL);      //发送状态指令0x31
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
//    USART2_COIN_BufCopy(USART2_dat, LEN_STATUS_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
//    USART2_dat[LEN_STATUS_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
//    USART2_dat[LEN_STATUS_ZHI + 2] = 0X0A;
//    //打印POLL指令的回复，回复16个字节
//    USART_SendBytes(USART1, USART2_dat, LEN_POLL_YING + 3);   //打印串口2接受的纸币器和投币器回复信息
//    memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
#endif
    Send_CMD_EXP_coin(IDENTIFICATION_ZHI, NULL);      //发送扩展指令0x3700
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
//    USART2_COIN_BufCopy(USART2_dat, LEN_IDENTIFICATION_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
//    USART2_dat[LEN_IDENTIFICATION_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
//    USART2_dat[LEN_IDENTIFICATION_ZHI + 2] = 0X0A;
//    //打印POLL指令的回复，回复16个字节
//    USART_SendBytes(USART1, USART2_dat, LEN_IDENTIFICATION_ZHI + 3);   //打印串口2接受的纸币器和投币器回复信息
//    memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
#endif
    Send_CMD_BASIC_coin(STACKER_ZHI, NULL);      //发送指令0x36
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
//    USART2_COIN_BufCopy(USART2_dat, LEN_STACKER_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
//    USART2_dat[LEN_STACKER_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
//    USART2_dat[LEN_STACKER_ZHI + 2] = 0X0A;
//    //打印POLL指令的回复，回复16个字节
//    USART_SendBytes(USART1, USART2_dat, LEN_STACKER_ZHI + 3);   //打印串口2接受的纸币器和投币器回复信息
//    memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
#endif
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x0F;     //纸币器可使用纸币类型B0~B3，1,5,TIME_DELAY_YING,20
    coin_dat[2] = 0x00;
    coin_dat[3] = 0x0F;     //使用暂保留功能
    Send_CMD_BASIC_coin(BILL_TYPE_ZHI, coin_dat);      //发送指令0x34
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
//    USART2_COIN_BufCopy(USART2_dat, LEN_BILL_TYPE_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
//    USART2_dat[LEN_BILL_TYPE_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
//    USART2_dat[LEN_BILL_TYPE_ZHI + 2] = 0X0A;
//    //打印POLL指令的回复，回复16个字节
//    USART_SendBytes(USART1, USART2_dat, LEN_BILL_TYPE_ZHI + 3);   //打印串口2接受的纸币器和投币器回复信息
//    memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
#endif
//    Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
//    while(1)
//    {
//        Send_CMD_BASIC_coin(POLL_ZHI, NULL);      //发送指令0x33
//#if(FLAG_WAIT == 1)
//        delay_ms(TIME_DELAY_YING);
////        USART2_COIN_BufCopy(USART2_dat, LEN_POLL_ZHI + 2);      //回复的信息和CHK检验和，多一个字节
////        USART2_dat[LEN_POLL_ZHI + 1] = 0X0D;     //用于刷新PC缓冲区，实现指令立即显示
////        USART2_dat[LEN_POLL_ZHI + 2] = 0X0A;
////        //打印POLL指令的回复，回复16个字节
////        USART_SendBytes(USART1, USART2_dat, LEN_POLL_ZHI + 3);   //打印串口2接受的纸币器和投币器回复信息
////        memset(USART2_dat, 0, sizeof(USART2_dat));      //全部清零
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
