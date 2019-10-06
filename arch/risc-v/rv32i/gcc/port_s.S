#include "riscv_port.h"

.global port_int_disable
.global port_int_enable

.global port_cpsr_save
.global port_cpsr_restore

.global port_systick_resume
.global port_systick_suspend
.global port_systick_pending_reset

.global port_sched_start
.global port_context_switch
.global rv32_exception_entry

.extern k_curr_task
.extern k_next_task

.equ    MSTATUS_MIE,        0x00000008
.equ    MSTATUS_MPP,        0x00001800

.equ    MIE_MTIE,           (1 << 7)        // machine mode interrupt enable

.equ    MIP_MTIP,           (1 << 7)        // machine mode timer interrupt pending

.equ    REGBYTES,           4

.text
.align 2

.type port_int_disable, %function
port_int_disable:
    csrci   mstatus, MSTATUS_MIE
    ret

.type port_int_enable, %function
port_int_enable:
    csrsi   mstatus, MSTATUS_MIE
    ret

.type port_cpsr_save, %function
port_cpsr_save:
    csrrci  a0, mstatus, MSTATUS_MIE
    ret

.type port_cpsr_restore, %function
port_cpsr_restore:
    csrw    mstatus, a0
    ret

.type port_systick_resume, %function
port_systick_resume:
    li t0, MIE_MTIE
    csrs mie, t0
    ret

.type port_systick_suspend, %function
port_systick_suspend:
    li t0, MIE_MTIE
    csrc mie, t0
    ret

.type port_systick_pending_reset, %function
port_systick_pending_reset:
    li t0, MIP_MTIP
    csrc mip, t0
    ret

.align 2
.type port_sched_start, %function
port_sched_start:
    // enable timer interrupt
    li      t0, MIE_MTIE
    csrs    mie, t0

    // load sp from k_curr_task->sp
    la      t0, k_curr_task         // t0 = &k_curr_task
    lw      t0, (t0)                // t0 = &(k_curr_task->sp)
    lw      sp, (t0)                // sp = k_curr_task->sp

    // save sp to stack
    addi    t1, sp, 32*REGBYTES
    sw      t1, (t0)

    // restore context
    lw      t0, 30*REGBYTES(sp)
    csrw    mepc, t0

    lw      t0, 31*REGBYTES(sp)
    csrw    mstatus, t0

    lw s0, 0*REGBYTES(sp)
    lw s1, 1*REGBYTES(sp)
    lw s2, 2*REGBYTES(sp)
    lw s3, 3*REGBYTES(sp)
    lw s4, 4*REGBYTES(sp)
    lw s5, 5*REGBYTES(sp)
    lw s6, 6*REGBYTES(sp)
    lw s7, 7*REGBYTES(sp)
    lw s8, 8*REGBYTES(sp)
    lw s9, 9*REGBYTES(sp)
    lw s10, 10*REGBYTES(sp)
    lw s11, 11*REGBYTES(sp)

    // caller save
    lw ra, 12*REGBYTES(sp)
    lw gp, 13*REGBYTES(sp)
    lw tp, 14*REGBYTES(sp)
    lw t0, 15*REGBYTES(sp)
    lw t1, 16*REGBYTES(sp)
    lw t2, 17*REGBYTES(sp)
    lw t3, 18*REGBYTES(sp)
    lw t4, 19*REGBYTES(sp)
    lw t5, 20*REGBYTES(sp)
    lw t6, 21*REGBYTES(sp)
    lw a0, 22*REGBYTES(sp)
    lw a1, 23*REGBYTES(sp)
    lw a2, 24*REGBYTES(sp)
    lw a3, 25*REGBYTES(sp)
    lw a4, 26*REGBYTES(sp)
    lw a5, 27*REGBYTES(sp)
    lw a6, 28*REGBYTES(sp)
    lw a7, 29*REGBYTES(sp)


    addi    sp, sp, 32*REGBYTES

    mret


