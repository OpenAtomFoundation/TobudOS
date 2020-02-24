/*
 * FreeModbus Libary: MCF5235 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
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

/* ----------------------- System includes --------------------------------- */
#include "port.h"
#include "mcf5xxx.h"
#include "mcf523x.h"

/* ----------------------- Modbus includes --------------------------------- */
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ----------------------------------------- */
#define PIT_PRESCALER 				1024UL
#define PIT_TIMER_TICKS             ( FCPU / PIT_PRESCALER )
#define PIT_MODULUS_REGISTER(t50us)	\
	( (t50us * PIT_TIMER_TICKS )/20000UL - 1UL)

/* ----------------------- Static variables -------------------------------- */
USHORT          usTimerModulus;

/* ----------------------- Start implementation ---------------------------- */

BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    usTimerModulus = PIT_MODULUS_REGISTER( usTim1Timerout50us );
    /* Configure prescaler */
    MCF_PIT_PCSR0 = MCF_PIT_PCSR_PRE( 0x9 ) | MCF_PIT_PCSR_OVW;
    /* Configure interrupt priority and level */
    MCF_INTC0_ICR36 = MCF_INTC0_ICRn_IL( 0x1 ) | MCF_INTC0_ICRn_IP( 0x2 );
    /* Unmask interrupt */
    MCF_INTC0_IMRH &= ~MCF_INTC0_IMRH_INT_MASK36;

    return TRUE;
}

inline void
vMBPortTimersEnable(  )
{
    MCF_PIT_PMR0 = usTimerModulus;
    MCF_PIT_PCSR0 |= MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_EN | MCF_PIT_PCSR_PIF;
}

inline void
vMBPortTimersDisable(  )
{
    MCF_PIT_PCSR0 |= MCF_PIT_PCSR_PIF;
    MCF_PIT_PCSR0 &= ~MCF_PIT_PCSR_PIE;
    MCF_PIT_PCSR0 &= ~MCF_PIT_PCSR_EN;
}

void
prvvMBPortTimerISR(  )
{
    ( void )pxMBPortCBTimerExpired(  );
    MCF_PIT_PCSR0 |= MCF_PIT_PCSR_PIF;
}
