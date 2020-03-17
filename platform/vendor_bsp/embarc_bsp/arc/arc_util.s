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

#define __ASSEMBLY__
#include "arc/arc.h"
#include "arc/arc_asm_common.h"

	.file "arc_utils.s"

/**
 * @brief Go to user mode
 *
 * @param target Target address to run in user mode, 0 means next
 *               line of code
 * @param sp Stack where the target address runs, 0 means using
 *           current stack
 */
	.text
	.global arc_goto_usermode
	.align 4
arc_goto_usermode:
	cmp	r0, 0
	mov.z	r0, blink
	cmp	r1, 0
	mov.nz	sp, r1
#if defined(ARC_FEATURE_SEC_PRESENT)
	sr	(1 << AUX_ERSEC_STAT_BIT_ERM), [AUX_ERSEC_STAT]
	sr	sp, [AUX_SEC_U_SP]
#else
	sr	sp, [AUX_USER_SP]
#endif
	lr	r1, [AUX_STATUS32]
	bset	r1, r1, AUX_STATUS_BIT_U
	sr	r1, [AUX_ERSTATUS]
	lr	r1, [AUX_STATUS32]
	bset	r1, r1, AUX_STATUS_BIT_AE
	kflag	r1
	sr	r0, [AUX_ERRET]
	rtie

/**
 * @brief go to kernel mode
 * this function uses trap exception to do switch from user mode to kernel mode,
 * please install exc_entry_arc_goto_kernelmode for trap exception before call this
 * function
 * @param target, the target address to run in kernel mode, 0 means next line of code
 * @param sp, the stack where the target address runs, 0 means using current stack
 */
	.text
	.global arc_goto_kernelmode
	.global exc_entry_arc_goto_kernelmode
	.align 4
arc_goto_kernelmode:
	cmp	r0, 0
	mov.z	r0, blink
	cmp	r1, 0
	mov.z	r1, sp
	trap_s	0
	.align 4
exc_entry_arc_goto_kernelmode:
	mov	sp, r1
	sr	r0, [AUX_ERRET]
	lr	r0, [AUX_ERSTATUS]
	bclr	r0, r0, AUX_STATUS_BIT_U
	sr	r0, [AUX_ERSTATUS]
	rtie
