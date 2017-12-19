#include "bsp_yingbiqi.h"

//硬币器，纸币器初始化，串口指令延时等待标志位
#define FLAG_WAIT   1
//硬币器延时时间，单位为ms
#define TIME_DELAY_YING     100

//串口2收到的纸币器和硬币器回复信息，通过串口1打印出去
u8 USART2_dat[50] = {0};
u8 rev_data_len = 0;        //串口2回复数据长度，用于没有收到数据继续发送
//u8 rev_data_0B = 0;         //POLL指令接收到的数据必须是0B 0B才可以

/********************* 硬币器 ********************/
u16 tmp_TUBE_YING = 0;      //硬币器钱管满状态缓存
//static u8 tmp1_05_TUBE_YING = 0;    //硬币器5角钱数量，当前次检测值
//static u8 tmp2_05_TUBE_YING = 0;    //硬币器5角钱数量，之前一次检测值
//static u8 tmp1_10_TUBE_YING = 0;   //硬币器1元钱数量，当前次检测值
//static u8 tmp2_10_TUBE_YING = 0;   //硬币器1元钱数量，之前一次检测值
u8 num_05_YING = 0;     //投入5角的总数
u8 num_10_YING = 0;    //投入1元的总数
//static u8 num_05_quehuo = 0;        //取货一次，投入的5角的数量
//static u8 num_10_quehuo = 0;       //取货一次，投入的1元的数量
extern char dat_quehuo[2];        //缓存取货几行几列，用于硬币器使用
extern bool flag_take_huowu;
extern bool flag_chu_success;
extern bool flag_chu_fail;


//功能：发送常规命令函数
//入口参数：basic_cmd为发送的常规命令字节，data为需要发送数据区常规指令的数据
//返回值：正常返回1，异常返回0
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址。命令后，接着CHK检验和
u8 Send_CMD_BASIC_coin(u16 basic_cmd, u8 *data)
{
    u16 cmd = 0;
    u8 dat_len = 0, i = 0;
    u8 dat[5] = {0};
    u8 num = 0;     //数据区总和

    if(IS_COIN_BASIC_COMMAND(basic_cmd) == 0)     //常规命令校验
    {
        return 0;   //如果不是定义的命令,会直接退出
    }

    //根据常规指令设置数据区长度，纸币器和硬币器
    if((COIN_TYPE_YING == basic_cmd) || (BILL_TYPE_ZHI == basic_cmd))
    {
        dat_len = DAT_COIN_TYPE;    //VMC数据长度为4
    }
    else if((DISPENSE_YING == basic_cmd) || (ESCROW_ZHI == basic_cmd))
    {
        dat_len = DAT_DISPENSE;    //VMC数据长度为1
    }
    else if(SECURITY_ZHI == basic_cmd)
    {
        dat_len = DAT_SECURITY_ZHI;    //VMC数据长度为2
    }

    cmd = (0x01 << 8) | basic_cmd;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART1, cmd);   //PC调试，发送对应地址字节

    if(dat_len != 0)    //常规指令对应数据区初始化及发送指令
    {
        for(i = 0; i < dat_len; i++)
        {
            dat[i] = data[i];
            USART_SendByte(USART2, dat[i]);     //发送基础指令数据区
            USART_SendByte(USART1, dat[i]);     //PC调试，发送基础指令数据区
            num += dat[i];      //检验和
        }
    }

    cmd = (basic_cmd + num) & 0x00ff;          //计算校验和
    USART_Send2Byte(USART2, cmd);           //发送CHK检验和
    USART_Send2Byte(USART1, cmd);           //PC调试，发送CHK检验和
//    delay_ms(10);
//    USART_Send2Byte(USART2, 0x00);       //ACK
//    USART_Send2Byte(USART1, 0x00);
    return 1;
}

