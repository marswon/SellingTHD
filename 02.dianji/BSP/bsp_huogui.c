#include "bsp_huogui.h"

//用于HuoDao_Init()初始化函数，一般情况下定义为1，只是保证货道运行在临界区，不保证运行在临界区的进入边界；
//定义为0，货道会运行一周，保证停止在临界区的进入边界
#define flag_init       1
u8 flag_line = 0;               //取货中，货道电机运行，对应层的标志位
bool flag_PUTTHING = FALSE;         //掉货检测标志位，默认为FALSE，有货物掉落标志位为TRUE，用在EXIT中断中
bool Enable_EXTI = FALSE;       //使能掉货检测外部中断

//功能：货道开机初始化
//说明：保证货道电机运行在触发区，主要是便于存货并且保证货道电机运行时间固定。
//注意：行反馈信号，对应行列不通电是不能触发的，所以，尽管每一行共用反馈引脚之间也是无影响的，第1行第1列触发后，随即检测第1行第2列不会有影响。
void HuoDao_Init(void)
{
    u8 i, j;
    u16 flag_times = 0;
#if flag_init

    for(i = 1; i <= LINE_MAX; i++)      //对应行编号
    {
        for(j = 1; j <= ROW_MAX; j++)   //对应列编号
        {
            while(1)
            {
                Motor_HuoDao_Move(i, j);    //共用行反馈信号，之间不会有影响。检测顺序，同一列不同行
                delay_ms(5);                //通电运行这么短时间，无法越出临界区

                if((LINEFB1 == 1 && flag_line == 1) || (LINEFB2 == 1 && flag_line == 2) || (LINEFB3 == 1 && flag_line == 3) || (LINEFB4 == 1 && flag_line == 4) || (LINEFB5 == 1 && flag_line == 5) || (LINEFB6 == 1 && flag_line == 6)
                        || (LINEFB7 == 1 && flag_line == 7) || (LINEFB8 == 1 && flag_line == 8) || (LINEFB9 == 1 && flag_line == 9) || (LINEFB10 == 1 && flag_line == 10))
                {
                    Motor_HuoDao_Stop(i, j);    //对应货道电机停转
                    break;
                }
            }

            sprintf(strtemp, "LINE: %d; ROW: %d flag_times: %d\r\n", i, j, flag_times);
            USART_DEBUG(strtemp);     //打印PC调试
        }
    }

#else

    for(i = 1; i <= LINE_MAX; i++)      //对应行编号
    {
        for(j = 1; j <= ROW_MAX; j++)   //对应列编号
        {
            while(1)
            {
                Motor_HuoDao_Move(i, j);    //共用行反馈信号，之间不会有影响。检测顺序，同一列不同行

                if(flag_times == 0)
                {
                    delay_ms(500);      //延时时间长，保证动作越过临界区
                    USART_DEBUG("TIMES init\r\n");
                }
                else
                {
                    delay_ms(5);
                }

                flag_times++;

                if((LINEFB1 == 1 && flag_line == 1) || (LINEFB2 == 1 && flag_line == 2) || (LINEFB3 == 1 && flag_line == 3) || (LINEFB4 == 1 && flag_line == 4) || (LINEFB5 == 1 && flag_line == 5) || (LINEFB6 == 1 && flag_line == 6)
                        || (LINEFB7 == 1 && flag_line == 7) || (LINEFB8 == 1 && flag_line == 8) || (LINEFB9 == 1 && flag_line == 9) || (LINEFB10 == 1 && flag_line == 10))
                {
                    Motor_HuoDao_Stop(i, j);    //对应货道电机停转
                    break;
                }
            }

            sprintf(strtemp, "LINE: %d; ROW: %d flag_times: %d\r\n", i, j, flag_times);
            USART_DEBUG(strtemp);     //打印PC调试
            flag_times = 0;     //标志位清零
        }
    }

#endif
}

