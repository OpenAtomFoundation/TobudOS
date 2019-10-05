#include "port.h"

.global irq_entry
.global trap_entry

.extern k_curr_task
.extern k_next_task

.align 2
irq_entry:
    addi   sp,  sp, -32*REGBYTES
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

    csrr    t0,  mepc
    sw      t0,  0*REGBYTES(sp)

    csrr    t0,  mstatus
    sw      t0,  1*REGBYTES(sp)

    // save sp to k_curr_task.sp
    la      t0, k_curr_task         // t0 = &k_curr_task
    lw      t1, (t0)
    sw      sp, (t1)

    csrr    a0,  mcause
    mv      a1,  sp

    // the bumblebee mstatus register is different
    // name         bit         detail
    // INTERRUPT    31          0: exception or nmi, 1 irq
    // MINHV        30          reading irq vector table
    // MPP          29:28       == mstatus.MPP
    // MPIE         27          == mstatus.MPIE
    // Reserved     26:24       0
    // MPIL         23:16       previous interrupt level
    // Reserved     15:12       0
    // EXCCODE      11:0        exception code
    slli    a0,  a0, 20
    srli    a0,  a0, 20
    call    cpu_irq_entry

    la      t0,  k_curr_task
    la      t1,  k_next_task

    beq     t0,  t1, irq_restore

    // save sp to k_curr_task.sp
    la      t0, k_curr_task         // t0 = &k_curr_task
    lw      t2, (t0)                // t2 =  k_curr_task->sp
    sw      sp, (t2)                // k_curr_task->sp = sp

    // switch task
    lw      t1, (t1)                // t1 =  k_next_task
    sw      t1, (t0)                // k_curr_task = k_next_task

    // load new task sp
    lw      sp, (t1)

irq_restore:
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


.align 6
trap_entry:
    addi   sp,  sp, -32*REGBYTES
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

    csrr    t0,  mepc
    sw      t0,  0*REGBYTES(sp)

    csrr    t0,  mstatus
    sw      t0,  1*REGBYTES(sp)

    call cpu_trap_entry
1:
    j 1b
