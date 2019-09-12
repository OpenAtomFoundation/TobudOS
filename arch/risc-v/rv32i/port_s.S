.global port_int_disable
.global port_int_enable

.global port_cpsr_save
.global port_cpsr_restore

.global port_sched_start
.global port_context_switch
.global port_irq_context_switch

#include "riscv_encoding.h"

.text
.align 2

.type port_int_disable, %function
port_int_disable:
	csrci	mstatus, MSTATUS_MIE
	ret

.type port_int_enable, %function
port_int_enable:
	csrsi	mstatus, MSTATUS_MIE
	ret

.type port_cpsr_save, %function
port_cpsr_save:
	csrrci	a0, mstatus, MSTATUS_MIE
	#csrrci  a0, mstatus, MSTATUS_MPIE
	ret

.type port_cpsr_restore, %function
port_cpsr_restore:
	csrw	mstatus, a0
	ret


#define REGBYTES	4
#define LOAD		lw
#define STORE		sw
.extern k_curr_task
.type port_sched_start, %function
port_sched_start:
	// enable timer interrupt
	li	    t0, MIE_MTIE
	csrs    mie, t0

	la 		t0, k_curr_task		// t0 = &k_curr_task
	LOAD 	t0, (t0)			// t0 = &(k_curr_task->sp)
	LOAD	sp, (t0)			// k_curr_task->sp = sp

	// save sp
    addi  	t1, sp, 32*REGBYTES
	STORE	t1, (t0)

	LOAD	t0,   0*REGBYTES(sp)
	csrw  	mepc, t0

	LOAD  	t0,   1*REGBYTES(sp)
	csrw  	mstatus, t0

	LOAD	x1,	  2*REGBYTES(sp)
	LOAD	x3,   3*REGBYTES(sp)
	LOAD	x4,   4*REGBYTES(sp)
	LOAD	x5,   5*REGBYTES(sp)
	LOAD	x6,   6*REGBYTES(sp)
	LOAD	x7,   7*REGBYTES(sp)
	LOAD	x8,   8*REGBYTES(sp)
	LOAD	x9,   9*REGBYTES(sp)
	LOAD	x10, 10*REGBYTES(sp)
	LOAD	x11, 11*REGBYTES(sp)
	LOAD	x12, 12*REGBYTES(sp)
	LOAD	x13, 13*REGBYTES(sp)
	LOAD	x14, 14*REGBYTES(sp)
	LOAD	x15, 15*REGBYTES(sp)
	LOAD	x16, 16*REGBYTES(sp)
	LOAD	x17, 17*REGBYTES(sp)
	LOAD	x18, 18*REGBYTES(sp)
	LOAD	x19, 19*REGBYTES(sp)
	LOAD	x20, 20*REGBYTES(sp)
	LOAD	x21, 21*REGBYTES(sp)
	LOAD	x22, 22*REGBYTES(sp)
	LOAD	x23, 23*REGBYTES(sp)
	LOAD	x24, 24*REGBYTES(sp)
	LOAD	x25, 25*REGBYTES(sp)
	LOAD	x26, 26*REGBYTES(sp)
	LOAD	x27, 27*REGBYTES(sp)
	LOAD	x28, 28*REGBYTES(sp)
	LOAD	x29, 29*REGBYTES(sp)
	LOAD	x30, 30*REGBYTES(sp)
	LOAD	x31, 31*REGBYTES(sp)

    addi  	sp, sp, 32*REGBYTES

    mret

