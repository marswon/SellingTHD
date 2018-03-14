#include "bsp_common.h"

void I2C_delay(void);
bool I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(u8 i);
u8 I2C_Read(void);
void I2C_NoACK(void);
bool I2C_WaitAck(void);
u8 I2C_ReadOneByte(u8 DeviceAddr, u8 High_8Addr, u8 Low_8Addr);
bool I2C_WriteOneByte(u8 DeviceAddr, u8 High_8Addr, u8 Low_8Addr, u8 wData);

void I2C_delay(void)
{
    delay_us(5);
}

// START: when CLK is high,DATA change form high --> low
bool I2C_Start(void)
{
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();

    if(!SDA_read)
        return FALSE;

    SDA_L;
    I2C_delay();

    if(SDA_read)
        return FALSE;

    SDA_L;
    I2C_delay();
    return TRUE;
}

void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

void I2C_Write(u8 i)
{
    u8 j;

    for(j = 0; j < 8; j++)
    {
        SCL_L;   // prepare for data
        I2C_delay();

        if((i << j) & 0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }

        I2C_delay();
        SCL_H;   // start send data
        I2C_delay();
        I2C_delay();
    }

    SCL_L;   // prepare for next data
    I2C_delay();
}

u8 I2C_Read(void)
{
    u8 i;
    u8 j = 0;
    SDA_H;
    I2C_delay();

    for(i = 0; i < 8; i++)
    {
        SCL_L;
        I2C_delay();
        SCL_H; // prepare for receive
        I2C_delay();
        I2C_delay();
        j <<= 1;

        if(SDA_read)
        {
            j++;
        }
    }

    I2C_delay();
    SCL_L;
    return j;
}

void I2C_NoACK(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

bool I2C_WaitAck(void)
{
    u8 k = 255;
    SCL_L;
    I2C_delay();
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    I2C_delay();

    while(SDA_read)
    {
        k--;
        I2C_delay();

        if(k == 0)
        {
            I2C_Stop();
            return FALSE;
        }
    }

    SCL_L;
    I2C_delay();
    return TRUE;
}

u8 I2C_ReadOneByte(u8 DeviceAddr, u8 High_8Addr, u8 Low_8Addr)
{
    u8 vDat;

    if(!I2C_Start())
    {
        I2C_Stop();
        return FALSE;
    }

    I2C_Write(DeviceAddr & 0xFE);

    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return FALSE;
    }

    I2C_Write(High_8Addr);
    I2C_WaitAck();
    I2C_Write(Low_8Addr);
    I2C_WaitAck();
    I2C_Start();
    I2C_Write(DeviceAddr | 0x01);
    I2C_WaitAck();
    vDat = I2C_Read();
    I2C_NoACK();
    I2C_Stop();
    return vDat;
}

bool I2C_WriteOneByte(u8 DeviceAddr, u8 High_8Addr, u8 Low_8Addr, u8 wData)
{
    if(!I2C_Start())
    {
        I2C_Stop();
        return FALSE;
    }

    I2C_Write(DeviceAddr & 0xFE);

    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return FALSE;
    }

    I2C_Write(High_8Addr);
    I2C_WaitAck();
    I2C_Write(Low_8Addr);
    I2C_WaitAck();
    I2C_Write(wData);
    I2C_WaitAck();
    I2C_Stop();
    return TRUE;
}
