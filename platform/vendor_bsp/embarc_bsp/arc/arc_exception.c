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
 * @brief ARC interrupt and exception handling
 */
#include "arc/arc_exception.h"
#include "arc/arc_cache.h"

#define  DBG_LESS
#include "embARC_debug.h"

/**
 * @addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 * @var EXC_ENTRY_T exc_entry_table
 * @brief exception entry table
 *
 * Install exception entry table to ARC_AUX_INT_VECT_BASE in startup.
 * According to ARCv2 ISA, vectors are fetched in instruction space and thus
 * may be present in ICCM, Instruction Cache, or
 * main memory accessed by instruction fetch logic.
 * So it is put into a specific section .vector.
 *
 * Please note that the exc_entry_table maybe cached in ARC. Some functions is
 * defined in .s files.
 *
 */

#ifdef CONFIG_ARC_EXCEPTION_DEBUG
/* For EV_ProtV, the numbering/semantics of the parameter are consistent across
 * several codes, although not all combination will be reported.
 *
 * These codes and parameters do not have associated* names in
 * the technical manual, just switch on the values in Table 6-5
 */
static void dump_protv_access_err(uint32_t parameter)
{
	switch (parameter) {
	case 0x1:
		EMBARC_PRINTF("code protection scheme");
		break;
	case 0x2:
		EMBARC_PRINTF("stack checking scheme");
		break;
	case 0x4:
		EMBARC_PRINTF("MPU");
		break;
	case 0x8:
		EMBARC_PRINTF("MMU");
		break;
	case 0x10:
		EMBARC_PRINTF("NVM");
		break;
	case 0x24:
		EMBARC_PRINTF("Secure MPU");
		break;
	case 0x44:
		EMBARC_PRINTF("Secure MPU with SID mismatch");
		break;
	default:
		EMBARC_PRINTF("unknown parameter");
		break;
	}
}

