#ifndef __bsp_huogui_h
#define __bsp_huogui_h

#include "bsp_sys.h"


//货道电机通电，断电对应行，列的电平
#define LINE_ENABLE     0
#define ROW_ENABLE      1
#define LINE_DISABLE    1
#define ROW_DISABLE     0

//行列参数最大值，最小值
#define LINE_MAX    6
#define LINE_MIN    1
#define ROW_MAX     10
#define ROW_MIN     1

//货道电机初始化函数，保证货道电机运行到触发区
void HuoDao_Init(void);
//货道电机运行，动作函数
void Motor_HuoDao_Move(u8 m, u8 n);
//货道电机全部断电，停止动作函数
void Motor_HuoDao_All_Stop(void);
//货道电机对应行列断电
void Motor_HuoDao_Stop(u8 m, u8 n);
//取货对应函数
u8 HUOWU_Take(u8 m, u8 n);
//测试函数：测试每一行货道运行情况
void HuoDao_line_test(u8 i);

//行列参数校验
void Detect_Line_And_Row(u8 line, u8 row);

//int Get_motor_voltage(int n);
//void ADC_Dection1(void);
//void ADC_Dection2(void);
//void close_door(u8 st);
#endif

