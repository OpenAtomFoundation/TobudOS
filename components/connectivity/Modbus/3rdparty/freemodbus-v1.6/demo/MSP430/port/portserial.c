/*
 * FreeModbus Libary: MSP430 Port
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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define U0_CHAR                 ( 0x10 )        /* Data 0:7-bits / 1:8-bits */

#define DEBUG_PERFORMANCE       ( 1 )

#if DEBUG_PERFORMANCE == 1
#define DEBUG_PIN_RX            ( 0 )
#define DEBUG_PIN_TX            ( 1 )
#define DEBUG_PORT_DIR          ( P1DIR )
#define DEBUG_PORT_OUT          ( P1OUT )
#define DEBUG_INIT( )           \
  do \
  { \
    DEBUG_PORT_DIR |= ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ); \
    DEBUG_PORT_OUT &= ~( ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ) ); \
  } while( 0 ); 
#define DEBUG_TOGGLE_RX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_RX )
#define DEBUG_TOGGLE_TX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_TX )

#else

#define DEBUG_INIT( )
#define DEBUG_TOGGLE_RX( )
#define DEBUG_TOGGLE_TX( )
#endif

/* ----------------------- Static variables ---------------------------------*/
UCHAR           ucGIEWasEnabled = FALSE;
UCHAR           ucCriticalNesting = 0x00;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    ENTER_CRITICAL_SECTION(  );
    if( xRxEnable )
    {
        IE1 |= URXIE0;
    }
    else
    {
        IE1 &= ~URXIE0;
    }
    if( xTxEnable )
    {
        IE1 |= UTXIE0;
        IFG1 |= UTXIFG0;
    }
    else
    {
        IE1 &= ~UTXIE0;
    }
    EXIT_CRITICAL_SECTION(  );
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    USHORT          UxCTL = 0;
    USHORT          UxBR = ( USHORT ) ( SMCLK / ulBaudRate );

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;
    case MB_PAR_ODD:
        UxCTL |= PENA;
        break;
    case MB_PAR_EVEN:
        UxCTL |= PENA | PEV;
        break;
    }
    switch ( ucDataBits )
    {
    case 8:
        UxCTL |= U0_CHAR;
        break;
    case 7:
        break;
    default:
        bInitialized = FALSE;
    }
    if( bInitialized )
    {
        ENTER_CRITICAL_SECTION(  );
        /* Reset USART */
        U0CTL |= SWRST;
        /* Initialize all UART registers */
        U0CTL = UxCTL | SWRST;
        /* SSELx = 11 = SMCLK. Use only if PLL is synchronized ! */
        U0TCTL = SSEL1 | SSEL0;
        U0RCTL = URXEIE;
        /* Configure USART0 Baudrate Registers. */
        U0BR0 = ( UxBR & 0xFF );
        U0BR1 = ( UxBR >> 8 );
        U0MCTL = 0;
        /* Enable UART */
        ME1 |= UTXE0 | URXE0;
        /* Clear reset flag. */
        U0CTL &= ~SWRST;

        /* USART0 TXD/RXD */
        P3SEL |= 0x30;
        P3DIR |= 0x10;

        EXIT_CRITICAL_SECTION(  );

        DEBUG_INIT( );
    }
    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    TXBUF0 = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = RXBUF0;
    return TRUE;
}

#if defined (__GNUC__)
interrupt (USART0RX_VECTOR) prvvMBSerialRXIRQHandler( void )
#else
void
prvvMBSerialRXIRQHandler( void ) __interrupt[USART0RX_VECTOR]
#endif
{
    DEBUG_TOGGLE_RX( );
    pxMBFrameCBByteReceived(  );
}

#if defined (__GNUC__)
interrupt (USART0TX_VECTOR) prvvMBSerialTXIRQHandler( void )
#else
void
prvvMBSerialTXIRQHandler( void ) __interrupt[USART0TX_VECTOR]
#endif
{
    DEBUG_TOGGLE_TX( );
    pxMBFrameCBTransmitterEmpty(  );
}

void
EnterCriticalSection( void )
{
    USHORT usOldSR;
    if( ucCriticalNesting == 0 )
    {
#if defined (__GNUC__)
        usOldSR = READ_SR;
        _DINT( );
#else
        usOldSR = _DINT( );
#endif
        ucGIEWasEnabled = usOldSR & GIE ? TRUE : FALSE;
    }
    ucCriticalNesting++;
}

void
ExitCriticalSection( void )
{
    ucCriticalNesting--;
    if( ucCriticalNesting == 0 )
    {
        if( ucGIEWasEnabled )
        {
            _EINT(  );
        }
    }
}