static void dump_protv_exception(uint32_t cause, uint32_t parameter)
{
	switch (cause) {
	case 0x0:
		EMBARC_PRINTF("Instruction fetch violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x1:
		EMBARC_PRINTF("Memory read protection violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x2:
		EMBARC_PRINTF("Memory write protection violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x3:
		EMBARC_PRINTF("Memory read-modify-write violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x10:
		EMBARC_PRINTF("Normal vector table in secure memory");
		break;
	case 0x11:
		EMBARC_PRINTF("NS handler code located in S memory");
		break;
	case 0x12:
		EMBARC_PRINTF("NSC Table Range Violation");
		break;
	default:
		EMBARC_PRINTF("unknown cause");
		break;
	}
}

static void dump_machine_check_exception(uint32_t cause, uint32_t parameter)
{
	switch (cause) {
	case 0x0:
		EMBARC_PRINTF("double fault");
		break;
	case 0x1:
		EMBARC_PRINTF("overlapping TLB entries");
		break;
	case 0x2:
		EMBARC_PRINTF("fatal TLB error");
		break;
	case 0x3:
		EMBARC_PRINTF("fatal cache error");
		break;
	case 0x4:
		EMBARC_PRINTF("internal memory error on instruction fetch");
		break;
	case 0x5:
		EMBARC_PRINTF("internal memory error on data fetch");
		break;
	case 0x6:
		EMBARC_PRINTF("illegal overlapping MPU entries");
		if (parameter == 0x1) {
			EMBARC_PRINTF(" (jump and branch target)");
		}
		break;
	case 0x10:
		EMBARC_PRINTF("secure vector table not located in secure memory");
		break;
	case 0x11:
		EMBARC_PRINTF("NSC jump table not located in secure memory");
		break;
	case 0x12:
		EMBARC_PRINTF("secure handler code not located in secure memory");
		break;
	case 0x13:
		EMBARC_PRINTF("NSC target address not located in secure memory");
		break;
	case 0x80:
		EMBARC_PRINTF("uncorrectable ECC or parity error in vector memory");
		break;
	default:
		EMBARC_PRINTF("unknown cause");
		break;
	}
}

static void dump_privilege_exception(uint32_t cause, uint32_t parameter)
{
	switch (cause) {
	case 0x0:
		EMBARC_PRINTF("Privilege violation");
		break;
	case 0x1:
		EMBARC_PRINTF("disabled extension");
		break;
	case 0x2:
		EMBARC_PRINTF("action point hit");
		break;
	case 0x10:
		switch (parameter) {
		case 0x1:
			EMBARC_PRINTF("N to S return using incorrect return mechanism");
			break;
		case 0x2:
			EMBARC_PRINTF("N to S return with incorrect operating mode");
			break;
		case 0x3:
			EMBARC_PRINTF("IRQ/exception return fetch from wrong mode");
			break;
		case 0x4:
			EMBARC_PRINTF("attempt to halt secure processor in NS mode");
			break;
		case 0x20:
			EMBARC_PRINTF("attempt to access secure resource from normal mode");
			break;
		case 0x40:
			EMBARC_PRINTF("SID violation on resource access (APEX/UAUX/key NVM)");
			break;
		default:
			EMBARC_PRINTF("unknown parameter");
			break;
		}
		break;
	case 0x13:
		switch (parameter) {
		case 0x20:
			EMBARC_PRINTF("attempt to access secure APEX feature from NS mode");
			break;
		case 0x40:
			EMBARC_PRINTF("SID violation on access to APEX feature");
			break;
		default:
			EMBARC_PRINTF("unknown parameter");
			break;
		}
		break;
	default:
		EMBARC_PRINTF("unknown cause");
		break;
	}
}

static void dump_exception_info(uint32_t vector, uint32_t cause, uint32_t param)
{
	/* Names are exactly as they appear in Designware ARCv2 ISA
	 * Programmer's reference manual for easy searching
	 */
	switch (vector) {
	case EXC_NO_RESET:
		EMBARC_PRINTF("Reset");
		break;
	case EXC_NO_MEM_ERR:
		EMBARC_PRINTF("Memory Error");
		break;
	case EXC_NO_INS_ERR:
		EMBARC_PRINTF("Instruction Error");
		break;
	case EXC_NO_MAC_CHK:
		EMBARC_PRINTF("EV_MachineCheck: ");
		dump_machine_check_exception(cause, param);
		break;
	case EXC_NO_TLB_MISS_I:
		EMBARC_PRINTF("EV_TLBMissI");
		break;
	case EXC_NO_TLB_MISS_D:
		EMBARC_PRINTF("EV_TLBMissD");
		break;
	case EXC_NO_PRO_VIO:
		EMBARC_PRINTF("EV_ProtV: ");
		dump_protv_exception(cause, param);
		break;
	case EXC_NO_PRI_VIO:
		EMBARC_PRINTF("EV_PrivilegeV: ");
		dump_privilege_exception(cause, param);
		break;
	case EXC_NO_SWI:
		EMBARC_PRINTF("EV_SWI");
		break;
	case EXC_NO_TRAP:
		EMBARC_PRINTF("EV_Trap");
		break;
	case EXC_NO_EXT:
		EMBARC_PRINTF("EV_Extension");
		break;
	case EXC_NO_DIV_ZER0:
		EMBARC_PRINTF("EV_DivZero");
		break;
	case EXC_NO_DC_ERR:
		EMBARC_PRINTF("EV_DCError");
		break;
	case EXC_NO_MAL_ALIGN:
		EMBARC_PRINTF("EV_Misaligned");
		break;
	case EXC_NO_VEC_UNIT:
		EMBARC_PRINTF("EV_VecUnit");
		break;
	default:
		EMBARC_PRINTF("unknown exception vector");
		break;
	}

	EMBARC_PRINTF("\n");
}

#endif /* CONFIG_ARC_EXCEPTION_DEBUG */

/**
 * @ingroup ARC_HAL_EXCEPTION_CPU
 * @brief  Install default cpu exception handler
 * @param p_excinf Pointer to the exception frame
 */
static void exc_handler_default(void *p_excinf)
{
	uint32_t excpt_cause_reg = 0;
	uint32_t excpt_ret_reg = 0;
	uint32_t exc_no = 0;
	uint32_t exc_cause = 0;
	uint32_t exc_param = 0;

	excpt_cause_reg = arc_aux_read(AUX_ECR);
	excpt_ret_reg = arc_aux_read(AUX_ERRET);
	exc_no = (excpt_cause_reg >> 16) & 0xff;
	exc_cause = (excpt_cause_reg >> 8) & 0xff;
	exc_param = (excpt_cause_reg >> 0) & 0xff;

	dbg_printf(DBG_LESS_INFO, "default cpu exception handler\r\n");
	dbg_printf(DBG_LESS_INFO, "exc_no:%d, last sp:0x%08x, ecr:0x%08x, eret:0x%08x\r\n",
		   exc_no, p_excinf, excpt_cause_reg, excpt_ret_reg);
#ifdef CONFIG_ARC_EXCEPTION_DEBUG
	dump_exception_info(exc_no, exc_cause, exc_param);
#endif

#ifndef EMBARC_UNIT_TEST
	arc_kflag(1);
#endif
}

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @brief  Register default interrupt handler
 * @param p_excinf	Information for interrupt handler
 */
static void int_handler_default(void *p_excinf)
{
	uint32_t int_cause_reg = 0;

	int_cause_reg = arc_aux_read(AUX_IRQ_CAUSE);
	dbg_printf(DBG_LESS_INFO, "default interrupt handler\r\n");
	dbg_printf(DBG_LESS_INFO, "last sp:0x%08x, icause:0x%08x\r\n", p_excinf, int_cause_reg);
	arc_kflag(1);
}

/* Enforcement of 1024 byte alignment */
__attribute__ ((aligned(1024), section(".vector")))

/**
 * @cond Doxygen_Suppress
 */
EXC_ENTRY_T exc_entry_table[NUM_EXC_ALL] = { [0] = exc_entry_reset,
	[1 ... NUM_EXC_CPU - 1] = exc_entry_cpu,
	[NUM_EXC_CPU ... NUM_EXC_ALL - 1] = exc_entry_int
};
/**
 * @var EXC_HANDLER_T exc_int_handler_table
 * @brief CPU exception and interrupt exception handler table
 * called in exc_entry_default and exc_entry_int
 */
EXC_HANDLER_T exc_int_handler_table[NUM_EXC_ALL] = {
	[0 ... NUM_EXC_CPU - 1] = exc_handler_default,
	[NUM_EXC_CPU ... NUM_EXC_ALL - 1] = int_handler_default
};

/**
 * @endcond
 */
typedef struct {
	/* note: little endian */
	uint32_t save_nr_gpr_pairs : 5; /** Indicates number of general-purpose register pairs saved, from 0 to 8/16 */
	uint32_t res : 4;               /** Reserved */
	uint32_t save_blink : 1;        /** Indicates whether to save and restore BLINK */
	uint32_t save_lp_regs : 1;      /** Indicates whether to save and restore loop registers (LP_COUNT, LP_START, LP_END) */
	uint32_t save_u_to_u : 1;       /** Indicates if user context is saved to user stack */
	uint32_t res2 : 1;              /** Reserved */
	uint32_t save_idx_regs : 1;     /** Indicates whether to save and restore code-density registers (EI_BASE, JLI_BASE, LDI_BASE) */
	uint32_t res3 : 18;             /** Reserved */
} AUX_IRQ_CTRL_FIELD_T;

typedef union {
	AUX_IRQ_CTRL_FIELD_T bits;
	uint32_t value;
} AUX_IRQ_CTRL_T;

extern uint8_t _f_stack[];
/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @brief  Initialize the exception and interrupt handling
 */
void exc_int_init(void)
{
	uint32_t i;
	uint32_t status;
	AUX_IRQ_CTRL_T ictrl;

	ictrl.value = 0;

#ifndef ARC_FEATURE_RF16
	ictrl.bits.save_nr_gpr_pairs = 6;       /* r0 to r11 (r12 saved manually) */
#else
	ictrl.bits.save_nr_gpr_pairs = 3;       /* r0 to r3, r10, r11  */
#endif
	ictrl.bits.save_blink = 1;
	ictrl.bits.save_lp_regs = 1;    /* LP_COUNT, LP_START, LP_END */
	ictrl.bits.save_u_to_u = 0;     /* user ctxt saved on kernel stack */

#if ARC_FEATURE_CODE_DENSITY
	ictrl.bits.save_idx_regs = 1;       /* JLI, LDI, EI */
#endif

	status = arc_lock_save();
	for (i = NUM_EXC_CPU; i < NUM_EXC_ALL; i++) {
		/* interrupt level triggered, disabled, priority is the lowest */
		arc_aux_write(AUX_IRQ_SELECT, i);
		arc_aux_write(AUX_IRQ_ENABLE, 0);
		arc_aux_write(AUX_IRQ_TRIGGER, 0);
#if defined(ARC_FEATURE_SEC_PRESENT)
		arc_aux_write(AUX_IRQ_PRIORITY, (1 << AUX_IRQ_PRIORITY_BIT_S)|(INT_PRI_MAX - INT_PRI_MIN));
#else
		arc_aux_write(AUX_IRQ_PRIORITY, INT_PRI_MAX - INT_PRI_MIN);
#endif
	}
	arc_aux_write(AUX_IRQ_CTRL, ictrl.value);

	arc_unlock_restore(status);

	/** ipm should be set after cpu unlock restore to avoid reset of the status32 value */
	arc_int_ipm_set((INT_PRI_MAX - INT_PRI_MIN));

#if ARC_FEATURE_RGF_BANKED_REGS >= 16 && ARC_FEATURE_FIRQ == 1
#if _STACKSIZE < 512
#error "not enough stack size for irq and firq"
#endif
	/* top 256 bytes of stack used as firq stack */
	arc_firq_stack_set(_f_stack + 256);
#endif
}

/**
 * @ingroup ARC_HAL_EXCEPTION_CPU
 * @brief  Install a CPU exception entry
 * @param excno Exception number
 * @param entry Exception entry to install
 */
int32_t exc_entry_install(const uint32_t excno, EXC_ENTRY_T entry)
{
	uint32_t status;
	EXC_ENTRY_T *table;

#if defined(ARC_FEATURE_SEC_PRESENT)
	table = (EXC_ENTRY_T *)arc_aux_read(AUX_INT_VECT_BASE_S);
#else
	table = (EXC_ENTRY_T *)arc_aux_read(AUX_INT_VECT_BASE);
#endif

	if (excno < NUM_EXC_ALL && entry != NULL
	    && table[excno] != entry) {
		status = cpu_lock_save();
		/* directly write to mem, as arc gets exception handler from mem not from cache */
		/* FIXME, here maybe icache is dirty, need to be invalidated */
		table[excno] = entry;

		if (arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
			/* dcache is available */
			dcache_flush_line((uint32_t)&table[excno]);
		}

		if (arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
			/* icache is available */
			icache_invalidate_line((uint32_t)&table[excno]);
		}
		cpu_unlock_restore(status);
		return 0;
	}
	return -1;
}

/**
 * @ingroup ARC_HAL_EXCEPTION_CPU
 * @brief  Get the installed CPU exception entry
 * @param excno Exception number
 * @return The installed CPU exception entry
 */
EXC_ENTRY_T exc_entry_get(const uint32_t excno)
{
	if (excno < NUM_EXC_ALL) {
		return exc_entry_table[excno];
	}
	return NULL;
}

/**
 * @ingroup ARC_HAL_EXCEPTION_CPU
 * @brief  Install an exception handler
 * @param excno Exception number
 * @param handler The installed CPU exception handler
 */
int32_t exc_handler_install(const uint32_t excno, EXC_HANDLER_T handler)
{
	if (excno < NUM_EXC_ALL && handler != NULL) {
		exc_int_handler_table[excno] = handler;
		return 0;
	}

	return -1;
}

/**
 * @ingroup ARC_HAL_EXCEPTION_CPU
 * @brief Get the installed exception handler
 * @param excno Exception number
 * @return The installed exception handler or NULL
 */
EXC_HANDLER_T exc_handler_get(const uint32_t excno)
{
	if (excno < NUM_EXC_ALL) {
		return exc_int_handler_table[excno];
	}

	return NULL;
}

#ifndef EMBARC_OVERRIDE_ARC_INTERRUPT_MANAGEMENT
/**
 * @brief Disable interrupt
 *
 * @param intno Interrupt number
 */
int32_t int_disable(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_disable(intno);
		return 0;
	}

	return -1;
}

/**
 * @brief Enable interrupt
 *
 * @param intno Interrupt number
 */
int32_t int_enable(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_enable(intno);
		return 0;
	}

	return -1;
}

/**
 * @brief Get interrupt enable state
 *
 * @param intno Interrupt number
 * @return 0 disabled, 1 enabled, < 0 error
 */
int32_t int_enabled(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_aux_write(AUX_IRQ_SELECT, intno);
		return arc_aux_read(AUX_IRQ_ENABLE);
	}

	return -1;
}

/**
 * @brief Get interrupt priority masking threshold
 *
 * @returns Interrupt priority masking threshold, negative num
 */
int32_t int_ipm_get(void)
{
	return ((int32_t)arc_int_ipm_get() + INT_PRI_MIN);
}

/**
 * @brief Set interrupt priority masking threshold
 *
 * @param intpri Interrupt priority masking threshold
 */
int32_t int_ipm_set(int32_t intpri)
{
	if (intpri >= INT_PRI_MIN && intpri <= INT_PRI_MAX) {
		arc_int_ipm_set(intpri - INT_PRI_MIN);
		return 0;
	}

	return -1;
}

/**
 * @brief Get current interrupt priority masking threshold
 *
 * @param intno Interrupt number
 * @return  < 0 interrupt priority, 0 error
 */
int32_t int_pri_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return (int32_t)arc_int_pri_get(intno) + INT_PRI_MIN;
	}

	return 0;
}

