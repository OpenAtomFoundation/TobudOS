/*
 * FreeModbus Libary: HCS08 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Copyright (C) 2008 Trond Melen
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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void     prvvUARTTxReadyISR( void );
static void     prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts.
     * If xTxENable enable transmitter empty interrupts.
     */

    /* Compute control register value */
    SCI1C2 = 0x0C;              // TIE=0,TCIE=0,RIE=0,ILIE=0,TE=1,RE=1,RWU=0,SBK=0

    /* Enable the requested interrupts */
    if( xRxEnable )
        SCI1C2_RIE = 1;
    if( xTxEnable )
        SCI1C2_TIE = 1;
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    /* Compute baud rate generator divider */
    SCI1BD = ( USHORT ) ( ( ( BM_BUS_CLOCK + 16 / 2 ) / 16 + ulBaudRate / 2 ) / ulBaudRate );

    /* Configure the serial port */
//      SCI1C1 = 0x00;          // LOOPS=0,SCISWAI=0,RSRC=0,M=0,ILT=0,PE=0,PT=0
    if( ucDataBits == 8 && eParity != MB_PAR_NONE )
        SCI1C1_M = 1;
    if( eParity != MB_PAR_NONE )
        SCI1C1_PE = 1;
    if( eParity == MB_PAR_ODD || ( eParity == MB_PAR_NONE && ucDataBits == 7 ) )
        SCI1C1_PT = 1;

    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    while( SCI1S1_TDRE == FALSE );
    SCI1D = ucByte;
    SCI1C2_TIE = TRUE;

    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    BOOL            parityOK;

    while( SCI1S1_RDRF == FALSE );
    parityOK = !SCI1S1_PF;
    if( SCI1C1_M )
        *pucByte = SCI1C3;      // for coherent 9 bit reads of the receiver buffer
    *pucByte = SCI1D;
    if( !SCI1C1_M )
        if( SCI1C1_PE || ( !SCI1C1_PE && SCI1C1_PT ) )
            *pucByte &= 0x7F;
    SCI1C2_RIE = TRUE;

    return parityOK;            // return TRUE if Parity Error Flag was not set
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
interrupt VectorNumber_Vsci1tx void
prvvUARTTxReadyISR( void )
{
    SCI1C2_TIE = FALSE;
    ( void )pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
interrupt VectorNumber_Vsci1rx void
prvvUARTRxISR( void )
{
    SCI1C2_RIE = FALSE;
    ( void )pxMBFrameCBByteReceived(  );
}


/* assert( FALSE ) generate software interrupts so they should be handled somehow
 * This implementation will generate padding charactres until the stack overflows
 */
interrupt VectorNumber_Vswi void
prvvAssertISR( void )
{
    ( void )xMBPortSerialPutByte( 0x00 );
    SCI1C2_TIE = FALSE;
    EnableInterrupts;
}