//测试每一行货道运行
//入口参数：i为行编号
//说明：每次测试一行，保证一行运行完整
void HuoDao_line_test(u8 i)
{
    u8 j;
    char strtemp[30] = {0};     //调试
    u16 flag_times = 0;

    for(j = 1; j <= ROW_MAX; j++)   //对应列编号
    {
        while(1)
        {
            //行列参数校验
            Detect_Line_And_Row(i, j);
            Motor_HuoDao_Move(i, j);    //共用行反馈信号，之间不会有影响。检测顺序，同一列不同行

            if(flag_times == 0)
            {
                delay_ms(500);      //延时时间长，保证动作越过临界区
                USART_DEBUG("TIMES init\r\n");
            }
            else
            {
                delay_ms(5);
            }

            flag_times++;

            if((LINEFB1 == 1 && flag_line == 1) || (LINEFB2 == 1 && flag_line == 2) || (LINEFB3 == 1 && flag_line == 3) || (LINEFB4 == 1 && flag_line == 4) || (LINEFB5 == 1 && flag_line == 5) || (LINEFB6 == 1 && flag_line == 6)
                    || (LINEFB7 == 1 && flag_line == 7) || (LINEFB8 == 1 && flag_line == 8) || (LINEFB9 == 1 && flag_line == 9) || (LINEFB10 == 1 && flag_line == 10))
            {
                Motor_HuoDao_Stop(i, j);    //对应货道电机停转
                break;
            }
        }

        sprintf(strtemp, "LINE: %d; ROW: %d flag_times: %d\r\n", i, j, flag_times);
        USART_DEBUG(strtemp);     //打印PC调试
        flag_times = 0;     //标志位清零
    }
}

//函数功能：弹簧道售货机取货
//入口参数：m为行位置，n为列位置
//返回值：正常出货，取货返回1；反之返回0，没有实际作用
//说明：暂时没有取货检测，掉货检测正在测试中
u8 HUOWU_Take(u8 m, u8 n)
{
    u16 flag_times = 0;   //运行时间标志位，第一次运行时间必须保证越过临界区
    u16 j = 0;
    u8 strstr[2] = {0};
    Motor_HuoDao_Move(m, n);    //对应货道电机运行
    flag_PUTTHING = FALSE;      //清零
    strstr[0] = m;      //行号
    strstr[1] = n;      //列号
#if (ENABLE_Diaohuo == 0)
    //开启掉货检测，电机动作之前开启掉货检测
    Enable_duishe();      //目前有异响，关闭掉货检测
    Enable_EXTI = TRUE;     //开启外部检测
    
#endif
    for(;;)
    {
        if(flag_times == 0)
        {
            delay_ms(600);      //延时时间长，保证动作越过临界区
            USART_DEBUG("TIMES take\r\n");
        }
        else
        {
            delay_ms(5);
        }

        flag_times++;
//#if (ENABLE_Diaohuo == 0)

//        if(flag_times == 5)     //结束时一般大于100
//        {
//            //开启掉货检测
//            Enable_duishe();      //目前有异响，关闭掉货检测
//        }
//        else if(flag_times == 10)
//        {
//            Enable_EXTI = TRUE;     //开启外部检测
//        }

//#endif

        if((LINEFB1 == 1 && flag_line == 1) || (LINEFB2 == 1 && flag_line == 2) || (LINEFB3 == 1 && flag_line == 3) || (LINEFB4 == 1 && flag_line == 4) || (LINEFB5 == 1 && flag_line == 5) || (LINEFB6 == 1 && flag_line == 6)
                || (LINEFB7 == 1 && flag_line == 7) || (LINEFB8 == 1 && flag_line == 8) || (LINEFB9 == 1 && flag_line == 9) || (LINEFB10 == 1 && flag_line == 10))
        {
//            sprintf(strtemp, "flag_times: %d\r\n", flag_times);     //一般大于100
//            USART_DEBUG(strtemp);     //打印PC调试
            delay_ms(10);
            Motor_HuoDao_Stop(m, n);    //对应货道电机停转
            break;
        }
    }

#if (ENABLE_Diaohuo == 0)

    for(;;)
    {
        if(flag_PUTTHING == TRUE)       //检测到货物，高电平
        {
            Disable_duishe();       //关闭掉货检测，需要取货检测
#if (HUOWU_Continue == 2)
            //电机->主控，出货成功，指定行列
            Send_CMD(USART2, HBYTE(DIANJI_ZHUKON_NUMb1), LBYTE(DIANJI_ZHUKON_NUMb1));
            //PC调试
            Send_CMD(USART1, HBYTE(DIANJI_ZHUKON_NUMb1), LBYTE(DIANJI_ZHUKON_NUMb1));
#elif (HUOWU_Continue == 1)
            //电机->主控，出货成功，指定行列
            Send_CMD_DAT(USART2, HBYTE(DIANJI_ZHUKON_NUMb1), LBYTE(DIANJI_ZHUKON_NUMb1), (char*)strstr, 2);
            //PC调试
            Send_CMD_DAT(USART1, HBYTE(DIANJI_ZHUKON_NUMb1), LBYTE(DIANJI_ZHUKON_NUMb1), (char*)strstr, 2);
#endif
            flag_PUTTHING = FALSE;      //清零
            Enable_EXTI = FALSE;     //关闭外部检测
//            USART_DEBUG("Diao huo\r\n");     //打印PC调试
            break;
        }

        j++;        //检测总次数纪录，达到设定值退出

        if(j >= 1000)         //达到10次，还没有检查到出货成功，认为出货失败,暂定5s检测
        {
            Disable_duishe();       //关闭掉货检测，需要取货检测
#if (HUOWU_Continue == 2)
            //电机->主控，出货失败，指定行列
            Send_CMD(USART2, HBYTE(DIANJI_ZHUKON_NUMb2), LBYTE(DIANJI_ZHUKON_NUMb2));
            Enable_EXTI = FALSE;     //关闭外部检测
            //PC调试
            Send_CMD(USART1, HBYTE(DIANJI_ZHUKON_NUMb2), LBYTE(DIANJI_ZHUKON_NUMb2));
#elif (HUOWU_Continue == 1)
            //电机->主控，出货失败，指定行列
            Send_CMD_DAT(USART2, HBYTE(DIANJI_ZHUKON_NUMb2), LBYTE(DIANJI_ZHUKON_NUMb2), (char*)strstr, 2);
            Enable_EXTI = FALSE;     //关闭外部检测
            //PC调试
            Send_CMD_DAT(USART1, HBYTE(DIANJI_ZHUKON_NUMb2), LBYTE(DIANJI_ZHUKON_NUMb2), (char*)strstr, 2);
#endif
            return 0;
        }

        delay_ms(5);   //每隔5ms检测一次
    }

#endif
    return 1;
}

