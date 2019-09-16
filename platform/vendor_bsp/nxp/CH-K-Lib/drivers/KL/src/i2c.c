/**
  ******************************************************************************
  * @file    i2c.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.06.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "i2c.h"
#include "gpio.h"
#include "common.h"

typedef struct
{
    uint32_t instace;
    uint32_t sda;
    uint32_t scl;
}i2c_gpio;

static i2c_gpio i2c;

#define SDA_DDR_OUT()       GPIO_SetPinDir(i2c.instace, i2c.sda, 1)
#define SDA_DDR_IN()        GPIO_SetPinDir(i2c.instace, i2c.sda, 0)
#define SDA_H()             GPIO_PinWrite(i2c.instace, i2c.sda, 1)
#define SDA_L()             GPIO_PinWrite(i2c.instace, i2c.sda, 0)
#define SCL_H()             GPIO_PinWrite(i2c.instace, i2c.scl, 1)
#define SCL_L()             GPIO_PinWrite(i2c.instace, i2c.scl, 0)
#define SDA_IN()            GPIO_PinRead(i2c.instace, i2c.sda)
#define I2C_DELAY()         __NOP();


uint8_t I2C_Init(uint32_t MAP, uint32_t baudrate)
{
    uint8_t i;
    map_t * pq = (map_t*)&(MAP);
    
    /* open drain and pull up */
    for(i = 0; i < pq->pin_count; i++)
    {
        GPIO_Init(pq->io, pq->pin_start + i, kGPIO_Mode_OPP);
        GPIO_Init(pq->io, pq->pin_start + i, kGPIO_Mode_OPP);
        GPIO_PinWrite(pq->io, pq->pin_start + i, 1);
        SetPinPull(pq->io, pq->pin_start + i, 1);
    }

    /* i2c_gpio struct setup */
    i2c.instace = pq->io;
    
    switch(MAP)
    {
        case I2C1_SCL_PE01_SDA_PE00:
            i2c.scl = 1;i2c.sda = 0;
            break;
        case I2C0_SCL_PE19_SDA_PE18:
            i2c.scl = 19;i2c.sda = 18;
            break;
        case I2C0_SCL_PF22_SDA_PF23:
            i2c.scl = 22;i2c.sda = 23;
            break;
        case I2C0_SCL_PB00_SDA_PB01:
            i2c.scl = 0;i2c.sda = 1;
            break;
        case I2C0_SCL_PB02_SDA_PB03:
            i2c.scl = 2;i2c.sda = 3;
            break;
        case I2C1_SCL_PC10_SDA_PC11:
            i2c.scl = 10;i2c.sda = 11;
            break;
        case I2C0_SCL_PD08_SDA_PD09:
            i2c.scl = 8;i2c.sda = 9;
            break;
        case I2C0_SCL_PE24_SDA_PE25:
            i2c.scl = 24;i2c.sda = 25;
            break;
        case I2C1_SCL_PC01_SDA_PC02:
            i2c.scl = 1;i2c.sda = 2;
            break;
        default:
            LIB_TRACE("no PINMAP found\r\n");
            break;
    }
    return pq->ip;
}



static bool I2C_Start(void)
{
    SDA_DDR_OUT();
    SDA_H();
    SCL_H();
    I2C_DELAY();
    SDA_L();
    I2C_DELAY();
    SCL_L();
    return true;
}

static void I2C_Stop(void)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    SDA_H();
    I2C_DELAY();
}

static void I2C_Ack(void)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static void I2C_NAck(void)
{
    SCL_L();
    I2C_DELAY();
    SDA_H();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static bool I2C_WaitAck(void)
{
    uint8_t ack;
    SDA_DDR_IN();
    SCL_L();
    
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    ack = SDA_IN();
    SCL_L();
    SDA_DDR_OUT();
    
    return ack;
}

static void I2C_SendByte(uint8_t data)
{
    volatile uint8_t i;
    
    i = 8;
    while(i--)
    {
        if(data & 0x80) SDA_H();
        else SDA_L();
        data <<= 1;
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        SCL_L();
    }

}

static uint8_t I2C_GetByte(void)
{
    uint8_t i,byte;
    
    i = 8;
    byte = 0;

    SDA_DDR_IN();
    while(i--)
    {
        SCL_L();
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        byte = (byte<<1)|(SDA_IN() & 1);
    }
    SCL_L();
    SDA_DDR_OUT();
    return byte;
}

/**
 * @brief  I2C write mutiple data
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg addrLen    : len of slave register addr(in byte)
 *         @arg buf        : data buf
 *         @arg buf        : read len
 * @note 
 */
uint8_t I2C_BurstWrite(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&addr;
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err += I2C_WaitAck();

    while(addrLen--)
    {
        I2C_SendByte(*p++);
        err += I2C_WaitAck();
    }
    
    while(len--)
    {
        I2C_SendByte(*buf++);
        err += I2C_WaitAck();  
    }

    I2C_Stop();
    return err;
}

/**
 * @brief  write single register value
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg buf      : data pointer
 * @note   usually used on i2c sensor devices
 */
uint8_t I2C_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data)
{
    return I2C_BurstWrite(instance, chipAddr, addr, 1, &data, 1);
}

/**
 * @brief  I2C read mutiple data
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg addrLen    : len of slave register addr(in byte)
 *         @arg buf        : data buf
 *         @arg buf        : read len
 * @note 
 */
int32_t I2C_BurstRead(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&addr;
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err += I2C_WaitAck();
    
    while(addrLen--)
    {
        I2C_SendByte(*p++);
        err += I2C_WaitAck();
    }
    
    I2C_Start();
    I2C_SendByte(chipAddr+1);
    err += I2C_WaitAck();
    
    while(len--)
    {
        *buf++ = I2C_GetByte();
        if(len)
        {
            I2C_Ack();
        }
    }
    
    I2C_NAck();
    I2C_Stop();
    
    return err;
}

/**
 * @brief  proble i2c bus
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 * @note   see if it's available i2c slave on the bus
 */
uint8_t I2C_Probe(uint32_t instance, uint8_t chipAddr)
{
    uint8_t err;
    
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err = I2C_WaitAck();
    I2C_Stop();
    return err;
}

/**
 * @brief  read single register value
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg buf      : data pointer
 * @note   usually used on i2c sensor devices
 */
uint8_t I2C_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* buf)
{
    return I2C_BurstRead(instance, chipAddr, addr, 1, buf, 1);
}

/**
 * @brief  read single register value(SCCB)
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg buf      : data pointer
 * @note   usually used on i2c sensor devices
 */
int SCCB_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* buf)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    chipAddr <<= 1;
    
    while(retry--)
    {
        err = 0;
        I2C_Start();
        I2C_SendByte(chipAddr);
        err += I2C_WaitAck();
        
        I2C_SendByte(addr);
        err += I2C_WaitAck();
        
        I2C_Stop();
        I2C_Start();
        I2C_SendByte(chipAddr+1);
        err += I2C_WaitAck();
        
        *buf = I2C_GetByte();
       // err += I2C_WaitAck();
        
        I2C_NAck();
        I2C_Stop();
        if(!err)
        {
            break;
        }
    }

    return err;
}

/**
 * @brief  write single register value(SCCB)
 * @param  instance: instance of i2c moudle
 *         @arg chipAddr   : i2c slave addr
 *         @arg addr       : i2c slave register offset
 *         @arg buf      : data pointer
 * @note   usually used on i2c sensor devices
 */
int SCCB_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    
    while(retry--)
    {
        err = I2C_WriteSingleRegister(instance, chipAddr, addr, data);
        if(!err)
        {
            break;
        }
    }
    return err;
}