.align 2
.type port_context_switch, %function
port_context_switch:
    addi   sp, sp, -32*REGBYTES

    sw s0, 0*REGBYTES(sp)
    sw s1, 1*REGBYTES(sp)
    sw s2, 2*REGBYTES(sp)
    sw s3, 3*REGBYTES(sp)
    sw s4, 4*REGBYTES(sp)
    sw s5, 5*REGBYTES(sp)
    sw s6, 6*REGBYTES(sp)
    sw s7, 7*REGBYTES(sp)
    sw s8, 8*REGBYTES(sp)
    sw s9, 9*REGBYTES(sp)
    sw s10, 10*REGBYTES(sp)
    sw s11, 11*REGBYTES(sp)

    // caller save
    sw ra, 12*REGBYTES(sp)
    sw gp, 13*REGBYTES(sp)
    sw tp, 14*REGBYTES(sp)
    sw t0, 15*REGBYTES(sp)
    sw t1, 16*REGBYTES(sp)
    sw t2, 17*REGBYTES(sp)
    sw t3, 18*REGBYTES(sp)
    sw t4, 19*REGBYTES(sp)
    sw t5, 20*REGBYTES(sp)
    sw t6, 21*REGBYTES(sp)
    sw a0, 22*REGBYTES(sp)
    sw a1, 23*REGBYTES(sp)
    sw a2, 24*REGBYTES(sp)
    sw a3, 25*REGBYTES(sp)
    sw a4, 26*REGBYTES(sp)
    sw a5, 27*REGBYTES(sp)
    sw a6, 28*REGBYTES(sp)
    sw a7, 29*REGBYTES(sp)

    sw     ra,  30*REGBYTES(sp)

    csrr   t0,  mstatus
    li     t1,  MSTATUS_MPP
    or     t0,  t0, t1
    sw     t0,  31*REGBYTES(sp)

    // save sp to k_curr_task.sp
    la      t0, k_curr_task         // t0 = &k_curr_task
    lw      t1, (t0)
    sw      sp, (t1)

    // switch task
    // k_curr_task = k_next_task
    la      t1, k_next_task         // t1 = &k_next_task
    lw      t1, (t1)                // t1 = k_next_task
    sw      t1, (t0)

    // load new task sp
    lw      sp, (t1)

    // restore context
    lw      t0,   30*REGBYTES(sp)
    csrw    mepc, t0

    lw      t0,   31*REGBYTES(sp)
    csrw    mstatus, t0


    lw s0, 0*REGBYTES(sp)
    lw s1, 1*REGBYTES(sp)
    lw s2, 2*REGBYTES(sp)
    lw s3, 3*REGBYTES(sp)
    lw s4, 4*REGBYTES(sp)
    lw s5, 5*REGBYTES(sp)
    lw s6, 6*REGBYTES(sp)
    lw s7, 7*REGBYTES(sp)
    lw s8, 8*REGBYTES(sp)
    lw s9, 9*REGBYTES(sp)
    lw s10, 10*REGBYTES(sp)
    lw s11, 11*REGBYTES(sp)

    // caller save
    lw ra, 12*REGBYTES(sp)
    lw gp, 13*REGBYTES(sp)
    lw tp, 14*REGBYTES(sp)
    lw t0, 15*REGBYTES(sp)
    lw t1, 16*REGBYTES(sp)
    lw t2, 17*REGBYTES(sp)
    lw t3, 18*REGBYTES(sp)
    lw t4, 19*REGBYTES(sp)
    lw t5, 20*REGBYTES(sp)
    lw t6, 21*REGBYTES(sp)
    lw a0, 22*REGBYTES(sp)
    lw a1, 23*REGBYTES(sp)
    lw a2, 24*REGBYTES(sp)
    lw a3, 25*REGBYTES(sp)
    lw a4, 26*REGBYTES(sp)
    lw a5, 27*REGBYTES(sp)
    lw a6, 28*REGBYTES(sp)
    lw a7, 29*REGBYTES(sp)
    addi    sp, sp, 32*REGBYTES

    mret


