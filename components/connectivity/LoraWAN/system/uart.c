/*!
 * \file      uart.c
 *
 * \brief     UART driver implementation
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
#include "uart-board.h"
#include "uart.h"

void UartInit( Uart_t *obj, UartId_t uartId, PinNames tx, PinNames rx )
{
    if( obj->IsInitialized == false )
    {
        obj->IsInitialized = true;
        UartMcuInit( obj, uartId, tx, rx );
    }
}

void UartConfig( Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength, StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl )
{
    UartMcuConfig( obj, mode, baudrate, wordLength, stopBits, parity, flowCtrl );
}

void UartDeInit( Uart_t *obj )
{
    obj->IsInitialized = false;
    UartMcuDeInit( obj );
}

uint8_t UartPutChar( Uart_t *obj, uint8_t data )
{
     return UartMcuPutChar( obj, data );
}

uint8_t UartGetChar( Uart_t *obj, uint8_t *data )
{
    return UartMcuGetChar( obj, data );
}

uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    return UartMcuPutBuffer( obj, buffer, size );
}

uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes )
{
    return UartMcuGetBuffer( obj, buffer, size, nbReadBytes );
}
