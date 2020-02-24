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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* ----------------------- Defines ------------------------------------------*/
#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  unsigned portLONG
#define portBASE_TYPE   portLONG

#if( configUSE_16_BIT_TICKS == 1 )
typedef unsigned portSHORT portTickType;

#define portMAX_DELAY ( portTickType ) 0xffff
#else
typedef unsigned portLONG portTickType;

#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif

/* ----------------------- Defines (Architecture) ---------------------------*/

#define portSTACK_GROWTH        ( -1 )
#define portTICK_RATE_MS        ( ( portTickType ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT      4

/* ----------------------- Global variables ---------------------------------*/
extern volatile unsigned portLONG ulCriticalNesting;

/*------------------------ Defines (Scheduler) ------------------------------*/
/*
 * portRESTORE_CONTEXT, portRESTORE_CONTEXT, portENTER_SWITCHING_ISR
 * and portEXIT_SWITCHING_ISR can only be called from ARM mode, but
 * are included here for efficiency.  An attempt to call one from
 * THUMB mode code will result in a compile time error.
 */

#define portRESTORE_CONTEXT()                                           \
do {                                                                    \
    extern void * volatile pxCurrentTCB;                                \
    /* Set the LR to the task stack. */                                 \
    asm volatile ( "LDR     R0, %0" : : "m" (pxCurrentTCB) );           \
    asm volatile ( "LDR     LR, [R0]" );                                \
                                                                        \
    /* The critical nesting depth is the first item on the stack. */    \
    /* Load it into the ulCriticalNesting variable. */                  \
    asm volatile ( "LDR     R0, =ulCriticalNesting" );                  \
    asm volatile ( "LDMFD   LR!, {R1}" );                               \
    asm volatile ( "STR     R1, [R0]" );                                \
                                                                        \
    /* Get the SPSR from the stack. */                                  \
    asm volatile ( "LDMFD   LR!, {R0}" );                               \
    asm volatile ( "MSR     SPSR, R0" );                                \
                                                                        \
    /* Restore all system mode registers for the task. */               \
    asm volatile ( "LDMFD   LR, {R0-R14}^" );                           \
    asm volatile ( "NOP" );                                             \
                                                                        \
    /* Restore the return address. */                                   \
    asm volatile ( "LDR     LR, [LR, #+60]" );                          \
                                                                        \
    /* And return - correcting the offset in the LR to obtain the */    \
    /* correct address. */                                              \
    asm volatile ( "SUBS    PC, LR, #4" );                              \
} while( 0 );

/*-----------------------------------------------------------*/

#define portSAVE_CONTEXT()                                              \
do {                                                                    \
    extern void * volatile pxCurrentTCB;                                \
    /* Push R0 as we are going to use the register. */                  \
    asm volatile ( "STMDB   SP!, {R0}" );                               \
                                                                        \
    /* Set R0 to point to the task stack pointer. */                    \
    asm volatile ( "STMDB   SP,{SP}^" );                                \
    asm volatile ( "SUB     SP, SP, #4" );                              \
    asm volatile ( "LDMIA   SP!,{R0}" );                                \
                                                                        \
    /* Push the return address onto the stack. */                       \
    asm volatile ( "STMDB   R0!, {LR}" );                               \
                                                                        \
    /* Now we have saved LR we can use it instead of R0. */             \
    asm volatile ( "MOV     LR, R0" );                                  \
                                                                        \
    /* Pop R0 so we can save it onto the system mode stack. */          \
    asm volatile ( "LDMIA   SP!, {R0}" );                               \
                                                                        \
    /* Push all the system mode registers onto the task stack. */       \
    asm volatile ( "STMDB   LR,{R0-LR}^");                              \
    asm volatile ( "SUB     LR, LR, #60" );                             \
                                                                        \
    /* Push the SPSR onto the task stack. */                            \
    asm volatile ( "MRS     R0, SPSR" );                                \
    asm volatile ( "STMDB   LR!, {R0}" );                               \
                                                                        \
    asm volatile ( "LDR     R0, =ulCriticalNesting " );                 \
    asm volatile ( "LDR     R0, [R0]" );                                \
    asm volatile ( "STMDB   LR!, {R0}" );                               \
                                                                        \
    /* Store the new top of stack for the task. */                      \
    asm volatile ( "LDR     R0, %0" : : "m" (pxCurrentTCB) );           \
    asm volatile ( "STR     LR, [R0]" );                                \
} while( 0 );


/*-----------------------------------------------------------
 * ISR entry and exit macros.  These are only required if a task switch
 * is required from the ISR.
 *----------------------------------------------------------*/


#define portENTER_SWITCHING_ISR()                                       \
    /* Save the context of the interrupted task. */                     \
    portSAVE_CONTEXT();                                                 \
                                                                        \
    /* We don't know the stack requirements for the ISR, so the frame */\
    /* pointer will be set to the top of the task stack, and the stack*/\
    /* pointer left where it is.  The IRQ stack will get used for any */\
    /* functions calls made by this ISR. */                             \
    asm volatile ( "SUB     R11, LR, #4" );                             \
    do {

#define portEXIT_SWITCHING_ISR( SwitchRequired )                        \
        /* If a switch is required then we just need to call */         \
        /* vTaskSwitchContext() as the context has already been */      \
        /* saved. */                                                    \
        if( SwitchRequired )                                            \
        {                                                               \
            vTaskSwitchContext();                                       \
        }                                                               \
    } while( 0 );                                                       \
    /* Restore the context of which ever task is now the highest */     \
    /* priority that is ready to run. */                                \
    portRESTORE_CONTEXT();

#define portYIELD()                 asm volatile ( "SWI" );

/* ----------------------- Prototypes ---------------------------------------*/

#define portDISABLE_INTERRUPTS( ) vPortDisableInterrupts()
#define portENABLE_INTERRUPTS( ) vPortEnableInterrupts()

extern void     vPortDisableInterrupts( void ) __attribute__ ( ( naked ) );
extern void     vPortEnableInterrupts( void ) __attribute__ ( ( naked ) );
extern void     vPortEnterCritical( void );
extern void     vPortExitCritical( void );

#define portENTER_CRITICAL()    vPortEnterCritical();
#define portEXIT_CRITICAL()     vPortExitCritical();

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) \
    void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) \
    void vFunction( void *pvParameters )

#endif
