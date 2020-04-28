/*
 * STR71X/GCC Startup Scripts for FreeModbus

 * Copyright C) 2005 Anglia Design, Spencer Oliver
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: startup.S,v 1.1 2006/11/02 23:14:44 wolti Exp $
 */

/* ----------------------- Target settings ----------------------------------*/
    .equ    FOSC,           4000000
    .equ    FRTC,           32768

/* ----------------------- ARM7 CPU modes -----------------------------------*/
    .equ    MODE_USR,       0x10
    .equ    MODE_FIQ,       0x11
    .equ    MODE_IRQ,       0x12
    .equ    MODE_SVC,       0x13
    .equ    MODE_ABT,       0x17
    .equ    MODE_UND,       0x1B
    .equ    MODE_SYS,       0x1F          /* available on ARM Arch 4 and later */

    .equ    I_Bit,          0x80          /* when I bit is set, IRQ is disabled */
    .equ    F_Bit,          0x40          /* when F bit is set, FIQ is disabled */

/* ----------------------- System memory locations --------------------------*/
    .equ    EIC_ADDR,       0xFFFFF800    /* EIC base address */
    .equ    EIC_ICF_OFF,    0x00          /* Interrupt Control register offset */
    .equ    EIC_CIPR_OFF,   0x08          /* Current Interrupt Priority Register offset */
    .equ    EIC_IVR_OFF,    0x18          /* Interrupt Vector Register offset */
    .equ    EIC_FIR_OFF,    0x1C          /* Fast Interrupt Register offset */
    .equ    EIC_IER_OFF,    0x20          /* Interrupt Enable Register offset */
    .equ    EIC_IPR_OFF,    0x40          /* Interrupt Pending Bit Register offset */
    .equ    EIC_SIR0_OFF,   0x60          /* Source Interrupt Register 0 */

    .equ    CPM_ADDR,       0xA0000040    /* CPM Base Address */
    .equ    CPM_BC_OFF,     0x10          /* CPM - Boot Configuration Register */
    .equ    CPM_BC_FLASH,   0x0000        /* to remap FLASH at 0x0 */
    .equ    CPM_BC_RAM,     0x0002        /* to remap RAM at 0x0 */
    .equ    CPM_BC_EXTMEM,  0x0003        /* to remap EXTMEM at 0x0 */

/* ----------------------- Startup code -------------------------------------*/
    .text
    .arm
    .section .init, "ax"
    
    .global _start
    .global RCCU_Main_Osc
    .global RCCU_RTC_Osc

/* ----------------------- Exception vectors ( ROM mode with remap ) --------*/
.if ROM_RUN == 1
_vector_reset_rom:
    ldr   pc, =_start_rom
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    /* Copy the final vectors from ROM into RAM and map RAM at address 
     * 0x00000000 */
_start_rom:
    ldr   r1, =_vecstart                  /* r1 = start address from which to copy */
    ldr   r3, =_vecend
    sub   r3, r3, r1                      /* r3 = number of bytes to copy */
    ldr   r0, =_vectext                   /* r0 = start address where to copy */
