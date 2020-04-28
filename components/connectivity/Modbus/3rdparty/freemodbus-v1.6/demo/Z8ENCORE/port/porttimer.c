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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void interrupt prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* Configure Timer 0 (One Shot, Prescale = 1, Disabled) 
     *
     * Timer Control #00000000b
     *                 || ||_|_______ One Shot
     * Disabled = 0  __||_|_____________/1 Prescale                                 
     * */
    T0CTL1 = 0x00;

    T0H = 0x00;
    T0L = 0x00;

    /* Time = (reload x prescale)/CLOCK
     * reload = (Time*CLOCK)/prescale
     * 50us = CLOCK/20000
     * reload = usTim1Timerout50us*(CLOCK/20000)
     */
    T0CPH = ( usTim1Timerout50us * ( CLOCK / 20000 ) ) >> 8;
    T0CPL = ( usTim1Timerout50us * ( CLOCK / 20000 ) ) & 0x00FF;

    IRQ0ENH |= 0x20;
    IRQ0ENL |= 0x20;            /* Enable Timer0 High Priority */

    /* Set Interrupt Vector */
    SET_VECTOR( TIMER0, prvvTIMERExpiredISR );

    return TRUE;
}


void
vMBPortTimersEnable(  )
{
    T0H = 0x00;
    T0L = 0x00;

    T0CTL1 |= 0x80;
}

void
vMBPortTimersDisable(  )
{
    T0CTL1 &= ~0x80;
}

/* 
 * Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void interrupt
prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}
