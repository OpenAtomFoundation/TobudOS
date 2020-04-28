#ifndef __DWT_H_
#define __DWT_H_

#include "stm32f1xx.h"

#define  DWT_CR      *(volatile uint32_t *)0xE0001000

#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004

#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC

#define  DEM_CR_TRCENA                   (1 << 24)

#define  DWT_CR_CYCCNTENA                (1 <<  0)

#define delayms(msec)         delayus(msec*1000)

void dwt_delay_init(uint32_t clk);
void delay_us(uint32_t usec);
void delay_ms(__IO uint32_t nTime);
#endif
