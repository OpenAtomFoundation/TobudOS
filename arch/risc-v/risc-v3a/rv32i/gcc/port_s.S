/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/
 #include "port_config.h"
.global port_int_disable
.global port_int_enable

.global port_cpsr_save
.global port_cpsr_restore
.global port_sched_start

.extern k_curr_task
.extern k_next_task


.text
.align 2
.type port_int_disable, %function
port_int_disable:
    csrci   mstatus, 0x8
    ret

.type port_int_enable, %function
port_int_enable:
    csrsi   mstatus, 0x8
    ret

.type port_cpsr_save, %function
port_cpsr_save:
    csrrci  a0, mstatus, 0x8
    ret

.type port_cpsr_restore, %function
port_cpsr_restore:
    csrw    mstatus, a0
    ret



.align 2
.type port_sched_start, %function
port_sched_start:

     /* load sp from k_curr_task->sp */
    lw      t0, k_curr_task
    /* sp = k_curr_task->sp */
    lw      sp, (t0)
    j       restore_context


.align 2
.type restore_context, %function
restore_context:
    // restore context
    lw      t0,   __reg_mepc_OFFSET(sp)
    csrw    mepc, t0
    lw      t0,   __reg_mstatus_OFFSET(sp)
    csrw    mstatus, t0

    lw x1, __reg_x1_OFFSET(sp)
    lw x3, __reg_x3_OFFSET(sp)
    lw x4, __reg_x4_OFFSET(sp)
    lw x5, __reg_x5_OFFSET(sp)
    lw x6, __reg_x6_OFFSET(sp)
    lw x7, __reg_x7_OFFSET(sp)
    lw x8, __reg_x8_OFFSET(sp)
    lw x9, __reg_x9_OFFSET(sp)
    lw x10, __reg_x10_OFFSET(sp)
    lw x11, __reg_x11_OFFSET(sp)
    lw x12, __reg_x12_OFFSET(sp)
    lw x13, __reg_x13_OFFSET(sp)
    lw x14, __reg_x14_OFFSET(sp)
    lw x15, __reg_x15_OFFSET(sp)
    lw x16, __reg_x16_OFFSET(sp)
    lw x17, __reg_x17_OFFSET(sp)
    lw x18, __reg_x18_OFFSET(sp)
    lw x19, __reg_x19_OFFSET(sp)
    lw x20, __reg_x20_OFFSET(sp)
    lw x21, __reg_x21_OFFSET(sp)
    lw x22, __reg_x22_OFFSET(sp)
    lw x23, __reg_x23_OFFSET(sp)
    lw x24, __reg_x24_OFFSET(sp)
    lw x25, __reg_x25_OFFSET(sp)
    lw x26, __reg_x26_OFFSET(sp)
    lw x27, __reg_x27_OFFSET(sp)
    lw x28, __reg_x28_OFFSET(sp)
    lw x29, __reg_x29_OFFSET(sp)
    lw x30, __reg_x30_OFFSET(sp)
    lw x31, __reg_x31_OFFSET(sp)
    addi    sp, sp, 128

#if ARCH_RISCV_FPU
	flw  f0,  __reg_f0_OFFSET(sp)
	flw  f1,  __reg_f1_OFFSET(sp)
	flw  f2,  __reg_f2_OFFSET(sp)
	flw  f3,  __reg_f3_OFFSET(sp)
	flw  f4,  __reg_f4_OFFSET(sp)
	flw  f5,  __reg_f5_OFFSET(sp)
	flw  f6,  __reg_f6_OFFSET(sp)
	flw  f7,  __reg_f7_OFFSET(sp)
	flw  f8,  __reg_f8_OFFSET(sp)
	flw  f9,  __reg_f9_OFFSET(sp)
	flw  f10, __reg_f10_OFFSET(sp)
	flw  f11, __reg_f11_OFFSET(sp)
	flw  f12, __reg_f12_OFFSET(sp)
	flw  f13, __reg_f13_OFFSET(sp)
	flw  f14, __reg_f14_OFFSET(sp)
	flw  f15, __reg_f15_OFFSET(sp)
	flw  f16, __reg_f16_OFFSET(sp)
	flw  f17, __reg_f17_OFFSET(sp)
	flw  f18, __reg_f18_OFFSET(sp)
	flw  f19, __reg_f19_OFFSET(sp)
	flw  f20, __reg_f20_OFFSET(sp)
	flw  f21, __reg_f21_OFFSET(sp)
	flw  f22, __reg_f22_OFFSET(sp)
	flw  f23, __reg_f23_OFFSET(sp)
	flw  f24, __reg_f24_OFFSET(sp)
	flw  f25, __reg_f25_OFFSET(sp)
	flw  f26, __reg_f26_OFFSET(sp)
	flw  f27, __reg_f27_OFFSET(sp)
	flw  f28, __reg_f28_OFFSET(sp)
	flw  f29, __reg_f29_OFFSET(sp)
	flw  f30, __reg_f30_OFFSET(sp)
	flw  f31, __reg_f31_OFFSET(sp)
	addi sp, sp, 128
