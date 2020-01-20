/* Fuchsia's code is nice here, so I learn from it(fine, almost copy). thanks */

#include "exceptions.h"

    .global arm_undefined
    .global arm_syscall
    .global arm_prefetch_abort
    .global arm_data_abort
    .global arm_reserved
    .global arm_irq
    .global arm_fiq

    .extern tos_knl_irq_enter
    .extern tos_knl_irq_leave
    .extern interrupt_irq

    .syntax unified
    .text

.type arm_undefined, %function
arm_undefined:
    save
    /* r0 now holds pointer to iframe */

    bl      arm_undefined_handler

    restore


.type arm_syscall, %function
arm_syscall:
    b       .


.type arm_prefetch_abort, %function
arm_prefetch_abort:
    saveall_offset #4
    /* r0 now holds pointer to iframe */

    bl      arm_prefetch_abort_handler

    restoreall


.type arm_data_abort, %function
arm_data_abort:
    saveall_offset #8
    /* r0 now holds pointer to iframe */

    bl      arm_data_abort_handler

    restoreall


.type arm_reserved, %function
arm_reserved:
    b       .


.type arm_irq, %function
arm_irq:
    saveall_offset  #4

    /* r0 now holds pointer to iframe */
    bl      tos_knl_irq_enter

    /* call into higher level code */
    bl      interrupt_irq

    bl      tos_knl_irq_leave

    restoreall


.type arm_fiq, %function
arm_fiq:
    b       .

.end

