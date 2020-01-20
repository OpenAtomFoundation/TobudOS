    .global port_int_disable
    .global port_int_enable
    .global port_cpsr_save
    .global port_cpsr_restore
    .global port_sched_start
    .global port_context_switch
    .global port_irq_context_switch

    .extern k_curr_task
    .extern k_next_task

    .syntax unified
    .align 2
    .text

.type port_int_disable, %function
port_int_disable:
    CPSID   I

.type port_int_enable, %function
port_int_enable:
    CPSIE   I

.type port_cpsr_save, %function
port_cpsr_save:
    MRS     R0, CPSR
    CPSID   I
    BX      LR

.type port_cpsr_restore, %function
port_cpsr_restore:
    MSR     CPSR, R0
    BX      LR

.type port_sched_start, %function
port_sched_start:
    B       .L__context_restore

.type port_context_switch, %function
port_context_switch:
.L__context_save:
    STR     R0, [SP, #-0xC]         /* backup R0 */

    MRS     R0, CPSR
    TST     LR, #0x01
    ORRNE   R0, R0, 0x20            /* set thumb bit*/

    STMFD   SP!, { R0 }             /* save CPSR */
    STMFD   SP!, { LR }             /* save PC */

    LDR     R0, [SP, #-0x4]         /* restore R0 */
    STMFD   SP!, { R0 - R12, LR }

    /* k_curr_task->sp = SP */
    LDR     R0, =k_curr_task
    LDR     R0, [R0]
    STR     SP, [R0]

.L__context_restore:
    /* k_curr_task = k_next_task */
    LDR     R0, =k_next_task
    LDR     R0, [R0]
    LDR     R1, =k_curr_task
    STR     R0, [R1]

    /* SP = k_next_task->sp */
    LDR     SP, [R0]

    LDMFD   SP!, { R0 - R12, LR }
    RFEIA   SP!

.type port_irq_context_switch, %function
port_irq_context_switch:
    /* we already store the k_curr_task's context onto its stack by arm_irq(see saveall_offset) */

    /* ATTENTION:
        our kernel always runs in SVC mode even in user task,
        if one day we make the user task run in USR mode(although I cannot see any meaning to do this in RTOS),
        we must deal with more logic
     */
    B       .L__context_restore     /* magnificent */