#endif

    mret


/* just switch at Software interrupt */
.align 2
.global SW_handler
SW_handler:
#if ARCH_RISCV_FPU
	addi sp, sp, -128
	fsw  f0,  __reg_f0_OFFSET(sp)
	fsw  f1,  __reg_f1_OFFSET(sp)
	fsw  f2,  __reg_f2_OFFSET(sp)
	fsw  f3,  __reg_f3_OFFSET(sp)
	fsw  f4,  __reg_f4_OFFSET(sp)
	fsw  f5,  __reg_f5_OFFSET(sp)
	fsw  f6,  __reg_f6_OFFSET(sp)
	fsw  f7,  __reg_f7_OFFSET(sp)
	fsw  f8,  __reg_f8_OFFSET(sp)
	fsw  f9,  __reg_f9_OFFSET(sp)
	fsw  f10, __reg_f10_OFFSET(sp)
	fsw  f11, __reg_f11_OFFSET(sp)
	fsw  f12, __reg_f12_OFFSET(sp)
	fsw  f13, __reg_f13_OFFSET(sp)
	fsw  f14, __reg_f14_OFFSET(sp)
	fsw  f15, __reg_f15_OFFSET(sp)
	fsw  f16, __reg_f16_OFFSET(sp)
	fsw  f17, __reg_f17_OFFSET(sp)
	fsw  f18, __reg_f18_OFFSET(sp)
	fsw  f19, __reg_f19_OFFSET(sp)
	fsw  f20, __reg_f20_OFFSET(sp)
	fsw  f21, __reg_f21_OFFSET(sp)
	fsw  f22, __reg_f22_OFFSET(sp)
	fsw  f23, __reg_f23_OFFSET(sp)
	fsw  f24, __reg_f24_OFFSET(sp)
	fsw  f25, __reg_f25_OFFSET(sp)
	fsw  f26, __reg_f26_OFFSET(sp)
	fsw  f27, __reg_f27_OFFSET(sp)
	fsw  f28, __reg_f28_OFFSET(sp)
	fsw  f29, __reg_f29_OFFSET(sp)
	fsw  f30, __reg_f30_OFFSET(sp)
	fsw  f31, __reg_f31_OFFSET(sp)
