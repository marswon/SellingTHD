#include "bsp_zhibiqi.h"

//纸币器使用的标志位，默认都是不需要使用的
#define flag_ZhiBiQi_USE   0

/********************* 纸币器 ********************/
extern char strtmp[100];     //打印调试信息缓存信息
static u16 num_ZHIBI = 0;          //现金盒纸币的张数

//功能：纸币器初始化
//入口参数：
//说明：纸币器初始化函数，按照文档初始化流程
void ZhiBiQi_Init(void)
{
    u8 rev = 0;
    DET_RESET_ZHI();        //复位RESET
    DET_STATUS_ZHI();       //纸币器参数31H
    DET_IDENTIFICATION_ZHI();       //指令3700H，返回厂家信息

    do
    {
        rev = DET_STACKER_ZHI(&num_ZHIBI);      //指令36H，返回现金盒状态
    }
    while(rev == 0);        //CHK有误

    if(rev == 2)    //纸币器钱盒装满
    {
        Send_CMD(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_ZBQ_FULL), LBYTE(USARTCMD_ZHUKONG_ANDROID_ZBQ_FULL));     //主控->安卓，纸币器钱盒装满
        USART_DEBUG("ZHIBIQI : full\r\n");
    }

    DET_BILL_TYPE_ZHI(1);        //设置现金盒可装币种和可暂保留币种34H

    while(!(1 == DET_POLL_ZHI()));       //指令33H,初始化必须接收到ACK
}

#if (flag_ZhiBiQi_USE == 1)
//功能：纸币器单独工作
//入口参数：
//说明：纸币器要想检测纸币，必须循环发送指定指令。否则，机器不会检测纸币
void ZhiBiQi_USE(void)
{
    u8 rev = 0;
    static u8 num_ZHIBI = 0;     //纪录投入纸币的金额
    u16 num_coin = 0;
    static u8 balance = 0;  //投入货币取货后投入的余额
    rev = DET_POLL_ZHI();       //POLL指令，33H

    switch(rev)
    {
        case 5:
        case 6:
        case 7:
        case 8:
            DET_BILL_TYPE_ZHI(2);       //禁止收钱
            DET_ESCROW_ZHI(1);      //接收钱
            break;

        case 9:
            num_ZHIBI += 1;          //钱数自增1元
            DET_BILL_TYPE_ZHI(1);       //发送可收钱
            break;

        case 10:
            num_ZHIBI += 5;          //钱数自增5元
            DET_BILL_TYPE_ZHI(1);       //发送可收钱
            break;

        case 11:
            num_ZHIBI += 10;         //钱数自增10元
            DET_BILL_TYPE_ZHI(1);       //发送可收钱
            break;

        case 12:
            num_ZHIBI += 20;         //钱数自增20元
            DET_BILL_TYPE_ZHI(1);       //发送可收钱
            break;

        default:
            break;
    }

    sprintf((char*)strtmp, "num_ZHIBI : %d\r\n", num_ZHIBI);    //打印投入金额
    USART_DEBUG((char*)strtmp);

    if(flag_take_huowu == TRUE)        //安卓->主控，发送"取货"命令
    {
//        sprintf((char*)strtmp, "num_ZHIBI : %d\r\n", num_ZHIBI);    //打印投入金额
//        USART_DEBUG((char*)strtmp);
        num_coin = num_ZHIBI * 10 + balance;       //取货，投入的钱数

        if(num_coin >= price_num)      //高于指定货物的价格
        {
            DET_BILL_TYPE_ZHI(2);       //禁止收钱
            Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), dat_quehuo, 2);     //主控->电机，取货
            sprintf((char*)strtmp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d %d\r\n", ZHUKON_DIANJI_HANGLIE, dat_quehuo[0], dat_quehuo[1], price_num);
            USART_DEBUG((char*)strtmp);
            balance = num_coin - price_num;     //更新余额
            sprintf((char*)strtmp, "balance : %d\r\n", balance);    //打印余额
            USART_DEBUG((char*)strtmp);
            num_ZHIBI = 0;       //清零
            flag_take_huowu = FALSE;    //判定该次取货完成
            delay_ms(1000);         //延时1s
            DET_BILL_TYPE_ZHI(1);       //发送可收钱
        }
        else
        {
            //取货，投入金额不足，主控->安卓
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), LBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), dat_quehuo, 2);     //主控->电机，取货
            sprintf(strtmp, "USARTCMD_ZHUKONG_ANDROID_CoinNoEnough: %04X,%d-%d %d\r\n", USARTCMD_ZHUKONG_ANDROID_CoinNoEnough, dat_quehuo[0], dat_quehuo[1], price_num);
            USART_DEBUG((char*)strtmp);
        }
    }
}
#endif


