/*
 * port_c.c
 *
 *  Created on: Sep 9, 2019
 *      Author: ace
 */
#include "riscv_encoding.h"
#include <tos.h>
#define CLINT_CTRL_ADDR 0x2000000
#define CLINT_MSIP		0x0000
#define CLINT_MTIMECMP 	0x4000
#define CLINT_MTIME 	0xBFF8


static uint32_t mtime_lo(void)
{
  return *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME);
}


static uint32_t mtime_hi(void)
{
  return *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME + 4);
}

uint64_t get_mtime_val()
{
  while (1) {
    uint32_t hi = mtime_hi();
    uint32_t lo = mtime_lo();
    uint32_t nhi = mtime_hi();
    if (hi == nhi) {
      return (((uint64_t)hi) << 32) | lo;
    } {
    	uint32_t a = hi;
    	uint32_t b = nhi;
    }
  }
}

void set_mtimecmp_lo(uint32_t v)
{
	*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP) = v;
}

void set_mtimecmp_hi(uint32_t v)
{
	*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 4) = v;
}

void set_mtimecmp_val(uint64_t v)
{
    uint32_t hi = (v >> 32) & 0xFFFFFFFF;
    uint32_t lo = (v >>  0) & 0xFFFFFFFF;
    set_mtimecmp_lo(0xFFFFFFFF); // No smaller than old value.
    set_mtimecmp_hi(hi);         // No smaller than new value.
    set_mtimecmp_lo(lo);         // New value.
}



__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
#if 0
	asm("csrc mie, %0"::"r"(MIP_MTIP));

#if 1
	uint64_t mtime = get_mtime_val();
#else
	static uint64_t last_mtime = 0;
	if(last_mtime == 0) {
		last_mtime = get_mtime_val();
	}
	uint64_t mtime = get_mtime_val();
	if((mtime - last_mtime)/cycle_per_tick >= 4) {
		mtime = last_mtime+2*cycle_per_tick;
	}
	last_mtime = mtime;
#endif
	uint64_t mtimecmp;
	do {
		tick_inc();
		mtimecmp = ((uint64_t)tos_get_tick())* cycle_per_tick;
	} while(mtimecmp <= mtime);

	set_mtimecmp_val(mtimecmp);

	asm("csrs mie, %0"::"r"(MIP_MTIP));
#endif
#if 0
	  clear_csr(mie, MIP_MTIP);
	  static uint64_t then = 0;

	  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
	  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
	  if(then != 0)  {
	    //next timer irq is 1 second from previous
	    then += 1*cycle_per_tick;
	  } else{ //first time setting the timer
	    uint64_t now = *mtime;
	    then = now + 1*cycle_per_tick;
	  }
	  *mtimecmp = then;

	  set_csr(mie, MIP_MTIP);
#endif
	//asm("csrc mie, %0"::"r"(MIP_MTIP));
#if 0
	uint64_t next_tick = get_mtime_val();
	next_tick += cycle_per_tick;
	set_mtimecmp_val(next_tick);
#endif

#if 0
	uint64_t new_tick = get_mtime_val() + cycle_per_tick;
	uint64_t tick = 0;
	if(new_tick - next_tick > 50000) {
		tick = get_mtime_val() + cycle_per_tick;
	}
	next_tick = new_tick;
	set_mtimecmp_val(next_tick);
#endif

#if 1
	// this is illegal in ricsv-32
	// it cost cpu read two times, first mtime_lo and then mtime_hi
	// if mtime_lo == 0xFFFFFFFF and mtime_hi = 0 at first read
	// then mtime_lo == 0 and mtime_hi = 1 at next read
	// the result will be  0x1FFFFFFFF, not 0x100000000
	//*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 4) = 0xFFFFFFFF;
	//*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 0) = 0xFFFFFFFF;

	uint64_t mtime = *(volatile uint64_t *)(CLINT_CTRL_ADDR + CLINT_MTIME);

	mtime += cycle_per_tick;

	*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 4) = (mtime >> 32) & 0xFFFFFFFF;
	asm("csrc mip, %0"::"r"(MIP_MTIP));
	*(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 0) = (mtime >>  0) & 0xFFFFFFFF;
	//asm("csrc mip, %0"::"r"(MIP_MTIP));
#endif
	//asm("csrs mie, %0"::"r"(MIP_MTIP));
}

__PORT__ void port_systick_priority_set(uint32_t prio)
{
    //NVIC_SetPriority(SysTick_IRQn, prio);
}