.align 2
.global machine_trap_entry
machine_trap_entry:
  	addi   sp, sp, -32*REGBYTES
  	STORE  x1,  2*REGBYTES(sp)
	STORE  x3,  3*REGBYTES(sp)
	STORE  x4,  4*REGBYTES(sp)
	STORE  x5,  5*REGBYTES(sp)
	STORE  x6,  6*REGBYTES(sp)
	STORE  x7,  7*REGBYTES(sp)
	STORE  x8,  8*REGBYTES(sp)
	STORE  x9,  9*REGBYTES(sp)
	STORE x10, 10*REGBYTES(sp)
	STORE x11, 11*REGBYTES(sp)
	STORE x12, 12*REGBYTES(sp)
	STORE x13, 13*REGBYTES(sp)
	STORE x14, 14*REGBYTES(sp)
	STORE x15, 15*REGBYTES(sp)
  	STORE x16, 16*REGBYTES(sp)
	STORE x17, 17*REGBYTES(sp)
	STORE x18, 18*REGBYTES(sp)
	STORE x19, 19*REGBYTES(sp)
	STORE x20, 20*REGBYTES(sp)
	STORE x21, 21*REGBYTES(sp)
	STORE x22, 22*REGBYTES(sp)
	STORE x23, 23*REGBYTES(sp)
	STORE x24, 24*REGBYTES(sp)
	STORE x25, 25*REGBYTES(sp)
	STORE x26, 26*REGBYTES(sp)
	STORE x27, 27*REGBYTES(sp)
	STORE x28, 28*REGBYTES(sp)
	STORE x29, 29*REGBYTES(sp)
	STORE x30, 30*REGBYTES(sp)
	STORE x31, 31*REGBYTES(sp)

#if 1
	//li t0, MSTATUS_MPP | MSTATUS_MPIE	// acutally MPIE is not need
	csrr   t0,  mstatus
	STORE  t0,  1*REGBYTES(sp)

	csrr   t0,  mepc					// just save information for handler
	STORE  t0,  0*REGBYTES(sp)


	// save sp to k_curr_task.sp
	la 		t0, k_curr_task		// t0 = &k_curr_task
	LOAD 	t1, (t0)
	STORE	sp, (t1)

	csrr   a0,  mcause
	mv 	   a1,  sp
	bltz   a0,  irq
	call   cpu_trap_entry
	j      restore
irq:
	slli   a0, a0, 1
	srli   a0, a0, 1
	call   cpu_irq_entry
restore:
	LOAD	t0,   0*REGBYTES(sp)
	csrw  	mepc, t0

	LOAD  	t0,   1*REGBYTES(sp)
	csrw  	mstatus, t0
#else
	// for debug timer interrupt only
	csrr   a0,  mcause
	slli   a0, a0, 1
	srli   a0, a0, 1
	call   cpu_irq_entry
#endif
	LOAD   x1,  2*REGBYTES(sp)
	LOAD   x3,  3*REGBYTES(sp)
	LOAD   x4,  4*REGBYTES(sp)
	LOAD   x5,  5*REGBYTES(sp)
	LOAD   x6,  6*REGBYTES(sp)
	LOAD   x7,  7*REGBYTES(sp)
	LOAD   x8,  8*REGBYTES(sp)
	LOAD   x9,  9*REGBYTES(sp)
	LOAD  x10, 10*REGBYTES(sp)
	LOAD  x11, 11*REGBYTES(sp)
	LOAD  x12, 12*REGBYTES(sp)
	LOAD  x13, 13*REGBYTES(sp)
	LOAD  x14, 14*REGBYTES(sp)
	LOAD  x15, 15*REGBYTES(sp)
	LOAD  x16, 16*REGBYTES(sp)
	LOAD  x17, 17*REGBYTES(sp)
	LOAD  x18, 18*REGBYTES(sp)
	LOAD  x19, 19*REGBYTES(sp)
	LOAD  x20, 20*REGBYTES(sp)
	LOAD  x21, 21*REGBYTES(sp)
	LOAD  x22, 22*REGBYTES(sp)
	LOAD  x23, 23*REGBYTES(sp)
	LOAD  x24, 24*REGBYTES(sp)
	LOAD  x25, 25*REGBYTES(sp)
	LOAD  x26, 26*REGBYTES(sp)
	LOAD  x27, 27*REGBYTES(sp)
	LOAD  x28, 28*REGBYTES(sp)
	LOAD  x29, 29*REGBYTES(sp)
	LOAD  x30, 30*REGBYTES(sp)
	LOAD  x31, 31*REGBYTES(sp)

	addi   sp, sp, 32*REGBYTES

	mret


