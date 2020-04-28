/*
 * FreeModbus Libary: Win32 Port
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

/* ----------------------- Static variables ---------------------------------*/
DWORD           dwTimeOut;
BOOL            bTimeoutEnable;
DWORD           dwTimeLast;
extern HANDLE   g_hSerial;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    dwTimeOut = usTim1Timerout50us / 20U;
    if( dwTimeOut == 0 )
        dwTimeOut = 1;

    return xMBPortSerialSetTimeout( dwTimeOut );
}

void
xMBPortTimersClose(  )
{
    /* Does not use any hardware resources. */
}

void
vMBPortTimerPoll(  )
{

    /* Timers are called from the serial layer because we have no high
     * res timer in Win32. */
    if( bTimeoutEnable )
    {
        DWORD           dwTimeCurrent = GetTickCount(  );

        if( ( dwTimeCurrent - dwTimeLast ) > dwTimeOut )
        {
            bTimeoutEnable = FALSE;
            ( void )pxMBPortCBTimerExpired(  );
        }
    }
}

void
vMBPortTimersEnable(  )
{
    bTimeoutEnable = TRUE;
    dwTimeLast = GetTickCount(  );
}

void
vMBPortTimersDisable(  )
{
    bTimeoutEnable = FALSE;
}
