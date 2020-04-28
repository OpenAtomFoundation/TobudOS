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
/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#define MB_TIMER_TICKS          ( 20000L )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usTimerOCRADelta;
static USHORT   usTimerOCRBDelta;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timeout50us )
{
    BOOL            bInitialized = FALSE;
    ULONG           ulReloadValue = ( ACLK * ( ULONG )usTim1Timeout50us ) / MB_TIMER_TICKS;

    if( ulReloadValue <= 1 )
    {
        ulReloadValue = 1;
    }
    else
    {
        ulReloadValue -= 1;
    }

    if( ulReloadValue < 0xFFFE )
    {
        /* Timer A clock source is ACLK, Start disabled. */
        TACTL = TASSEL0;
        TACCR0 = ( USHORT ) ulReloadValue;
        /* Enable Timer A caputer compare interrupt. */
        TACCTL0 = CCIE;

        bInitialized = TRUE;
    }
    return bInitialized;
}

void
vMBPortTimersEnable( void )
{
    /* Reset timer counter and set compare interrupt. */
    TAR = 0;
    TACCTL0 |= CCIE;
    TACTL |= MC0;
}

void
vMBPortTimersDisable( void )
{
    TACCTL0 &= ~CCIE;
    TACTL &= ~( MC0 | MC1 );
}

#if defined (__GNUC__)
interrupt (TIMERA0_VECTOR) prvvMBTimerIRQHandler( void )
#else
void
prvvMBTimerIRQHandler( void ) __interrupt[TIMERA0_VECTOR]
#endif
{
    ( void )pxMBPortCBTimerExpired(  );
}