copy_ram:
    ldr   r2, [r0], #4                    /* Read a word from the source */
    str   r2, [r1], #4                    /* copy the word to destination */
    subs  r3, r3, #4                      /* Decrement number of words to copy */
    bne   copy_ram
        
    ldr   r1, =CPM_ADDR
    ldrh  r2, [r1, #CPM_BC_OFF]           /* Read BOOTCONF Register */
    bic   r2, r2, #0x03                   /* Reset the two LSB bits of BOOTCONF Register */
    orr   r2, r2, #CPM_BC_RAM             /* change the two LSB bits of BOOTCONF Register */
    strh  r2, [r1, #CPM_BC_OFF]           /* Write BOOTCONF Register */
.endif

/* ----------------------- Default reset handler (After remap ) -------------*/
_start:
    ldr   pc, =NextInst
NextInst:
    nop                                   /* Wait for OSC stabilization */
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    /* Enter Undefined Instruction Mode and set its Stack Pointer */
    msr   cpsr_c, #MODE_UND|I_Bit|F_Bit
    ldr   sp, =__stack_und_end__

    /* Enter Abort Mode and set its Stack Pointer */
    msr   cpsr_c, #MODE_ABT|I_Bit|F_Bit
    ldr   sp, =__stack_abt_end__

    /* Enter IRQ Mode and set its Stack Pointer */
    msr   cpsr_c, #MODE_IRQ|I_Bit|F_Bit
    ldr   sp, =__stack_irq_end__

    /* Enter FIQ Mode and set its Stack Pointer */
    msr   cpsr_c, #MODE_FIQ|I_Bit|F_Bit
    ldr   sp, =__stack_fiq_end__

    /* Enter Supervisor Mode and set its Stack Pointer */
    msr   cpsr_c, #MODE_SVC|I_Bit|F_Bit
    ldr   sp, =__stack_svc_end__

    /* Set User Mode Stack pointer but remain in Supervisor Mode */
    ldr   r1, =__stack_end__
    mov   r2, sp
    stmfd r2!, {r1}
    ldmfd r2, {sp}^

    /* Setup a default Stack Limit (when compiled with "-mapcs-stack-check") */
    ldr   sl, =__bss_end__

/* ----------------------- EIC initialization -------------------------------
/*
 * EIC is initialized with:
 *  - IRQ disabled
 *  - FIQ disabled
 *  - IVR contain the load PC opcode (0xF59FF00)
 *  - Current priority level equal to 0
 *  - All channels are disabled
 *  - All channels priority equal to 0
 *  - All SIR registers contain offset to the related IRQ table entry
 */

eic_init:
    ldr   r3, =EIC_ADDR
    ldr   r4, =0x00000000         
    str   r4, [r3, #EIC_ICF_OFF]          /* Disable FIQ and IRQ */
    str   r4, [r3, #EIC_IER_OFF]          /* Disable all channels interrupts */
    ldr   r4, =0xFFFFFFFF
    str   r4, [r3, #EIC_IPR_OFF]          /* Clear all IRQ pending bits */
    ldr   r4, =0x0C
    str   r4, [r3, #EIC_FIR_OFF]          /* Disable FIQ channels and clear FIQ pending bits */
    ldr   r4, =0x00000000
    str   r4, [r3, #EIC_CIPR_OFF]         /* Reset the current priority register */
    ldr   r4, =0xE59F0000
    str   r4, [r3, #EIC_IVR_OFF]          /* Write the LDR pc,pc,#offset instruction code in IVR[31:16] */
    ldr   r2, =32                         /* 32 Channel to initialize */
    ldr   r0, =T0TIMI_Addr                /* Read the address of the IRQs address table */
    ldr   r1, =0x00000FFF
    and   r0, r0, r1
    ldr   r5, =EIC_SIR0_OFF               /* Read SIR0 address */
    sub   r4, r0, #8                      /* subtract 8 for prefetch */
    ldr   r1, =0xF7E8                     /* add the offset to the 0x00000000 address(IVR address + 7E8 = 0x00000000) */
                                          /* 0xF7E8 used to complete the LDR pc,pc,#offset opcode */
    add   r1, r4, r1                      /* compute the jump offset */
eic_ini:
    mov   r4, r1, LSL #16                 /* Left shift the result */
    str   r4, [r3, r5]                    /* Store the result in SIRx register */
    add   r1, r1, #4                      /* Next IRQ address */
    add   r5, r5, #4                      /* Next SIR */
    subs  r2, r2, #1                      /* Decrement the number of SIR registers to initialize */
    bne   eic_ini                         /* If more then continue */

    /* Relocate .data section (Copy from ROM to RAM) */
.if ROM_RUN == 1
    ldr   r1, =_etext
    ldr   r2, =_data
    ldr   r3, =_edata
_loop_relocate:
    cmp   r2, r3
    ldrlo r0, [r1], #4
    strlo r0, [r2], #4
    blo   _loop_relocate
.endif

    /* Clear .bss section (Zero init) */
    mov   r0, #0
    ldr   r1, =__bss_start__
    ldr   r2, =__bss_end__
_loop_clear_bss:
    cmp   r1, r2
    strlo r0, [r1], #4
    blo   _loop_clear_bss
        
    /* Call C++ constructors */
    ldr   r0, =__ctors_start__
    ldr   r1, =__ctors_end__
ctor_loop:
    cmp   r0, r1
    beq   ctor_end
    ldr   r2, [r0], #4
    stmfd sp!, {r0-r1}
    mov   lr, pc
    mov   pc, r2
    ldmfd sp!, {r0-r1}
    b     ctor_loop
ctor_end:

    /* Need to set up standard file handles */

    /* if we use debug version of str7lib this will call the init function */
    bl    libdebug
libdebug:       

    /* Call main */
    bl    main

    /* Call destructors */
    ldr   r0, =__dtors_start__
    ldr   r1, =__dtors_end__
dtor_loop:
    cmp   r0, r1
    beq   dtor_end
    ldr   r2, [r0], #4
    stmfd sp!, {r0-r1}
    mov   lr, pc
    mov   pc, r2
    ldmfd sp!, {r0-r1}
    b     dtor_loop
dtor_end:

/* Return from main, loop forever. */
exit_loop:
    b     exit_loop
    
/* Fosc values, used by libstr7 */

RCCU_Main_Osc:  .long   FOSC
RCCU_RTC_Osc:   .long   FRTC

    .weak libdebug
    
    .end
