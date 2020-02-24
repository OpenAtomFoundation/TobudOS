/*
 * FreeModbus Libary: HCS08 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Copyright (C) 2008 Trond Melen
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

/* ----------------------- Persistent variable ------------------------------*/
static USHORT   usDelta;

/* ----------------------- Start implementation -----------------------------*/

USHORT
usMBMulDiv( USHORT a, USHORT b, USHORT c )
{
    ULONG           x;

    x = a;
    x *= b;
    x /= c;

    return ( USHORT ) x;
}


/* Initialize Timer 1 as free running, and Channel 0 as output compare (with no pin)
 */
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* We assume 4194304 Hz bus clock and let the timer run at 1/128 of this frequency, ie. 32768 Hz */
    ( void )assert( BM_BUS_CLOCK == 4194304 );
    TPM1SC = 0x0F;              // TOF=0,TOIE=0,CPWMS=0,CLKS=0:1,PS=1:1:1
    TPM1C0SC = 0x10;            // CH0F=0,CH0IE=0,MS0=0:1,ELS0=0:0;??=0:0

    /* Compute numer of timer tics corresponding to the given timeout periode */
    usDelta = usMBMulDiv( usTim1Timerout50us, 1024, 625 );

    return TRUE;
}

/* Set the output compare register value and enable timer interrupt
 */
INLINE void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    SaveStatusReg(  );
    TPM1C0V = TPM1CNT + usDelta;
    RestoreStatusReg(  );
    if( TPM1C0SC_CH0F )
        TPM1C0SC_CH0F = FALSE;
    TPM1C0SC_CH0IE = TRUE;
}

/* Disable timer interrupts
 */
INLINE void
vMBPortTimersDisable(  )
{
    TPM1C0SC_CH0IE = FALSE;
}

/* Handle the interrupt and disable further timeouts
 */
interrupt VectorNumber_Vtpm1ch0 void
prvvTIMERExpiredISR( void )
{
    TPM1C0SC_CH0IE = FALSE;
    ( void )pxMBPortCBTimerExpired(  );
}
