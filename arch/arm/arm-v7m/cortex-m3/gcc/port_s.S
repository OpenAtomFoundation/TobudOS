    .global  port_int_disable
    .global  port_int_enable

    .global  port_cpsr_save
    .global  port_cpsr_restore

    .global  port_sched_start
    .global  port_context_switch
    .global  port_irq_context_switch

    .global  port_clz

    .global  PendSV_Handler

    .global  k_curr_task
    .global  k_next_task


.equ NVIC_INT_CTRL   ,     0xE000ED04                              @Interrupt control state register.
.equ NVIC_SYSPRI14   ,     0xE000ED22                              @System priority register (priority 14).
.equ NVIC_PENDSV_PRI ,     0x000000FF                              @ PendSV priority value (lowest).
.equ NVIC_PENDSVSET  ,     0x10000000                              @ Value to trigger PendSV exception.


   .text
   .align 2
   .thumb
   .syntax unified

.type port_int_disable, %function
port_int_disable:
    CPSID   I
    BX      LR


.type port_int_enable, %function
port_int_enable:
    CPSIE   I
    BX      LR


.type port_cpsr_save, %function
port_cpsr_save:
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR


.type port_cpsr_restore, %function
port_cpsr_restore:
    MSR     PRIMASK, R0
    BX      LR


.type port_clz, %function
port_clz:
    CLZ     R0, R0
    BX      LR


.thumb_func
.type port_sched_start, %function
port_sched_start:
    LDR     R0, =NVIC_SYSPRI14
    LDR     R1, =NVIC_PENDSV_PRI
    STR    R1, [R0]

    MOVS    R0, #0
    MSR     PSP, R0

    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I

__unreachable:
    B       __unreachable


.thumb_func
.type port_context_switch, %function
port_context_switch:
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


.thumb_func
.type port_irq_context_switch, %function
port_irq_context_switch:
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


.thumb_func
.type PendSV_Handler, %function
PendSV_Handler:
    CPSID   I
    MRS     R0, PSP
    CBZ     R0, PendSVHandler_nosave

    SUBS    R0, R0, #0x20
    STM     R0, {R4 - R11}

    LDR     R1, =k_curr_task
    LDR     R1, [R1]
    STR     R0, [R1]

PendSVHandler_nosave:
    LDR     R0, =k_curr_task
    LDR     R1, =k_next_task
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]
    LDM     R0, {R4 - R11}
    ADDS    R0, R0, #0x20
    MSR     PSP, R0
    ORR     LR, LR, #0x04
    CPSIE   I
    BX      LR
    NOP
.end

