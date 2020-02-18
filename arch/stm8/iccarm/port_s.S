    SECTION .near_func.text:code

; Get definitions for virtual registers used by the compiler
#include "vregs.inc"

    EXTERN k_curr_task
    EXTERN k_next_task

    PUBLIC port_int_disable
    PUBLIC port_int_enable
    PUBLIC port_cpsr_save
    PUBLIC port_cpsr_restore
    PUBLIC port_sched_start
    PUBLIC port_irq_context_switch
    PUBLIC port_context_switch

port_int_disable:
    SIM
    RET


port_int_enable:
    RIM
    RET


port_cpsr_save:
    /* does not work:
     LD      A, CC
     */
    PUSH    CC
    POP     A
    SIM
    RET


port_cpsr_restore:
    /* does not work:
     LD      CC, A
     */
    PUSH    A
    POP     CC
    RET


port_sched_start:
    JP      _context_restore


port_irq_context_switch:
port_context_switch:
_context_save:
    PUSHW   Y
    PUSHW   X
    PUSH    A
    PUSH    CC

    PUSH    ?b8
    PUSH    ?b9
    PUSH    ?b10
    PUSH    ?b11
    PUSH    ?b12
    PUSH    ?b13
    PUSH    ?b14
    PUSH    ?b15

    /* k_curr_task->sp = SP */
    LDW     X, k_curr_task  /* equals to: "ldr r0, =k_curr_task; ldr r0, [r0]" */
    LDW     Y, SP
    LDW     (X), Y

_context_restore:
    ; k_curr_task = k_next_task
    LDW     X, k_next_task
    LDW     Y, #k_curr_task /* equals to: "ldr r0, =k_curr_task "*/
    LDW     (Y), X

    /* SP = k_next_task->sp */
    LDW     X, (X)
    LDW     SP, X

    POP     ?b15
    POP     ?b14
    POP     ?b13
    POP     ?b12
    POP     ?b11
    POP     ?b10
    POP     ?b9
    POP     ?b8

    POP     CC
    POP     A
    POPW    X
    POPW    Y

    RET

    END

