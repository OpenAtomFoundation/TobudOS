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
 * @ingroup ARC_HAL_BUILTIN
 * @brief Header file of builtin and helper functions
 *
 * The Metaware toolchain and the GNU toolchain are supported. The details please go to see the file.
 */

/**
 * @addtogroup	ARC_HAL_BUILTIN
 * @{
 */

#ifndef H_ARC_BUILTIN
#define H_ARC_BUILTIN

#include "embARC_toolchain.h"
#include "arc/arc.h"

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#if defined (__MW__)    /* Metaware toolchain */

#elif defined (__GNU__) /* GNU toolchain */

#endif

/**
 * @brief arc_compiler_usually(expr) evaluates expression expr and
 * 		  informs the compiler that the value is usually true.
 */
#define arc_compiler_usually(a)         __builtin_expect(!!(a), 1)

/**
 * @brief arc_compiler_rarely(expr) evaluates expression expr and
 *	informs the compiler that the value is rarely true.
 */
#define arc_compiler_rarely(a)          __builtin_expect(!!(a), 0)

/**
 * @fn void arc_mb(void)
 * @brief Do memory barrier
 *
 */
Inline void arc_mb(void)
{
	Asm("dmb 3\n" : : : "memory");
}

/**
 * @fn void arc_rmb(void)
 * @brief Read memory barrier
 *
 */
Inline void arc_rmb(void)
{
	Asm("dmb 1\n" : : : "memory");
}

/**
 * @fn void arc_wmb(void)
 * @brief Write memory barrier
 *
 */
Inline void arc_wmb(void)
{
	Asm("dmb 2\n" : : : "memory");
}

/**
 * @fn uint32_t arc_aux_read(uint32_t aux)
 * @brief Read auxiliary register
 *
 * @param aux auxiliary register address
 * @return value of auxiliary register
 */
Inline uint32_t arc_aux_read(uint32_t aux)
{
	uint32_t ret;

	Asm("lr %0, [%1]"
	    : "=r" (ret)
	    : "r" (aux));

	return ret;
}

/**
 * @fn void arc_aux_write(uint32_t aux, uint32_t val)
 * @brief Write auxiliary register
 *
 * @param aux auxiliary register address
 * @param val Value to write
 */
Inline void arc_aux_write(uint32_t aux, uint32_t val)
{
	Asm(
		"sr   %0, [%1]	\n"
		:
		: "ir" (val), "r" (aux));
}

/**
 * @fn void arc_brk(void)
 * @brief Call brk instruction
 * stop the core through a brk instruction
 */
Inline void arc_brk(void)
{
	Asm("brk");
}

/**
 * @fn void arc_sync(void)
 * @brief Call sync instruction
 *
 */
Inline void arc_sync(void)
{
	Asm("sync");
}

/**
 * @fn void arc_kflag(uint32_t flag)
 * @brief Call kflag instruction to change status32
 *
 * @param flag Flag to set in status32
 */
Inline void arc_kflag(uint32_t flag)
{
	/*sr cannot write AUX_STATUS32 */
	Asm("kflag %0" ::"ir" (flag));
}

/**
 * @fn void arc_nop(void)
 * @brief Call nop_s function
 * flush the pipeline by nop_s instruction
 */
Inline void arc_nop(void)
{
	Asm("nop_s");
}

/**
 * @fn uint32_t arc_clri(void)
 * @brief Call clri instruction
 * call a clri instruction to disable interrupt
 * @return interrupt related bits in status32
 */
Inline uint32_t arc_clri(void)
{
	uint32_t v;

	Asm("clri %0" : "=r" (v):: "memory");
	return v;
}

/**
 * @fn void arc_seti(uint32_t key)
 * @brief Call seti instruction
 * call a set instruction to change interrupt status
 * @param key  interrupt status
 */
Inline void arc_seti(uint32_t key)
{
	Asm("seti %0" : : "ir" (key) : "memory");
}

/**
 * @fn uint32_t arc_swap32(uint32_t val)
 * @brief Swap bytes order of a 32-bits value
 *
 * @param val Target value
 * @return Swapped value
 */
