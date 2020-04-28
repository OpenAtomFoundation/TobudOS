/*
 * FreeModbus Libary: LPC214X Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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
#include <LPC214X.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void
prvvTIMERExpiredISR( void )
    __irq;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    // Timer0 Configuration
    T0PR = 0;                   // Prscaler Register = 0
    T0PC = 0;                   // Prscaler Counter = 0

    T0TC = 0;                   // Timer Counter = 0

    T0MR0 = ( PCLK / 20000 ) * usTim1Timerout50us;      // Interval of (50us * usTim1Timerout50us)
    T0MCR = 3;                  // Bit 0 = 1 - Interruption on MR0
    // Bit 1 = 1 - Reset on MR0

    T0TCR = 0;                  // Timer Counter and Prescale Counter Disabled

    // Configure Timer0 Interruption
    VICVectAddr1 = ( unsigned int )prvvTIMERExpiredISR; // Timer0 Interruption - Priority 1
    VICVectCntl1 = 0x20 | 4;
    VICIntEnable = ( 1 << 4 );  // Enable Timer0 Interruption

    return TRUE;
}


void
vMBPortTimersEnable(  )
{
    T0TCR = 0x02;               // Disable Timer and Reset Counter
    T0TCR = 0x01;               // Enable Timer
}

void
vMBPortTimersDisable(  )
{
    T0TCR = 0x02;               // Disable Timer and Reset Counter
}

static void
prvvTIMERExpiredISR( void )
    __irq
{
    ( void )pxMBPortCBTimerExpired(  );

    T0IR = 0xFF;
    VICVectAddr = 0xFF;         // Acknowledge Interrupt
}
