/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
 * @file
 * @ingroup ARC_HAL_STARTUP
 * @brief Assembly part of startup process
 */

/**
 * @addtogroup ARC_HAL_STARTUP
 * @{
 */
/** @cond STARTUP_ASM */

#define __ASSEMBLY__
#include "arc/arc.h"
#include "arc/arc_asm_common.h"

	.file "arc_startup.s"

.weak	_f_sdata		/* start of small data, defined in link script */
.weak	init_hardware_hook	/* app hardware init hook */

.extern	board_main
.extern exc_entry_table

/* initial vector table */
	.section .init_vector, "a"
	.long exc_entry_reset
	.section .init_bootstrap, "ax"
	.global exc_entry_reset
	.global _start
	.align 4
_start:
exc_entry_reset:
_arc_reset_stage1:
	kflag	STATUS32_RESET_VALUE

/* STAGE 1 */

/* necessary hardware should be done first to speed up initialization
	1. system clk
	2. mem controller must be initialized before any access to external
	mem.
	3. others
*/
_arc_cache_init_start:
	lr	r0, [AUX_BCR_D_CACHE]
	cmp	r0, 2
	/* invalidate dcache */
	jle	_arc_icache_init
	mov	r0, 1
	sr	r0, [AUX_DC_IVDC]
	sr	r0, [AUX_DC_CTRL]
_arc_icache_init:
	lr	r0, [AUX_BCR_I_CACHE]
	cmp	r0, 2
	jle	_arc_cache_init_end
	/* invalidate icache */
	mov	r0, 1
	sr	r0, [AUX_IC_IVIC]
	nop_s
	nop_s
	nop_s
	sr	r0, [AUX_IC_CTRL]

_arc_cache_init_end:
	mov	r0, init_hardware_hook
	cmp	r0, 0
	jlne	[r0]

/* STAGE 2: init necessary registers */

_arc_reset_stage2:
	mov	r0, 0

/* interrupt related init */
	sr	r0, [AUX_IRQ_ACT]
	sr	r0, [AUX_IRQ_CTRL]
	sr	r0, [AUX_IRQ_HINT]

/* use the new vector table to replace the old one */
#if defined(ARC_FEATURE_SEC_PRESENT)
	sr	exc_entry_table, [AUX_INT_VECT_BASE_S]
#else
	sr	exc_entry_table, [AUX_INT_VECT_BASE]
#endif
	mov	gp, _f_sdata	/* init small-data base register */
	mov	fp, 0		/* init fp register */

#if ARC_FEATURE_MP_NUM_CPUS > 1
	GET_CORE_ID r0
	breq r0, 0, _master_core_startup

_slave_core_wait:
	ld r1, [arc_cpu_wake_flag]
	brne r0, r1, _slave_core_wait

	ld sp, [arc_cpu_sp]
	/* signal master core that slave core runs */
	st 0, [arc_cpu_wake_flag]

	j arc_slave_start

_master_core_startup:
#endif
	mov	sp, _e_stack	/* init stack pointer */

_arc_reset_stage3:
_s3_copy_text:
	mov	r0, _f_text
	mov	r1, _load_addr_text
	cmp	r0, r1

/* if load addr == run addr, no need to copy */
	jeq	_s3_copy_rodata
	mov	r3, _e_text
_s3_copy_text_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_s3_copy_text_loop
_s3_copy_rodata:
	mov	r0, _f_rodata
	mov	r1, _load_addr_rodata
	cmp	r0, r1

/* if load addr == run addr, no need to copy */
	jeq	_s3_copy_data
	mov	r3, _e_rodata
_s3_copy_rodata_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_s3_copy_rodata_loop
_s3_copy_data:
	mov	r0, _f_data
	mov	r1, _load_addr_data
	cmp	r0, r1
	jeq	_s3_clear_bss

/* if load addr == run addr, no need to copy */
	mov	r3, _e_data
_s3_copy_data_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_s3_copy_data_loop
_s3_clear_bss:
	mov	r0, _f_bss
	mov	r1, _e_bss
	cmp	r0, r1
	jge	_arc_reset_call_main
	mov	r2, 0
_s3_clear_bss_loop:
	st.ab	r2, [r0, 4]
	cmp	r0, r1
	jlt	_s3_clear_bss_loop

/* STAGE 3: go to next level initialization */
_arc_reset_call_main:

	jl	board_main	/* board-level main */
	b	_exit_loop

	.global _exit_loop
	.global _exit_halt
	.align 4
_exit_halt:
_exit_loop:
	flag	AUX_STATUS_MASK_HALT
	nop
	nop
	nop
	b	_exit_loop

#if defined(__MW__)
	.global arc_mwdt_init, arc_mwdt_fini
	.section ".init",text
arc_mwdt_init:
	.cfa_bf	arc_mwdt_init
	push	%blink
	.cfa_push	{%blink}

	.section ".init$999999", text, 1, 2, check_text_align=0
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef

	.section ".fini", text
arc_mwdt_fini:
	.cfa_bf	_fini
	push	%blink
	.cfa_push	{%blink}

	.section ".fini$999999", text, 1, 2, check_text_align=0
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef
#endif
/** @endcond */

/** }@*/
