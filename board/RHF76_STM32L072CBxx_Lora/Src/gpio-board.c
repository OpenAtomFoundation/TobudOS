/*!
 * \file      gpio-board.c
 *
 * \brief     Target board GPIO driver implementation
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
#include "rtc-board.h"
#include "gpio-board.h"
#if defined( BOARD_IOE_EXT )
#include "gpio-ioe.h"
#endif

static Gpio_t *GpioIrq[16];

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    if( pin < IOE_0 )
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        obj->pin = pin;

        if( pin == NC )
        {
            return;
        }

        obj->pinIndex = ( 0x01 << ( obj->pin & 0x0F ) );

        if( ( obj->pin & 0xF0 ) == 0x00 )
        {
            obj->port = GPIOA;
            __HAL_RCC_GPIOA_CLK_ENABLE( );
        }
        else if( ( obj->pin & 0xF0 ) == 0x10 )
        {
            obj->port = GPIOB;
            __HAL_RCC_GPIOB_CLK_ENABLE( );
        }
        else if( ( obj->pin & 0xF0 ) == 0x20 )
        {
            obj->port = GPIOC;
            __HAL_RCC_GPIOC_CLK_ENABLE( );
        }
        else if( ( obj->pin & 0xF0 ) == 0x30 )
        {
            obj->port = GPIOD;
            __HAL_RCC_GPIOD_CLK_ENABLE( );
        }
        else
        {
            assert_param( LORA_FAIL );
        }

        GPIO_InitStructure.Pin =  obj->pinIndex ;
        GPIO_InitStructure.Pull = obj->pull = type;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

        if( mode == PIN_INPUT )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        }
        else if( mode == PIN_ANALOGIC )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        }
        else if( mode == PIN_ALTERNATE_FCT )
        {
            if( config == PIN_OPEN_DRAIN )
            {
                GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
            }
            else
            {
                GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
            }
            GPIO_InitStructure.Alternate = value;
        }
        else // mode output
        {
            if( config == PIN_OPEN_DRAIN )
            {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
            }
            else
            {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
            }
        }

        // Sets initial output value
        if( mode == PIN_OUTPUT )
        {
            GpioMcuWrite( obj, value );
        }

        HAL_GPIO_Init( obj->port, &GPIO_InitStructure );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeInit( obj, pin, mode, config, type, value );
#endif
    }
}

void GpioMcuSetContext( Gpio_t *obj, void* context )
{
    obj->Context = context;
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    if( obj->pin < IOE_0 )
    {
        uint32_t priority = 0;

        IRQn_Type IRQnb = EXTI0_1_IRQn;
        GPIO_InitTypeDef   GPIO_InitStructure;

        if( irqHandler == NULL )
        {
            return;
        }

        obj->IrqHandler = irqHandler;

        GPIO_InitStructure.Pin =  obj->pinIndex;

        if( irqMode == IRQ_RISING_EDGE )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
        }
        else if( irqMode == IRQ_FALLING_EDGE )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
        }
        else
        {
            GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
        }

        GPIO_InitStructure.Pull = obj->pull;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

        HAL_GPIO_Init( obj->port, &GPIO_InitStructure );

        switch( irqPriority )
        {
        case IRQ_VERY_LOW_PRIORITY:
        case IRQ_LOW_PRIORITY:
            priority = 3;
            break;
        case IRQ_MEDIUM_PRIORITY:
            priority = 2;
            break;
        case IRQ_HIGH_PRIORITY:
            priority = 1;
            break;
        case IRQ_VERY_HIGH_PRIORITY:
        default:
            priority = 0;
            break;
        }

        switch( obj->pinIndex )
        {
        case GPIO_PIN_0:
        case GPIO_PIN_1:
            IRQnb = EXTI0_1_IRQn;
            break;
        case GPIO_PIN_2:
        case GPIO_PIN_3:
            IRQnb = EXTI2_3_IRQn;
            break;
        case GPIO_PIN_4:
        case GPIO_PIN_5:
        case GPIO_PIN_6:
        case GPIO_PIN_7:
        case GPIO_PIN_8:
        case GPIO_PIN_9:
        case GPIO_PIN_10:
        case GPIO_PIN_11:
        case GPIO_PIN_12:
        case GPIO_PIN_13:
        case GPIO_PIN_14:
        case GPIO_PIN_15:
            IRQnb = EXTI4_15_IRQn;
            break;
        default:
            break;
        }

        GpioIrq[( obj->pin ) & 0x0F] = obj;

        HAL_NVIC_SetPriority( IRQnb , priority, 0 );
        HAL_NVIC_EnableIRQ( IRQnb );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeSetInterrupt( obj, irqMode, irqPriority, irqHandler );
#endif
    }
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    if( obj->pin < IOE_0 )
    {
        // Clear callback before changing pin mode
        GpioIrq[( obj->pin ) & 0x0F] = NULL;

        GPIO_InitTypeDef   GPIO_InitStructure;

        GPIO_InitStructure.Pin =  obj->pinIndex ;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init( obj->port, &GPIO_InitStructure );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeRemoveInterrupt( obj );
#endif
    }
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    if( obj == NULL )
    {
        assert_param( LORA_FAIL );
    }

    if( obj->pin < IOE_0 )
    {
        // Check if pin is not connected
        if( obj->pin == NC )
        {
            return;
        }
        HAL_GPIO_WritePin( obj->port, obj->pinIndex , ( GPIO_PinState )value );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeWrite( obj, value );
#endif
    }
}

void GpioMcuToggle( Gpio_t *obj )
{
    if( obj == NULL )
    {
        assert_param( LORA_FAIL );
    }

    if( obj->pin < IOE_0 )
    {
        // Check if pin is not connected
        if( obj->pin == NC )
        {
            return;
        }
        HAL_GPIO_TogglePin( obj->port, obj->pinIndex );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeToggle( obj );
#endif
    }
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    if( obj == NULL )
    {
        assert_param( LORA_FAIL );
    }

    if( obj->pin < IOE_0 )
    {
        // Check if pin is not connected
        if( obj->pin == NC )
        {
            return 0;
        }
        return HAL_GPIO_ReadPin( obj->port, obj->pinIndex );
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        return GpioIoeRead( obj );
#else
        return 0;
#endif
    }
}

void EXTI0_1_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
}

void EXTI2_3_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
}

void EXTI4_15_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
}

void HAL_GPIO_EXTI_Callback( uint16_t gpioPin )
{
    uint8_t callbackIndex = 0;

    if( gpioPin > 0 )
    {
        while( gpioPin != 0x01 )
        {
            gpioPin = gpioPin >> 1;
            callbackIndex++;
        }
    }

    if( ( GpioIrq[callbackIndex] != NULL ) && ( GpioIrq[callbackIndex]->IrqHandler != NULL ) )
    {
        GpioIrq[callbackIndex]->IrqHandler( GpioIrq[callbackIndex]->Context );
    }
}