.align 2
.type port_context_switch, %function
port_context_switch:
	nop
	nop
  	addi   sp, sp, -32*REGBYTES
  	STORE  x1,  2*REGBYTES(sp)
	STORE  x3,  3*REGBYTES(sp)
	STORE  x4,  4*REGBYTES(sp)
	STORE  x5,  5*REGBYTES(sp)
	STORE  x6,  6*REGBYTES(sp)
	STORE  x7,  7*REGBYTES(sp)
	STORE  x8,  8*REGBYTES(sp)
	STORE  x9,  9*REGBYTES(sp)
	STORE x10, 10*REGBYTES(sp)
	STORE x11, 11*REGBYTES(sp)
	STORE x12, 12*REGBYTES(sp)
	STORE x13, 13*REGBYTES(sp)
	STORE x14, 14*REGBYTES(sp)
	STORE x15, 15*REGBYTES(sp)
  	STORE x16, 16*REGBYTES(sp)
	STORE x17, 17*REGBYTES(sp)
	STORE x18, 18*REGBYTES(sp)
	STORE x19, 19*REGBYTES(sp)
	STORE x20, 20*REGBYTES(sp)
	STORE x21, 21*REGBYTES(sp)
	STORE x22, 22*REGBYTES(sp)
	STORE x23, 23*REGBYTES(sp)
	STORE x24, 24*REGBYTES(sp)
	STORE x25, 25*REGBYTES(sp)
	STORE x26, 26*REGBYTES(sp)
	STORE x27, 27*REGBYTES(sp)
	STORE x28, 28*REGBYTES(sp)
	STORE x29, 29*REGBYTES(sp)
	STORE x30, 30*REGBYTES(sp)
	STORE x31, 31*REGBYTES(sp)

	li	   t0,  MSTATUS_MPP				// force use machine mode
	csrr   t1,  mstatus
	and    t1,  t1, MSTATUS_MIE
	beqz   t1,  save_mstatus
	or	   t0,  t0, MSTATUS_MPIE
save_mstatus:
	STORE  t0,  1*REGBYTES(sp)
	la     t0,  port_context_switch_return	// just save information for handler
	STORE  t0,  0*REGBYTES(sp)

	// save sp to k_curr_task.sp
	la 		t0, k_curr_task		// t0 = &k_curr_task
	LOAD 	t1, (t0)
	STORE	sp, (t1)

	// k_curr_task = k_next_task
	la		t1, k_next_task		// t1 = &k_next_task
	LOAD	t1, (t1)			// t1 = k_next_task
	STORE	t1, (t0)

	// load sp from k_next_task.sp
	LOAD	sp, (t1)

	LOAD   t0,  0*REGBYTES(sp)
	csrw   mepc, t0

	LOAD   t0,  1*REGBYTES(sp)
	csrw   mstatus, t0

	LOAD   x1,  2*REGBYTES(sp)
	LOAD   x3,  3*REGBYTES(sp)
	LOAD   x4,  4*REGBYTES(sp)
	LOAD   x5,  5*REGBYTES(sp)
	LOAD   x6,  6*REGBYTES(sp)
	LOAD   x7,  7*REGBYTES(sp)
	LOAD   x8,  8*REGBYTES(sp)
	LOAD   x9,  9*REGBYTES(sp)
	LOAD  x10, 10*REGBYTES(sp)
	LOAD  x11, 11*REGBYTES(sp)
	LOAD  x12, 12*REGBYTES(sp)
	LOAD  x13, 13*REGBYTES(sp)
	LOAD  x14, 14*REGBYTES(sp)
	LOAD  x15, 15*REGBYTES(sp)
	LOAD  x16, 16*REGBYTES(sp)
	LOAD  x17, 17*REGBYTES(sp)
	LOAD  x18, 18*REGBYTES(sp)
	LOAD  x19, 19*REGBYTES(sp)
	LOAD  x20, 20*REGBYTES(sp)
	LOAD  x21, 21*REGBYTES(sp)
	LOAD  x22, 22*REGBYTES(sp)
	LOAD  x23, 23*REGBYTES(sp)
	LOAD  x24, 24*REGBYTES(sp)
	LOAD  x25, 25*REGBYTES(sp)
	LOAD  x26, 26*REGBYTES(sp)
	LOAD  x27, 27*REGBYTES(sp)
	LOAD  x28, 28*REGBYTES(sp)
	LOAD  x29, 29*REGBYTES(sp)
	LOAD  x30, 30*REGBYTES(sp)
	LOAD  x31, 31*REGBYTES(sp)

	addi   sp, sp, 32*REGBYTES

	mret

.align 2
port_context_switch_return:
	ret


