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

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static eMBEventType eQueuedEvent;
static BOOL     xEventInQueue;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
