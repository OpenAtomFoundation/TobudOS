/*!
 * \file      i2c-board.c
 *
 * \brief     Target board I2C driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "stm32l0xx.h"
#include "utilities.h"
#include "board-config.h"
#include "i2c-board.h"

/*!
 *  The value of the maximal timeout for I2C waiting loops
 */
#define TIMEOUT_MAX                                 0x8000

static I2C_HandleTypeDef I2cHandle = { 0 };

static I2cAddrSize I2cInternalAddrSize = I2C_ADDR_SIZE_8;

void I2cMcuInit( I2c_t *obj, I2cId_t i2cId, PinNames scl, PinNames sda )
{
    __HAL_RCC_I2C1_CLK_DISABLE( );
    __HAL_RCC_I2C1_CLK_ENABLE( );
    __HAL_RCC_I2C1_FORCE_RESET( );
    __HAL_RCC_I2C1_RELEASE_RESET( );

    obj->I2cId = i2cId;

    I2cHandle.Instance  = ( I2C_TypeDef * )I2C1_BASE;

    GpioInit( &obj->Scl, scl, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, GPIO_AF1_I2C1 );
    GpioInit( &obj->Sda, sda, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, GPIO_AF1_I2C1 );
}

void I2cMcuFormat( I2c_t *obj, I2cMode mode, I2cDutyCycle dutyCycle, bool I2cAckEnable, I2cAckAddrMode AckAddrMode, uint32_t I2cFrequency )
{
    __HAL_RCC_I2C1_CLK_ENABLE( );

    if( I2cFrequency == 100000 )
    {
        I2cHandle.Init.Timing = 0x00707CBB;
    }
    else if( I2cFrequency == 400000 )
    {
        I2cHandle.Init.Timing = 0x00300F38;
    }

    I2cHandle.Init.OwnAddress1 = 0;
    I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2 = 0;
    I2cHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init( &I2cHandle );

    HAL_I2CEx_ConfigAnalogFilter( &I2cHandle, I2C_ANALOGFILTER_ENABLE );
}

void I2cMcuResetBus( I2c_t *obj )
{
    __HAL_RCC_I2C1_CLK_DISABLE( );
    __HAL_RCC_I2C1_CLK_ENABLE( );
    __HAL_RCC_I2C1_FORCE_RESET( );
    __HAL_RCC_I2C1_RELEASE_RESET( );

    GpioInit( &obj->Scl, I2C_SCL, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, GPIO_AF1_I2C1 );
    GpioInit( &obj->Sda, I2C_SDA, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, GPIO_AF1_I2C1 );

    I2cMcuFormat( obj, MODE_I2C, I2C_DUTY_CYCLE_2, true, I2C_ACK_ADD_7_BIT, 400000 );
}

void I2cMcuDeInit( I2c_t *obj )
{

    HAL_I2C_DeInit( &I2cHandle );

    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    __HAL_RCC_I2C1_CLK_DISABLE( );

    GpioInit( &obj->Scl, obj->Scl.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &obj->Sda, obj->Sda.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

void I2cSetAddrSize( I2c_t *obj, I2cAddrSize addrSize )
{
    I2cInternalAddrSize = addrSize;
}

uint8_t I2cMcuWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    uint8_t status = LORA_FAIL;
    uint16_t memAddSize = 0;

    if( I2cInternalAddrSize == I2C_ADDR_SIZE_8 )
    {
        memAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        memAddSize = I2C_MEMADD_SIZE_16BIT;
    }
    status = ( HAL_I2C_Mem_Write( &I2cHandle, deviceAddr, addr, memAddSize, buffer, size, 2000 ) == HAL_OK ) ? LORA_SUCCESS : LORA_FAIL;

    return status;
}

uint8_t I2cMcuReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    uint8_t status = LORA_FAIL;
    uint16_t memAddSize = 0;

    if( I2cInternalAddrSize == I2C_ADDR_SIZE_8 )
    {
        memAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        memAddSize = I2C_MEMADD_SIZE_16BIT;
    }
    status = ( HAL_I2C_Mem_Read( &I2cHandle, deviceAddr, addr, memAddSize, buffer, size, 2000 ) == HAL_OK ) ? LORA_SUCCESS : LORA_FAIL;

    return status;
}

uint8_t I2cMcuWaitStandbyState( I2c_t *obj, uint8_t deviceAddr )
{
    uint8_t status = LORA_FAIL;

    status = ( HAL_I2C_IsDeviceReady( &I2cHandle, deviceAddr, 300, 4096 ) == HAL_OK ) ? LORA_SUCCESS : LORA_FAIL;

    return status;
}