.align 2
.type port_irq_context_switch, %function
port_irq_context_switch:
  	addi   sp, sp, -32*REGBYTES
  	STORE  x1,  2*REGBYTES(sp)
	STORE  x3,  3*REGBYTES(sp)
	STORE  x4,  4*REGBYTES(sp)
	STORE  x5,  5*REGBYTES(sp)
	STORE  x6,  6*REGBYTES(sp)
	STORE  x7,  7*REGBYTES(sp)
	STORE  x8,  8*REGBYTES(sp)
	STORE  x9,  9*REGBYTES(sp)
	STORE x10, 10*REGBYTES(sp)
	STORE x11, 11*REGBYTES(sp)
	STORE x12, 12*REGBYTES(sp)
	STORE x13, 13*REGBYTES(sp)
	STORE x14, 14*REGBYTES(sp)
	STORE x15, 15*REGBYTES(sp)
  	STORE x16, 16*REGBYTES(sp)
	STORE x17, 17*REGBYTES(sp)
	STORE x18, 18*REGBYTES(sp)
	STORE x19, 19*REGBYTES(sp)
	STORE x20, 20*REGBYTES(sp)
	STORE x21, 21*REGBYTES(sp)
	STORE x22, 22*REGBYTES(sp)
	STORE x23, 23*REGBYTES(sp)
	STORE x24, 24*REGBYTES(sp)
	STORE x25, 25*REGBYTES(sp)
	STORE x26, 26*REGBYTES(sp)
	STORE x27, 27*REGBYTES(sp)
	STORE x28, 28*REGBYTES(sp)
	STORE x29, 29*REGBYTES(sp)
	STORE x30, 30*REGBYTES(sp)
	STORE x31, 31*REGBYTES(sp)

	li	   t0,  MSTATUS_MPP
	STORE  t0,  1*REGBYTES(sp)
	la	   t0,  irq_context_return
	STORE  t0,  0*REGBYTES(sp)

	// save sp to k_curr_task.sp
	la 		t0, k_curr_task		// t0 = &k_curr_task
	LOAD 	t1, (t0)
	STORE	sp, (t1)

	// k_curr_task = k_next_task
	la		t1, k_next_task		// t1 = &k_next_task
	LOAD	t1, (t1)			// t1 = k_next_task
	STORE	t1, (t0)

	// load sp from k_next_task.sp
	LOAD	sp, (t1)

	LOAD   t0,  1*REGBYTES(sp)
	csrw   mstatus, t0

	LOAD   t0,  0*REGBYTES(sp)
	csrw   mepc, t0

	LOAD   x1,  2*REGBYTES(sp)
	LOAD   x3,  3*REGBYTES(sp)
	LOAD   x4,  4*REGBYTES(sp)
	LOAD   x5,  5*REGBYTES(sp)
	LOAD   x6,  6*REGBYTES(sp)
	LOAD   x7,  7*REGBYTES(sp)
	LOAD   x8,  8*REGBYTES(sp)
	LOAD   x9,  9*REGBYTES(sp)
	LOAD  x10, 10*REGBYTES(sp)
	LOAD  x11, 11*REGBYTES(sp)
	LOAD  x12, 12*REGBYTES(sp)
	LOAD  x13, 13*REGBYTES(sp)
	LOAD  x14, 14*REGBYTES(sp)
	LOAD  x15, 15*REGBYTES(sp)
	LOAD  x16, 16*REGBYTES(sp)
	LOAD  x17, 17*REGBYTES(sp)
	LOAD  x18, 18*REGBYTES(sp)
	LOAD  x19, 19*REGBYTES(sp)
	LOAD  x20, 20*REGBYTES(sp)
	LOAD  x21, 21*REGBYTES(sp)
	LOAD  x22, 22*REGBYTES(sp)
	LOAD  x23, 23*REGBYTES(sp)
	LOAD  x24, 24*REGBYTES(sp)
	LOAD  x25, 25*REGBYTES(sp)
	LOAD  x26, 26*REGBYTES(sp)
	LOAD  x27, 27*REGBYTES(sp)
	LOAD  x28, 28*REGBYTES(sp)
	LOAD  x29, 29*REGBYTES(sp)
	LOAD  x30, 30*REGBYTES(sp)
	LOAD  x31, 31*REGBYTES(sp)


	addi   sp, sp, 32*REGBYTES

	mret


irq_context_return:
	ret
