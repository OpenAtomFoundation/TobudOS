/*
 * FreeModbus Libary: STR71x Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- STR71X includes ----------------------------------*/
#include "gpio.h"
#include "eic.h"
#include "uart.h"
#include "tim.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_UART_DEV             ( UART0 )
#define MB_UART_RX_PORT         ( GPIO0 )
#define MB_UART_RX_PIN          ( 8 )
#define MB_UART_TX_PORT         ( GPIO0 )
#define MB_UART_TX_PIN          ( 9 )
#define MB_UART_IRQ_CH          ( UART0_IRQChannel )
#define MB_UART_TX_QUEUE_LEN    ( 8 )
#define MB_IRQ_PRIORITY         ( 1 )

/* ----------------------- Static functions ---------------------------------*/
void            prvvMBSerialIRQHandler( void ) __attribute__ ( ( naked ) );

static inline BOOL prvMBPortTXIsEnabled(  );

static inline BOOL prvMBPortRXIsEnabled(  );

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            xResult = TRUE;
    UARTParity_TypeDef eUARTParity;
    UARTMode_TypeDef eUARTMode;

    (void)ucPort;

    switch ( eParity )
    {
        case MB_PAR_EVEN:
            eUARTParity = UART_EVEN_PARITY;
            break;
        case MB_PAR_ODD:
            eUARTParity = UART_ODD_PARITY;
            break;
        case MB_PAR_NONE:
            eUARTParity = UART_NO_PARITY;
            break;
    }

    switch ( ucDataBits )
    {
        case 7:
            if( eParity == MB_PAR_NONE )
            {
                /* not supported by our hardware. */
                xResult = FALSE;
            }
            else
            {
                eUARTMode = UARTM_7D_P;
            }
            break;
        case 8:
            if( eParity == MB_PAR_NONE )
            {
                eUARTMode = UARTM_8D;
            }
            else
            {
                eUARTMode = UARTM_8D_P;
            }
            break;
        default:
            xResult = FALSE;
    }

    if( xResult != FALSE )
    {
        /* Setup the UART port pins. */
        GPIO_Config( MB_UART_TX_PORT, 1 << MB_UART_TX_PIN, GPIO_AF_PP );
        GPIO_Config( MB_UART_RX_PORT, 1 << MB_UART_RX_PIN, GPIO_IN_TRI_CMOS );

        /* Configure the UART. */
        UART_OnOffConfig( MB_UART_DEV, ENABLE );
        UART_FifoConfig( MB_UART_DEV, DISABLE );
        UART_FifoReset( MB_UART_DEV, UART_RxFIFO );
        UART_FifoReset( MB_UART_DEV, UART_TxFIFO );
        UART_LoopBackConfig( MB_UART_DEV, DISABLE );
        UART_Config( MB_UART_DEV, ulBaudRate, eUARTParity, UART_1_StopBits,
                     eUARTMode );
        UART_RxConfig( UART0, ENABLE );
        vMBPortSerialEnable( FALSE, FALSE );

        /* Configure the IEC for the UART interrupts. */
        EIC_IRQChannelPriorityConfig( MB_UART_IRQ_CH, MB_IRQ_PRIORITY );
        EIC_IRQChannelConfig( MB_UART_IRQ_CH, ENABLE );
    }
    return xResult;
}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
        UART_ItConfig( MB_UART_DEV, UART_RxBufFull, ENABLE );
    else
        UART_ItConfig( MB_UART_DEV, UART_RxBufFull, DISABLE );

    if( xTxEnable )
        UART_ItConfig( MB_UART_DEV, UART_TxHalfEmpty, ENABLE );
    else
        UART_ItConfig( MB_UART_DEV, UART_TxHalfEmpty, DISABLE );
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    MB_UART_DEV->TxBUFR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = MB_UART_DEV->RxBUFR;
    return TRUE;
}

BOOL
prvMBPortTXIsEnabled(  )
{
    return ( MB_UART_DEV->IER & UART_TxHalfEmpty ) == UART_TxHalfEmpty;
}

BOOL
prvMBPortRXIsEnabled(  )
{
    return ( MB_UART_DEV->IER & UART_RxBufFull ) == UART_RxBufFull;
}


void
prvvMBSerialIRQHandler( void )
{
    portENTER_SWITCHING_ISR(  );

    static BOOL     xTaskWokenReceive = FALSE;
    static BOOL     xTaskWokenTransmit = FALSE;
    static USHORT   usStatus;

    usStatus = UART_FlagStatus( MB_UART_DEV );

    if( prvMBPortTXIsEnabled(  ) && ( usStatus & UART_TxHalfEmpty ) )
    {
        xTaskWokenReceive = pxMBFrameCBTransmitterEmpty(  );
    }
    if( prvMBPortRXIsEnabled(  ) && ( usStatus & UART_RxBufFull ) )
    {
        xTaskWokenReceive = pxMBFrameCBByteReceived(  );
    }

    /* End the interrupt in the EIC. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );

    portEXIT_SWITCHING_ISR( ( xTaskWokenReceive
                              || xTaskWokenTransmit ) ? pdTRUE : pdFALSE );
}
