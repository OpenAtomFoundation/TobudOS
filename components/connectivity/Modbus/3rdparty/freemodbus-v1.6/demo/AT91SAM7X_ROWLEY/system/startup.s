/*****************************************************************************
  Exception handlers and startup code for Atmel AT91SAM7.

  Copyright (c) 2004 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software. 
 
  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************************/

/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 *
 * VECTORED_IRQ_INTERRUPTS
 *
 *   Enable vectored IRQ interrupts. If defined, the PC register will be loaded
 *   with the contents of the AIC_IVR register on an IRQ exception.
 *
 * STARTUP_FROM_RESET
 *
 *   If defined, the program will startup from power-on/reset. If not defined
 *   the program will just loop endlessly from power-on/reset.
 *
 *   This definition is not defined by default on this target because the
 *   debugger is unable to reset this target and maintain control of it over the
 *   JTAG interface. The advantage of doing this is that it allows the debugger
 *   to reset the CPU and run programs from a known reset CPU state on each run.
 *   It also acts as a safety net if you accidently download a program in FLASH
 *   that crashes and prevents the debugger from taking control over JTAG
 *   rendering the target unusable over JTAG. The obvious disadvantage of doing
 *   this is that your application will not startup without the debugger.
 *
 *   We advise that on this target you keep STARTUP_FROM_RESET undefined whilst
 *   you are developing and only define STARTUP_FROM_RESET when development is
 *   complete.
 *
 * MC_FMR_FMCN_VAL
 *
 *   Override the default Flash Mode Register FMCN value by defining 
 *   MC_FMR_FMCN_VAL.
 *
 * MC_FMR_FWS_VAL
 *
 *   Override the default Flash Mode Register FWS value by defining 
 *   MC_FMR_FWS_VAL.
 *
 * CKGR_MOR_VAL
 *
 *   Override the default Main Oscillator Register value by defining 
 *   CKGR_MOR_VAL.
 *
 * CKGR_PLLR_VAL
 *
 *   Override the default Clock Generator PLL Register value by defining
 *   CKGR_PLLR_VAL.
 *
 * NO_WATCHDOG_DISABLE
 *
 *   If defined, the watchdog will not be disabled.
 *
 * NO_USER_RESET
 *
 *   If defined, user reset will not be enabled.
 *
 * NO_PROTECTION_MODE
 *   
 *   If defined, AIC protection mode will not be enabled. AIC protection
 *   mode allows the AIC_IVR register to be accessed by the debugger (for
 *   example through the memory or register window). With protection mode
 *   disabled accesses of the AIC_IVR register by the debugger are likely
 *   to disrupt interrupt behaviour.
 *
 *****************************************************************************/

#include <targets/AT91SAM7.h>

#ifndef MC_FMR_FMCN_VAL
#define MC_FMR_FMCN_VAL 72
#endif

#ifndef MC_FMR_FWS_VAL
#define MC_FMR_FWS_VAL 1
#endif

#ifndef CKGR_MOR_VAL
#define CKGR_MOR_VAL (0x40 << CKGR_MOR_OSCOUNT_BIT) | CKGR_MOR_MOSCEN
#endif

#ifndef CKGR_PLLR_VAL
#if OSCILLATOR_CLOCK_FREQUENCY==18432000
/* MCK, UDPCK = 18.432MHz * 73 / 14 / 2 = 48.05486MHz */  
#define CKGR_PLLR_VAL (1 << CKGR_PLLR_USBDIV_BIT) | (72 << CKGR_PLLR_MUL_BIT) | (0 << CKGR_PLLR_OUT_BIT) | (63 << CKGR_PLLR_PLLCOUNT_BIT) | (14 << CKGR_PLLR_DIV_BIT)
#else
#error OSCILLATOR_CLOCK_FREQUENCY not supported 
#endif
#endif

  .section .vectors, "ax"
  .code 32
  .align 0
  .global _vectors
  .global reset_handler

/*****************************************************************************
  Exception Vectors
 *****************************************************************************/
