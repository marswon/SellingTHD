#include "bsp_key.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);      //对应按键KEY0引脚
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);      //拨码开关1~4
}

/*******************************************************************************************************
功能：按键扫描函数
入口参数：mode:0,不支持连续按;1,支持连续按;
返回值：不同按键对应不同按键值，0，没有任何按键按下;1，KEY0按下;2，KEY1按下;3，KEY2按下;4，KEY3按下 WK_UP
说明：注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
********************************************************************************************************/
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; //按键按松开标志

    if(mode)
        key_up = 1; //支持连按

    if(key_up && (KEY0 == 0))
    {
        delay_ms(10);//去抖动
        key_up = 0;

        if(KEY0 == 0)
            return KEY0_PRES;
    }
    else if(KEY0 == 1)
        key_up = 1;

    return 0;// 无按键按下
}

/**************************************************************************************************
功能：拨码开关扫描函数
入口参数:无
返回值:不同拨码开关代表不同的值
说明：注意此函数有响应优先级,DKEY1>DKEY2>DKEY3>DKEY4!!
***************************************************************************************************/
u8 DKEY_Scan(void)
{
    if((DKEY1 == 0) | (DKEY2 == 0) | (DKEY3 == 0) | (DKEY4 == 0))
    {
        delay_ms(10);//去抖动

        if(DKEY1 == 0)
            return 1;
        else if(DKEY2 == 0)
            return 2;
        else if(DKEY3 == 0)
            return 3;
        else if(DKEY4 == 0)
            return 4;
    }

    return 0;// 无按键按下
}

