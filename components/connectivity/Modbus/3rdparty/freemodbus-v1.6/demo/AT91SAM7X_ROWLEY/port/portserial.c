/*
 * FreeModbus Libary: ARM7/AT91SAM7X Port
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
#include <stdlib.h>
#include <assert.h>

/* ----------------------- Platform includes --------------------------------*/
#include "AT91SAM7X256.h"
#include "lib_AT91SAM7X256.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

#define USART_INTERRUPT_LEVEL           ( 7 )
#define USART_USART0_IDX                ( 0 )
#define USART_USART1_IDX                ( 1 )

#define IDX_INVALID                     ( 255 )
#define UART_BAUDRATE_MIN               ( 300 )
#define UART_BAUDRATE_MAX               ( 115200 )

#define UART_INIT( ubIdx )      do { \
    if( AT91C_ID_US0 == ubIdx ) \
    { \
        AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, AT91C_PA0_RXD0 | AT91C_PA1_TXD0 | AT91C_PA3_RTS0, 0 ); \
    } \
    else if( AT91C_ID_US1 == ubIdx ) \
    { \
        AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, AT91C_PA5_RXD1 | AT91C_PA6_TXD1 | AT91C_PA8_RTS1, 0 ); \
    } \
    else \
    { \
        assert( 0 ); \
    } \
} while( 0 )

#define HDL_RESET( x ) do { \
    ( x )->pxCOM = NULL; \
    ( x )->uiAT91C_ID_USX = 0; \
    ( x )->pvIRQHandlerFN = NULL; \
    ( x )->bIsRxEnabled = FALSE; \
    ( x )->bIsTxEnabled = FALSE; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/

typedef struct
{
    AT91PS_USART    pxCOM;
    unsigned int    uiAT91C_ID_USX;
    volatile BOOL   bIsRxEnabled;
    volatile BOOL   bIsTxEnabled;
    void            ( *pvIRQHandlerFN ) ( void );
} xMBPSerialIntHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xMBPSerialIntHandle xSerialHdls[1];
STATIC BOOL     bIsInitalized = FALSE;

/* ----------------------- Static functions ---------------------------------*/
STATIC void     vUSART0ISR( void ) __attribute__ ( ( interrupt( "IRQ" ) ) );
STATIC void     vUSART1ISR( void ) __attribute__ ( ( interrupt( "IRQ" ) ) );

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bOkay = TRUE;
    unsigned int    uiUARTMode = 0;

    ENTER_CRITICAL_SECTION(  );
    if( !bIsInitalized )
    {
        HDL_RESET( &xSerialHdls[0] );
        bIsInitalized = TRUE;
    }

    uiUARTMode = AT91C_US_USMODE_RS485 | AT91C_US_CLKS_CLOCK;
    switch ( eParity )
    {
    case MB_PAR_NONE:
        uiUARTMode |= AT91C_US_PAR_NONE;
        uiUARTMode |= AT91C_US_NBSTOP_2_BIT;
        break;
    case MB_PAR_EVEN:
        uiUARTMode |= AT91C_US_PAR_EVEN;
        uiUARTMode |= AT91C_US_NBSTOP_1_BIT;
        break;
    case MB_PAR_ODD:
        uiUARTMode |= AT91C_US_PAR_ODD;
        uiUARTMode |= AT91C_US_NBSTOP_1_BIT;
        break;
    default:
        break;
    }
    switch ( ucDataBits )
    {
    case 8:
        uiUARTMode |= AT91C_US_CHRL_8_BITS;
        break;
    case 7:
        uiUARTMode |= AT91C_US_CHRL_7_BITS;
        break;
    default:
        break;
    }
    if( bOkay )
    {
        if( ( ucPort == USART_USART0_IDX ) && ( NULL == xSerialHdls[0].pxCOM ) )
        {
            xSerialHdls[0].pxCOM = AT91C_BASE_US0;
            xSerialHdls[0].uiAT91C_ID_USX = AT91C_ID_US0;
            xSerialHdls[0].pvIRQHandlerFN = vUSART0ISR;
        }
        else if( ( ucPort == USART_USART1_IDX ) && ( NULL == xSerialHdls[0].pxCOM ) )
        {
            xSerialHdls[0].pxCOM = AT91C_BASE_US1;
            xSerialHdls[0].uiAT91C_ID_USX = AT91C_ID_US1;
            xSerialHdls[0].pvIRQHandlerFN = vUSART1ISR;
        }
        else
        {
            bOkay = FALSE;
        }

        if( bOkay )
        {
            AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << xSerialHdls[0].uiAT91C_ID_USX );
            AT91F_US_Configure( xSerialHdls[0].pxCOM, configCPU_CLOCK_HZ, uiUARTMode, ulBaudRate,
                                0 );
            xSerialHdls[0].pxCOM->US_CR = AT91C_US_TXEN | AT91C_US_RXEN;
            AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, xSerialHdls[0].uiAT91C_ID_USX,
                                   USART_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
                                   xSerialHdls[0].pvIRQHandlerFN );
            AT91F_AIC_EnableIt( AT91C_BASE_AIC, xSerialHdls[0].uiAT91C_ID_USX );
            UART_INIT( xSerialHdls[0].uiAT91C_ID_USX );
        }
    }
    else
    {
        bOkay = FALSE;
    }

    EXIT_CRITICAL_SECTION(  );
    return bOkay;
}