//功能：对应的货道行列电机运行一段时间
//说明：行列电机通电运行，用于在出货后继续运行一段时间
void HuoDao_Move_Step(u8 m, u8 n)
{
    Motor_HuoDao_Move(m, n);    //对应货道电机通电
    delay_ms(10);               //延时10ms
    Motor_HuoDao_Stop(m, n);    //对应货道电机停转
}

//功能：货道对应行列电机运行
//入口参数：m为行编号，n为列编号
//说明：货道电机通电，flag_line为全局变量
void Motor_HuoDao_Move(u8 m, u8 n)
{
    //行列参数校验
    Detect_Line_And_Row(m, n);

    //对应货道电机通电
    switch(m & 0x0f)
    {
        case 1:
            flag_line = 1;      //对应行的标记位
            LINE1 = LINE_ENABLE;          //货道电机运行，对应行LINEx=LINE_ENABLE,对应列ROWx=1
            break;

        case 2:
            flag_line = 2;
            LINE2 = LINE_ENABLE;
            break;

        case 3:
            flag_line = 3;
            LINE3 = LINE_ENABLE;
            break;

        case 4:
            flag_line = 4;
            LINE4 = LINE_ENABLE;
            break;

        case 5:
            flag_line = 5;
            LINE5 = LINE_ENABLE;
            break;

        case 6:
            flag_line = 6;
            LINE6 = LINE_ENABLE;
            break;

        case 7:
            flag_line = 7;
            LINE7 = LINE_ENABLE;
            break;

        case 8:
            flag_line = 8;
            LINE8 = LINE_ENABLE;
            break;

        case 9:
            flag_line = 9;
            LINE9 = LINE_ENABLE;
            break;

        case 10:
            flag_line = 10;
            LINE10 = LINE_ENABLE;
            break;

        default:
            //printf("TAKE huowu fail: m error!!\r\n");
            break;
    } /* end of switch(m & 0x0f) */

    //对应层的货道电机运行,对应列引脚
    switch(n & 0x0f)
    {
        case 1:
            ROW1 = ROW_ENABLE;       //货道电机运行，对应行LINEx=0,对应列ROWx=ROW_ENABLE
            break;

        case 2:
            ROW2 = ROW_ENABLE;
            break;

        case 3:
            ROW3 = ROW_ENABLE;
            break;

        case 4:
            ROW4 = ROW_ENABLE;
            break;

        case 5:
            ROW5 = ROW_ENABLE;
            break;

        case 6:
            ROW6 = ROW_ENABLE;
            break;

        case 7:
            ROW7 = ROW_ENABLE;
            break;

        case 8:
            ROW8 = ROW_ENABLE;
            break;

        case 9:
            ROW9 = ROW_ENABLE;
            break;

        case 10:
            ROW10 = ROW_ENABLE;
            break;

        default:
            //printf("TAKE huowu fail: n error!!\r\n");
            break;
    } /* end of switch(n & 0x0f) */
}

