.global port_int_disable
.global port_int_enable

.global port_cpsr_save
.global port_cpsr_restore

.global port_systick_resume
.global port_systick_suspend
.global port_systick_pending_reset

.global port_sched_start
.global port_context_switch

.extern k_curr_task
.extern k_next_task
.extern k_task_irq_switch_flag

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
    lw      t0,   0*REGBYTES(sp)
    csrw    mepc, t0

    lw      t0,   1*REGBYTES(sp)
    csrw    mstatus, t0

    lw      x1,   2*REGBYTES(sp)
    lw      x3,   3*REGBYTES(sp)
    lw      x4,   4*REGBYTES(sp)
    lw      x5,   5*REGBYTES(sp)
    lw      x6,   6*REGBYTES(sp)
    lw      x7,   7*REGBYTES(sp)
    lw      x8,   8*REGBYTES(sp)
    lw      x9,   9*REGBYTES(sp)
    lw      x10, 10*REGBYTES(sp)
    lw      x11, 11*REGBYTES(sp)
    lw      x12, 12*REGBYTES(sp)
    lw      x13, 13*REGBYTES(sp)
    lw      x14, 14*REGBYTES(sp)
    lw      x15, 15*REGBYTES(sp)
    lw      x16, 16*REGBYTES(sp)
    lw      x17, 17*REGBYTES(sp)
    lw      x18, 18*REGBYTES(sp)
    lw      x19, 19*REGBYTES(sp)
    lw      x20, 20*REGBYTES(sp)
    lw      x21, 21*REGBYTES(sp)
    lw      x22, 22*REGBYTES(sp)
    lw      x23, 23*REGBYTES(sp)
    lw      x24, 24*REGBYTES(sp)
    lw      x25, 25*REGBYTES(sp)
    lw      x26, 26*REGBYTES(sp)
    lw      x27, 27*REGBYTES(sp)
    lw      x28, 28*REGBYTES(sp)
    lw      x29, 29*REGBYTES(sp)
    lw      x30, 30*REGBYTES(sp)
    lw      x31, 31*REGBYTES(sp)

    addi    sp, sp, 32*REGBYTES

    mret


.align 2
.type port_context_switch, %function
port_context_switch:
    addi   sp, sp, -32*REGBYTES
    sw     x1,  2*REGBYTES(sp)
    sw     x3,  3*REGBYTES(sp)
    sw     x4,  4*REGBYTES(sp)
    sw     x5,  5*REGBYTES(sp)
    sw     x6,  6*REGBYTES(sp)
    sw     x7,  7*REGBYTES(sp)
    sw     x8,  8*REGBYTES(sp)
    sw     x9,  9*REGBYTES(sp)
    sw    x10, 10*REGBYTES(sp)
    sw    x11, 11*REGBYTES(sp)
    sw    x12, 12*REGBYTES(sp)
    sw    x13, 13*REGBYTES(sp)
    sw    x14, 14*REGBYTES(sp)
    sw    x15, 15*REGBYTES(sp)
    sw    x16, 16*REGBYTES(sp)
    sw    x17, 17*REGBYTES(sp)
    sw    x18, 18*REGBYTES(sp)
    sw    x19, 19*REGBYTES(sp)
    sw    x20, 20*REGBYTES(sp)
    sw    x21, 21*REGBYTES(sp)
    sw    x22, 22*REGBYTES(sp)
    sw    x23, 23*REGBYTES(sp)
    sw    x24, 24*REGBYTES(sp)
    sw    x25, 25*REGBYTES(sp)
    sw    x26, 26*REGBYTES(sp)
    sw    x27, 27*REGBYTES(sp)
    sw    x28, 28*REGBYTES(sp)
    sw    x29, 29*REGBYTES(sp)
    sw    x30, 30*REGBYTES(sp)
    sw    x31, 31*REGBYTES(sp)


    sw      ra,  0*REGBYTES(sp)

    csrr   t0,  mstatus
    li     t1,  MSTATUS_MPP
    or     t0,  t0, t1
    sw     t0,  1*REGBYTES(sp)

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
    lw      t0,   0*REGBYTES(sp)
    csrw    mepc, t0

    lw      t0,   1*REGBYTES(sp)
    csrw    mstatus, t0

    lw      x1,   2*REGBYTES(sp)
    lw      x3,   3*REGBYTES(sp)
    lw      x4,   4*REGBYTES(sp)
    lw      x5,   5*REGBYTES(sp)
    lw      x6,   6*REGBYTES(sp)
    lw      x7,   7*REGBYTES(sp)
    lw      x8,   8*REGBYTES(sp)
    lw      x9,   9*REGBYTES(sp)
    lw      x10, 10*REGBYTES(sp)
    lw      x11, 11*REGBYTES(sp)
    lw      x12, 12*REGBYTES(sp)
    lw      x13, 13*REGBYTES(sp)
    lw      x14, 14*REGBYTES(sp)
    lw      x15, 15*REGBYTES(sp)
    lw      x16, 16*REGBYTES(sp)
    lw      x17, 17*REGBYTES(sp)
    lw      x18, 18*REGBYTES(sp)
    lw      x19, 19*REGBYTES(sp)
    lw      x20, 20*REGBYTES(sp)
    lw      x21, 21*REGBYTES(sp)
    lw      x22, 22*REGBYTES(sp)
    lw      x23, 23*REGBYTES(sp)
    lw      x24, 24*REGBYTES(sp)
    lw      x25, 25*REGBYTES(sp)
    lw      x26, 26*REGBYTES(sp)
    lw      x27, 27*REGBYTES(sp)
    lw      x28, 28*REGBYTES(sp)
    lw      x29, 29*REGBYTES(sp)
    lw      x30, 30*REGBYTES(sp)
    lw      x31, 31*REGBYTES(sp)

    addi    sp, sp, 32*REGBYTES

    mret
