#ifndef __ASM_ARM_DIV64
#define __ASM_ARM_DIV64
//#include <asm/system.h>

/*
 * The semantics of do_div() are:
 *
 * uint32_t do_div(uint64_t *n, uint32_t base)
 * {
 * 	uint32_t remainder = *n % base;
 * 	*n = *n / base;
 * 	return remainder;
 * }
 *
 * In other words, a 64-bit dividend with a 32-bit divisor producing
 * a 64-bit result and a 32-bit remainder.  To accomplish this optimally
 * we call a special __do_div64 helper with completely non standard
 * calling convention for arguments and results (beware).
 */
extern unsigned int __div64_32(unsigned long long *dividend, unsigned int divisor);

# define do_div(n,base) ({				\
	unsigned int __base = (base);			\
	unsigned int __rem;					\
	(void)(((typeof((n)) *)0) == ((uint64_t *)0));	\
	if (((n) >> 32) == 0) {			\
		__rem = (unsigned int)(n) % __base;		\
		(n) = (unsigned int)(n) / __base;		\
	} else						\
		__rem = __div64_32(&(n), __base);	\
	__rem;						\
 })


#endif
