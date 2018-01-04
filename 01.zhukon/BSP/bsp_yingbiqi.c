#include "bsp_yingbiqi.h"

//硬币器使用的标志位，默认都是不需要使用的
#define flag_YingBiQi_USE   0

//串口2收到的纸币器和硬币器回复信息，通过串口1打印出去
u8 USART2_dat[50] = {0};
u8 rev_data_len = 0;        //串口2回复数据长度，用于没有收到数据继续发送

/********************* 硬币器 ********************/
//u16 tmp_TUBE_YING = 0;      //硬币器钱管满状态缓存
u8 pre_05_TUBE = 0;   //硬币器5角钱数量，上次取货后5角数量
u8 pre_10_TUBE = 0;   //硬币器1元钱数量，上次取货后1元数量
//u8 CNT_05_YING = 0;     //投入5角的总数
//u8 CNT_10_YING = 0;    //投入1元的总数
//上电初始化之后，5角和1元的枚数，用于计算投入的总枚数
static u8 Init_05_YING = 0;
static u8 Init_10_YING = 0;

char strtmp[100] = {0};     //打印调试信息缓存信息

//功能：硬币器初始化
//入口参数：
//说明：硬币器初始化函数，按照文档初始化流程
void YingBiQi_Init(void)
{
    DET_RESET_YING();

    while(!(5 == DET_POLL_YING()));       //初始化必须接收到ACK

    DET_STATUS_YING();
    DET_IDENTIFICATION_YING();
    DET_FEATURE_ENABLE_YING();

    while(0 == DET_TUBE_STATUS_YING(&Init_05_YING, &Init_10_YING));       //CHK校验和有误，继续发送

    while(!(1 == DET_SEND_DIAGNOSTIC_YING()));      //初始化必须接收到03 00 03

    DET_COIN_ENABLE_YING();
    //延时3s保证初始化的钱数是正确的
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);

    while(!(5 == DET_POLL_YING()));       //初始化必须接收到ACK

    while(0 == DET_TUBE_STATUS_YING(&Init_05_YING, &Init_10_YING));     //发送0A，回复硬币初始枚数，延时3s不可少

    //用于出货
    pre_05_TUBE = Init_05_YING;      //五角钱数量
    pre_10_TUBE = Init_10_YING;      //一元钱数量
}

