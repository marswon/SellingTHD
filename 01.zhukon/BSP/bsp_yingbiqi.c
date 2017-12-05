#include "bsp_yingbiqi.h"

//功能：发送地址字节函数
//入口参数：addr为发送的地址字节
//说明：MDB协议定义了地址字节的格式，低3位为命令值，高5位为硬币识别器地址
void Send_ADDR_coin(u8 addr)
{
    u16 data = 0;
    assert_param(IS_COIN_COMMAND(addr));     //地址字节命令校验
    
    data = (0x01 << 8) | addr;      //对应模式位置1，表示地址字节
    USART_Send2Byte(USART2, data);   //发送对应地址字节
    USART_Send2Byte(USART1, data);   //PC调试，发送对应地址字节
//    USART_Send2Byte(USART1, addr);   //PC调试，发送对应地址字节
    data = addr;
    USART_Send2Byte(USART2, data);           //发送CHK检验和
    USART_Send2Byte(USART1, data);           //PC调试，发送CHK检验和
}

//功能：
//入口参数：
//说明：
//void Send_DATA_coin()
//{

//}





