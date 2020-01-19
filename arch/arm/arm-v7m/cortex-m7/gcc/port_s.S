    .global port_int_disable
    .global port_int_enable

    .global port_cpsr_save
    .global port_cpsr_restore

    .global port_sched_start
    .global port_context_switch
    .global port_irq_context_switch

    .global port_clz

    .global PendSV_Handler

    .extern k_curr_task
    .extern k_next_task


.equ SCB_VTOR,              0xE000ED08
.equ NVIC_INT_CTRL,         0xE000ED04
.equ NVIC_SYSPRI14,         0xE000ED22
.equ NVIC_PENDSV_PRI,       0xFF
.equ NVIC_PENDSVSET,        0x10000000


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
    CPSID   I

    @ set pendsv priority lowest
    @ otherwise trigger pendsv in port_irq_context_switch will cause a context swich in irq
    @ that would be a disaster
    MOVW    R0, #:lower16:NVIC_SYSPRI14
    MOVT    R0, #:upper16:NVIC_SYSPRI14

    MOVW    R1, #:lower16:NVIC_PENDSV_PRI
    MOVT    R1, #:upper16:NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVW    R0, #:lower16:SCB_VTOR
    MOVT    R0, #:upper16:SCB_VTOR
    LDR     R0, [R0]
    LDR     R0, [R0]
    MSR     MSP, R0

    MOVW    R0, #:lower16:k_curr_task
    MOVT    R0, #:upper16:k_curr_task

    @ k_curr_task = k_next_task;
    MOVW    R1, #:lower16:k_next_task
    MOVT    R1, #:upper16:k_next_task
    LDR     R2, [R1]
    STR     R2, [R0]

    @ sp = k_next_task->sp
    LDR     R0, [R2]
    @ PSP = sp
    MSR     PSP, R0

    MRS     R0, CONTROL
    ORR     R0, R0, #2
    MSR     CONTROL, R0

    ISB

    @ restore r4-11 from new process stack
    LDMFD    SP!, {R4 - R11}

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ ignore EXC_RETURN the first switch
    LDMFD   SP!, {R0}
    #endif

    @ restore r0, r3
    LDMFD    SP!, {R0 - R3}
    @ load R12 and LR
    LDMFD    SP!, {R12, LR}
    @ load PC and discard xPSR
    LDMFD    SP!, {R1, R2}

    CPSIE    I
    BX       R1


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

_context_save:
    @ R0-R3, R12, LR, PC, xPSR is saved automatically here
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ is it extended frame?
    TST     LR, #0x10
    IT      EQ
    VSTMDBEQ  R0!, {S16 - S31}
    @ S0 - S16, FPSCR saved automatically here

    @ save EXC_RETURN
    STMFD   R0!, {LR}
    #endif

    @ save remaining regs r4 - 11 on process stack
    STMFD   R0!, {R4 - R11}

    @ k_curr_task->sp = PSP;
    MOVW    R5, #:lower16:k_curr_task
    MOVT    R5, #:upper16:k_curr_task
    LDR     R6, [R5]
    @ R0 is SP of process being switched out
    STR     R0, [R6]

_context_restore:
    @ k_curr_task = k_next_task;
    MOVW    R1, #:lower16:k_next_task
    MOVT    R1, #:upper16:k_next_task
    LDR     R2, [R1]
    STR     R2, [R5]

    @ R0 = k_next_task->sp
    LDR     R0, [R2]

    @ restore R4 - R11
    LDMFD   R0!, {R4 - R11}

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ restore EXC_RETURN
    LDMFD   R0!, {LR}
    @ is it extended frame?
    TST     LR, #0x10
    IT      EQ
    VLDMIAEQ    R0!, {S16 - S31}
    #endif

    @ Load PSP with new process SP
    MSR     PSP, R0

    CPSIE   I

    @ R0-R3, R12, LR, PC, xPSR restored automatically here
    @ S0 - S16, FPSCR restored automatically here if FPCA = 1
    BX      LR

.end

