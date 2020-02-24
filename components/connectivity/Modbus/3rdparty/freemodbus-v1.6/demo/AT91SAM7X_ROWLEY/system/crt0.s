/*****************************************************************************
 * Copyright (c) 2001, 2002 Rowley Associates Limited.                       *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/

/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 * APP_ENTRY_POINT
 *
 *   Defines the application entry point function, if undefined this setting
 *   defaults to "main".
 *
 * INITIALIZE_STACKS
 *
 *   If defined, the contents of the stacks will be initialized to a the 
 *   value 0xCC.
 *
 * INITIALIZE_SECONDARY_SECTIONS
 *
 *   If defined, the .text2, .data2 and .bss2 sections will be initialized.
 *
 * SUPERVISOR_START
 *
 *   If defined, the application will start up in supervisor mode. If 
 *   undefined the application will start up in system mode.
 *
 *****************************************************************************/

  .section .init, "ax"
  .code 32
  .align 4

#ifndef APP_ENTRY_POINT
#define APP_ENTRY_POINT main
#endif
                
  .weak _start
  .global __start
  .global __gccmain
  .extern APP_ENTRY_POINT
  .extern exit

/*****************************************************************************
 * Function    : _start                                                      *
 * Description : Main entry point and startup code for C system.             *
 *****************************************************************************/
_start:
__start:                        
  mrs r0, cpsr
  bic r0, r0, #0x1F

  /* Setup stacks */ 
  orr r1, r0, #0x1B /* Undefined mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_und_end__
  
  orr r1, r0, #0x17 /* Abort mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_abt_end__

  orr r1, r0, #0x12 /* IRQ mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_irq_end__

  orr r1, r0, #0x11 /* FIQ mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_fiq_end__

  orr r1, r0, #0x13 /* Supervisor mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_svc_end__
#ifdef SUPERVISOR_START
  /* Start application in supervisor mode */
  ldr r1, =__stack_end__ /* Setup user/system mode stack */ 
  mov r2, sp
  stmfd r2!, {r1}
  ldmfd r2, {sp}^
#else
  /* Start application in system mode */
  orr r1, r0, #0x1F /* System mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_end__
#endif

#ifdef INITIALIZE_STACKS  
  mov r2, #0xCC
  ldr r0, =__stack_und_start__
  ldr r1, =__stack_und_end__
  bl memory_set
  ldr r0, =__stack_abt_start__
  ldr r1, =__stack_abt_end__
  bl memory_set
  ldr r0, =__stack_irq_start__
  ldr r1, =__stack_irq_end__
  bl memory_set
  ldr r0, =__stack_fiq_start__
  ldr r1, =__stack_fiq_end__
  bl memory_set
  ldr r0, =__stack_svc_start__
  ldr r1, =__stack_svc_end__
  bl memory_set  
  ldr r0, =__stack_start__
  ldr r1, =__stack_end__
  bl memory_set  
#endif

  /* Copy from initialised data section to data section (if necessary). */
  ldr r0, =__data_load_start__
  ldr r1, =__data_start__
  ldr r2, =__data_end__
  bl memory_copy

  /* Copy from initialised text section to text section (if necessary). */
  ldr r0, =__text_load_start__
  ldr r1, =__text_start__
  ldr r2, =__text_end__
  bl memory_copy
  
  /* Copy from initialised fast_text section to fast_text section (if necessary). */
  ldr r0, =__fast_load_start__
  ldr r1, =__fast_start__
  ldr r2, =__fast_end__
  bl memory_copy
  
  /* Zero the bss. */
  ldr r0, =__bss_start__
  ldr r1, =__bss_end__
  mov r2, #0
  bl memory_set

#ifdef INITIALIZE_SECONDARY_SECTIONS
  ldr r0, =__data2_load_start__
  ldr r1, =__data2_start__
  ldr r2, =__data2_end__
  bl memory_copy

  /* Copy from initialised text2 section to text2 section (if necessary). */
  ldr r0, =__text2_load_start__
  ldr r1, =__text2_start__
  ldr r2, =__text2_end__
  bl memory_copy

  /* Zero bss2. */
  ldr r0, =__bss2_start__
  ldr r1, =__bss2_end__
  mov r2, #0
  bl memory_set

#endif /* #ifdef INITIALIZE_SECONDARY_SECTIONS */

  /* Initialise the heap */
  ldr r0, = __heap_start__
  ldr r1, = __heap_end__
  sub r1, r1, r0     /* r1 = r1-r0 */ 
  mov r2, #0
  str r2, [r0], #+4 /* *r0++ = 0 */
  str r1, [r0]      /* *r0 = __heap_end__ - __heap_start__ */

  /* Call constructors */
  ldr r0, =__ctors_start__
  ldr r1, =__ctors_end__
ctor_loop:
  cmp r0, r1
  beq ctor_end
  ldr r2, [r0], #+4
  stmfd sp!, {r0-r1}
  mov lr, pc
  mov pc, r2
  ldmfd sp!, {r0-r1}
  b ctor_loop
ctor_end:

  /* Setup initial call frame */
  mov lr, #4
  mov r12, sp
  stmfd sp!, {r11-r12, lr-pc}
  sub r11, r12, #0x00000004

start:
  /* Jump to application entry point */
  mov r0, #0
  mov r1, #0
  ldr r2, =APP_ENTRY_POINT
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif

  /* Call destructors */
  ldr r0, =__dtors_start__
  ldr r1, =__dtors_end__
dtor_loop:
  cmp r0, r1
  beq dtor_end
  ldr r2, [r0], #+4
  stmfd sp!, {r0-r1}
  mov lr, pc
  mov pc, r2
  ldmfd sp!, {r0-r1}
  b dtor_loop
dtor_end:

  /* Returned from application entry point, loop forever. */
exit_loop:
  b exit_loop

memory_copy:
  cmp r0, r1
  moveq pc, lr  
  subs r2, r2, r1
  moveq pc, lr  
1:
  ldrb r3, [r0], #+1
  strb r3, [r1], #+1
  subs r2, r2, #1
  bne 1b
  mov pc, lr

memory_set:
  cmp r0, r1
  moveq pc, lr
  strb r2, [r0], #1
  b memory_set

