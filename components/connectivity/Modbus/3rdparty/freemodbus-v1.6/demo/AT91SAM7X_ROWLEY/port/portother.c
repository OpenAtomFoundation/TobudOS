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

#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- Platform includes --------------------------------*/
#include "AT91SAM7X256.h"
#include "lib_AT91SAM7X256.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Function prototypes ------------------------------*/
void            vMBPortSerialClose( void );
void            vMBPortEventClose( void );
void            vMBPortTimerClose( void );

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Static functions ---------------------------------*/
STATIC void     vDefaultHandler( void ) __attribute__ ( ( interrupt( "IRQ" ) ) );
STATIC void     vDefaultSpuriousHandler( void ) __attribute__ ( ( interrupt( "IRQ" ) ) );

/* ----------------------- Start implementation -----------------------------*/

void
vMBPInit( void )
{
    AT91F_AIC_Open( AT91C_BASE_AIC, AT91C_AIC_BRANCH_OPCODE, AT91C_AIC_BRANCH_OPCODE,
                    vDefaultHandler, vDefaultSpuriousHandler, AT91C_AIC_DCR_PROT );
}

void
__assert( const char *pcFile, const char *pcAssertion, int iLine )
{
    volatile BOOL   bBreakOut = FALSE;

    ( void )pcFile;
    ( void )pcAssertion;
    ( void )iLine;
    ENTER_CRITICAL_SECTION(  );
    while( !bBreakOut );
}

BOOL
bMBPIsWithinException( void )
{
    BOOL            bMBPIsWithinException = TRUE;
    unsigned int    uiCPSR;
    asm volatile    ( "MRS  %0, CPSR":"=r" ( uiCPSR ): );

    switch ( uiCPSR & 0x000000001F )
    {
    case 0x00000010U:          /* User Mode */
    case 0x0000001FU:          /* System Mode */
        bMBPIsWithinException = FALSE;
        break;
    }
    return bMBPIsWithinException;
}

void
vMBPPortEnterCritical( void )
{
    if( !bMBPIsWithinException(  ) )
    {
        portENTER_CRITICAL(  );
    }
}

void
vMBPPortExitCritical( void )
{
    if( !bMBPIsWithinException(  ) )
    {
        portEXIT_CRITICAL(  );
    }
}

void
vMBPortClose( void )
{
    vMBPortSerialClose(  );
    vMBPortEventClose(  );
    vMBPortTimerClose(  );
}

void
vDefaultSpuriousHandler( void )
{
}

void
vDefaultHandler( void )
{
}