/**
 * @brief Set interrupt priority
 *
 * @param intno Interrupt number
 * @param intpri Interrupt priority
 * @return  < 0 error, 0 ok
 */
int32_t int_pri_set(const uint32_t intno, int32_t intpri)
{
	uint32_t status;

	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		status = cpu_lock_save();
		intpri = intpri - INT_PRI_MIN;
		arc_int_pri_set(intno, (uint32_t)intpri);
		cpu_unlock_restore(status);
		return 0;
	}
	return -1;
}

/**
 * @brief Set interrupt secure or not secure
 * @param intno Interrupt number
 * @param secure 0 for normal, > 0 for secure
 * @return < 0 error, 0 ok
 */
int32_t int_secure_set(const uint32_t intno, uint32_t secure)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_secure_set(intno, secure);
		return 0;
	}
	return -1;

}

/**
 * @brief Probe interrupt pending state
 *
 * @param intno Interrupt number
 *
 * @returns 1 pending, 0 no pending, -1 error
 */
int32_t int_probe(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return arc_int_probe(intno);
	}
	return -1;
}

/**
 * @brief Trigger interrupt through software
 *
 * @param intno Interrupt number
 * @return 0 ok, -1 error
 */
int32_t int_sw_trigger(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_sw_trigger(intno);
		return 0;
	}
	return -1;
}

