#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stdio.h"
#include "bsp_sys.h"

/**********************************************/
#define USART_BUFFER_LEN 5000
#define SYS_ENABLE_IAP   0
//定义配置使能
#define USART1_CONFIG_ENABLED                (1)
#define USART2_CONFIG_ENABLED                (1)
#define USART3_CONFIG_ENABLED                (1)

//波特率为9600，避免数据有错误
#define USART1_BAUDRATE                       9600
#define USART2_BAUDRATE                       9600
#define USART3_BAUDRATE                       9600
/**********************************************/
#define HBYTE(a)               ((unsigned char)(((unsigned short)(a) >> 8) & 0xFF))
#define LBYTE(a)               ((unsigned char)((unsigned short)(a)& 0xff))
#define MAKEWORD(bLow,bHigh)   ((unsigned short)(((unsigned char)(bLow))|(((unsigned short)((unsigned char)(bHigh)))<<8)))
/**********************************************/

#define ZHUKON_DIANJI_NUMb1   0x0211//取货x行y列
#define DIANJI_ZHUKON_NUMb1   0x023A//出货成功
#define DIANJI_ZHUKON_NUMb2   0x023B//出货失败
//#define DIANJI_ZHUKON_NUMb3   0x023C//取货失败
//#define DIANJI_ZHUKON_NUMb4   0x023D//取货成功
//#define DIANJI_ZHUKON_NUMb5   0x023E//层反馈异常
#define DIANJI_ZHUKON_NUMb6   0x023F//货道检测异常


//准备升级电机板
#define USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji        0x024F
//开始升级电机板
#define USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji       0x025E
//结束升级电机板
#define USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji        0x025A
//电机板复位
#define USARTCMD_ZHUKONG_DIANJI_ResetDianji              0x0213

//获取电机板软件版本
#define USARTCMD_ZHUKONG_DIANJI_GetDianjiVer            0x024B
//主控板->电机板，ADC检测门电机和升降电机，电压超值
#define USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT             0x0215
//主控板->电机板，ADC检测货道电机，电压超值
#define USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT             0x0216
/**********************************************/

extern u8 flag_take_huowu;          //在货柜取货的标志位，取货过程中为1，其他情况下为0
extern u8 flag_start_flash;         //远程升级标志位，升级过程中为1，其他情况下为0

//extern u8 flag_test;                //调试标记位，用于PC机调试，根据不同值执行不同动作
/**********************************************/
unsigned short CRC16_isr(unsigned char *Dat, unsigned int len);
void GPIO_Configure(void);
void USART_SendByte(USART_TypeDef* USARTx, uint8_t byte);
void USART_SendBytes(USART_TypeDef* USARTx, uint8_t *str, uint8_t len);
void USART_SendBytess(USART_TypeDef* USARTx, char *str);
void USART_DEBUG(char *str);
void Handle_USART_CMD(u16 Data, char *Dat, u16 dat_len);
void Send_CMD(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD);//无数据区数据包
void Send_CMD_DAT(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD, char *dat, u16 dat_len);//完整数据包
u8 USART_BufferRead(u8 *data);
void USART_BufferWrite(u8 ntemp);
#define UPDATE_FLAG_FLASH_ADDR             0x800BB80    //写入升级标志地址
#define UPDATE_FLAG_FLASH_DATA             0xDDCCBBAA   //写入升级标志值
#define UPDATE_FLAG_FLASH_DATA1            0x3210ABCD   //写入升级标志值
void IAP_Write_UpdateFLAG(void);
void IAP_Reset_UpdateFLAG(void);
u8 IAP_Read_UpdateFLAG(void);

void SoftwareRESET(void);

#endif


