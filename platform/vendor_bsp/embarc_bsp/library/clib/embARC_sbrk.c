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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "clib/embARC_syscalls.h"

#ifndef _HOSTLINK_ /* Not using hostlink library */

/* Memory Management */
/** Allocating Memory */
#define EMBARC_WITH_MWSBRK

/* Whether heap has been initialized. */
#if defined(__MW__) && defined(EMBARC_WITH_MWSBRK)

static int32_t __heap_init = 0;
static char *__heap_end, *__heap_cur;
extern char _f_heap[];
extern char _e_heap[];

/** Allocating Memory */
void *SYSCALL_PREFIX(_sbrk)(uint32_t size){
	char *new_cur;

	if (__heap_init == 0) {
		__heap_cur = _f_heap;
		__heap_end = _e_heap;
		__heap_init = 1;
	}
	new_cur = __heap_cur + size;
	if (__heap_cur <= new_cur && new_cur <= __heap_end) {
		void *p = (void *)__heap_cur;
		__heap_cur = new_cur;
		return (void *)p; /** return old __heap_cur */
	}

	return NULL;
}

/** Freeing Memory */
void *SYSCALL_PREFIX(_brk)(void *p){
	__heap_cur = p;
	return NULL;
}

#pragma weak sbrk = _sbrk
#pragma weak brk = _brk

#elif defined(__GNU__)

extern char __start_heap;
extern char __end_heap;

caddr_t *SYSCALL_PREFIX(_sbrk)(size_t nbytes){
	static char *heap_ptr = NULL;
	char *prev_heap_ptr;

	if (heap_ptr == NULL) {
		heap_ptr = &__start_heap;
	}

	/* Align the 'heap_ptr' so that memory will always be allocated at word
	   boundaries. */
	heap_ptr = (char *) ((((uint32_t) heap_ptr) + 7) & ~7);
	prev_heap_ptr = heap_ptr;

	if ((heap_ptr + nbytes) < &__end_heap) {
		heap_ptr += nbytes;
		return (caddr_t *) prev_heap_ptr;
	}
	errno = ENOMEM;
	return (caddr_t *) -1;
}
#endif

#endif /* !defined(_HOSTLINK_) */
