#ifndef __BSP_SYS_H
#define __BSP_SYS_H
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_delay.h"
#include "bsp_adc.h"
#include "bsp_timer.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_sys.h"
#include "bsp_huogui.h"
#include <string.h>
#include <math.h>

typedef enum
{
    FALSE = 0, TRUE = 1
} bool;

/*****************************************************/
#define   Version_Year       "47"       // year
#define   Version_Month      "11"      // month
#define   Version_Day        "20"      // day
/*****************************************************/
//UCOS编译标志位，使用UCOS需要编译对应程序，值为1；不使用UCOS不需要编译对应程序，值为0
#define SYSTEM_SUPPORT_OS       0
/*****************************************************/
//位带操作
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)       //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)       //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)


#define FOWD4() (DC41=0,DC40=1) // 升降电机正传
#define REWS4() (DC41=1,DC40=0) // 升降电机反转
#define FOWD5() (DC51=0,DC50=1) // 门电机正传
#define REWS5() (DC51=1,DC50=0) // 门电机反转
#define NOML4() (DC41=0,DC40=0) // 升降电机停止
#define NOML5() (DC51=0,DC50=0) // 门电机停止

// 货道电机
// LINE2=0,ROW1=1 2行1列电机运动
// LINE2=1,ROW1=0 2行1列电机停止
#define ASMT1_1() (LINE1=0,ROW1=1)
#define ASMT1_2() (LINE1=0,ROW2=1)
#define ASMT2_1() (LINE2=0,ROW1=1)
#define ASMT2_2() (LINE2=0,ROW2=1)
#define ASMT3_1() (LINE3=0,ROW1=1)
#define ASMT3_2() (LINE3=0,ROW2=1)
#define ASMT4_1() (LINE4=0,ROW1=1)
#define ASMT4_2() (LINE4=0,ROW2=1)
#define ASMT5_1() (LINE5=0,ROW1=1)
#define ASMT5_2() (LINE5=0,ROW2=1)
#define ASMT6_1() (LINE6=0,ROW1=1)
#define ASMT6_2() (LINE6=0,ROW2=1)
#define ASMT7_1() (LINE7=0,ROW1=1)
#define ASMT7_2() (LINE7=0,ROW2=1)
#define ASMT8_1() (LINE8=0,ROW1=1)
#define ASMT8_2() (LINE8=0,ROW2=1)


//行反馈信号
#define LINEFB1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//第1行反馈, 为高电平时为反馈信号
#define LINEFB2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//第2行反馈, 为高电平时为反馈信号
#define LINEFB3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//第3行反馈, 为高电平时为反馈信号
#define LINEFB4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//第4行反馈, 为高电平时为反馈信号
#define LINEFB5  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)//第5行反馈, 为高电平时为反馈信号
#define LINEFB6  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)//第6行反馈, 为高电平时为反馈信号
#define LINEFB7  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)//第7行反馈, 为高电平时为反馈信号
#define LINEFB8  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)//第8行反馈, 为高电平时为反馈信号
#define LINEFB9  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)//第9行反馈, 为高电平时为反馈信号
#define LINEFB10  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)//第10行反馈, 为高电平时为反馈信号

//层反馈信号，用于检测层数，常态下为高电平，到达某一层为低电平
//#define LEVEL1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
//#define LEVEL2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
//#define LEVEL3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)
//#define LEVEL4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)
//#define LEVEL5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
//#define LEVEL6  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
//#define LEVEL7  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)
//#define LEVEL8  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
//#define LEVEL9  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
//#define LEVEL10  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)


//货道电机行驱动，常态下,LINEx=1;如果让某一行的某一列电机运动，LINEx=0
#define LINE1 PBout(9)
#define LINE2 PBout(8)
#define LINE3 PBout(7)
#define LINE4 PBout(6)
#define LINE5 PBout(5)
#define LINE6 PBout(4)
#define LINE7 PBout(3)
#define LINE8 PBout(2)
#define LINE9 PBout(1)
#define LINE10 PBout(0)
//货道电机列驱动，常态下,ROWx=0;如果让某一行的某一列电机运动，ROWx=1
#define ROW1 PDout(9)
#define ROW2 PDout(8)
#define ROW3 PDout(7)
#define ROW4 PDout(6)
#define ROW5 PDout(5)
#define ROW6 PDout(4)
#define ROW7 PDout(3)
#define ROW8 PDout(2)
#define ROW9 PDout(1)
#define ROW10 PDout(0)

//掉货检测引脚，常态（接收到光）检测到低电平；掉货：检测到高电平
#define PUTThing  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)
//红外对射使能引脚，常态下，红外低电平不使能；高电平使能红外发射
#define Enable_HongWai  PEout(0)

/**********************************************************************************************/

//#define TIME_TO_DOOR   800     //升降电机从下限位上升到堵住货柜门时间

/**********************************************************************************************/

#endif