//功能：发送复位0X30指令并校验返回值
void DET_RESET_ZHI(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_RESET_ZHI();      //发送复位指令30H
        delay_ms(10);

        if(rev_data_len > 0)       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}

//功能：发送纸币器参数指令0x31并校验返回值
void DET_STATUS_ZHI(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_STATUS_ZHI();      //发送硬币器状态指令31H
        delay_ms(50);

        if(rev_data_len >= (LEN_STATUS_ZHI + 1))       //判断接受的数据长度
//         if(rev_data_len > 0)
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}

//功能：发送纸币器安全等级指令0x32并校验返回值
//说明：暂时基本没用到
void DET_SECURITY_ZHI(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_SECURITY_ZHI();            //设置纸币器安全等级指令32H
        delay_ms(10);

        if(rev_data_len > 0)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送纸币器动作指令0x33并校验返回值
//返回值：POLL指令不同的回复信息，返回不同的值。实际中，重要的就是收钱
u8 DET_POLL_ZHI(void)
{
    u8 REV_33_YING = 0;          //纸币器发送33后，接收到内容标志位，默认为0
//    static u8 flag_huishou = 0;     //回收支出的标志位
    Wptr_mode = 0x33;
    Wptr_YING = 0;

    while(1)
    {
        Send_POLL_ZHI();    //回复机器动作类型0x33
        delay_ms(50);

        if(Wptr_YING > 0)       //判断接受的数据长度
        {
            break;
        }
    }

    USART_SendByte(USART2, 0x00);       //ACK

    if(0 == Get_CHK(BUF_POLL, Wptr_YING))
    {
        return 0;   //CHK有误
    }

    if((Wptr_YING == 1) && BUF_POLL[Wptr_YING - 1] == 0x00)
    {
        REV_33_YING = 1;        //POLL后，接收到00(ACK)标志位
//        if(flag_huishou == 100)
//        {
//            flag_huishou--;     //保证前一次手动支出，随后支出结束
//            REV_33_YING = flag_huishou;     //自减1，返回99，下次不会执行
//            Wptr_YING = 0;
//            Wptr_mode = 0;
//            return REV_33_YING;
//        }
        //POLL后接收到00指令，代表硬币器复位
        USART_DEBUG("ZhiBiQi ACK \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x06 && BUF_POLL[1] == 0x06)
    {
        REV_33_YING = 2;        //POLL后，接收到06 06标志位
        //POLL后接收到06 06指令，代表纸币器复位
        USART_DEBUG("ZhiBiQi RESET \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x03 && BUF_POLL[1] == 0x03)
    {
        REV_33_YING = 3;        //POLL后，接收到03 03标志位
        //POLL后接收到03 03指令，代表纸币器忙，一般是正在收钱
        USART_DEBUG("ZhiBiQi 0303 \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x09 && BUF_POLL[1] == 0x09)
    {
        REV_33_YING = 4;        //POLL后，接收到09 09标志位,纸币器不可用
        USART_DEBUG("ZhiBiQi NO USE \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0x90 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x99)
    {
        REV_33_YING = 5;        //POLL后，接收到90 09 99标志位,收入1元纸币到暂保留位置
        USART_DEBUG("ZhiBiQi ZAN1 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0x91 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9A)
    {
        REV_33_YING = 6;        //POLL后，接收到91 09 9A标志位,收入5元纸币到暂保留位置
        USART_DEBUG("ZhiBiQi ZAN5 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0x92 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9B)
    {
        REV_33_YING = 7;        //POLL后，接到92 09 9B标志位,收入10元纸币到暂保留位置
//        flag_huishou = 100;      //设置回收支出标志
        USART_DEBUG("ZhiBiQi ZAN10 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0x93 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9C)
    {
        REV_33_YING = 8;        //POLL后，接到93 09 9C标志位,收入20元纸币到暂保留位置
//        flag_huishou = 100;      //设置回收支出标志,暂时为100
        USART_DEBUG("ZhiBiQi ZAN20 \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x80 && BUF_POLL[1] == 0x80)
    {
        REV_33_YING = 9;        //POLL后，接收到80 80标志位,收入1元纸币到钱盒
        USART_DEBUG("ZhiBiQi RU1 \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x81 && BUF_POLL[1] == 0x81)
    {
        REV_33_YING = 10;        //POLL后，接收到81 81标志位,收入5元纸币到钱盒
        USART_DEBUG("ZhiBiQi RU5 \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x82 && BUF_POLL[1] == 0x82)
    {
        REV_33_YING = 11;        //POLL后，接到82 82标志位,收入10元纸币到钱盒
//        flag_huishou = 100;      //设置回收支出标志
        USART_DEBUG("ZhiBiQi RU10 \r\n");
    }
    else if((Wptr_YING == 2) && BUF_POLL[0] == 0x83 && BUF_POLL[1] == 0x83)
    {
        REV_33_YING = 12;        //POLL后，接到83 83标志位,收入20元纸币到钱盒
//        flag_huishou = 100;      //设置回收支出标志,暂时为100
        USART_DEBUG("ZhiBiQi RU20 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA0 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xA9)
    {
        REV_33_YING = 9;        //POLL后，接收到A0 09 89标志位,退回1元纸币
        USART_DEBUG("ZhiBiQi FAN1 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA1 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAA)
    {
        REV_33_YING = 10;        //POLL后，接收到A1 09 8A标志位,退回5元纸币
        USART_DEBUG("ZhiBiQi FAN5 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA2 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAB)
    {
        REV_33_YING = 11;        //POLL后，接到A2 09 8B标志位,退回10元纸币
//        flag_huishou = 100;      //设置回收支出标志
        USART_DEBUG("ZhiBiQi FAN10 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA3 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAC)
    {
        REV_33_YING = 12;        //POLL后，接到A3 09 8C标志位,退回20元纸币
//        flag_huishou = 100;      //设置回收支出标志,暂时为100
        USART_DEBUG("ZhiBiQi FAN20 \r\n");
    }
    else if((Wptr_YING == 3) && BUF_POLL[0] == 0x06 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x0F)
    {
        REV_33_YING = 13;        //开机上电第一次初始化后，POLL后，首先回复06 09 0F
//        flag_huishou = 100;      //设置回收支出标志,暂时为100
        USART_DEBUG("ZhiBiQi INIT \r\n");
    }
    else
    {
        REV_33_YING = 0xFF;        //POLL后，接收到未定义的数据
    }

//    switch(Wptr_YING)
//    {
//        case 1:
//            if(BUF_POLL[0] == 0x00)
//            {
//                REV_33_YING = 1;        //POLL后，接收到00(ACK)标志位
//            }
//            break;
//        case 2:
//            if(BUF_POLL[0] == 0x03 && BUF_POLL[1] == 0x03)
//            {
//                REV_33_YING = 3;        //POLL后，接收到03 03标志位
//                //POLL后接收到03 03指令，代表纸币器忙，一般是正在收钱
//                USART_DEBUG("ZhiBiQi 0303 \r\n");
//            }
//            else if(BUF_POLL[0] == 0x09 && BUF_POLL[1] == 0x09)
//            {
//                REV_33_YING = 4;        //POLL后，接收到09 09标志位,纸币器不可用
//                USART_DEBUG("ZhiBiQi NO USE \r\n");
//            }
//            else if((Wptr_YING == 2) && BUF_POLL[0] == 0x06 && BUF_POLL[1] == 0x06)
//            {
//                REV_33_YING = 2;        //POLL后，接收到06 06标志位
//                //POLL后接收到06 06指令，代表纸币器复位
//                USART_DEBUG("ZhiBiQi RESET \r\n");
//            }
//            break;
//        case 3:
//            if((Wptr_YING == 3) && BUF_POLL[0] == 0x90 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x99)
//            {
//                REV_33_YING = 5;        //POLL后，接收到90 09 99标志位,收入1元纸币到暂保留位置
//                USART_DEBUG("ZhiBiQi ZAN1 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x91 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9A)
//            {
//                REV_33_YING = 6;        //POLL后，接收到91 09 9A标志位,收入5元纸币到暂保留位置
//                USART_DEBUG("ZhiBiQi ZAN5 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x92 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9B)
//            {
//                REV_33_YING = 7;        //POLL后，接到92 09 9B标志位,收入10元纸币到暂保留位置
//                USART_DEBUG("ZhiBiQi ZAN10 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x93 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x9C)
//            {
//                REV_33_YING = 8;        //POLL后，接到93 09 9C标志位,收入20元纸币到暂保留位置
//                USART_DEBUG("ZhiBiQi ZAN20 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x80 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x89)
//            {
//                REV_33_YING = 9;        //POLL后，接收到80 09 89标志位,收入1元纸币到钱盒
//                USART_DEBUG("ZhiBiQi RU1 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x81 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x8A)
//            {
//                REV_33_YING = 10;        //POLL后，接收到81 09 8A标志位,收入5元纸币到钱盒
//                USART_DEBUG("ZhiBiQi RU5 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x82 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x8B)
//            {
//                REV_33_YING = 11;        //POLL后，接到82 09 8B标志位,收入10元纸币到钱盒
//                USART_DEBUG("ZhiBiQi RU10 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0x83 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0x8C)
//            {
//                REV_33_YING = 12;        //POLL后，接到83 09 8C标志位,收入20元纸币到钱盒
//                USART_DEBUG("ZhiBiQi RU20 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA0 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xA9)
//            {
//                REV_33_YING = 9;        //POLL后，接收到A0 09 89标志位,退回1元纸币
//                USART_DEBUG("ZhiBiQi FAN1 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA1 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAA)
//            {
//                REV_33_YING = 10;        //POLL后，接收到A1 09 8A标志位,退回5元纸币
//                USART_DEBUG("ZhiBiQi FAN5 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA2 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAB)
//            {
//                REV_33_YING = 11;        //POLL后，接到A2 09 8B标志位,退回10元纸币
//                USART_DEBUG("ZhiBiQi FAN10 \r\n");
//            }
//            else if((Wptr_YING == 3) && BUF_POLL[0] == 0xA3 && BUF_POLL[1] == 0x09 && BUF_POLL[2] == 0xAC)
//            {
//                REV_33_YING = 12;        //POLL后，接到A3 09 8C标志位,退回20元纸币
//                USART_DEBUG("ZhiBiQi FAN20 \r\n");
//            }
//            break;
//        default:
//            break;
//    }
//    sprintf((char*)strtmp, "REV : %d\r\n", REV_33_YING);
//    //串口2改为串口1作为PC调试,串口2作为投币器和纸币器通信
//    USART_DEBUG((char*)strtmp);
//    USART_SendByte(USART1, REV_33_YING);    //打印返回值
    Wptr_YING = 0;
    Wptr_mode = 0;
    return REV_33_YING;
}
//功能：发送纸币器可接收纸币类型指令34H，数据区4字节
//入口参数：mode为1，可接收钱；mode为2，不可接收钱
void DET_BILL_TYPE_ZHI(u8 mode)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        switch(mode)
        {
            case 1:
                Send_BILL_TYPE_ZHI(1);       //设置纸币器的可接收货币和暂保留的可接收货币
                break;

            case 2:
                Send_BILL_TYPE_ZHI(2);       //设置纸币器的不接收货币和暂保留的可接收货币
                break;

            default:
                return;
        }

        delay_ms(10);

        if(rev_data_len > 0)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：处理处于暂保留位置的纸币
//入口参数：mode为1接收钱，mode为2退回钱
void DET_ESCROW_ZHI(u8 mode)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        switch(mode)
        {
            case 1:
                Send_ESCROW_ZHI(1);       //从暂保留位置收钱
                break;

            case 2:
                Send_ESCROW_ZHI(2);       //从暂保留位置退回钱
                break;

            default:
                return;
        }

        delay_ms(10);

        if(rev_data_len > 0)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}

//功能：发送读取现金盒钱数指令36H
//入口参数：num_coin为钱币张数
//返回值：CHK有误，返回0;钱盒满，返回2;钱盒未满，返回1
u8 DET_STACKER_ZHI(u16* num_coin)
{
    Wptr_mode = 0x36;
    Wptr_YING = 0;

    while(1)
    {
        Send_STACKER_ZHI();         //现金盒钱数
        delay_ms(10);

        if(Wptr_YING == (LEN_STACKER_ZHI + 1))       //判断接受的数据长度
        {
//            Wptr_YING = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK

    if(0 == Get_CHK(BUF_0A, Wptr_YING))
    {
        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
        Wptr_mode = 0;
        return 0;   //CHK有误
    }

    *num_coin = MAKEWORD(BUF_0A[1], (BUF_0A[0] & 0x7F));    //字节1最高位清零，获取钱数

    if(BUF_0A[0] & 0x80)        //钱盒满
    {
        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
        Wptr_mode = 0;
        return 2;   //钱盒满
    }

    Wptr_YING = 0;   //清零，记录下一条指令回复的长度
    Wptr_mode = 0;
    return 1;       //钱盒未满
}
//功能：发送扩展指令0X3700
void DET_IDENTIFICATION_ZHI(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_IDENTIFICATION_ZHI();     //发送扩展指令0x3700
        delay_ms(50);

        if(rev_data_len == (LEN_IDENTIFICATION_ZHI + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
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
//入口参数：mode为模式位，值为1，可收钱；值为2，不可收钱
void Send_BILL_TYPE_ZHI(u8 mode)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[2] = 0x00;
    coin_dat[3] = 0x0F;     //暂保留区

    switch(mode)
    {
        case 1:
            coin_dat[1] = 0x0F;     //可收钱，34 00 0F 00 0FH
            num = (u8)(0x34 + 0x00 + 0x0f + 0x0f);
            break;

        case 2:
            coin_dat[1] = 0x00;     //不可收钱，34 00 00 00 0FH
            num = (u8)(0x34 + 0x00 + 0x00 + 0x0f);
            break;

        default:
            return;
    }

    cmd = (0x01 << 8) | 0x34;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 4);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送暂保留状态指令35H，数据区1字节
//入口参数：mode为1，表示从暂保留收钱；mode为2表示从暂保留返回钱
void Send_ESCROW_ZHI(u8 mode)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    u8 dat = 0;     //数据区，单字节

    switch(mode)
    {
        case 1:
            dat = 0x01;
            break;

        case 2:
            dat = 0x00;
            break;

        default:
            return;
//            break;
    }

    num = (u8)(0x35 + dat);
    cmd = (0x01 << 8) | 0x35;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, dat);   //发送数据，对应支出硬币的数值
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送读取现金盒钱数指令36H
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





