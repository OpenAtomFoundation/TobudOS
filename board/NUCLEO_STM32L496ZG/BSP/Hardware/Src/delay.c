#include "delay.h"
#include "sys.h"	    

#ifdef __cplusplus
extern "C" {
#endif

void delay_us(uint32_t time_us) {
  uint32_t clk = 80;               // CPU 80MHz
  uint32_t ticks = time_us * clk;  // time is us
  uint32_t told = SysTick->VAL;
  uint32_t tnow = told;
  uint32_t tcnt = 0;
  for(; tcnt<ticks; tnow=SysTick->VAL)
  {
    if(tnow != told) {
      if(tnow < told) {
        tcnt += told - tnow;
      } else { 
        tcnt += SysTick->LOAD-tnow + told;
      } told = tnow;
    }
  }
}

void delay_ms(uint32_t time_ms) {
  uint32_t clk = 80;               // CPU 80MHz
  uint32_t ticks = time_ms * clk * 1000;  // time is ms
  uint32_t told = SysTick->VAL;
  uint32_t tnow = told;
  uint32_t tcnt = 0;
  for(; tcnt<ticks; tnow=SysTick->VAL)
  {
    if(tnow != told) {
      if(tnow < told) {
        tcnt += told - tnow;
      } else { 
        tcnt += SysTick->LOAD-tnow + told;
      } told = tnow;
    }
  }
}
#ifdef __cplusplus
}
#endif

	



































