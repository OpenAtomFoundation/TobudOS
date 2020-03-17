    .global port_int_disable
    .global port_int_enable

    .global port_cpsr_save
    .global port_cpsr_restore

    .global port_sched_start
    .global port_context_switch


    .extern k_curr_task
    .extern k_next_task


   .text
   .align 4


.type port_int_disable, %function
port_int_disable:
    clri
    j [blink]


.type port_int_enable, %function
port_int_enable:
    seti
    j [blink]

.type port_cpsr_save, %function
port_cpsr_save:
    clri r0
    j [blink]


.type port_cpsr_restore, %function
port_cpsr_restore:
    seti r0
    j [blink]

.type port_sched_start, %function
port_sched_start:
    push_s	blink
    clri
    mov r0, 0
    mov r0, k_curr_task
    mov r1, k_next_task
    ld r2, [r1]
    st r2, [r0]   /* k_curr_task = k_next_task*/
    ld sp, [r2] 
    seti
    j	[r0]


.type port_context_switch, %function
port_context_switch:
    pop r15
	pop r14
	pop r13
	pop blink
	pop fp
	pop gp
    j	[blink]




.type PendSV_Handler, %function
TaskSwitchContext:
    clri
    push gp
    push fp
    push blink
    push r13
    push r14
    push r15
    mov r2, dispatch_r
    push r2
    mov r5, k_curr_task
    ld r6, [r5]
    st r0, [r6]
    mov r1, k_next_task
    ld r2, [r1]
    st r2, [r5]
    ld r0, [r2]
    ld sp, [r0]
    seti
    j	[r0]

dispatch_r:
    pop r15
	pop r14
	pop r13
	pop blink
	pop fp
	pop gp
	j	[blink]