#endif
	addi sp, sp, -128
	sw   t0, __reg_x5_OFFSET(sp)

	/* disable HPE */
    li   t0,    0x20
    csrs 0x804, t0

	csrr t0, mstatus
	sw   t0, __reg_mstatus_OFFSET(sp)
	csrr t0, mepc
	sw   t0, __reg_mepc_OFFSET(sp)

    sw 	 x1, __reg_x1_OFFSET(sp)
    sw   x3, __reg_x3_OFFSET(sp)
    sw   x4, __reg_x4_OFFSET(sp)

    sw   x6, __reg_x6_OFFSET(sp)
    sw   x7, __reg_x7_OFFSET(sp)
    sw   x8, __reg_x8_OFFSET(sp)
    sw   x9, __reg_x9_OFFSET(sp)
    sw   x10, __reg_x10_OFFSET(sp)
    sw   x11, __reg_x11_OFFSET(sp)
    sw   x12, __reg_x12_OFFSET(sp)
    sw   x13, __reg_x13_OFFSET(sp)
    sw   x14, __reg_x14_OFFSET(sp)
    sw   x15, __reg_x15_OFFSET(sp)
    sw   x16, __reg_x16_OFFSET(sp)
    sw   x17, __reg_x17_OFFSET(sp)
    sw   x18, __reg_x18_OFFSET(sp)
    sw   x19, __reg_x19_OFFSET(sp)
    sw   x20, __reg_x20_OFFSET(sp)
    sw   x21, __reg_x21_OFFSET(sp)
    sw   x22, __reg_x22_OFFSET(sp)
    sw   x23, __reg_x23_OFFSET(sp)
    sw   x24, __reg_x24_OFFSET(sp)
    sw   x25, __reg_x25_OFFSET(sp)
    sw   x26, __reg_x26_OFFSET(sp)
    sw   x27, __reg_x27_OFFSET(sp)
    sw   x28, __reg_x28_OFFSET(sp)
    sw   x29, __reg_x29_OFFSET(sp)
    sw   x30, __reg_x30_OFFSET(sp)
    sw   x31, __reg_x31_OFFSET(sp)

	/* switch to irq stk */
    mv t0, sp
    lw sp, k_irq_stk_top     	/* cpu_init中减去一个字空间 */
    sw t0, 0(sp)
    /* clear software interrupt */
	call   sw_clearpend
	/* resume sp */
	lw sp, 0(sp)

    la t0, k_curr_task         // t0 = &k_curr_task
    la t1, k_next_task         // t1 = &k_next_task
    // save sp to k_curr_task.sp
    lw t2, (t0)
    sw sp, (t2)

    # switch task
    # k_curr_task = k_next_task
    lw t1, (t1)
    sw t1, (t0)
    # load new task sp
    lw sp, (t1)

	/* new thread restore */
	lw t0, __reg_mstatus_OFFSET(sp)
	csrw mstatus, t0
	lw t0, __reg_mepc_OFFSET(sp)
	csrw mepc, t0

 	lw  x1,  __reg_x1_OFFSET(sp)
	lw  x3,  __reg_x3_OFFSET(sp)
	lw  x4,  __reg_x4_OFFSET(sp)
	lw  x5,  __reg_x5_OFFSET(sp)
	lw  x6,  __reg_x6_OFFSET(sp)
	lw  x7,  __reg_x7_OFFSET(sp)
	lw  x8,  __reg_x8_OFFSET(sp)
	lw  x9,  __reg_x9_OFFSET(sp)
	lw  x10, __reg_x10_OFFSET(sp)
	lw  x11, __reg_x11_OFFSET(sp)
	lw  x12, __reg_x12_OFFSET(sp)
	lw  x13, __reg_x13_OFFSET(sp)
	lw  x14, __reg_x14_OFFSET(sp)
	lw  x15, __reg_x15_OFFSET(sp)
	lw  x16, __reg_x16_OFFSET(sp)
	lw  x17, __reg_x17_OFFSET(sp)
	lw  x18, __reg_x18_OFFSET(sp)
	lw  x19, __reg_x19_OFFSET(sp)
	lw  x20, __reg_x20_OFFSET(sp)
	lw  x21, __reg_x21_OFFSET(sp)
	lw  x22, __reg_x22_OFFSET(sp)
	lw  x23, __reg_x23_OFFSET(sp)
	lw  x24, __reg_x24_OFFSET(sp)
	lw  x25, __reg_x25_OFFSET(sp)
	lw  x26, __reg_x26_OFFSET(sp)
	lw  x27, __reg_x27_OFFSET(sp)
	lw  x28, __reg_x28_OFFSET(sp)
	lw  x29, __reg_x29_OFFSET(sp)
	lw  x30, __reg_x30_OFFSET(sp)
	lw  x31, __reg_x31_OFFSET(sp)
	addi sp, sp, 128

#if ARCH_RISCV_FPU
	flw  f0,  __reg_f0_OFFSET(sp)
	flw  f1,  __reg_f1_OFFSET(sp)
	flw  f2,  __reg_f2_OFFSET(sp)
	flw  f3,  __reg_f3_OFFSET(sp)
	flw  f4,  __reg_f4_OFFSET(sp)
	flw  f5,  __reg_f5_OFFSET(sp)
	flw  f6,  __reg_f6_OFFSET(sp)
	flw  f7,  __reg_f7_OFFSET(sp)
	flw  f8,  __reg_f8_OFFSET(sp)
	flw  f9,  __reg_f9_OFFSET(sp)
	flw  f10, __reg_f10_OFFSET(sp)
	flw  f11, __reg_f11_OFFSET(sp)
	flw  f12, __reg_f12_OFFSET(sp)
	flw  f13, __reg_f13_OFFSET(sp)
	flw  f14, __reg_f14_OFFSET(sp)
	flw  f15, __reg_f15_OFFSET(sp)
	flw  f16, __reg_f16_OFFSET(sp)
	flw  f17, __reg_f17_OFFSET(sp)
	flw  f18, __reg_f18_OFFSET(sp)
	flw  f19, __reg_f19_OFFSET(sp)
	flw  f20, __reg_f20_OFFSET(sp)
	flw  f21, __reg_f21_OFFSET(sp)
	flw  f22, __reg_f22_OFFSET(sp)
	flw  f23, __reg_f23_OFFSET(sp)
	flw  f24, __reg_f24_OFFSET(sp)
	flw  f25, __reg_f25_OFFSET(sp)
	flw  f26, __reg_f26_OFFSET(sp)
	flw  f27, __reg_f27_OFFSET(sp)
	flw  f28, __reg_f28_OFFSET(sp)
	flw  f29, __reg_f29_OFFSET(sp)
	flw  f30, __reg_f30_OFFSET(sp)
	flw  f31, __reg_f31_OFFSET(sp)
	addi sp, sp, 128
#endif

	mret