#if (flag_YingBiQi_USE == 1)
//功能：硬币器单独使用
//入口参数：
//说明：硬币器要想检测硬币，必须循环发送指定指令。否则，机器不会检测硬币
void YingBiQi_USE(void)
{
    u16 num_coin = 0;   //投入钱币的总金额
    static u8 balance = 0;  //投入货币取货后投入的余额
    u8 num_05_TUBE = 0;
    u8 num_10_TUBE = 0;
    u8 rev = 0;
    u8 num_PAY = 0;      //支出硬币金额
    rev = DET_POLL_YING();    //发送0B,回复机器动作类型

    if(rev == 2)        //暂保留杆动作
    {
        rev = 0;
        DET_COIN_DISENABLE_YING();      //禁止收钱

        do
        {
            rev = DET_TUBE_STATUS_YING(&num_05_TUBE, &num_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        switch(rev)     //硬币数0A指令返回值
        {
            case 1:     //5角钱.1元钱管都未满
                USART_DEBUG("YINGBIQI : 5,10 NO FULL");
                break;

            case 2:     //5角钱.1元钱管满
                break;

            case 3:     //5角钱管满
                break;

            case 4:     //1元钱管满
                break;

            default:
                break;
        }

        //自上次出货后投入的硬币数
        num_05_TUBE -= pre_05_TUBE;
        num_10_TUBE -= pre_10_TUBE;
//        USART_SendByte(USART1, rev);
        num_PAY = num_05_TUBE + num_10_TUBE * 2 + balance / 5;      //连同余额和新投入的硬币一起支出

        if(num_PAY > 0)     //只有支出的金额大于0，才会支出
        {
            DET_PAYOUT_YING(num_PAY);            //支出指定金额硬币
            DET_PAYOUT_VALUE_POLL_YING();   //支出完成，回复ACK结束
        }

        //更新当前5角，1元硬币数，因为可能支出余额
        do
        {
            rev = DET_TUBE_STATUS_YING(&pre_05_TUBE, &pre_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        DET_COIN_ENABLE_YING();    //发送"可收钱"指令
        balance = 0;        //余额清零
        flag_take_huowu = FALSE;    //先选货，投入硬币后，动作保留杆表示该次出货取消
    }
    else if(rev == 99)        //手动支出结束，需要更新硬币值
    {
        rev = 0;
        sprintf((char*)strtmp, "REV : %d\r\n", rev);
        //串口2改为串口1作为PC调试,串口2作为投币器和纸币器通信
        USART_DEBUG((char*)strtmp);
        DET_COIN_DISENABLE_YING();      //禁止收钱

        //发送0A，纪录下硬币枚数
        do
        {
            rev = DET_TUBE_STATUS_YING(&pre_05_TUBE, &pre_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        DET_COIN_ENABLE_YING();    //发送"可收钱"指令
    }

    if(flag_take_huowu == TRUE)        //安卓->主控，发送"取货"命令
    {
        do
        {
            rev = DET_TUBE_STATUS_YING(&num_05_TUBE, &num_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        sprintf((char*)strtmp, "num_05_TUBE:%d num_10_TUBE:%d\r\n", num_05_TUBE, num_10_TUBE);
        //串口2改为串口1作为PC调试,串口2作为投币器和纸币器通信
        USART_DEBUG((char*)strtmp);
        //自上次出货后投入的硬币数
        num_05_TUBE = num_05_TUBE - pre_05_TUBE;
        num_10_TUBE = num_10_TUBE - pre_10_TUBE;
        num_coin = num_05_TUBE * 5 + num_10_TUBE * 10 + balance;      //当次投入的钱币的总额 + 之前的余额

        if(num_coin >= price_num)      //高于指定货物的价格
        {
            DET_COIN_DISENABLE_YING();      //禁止收钱
            Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), dat_quehuo, 2);     //主控->电机，取货
            sprintf((char*)strtmp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d %d\r\n", ZHUKON_DIANJI_HANGLIE, dat_quehuo[0], dat_quehuo[1], price_num);
            USART_DEBUG((char*)strtmp);
            balance = num_coin - price_num;     //更新余额
            flag_take_huowu = FALSE;    //判定该次取货完成
            delay_ms(1000);         //延时1s

            //目前，投入硬币，选货后立即会找零
//            num_PAY = (num_coin - price_num) / 5;       //换算为硬币计算系数为单位
//            DET_PAYOUT_YING(num_PAY);            //支出指定金额硬币
//            DET_PAYOUT_VALUE_POLL_YING();   //支出完成，回复ACK结束
//            DET_COIN_ENABLE_YING();    //发送"可收钱"指令
            //更新当前5角，1元硬币数
            do
            {
                rev = DET_TUBE_STATUS_YING(&pre_05_TUBE, &pre_10_TUBE);     //发送0A，回复硬币枚数
            }
            while(rev == 0);        //CHK校验和有误

            DET_COIN_ENABLE_YING();    //发送"可收钱"指令
        }
        else
        {
            //取货，投入金额不足，主控->安卓
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), LBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), dat_quehuo, 2);     //主控->电机，取货
            sprintf(strtmp, "USARTCMD_ZHUKONG_ANDROID_CoinNoEnough: %04X,%d-%d %d\r\n", USARTCMD_ZHUKONG_ANDROID_CoinNoEnough, dat_quehuo[0], dat_quehuo[1], price_num);
            USART_DEBUG((char*)strtmp);
        }
    }

//    if(flag_chu_success == TRUE)       //出货成功，暂时用不到
//    {
//        DET_PAYOUT_YING(num_PAY);            //支出指定金额硬币
//        DET_PAYOUT_VALUE_POLL_YING();   //支出完成，回复ACK结束
//        DET_COIN_ENABLE_YING();    //发送"可收钱"指令
//        flag_chu_success = FALSE;
//    }
}
#endif

//功能：发送复位0X08指令并校验返回值
void DET_RESET_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_RESET_YING();      //发送复位指令08H
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

//功能：发送投币器参数指令0x09并校验返回值
void DET_STATUS_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_STATUS_YING();      //发送硬币器状态指令09H
        delay_ms(50);

        if(rev_data_len == (LEN_STATUS_YING + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送投币器参数指令0x0A并校验返回值
//入口参数：num_05为纪录5角钱数，num_10为纪录1元钱数
//返回值：CHK有误返回0，钱管未满返回1，5角钱钱管满返回3，1元钱钱管满返回4，
//5角钱.1元钱钱管都满返回2
//问题：实际中入口参数为NULL,NULL。发现程序会死机。真是会跑飞
u8 DET_TUBE_STATUS_YING(u8* num_05, u8* num_10)
{
    Wptr_mode = 0x0A;
    Wptr_YING = 0;

    while(1)
    {
        Send_TUBE_STATUS_YING();    //发送钱管状态指令0x0A，回复剩余各个钱管状态
        delay_ms(50);

        if(Wptr_YING == (LEN_TUBE_STATUS_YING + 1))       //判断接受的数据长度
        {
//            Wptr_YING = 0;
            break;
        }

        Wptr_YING = 0;
    }

    USART_SendByte(USART2, 0x00);       //ACK

    if(0 == Get_CHK(BUF_0A, Wptr_YING))
    {
        Wptr_YING = 0;
        Wptr_mode = 0;
        return 0;   //CHK有误
    }

    *num_05 = BUF_0A[2];        //纪录5角钱数
    *num_10 = BUF_0A[3];        //纪录1元钱数

    if((BUF_0A[1] & 0x03) == 0x03)
    {
        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
        Wptr_mode = 0;
        return 2;   //5角钱.1元钱管满
    }
    else if(BUF_0A[1] & 0x01)        //5角钱管满
    {
        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
        Wptr_mode = 0;
        return 3;   //5角钱管满
    }
    else if(BUF_0A[1] & 0x02)   //1元钱管满
    {
        Wptr_YING = 0;   //清零，记录下一条指令回复的长度
        Wptr_mode = 0;
        return 4;       //1元钱管满
    }

    Wptr_YING = 0;
    Wptr_mode = 0;
    return 1;
}
//功能：发送投币器参数指令0x0B并校验返回值
//返回值：硬币器接收到POLL指令后，回复不同的动作。我们根据回复的数据，返回不同的值
u8 DET_POLL_YING(void)
{
    u8 REV_0B_YING = 0;          //硬币器发送0B后，接收到内容标志位，默认为0
    static u8 flag_huishou = 0;     //回收支出的标志位
    Wptr_mode = 0x0B;
    Wptr_YING = 0;

    while(1)
    {
        Send_POLL_YING();    //回复机器动作类型0x0B
        delay_ms(50);

        if(Wptr_YING > 0)       //判断接受的数据长度
        {
            break;
        }
    }

    USART_SendByte(USART2, 0x00);       //ACK

    if(0 == Get_CHK(BUF_POLL, Wptr_YING))
    {
        Wptr_YING = 0;
        Wptr_mode = 0;
        return 0;   //CHK有误
    }

    if((Wptr_YING >= 2) && BUF_POLL[Wptr_YING - 1] == 0x0B && BUF_POLL[Wptr_YING - 2] == 0x0B)
    {
        REV_0B_YING = 1;        //POLL后，接收到0B 0B标志位
        //POLL后接收到0B 0B指令，代表硬币器复位
        USART_DEBUG("YingBiQi RESET \r\n");
    }
    else if((Wptr_YING >= 2) && BUF_POLL[Wptr_YING - 1] == 0x01 && BUF_POLL[Wptr_YING - 2] == 0x01)
    {
        REV_0B_YING = 2;        //POLL后，接收到01 01标志位
        //POLL后接收到01 01指令，代表硬币器暂保留杆动作
        USART_DEBUG("YingBiQi 0101 \r\n");
    }
    else if((Wptr_YING >= 3) && BUF_POLL[0] == 0x51)
    {
        REV_0B_YING = 3;        //POLL后，接收到51 xx xx标志位,投入1元硬币到钱管
        USART_DEBUG("YingBiQi RU51 \r\n");
    }
    else if((Wptr_YING >= 3) && BUF_POLL[0] == 0x50)
    {
        REV_0B_YING = 4;        //POLL后，接收到50 xx xx标志位,投入5角硬币到钱管
        USART_DEBUG("YingBiQi RU50 \r\n");
    }
    else if((Wptr_YING >= 4) && BUF_POLL[0] == 0x02 && BUF_POLL[1] == 0x91)
    {
        REV_0B_YING = 6;        //POLL后，接到02 91 xx xx标志位,手动支出1元硬币到零钱盒
        flag_huishou = 100;      //设置回收支出标志
        USART_DEBUG("YingBiQi CHU91 \r\n");
    }
    else if((Wptr_YING >= 4) && BUF_POLL[0] == 0x02 && BUF_POLL[1] == 0x90)
    {
        REV_0B_YING = 7;        //POLL后，接收到02 90 xx xx标志位,手动支出5角硬币到零钱盒
        flag_huishou = 100;      //设置回收支出标志,暂时为100
        USART_DEBUG("YingBiQi CHU90 \r\n");
    }
    else if((Wptr_YING == 1) && BUF_POLL[Wptr_YING - 1] == 0x00)
    {
        REV_0B_YING = 5;        //POLL后，接收到00(ACK)标志位

        if(flag_huishou == 100)
        {
            flag_huishou--;     //保证前一次手动支出，随后支出结束
            REV_0B_YING = flag_huishou;     //自减1，返回99，下次不会执行
            Wptr_YING = 0;
            Wptr_mode = 0;
            return REV_0B_YING;
        }

        //POLL后接收到00指令，代表硬币器复位
        USART_DEBUG("YingBiQi ACK \r\n");
    }
    else
    {
        REV_0B_YING = 0xFF;        //POLL后，接收到未定义的数据
    }

    Wptr_YING = 0;
    Wptr_mode = 0;
    return REV_0B_YING;
}

//功能：发送硬币类型0C0003FFFFh，使能收钱并校验返回值
void DET_COIN_ENABLE_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_COIN_ENABLE_YING();    //回复机器可用硬币类型0C0003FFFFh
        delay_ms(10);

        if(rev_data_len == 1)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送硬币类型0C0000FFFFh，禁止收钱
void DET_COIN_DISENABLE_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_COIN_DISENABLE_YING();    //回复机器可用硬币类型0C0000FFFFh
        delay_ms(10);

        if(rev_data_len == 1)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送支出的硬币类型指令0DH
void DET_DISPENSE_YING(u8 dat)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_DISPENSE_YING(dat);        //支出硬币个数
        delay_ms(100);

        if(rev_data_len == 1)       //判断接受的数据长度,接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送扩展指令0X0F00
void DET_IDENTIFICATION_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_IDENTIFICATION_YING();     //发送扩展指令0x0F00
        delay_ms(100);      //延时必须100ms

        if(rev_data_len == (LEN_IDENTIFICATION_YING + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送扩展指令0X0F01和数据区
void DET_FEATURE_ENABLE_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_FEATURE_ENABLE_YING();     //发送扩展指令0x0F01和数据区
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
//功能：发送扩展指令0X0F02和数据区
//说明：待测试，文档上标注无返回数据
void DET_PAYOUT_YING(u8 dat)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_PAYOUT_YING(dat);     //发送扩展指令0x0F02和数据区
        delay_ms(10);

        if(rev_data_len == 1)       //判断接受的数据长度,测试是否返回接收到ACK
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送扩展指令0X0F03
void DET_PAYOUT_STATUS_YING(void)
{
    rev_data_len = 0;   //清零，记录下一条指令回复的长度

    while(1)
    {
        Send_PAYOUT_STATUS_YING();     //发送扩展指令0x0F03
        delay_ms(100);

        if(rev_data_len == (LEN_PAYOUT_STATUS_YING + 1))       //判断接受的数据长度
        {
            rev_data_len = 0;   //清零，记录下一条指令回复的长度
            break;
        }

        rev_data_len = 0;   //清零，记录下一条指令回复的长度
    }

    USART_SendByte(USART2, 0x00);       //ACK
    return;
}
//功能：发送扩展指令0X0F04
void DET_PAYOUT_VALUE_POLL_YING(void)
{
    Wptr_YING = 0;
    Wptr_mode = 0x0F04;

    while(1)
    {
        Send_PAYOUT_VALUE_POLL_YING();     //发送扩展指令0x0F04
        delay_ms(10);

        if((Wptr_YING == 2) && (BUF_common[0] == 0x00) && (BUF_common[1] == 0x00))
        {
            //未支付硬币
//            USART_SendByte(USART1, 00);
        }
        else if((Wptr_YING == 2) && (BUF_common[0] == 0x01) && (BUF_common[1] == 0x01))
        {
            //支付5角
//            USART_SendByte(USART1, 01);
        }
        else if((Wptr_YING == 2) && (BUF_common[0] == 0x01) && (BUF_common[1] == 0x01))
        {
            //支付1元
//            USART_SendByte(USART1, 02);
        }
        else if((Wptr_YING == 2) && (BUF_common[0] == 0x04) && (BUF_common[1] == 0x04))
        {
            //支付2元
//            USART_SendByte(USART1, 04);
        }
        else if((Wptr_YING == 1) && (BUF_common[0] == 0x00))
        {
            break;          //接收到ACK，支付完成
        }

        Wptr_YING = 0;
    }

    USART_SendByte(USART2, 0x00);       //ACK
    Wptr_mode = 0;
    Wptr_YING = 0;
    return;
}
//功能：发送扩展指令0X0F05
u8 DET_SEND_DIAGNOSTIC_YING(void)
{
    u8 REV_0F05_YING = 0;   //清零
    Wptr_mode = 0x0f05;     //选择对应模式位，用于缓存回复信息
    Wptr_YING = 0;          //纸币器和硬币器回复信息纪录指针

    while(1)
    {
        Send_SEND_DIAGNOSTIC_YING();     //发送扩展指令0x0F05
        delay_ms(50);

        if(Wptr_YING > 0)       //接受到数据
        {
            break;
        }

        Wptr_YING = 0;
    }

    USART_SendByte(USART2, 0x00);       //ACK

    if(0 == Get_CHK(BUF_common, Wptr_YING))
    {
        return 0;   //CHK有误
    }

    if((Wptr_YING >= 3) && (BUF_common[0] == 0x03) && (BUF_common[1] == 0x00) && (BUF_common[2] == 0x03))
    {
        REV_0F05_YING = 1;      //接收到03 00 03，表示硬币器正常
    }
    else
    {
        REV_0F05_YING = 0xFF;      //接收到其他数据
    }

    Wptr_mode = 0;
    Wptr_YING = 0;
    return REV_0F05_YING;
}


//功能：发送复位0X08指令
void Send_RESET_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x08;  //对应模式位置1，表示地址字节
    num = 0X08;
    rev_data_len = 0;   // 清零，记录下一条指令回复的长度
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}

//功能：发送投币器参数指令0x09
void Send_STATUS_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x09;  //对应模式位置1，表示地址字节
    num = 0X09;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}

//功能：发送投币器参数指令0x0A
void Send_TUBE_STATUS_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x0A;  //对应模式位置1，表示地址字节
    num = 0X0A;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送投币器参数指令0x0B
void Send_POLL_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    cmd = (0x01 << 8) | 0x0B;  //对应模式位置1，表示地址字节
    num = 0X0B;
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送硬币类型0C0003FFFFh，使能收钱
void Send_COIN_ENABLE_YING(void)
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

//功能：发送硬币类型0C0000FFFFh，禁止收钱
void Send_COIN_DISENABLE_YING(void)
{
    u16 cmd = 0;
    u8 coin_dat[4] = {0};
    u8 num = 0;     //数据区总和
    coin_dat[0] = 0x00;    //发送的第一个字节，实际顺序待测
    coin_dat[1] = 0x00;
    coin_dat[2] = 0xff;
    coin_dat[3] = 0xff;
    num = (u8)(0x0c + 0xff + 0xff);
    cmd = (0x01 << 8) | 0x0c;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_SendBytes(USART2, coin_dat, 4);
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}
//功能：发送支出的硬币类型指令0DH
void Send_DISPENSE_YING(u8 dat)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = (u8)(0x0d + dat);
    cmd = (0x01 << 8) | 0x0D;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, dat);   //发送数据，对应支出硬币的类型和数值
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
//功能：发送扩展指令0X0F02和数据区
void Send_PAYOUT_YING(u8 dat)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = (u8)(0x0F + 0x02 + dat);
    cmd = (0x01 << 8) | 0x0F;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X02);           //发送副指令
    USART_Send2Byte(USART2, dat);   //发送数据，对应支出硬币的数值
    USART_Send2Byte(USART2, num);           //发送CHK检验和
}

//功能：发送扩展指令0X0F03
void Send_PAYOUT_STATUS_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x0f + 0x03;
    cmd = (0x01 << 8) | 0x0f;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X03);           //发送副指令
    USART_SendByte(USART2, num);           //发送CHK检验和
}
//功能：发送扩展指令0X0F04
void Send_PAYOUT_VALUE_POLL_YING(void)
{
    u16 cmd = 0;
    u8 num = 0;     //数据区总和
    num = 0x0f + 0x04;
    cmd = (0x01 << 8) | 0x0f;  //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, cmd);   //发送对应地址字节
    USART_Send2Byte(USART2, 0X04);           //发送副指令
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

//功能：计算结束数据的校验和CHK
//入口参数：str为接收的字符串，str_len为接收的数据长度
//返回值：
u8 Get_CHK(u8* str, u8 str_len)
{
    u8 i;
    u8 num = 0;

    for(i = 0; i < (str_len - 1); i++)
    {
        num += str[i];
    }

    if(num == str[str_len - 1])
        return 1;       //CHK正确
    else
        return 0;
}

#if (FLAG_RUN == 0)
//功能：发送常规命令函数，适用于纸币器和硬币器
//入口参数：basic_cmd为发送的常规命令字节，data为需要发送数据区常规指令的数据
//返回值：正常返回1，异常返回0
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址。命令后，接着CHK检验和。
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

//功能：发送扩展命令函数，适用于纸币器和硬币器
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
        dat_len = DAT_FEATURE_ENABLE_YING;    //VMC数据长度为4
    }
    else if(PAYOUT_YING == exp_cmd)
    {
        dat_len = DAT_PAYOUT_YING;    //VMC数据长度为1
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
#endif
