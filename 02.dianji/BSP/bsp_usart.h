#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "stdio.h"
#include "bsp_sys.h"

/**********************************************/
#define USART_BUFFER_LEN 5000
#define SYS_ENABLE_IAP   0
/**********************************************/
#define HBYTE(a)               ((unsigned char)(((unsigned short)(a) >> 8) & 0xFF))
#define LBYTE(a)               ((unsigned char)((unsigned short)(a)& 0xff))
#define MAKEWORD(bLow,bHigh)   ((unsigned short)(((unsigned char)(bLow))|(((unsigned short)((unsigned char)(bHigh)))<<8)))
/**********************************************/
#define ZHUKON_DIANJI_NUMb1   0x0211//取货x行y列
#define DIANJI_ZHUKON_NUMb1   0x023A//出货成功
#define DIANJI_ZHUKON_NUMb2   0x023B//出货失败
#define DIANJI_ZHUKON_NUMb3   0x023C//取货失败
#define DIANJI_ZHUKON_NUMb4   0x023D//取货成功
#define DIANJI_ZHUKON_NUMb5   0x023E//层反馈异常
#define DIANJI_ZHUKON_NUMb6   0x023F//货道检测异常

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

//准备升级电机板
#define USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji        0x024F

//结束升级电机板
#define USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji       0x025A

//获取电机板软件版本
#define USARTCMD_ZHUKONG_DIANJI_GetDianjiVer            0x024B

//升级对射板
#define USARTCMD_ZHUKONG_DIANJI_WillUpdateDuishe        0x025B
#define USARTCMD_DIANJI_DUISHE_WillUpdateDuishe         0x035B

//开始升级对射板
#define USARTCMD_ZHUKONG_DIANJI_StartUpdateDuishe       0x025F
#define USARTCMD_DIANJI_DUISHE_StartUpdateDuishe        0x035F

//电机板->对射板，停止升级对射板
#define USARTCMD_ZHUKONG_DIANJI_StopUpdateDuishe        0x025C
#define USARTCMD_DIANJI_DUISHE_StopUpdateDuishe         0x035C

//获取对射板版本
#define USARTCMD_ZHUKONG_DIANJI_GetDuisheVer            0x024C
#define USARTCMD_DIANJI_DUISHE_GetDuisheVer             0x034C

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
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void uart4_init(u32 bound);
void uart5_init(u32 bound);
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



#endif


