/*!
 * \file      gpio-ioe.h
 *
 * \brief     IO expander driver implementation (based on the sx1509)
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
#include <stdlib.h>
#include <stdbool.h>
#include "gpio-ioe.h"
#include "sx1509.h"

static Gpio_t *GpioIrq[16];

void GpioIoeInit( Gpio_t *obj, PinNames pin, PinModes mode,  PinConfigs config, PinTypes type, uint32_t value )
{
    uint8_t regAdd = 0;
    uint8_t regVal = 0;
    uint8_t tempVal = 0;

    SX1509Init( );

    obj->pin = pin;
    obj->pinIndex = ( 0x01 << pin % 16 );

    if( ( obj->pin % 16 ) > 0x07 )
    {
        regAdd = RegDirB;
        obj->pinIndex = ( obj->pinIndex >> 8 ) & 0x00FF;
    }
    else
    {
        regAdd = RegDirA;
        obj->pinIndex = ( obj->pinIndex ) & 0x00FF;
    }

    SX1509Read( regAdd, &regVal );

    if( mode == PIN_OUTPUT )
    {
        regVal = regVal & ~obj->pinIndex;
    }
    else
    {
        regVal = regVal | obj->pinIndex;
    }
    SX1509Write( regAdd, regVal );


    if( ( obj->pin % 16 ) > 0x07 )
    {
        SX1509Read( RegOpenDrainB, &tempVal );
        if( config == PIN_OPEN_DRAIN )
        {
            SX1509Write( RegOpenDrainB, tempVal | obj->pinIndex );
        }
        else
        {
            SX1509Write( RegOpenDrainB, tempVal & ~obj->pinIndex );
        }
        regAdd = RegDataB;
    }
    else
    {
        SX1509Read( RegOpenDrainA, &tempVal );
        if( config == PIN_OPEN_DRAIN )
        {
            SX1509Write( RegOpenDrainA, tempVal | obj->pinIndex );
        }
        else
        {
            SX1509Write( RegOpenDrainA, tempVal & ~obj->pinIndex );
        }
        regAdd = RegDataA;
    }

    SX1509Read( regAdd, &regVal );

    // Sets initial output value
    if( value == 0 )
    {
        regVal = regVal & ~obj->pinIndex;
    }
    else
    {
        regVal = regVal | obj->pinIndex;
    }
    SX1509Write( regAdd, regVal );
}

void GpioIoeSetContext( Gpio_t *obj, void* context )
{
    obj->Context = context;
}

void GpioIoeSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    uint8_t regAdd = 0;
    uint8_t regVal = 0;
    uint8_t i = 0;
    uint16_t tempVal = 0;
    uint8_t val = 0;

    if( irqHandler == NULL )
    {
        return;
    }

    obj->IrqHandler = irqHandler;

    if( ( obj->pin % 16 ) > 0x07 )
    {
        regAdd = RegInterruptMaskB;
    }
    else
    {
        regAdd = RegInterruptMaskA;
    }

    SX1509Read( regAdd, &regVal );

    regVal = regVal & ~( obj->pinIndex );
    SX1509Write( regAdd, regVal );

    if( irqMode == IRQ_RISING_EDGE )
    {
        val = 0x01;
    }
    else if( irqMode == IRQ_FALLING_EDGE )
    {
        val = 0x02;
    }
    else // IRQ_RISING_FALLING_EDGE
    {
        val = 0x03;
    }

    tempVal = 0x0000;
    i = 0;
    while( tempVal != obj->pinIndex )
    {
        tempVal = 0x01 << i;
        i++;
    }

    if( i < 4 )
    {
        regAdd = RegSenseLowA;
    }
    else if( i < 9 )
    {
        regAdd = RegSenseHighA;
    }
    else if( i < 13 )
    {
        regAdd = RegSenseLowB;
    }
    else
    {
        regAdd = RegSenseHighB;
    }
    SX1509Read( regAdd, &regVal );

    switch( i )
    {
        case 1:
        case 5:
        case 9:
        case 13:
            regVal = ( regVal & REG_SENSE_PIN_MASK_1 ) | val;
            break;

        case 2:
        case 6:
        case 10:
        case 14:
            regVal = ( regVal & REG_SENSE_PIN_MASK_2 ) | ( val << 2 );
            break;

        case 3:
        case 7:
        case 11:
        case 15:
            regVal = ( regVal & REG_SENSE_PIN_MASK_3 ) | ( val << 4 );
            break;

        case 4:
        case 8:
        case 12:
        case 16:
            regVal = ( regVal & REG_SENSE_PIN_MASK_4 ) | ( val << 6 );
            break;
    }
    SX1509Write( regAdd, regVal );

    GpioIrq[obj->pin & 0x0F] = obj;
}

void GpioIoeRemoveInterrupt( Gpio_t *obj )
{
    uint8_t regAdd = 0;
    uint8_t regVal = 0;
    uint8_t i = 0;
    uint16_t tempVal = 0;

    // Clear callback before changing pin mode
    GpioIrq[obj->pin & 0x0F] = NULL;

    if( ( obj->pin % 16 ) > 0x07 )
    {
        regAdd = RegInterruptMaskB;
    }
    else
    {
        regAdd = RegInterruptMaskA;
    }

    SX1509Read( regAdd, &regVal );

    regVal = regVal | obj->pinIndex;
    SX1509Write( regAdd, regVal );

    tempVal = 0x0000;
    i = 0;
    while( tempVal != obj->pinIndex )
    {
        tempVal = 0x01 << i;
        i++;
    }

    if( i < 4 )
    {
        regAdd = RegSenseLowA;
    }
    else if( i < 9 )
    {
        regAdd = RegSenseHighA;
    }
    else if( i < 13 )
    {
        regAdd = RegSenseLowB;
    }
    else
    {
        regAdd = RegSenseHighB;
    }
    SX1509Read( regAdd, &regVal );

    switch( i )
    {
        case 1:
        case 5:
        case 9:
        case 13:
            regVal = ( regVal & REG_SENSE_PIN_MASK_1 );
            break;

        case 2:
        case 6:
        case 10:
        case 14:
            regVal = ( regVal & REG_SENSE_PIN_MASK_2 );
            break;

        case 3:
        case 7:
        case 11:
        case 15:
            regVal = ( regVal & REG_SENSE_PIN_MASK_3 );
            break;

        case 4:
        case 8:
        case 12:
        case 16:
            regVal = ( regVal & REG_SENSE_PIN_MASK_4 );
            break;
    }
    SX1509Write( regAdd, regVal );
}

void GpioIoeWrite( Gpio_t *obj, uint32_t value )
{
    uint8_t regAdd = 0;
    uint8_t regVal = 0;

    if( ( obj->pin % 16 ) > 0x07 )
    {
        regAdd = RegDataB;
    }
    else
    {
        regAdd = RegDataA;
    }

    SX1509Read( regAdd, &regVal );

    // Sets initial output value
    if( value == 0 )
    {
        regVal = regVal & ~obj->pinIndex;
    }
    else
    {
        regVal = regVal | obj->pinIndex;
    }
    SX1509Write( regAdd, regVal );
}

void GpioIoeToggle( Gpio_t *obj )
{
    GpioIoeWrite( obj, GpioIoeRead( obj ) ^ 1 );
}

uint32_t GpioIoeRead( Gpio_t *obj )
{
    uint8_t regAdd = 0;
    uint8_t regVal = 0;

    if( ( obj->pin % 16 ) > 0x07 )
    {
        regAdd = RegDataB;
    }
    else
    {
        regAdd = RegDataA;
    }

    SX1509Read( regAdd, &regVal );

    if( ( regVal & obj->pinIndex ) == 0x00 )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void GpioIoeInterruptHandler( void )
{
    uint8_t irqLsb = 0;
    uint8_t irqMsb = 0;
    uint16_t irq = 0;

    SX1509Read( RegInterruptSourceA, &irqLsb );
    SX1509Read( RegInterruptSourceB, &irqMsb );

    irq = ( irqMsb << 8 ) | irqLsb;
    if( irq != 0x00 )
    {
        for( uint16_t mask = 0x0001, pinIndex = 0; mask != 0x000; mask <<= 1, pinIndex++ )
        {
            if( ( irq & mask ) != 0 )
            {
                if( ( GpioIrq[pinIndex] != NULL ) && ( GpioIrq[pinIndex]->IrqHandler != NULL ) )
                {
                    GpioIrq[pinIndex]->IrqHandler( GpioIrq[pinIndex]->Context );
                }
            }
        }
    }

    // Clear all interrupts/events
    SX1509Write( RegInterruptSourceA, 0xFF );
    SX1509Write( RegInterruptSourceB, 0xFF );
    SX1509Write( RegEventStatusB, 0xFF );
    SX1509Write( RegEventStatusA, 0xFF );
}
