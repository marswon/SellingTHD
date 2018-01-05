#include "bsp_coin.h"

extern char strtmp[100];     //打印调试信息缓存信息
static u8 num_ZHIBI = 0;     //纪录投入纸币的金额
static u16 balance = 0;  //投入货币取货后投入的余额


//功能：纸币器和硬币器联合工作
//说明：投入纸币后，按下暂保留杆，会有同样的硬币数退回
void COIN_use(void)
{
    u16 num_coin = 0;   //投入钱币的总金额
    static u16 num_queqian = 0;     //纪录之前一次检测缺钱的数额
    u8 num_05_TUBE = 0;
    u8 num_10_TUBE = 0;
    u8 rev = 0;
    u8 num_PAY = 0;      //支出硬币金额
//    char cnt = 0;
    rev = DET_POLL_YING();    //硬币器发送0B,回复机器动作类型

    if(rev == 2)        //暂保留杆动作
    {
        rev = 0;
        DET_COIN_DISENABLE_YING();  //硬币器禁止收钱
        DET_BILL_TYPE_ZHI(2);       //纸币器禁止收钱

        do
        {
            rev = DET_TUBE_STATUS_YING(&num_05_TUBE, &num_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        switch(rev)     //硬币数0A指令返回值
        {
            case 1:     //5角钱.1元钱管都未满
//                USART_DEBUG("YINGBIQI : 5,10 NO FULL");
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
        //连同余额和新投入的硬币，包括纸币对应的硬币数一起支出
        num_PAY = num_05_TUBE + num_10_TUBE * 2 + balance / 5 + num_ZHIBI * 2;
        num_ZHIBI = 0;      //纸币器钱数清零

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

        DET_COIN_ENABLE_YING();    //硬币器发送"可收钱"指令
        DET_BILL_TYPE_ZHI(1);       //纸币器发送可收钱
        balance = 0;        //余额清零
        flag_take_huowu = FALSE;    //先选货，投入硬币后，动作保留杆表示该次出货取消
    }
    else if(rev == 99)        //手动支出结束，需要更新硬币值
    {
        rev = 0;
//        sprintf((char*)strtmp, "REV : %d\r\n", rev);
//        //串口2改为串口1作为PC调试,串口2作为投币器和纸币器通信
//        USART_DEBUG((char*)strtmp);
        DET_COIN_DISENABLE_YING();      //禁止收钱

        //发送0A，纪录下硬币枚数
        do
        {
            rev = DET_TUBE_STATUS_YING(&pre_05_TUBE, &pre_10_TUBE);     //发送0A，回复硬币枚数
        }
        while(rev == 0);        //CHK校验和有误

        DET_COIN_ENABLE_YING();    //发送"可收钱"指令
    }

    rev = DET_POLL_ZHI();       //纸币器POLL指令，33H

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
            DET_BILL_TYPE_ZHI(1);       //发送可收钱，直到真正收入到钱盒
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

//    sprintf((char*)strtmp, "num_ZHIBI : %d\r\n", num_ZHIBI);    //打印投入金额
//    USART_DEBUG((char*)strtmp);

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
        num_coin = num_05_TUBE * 5 + num_10_TUBE * 10 + num_ZHIBI * 10 + balance;      //当次投入的钱币的总额 + 之前的余额

        if(num_coin >= price_num)      //高于指定货物的价格
        {
            char str[2] = {0};      //用于返回取货后的余额
//            sprintf(strtmp, "num_coin: %d price_num: %d\r\n", num_coin, price_num);
//            USART_DEBUG((char*)strtmp);
            DET_COIN_DISENABLE_YING();      //硬币器禁止收钱
            DET_BILL_TYPE_ZHI(2);       //纸币器禁止收钱
            Send_CMD_DAT(UART4, HBYTE(ZHUKON_DIANJI_HANGLIE), LBYTE(ZHUKON_DIANJI_HANGLIE), dat_quehuo, 2);     //主控->电机，取货
            sprintf((char*)strtmp, "ZHUKON_DIANJI_HANGLIE: %04X,%d-%d %d\r\n", ZHUKON_DIANJI_HANGLIE, dat_quehuo[0], dat_quehuo[1], price_num);
            USART_DEBUG((char*)strtmp);
            balance = num_coin - price_num;     //更新余额
            num_ZHIBI = 0;       //清零
            num_queqian = 0;         //清零之前缺钱的数额
            flag_take_huowu = FALSE;    //判定该次取货完成
//            cnt = (char)balance;
            str[0] = LBYTE(balance);    //余额低字节
            str[1] = HBYTE(balance);    //余额高字节
//            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_Reply_Balance), LBYTE(USARTCMD_ZHUKONG_ANDROID_Reply_Balance), &cnt, 1);     //主控->安卓，钱数不足
            Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_Reply_Balance), LBYTE(USARTCMD_ZHUKONG_ANDROID_Reply_Balance), str, 2);     //主控->安卓，钱数不足
            sprintf((char*)strtmp, "Balance: %d\r\n", balance);
            USART_DEBUG((char*)strtmp);
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

            DET_COIN_ENABLE_YING();    //硬币器发送"可收钱"指令
            DET_BILL_TYPE_ZHI(1);       //纸币器发送可收钱
        }
        else
        {
//            printf("num_coin :%d num_queqian: %d\r\n", num_coin, num_queqian);
            //取货，投入金额不足，主控->安卓
            num_coin = price_num - num_coin;        //计算当前缺钱的数额
            dat_quehuo[2] = num_coin;           //发送给安卓板
            sprintf(strtmp, "num_coin: %d num_queqian: %d\r\n", num_coin, num_queqian);
            USART_DEBUG((char*)strtmp);

            if(num_queqian != num_coin)
            {
                Send_CMD_DAT(USART3, HBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), LBYTE(USARTCMD_ZHUKONG_ANDROID_CoinNoEnough), dat_quehuo, 3);     //主控->安卓，钱数不足
                sprintf(strtmp, "USARTCMD_ZHUKONG_ANDROID_CoinNoEnough: %04X,%d-%d %d\r\n", USARTCMD_ZHUKONG_ANDROID_CoinNoEnough, dat_quehuo[0], dat_quehuo[1], dat_quehuo[2]);
                USART_DEBUG((char*)strtmp);
            }

            num_queqian = num_coin;         //更新之前缺钱的数额
        }
    }
}

