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
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static xQueueHandle xQueueHdl;


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    BOOL            bStatus = FALSE;
    if( 0 != ( xQueueHdl = xQueueCreate( 1, sizeof( eMBEventType ) ) ) )
    {
        bStatus = TRUE;
    }
    return bStatus;
}

void
vMBPortEventClose( void )
{
    if( 0 != xQueueHdl )
    {
        vQueueDelete( xQueueHdl );
        xQueueHdl = 0;
    }
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    BOOL            bStatus = TRUE;
    if( bMBPortIsWithinException(  ) )
    {
        ( void )xQueueSendFromISR( xQueueHdl, ( const void * )&eEvent, pdFALSE );
    }
    else
    {
        ( void )xQueueSend( xQueueHdl, ( const void * )&eEvent, pdFALSE );
    }

    return bStatus;
}

BOOL
xMBPortEventGet( eMBEventType * peEvent )
{
    BOOL            xEventHappened = FALSE;

    if( pdTRUE == xQueueReceive( xQueueHdl, peEvent, portTICK_RATE_MS * 50 ) )
    {
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
