/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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

#include <LPC214X.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void
sio_irq( void )
    __irq;
     static void     prvvUARTTxReadyISR( void );
     static void     prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
     void            vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        U1IER |= 0x01;
    }
    else
    {
        U1IER &= ~0x01;
    }
    if( xTxEnable )
    {
        U1IER |= 0x02;
        prvvUARTTxReadyISR(  );
    }
    else
    {
        U1IER &= ~0x02;
    }
}

void
vMBPortClose( void )
{
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    USHORT          cfg = 0;
    ULONG           reload = ( ( PCLK / ulBaudRate ) / 16UL );
    volatile char   dummy;

    ( void )ucPORT;
    /* Configure UART1 Pins */
    PINSEL0 = 0x00050000;       /* Enable RxD1 and TxD1 */

    switch ( ucDataBits )
    {
    case 5:
        break;

    case 6:
        cfg |= 0x00000001;
        break;

    case 7:
        cfg |= 0x00000002;
        break;

    case 8:
        cfg |= 0x00000003;
        break;

    default:
        bInitialized = FALSE;
    }

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;

    case MB_PAR_ODD:
        cfg |= 0x00000008;
        break;

    case MB_PAR_EVEN:
        cfg |= 0x00000018;
        break;
    }

    if( bInitialized )
    {
        U1LCR = cfg;            /* Configure Data Bits and Parity */
        U1IER = 0;              /* Disable UART1 Interrupts */

        U1LCR |= 0x80;          /* Set DLAB */
        U1DLL = reload;         /* Set Baud     */
        U1DLM = reload >> 8;    /* Set Baud */
        U1LCR &= ~0x80;         /* Clear DLAB */

        /* Configure UART1 Interrupt */
        VICVectAddr0 = ( unsigned long )sio_irq;
        VICVectCntl0 = 0x20 | 7;
        VICIntEnable = 1 << 7;  /* Enable UART1 Interrupt */

        dummy = U1IIR;          /* Required to Get Interrupts Started */
    }

    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    U1THR = ucByte;

    /* Wait till U0THR and U0TSR are both empty */
    while( !( U1LSR & 0x20 ) )
    {
    }

    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    while( !( U1LSR & 0x01 ) )
    {
    }

    /* Receive Byte */
    *pucByte = U1RBR;

    return TRUE;
}


void
sio_irq( void )
    __irq
{
    volatile char   dummy;
    volatile char   IIR;

    while( ( ( IIR = U1IIR ) & 0x01 ) == 0 )
    {
        switch ( IIR & 0x0E )
        {
        case 0x06:             /* Receive Line Status */
            dummy = U1LSR;      /* Just clear the interrupt source */
            break;

        case 0x04:             /* Receive Data Available */
        case 0x0C:             /* Character Time-Out */
            prvvUARTRxISR(  );
            break;

        case 0x02:             /* THRE Interrupt */
            prvvUARTTxReadyISR(  );
            break;

        case 0x00:             /* Modem Interrupt */
            dummy = U1MSR;      /* Just clear the interrupt source */
            break;

        default:
            break;
        }
    }

    VICVectAddr = 0xFF;         /* Acknowledge Interrupt */
}


/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void
prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void
prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