//功能：发送扩展命令函数
//入口参数：exp_cmd为发送的扩展命令字节，data为需要发送数据区扩展指令的数据
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址
u8 Send_CMD_EXP_coin(u16 exp_cmd, u8 *data)
{
    u16 cmd = 0;
    u8 dat_len = 0, i = 0;
    u8 dat[5] = {0};
    u8 num = 0;     //数据区总和

    if((IS_COIN_EXP_COMMAND_YING(exp_cmd) == 0) && (IS_COIN_EXP_COMMAND_ZHI(exp_cmd) == 0))     //地址字节命令校验
    {
        return 0;   //如果不是定义的命令,会直接退出
    }

    //根据扩展指令设置数据区长度
    if((FEATURE_ENABLE_YING == exp_cmd) || (FEATURE_ENABLE_ZHI == exp_cmd))
    {
        dat_len = DAT_FEATURE_ENABLE_YING;
    }
    else if(PAYOUT_YING == exp_cmd)
    {
        dat_len = DAT_PAYOUT_YING;
    }

    if(IS_COIN_EXP_COMMAND_YING(exp_cmd) == 1)     //硬币器扩展指令
    {
        cmd = (0x01 << 8) | 0x0F;           //对应模式位置1，表示地址字节,所以扩展字节高位都是0x0F
        USART_Send2Byte(USART2, cmd);       //发送对应地址字节
        USART_Send2Byte(USART1, cmd);       //PC调试，发送对应地址字节
    }
    else
    {
        //纸币器扩展指令
        cmd = (0x01 << 8) | 0x37;           //对应模式位置1，表示地址字节,所以扩展字节高位都是0x37
        USART_Send2Byte(USART2, cmd);       //发送对应地址字节
        USART_Send2Byte(USART1, cmd);       //PC调试，发送对应地址字节
    }

//    if(IS_COIN_EXP_COMMAND_ZHI(exp_cmd) == 1)     //纸币器扩展指令
//    {
//        cmd = (0x01 << 8) | 0x37;           //对应模式位置1，表示地址字节,所以扩展字节高位都是0x37
//        USART_Send2Byte(USART2, cmd);       //发送对应地址字节
//        USART_Send2Byte(USART1, cmd);       //PC调试，发送对应地址字节
//    }
    cmd = (exp_cmd & 0xFF);             //副指令，实际测试结果不需要地址位置1
    USART_Send2Byte(USART2, cmd);       //发送副指令
    USART_Send2Byte(USART1, cmd);       //PC调试，发送副指令

    if(dat_len != 0)    //扩展指令对应数据区初始化及发送指令
    {
        for(i = 0; i < dat_len; i++)
        {
            dat[i] = data[i];
            USART_SendByte(USART2, dat[i]);     //发送扩展数据区
            USART_SendByte(USART1, dat[i]);     //PC调试，发送扩展数据区
            num += dat[i];      //检验和
        }
    }

    cmd = ((exp_cmd >> 0x08) + (exp_cmd & 0xFF) + num) & 0x00ff;       //计算校验和
    USART_SendByte(USART2, (u8) cmd);          //发送CHK检验和
    USART_SendByte(USART1, (u8) cmd);          //PC调试，发送CHK检验和
    return 1;
}

//功能：发送带地址的多个命令函数
//入口参数：cmd为发送命令的地址，len为命令长度
//
void Send_CMD_coin(u8 *cmd, u8 len)
{
}

//功能：硬币器初始化
//入口参数：
//说明：硬币器初始化函数，按照文档初始化流程
//void YingBiQi_Init(void)
//{
//    u8 coin_dat[4] = {0};
//    Send_CMD_BASIC_coin(RESET_YING, NULL);      //发送复位指令
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    Send_CMD_BASIC_coin(STATUS_YING, NULL);      //发送硬币器状态指令
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    delay_ms(1000);
//    rev_data_len = 0;   //清零，记录下一条指令回复的长度
//    Send_CMD_EXP_coin(IDENTIFICATION_YING, NULL);     //发送扩展指令0x0F00
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
//    coin_dat[1] = 0x00;
//    coin_dat[2] = 0x00;
//    coin_dat[3] = 0x03;
//    Send_CMD_EXP_coin(FEATURE_ENABLE_YING, coin_dat);     //发送扩展指令0x0F01和数据区
////    delay_ms(1000);
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    Send_CMD_BASIC_coin(TUBE_STATUS_YING, NULL);    //发送钱管状态指令，回复剩余各个钱管状态
////    delay_ms(1000);
////    delay_ms(1000);
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    Send_CMD_BASIC_coin(POLL_YING, NULL);    //回复机器动作类型
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    Send_CMD_EXP_coin(SEND_DIAGNOSTIC_YING, NULL);     //发送扩展指令0x0F05
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    coin_dat[0] = 0x00;
//    coin_dat[1] = 0x03;
//    coin_dat[2] = 0xFF;
//    coin_dat[3] = 0xFF;
//    Send_CMD_BASIC_coin(COIN_TYPE_YING, coin_dat);    //回复机器可用硬币类型
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
////    Send_CMD_EXP_coin(IDENTIFICATION_YING, NULL);     //发送扩展指令0x0F00
////#if(FLAG_WAIT == 1)
////    delay_ms(TIME_DELAY_YING);
////#endif
////    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
////    coin_dat[1] = 0x00;
////    coin_dat[2] = 0x00;
////    coin_dat[3] = 0x03;
////    Send_CMD_EXP_coin(FEATURE_ENABLE_YING, coin_dat);     //发送扩展指令0x0F01和数据区
////#if(FLAG_WAIT == 1)
////    delay_ms(TIME_DELAY_YING);
////#endif
//}

