/*
    FreeRTOS V4.0.1 - Copyright (C) 2003-2006 Richard Barry.
    MCF5235 Port - Copyright (C) 2006 Christian Walter.

    This file is part of the FreeRTOS distribution.

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

/* ----------------------- Stamdard cludes --------------------------------- */
#include <stdlib.h>

/* ----------------------- FreeRTOS includes ------------------------------- */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* ----------------------- System includes --------------------------------- */
#include "port.h"
#include "m523xbcc.h"

/* ------------------------ Defines --------------------------------------- */
#define portVECTOR_TABLE                __VECTOR_RAM
#define portVECTOR_SYSCALL              ( 32 + portTRAP_YIELD )
#define portVECTOR_TIMER                ( 64 + 36 )

#define MCF_PIT_PRESCALER               512UL
#define MCF_PIT_TIMER_TICKS             ( FSYS_2 / MCF_PIT_PRESCALER )
#define MCF_PIT_MODULUS_REGISTER(freq)  ( MCF_PIT_TIMER_TICKS / ( freq ) - 1UL)

#define portNO_CRITICAL_NESTING         ( ( unsigned portLONG ) 0 )
#define portINITIAL_CRITICAL_NESTING    ( ( unsigned portLONG ) 10 )

#define TIMER_RESET_ASM_IMPL  \
    lea         __IPSBAR,a0;  \
    adda.l      #0x150000,a0; \
    move.w      (a0),d1;      \
    moveq       #0,d0;        \
    move.w      d1,d0;        \
    ori.l       #0x4,d0;      \
    move.w      d0,(a0)

/* ------------------------ Global variables ------------------------------ */
extern volatile void *pxCurrentTCB;

/* ------------------------ Static variables ------------------------------ */
volatile unsigned portLONG ulCriticalNesting = portINITIAL_CRITICAL_NESTING;

/* ------------------------ Static functions ------------------------------ */

/* ------------------------ Start implementation -------------------------- */

portSTACK_TYPE *
pxPortInitialiseStack( portSTACK_TYPE * pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
    /* Place the parameter on the stack in the expected location. */
    *pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
    pxTopOfStack--;

    /* Place dummy return address on stack. Tasks should never terminate so
     * we can set this to anything. */
    *pxTopOfStack = ( portSTACK_TYPE ) 0;
    pxTopOfStack--;

    /* Create a Motorola Coldfire exception stack frame. First comes the return
     * address. */
    *pxTopOfStack = ( portSTACK_TYPE ) pxCode;
    pxTopOfStack--;

    /* Format, fault-status, vector number for exception stack frame. Task
     * run in supervisor mode. */
    *pxTopOfStack = 0x40002000UL | ( portVECTOR_SYSCALL + 32 ) << 18;
    pxTopOfStack--;

    /* Set the initial critical section nesting counter to zero. This value
     * is used to restore the value of ulCriticalNesting. */
    *pxTopOfStack = 0;
    *pxTopOfStack--;

    *pxTopOfStack = ( portSTACK_TYPE ) 0xA6;    /* A6 / FP */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA5;    /* A5 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA4;    /* A4 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA3;    /* A3 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA2;    /* A2 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA1;    /* A1 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xA0;    /* A0 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD7;    /* D7 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD6;    /* D6 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD5;    /* D5 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD4;    /* D4 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD3;    /* D3 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD2;    /* D2 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD1;    /* D1 */
    pxTopOfStack--;
    *pxTopOfStack = ( portSTACK_TYPE ) 0xD0;    /* D0 */

    return pxTopOfStack;
}

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 */
asm void
prvPortYield( void )
{
    move.w #0x2700, sr;
    /* Perform the context switch.  First save the context of the current task. */
    portSAVE_CONTEXT_IMPL(  );
    /* Find the highest priority task that is ready to run. */
    jsr vTaskSwitchContext;
    /* Restore the context of the new task. */
    portRESTORE_CONTEXT_IMPL(  );
}

#if configUSE_PREEMPTION == 0
/*
 * The ISR used for the scheduler tick depends on whether the cooperative or
 * the preemptive scheduler is being used.
 */
__declspec( interrupt )
static void     prvPortPreemptiveTick( void )
{
    /* The cooperative scheduler requires a normal IRQ service routine to
     * simply increment the system tick.
     */
    vTaskIncrementTick(  );
    MCF_PIT_PCSR0 |= MCF_PIT_PCSR_PIF;
}

#else

asm void
prvPortPreemptiveTick(  )
{
    move.w #0x2700, sr;
    /* Perform the context switch.  First save the context of the current task. */
    portSAVE_CONTEXT_IMPL(  );
    TIMER_RESET_ASM_IMPL;
    jsr vTaskIncrementTick;
    /* Find the highest priority task that is ready to run. */
    jsr vTaskSwitchContext;
    /* Restore the context of the new task. */
    portRESTORE_CONTEXT_IMPL(  );
}

#endif

void
vPortEnterCritical(  )
{
    /* FIXME: We should store the old IPL here - How are we supposed to do
     * this.
     */
    ( void )portSET_IPL( portIPL_MAX );

    /* Now interrupts are disabled ulCriticalNesting can be accessed
     * directly.  Increment ulCriticalNesting to keep a count of how many times
     * portENTER_CRITICAL() has been called. */
    ulCriticalNesting++;
}

void
vPortExitCritical(  )
{
    if( ulCriticalNesting > portNO_CRITICAL_NESTING )
    {
        /* Decrement the nesting count as we are leaving a critical section. */
        ulCriticalNesting--;

        /* If the nesting level has reached zero then interrupts should be
           re-enabled. */
        if( ulCriticalNesting == portNO_CRITICAL_NESTING )
        {
            ( void )portSET_IPL( 0 );
        }
    }
}

portBASE_TYPE
xPortStartScheduler( void )
{
    extern void     ( *portVECTOR_TABLE[] ) (  );

    /* Add entry in vector table for yield system call. */
    portVECTOR_TABLE[portVECTOR_SYSCALL] = prvPortYield;
    /* Add entry in vector table for periodic timer. */
    portVECTOR_TABLE[portVECTOR_TIMER] = prvPortPreemptiveTick;

    /* Configure the timer for the system clock. */
    if( configTICK_RATE_HZ > 0 )
    {
        /* Configure prescaler */
        MCF_PIT_PCSR0 = MCF_PIT_PCSR_PRE( 0x9 ) | MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_OVW;
        /* Initialize the periodic timer interrupt. */
        MCF_PIT_PMR0 = MCF_PIT_MODULUS_REGISTER( configTICK_RATE_HZ );
        /* Configure interrupt priority and level and unmask interrupt. */
        MCF_INTC0_ICR36 = MCF_INTC0_ICRn_IL( 0x1 ) | MCF_INTC0_ICRn_IP( 0x1 );
        MCF_INTC0_IMRH &= ~( MCF_INTC0_IMRH_INT_MASK36 );
        MCF_INTC0_IMRL &= ~( MCF_INTC0_IMRL_MASKALL );
        /* Enable interrupts */
        MCF_PIT_PCSR0 |= MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_EN | MCF_PIT_PCSR_PIF;
    }

    /* Restore the context of the first task that is going to run. */
    portRESTORE_CONTEXT(  );

    /* Should not get here. */
    return pdTRUE;
}

void
vPortEndScheduler( void )
{
}