_vectors:
#ifdef STARTUP_FROM_RESET
  ldr pc, [pc, #reset_handler_address - . - 8]  /* reset */
#else
  b .                                           /* reset - infinite loop */
#endif
  ldr pc, [pc, #undef_handler_address - . - 8]  /* undefined instruction */
  ldr pc, [pc, #swi_handler_address - . - 8]    /* swi handler */
  ldr pc, [pc, #pabort_handler_address - . - 8] /* abort prefetch */
  ldr pc, [pc, #dabort_handler_address - . - 8] /* abort data */
  nop
  ldr pc, [pc, #-0xF20]                         /* irq */
  ldr pc, [pc, #fiq_handler_address - . - 8]    /* fiq */

reset_handler_address:
  .word reset_handler
undef_handler_address:
  .word undef_handler
swi_handler_address:
  .word swi_handler
pabort_handler_address:
  .word pabort_handler
dabort_handler_address:
  .word dabort_handler
irq_handler_address:
  .word irq_handler
fiq_handler_address:
  .word fiq_handler

  .section .init, "ax"
  .code 32
  .align 0

/******************************************************************************
  Reset handler
 ******************************************************************************/
reset_handler:
  /* Set up FLASH wait state */
  ldr r1, =MC_BASE
  ldr r0, =(MC_FMR_FMCN_VAL << MC_FMR_FMCN_BIT) | (MC_FMR_FWS_VAL << MC_FMR_FWS_BIT)
  str r0, [r1, #MC_FMR_OFFSET]

  /* Disable Watchdog */
  ldr r1, =WDT_BASE
  ldr r0, =WDT_MR_WDDIS
  str r0, [r1, #WDT_MR_OFFSET]
  
#ifndef NO_PROTECTION_MODE
  /* Enable protect mode */
  ldr r1, =AIC_BASE
  ldr r0, =0x00000001
  str r0, [r1, #AIC_DCR_OFFSET]
#endif

  /* Enable the main oscillator */
  ldr r1, =PMC_BASE
  ldr r0, =CKGR_MOR_VAL
  str r0, [r1, #CKGR_MOR_OFFSET]
  
1:/* Wait for main oscillator to stabilize */
  ldr r0, [r1, #PMC_SR_OFFSET]
  tst r0, #PMC_SR_MOSCS
  beq 1b

  /* Set up the PLL */
  ldr r0, =CKGR_PLLR_VAL
  str r0, [r1, #CKGR_PLLR_OFFSET]
  
1:/* Wait for PLL to lock */
  ldr r0, [r1, #PMC_SR_OFFSET]
  tst r0, #PMC_SR_LOCK
  beq 1b

  /* Set the master clock prescaler */
  ldr r0, =(1 << PMC_MCKR_PRES_BIT)
  str r0, [r1, #PMC_MCKR_OFFSET]

1:/* Wait for MCKRDY */
  ldr r0, [r1, #PMC_SR_OFFSET]
  tst r0, #PMC_SR_MCKRDY
  beq 1b

  /* Select PLL as clock source */
  ldr r0, =((1 << PMC_MCKR_PRES_BIT) | 3 << PMC_MCKR_CSS_BIT)
  str r0, [r1, #PMC_MCKR_OFFSET]

#ifdef __FLASH_BUILD
  /* Copy exception vectors into Internal SRAM */
  mov r8, #0x00200000
  ldr r9, =_vectors
  ldmia r9!, {r0-r7}
  stmia r8!, {r0-r7}
  ldmia r9!, {r0-r6}
  stmia r8!, {r0-r6}

  /* Remap Internal SRAM to 0x00000000 */
  ldr r1, =MC_BASE
  ldr r0, =1
  strb r0, [r1, #MC_RCR_OFFSET]
#endif

  /* Jump to the default C runtime startup code. */
  b _start

/******************************************************************************
  Default exception handlers
  (These are declared weak symbols so they can be redefined in user code)
 ******************************************************************************/
undef_handler:
  b .  /* Endless loop */
  
swi_handler:
  b .  /* Endless loop */
  
pabort_handler:
  b .  /* Endless loop */
  
dabort_handler:
  b .  /* Endless loop */
  
irq_handler:
  b .  /* Endless loop */
  
fiq_handler:
  b .  /* Endless loop */

  .weak undef_handler, swi_handler, pabort_handler, dabort_handler, irq_handler, fiq_handler