void YingBiQi_Init(void)
{
    Send_RESET_YING();      //发送复位指令
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
#endif
    Send_STATUS_YING();      //发送硬币器状态指令
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
#endif
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_IDENTIFICATION_YING();     //发送扩展指令0x0F00
#if(FLAG_WAIT == 1)
        delay_ms(TIME_DELAY_YING);
#endif

        if(rev_data_len == (LEN_IDENTIFICATION_YING + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

//    Send_FEATURE_ENABLE_YING();     //发送扩展指令0x0F01和数据区
    
    while(1)
    {
        Send_FEATURE_ENABLE_YING();     //发送扩展指令0x0F01和数据区
#if(FLAG_WAIT == 1)
        delay_ms(TIME_DELAY_YING);
#endif

        if(rev_data_len > 0)       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    while(1)
    {
        Send_TUBE_STATUS_YING();    //发送钱管状态指令0x0A，回复剩余各个钱管状态
#if(FLAG_WAIT == 1)
        delay_ms(TIME_DELAY_YING);
#endif

        if(rev_data_len == (LEN_TUBE_STATUS_YING + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }
    while(1)
    {
        Send_POLL_YING();    //回复机器动作类型0x0B
#if(FLAG_WAIT == 1)
        delay_ms(TIME_DELAY_YING);
#endif

        if(rev_data_len > 0)       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    while(1)
    {
        Send_SEND_DIAGNOSTIC_YING();     //发送扩展指令0x0F05
#if(FLAG_WAIT == 1)
        delay_ms(TIME_DELAY_YING);
#endif

        if(rev_data_len >= 3)       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    while(1)
    {
        Send_COIN_TYPE_YING();    //回复机器可用硬币类型0C0003FFFFh
#if(FLAG_WAIT == 1)
    delay_ms(TIME_DELAY_YING);
#endif
        if(rev_data_len > 0)       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

//    Send_CMD_EXP_coin(IDENTIFICATION_YING, NULL);     //发送扩展指令0x0F00
//
//#if(FLAG_WAIT == 1)
//    delay_ms(TIME_DELAY_YING);
//#endif
//    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
//    coin_dat[1] = 0x00;
//    coin_dat[2] = 0x00;
//    coin_dat[3] = 0x03;
//    Send_CMD_EXP_coin(FEATURE_ENABLE_YING, coin_dat);     //发送扩展指令0x0F01和数据区
}

//功能：硬币器工作
//入口参数：
//说明：硬币器要想检测硬币，必须循环发送指定指令。否则，机器不会检测硬币
//void YingBiQi_USE(void)
//{
//    u8 tmp = 0;     //暂存钱管钱数的差值
//    u16 num_coin = 0;   //投入钱币的金额
//    char strtmp[50] = {0};
//    u8 coin_dat[4] = {0};

//    while(1)
//    {
//        Send_CMD_BASIC_coin(TUBE_STATUS_YING, NULL);    //发送钱管状态指令，回复剩余各个钱管状态
////        USART_SendByte(USART2, 0x00);       //ACK
//#if(FLAG_WAIT == 1)
//        delay_ms(TIME_DELAY_YING);
//        USART2_COIN_BufCopy(USART2_dat, LEN_TUBE_STATUS_YING + 1);      //回复的信息和CHK检验和，多一个字节
//        tmp_TUBE_YING = MAKEWORD(USART2_dat[0], USART2_dat[1]);         //缓存钱管满状态
//        tmp1_05_TUBE_YING = USART2_dat[2];        //五角钱数量
//        tmp1_10_TUBE_YING = USART2_dat[3];       //一元钱数量
//        tmp = tmp1_05_TUBE_YING - tmp2_05_TUBE_YING;        //5角钱前后的差值

//        if(1 == (tmp1_05_TUBE_YING - tmp2_05_TUBE_YING))      //前后一次5角的差值
//        {
//            num_05_YING++;       //5角硬币计数
//            num_05_quehuo++;    //取货当次，投入5角的数量
////            printf("num_05_YING : %d num_05_quehuo : %d\r\n", num_05_YING, num_05_quehuo);      //输出5角数量
//        }

////        if(tmp > 0)      //前后一次5角的差值，与上面对比，避免出现前后增加多个的问题
////        {
////            num_05_YING = num_05_YING + tmp;       //5角硬币计数
////            num_05_quehuo = num_05_quehuo + tmp;
////            printf("num_05_YING : %d num_05_quehuo : %d\r\n", num_05_YING, num_05_quehuo);      //输出5角数量
////        }
//        tmp = tmp1_10_TUBE_YING - tmp2_10_TUBE_YING;        //一元钱前后的差值

//        if(1 == (tmp1_10_TUBE_YING - tmp2_10_TUBE_YING))      //前后一次一元的差值
//        {
//            num_10_YING++;       //1元硬币计数
//            num_10_quehuo++;    //取货当次，投入1元的数量
////            printf("num_10_YING : %d num_10_quehuo : %d\r\n", num_10_YING, num_10_quehuo);    //输出1元数量
//        }

////        if(tmp > 0)      //前后一次一元的差值
////        {
////            num_10_YING = num_10_YING + tmp;       //1元硬币计数
////            num_10_quehuo = num_10_quehuo + tmp;
////            printf("num_10_YING : %d num_10_quehuo : %d\r\n", num_10_YING, num_10_quehuo);    //输出1元数量
////        }
//        tmp2_05_TUBE_YING = tmp1_05_TUBE_YING;          //缓存5角当前值
//        tmp2_10_TUBE_YING = tmp1_10_TUBE_YING;          //缓存1元当前值
//#endif
//        delay_ms(1000);
//        Send_CMD_BASIC_coin(POLL_YING, NULL);    //回复机器动作类型
////        USART_SendByte(USART2, 0x00);       //ACK
//#if(FLAG_WAIT == 1)
//        delay_ms(TIME_DELAY_YING);
//#endif
//        delay_ms(1000);

//        if(flag_take_huowu == 1)        //安卓->主控，发送"取货"命令
//        {
//            num_coin = num_10_quehuo * 10 + num_05_quehuo * 5;      //当次投入的钱币的总额

//            if(num_coin >= 30)      //测试金额3元
//            {
//                flag_take_huowu = FALSE;    //判定该次取货完成
//                num_05_quehuo = 0;      //当次5角计数清零
//                num_10_quehuo = 0;      //当次1元计数清零
//                coin_dat[0] = 0x00;
//                coin_dat[1] = 0x00;
//                coin_dat[2] = 0xFF;
//                coin_dat[3] = 0xFF;
//                Send_CMD_BASIC_coin(COIN_TYPE_YING, coin_dat);    //发送"禁止收钱"指令
//                Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), dat_quehuo, 2);     //主控->电机，取货
//                sprintf(strtmp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d\r\n", ZHUKON_DIANJI_HANGLIE, dat_quehuo[0], dat_quehuo[1]);
//                USART_DEBUG(strtmp);
//            }
//            else
//            {
//                //取货，投入金额不足，主控->安卓
//                Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKON_ANZHUO_CoinNoEnough), LBYTE(USARTCMD_ZHUKON_ANZHUO_CoinNoEnough), dat_quehuo, 2);     //主控->电机，取货
////                sprintf(strtmp, "USARTCMD_ZHUKON_ANZHUO_CoinNoEnough: %04X,%d-%d\r\n", USARTCMD_ZHUKON_ANZHUO_CoinNoEnough, dat_quehuo[0], dat_quehuo[1]);
//                USART_DEBUG(strtmp);
//            }
//        }

//        if(flag_chu_success == 1)       //出货成功
//        {
//            flag_chu_success = FALSE;
//            coin_dat[0] = 0x00;
//            coin_dat[1] = 0x03;
//            coin_dat[2] = 0xFF;
//            coin_dat[3] = 0xFF;
//            Send_CMD_BASIC_coin(COIN_TYPE_YING, coin_dat);    //发送"可收钱"指令
//        }
//    }
//}

void YingBiQi_USE(void)
{
    while(1)
    {
        rev_data_len = 0;   //清零，记录下一条指令回复的长度

        while(1)
        {
            Send_TUBE_STATUS_YING();    //发送钱管状态指令0x0A，回复剩余各个钱管状态
#if(FLAG_WAIT == 1)
            delay_ms(TIME_DELAY_YING);

#endif

            if(rev_data_len == (LEN_TUBE_STATUS_YING + 1))       //判断接受的数据长度
            {
                rev_data_len = 0;   //清零，记录下一条指令回复的长度
                break;
            }

            rev_data_len = 0;   //清零，记录下一条指令回复的长度
        }

        delay_ms(1000);
//        delay_ms(500);
//        while(1)
//        {
//            Send_POLL_YING();    //回复机器动作类型0x0B
//#if(FLAG_WAIT == 1)
//            delay_ms(TIME_DELAY_YING);
////            delay_ms(1000);
////            delay_ms(500);
//#endif

//            if(rev_data_len > 0)       //判断接受的数据长度
//            {
//                rev_data_len = 0;   //清零，记录下一条指令回复的长度
//                break;
//            }

//            rev_data_len = 0;   //清零，记录下一条指令回复的长度
//        }
//        delay_ms(1000);
//        delay_ms(500);
//        while(1)
//        {
//            Send_SEND_DIAGNOSTIC_YING();     //发送扩展指令0x0F05
//#if(FLAG_WAIT == 1)
//            delay_ms(TIME_DELAY_YING);
////            delay_ms(20);
//#endif

//            if(rev_data_len >= 3)       //判断接受的数据长度
//            {
//                rev_data_len = 0;   //清零，记录下一条指令回复的长度
//                break;
//            }

//            rev_data_len = 0;   //清零，记录下一条指令回复的长度
//        }

//        delay_ms(1000);     //间隔1s发送0x0A和0x0B
    }
}

//功能：发送复位0X08指令
void Send_RESET_YING(void)
{
    u16 cmd = 0;
    cmd = (0x01 << 8) | 0x08;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X08);           //发送CHK检验和
}

//功能：发送投币器参数指令0x09
void Send_STATUS_YING(void)
{
    u16 cmd = 0;
    cmd = (0x01 << 8) | 0x09;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X09);           //发送CHK检验和
}

//功能：发送投币器参数指令0x0A
void Send_TUBE_STATUS_YING(void)
{
    u16 cmd = 0;
    cmd = (0x01 << 8) | 0x0A;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X0A);           //发送CHK检验和
}
//功能：发送投币器参数指令0x0b
void Send_POLL_YING(void)
{
    u16 cmd = 0;
    cmd = (0x01 << 8) | 0x0B;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X0B);           //发送CHK检验和
}
//功能：发送硬币类型0C0003FFFFh
void Send_COIN_TYPE_YING(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x03;
    coin_dat[2] = 0xff;
    coin_dat[3] = 0xff;
    num = (u8)(0x0c + 0x03 + 0xff + 0xff);
    cmd = (0x01 << 8) | 0x0c;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 4);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X0F00
void Send_IDENTIFICATION_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x0f + 0x00;
    cmd = (0x01 << 8) | 0x0f;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X00);           //发送副指令
    USART_SendByte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X0F01和数据区
void Send_FEATURE_ENABLE_YING(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x00;
    coin_dat[2] = 0x00;
    coin_dat[3] = 0x03;
    num = 0x0f + 0x01 + 0x03;
    cmd = (0x01 << 8) | 0x0f;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X01);           //发送副指令
    USART_SendBytes(USART2, coin_dat, 4);
    USART_SendByte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X0F05
void Send_SEND_DIAGNOSTIC_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x0f + 0x05;
    cmd = (0x01 << 8) | 0x0f;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X05);           //发送副指令
    USART_SendByte(USART2, num);           //发送CHK检验和
}

//功能：发送硬币类型0C0003FFFFh
void Send_COIN_DISENABLE_YING(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x00;
    coin_dat[2] = 0xff;
    coin_dat[3] = 0xff;
    num = (u8)(0x0c + 0x03 + 0xff + 0xff);
    cmd = (0x01 << 8) | 0x0c;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 4);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}

