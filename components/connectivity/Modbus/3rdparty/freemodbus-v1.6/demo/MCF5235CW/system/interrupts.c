/*
    FreeRTOS MCF5235 port - Copyright (C) 2006 Christian Walter.
    Copyright @ 1995-2005 Freescale Semiconductor, Inc. All rights reserved

    FreeRTOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FreeRTOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeRTOS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes FreeRTOS, without being obliged to provide
    the source code for any proprietary components.  See the licensing section
    of http://www.FreeRTOS.org for full details of how and when the exception
    can be applied.

    ***************************************************************************
    See http://www.FreeRTOS.org for documentation, latest information, license
    and contact details.  Please ensure to read the configuration and relevant
    port sections of the online documentation.
    ***************************************************************************
*/

#include "m523xbcc.h"

void
exception_handler( void *framep )
{
    ( void )framep;
}

__interrupt__ void
irq_handler( void )
{
    /* 
     * This is the catch all interrupt handler for all user defined
     * interrupts.  To create specific handlers, create a new interrupt
     * handler and change vectors.s to point to the new handler.
     */
}
