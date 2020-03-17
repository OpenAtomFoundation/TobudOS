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
 * @ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * @brief Public interface for configuring interrupts and exceptions
 */

#ifndef H_ARC_EXCEPTION
#define H_ARC_EXCEPTION

#include "arc/arc_builtin.h"

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
#ifndef NUM_EXC_CPU
/*!< number of CPU exceptions */
#define NUM_EXC_CPU     16
#endif

#ifndef NUM_EXC_INT
/*!< number of interrupt exceptions, defined in arc_feature_config.h */
#define NUM_EXC_INT     9
#endif

/*!< total number of exceptions */
#define NUM_EXC_ALL     (NUM_EXC_CPU + NUM_EXC_INT)

#ifdef ARC_FEATURE_SEC_PRESENT
typedef struct {
	uint32_t erbta;

	uint32_t r30;   /* r30 is useless, skipped? */
	uint32_t ilink; /* r29 is useless, skipped?*/
	/* r28 is sp, saved other place */
	uint32_t fp;    /* r27 */
	uint32_t gp;    /* r26 */

#if ARC_FEATURE_DSP || ARC_FEATURE_FPU || ARC_FEATURE_MPU_OPTION_NUM > 6
	/* accl and acch, common for mpy_option >6 and fpu_fma option */
	uint32_t r59;
	uint32_t r58;
#endif

	uint32_t r12;

	uint32_t lp_end, lp_start, lp_count;

#if ARC_FEATURE_CODE_DENSITY
	uint32_t ei, ldi, jli;
#endif

	uint32_t r0, r1, r2, r3;
#ifndef ARC_FEATURE_RF16
	uint32_t r4, r5, r6, r7, r8, r9;
#endif
	uint32_t r10, r11;

	uint32_t blink; /* r31 */
	uint32_t ret;
	uint32_t sec_stat;
	uint32_t status32;
} EMBARC_PACKED INT_EXC_FRAME_T;
#else
typedef struct {
	uint32_t erbta;

	uint32_t r30;   /* r30 is useless, skipped? */
	uint32_t ilink; /* r29 is useless, skipped?*/
	/* r28 is sp, saved other place */
	uint32_t fp;    /* r27 */
	uint32_t gp;    /* r26 */

#if ARC_FEATURE_DSP || ARC_FEATURE_FPU || ARC_FEATURE_MPU_OPTION_NUM > 6
	/* accl and acch, common for mpy_option >6 and fpu_fma option */
	uint32_t r59;
	uint32_t r58;
#endif
	uint32_t r12;

	uint32_t r0, r1, r2, r3;
#ifndef ARC_FEATURE_RF16
	uint32_t r4, r5, r6, r7, r8, r9;
#endif
	uint32_t r10, r11;

	uint32_t blink; /* r31 */

	uint32_t lp_end, lp_start, lp_count;

#if ARC_FEATURE_CODE_DENSITY
	uint32_t ei, ldi, jli;
#endif

	uint32_t ret;
	uint32_t status32;
} EMBARC_PACKED INT_EXC_FRAME_T;
#endif

typedef struct {
	/*  todo xy memory support */
#if ARC_FEATURE_DSP_COMPLEX
	uint32_t dsp_fft_ctrl;
	uint32_t dsp_bfly0;
#endif
	uint32_t acc0_ghi;
	uint32_t acc0_hi;
	uint32_t acc0_glo;
	uint32_t acc0_lo;
	uint32_t dsp_ctrl;

} EMBARC_PACKED DSP_EXT_FRAME_T;

typedef struct {
#if ARC_FEATURE_FPU_DA
	uint32_t dpfp2h;
	uint32_t dpfp2l;
	uint32_t dpfp1h;
	uint32_t dpfp1l;
#endif

	uint32_t fpu_status;
	uint32_t fpu_ctrl;

} EMBARC_PACKED FPU_EXT_FRAME_T;

typedef struct {

#if ARC_FEATURE_FPU_DSP_CONTEXT

#if ARC_FEATURE_DSP
	DSP_EXT_FRAME_T dsp_regs;
#endif

#if ARC_FEATURE_FPU
	FPU_EXT_FRAME_T fpu_ext_regs;
#endif

#endif /* ARC_FEATURE_FPU_DSP_CONTEXT */

#if defined(ARC_ENABLE_EXTRA_CALLEE)
#if ARC_FEATURE_SEC_PRESENT
	uint32_t secure_kernel_sp;
	uint32_t kernel_sp;
#else
	uint32_t user_sp;
#endif
#endif

#ifndef ARC_FEATURE_RF16
	uint32_t r25;
	uint32_t r24;
	uint32_t r23;
	uint32_t r22;
	uint32_t r21;
	uint32_t r20;
	uint32_t r19;
	uint32_t r18;
	uint32_t r17;
	uint32_t r16;
#endif
	uint32_t r15;
	uint32_t r14;
	uint32_t r13;
} EMBARC_PACKED CALLEE_FRAME_T;

