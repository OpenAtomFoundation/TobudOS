    PUBLIC  port_int_disable
    PUBLIC  port_int_enable

    PUBLIC  port_cpsr_save
    PUBLIC  port_cpsr_restore

    PUBLIC  port_sched_start
    PUBLIC  port_context_switch
    PUBLIC  port_irq_context_switch

    PUBLIC  PendSV_Handler

    EXTERN  k_curr_task
    EXTERN  k_next_task


NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED20                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU     0x00FF0000                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.


    RSEG CODE:CODE:NOROOT(2)
    THUMB

port_int_disable
    CPSID   I
    BX      LR


port_int_enable
    CPSIE   I
    BX      LR


port_cpsr_save
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR


port_cpsr_restore
    MSR     PRIMASK, R0
    BX      LR


port_sched_start
    LDR     R0, =NVIC_SYSPRI14
    LDR     R1, =NVIC_PENDSV_PRI
    STR     R1, [R0]

    MOVS    R0, #0
    MSR     PSP, R0

    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I

__unreachable
    B       __unreachable


port_context_switch
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


port_irq_context_switch
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


PendSV_Handler
    CPSID   I
    MRS     R0, PSP
    CMP     R0, #0
    BEQ     PendSVHandler_nosave

    SUBS    R0, R0, #0x20
    STMIA   R0!, {R4 - R7}
    MOV     R4, R8                
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4-R7}
    SUBS    R0, R0, #0x20

    LDR     R1, =k_curr_task
    LDR     R1, [R1]
    STR     R0, [R1]

PendSVHandler_nosave
    LDR     R0, =k_curr_task
    LDR     R1, =k_next_task
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]

    LDMIA   R0!, {R4 - R7}
    LDMIA   R0!, {R2 - R3}
    MOV     R8, R2       
    MOV     R9, R3
    LDMIA   R0!, {R2 - R3}          
    MOV     R10, R2                 
    MOV     R11, R3 
    MSR     PSP, R0

    MOV     R0, R14
    MOVS    R1, #0x04
    ORRS    R0, R1
    MOV     R14, R0

    CPSIE   I

    BX      LR


    END

