#ifndef _BSP_USART_H
#define _BSP_USART_H
#include "stm32f10x_conf.h"

/**********************************************************/
//串口接收队列缓存长度
#define USART_BUFFER_LEN    4000
#define SYS_ENABLE_IAP      0
/**********************************************************/
//定义配置使能
#define USART1_CONFIG_ENABLED                (1)
#define USART2_CONFIG_ENABLED                (1)
#define USART3_CONFIG_ENABLED                (0)
#define UART4_CONFIG_ENABLED                 (0)
#define UART5_CONFIG_ENABLED                 (0)

//定义串口波特率
#define USART1_BAUDRATE                       115200
#define USART2_BAUDRATE                       115200
#define USART3_BAUDRATE                       115200
#define UART4_BAUDRATE                        115200
#define UART5_BAUDRATE                        115200
/**********************************************************/
#define DIANJI_DUISHE_NUMb4   0x035D//开启掉货检测功能
#define DIANJI_DUISHE_NUMb5   0x035E//关闭掉货检测功能
#define DIANJI_DUISHE_NUMb6   0x035F//收到层反馈、对射板红灯亮

#define DUISHE_DIANJI_NUMb1   0x0301//第一层反馈
#define DUISHE_DIANJI_NUMb2   0x0302//第二层反馈
#define DUISHE_DIANJI_NUMb3   0x0303//第三层反馈
#define DUISHE_DIANJI_NUMb4   0x0304//第四层反馈
#define DUISHE_DIANJI_NUMb5   0x0305//第五层反馈
#define DUISHE_DIANJI_NUMb6   0x0306//第六层反馈
#define DUISHE_DIANJI_NUMb7   0x0307//第七层反馈
#define DUISHE_DIANJI_NUMb8   0x0308//第八层反馈
#define DUISHE_DIANJI_NUMb9   0x0309//第九层反馈
#define DUISHE_DIANJI_NUMb10  0x030A//第十层反馈
#define DUISHE_DIANJI_NUMb15   0x030f//检测到货物


#define USARTCMD_DIANJI_DUISHE_WillUpdateDuishe       0x035B
#define USARTCMD_DIANJI_DUISHE_StopUpdateDuishe       0x035C
#define USARTCMD_DIANJI_DUISHE_GetDuisheVer           0x034C
/**********************************************************/

void Send_CMD(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD);
void Send_CMD_DAT(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD, char *dat, u16 dat_len);
u8 USART_BufferRead(u8 *data);
void USART_BufferWrite(u8 ntemp);
void Handle_USART_CMD(u16 Data, char *Dat, u16 dat_len);
/**********************************************************/
void USART_Configure(void);
void USART_SendByte(USART_TypeDef* USARTx, uint8_t byte);
void USART_SendBytes(USART_TypeDef* USARTx, uint8_t *str, uint8_t len);
void USART_SendBytess(USART_TypeDef* USARTx, char *str);
#define UPDATE_FLAG_FLASH_ADDR              0x800BB80    //写入升级标志地址
#define UPDATE_FLAG_FLASH_DATA             0xDDCCBBAA   //写入升级标志值
#define UPDATE_FLAG_FLASH_DATA1            0x3210ABCD   //写入升级标志值
void IAP_Write_UpdateFLAG(void);
void IAP_Reset_UpdateFLAG(void);
u8 IAP_Read_UpdateFLAG(void);



#endif  //_BSP_USART_H