//功能：货道对应行列电机停转
//入口参数：m为行编号，n为列编号
//说明：货道电机断电，停转
void Motor_HuoDao_Stop(u8 m, u8 n)
{
    //行列参数校验
    Detect_Line_And_Row(m, n);

    //对应货道电机断电，停转
    switch(m & 0x0f)
    {
        case 1:
            LINE1 = LINE_DISABLE;          //货道电机停转，对应行LINEx=LINE_DISENABLE,对应列ROWx=ROW_DISABLE
            break;

        case 2:
            LINE2 = LINE_DISABLE;
            break;

        case 3:
            LINE3 = LINE_DISABLE;
            break;

        case 4:
            LINE4 = LINE_DISABLE;
            break;

        case 5:
            LINE5 = LINE_DISABLE;
            break;

        case 6:
            LINE6 = LINE_DISABLE;
            break;

        case 7:
            LINE7 = LINE_DISABLE;
            break;

        case 8:
            LINE8 = LINE_DISABLE;
            break;

        case 9:
            LINE9 = LINE_DISABLE;
            break;

        case 10:
            LINE10 = LINE_DISABLE;
            break;

        default:
            //printf("TAKE huowu fail: m error!!\r\n");
            break;
    } /* end of switch(m & 0x0f) */

    //对应层的货道电机运行,对应列引脚
    switch(n & 0x0f)
    {
        case 1:
            ROW1 = ROW_DISABLE;       //货道电机运行，对应行LINEx=0,对应列ROWx=ROW_DISENABLE
            break;

        case 2:
            ROW2 = ROW_DISABLE;
            break;

        case 3:
            ROW3 = ROW_DISABLE;
            break;

        case 4:
            ROW4 = ROW_DISABLE;
            break;

        case 5:
            ROW5 = ROW_DISABLE;
            break;

        case 6:
            ROW6 = ROW_DISABLE;
            break;

        case 7:
            ROW7 = ROW_DISABLE;
            break;

        case 8:
            ROW8 = ROW_DISABLE;
            break;

        case 9:
            ROW9 = ROW_DISABLE;
            break;

        case 10:
            ROW10 = ROW_DISABLE;
            break;

        default:
            //printf("TAKE huowu fail: n error!!\r\n");
            break;
    } /* end of switch(n & 0x0f) */
}
/******************************************************************
功能：停止所有货道电机运行
入口参数：无
返回值：无
说明：直流货道电机引脚恢复初始的状态，行引脚为高，列引脚为低电平
*******************************************************************/
void Motor_HuoDao_All_Stop(void)
{
    LINE1 = LINE_DISABLE;
    ROW1 = ROW_DISABLE;
    LINE2 = LINE_DISABLE;
    ROW2 = ROW_DISABLE;
    LINE3 = LINE_DISABLE;
    ROW3 = ROW_DISABLE;
    LINE4 = LINE_DISABLE;
    ROW4 = ROW_DISABLE;
    LINE5 = LINE_DISABLE;
    ROW5 = ROW_DISABLE;
    LINE6 = LINE_DISABLE;
    ROW6 = ROW_DISABLE;
    LINE7 = LINE_DISABLE;
    ROW7 = ROW_DISABLE;
    LINE8 = LINE_DISABLE;
    ROW8 = ROW_DISABLE;
    LINE9 = LINE_DISABLE;
    ROW9 = ROW_DISABLE;
    LINE10 = LINE_DISABLE;
    ROW10 = ROW_DISABLE;
    //printf("Motor huodao stop!!\r\n");        //串口1打印PC调试信息
}