.align 2
.global rv32_exception_entry
rv32_exception_entry:
    addi   sp,  sp, -20*REGBYTES
    sw ra, (12-12)*REGBYTES(sp)
    sw gp, (13-12)*REGBYTES(sp)
    sw tp, (14-12)*REGBYTES(sp)
    sw t0, (15-12)*REGBYTES(sp)
    sw t1, (16-12)*REGBYTES(sp)
    sw t2, (17-12)*REGBYTES(sp)
    sw t3, (18-12)*REGBYTES(sp)
    sw t4, (19-12)*REGBYTES(sp)
    sw t5, (20-12)*REGBYTES(sp)
    sw t6, (21-12)*REGBYTES(sp)
    sw a0, (22-12)*REGBYTES(sp)
    sw a1, (23-12)*REGBYTES(sp)
    sw a2, (24-12)*REGBYTES(sp)
    sw a3, (25-12)*REGBYTES(sp)
    sw a4, (26-12)*REGBYTES(sp)
    sw a5, (27-12)*REGBYTES(sp)
    sw a6, (28-12)*REGBYTES(sp)
    sw a7, (29-12)*REGBYTES(sp)

    csrr    t0,  mepc
    sw      t0,  (30-12)*REGBYTES(sp)

    csrr    t0,  mstatus
    sw      t0,  (31-12)*REGBYTES(sp)

    // get irq num and call irq handler
    li      t0,  SOC_MCAUSE_EXP_MASK
    csrr    a0,  mcause
    and     a0,  a0, t0
    call    cpu_irq_entry

    la      t0,  k_curr_task
    la      t1,  k_next_task

    beq     t0,  t1, irq_restore
    addi   sp,  sp, -12*REGBYTES
    sw s0, 0*REGBYTES(sp)
    sw s1, 1*REGBYTES(sp)
    sw s2, 2*REGBYTES(sp)
    sw s3, 3*REGBYTES(sp)
    sw s4, 4*REGBYTES(sp)
    sw s5, 5*REGBYTES(sp)
    sw s6, 6*REGBYTES(sp)
    sw s7, 7*REGBYTES(sp)
    sw s8, 8*REGBYTES(sp)
    sw s9, 9*REGBYTES(sp)
    sw s10, 10*REGBYTES(sp)
    sw s11, 11*REGBYTES(sp)

    // save sp to k_curr_task.sp
    la      t0, k_curr_task         // t0 = &k_curr_task
    lw      t2, (t0)                // t2 =  k_curr_task->sp
    sw      sp, (t2)                // k_curr_task->sp = sp

    // switch task
    lw      t1, (t1)                // t1 =  k_next_task
    sw      t1, (t0)                // k_curr_task = k_next_task

    // load new task sp
    lw      sp, (t1)

    lw s0, 0*REGBYTES(sp)
    lw s1, 1*REGBYTES(sp)
    lw s2, 2*REGBYTES(sp)
    lw s3, 3*REGBYTES(sp)
    lw s4, 4*REGBYTES(sp)
    lw s5, 5*REGBYTES(sp)
    lw s6, 6*REGBYTES(sp)
    lw s7, 7*REGBYTES(sp)
    lw s8, 8*REGBYTES(sp)
    lw s9, 9*REGBYTES(sp)
    lw s10, 10*REGBYTES(sp)
    lw s11, 11*REGBYTES(sp)
    addi    sp, sp, 12*REGBYTES

irq_restore:
    // restore context
    lw      t0,   (30-12)*REGBYTES(sp)
    csrw    mepc, t0

    lw      t0,   (31-12)*REGBYTES(sp)
    csrw    mstatus, t0

    lw ra, (12-12)*REGBYTES(sp)
    lw gp, (13-12)*REGBYTES(sp)
    lw tp, (14-12)*REGBYTES(sp)
    lw t0, (15-12)*REGBYTES(sp)
    lw t1, (16-12)*REGBYTES(sp)
    lw t2, (17-12)*REGBYTES(sp)
    lw t3, (18-12)*REGBYTES(sp)
    lw t4, (19-12)*REGBYTES(sp)
    lw t5, (20-12)*REGBYTES(sp)
    lw t6, (21-12)*REGBYTES(sp)
    lw a0, (22-12)*REGBYTES(sp)
    lw a1, (23-12)*REGBYTES(sp)
    lw a2, (24-12)*REGBYTES(sp)
    lw a3, (25-12)*REGBYTES(sp)
    lw a4, (26-12)*REGBYTES(sp)
    lw a5, (27-12)*REGBYTES(sp)
    lw a6, (28-12)*REGBYTES(sp)
    lw a7, (29-12)*REGBYTES(sp)
    addi    sp, sp, 20*REGBYTES

    mret


