    EXPORT  port_int_disable
    EXPORT  port_int_enable

    EXPORT  port_cpsr_save
    EXPORT  port_cpsr_restore

    EXPORT  port_sched_start
    EXPORT  port_context_switch
    EXPORT  port_irq_context_switch

    EXPORT  port_clz

    EXPORT  PendSV_Handler

    IMPORT  k_curr_task
    IMPORT  k_next_task

SCB_VTOR        EQU     0xE000ED08
NVIC_INT_CTRL   EQU     0xE000ED04
NVIC_SYSPRI14   EQU     0xE000ED22
NVIC_PENDSV_PRI EQU     0xFF
NVIC_PENDSVSET  EQU     0x10000000


    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8


    GLOBAL port_int_disable
port_int_disable
    CPSID   I
    BX      LR


    GLOBAL port_int_enable
port_int_enable
    CPSIE   I
    BX      LR


    GLOBAL port_cpsr_save
port_cpsr_save
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR


    GLOBAL port_cpsr_restore
port_cpsr_restore
    MSR     PRIMASK, R0
    BX      LR


    GLOBAL port_clz
port_clz
    CLZ     R0, R0
    BX      LR


    GLOBAL port_sched_start
port_sched_start
    CPSID   I

    ; set pendsv priority lowest
    ; otherwise trigger pendsv in port_irq_context_switch will cause a context switch in irq
    ; that would be a disaster
    MOV32   R0, NVIC_SYSPRI14
    MOV32   R1, NVIC_PENDSV_PRI
    STRB    R1, [R0]

    LDR     R0, =SCB_VTOR
    LDR     R0, [R0]
    LDR     R0, [R0]
    MSR     MSP, R0

    ; k_curr_task = k_next_task
    MOV32   R0, k_curr_task
    MOV32   R1, k_next_task
    LDR     R2, [R1]
    STR     R2, [R0]

    ; sp = k_next_task->sp
    LDR     R0, [R2]
    ; PSP = sp
    MSR     PSP, R0

    ; using PSP
    MRS     R0, CONTROL
    ORR     R0, R0, #2
    MSR     CONTROL, R0

    ISB

    ; restore r4-11 from new process stack
    LDMFD   SP!, {R4 - R11}

    IF {FPU} != "SoftVFP"
    ; ignore EXC_RETURN the first switch
    LDMFD   SP!, {R0}
    ENDIF

    ; restore r0, r3
    LDMFD    SP!, {R0 - R3}
    ; load R12 and LR
    LDMFD    SP!, {R12, LR}
    ; load PC and discard xPSR
    LDMFD    SP!, {R1, R2}

    CPSIE    I
    BX       R1


    GLOBAL port_context_switch
port_context_switch
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


    GLOBAL port_irq_context_switch
port_irq_context_switch
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


    GLOBAL PendSV_Handler
PendSV_Handler
    CPSID   I
    MRS     R0, PSP

_context_save
    ; R0-R3, R12, LR, PC, xPSR is saved automatically here
    IF {FPU} != "SoftVFP"
    ; is it extended frame?
    TST     LR, #0x10
    IT      EQ
    VSTMDBEQ  R0!, {S16 - S31}
    ; S0 - S16, FPSCR saved automatically here

    ; save EXC_RETURN
    STMFD   R0!, {LR}
    ENDIF

    ; save remaining regs r4-11 on process stack
    STMFD   R0!, {R4 - R11}

    ; k_curr_task->sp = PSP
    MOV32   R5, k_curr_task
    LDR     R6, [R5]
    ; R0 is SP of process being switched out
    STR     R0, [R6]

_context_restore
    ; k_curr_task = k_next_task
    MOV32   R1, k_next_task
    LDR     R2, [R1]
    STR     R2, [R5]

    ; R0 = k_next_task->sp
    LDR     R0, [R2]

    ; restore R4 - R11
    LDMFD   R0!, {R4 - R11}

    IF {FPU} != "SoftVFP"
    ; restore EXC_RETURN
    LDMFD   R0!, {LR}
    ; is it extended frame?
    TST     LR, #0x10
    IT      EQ
    VLDMIAEQ    R0!, {S16 - S31}
    ENDIF

    ; Load PSP with new process SP
    MSR     PSP, R0
    CPSIE   I
    ; R0-R3, R12, LR, PC, xPSR restored automatically here
    ; S0 - S16, FPSCR restored automatically here if FPCA = 1
    BX      LR

    ALIGN
    END