void
vMBPortSerialClose( void )
{
    if( bIsInitalized )
    {
        if( NULL != xSerialHdls[0].pxCOM )
        {
            AT91F_AIC_DisableIt( AT91C_BASE_AIC, xSerialHdls[0].uiAT91C_ID_USX );
            AT91F_US_Close( xSerialHdls[0].pxCOM );
            HDL_RESET( &xSerialHdls[0] );
        }
    }
}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    ENTER_CRITICAL_SECTION(  );
    assert( NULL != xSerialHdls[0].pxCOM );
    if( xRxEnable )
    {
        AT91F_US_EnableIt( xSerialHdls[0].pxCOM, AT91C_US_RXRDY );
        xSerialHdls[0].bIsRxEnabled = TRUE;
    }
    else
    {
        AT91F_US_DisableIt( xSerialHdls[0].pxCOM, AT91C_US_RXRDY );
        xSerialHdls[0].bIsRxEnabled = FALSE;
    }

    if( xTxEnable )
    {
        AT91F_US_EnableIt( xSerialHdls[0].pxCOM, AT91C_US_TXRDY );
        xSerialHdls[0].bIsTxEnabled = TRUE;
    }
    else
    {
        AT91F_US_DisableIt( xSerialHdls[0].pxCOM, AT91C_US_TXRDY );
        xSerialHdls[0].bIsTxEnabled = FALSE;
    }
    EXIT_CRITICAL_SECTION(  );
}

STATIC INLINE void
vUSARTIRQHandler( void )
{
    unsigned int    uiUSARTStatus = xSerialHdls[0].pxCOM->US_CSR;

    if( xSerialHdls[0].bIsRxEnabled && ( uiUSARTStatus & AT91C_US_RXRDY ) )
    {
        pxMBFrameCBByteReceived(  );
    }
    if( xSerialHdls[0].bIsTxEnabled && ( uiUSARTStatus & AT91C_US_TXRDY ) )
    {
        pxMBFrameCBTransmitterEmpty(  );
    }
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    assert( NULL != xSerialHdls[0].pxCOM );
    AT91F_US_PutChar( xSerialHdls[0].pxCOM, ucByte );
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    assert( NULL != xSerialHdls[0].pxCOM );
    *pucByte = ( CHAR ) AT91F_US_GetChar( xSerialHdls[0].pxCOM );
    return TRUE;
}

void
vUSART0ISR( void )
{
    assert( NULL != xSerialHdls[0].pxCOM );
    vUSARTIRQHandler(  );
    AT91C_BASE_AIC->AIC_EOICR = 0;
}

void
vUSART1ISR( void )
{
    assert( NULL != xSerialHdls[0].pxCOM );
    vUSARTIRQHandler(  );
    AT91C_BASE_AIC->AIC_EOICR = 0;
}
