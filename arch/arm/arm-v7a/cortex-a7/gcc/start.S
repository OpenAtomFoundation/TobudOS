.equ MODE_USR,      0x10
.equ MODE_FIQ,      0x11
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13
.equ MODE_ABT,      0x17
.equ MODE_UND,      0x1B
.equ MODE_SYS,      0x1F

.equ BIT_I,         0x80  @ when I bit is set, IRQ is disabled
.equ BIT_F,         0x40  @ when F bit is set, FIQ is disabled

.equ STACK_SIZE_USR,    0x00000100
.equ STACK_SIZE_FIQ,    0x00000100
.equ STACK_SIZE_IRQ,    0x00001000
.equ STACK_SIZE_ABT,    0x00000100
.equ STACK_SIZE_UND,    0x00000100
.equ STACK_SIZE_SYS,    0x00000800
.equ STACK_SIZE_SVC,    0x00001000

.syntax unified

.section ".text.vector", "ax"
.code 32
.align 0
.global _start
_start:
_vector:
    ldr     pc, vector_reset
    ldr     pc, vector_undefined
    ldr     pc, vector_swi
    ldr     pc, vector_prefetch_abort
    ldr     pc, vector_data_abort
    ldr     pc, vector_reserved
    ldr     pc, vector_irq
    ldr     pc, vector_fiq

.align 3

vector_reset:
    .word   arm_reset

vector_undefined:
    .word   arm_undefined

vector_swi:
    .word   arm_syscall

vector_prefetch_abort:
    .word   arm_prefetch_abort

vector_data_abort:
    .word   arm_data_abort

vector_reserved:
    .word   arm_reserved

vector_irq:
    .word   arm_irq

vector_fiq:
    .word   arm_fiq

.section ".text", "ax"
.global arm_reset
arm_reset:
.L__cache_disable:
    mrc     p15, 0, r12, c1, c0, 0  /* read SCTLR */
    bic     r12, #(1 << 12)         /* i-cache disable */
    bic     r12, #(1 << 2 | 1 << 0) /* d-cache, mmu disable */
    mcr     p15, 0, r12, c1, c0, 0  /* write SCTLR */

    /* set up the stack */
.L__stack_setup:
    cpsid   i, #MODE_IRQ    /* irq */
    ldr     sp, =__irq_stack_limit

    cpsid   i, #MODE_FIQ    /* fiq */
    ldr     sp, =__fiq_stack_limit

    cpsid   i, #MODE_ABT    /* abort */
    ldr     sp, =__abt_stack_limit

    cpsid   i, #MODE_UND    /* undefined */
    ldr     sp, =__und_stack_limit

    cpsid   i, #MODE_SYS    /* system */
    ldr     sp, =__sys_stack_limit

    cpsid   i, #MODE_SVC    /* supervisor */
    ldr     sp, =__svc_stack_limit

    /* init vector table */
.L__vector_setup:
    dsb
    isb
    ldr     r0, =_vector
    mcr     p15, 0, r0, c12, c0, #0 /* write VBAR */
    dsb
    isb

    /* clear bss */
.L__bss_clear:
    ldr     r0, =__bss_start__
    ldr     r1, =__bss_end__
    mov     r2, #0

.L__bss_loop:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     .L__bss_loop

    bl      main
    b       .

.section ".bss.prebss.exc_stk", "wa"
.bss
.align 2

__usr_stack_base:
.space STACK_SIZE_USR
__usr_stack_limit:

__fiq_stack_base:
.space STACK_SIZE_FIQ
__fiq_stack_limit:

__irq_stack_base:
.space STACK_SIZE_IRQ
__irq_stack_limit:

__abt_stack_base:
.space STACK_SIZE_ABT
__abt_stack_limit:

__und_stack_base:
.space STACK_SIZE_UND
__und_stack_limit:

__sys_stack_base:
.space STACK_SIZE_SYS
__sys_stack_limit:

__svc_stack_base:
.space STACK_SIZE_SVC
__svc_stack_limit:

.size       __usr_stack_base, . - __usr_stack_base

.end