//功能：查询目前可用于取货的金额
//返回值：投入的金额和剩余的钱数总和
//说明：上次出货后，投入的金额和剩余的钱数总和，可以用于目前取货的钱数
u16 GetBalance(void)
{
    u16 num_coin = 0;   //投入钱币的总余额
    u8 num_05_TUBE = 0;
    u8 num_10_TUBE = 0;
    u8 rev = 0;
    DET_COIN_DISENABLE_YING();  //硬币器禁止收钱
    DET_BILL_TYPE_ZHI(2);       //纸币器禁止收钱

    do
    {
        rev = DET_TUBE_STATUS_YING(&num_05_TUBE, &num_10_TUBE);     //发送0A，回复硬币枚数
    }
    while(rev == 0);        //CHK校验和有误

    switch(rev)     //硬币数0A指令返回值
    {
        case 1:     //5角钱.1元钱管都未满
//            USART_DEBUG("YINGBIQI : 5,10 NO FULL");
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
    num_coin = num_05_TUBE * 5 + num_10_TUBE * 10 + balance + num_ZHIBI * 10;       //纪录目前可用于下次取货的金额
    DET_COIN_ENABLE_YING();    //硬币器发送"可收钱"指令
    DET_BILL_TYPE_ZHI(1);       //纸币器发送可收钱
    return num_coin;
}