typedef struct {
	CALLEE_FRAME_T callee_regs;
	INT_EXC_FRAME_T exc_frame;
} EMBARC_PACKED PROCESSOR_FRAME_T;

#define ARC_PROCESSOR_FRAME_T_SIZE    (sizeof(PROCESSOR_FRAME_T) / sizeof(uint32_t))
#define ARC_INT_EXC_FRAME_T_SIZE      (sizeof(INT_EXC_FRAME_T) / sizeof(uint32_t))
#define ARC_CALLEE_FRAME_T_SIZE       (sizeof(CALLEE_FRAME_T) / sizeof(uint32_t))

/**
 * @fn void arc_vector_base_write(uint32_t vec_base)
 * @brief Write exception vector base
 * 
 * @param vec_base Target vector base
 */
Inline void arc_vector_base_write(uint32_t vec_base)
{
	arc_aux_write(AUX_INT_VECT_BASE, vec_base);
}

/**
 * @fn uint32_t arc_vector_base_read(void)
 * @brief Read current exception vector base
 * 
 * @return Exception vector base (uint32_t)
 */
Inline uint32_t arc_vector_base_read(void)
{
	return arc_aux_read(AUX_INT_VECT_BASE);
}
/** @}*/

/**
 * @addtogroup ARC_HAL_EXCEPTION_INTERRUPT
 * @{
 */
#ifndef INT_PRI_MIN
#define INT_PRI_MIN (-2)        /*!< the minimum interrupt priority */
#endif

#define INT_PRI_MAX (-1)        /*!< the maximum interrupt priority */
/**
 * @fn void arc_int_disable(const uint32_t intno)
 * @brief Disable interrupt
 * 
 * @param intno Interrupt number
 */
Inline void arc_int_disable(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_ENABLE, 0);
}

/**
 * @fn void arc_int_enable(const uint32_t intno)
 * @brief Enable interrupt
 * 
 * @param intno Interrupt number
 */
Inline void arc_int_enable(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_ENABLE, 1);
}

/**
 * @fn uint32_t arc_int_enabled(const uint32_t intno)
 * @brief Check whether the specific interrupt is enabled
 * 
 * @param intno Interrupt number
 * @return 0 disabled, 1 enabled
 */
Inline uint32_t arc_int_enabled(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_ENABLE);
}

/**
 * @fn uint32_t arc_int_ipm_get(void)
 * @brief Get interrupt priority masking threshold
 * 
 * @return Interrupt priority masking threshold, negative num
 */
Inline uint32_t arc_int_ipm_get(void)
{
	return ((arc_aux_read(AUX_STATUS32) >> 1) & 0x0f);
}

/**
 * @fn void arc_int_ipm_set(uint32_t intpri)
 * @brief Set interrupt priority masking threshold
 * 
 * @param intpri Interrupt priority masking threshold, negative num
 */
Inline void arc_int_ipm_set(uint32_t intpri)
{
	volatile uint32_t status;

	status = arc_aux_read(AUX_STATUS32) & ~0x1e;

	status = status | ((intpri << 1) & 0x1e);
	arc_kflag(status);
}

/**
 * @fn uint32_t arc_int_pri_get(const uint32_t intno)
 * @brief Get current interrupt priority masking threshold
 * 
 * @param intno Interrupt number
 */
Inline uint32_t arc_int_pri_get(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_PRIORITY) & 0xf;
}

/**
 * @fn void arc_int_pri_set(const uint32_t intno, uint32_t intpri)
 * @brief Set interrupt priority
 *
 * @param intno Interrupt number
 * @param intpri Interrupt priority
 */
Inline void arc_int_pri_set(const uint32_t intno, uint32_t intpri)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_PRIORITY, intpri | (arc_aux_read(AUX_IRQ_PRIORITY) & 0xfffffff0));
}

/**
 * @fn void arc_int_secure_set(const uint32_t intno, uint32_t secure)
 * @brief Set interrupt secure or not secure
 * @param intno Interrupt number
 * @param secure 0 for normal, > 0 for secure
 */
Inline void arc_int_secure_set(const uint32_t intno, uint32_t secure)
{

	arc_aux_write(AUX_IRQ_SELECT, intno);

	if (secure) {
		arc_aux_write(AUX_IRQ_PRIORITY, arc_aux_read(AUX_IRQ_PRIORITY) |
			      (1 << AUX_IRQ_PRIORITY_BIT_S));
	} else {
		arc_aux_write(AUX_IRQ_PRIORITY, arc_aux_read(AUX_IRQ_PRIORITY) & 0xf);
	}

}

/**
 * @fn uint32_t arc_int_probe(const uint32_t intno) 
 * @brief Probe interrupt pending state
 *
 * @param intno Interrupt number
 * @returns 1 pending, 0 no pending, -1 error
 */
Inline uint32_t arc_int_probe(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_PENDING);
}

/**
 * @fn void arc_int_sw_trigger(const uint32_t intno)
 * @brief Trigger software interrupt
 *
 * @param intno Interrupt number
 */
