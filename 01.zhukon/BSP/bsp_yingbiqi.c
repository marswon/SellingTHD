#include "bsp_yingbiqi.h"

//功能：发送基本命令函数
//入口参数：cmd为发送的基本命令字节
//返回值：正常返回1，异常返回0
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址。命令后，接着CHK检验和
u8 Send_ADDR_coin(u8 cmd)
{
    u16 data = 0;

    if(IS_COIN_BASIC_COMMAND(cmd) == 0)     //地址字节命令校验
    {
        return 0;   //如果不是定义的命令,会直接退出
    }

    data = (0x01 << 8) | cmd;      //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, data);   //发送对应地址字节
    USART_Send2Byte(USART1, data);   //PC调试，发送对应地址字节
    data = cmd;
    USART_Send2Byte(USART2, data);           //发送CHK检验和
    USART_Send2Byte(USART1, data);           //PC调试，发送CHK检验和
    return 1;
}

//功能：发送扩展命令函数
//入口参数：exp_cmd为发送的扩展命令字节
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址
u8 Send_CMD_EXP_coin(u16 exp_cmd, u8 *data)
{
    u16 cmd = 0;
    u8 dat_len = 0, i = 0;
    u8 dat[5] = {0};
    u8 num = 0;     //数据区总和

    if(IS_COIN_EXP_COMMAND(exp_cmd) == 0)     //地址字节命令校验
    {
        return 0;   //如果不是定义的命令,会直接退出
    }

    //根据扩展指令设置数据区长度
    if(FEATURE_ENABLE_EXP == exp_cmd)
    {
        dat_len = DAT_FEATURE_ENABLE;
    }
    else if(PAYOUT_EXP == exp_cmd)
    {
        dat_len = DAT_PAYOUT;
    }

    cmd = (0x01 << 8) | 0x0F;                  //对应模式位置1，表示地址字节,所以扩展字节高位都是0x0F
    USART_Send2Byte(USART2, cmd);              //发送对应地址字节
    USART_Send2Byte(USART1, cmd);              //PC调试，发送对应地址字节
    cmd = (0x01 << 8) | (exp_cmd & 0xFF);      //副指令，测试看看需要模式位置1么？
    USART_Send2Byte(USART2, cmd);              //发送副指令
    USART_Send2Byte(USART1, cmd);              //PC调试，发送副指令

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

    cmd = 0x0F + (exp_cmd & 0xFF) + num;       //计算校验和
    USART_SendByte(USART2, (u8) cmd);          //发送CHK检验和
    USART_SendByte(USART1, (u8) cmd);          //PC调试，发送CHK检验和
    return 1;
}

//功能：由扩展命令解析出需要的数据区长度
//说明：返回值为需要的数据区长度值，出错返回FLAG_ERROR_EXP
//u8 Get_len_EXP_coin(u16 exp_cmd)
//{
//    if(IDETIFICATION_EXP == exp_cmd)
//    {
//        return LEN_IDETIFICATION;
//    }
//    else if(FEATURE_ENABLE_EXP == exp_cmd)
//    {
//        return LEN_FEATURE_ENABLE;
//    }
//    else if(PAYOUT_EXP == exp_cmd)
//    {
//        return LEN_PAYOUT;
//    }
//    else if(PAYOUT_STATUS_EXP == exp_cmd)
//    {
//        return LEN_PAYOUT_STATUS;
//    }
//    else if(PAYOUT_VALUE_POLL_EXP == exp_cmd)
//    {
//        return LEN_PAYOUT_VALUE_POLL;
//    }
//    else if(SEND_DIAGNOSTIC_EXP == exp_cmd)
//    {
//        return LEN_SEND_DIAGNOSTIC;
//    }
//    else
//        return FLAG_ERROR_EXP;  //自定义，值区别于其他的返回值
//}

//功能：发送带地址的多个命令函数
//入口参数：cmd为发送命令的地址，len为命令长度
//
void Send_CMD_coin(u8 *cmd, u8 len)
{
}

//功能：
//入口参数：
//说明：
//void Send_DATA_coin()
//{

//}