Inline uint32_t arc_swap32(uint32_t val)
{
	uint32_t v;

	Asm("swape %0, %1" : "=r" (v) : "r" (val));
	return v;
}

/**
 * @fn uint16_t arc_swap16(uint32_t val)
 * @brief Swap bytes order of a 32-bits value and return high 16-bits
 *
 * @param val Target value
 * @return High 16 bits of the swapped value
 */
Inline uint16_t arc_swap16(uint32_t val)
{
	uint32_t temp;
	uint32_t v;

	Asm("swape %0, %1" : "=r" (temp) : "r" (val));
	Asm("lsr16 %0, %1" : "=r" (v) : "r" (temp));
	return (uint16_t)v;
}

/**
 * @name cache related helper function
 * @{
 */

/**
 * @fn arc_read_uncached_32(void *ptr)
 * @brief  Read memory and bypass the cache
 * @param ptr Memory address
 * @return data in the memory
 */
Inline uint32_t arc_read_uncached_32(void *ptr)
{
	uint32_t ret;

	Asm("ld.di %0, [%1]" : "=r" (ret) : "r" (ptr));
	return ret;
}

/**
 * @fn arc_write_uncached_32(void *ptr, uint32_t data)
 * @brief  Write memory and bypass the cache
 * @param ptr memory address
 * @param data value to be written
 */
Inline void arc_write_uncached_32(void *ptr, uint32_t data)
{
	Asm("st.di %0, [%1]" :: "r" (data), "r" (ptr));
}

/**
 * @fn arc_read_cached_32(void *ptr)
 * @brief  Read memory with cache
 * @param ptr Memory address
 * @returns data in the memory
 */
Inline uint32_t arc_read_cached_32(void *ptr)
{
	uint32_t ret;

	Asm("ld %0, [%1]" : "=r" (ret) : "r" (ptr));
	return ret;
}

/**
 * @fn void arc_write_cached_32(void *ptr, uint32_t data)
 * @brief  Read memory with cache
 * @param ptr Memory address
 * @param data Data to be written
 */
Inline void arc_write_cached_32(void *ptr, uint32_t data)
{
	Asm("st %0, [%1]" :: "r" (data), "r" (ptr));
}

/**
 * @fn int32_t arc_goto_main(int32_t argc, char **argv)
 * @brief Go to main function with proper arguments
 * @param argc Argument count
 * @param argv Argument content array
 * @retval Return value of main function
 */
Inline int32_t arc_goto_main(int32_t argc, char **argv)
{
	int32_t ret;

	Asm(
		"mov %%r0, %1\n"
		"mov %%r1, %2\n"
		"push_s %%blink\n"
		"jl main\n"
		"pop_s %%blink\n"
		"mov %0, %%r0"
		: "=r" (ret) : "r" (argc), "r" (argv));
	return (int32_t)ret;
}

/**
 * @fn uint32_t arc_find_msb(uint32_t val)
 * @brief Find most significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the most significant bit
 * in the argument passed in and returns the index of that bit. Bits are
 * numbered starting at 1 from the least significant bit. A return value of
 * zero indicates that the value passed is zero.
 *
 * @return Most significant bit set, 0 if @a val is 0
 */
Inline uint32_t arc_find_msb(uint32_t val)
{
	uint32_t bit;

	Asm(
		/* BITSCAN_OPTION is required */
		"fls.f %0, %1;\n"
		"add.nz %0, %0, 1;\n"
		: "=r" (bit)
		: "r" (val));

	return bit;
}

/**
 * @fn uint32_t arc_find_lsb(uint32_t val)
 * @brief Find least significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the least significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return Least significant bit set, 0 if @a val is 0
 */
Inline uint32_t arc_find_lsb(uint32_t val)
{
	uint32_t bit;

	Asm(
		/* BITSCAN_OPTION is required */
		"ffs.f %0, %1;\n"
		"add.nz %0, %0, 1;\n"
		"mov.z %0, 0;\n"
		: "=&r" (bit)
		: "r" (val));

	return bit;
}

