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

/* ----------------------- Platform includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define HDL_RESET( x ) do { \
    ( x )->xQueueHdl = 0; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    xQueueHandle    xQueueHdl;
} xEventInternalHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC BOOL     bIsInitialized = FALSE;
STATIC xEventInternalHandle arxEventHdls[1];

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortEventInit( void )
{
    BOOL            bOkay = FALSE;
    xQueueHandle    xQueueHdl;

    ENTER_CRITICAL_SECTION(  );
    xQueueHdl = xQueueCreate( 1, sizeof( eMBEventType ) );
    if( 0 != xQueueHdl )
    {
        arxEventHdls[0].xQueueHdl = xQueueHdl;
        bIsInitialized = TRUE;
        bOkay = TRUE;
    }
    EXIT_CRITICAL_SECTION(  );

    return bOkay;
}

void
vMBPortEventClose(  )
{
    ENTER_CRITICAL_SECTION(  );
    if( bIsInitialized )
    {
        if( 0 != arxEventHdls[0].xQueueHdl )
        {
            vQueueDelete( arxEventHdls[0].xQueueHdl );
        }
        HDL_RESET( &arxEventHdls[0] );
    }
    EXIT_CRITICAL_SECTION(  );
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    portBASE_TYPE   xEventSent = pdFALSE;

    ENTER_CRITICAL_SECTION(  );
    if( bIsInitialized )
    {
        if( bMBPIsWithinException(  ) )
        {
            xEventSent =
                xQueueSendFromISR( arxEventHdls[0].xQueueHdl, ( const void * )&eEvent, pdFALSE );
        }
        else
        {
            xEventSent = xQueueSend( arxEventHdls[0].xQueueHdl, ( const void * )&eEvent, pdFALSE );
        }
    }
    EXIT_CRITICAL_SECTION(  );
    return xEventSent == pdTRUE ? TRUE : FALSE;
}

BOOL
xMBPortEventGet( eMBEventType * peEvent )
{
    BOOL            bEventInQueue = FALSE;

    ENTER_CRITICAL_SECTION(  );
    if( bIsInitialized )
    {
        if( pdTRUE == xQueueReceive( arxEventHdls[0].xQueueHdl, peEvent, portTICK_RATE_MS * 50 ) )
        {
            bEventInQueue = TRUE;
        }
    }
    EXIT_CRITICAL_SECTION(  );
    return bEventInQueue;
}