Inline void arc_int_sw_trigger(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_HINT, intno);
}

/**
 * @fn void arc_int_level_config(const uint32_t intno, const uint32_t level)
 * @brief Configure interrupt request mode.
 *
 * @param intno Interrupt number
 * @param level 0-level triggered, 1-pluse triggered
 */
Inline void arc_int_level_config(const uint32_t intno, const uint32_t level)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_TRIGGER, level);
}

/**
 * @fn uint32_t arc_int_level_get(const uint32_t intno)
 * @brief Get interrupt request mode
 *
 * @param intno Interrupt number
 * @return 0-level triggered, 1-pluse triggered
 */
Inline uint32_t arc_int_level_get(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_TRIGGER) & 0x1;
}

/**
 * @fn void arc_lock(void)
 * @brief Lock cpu to disable interrupts
 */
Inline void arc_lock(void)
{
	arc_clri();
}

/**
 * @fn void arc_unlock(void)
 * @brief Unlock cpu to enable interrupts
 */
Inline void arc_unlock(void)
{
	arc_seti(0);
}

/**
 * @fn uint32_t arc_locked(void)
 * @brief Check if cpu is locked
 *
 * @return 1 locked, 0 unlocked
 */
Inline uint32_t arc_locked(void)
{
	if (arc_aux_read(AUX_STATUS32) & AUX_STATUS_MASK_IE) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * @fn uint32_t arc_lock_save(void)
 * @brief Lock cpu and return status
 *
 * @return CPU status
 */
Inline uint32_t arc_lock_save(void)
{
	return arc_clri();
}

/**
 * @fn void arc_unlock_restore(const uint32_t status)
 * @brief Unlock cpu with the specific status
 *
 * @param status  CPU status saved by cpu_lock_save
 */
Inline void arc_unlock_restore(const uint32_t status)
{
	arc_seti(status);
}

/**
 * @fn uint32_t arc_int_active(void)
 * @brief Check if interrupt is active
 *
 * @return 1 active, 0 inactive
 */
Inline uint32_t arc_int_active(void)
{
	if (arc_aux_read(AUX_IRQ_ACT) == 0) {
		return 0;
	} else {
		return 1;
	}
}
/** @}*/

/**
 * @addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
/**
 * @typedef EXC_ENTRY_T
 * @brief Data type for exception entry
 */
typedef void (*EXC_ENTRY_T) (void);
/**
 * @typedef EXC_HANDLER_T
 * @brief Data type for exception handler
 */
typedef void (*EXC_HANDLER_T) (void *exc_frame);
/** @}*/

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @typedef INT_HANDLER_T
 * @brief Data type for interrupt handler
 */
typedef void (*INT_HANDLER_T) (void *ptr);

extern EXC_ENTRY_T exc_entry_table[NUM_EXC_ALL];
extern EXC_HANDLER_T exc_int_handler_table[NUM_EXC_ALL];

/** @ingroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
/**
 * @fn exc_entry_reset
 * @brief Reset entry
 */
extern void exc_entry_reset(void);
/**
 * @fn exc_entry_cpu
 * @brief Default CPU exception entry
 */
extern void exc_entry_cpu(void);

/**
 * @fn exc_entry_firq
 * @brief Fast interrupt exception entry
 */
extern void exc_entry_firq(void);
/**
 * @fn exc_entry_int
 * @brief Interrupt exception entry
 */
extern void exc_entry_int(void);
/** @}*/

/* exception related apis */
extern void exc_int_init(void);
extern int32_t exc_entry_install(const uint32_t excno, EXC_ENTRY_T entry);
extern EXC_ENTRY_T exc_entry_get(const uint32_t excno);
extern int32_t exc_handler_install(const uint32_t excno, EXC_HANDLER_T handler);
extern EXC_HANDLER_T exc_handler_get(const uint32_t excno);

/* interrupt related apis */
extern int32_t int_disable(const uint32_t intno);
extern int32_t int_enable(const uint32_t intno);
extern int32_t int_enabled(const uint32_t intno);
extern int32_t int_ipm_get(void);
extern int32_t int_ipm_set(int32_t intpri);
extern int32_t int_pri_get(const uint32_t intno);
extern int32_t int_pri_set(const uint32_t intno, int32_t intpri);
extern int32_t int_probe(const uint32_t intno);
extern int32_t int_sw_trigger(const uint32_t intno);
extern int32_t int_level_config(const uint32_t intno, const uint32_t level);
extern int32_t int_level_get(const uint32_t intno);
extern void cpu_lock(void);
extern void cpu_unlock(void);
extern uint32_t cpu_lock_save(void);
extern void cpu_unlock_restore(const uint32_t status);
extern int32_t int_handler_install(const uint32_t intno, INT_HANDLER_T handler);
extern INT_HANDLER_T int_handler_get(const uint32_t intno);
extern int32_t int_secure_set(const uint32_t intno, uint32_t secure);
extern void arc_firq_stack_set(uint8_t *firq_sp);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* H_ARC_EXCEPTION*/
