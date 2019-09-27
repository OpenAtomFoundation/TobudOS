
#ifndef BITOPS_H
#define BITOPS_H

#include "wm_osal.h"

/*
 * These functions are the basis of our bit ops.
 *
 * First, the atomic bitops. These use native endian.
 */
static __inline void set_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	*p |= mask;
	tls_os_release_critical(cpu_sr);
}

static __inline void clear_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	*p &= ~mask;
	tls_os_release_critical(cpu_sr);
}

static __inline void change_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	*p ^= mask;
	tls_os_release_critical(cpu_sr);
}

static __inline int
test_and_set_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned int res;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	res = *p;
	*p = res | mask;
	tls_os_release_critical(cpu_sr);

	return res & mask;
}

static __inline int
test_and_clear_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned int res;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	res = *p;
	*p = res & ~mask;
	tls_os_release_critical(cpu_sr);

	return res & mask;
}

static __inline int
test_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned int res;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	res = *p;
	tls_os_release_critical(cpu_sr);

	return res & mask;
}

static __inline int
test_and_change_bit(unsigned int bit, volatile unsigned long *p)
{
	u32  cpu_sr;
	unsigned int res;
	unsigned long mask = 1 << (bit & 31);

	p += bit >> 5;

	cpu_sr = tls_os_set_critical();
	res = *p;
	*p = res ^ mask;
	tls_os_release_critical(cpu_sr);

	return res & mask;
}


#endif /* BITOPS_H */
