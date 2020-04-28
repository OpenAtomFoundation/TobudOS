/*
    FreeRTOS V4.1.1 - Copyright (C) 2003-2005 Richard Barry.
    Modification for STR71X/GCC -  Copyright (C) 2006 Christian Walter.

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

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- System includes ----------------------------------*/
#include "eic.h"
#include "wdg.h"

/* ----------------------- Defines ------------------------------------------*/
#define portNO_CRITICAL_NESTING     \
  ( ( unsigned portLONG ) 0 )

/* ----------------------- Prototypes ---------------------------------------*/

/* ISR to handle manual context switches (from a call to taskYIELD()). */
void            vPortYieldProcessor( void ) __attribute__ ( ( interrupt( "SWI" ), naked ) );

/* The scheduler can only be started from ARM mode, hence the inclusion of this
 * function here.
 */
void            vPortISRStartFirstTask( void );

/* ----------------------- Global variables ---------------------------------*/
extern const portSHORT portTimerOCRDeltaValue;
volatile unsigned portLONG ulCriticalNesting = 9999UL;

/* ----------------------- Start implementation -----------------------------*/

void
vPortISRStartFirstTask( void )
{
    /* Simply start the scheduler.  This is included here as it can only be
     * called from ARM mode. 
     */
    portRESTORE_CONTEXT(  );
}

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 *
 * When a context switch is performed from the task level the saved task 
 * context is made to look as if it occurred from within the tick ISR.  This
 * way the same restore context function can be used when restoring the context
 * saved from the ISR or that saved from a call to vPortYieldProcessor.
 */
void
vPortYieldProcessor( void )
{
    /* Within an IRQ ISR the link register has an offset from the true return 
       address, but an SWI ISR does not.  Add the offset manually so the same 
       ISR return code can be used in both cases. */
    asm volatile    ( "ADD     LR, LR, #4" );

    /* Perform the context switch.  First save the context of the current task. */
    portSAVE_CONTEXT(  );

    /* Find the highest priority task that is ready to run. */
    vTaskSwitchContext(  );

    /* Restore the context of the new task. */
    portRESTORE_CONTEXT(  );
}

/* 
 * The ISR used for the scheduler tick depends on whether the cooperative or
 * the preemptive scheduler is being used.
 */
#if configUSE_PREEMPTION == 0

/* The cooperative scheduler requires a normal IRQ service routine to 
 * simply increment the system tick.
 */
void            vPortNonPreemptiveTick( void ) __attribute__ ( ( interrupt( "IRQ" ) ) );
void
vPortNonPreemptiveTick( void )
{
    vTaskIncrementTick(  );

    /* Clear the interrupt in the watchdog and EIC. */
    WDG->SR = 0x0000;
    /* clear current interrupt pending flag for interupt source. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );
}

#else

/* The preemptive scheduler is defined as "naked" as the full context is
 * saved on entry as part of the context switch.
 */
void            vPortPreemptiveTick( void ) __attribute__ ( ( naked ) );
void
vPortPreemptiveTick( void )
{
    /* Save the context of the interrupted task. */
    portSAVE_CONTEXT(  );

    /* Increment the RTOS tick count, then look for the highest priority 
     * task that is ready to run. 
     */
    vTaskIncrementTick(  );
    vTaskSwitchContext(  );

    /* Clear the interrupt in the watchdog and EIC. */
    WDG->SR = 0x0000;
    /* clear current interrupt pending flag for interupt source. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );

    /* Restore the context of the new task. */
    portRESTORE_CONTEXT(  );
}

#endif


void
vPortDisableInterrupts( void )
{
    asm volatile    ( "STMDB   SP!, {R0}" );    /* Push R0. */
    asm volatile    ( "MRS     R0, CPSR" );     /* Get CPSR.  */
    asm volatile    ( "ORR     R0, R0, #0xC0" );        /* Disable IRQ, FIQ. */
    asm volatile    ( "MSR     CPSR, R0" );     /* Write back modified value. */
    asm volatile    ( "LDMIA   SP!, {R0}" );    /* Pop R0. */
    asm volatile    ( "BX      R14" );          /* Return back to thumb. */
}

void
vPortEnableInterrupts( void )
{
    asm volatile    ( "STMDB   SP!, {R0}" );    /* Push R0. */
    asm volatile    ( "MRS     R0, CPSR" );     /* Get CPSR. */
    asm volatile    ( "BIC     R0, R0, #0xC0" );        /* Enable IRQ, FIQ. */
    asm volatile    ( "MSR     CPSR, R0" );     /* Write back modified value. */
    asm volatile    ( "LDMIA   SP!, {R0}" );    /* Pop R0. */
    asm volatile    ( "BX      R14" );          /* Return back to thumb. */
}

/* The code generated by the GCC compiler uses the stack in different ways at
 * different optimisation levels.  The interrupt flags can therefore not always
 * be saved to the stack.  Instead the critical section nesting level is stored
 * in a variable, which is then saved as part of the stack context. 
 */
void
vPortEnterCritical( void )
{
    /* Disable interrupts as per portDISABLE_INTERRUPTS();                              */
    asm volatile    ( "STMDB   SP!, {R0}" );    /* Push R0. */
    asm volatile    ( "MRS     R0, CPSR" );     /* Get CPSR. */
    asm volatile    ( "ORR     R0, R0, #0xC0" );        /* Disable IRQ, FIQ. */
    asm volatile    ( "MSR     CPSR, R0" );     /* Write back modified value. */
    asm volatile    ( "LDMIA   SP!, {R0}" );    /* Pop R0. */

    /* Now interrupts are disabled ulCriticalNesting can be accessed 
     * directly.  Increment ulCriticalNesting to keep a count of how many times
     * portENTER_CRITICAL() has been called. 
     */
    ulCriticalNesting++;
}

void
vPortExitCritical( void )
{
    if( ulCriticalNesting > portNO_CRITICAL_NESTING )
    {
        /* Decrement the nesting count as we are leaving a critical section. */
        ulCriticalNesting--;

        /* If the nesting level has reached zero then interrupts should be
         * re-enabled. 
         */
        if( ulCriticalNesting == portNO_CRITICAL_NESTING )
        {
            /* Enable interrupts as per portEXIT_CRITICAL(). */
            asm volatile    ( "STMDB   SP!, {R0}" );    /* Push R0. */
            asm volatile    ( "MRS     R0, CPSR" );     /* Get CPSR. */
            asm volatile    ( "BIC     R0, R0, #0xC0" );        /* Enable IRQ, FIQ. */
            asm volatile    ( "MSR     CPSR, R0" );     /* Write back modified value. */
            asm volatile    ( "LDMIA   SP!, {R0}" );    /* Pop R0. */
        }
    }
}
