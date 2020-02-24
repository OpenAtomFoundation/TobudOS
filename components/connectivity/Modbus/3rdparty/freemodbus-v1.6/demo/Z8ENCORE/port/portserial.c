/*
 * FreeModbus Libary: Z8Encore Port for Z8F6422
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define PORTA_UART_RXD			0x10
#define PORTA_UART_TXD			0x20

#define RX_ENABLE				0x40
#define TX_ENABLE				0x80

#define UART0_RXD_INT_PENDING	0x10
#define UART0_TXD_INT_PENDING	0x08

#define UART0_RXD_INT_EN_H		0x10
#define UART0_RXD_INT_EN_L		0x10
#define UART0_TXD_INT_EN_H		0x08
#define UART0_TXD_INT_EN_L		0x08

#define UART_PARITY_ODD			0x18
#define UART_PARITY_EVEN		0x10

#define UART_ERRORS				0x70


/* ----------------------- static functions ---------------------------------*/
static void interrupt prvvUARTTxReadyISR( void );
static void interrupt prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if( xRxEnable )
    {
        IRQ0ENL |= UART0_RXD_INT_EN_L;
    }
    else
    {
        IRQ0ENL &= ~UART0_RXD_INT_EN_L;
    }
    if( xTxEnable )
    {
        IRQ0ENL |= UART0_TXD_INT_EN_L;
        /* Force Tx Interruption */
        IRQ0 |= UART0_TXD_INT_PENDING;
    }
    else
    {
        IRQ0ENL &= ~UART0_TXD_INT_EN_L;
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UCHAR           cfg = 0;

    if( ucDataBits != 8 )
    {
        return FALSE;
    }

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;

    case MB_PAR_ODD:
        cfg |= UART_PARITY_ODD;
        break;

    case MB_PAR_EVEN:
        cfg |= UART_PARITY_EVEN;
        break;

    default:
        return FALSE;
    }

    /* Baud Rate -> U0BR = (CLOCK/(16*BAUD) */
    U0BRH = CLOCK / ( 16 * ulBaudRate ) >> 8;
    U0BRL = CLOCK / ( 16 * ulBaudRate ) & 0x00FF;

    /* Enable Alternate Function of UART Pins */
    PAAF |= PORTA_UART_RXD | PORTA_UART_TXD;

    /* Disable Interrupts */
    DI(  );

    /* Configure Stop Bits, Parity and Enable Rx/Tx */
    U0CTL0 = cfg | RX_ENABLE | TX_ENABLE;
    U0CTL1 = 0x00;

    /* Low Priority Rx/Tx Interrupts */
    IRQ0ENH &= ~UART0_TXD_INT_EN_H & ~UART0_RXD_INT_EN_H;

    vMBPortSerialEnable( FALSE, FALSE );

    /* Set Rx/Tx Interruption Vectors */
    SET_VECTOR( UART0_RX, prvvUARTRxISR );
    SET_VECTOR( UART0_TX, prvvUARTTxReadyISR );

    /* Enable Interrupts */
    EI(  );

    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    while( !( U0STAT0 & 0x04 ) )
    {
    }

    U0D = ucByte;

    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    while( !( U0STAT0 & 0x80 ) )
    {
    }

    *pucByte = U0D;

    return TRUE;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static unsigned int uiCnt = 0;

static void interrupt
prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );

    IRQ0 &= ~UART0_TXD_INT_PENDING;
}


/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void interrupt
prvvUARTRxISR( void )
{
    UCHAR           tmp;

    /* Verify UART error flags */
    if( U0STAT0 & UART_ERRORS )
    {
        tmp = U0D;
    }
    else
    {
        pxMBFrameCBByteReceived(  );
    }

    IRQ0 &= ~UART0_RXD_INT_PENDING;
}
