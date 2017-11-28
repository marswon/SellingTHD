#ifndef __SYS_H
#define __SYS_H
#include "stm32f10x.h"


//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
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

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入





#define FOWD4() (DC41=0,DC40=1)//电机4正转
#define REWS4() (DC41=1,DC40=0)//电机4反转
#define FOWD5() (DC51=0,DC50=1)//电机5正转,为关门
#define REWS5() (DC51=1,DC50=0)//电机5反转，为开门
#define NOML4() (DC41=0,DC40=0)//电机4停止
#define NOML5() (DC51=0,DC50=0)

#define ASMT1_1() (LINE1=0,ROW1=1)//货道电机驱动低三行第二列
#define STOP1_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT1_2() (LINE1=0,ROW2=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT2_1() (LINE2=0,ROW1=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT2_2() (LINE2=0,ROW2=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT3_1() (LINE3=0,ROW1=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT3_2() (LINE3=0,ROW2=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT4_1() (LINE4=0,ROW1=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT4_2() (LINE4=0,ROW2=1)//货道电机驱动低三行第二列
#define STOP3_2() (LINE3=1,ROW2=0)//货道电机驱动停止
#define ASMT5_1() (LINE5=0,ROW1=1)
#define ASMT5_2() (LINE5=0,ROW2=1)
#define ASMT6_1() (LINE6=0,ROW1=1)
#define ASMT6_2() (LINE6=0,ROW2=1)
#define ASMT7_1() (LINE7=0,ROW1=1)
#define ASMT7_2() (LINE7=0,ROW2=1)
#define ASMT8_1() (LINE8=0,ROW1=1)
#define ASMT8_2() (LINE8=0,ROW2=1)


#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)//读取按键0

#define LINEFB1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define LINEFB2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define LINEFB3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define LINEFB4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)
#define LINEFB5  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)
#define LINEFB6  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)
#define LINEFB7  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)
#define LINEFB8  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)
#define LINEFB9  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)
#define LINEFB10  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)

#define LEVEL1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
#define LEVEL2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define LEVEL3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)
#define LEVEL4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)
#define LEVEL5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define LEVEL6  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define LEVEL7  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)
#define LEVEL8  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define LEVEL9  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define LEVEL10  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)



#define PKEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define PKEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)
#define PKEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)
#define PKEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)

#define DKEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)
#define DKEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
#define DKEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)
#define DKEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)



#define PUTThing  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)



#define LED0 PAout(15)

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



#define DC51 PEout(0)
#define DC50 PEout(1)
#define DC41 PEout(2)
#define DC40 PEout(3)





//以下为汇编函数
void WFI_SET(void);     //执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void); //开启所有中断
void MSR_MSP(u32 addr); //设置堆栈地址






#endif