/**
 * @fn uint32_t arc_core_id(void)
 * @brief Read core id
 *
 * @return Core id
 */
Inline uint32_t arc_core_id(void)
{
	uint32_t ret;

	Asm(
		"lr %0, [%1]\n"
		"xbfu %0, %0, 0xe8\n"
		: "=r" (ret)
		: "i" (AUX_IDENTITY));

	return ret;
}

/**
 * @fn void arc_stack_check_enable(void)
 * @brief Enable hardware stack checking
 */
Inline void arc_stack_check_enable(void)
{

#if ARC_FEATURE_SEC_PRESENT
	Asm(
		"bset %0, %0, %1\n"
		"sflag r0\n"
		:
		: "r" (arc_aux_read(AUX_SEC_STAT)), "i" (AUX_SEC_STAT_BIT_SSC));
#else
	Asm(
		"bset %0, %0, %1\n"
		"kflag r0\n"
		:
		: "r" (arc_aux_read(AUX_STATUS32)), "i" (AUX_STATUS_BIT_SC));
#endif
}

/**
 * @fn void arc_stack_check_disable(void)
 * @brief Disable hardware stack checking
 */
Inline void arc_stack_check_disable(void)
{
#if ARC_FEATURE_SEC_PRESENT
	Asm(
		"bclr %0, %0, %1\n"
		"sflag r0\n"
		:
		: "r" (arc_aux_read(AUX_SEC_STAT)), "i" (AUX_SEC_STAT_BIT_SSC));
#else
	Asm(
		"bclr %0, %0, %1\n"
		"kflag r0\n"
		:
		: "r" (arc_aux_read(AUX_STATUS32)), "i" (AUX_STATUS_BIT_SC));
#endif
}

/**
 * @fn void arc_kernel_stack_check_configure(uint32_t top, uint32_t base)
 * @brief Configure stack checking in kernel mode
 *
 * @param top Top of stack
 * @param base Base of stack
 */
Inline void arc_kernel_stack_check_configure(uint32_t top, uint32_t base)
{
#if ARC_FEATURE_SEC_PRESENT
	arc_aux_write(AUX_S_KSTACK_TOP, top);
	arc_aux_write(AUX_S_KSTACK_BASE, base);
#else
	arc_aux_write(AUX_KSTACK_TOP, top);
	arc_aux_write(AUX_KSTACK_BASE, base);
#endif
}

/**
 * @fn uint32_t arc_is_user_mode(void)
 * @brief Check whether processor is in user mode
 * this function relies on AUX_STATUS_BIT_US set in privileged mode first
 * @return 1 user mode, 0 privileged mode
 */
Inline uint32_t arc_is_user_mode(void)
{
	uint32_t status;

	Asm(
		"lr %0, [%1]\n"
		: "=r" (status)
		: "i" (AUX_STATUS32)
		: "memory");

	return !(status & AUX_STATUS_MASK_US) ? 1 : 0;
}

/**
 * @brief Go to user mode
 * It's suggested to disable interrupt before call this function especially
 * when using current stack, i.e., sp == 0. Because interrupt handling requires
 * kernel mode stack which is different with user mode stack.
 * @param target Target address to run in user mode, 0 means next
 *               line of code
 * @param sp Stack where the target address runs, 0 means using
 *           current stack
 */
extern void arc_goto_usermode(void *target, uint32_t *sp);

/**
 * @brief Go to kernel mode
 * this function uses trap exception to do switch from user mode to kernel mode,
 * please install exc_entry_arc_goto_kernelmode for trap exception before call this
 * function
 * @param target Target address to run in kernel mode, 0 means next line of code
 * @param sp Stack where the target address runs, 0 means using current stack
 */
extern void arc_goto_kernelmode(void *target, uint32_t *sp);

/**
 * @brief Trap exception entry used for arc_goto_kernelmode
 *
 * install this entry for trap exception before call arc_goto_kernelmode
 */
extern void exc_entry_arc_goto_kernelmode(void);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

/** @} */
#endif /* H_ARC_BUILTIN */
/** @} */