/**
 * @brief Configure interrupt trigger mode
 *
 * @param intno Interrupt number
 * @param level 0-level triggered, 1-pulse triggered
 * @return 0 ok, -1 error
 */
int32_t int_level_config(const uint32_t intno, const uint32_t level)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		arc_int_level_config(intno, level);
		return 0;
	}
	return -1;
}

/**
 * \brief Get interrupt request mode
 *
 * @param intno Interrupt number
 */
int32_t int_level_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return (int32_t)arc_int_level_get(intno);
	}
	return -1;
}

/**
 * @brief lock cpu, disable interrupts
 */
void cpu_lock(void)
{
	arc_lock();
}

/**
 * @brief Unlock cpu to enable interrupts
 */
void cpu_unlock(void)
{
	arc_unlock();
}

/**
 * @brief Lock cpu and return status
 *
 * @returns CPU status
 */
uint32_t cpu_lock_save(void)
{
	return arc_lock_save();
}

/**
 * @brief Unlock cpu with the specific status
 *
 * @param status  CPU status saved by cpu_lock_save
 */
void cpu_unlock_restore(const uint32_t status)
{
	arc_unlock_restore(status);
}

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @brief Install interrupt handler
 * @param intno	Interrupt number
 * @param handler Interrupt handler
 */
