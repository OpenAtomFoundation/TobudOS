/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
#ifndef H_ARC_MP
#define H_ARC_MP

/* ARC multi processors/cores related functions */
#include "arc/arc_builtin.h"

#ifndef INTNO_ICI
#define INTNO_ICI       19 /* inter-core interrupt intno */
#endif

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

/* smp barrier */
#define arc_smp_mb()    arc_mb()

typedef struct {
	volatile uint32_t slock;
} ARC_SPINLOCK_T;

#define ARC_SPIN_LOCK_LOCKED    1
#define ARC_SPIN_LOCK_UNLOCKED  0

/**
 * @fn void arc_spinlock_get(ARC_SPINLOCK_T *lock)
 * @brief Get a spin lock
 *
 * @param lock Spin lock to get
 */
Inline void arc_spinlock_get(ARC_SPINLOCK_T *lock)
{
#if ARC_FEATURE_ARC_FAMILY == ARC_FEATURE_ARC_HS
	uint32_t val;

	arc_smp_mb();

	Asm(
		"1:	llock	%[val], [%[slock]]	\n"
		"	breq	%[val], %[LOCKED], 1b	\n"     /* spin while LOCKED */
		"	scond	%[LOCKED], [%[slock]]	\n"     /* acquire */
		"	bnz	1b			\n"
		"					\n"
		:[val] "=&r" (val)
		:[slock] "r" (&(lock->slock)),
		[LOCKED] "r" (ARC_SPIN_LOCK_LOCKED)
		: "memory", "cc");

	arc_smp_mb();
#else
	lock->slock = arc_clri();
#endif
}

/**
 * @fn int32_t arc_spinlock_try(ARC_SPINLOCK_T *lock)
 * @brief Try to get a spinlock
 *
 * @param lock Spin lock to get
 * @return 0 failed, 1 success
 */
Inline int32_t arc_spinlock_try(ARC_SPINLOCK_T *lock)
{

#if ARC_FEATURE_ARC_FAMILY == ARC_FEATURE_ARC_HS
	uint32_t val, got_it = 0;

	arc_smp_mb();

	Asm(
		"1:	llock	%[val], [%[slock]]	\n"
		"	breq	%[val], %[LOCKED], 4f	\n"     /* already LOCKED, just bail */
		"	scond	%[LOCKED], [%[slock]]	\n"     /* acquire */
		"	bnz	1b			\n"
		"	mov	%[got_it], 1		\n"
		"4:					\n"
		"					\n"
		:[val] "=&r" (val), [got_it] "+&r" (got_it)
		:[slock] "r" (&(lock->slock)),
		[LOCKED] "r" (ARC_SPIN_LOCK_LOCKED)
		: "memory", "cc");
	arc_smp_mb();

	return got_it;
#else
	lock->slock = arc_clri();

	return 1;
#endif
}

/**
 * @fn void arc_spinlock_release(ARC_SPINLOCK_T *lock)
 * @brief release a spin lock
 *
 * @param lock Spin lock to release
 */
Inline void arc_spinlock_release(ARC_SPINLOCK_T *lock)
{
#if ARC_FEATURE_ARC_FAMILY == ARC_FEATURE_ARC_HS
	arc_smp_mb();

	lock->slock = ARC_SPIN_LOCK_UNLOCKED;

	arc_smp_mb();
#else
	arc_seti(lock->slock);
#endif
}

/* variables required by start up of multi-core */
extern volatile uint32_t arc_cpu_wake_flag;
extern volatile uint8_t *arc_cpu_sp;

extern void arc_start_slave_cpu(uint32_t cpu_num, uint8_t *stack, uint32_t sz,
				void (*fn)(uint32_t, void *), void *arg);
extern void arc_slave_start(uint32_t cpu_num);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* H_ARC_MP */
