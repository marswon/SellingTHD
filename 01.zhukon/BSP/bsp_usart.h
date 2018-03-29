#ifndef _BSP_USART_H
#define _BSP_USART_H
#include "stm32f10x_conf.h"
#include "bsp_common.h"

/***************************************************/
//串口接收队列缓存长度
#define USART_BUFFER_LEN 5000
//弹簧道主控升级HEX版本，定义ZHUKONG_SYS_UPDATE宏，其他版本没有定义
#ifndef ZHUKONG_SYS_UPDATE
#define SYS_ENABLE_IAP      0
#else
#define SYS_ENABLE_IAP      1
#endif

/***************************************************/
//定义配置使能
#define USART1_CONFIG_ENABLED                (1)
#define USART2_CONFIG_ENABLED                (1)
#define USART3_CONFIG_ENABLED                (1)
#define UART4_CONFIG_ENABLED                 (1)
#define UART5_CONFIG_ENABLED                 (0)

//定义串口波特率
#define USART1_BAUDRATE                       9600
//串口2连接纸币器，硬币器，定义的波特率为9600
#define USART2_BAUDRATE                       9600
#define USART3_BAUDRATE                       9600
#define UART4_BAUDRATE                        9600
#define UART5_BAUDRATE                        2400

extern char dat_quehuo[3];        //缓存取货几行几列，用于纸币器使用
extern u8 price_num;       //货物价格
extern bool flag_take_huowu;
extern bool flag_chu_success;
extern bool flag_chu_fail;
extern u8 BUF_common[40];
extern u8 BUF_0A[30];
extern u8 BUF_POLL[20];       //纸币器和硬币器POLL指令，回复数据专用缓存
extern u8 Wptr_YING;        //硬币器写指针
extern u16 Wptr_mode;      //硬币器发送串口指令模式位，默认为0
extern u8 flag_test;                //调试标记位，用于PC机调试，根据不同值执行不同动作

/***************************************************/
#define UPDATE_FLAG_FLASH_ADDR             0x800BB80    //写入升级标志地址
#define UPDATE_FLAG_FLASH_DATA             0xDDCCBBAA   //写入升级标志值
#define UPDATE_FLAG_FLASH_DATA1            0x3210ABCD   //写入升级标志值
void IAP_Write_UpdateFLAG(void);
void IAP_Reset_UpdateFLAG(void);
u8 IAP_Read_UpdateFLAG(void);
void Handle_USART_CMD(u16 Data, char *Dat, u16 dat_len);


/***************************************************/
//  串口协议
/***************************************************/
#define ZHUKON_ANZHUO_NUMb1   0x013A//出货成功
#define DIANJI_ZHUKON_NUMb1   0x023A//出货成功

#define ZHUKON_ANZHUO_NUMb2   0x013B//出货失败
#define DIANJI_ZHUKON_NUMb2   0x023B//出货失败

//#define ZHUKON_ANZHUO_NUMb3   0x013C//取货失败
//#define DIANJI_ZHUKON_NUMb3   0x023C//取货失败

//#define ZHUKON_ANZHUO_NUMb4   0x013D//取货成功
//#define DIANJI_ZHUKON_NUMb4   0x023D//取货成功

#define ZHUKON_ANZHUO_NUMb5   0x013E//售货机异常
#define DIANJI_ZHUKON_NUMb5   0x023E//售货机异常

#define ZHUKON_ANZHUO_NUMb6   0x013F//货道检测异常
#define DIANJI_ZHUKON_NUMb6   0x023F//货道检测异常

#define ZHUKON_ANZHUO_NUMb7   0x0139//后台显示

#define ANZHUO_ZHUKON_HANGLIE 0x0111  // 取货, xx行xx列,需要硬币器，纸币器参与，校验投入的货币
#define ANZHUO_ZHUKON_QUHUO   0x0114  // 取货, xx行xx列,不需要硬币器，纸币器参与，适用于支付宝，微信。只管出货
#define ZHUKON_DIANJI_HANGLIE 0x0211  // 取货, xx行xx列

#define USARTCMD_ANDROID_ZHUKONG_DIANJI1VOLT            0x0115 // 升降电机+门电机电压超压
#define USARTCMD_ZHUKONG_DIANJI_DIANJI1VOLT             0x0215 // 升降电机+门电机电压超压

#define USARTCMD_ANDROID_ZHUKONG_DIANJI2VOLT            0x0116 // 货道电机电压超压
#define USARTCMD_ZHUKONG_DIANJI_DIANJI2VOLT             0x0216 // 货道电机电压超压

//准备升级主控
#define USARTCMD_ANDROID_ZHUKONG_WillUpdateZhukong      0x014D
//开始升级主控
#define USARTCMD_ANDROID_ZHUKONG_StartUpdateZhukong     0x015D
//结束升级主控
#define USARTCMD_ANDROID_ZHUKONG_StopUpdateZhukong      0x014E

//准备升级电机
#define USARTCMD_ANDROID_ZHUKONG_WillUpdateDianji       0x014F
#define USARTCMD_ZHUKONG_DIANJI_WillUpdateDianji        0x024F
//开始升级电机
#define USARTCMD_ANDROID_ZHUKONG_StartUpdateDianji      0x015E
#define USARTCMD_ZHUKONG_DIANJI_StartUpdateDianji       0x025E
//停止升级电机
#define USARTCMD_ANDROID_ZHUKONG_StopUpdateDianji       0x015A
#define USARTCMD_ZHUKONG_DIANJI_StopUpdateDianji        0x025A

//读取电机版本
#define USARTCMD_ANDROID_ZHUKONG_GetDianjiVer           0x014B
#define USARTCMD_ZHUKONG_DIANJI_GetDianjiVer            0x024B
//读取主控版本
#define USARTCMD_ANDROID_ZHUKONG_GetZhukongVer          0x014A
//投入金额不足，主控->安卓
#define USARTCMD_ZHUKONG_ANDROID_CoinNoEnough           0x0120
//纸币器现金盒装满，主控->安卓
#define USARTCMD_ZHUKONG_ANDROID_ZBQ_FULL               0x0119
//单独测试货道电机，不需要纸币器和硬币器参与
#define USARTCMD_ANDROID_ZHUKONG_HUODAO                 0x0121
//取货后回复安卓余额
#define USARTCMD_ZHUKONG_ANDROID_Reply_Balance          0x0122
//安卓查询余额指令，回复安卓上次取货后，投入的金额
#define USARTCMD_ANDROID_ZHUKONG_GetBalance             0x0123


//USART发送9位数据
void USART_Send2Byte(USART_TypeDef* USARTx, uint16_t byte);

void USART_SendByte(USART_TypeDef* USARTx, uint8_t byte);
void USART_SendBytes(USART_TypeDef* USARTx, uint8_t *str, uint8_t len);
void USART_SendBytess(USART_TypeDef* USARTx, char *str);
void USART_DEBUG(char *str);

//串口2接收和发送
void BufWrite_COIN(u8 ntemp);


void USART_BufferWrite(u8 ntemp);
u16 USART_BufferLength(void);
u8 USART_BufferRead(u8 *data);
void Send_CMD(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD);
void Send_CMD_DAT(USART_TypeDef* USARTx, u8 HCMD, u8 LCMD, char *dat, u16 dat_len);

//功能：连续出货缓存发送的货架信息
u8 Continue_BufferWrite(const u8 line, const u8 row);
//功能：读取缓存的货架信息
u8 Continue_BufferRead(u8* dat_line, u8* dat_row);

//软件复位操作
void SoftwareRESET(void);

#endif  //_BSP_USART_H