int32_t int_handler_install(const uint32_t intno, INT_HANDLER_T handler)
{
	/*!< @todo parameter check ? */
	if (intno >= NUM_EXC_CPU) {
		return exc_handler_install(intno, handler);
	}

	return -1;
}

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @brief Get the installed interrupt handler
 * @param intno Interrupt number
 * @return The installed interrupt handler or NULL
 */
INT_HANDLER_T int_handler_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU) {
		return exc_handler_get(intno);
	}

	return NULL;
}

#endif /* EMBARC_OVERRIDE_ARC_INTERRUPT_MANAGEMENT */

/**
 * @brief Set the stack pointer for firq handling
 *
 * @param firq_sp stack pointer
 */
void arc_firq_stack_set(uint8_t *firq_sp)
{
	uint32_t status;

	if (firq_sp == NULL) {
		return;
	}

	status = arc_lock_save();

	Asm(
/* only ilink will not be banked, so use ilink as channel
 * between 2 banks
 */
		"mov %%ilink, %0		\n\t"
		"lr %0, [%1]			\n\t"
		"or %0, %0, %2			\n\t"
		"kflag %0			\n\t"
		"mov %%sp, %%ilink		\n\t"
/* switch back to bank0, use ilink to avoid the pollution of
 * bank1's gp regs.
 */
		"lr %%ilink, [%1]		\n\t"
		"and %%ilink, %%ilink, %3	\n\t"
		"kflag %%ilink			\n\t"
		:
		: "r" (firq_sp), "i" (AUX_STATUS32),
		"i" (AUX_STATUS_RB(1)),
		"i" (~AUX_STATUS_RB(7))
		);

	arc_unlock_restore(status);
}
/** @} end of group ARC_HAL_EXCEPTION_CPU */
