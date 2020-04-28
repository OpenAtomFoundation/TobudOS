/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "port.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define USART0_ENABLED          ( 1 )
#define USART0_IDX              ( 0 )

#define USART1_ENABLED          ( 1 )
#define USART1_IDX              ( USART0_IDX + USART0_ENABLED * 1 )

#define USART_IDX_LAST          ( USART1_IDX )

#define USART_INVALID_PORT      ( 0xFF )
#define USART_NOT_RE_IDX        ( 3 )
#define USART_DE_IDX            ( 4 )

/* ----------------------- Static variables ---------------------------------*/

#if USART1_ENABLED == 1
const Pin       xUSART0Pins[] = {
    PIN_USART0_TXD,
    PIN_USART0_RXD
};
#endif

#if USART1_ENABLED == 1
const Pin       xUSART1NotREPin = { 1 << 25, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };
const Pin       xUSART1DEPin = { 1 << 24, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };

const Pin       xUSART1Pins[] = {
    PIN_USART1_TXD,
    PIN_USART1_RXD,
    {1 << 23, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
};
#endif

const struct xUSARTHWMappings_t
{
    Usart          *pUsart;
    unsigned int    xUSARTID;
    IRQn_Type       xUSARTIrq;
    const Pin      *USARTNotREPin;
    const Pin      *USARTDEPin;
    const Pin      *xUSARTPins;
    uint32_t        xUSARTPinsCnt;


} xUSARTHWMappings[] =
{
#if USART0_ENABLED == 1
    {
    USART0, ID_USART0, USART0_IRQn, NULL, NULL, &xUSART0Pins[0], PIO_LISTSIZE( xUSART0Pins )},
#endif
#if USART1_ENABLED == 1
    {
    USART1, ID_USART1, USART1_IRQn, &xUSART1NotREPin, &xUSART1DEPin, &xUSART1Pins[0], PIO_LISTSIZE( xUSART1Pins )},
#endif
};

static UCHAR    ucUsedPort = USART_INVALID_PORT;

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{

    if( xRxEnable )
    {
        USART_SetReceiverEnabled( xUSARTHWMappings[ucUsedPort].pUsart, 1 );
        USART_EnableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IDR_RXRDY );
    }
    else
    {
        USART_DisableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IDR_RXRDY );
        USART_SetReceiverEnabled( xUSARTHWMappings[ucUsedPort].pUsart, 0 );
    }

    if( xTxEnable )
    {
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
        {
            PIO_Set( xUSARTHWMappings[ucUsedPort].USARTNotREPin );
        }
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
        {
            PIO_Set( xUSARTHWMappings[ucUsedPort].USARTDEPin );
        }
        USART_SetTransmitterEnabled( xUSARTHWMappings[ucUsedPort].pUsart, 1 );
        USART_EnableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IER_TXRDY );
        USART_DisableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IER_TXEMPTY );
    }
    else
    {
        USART_DisableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IDR_TXRDY );
        USART_EnableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IER_TXEMPTY );
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bStatus = FALSE;
    uint32_t        uiMode = US_MR_USART_MODE_NORMAL;
    if( ( ucPORT <= USART_IDX_LAST ) )
    {
        bStatus = TRUE;
        switch ( eParity )
        {
        case MB_PAR_NONE:
            uiMode |= US_MR_PAR_NONE | US_MR_NBSTOP_2_BIT;
            break;
        case MB_PAR_ODD:
            uiMode |= US_MR_PAR_ODD | US_MR_NBSTOP_1_BIT;
            break;
        case MB_PAR_EVEN:
            uiMode |= US_MR_PAR_EVEN | US_MR_NBSTOP_1_BIT;
            break;
        default:
            bStatus = FALSE;
            break;
        }

        switch ( ucDataBits )
        {
        case 8:
            uiMode |= US_MR_CHRL_8_BITS;
            break;
        case 7:
            uiMode |= US_MR_CHRL_7_BITS;
            break;
        default:
            bStatus = FALSE;
        }

        if( TRUE == bStatus )
        {
            ucUsedPort = ucPORT;

            NVIC_DisableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );

            PIO_Configure( xUSARTHWMappings[ucUsedPort].xUSARTPins, xUSARTHWMappings[ucUsedPort].xUSARTPinsCnt );
            if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
            {
                PIO_Configure( xUSARTHWMappings[ucUsedPort].USARTNotREPin, 1 );
            }
            if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
            {
                PIO_Configure( xUSARTHWMappings[ucUsedPort].USARTDEPin, 1 );
            }
            PMC_EnablePeripheral( xUSARTHWMappings[ucUsedPort].xUSARTID );
            USART_Configure( xUSARTHWMappings[ucUsedPort].pUsart, uiMode, ulBaudRate, BOARD_MCK );

            NVIC_ClearPendingIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );
            NVIC_SetPriority( xUSARTHWMappings[ucUsedPort].xUSARTIrq, 0xF << 4 );
            NVIC_EnableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );
        }
    }

    return bStatus;
}

void
vMBPortSerialClose( void )
{
    if( USART_INVALID_PORT != ucUsedPort )
    {
        NVIC_DisableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );
        PMC_DisablePeripheral( xUSARTHWMappings[ucUsedPort].xUSARTID );
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
        {
            PIO_Clear( xUSARTHWMappings[ucUsedPort].USARTDEPin );
        }
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
        {
            PIO_Clear( xUSARTHWMappings[ucUsedPort].USARTNotREPin );
        }
        ucUsedPort = USART_INVALID_PORT;
    }
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    USART1->US_THR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = USART1->US_RHR;
    return TRUE;
}

void
vUSARTHandler( void )
{
    uint32_t        uiCSR;
    uint32_t        uiIMR;
    BOOL            bTaskWoken = FALSE;

    vMBPortSetWithinException( TRUE );

    uiCSR = xUSARTHWMappings[ucUsedPort].pUsart->US_CSR;
    uiIMR = xUSARTHWMappings[ucUsedPort].pUsart->US_IMR;
    uint32_t        uiCSRMasked = uiCSR & uiIMR;

    if( uiCSRMasked & US_CSR_RXRDY )
    {
        bTaskWoken = pxMBFrameCBByteReceived(  );
    }
    if( uiCSRMasked & US_CSR_TXRDY )
    {
        bTaskWoken = pxMBFrameCBTransmitterEmpty(  );
    }
    if( uiCSRMasked & US_CSR_TXEMPTY )
    {
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
        {
            PIO_Clear( xUSARTHWMappings[ucUsedPort].USARTDEPin );
        }
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
        {
            PIO_Clear( xUSARTHWMappings[ucUsedPort].USARTNotREPin );
        }
        USART_DisableIt( xUSARTHWMappings[ucUsedPort].pUsart, US_IER_TXEMPTY );
    }
    vMBPortSetWithinException( FALSE );

    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
}

#if USART1_ENABLED == 1
void
USART1_IrqHandler( void )
{
    vUSARTHandler(  );
}
#endif

#if USART0_ENABLED == 1
void
USART0_IrqHandler( void )
{
    vUSARTHandler(  );
}
#endif