//行列参数校验
void Detect_Line_And_Row(u8 line, u8 row)
{
    if(line > LINE_MAX || line < LINE_MIN)
    {
        USART_DEBUG("LINE parameter error : out of range\r\n");
    }

    if(row > ROW_MAX || row < ROW_MIN)
    {
        USART_DEBUG("ROW parameter error : out of range\r\n");
    }

    return;
}

/*******************************************************************************************
功能：获取PA0(直流电机)和PA1(货道电机)的电压值
入口参数：n为选择模式位，n为2选择输出货道电机的电压值，n为1选择输出门电机和升降电机的电压值
返回值：正常情况下，返回对应电机的实际检测电压值x1000；
说明:
********************************************************************************************/
//int Get_motor_voltage(int n)
//{
//    u16 adcx1, adcx2;
//    float temp1, temp2;
////    u8 temp[128] = {0};      //发送缓存区，用于存储要发送ADC检测值，用于PC调试

//    if(n == 1) // 获取门电机和升降电机的电压值x1000
//    {
//        adcx1 = Get_Adc_Average(ADC_Channel_0, 5);
//        temp1 = (float)adcx1 * (3.3 / 4096);            //由ADC检测值转换为对应的电压值
//        adcx1 = temp1 * 1000;
////        memset(temp, 0, sizeof(temp));
////        sprintf((char*)temp, "ADC_value1:%umv\r\n", adcx1);
////        USART_SendBytes(USART1, temp, strlen((char*)temp));
//        return adcx1;
//    }

//    if(n == 2) //  获取货道电机的电压值x1000
//    {
//        adcx2 = Get_Adc_Average(ADC_Channel_1, 5);
//        temp2 = (float)adcx2 * (3.3 / 4096);            //由ADC检测值转换为对应的电压值
//        adcx2 = temp2 * 1000;
////        memset(temp, 0, sizeof(temp));
////        sprintf((char*)temp, "ADC_value2:%umv\r\n", adcx2);
////        USART_SendBytes(USART1, temp, strlen((char*)temp));
//        return adcx2;
//    }

//    return 0;
//}

/*********************************************************************************************
功能：升降电机+门电机电压异常
入口参数：无
返回值：无
说明：电路图中MOTOR5对应门电机，MOTOR4对应升降电机，MOTOR_1R为ADC12_IN0采样引脚，采样两个电机的
    电压值
**********************************************************************************************/
//void ADC_Dection1(void)
//{
//    u16 AD_value;
////    u8 temp[28] = {0};      //发送缓存区，用于存储要发送ADC检测值，用于PC调试
//    AD_value = Get_Adc_Average(ADC_Channel_0, 5);      //采样10次取平均值

////    memset(temp, 0, sizeof(temp));
////    sprintf((char*)temp, "ADC_Dection1:%u\r\n", AD_value);
////    USART_SendBytes(USART1, temp, strlen((char*)temp));

//    if(AD_value > 2482) // 2000mv
//    {
//        NOML4();  // 升降电机停止
//        NOML5();  // 门电机停止
//        Send_CMD(USART2, HBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT), LBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT)); // 升降电机+门电机电压超压
//        //Send_CMD(USART1, HBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT), LBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT));
//    }
//}

/********************************************************************************************
功能：货道电机电压异常
入口参数:无
返回值：无
说明：1R引脚为ADC12_IN1采样引脚，采样货道电机的电压值
********************************************************************************************/
//void ADC_Dection2(void)
//{
//    u16 AD_value;
////    u8 temp[28] = {0};      //发送缓存区，用于存储要发送ADC检测值，用于PC调试
//    AD_value = Get_Adc_Average(ADC_Channel_1, 5);

////    memset(temp, 0, sizeof(temp));
////    sprintf((char*)temp, "ADC_Dection2:%u\r\n", AD_value);
////    USART_SendBytes(USART1, temp, strlen((char*)temp));

//    if(AD_value > 2482) // 2000mv
//    {
//        Motor_HuoDao_Stop(); // 货道电机停止
//        Send_CMD(USART2, HBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT), LBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT)); // 货道电机电压超压
//        //Send_CMD(USART1, HBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT), LBYTE(USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT));
//    }
//}


