/*!
 * \file      i2c.c
 *
 * \brief     I2C driver implementation
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
#include <stdbool.h>
#include "utilities.h"
#include "i2c-board.h"

/*!
 * Flag to indicates if the I2C is initialized
 */
static bool I2cInitialized = false;

void I2cInit( I2c_t *obj, I2cId_t i2cId, PinNames scl, PinNames sda )
{
    if( I2cInitialized == false )
    {
        I2cInitialized = true;

        I2cMcuInit( obj, i2cId, scl, sda );
        I2cMcuFormat( obj, MODE_I2C, I2C_DUTY_CYCLE_2, true, I2C_ACK_ADD_7_BIT, 400000 );
    }
}

void I2cDeInit( I2c_t *obj )
{
    I2cInitialized = false;
    I2cMcuDeInit( obj );
}

void I2cResetBus( I2c_t *obj )
{
    I2cMcuResetBus( obj );
}

uint8_t I2cWrite( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t data )
{
    if( I2cInitialized == true )
    {
        if( I2cMcuWriteBuffer( obj, deviceAddr, addr, &data, 1 ) == LORA_FAIL )
        {
            // if first attempt fails due to an IRQ, try a second time
            if( I2cMcuWriteBuffer( obj, deviceAddr, addr, &data, 1 ) == LORA_FAIL )
            {
                return LORA_FAIL;
            }
            else
            {
                return LORA_SUCCESS;
            }
        }
        else
        {
            return LORA_SUCCESS;
        }
    }
    else
    {
        return LORA_FAIL;
    }
}

uint8_t I2cWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    if( I2cInitialized == true )
    {
        if( I2cMcuWriteBuffer( obj, deviceAddr, addr, buffer, size ) == LORA_FAIL )
        {
            // if first attempt fails due to an IRQ, try a second time
            if( I2cMcuWriteBuffer( obj, deviceAddr, addr, buffer, size ) == LORA_FAIL )
            {
                return LORA_FAIL;
            }
            else
            {
                return LORA_SUCCESS;
            }
        }
        else
        {
            return LORA_SUCCESS;
        }
    }
    else
    {
        return LORA_FAIL;
    }
}

uint8_t I2cRead( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *data )
{
    if( I2cInitialized == true )
    {
        return( I2cMcuReadBuffer( obj, deviceAddr, addr, data, 1 ) );
    }
    else
    {
        return LORA_FAIL;
    }
}

uint8_t I2cReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    if( I2cInitialized == true )
    {
        return( I2cMcuReadBuffer( obj, deviceAddr, addr, buffer, size ) );
    }
    else
    {
        return LORA_FAIL;
    }
}
