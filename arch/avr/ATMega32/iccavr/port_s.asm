#include <iom32.h>

    RSEG    CODE

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
    CLI
    RET


port_int_enable:
    SEI
    RET


port_cpsr_save:
    IN      R16, SREG
    CLI
    RET


port_cpsr_restore:
    OUT     SREG, R16
    RET


port_sched_start:
    JMP     _context_restore


port_irq_context_switch:
port_context_switch:
_context_save:
    PUSH    R0

    /* save SREG */
    IN      R0, SREG
    CLI
    PUSH    R0

    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R4
    PUSH    R5
    PUSH    R6
    PUSH    R7
    PUSH    R8
    PUSH    R9
    PUSH    R10
    PUSH    R11
    PUSH    R12
    PUSH    R13
    PUSH    R14
    PUSH    R15
    PUSH    R16
    PUSH    R17
    PUSH    R18
    PUSH    R19
    PUSH    R20
    PUSH    R21
    PUSH    R22
    PUSH    R23
    PUSH    R24
    PUSH    R25
    PUSH    R26
    PUSH    R27
    PUSH    R28
    PUSH    R29
    PUSH    R30
    PUSH    R31

    /* k_curr_task->sp = SP */
    LDS     R30, k_curr_task
    LDS     R31, k_curr_task + 1

    IN      R0, SPL
    IN      R1, SPH

    ST      Z, R0
    STD     Z + 1, R1

_context_restore:
    /* k_curr_task = k_next_task */
    LDI     R28, low(k_curr_task)
    LDI     R29, high(k_curr_task)

    LDI     R30, low(k_next_task)
    LDI     R31, high(k_next_task)

    LD      R0, Z
    LDD     R1, Z + 1

    ST      Y, R0
    STD     Y + 1, R1

    /* SP = k_next_task->sp */
    LDS     R30, k_next_task
    LDS     R31, k_next_task + 1

    LD      R0, Z
    LDD     R1, Z + 1

    OUT     SPL, R0
    OUT     SPH, R1

    /* do restore */
    POP     R31
    POP     R30
    POP     R29
    POP     R28
    POP     R27
    POP     R26
    POP     R25
    POP     R24
    POP     R23
    POP     R22
    POP     R21
    POP     R20
    POP     R19
    POP     R18
    POP     R17
    POP     R16
    POP     R15
    POP     R14
    POP     R13
    POP     R12
    POP     R11
    POP     R10
    POP     R9
    POP     R8
    POP     R7
    POP     R6
    POP     R5
    POP     R4
    POP     R3
    POP     R2
    POP     R1

    POP     R0
    OUT     SREG, R0
    POP     R0

    RET

    END

