#include "dwt.h"

static uint32_t cpuclkfeq;

void dwt_delay_init(uint32_t clk)
{
    cpuclkfeq = clk;
    DEM_CR         |=  DEM_CR_TRCENA;
    DWT_CYCCNT      = 0u;
    DWT_CR         |= DWT_CR_CYCCNTENA;
}

void delay_us(uint32_t usec)

{
    uint32_t startts, endts, ts;
	
    startts = DWT_CYCCNT;

    ts =  usec * (cpuclkfeq / (1000 * 1000));
    endts = startts + ts;
    if(endts > startts)
    {
        while(DWT_CYCCNT < endts);
    }
    else
    {
        while(DWT_CYCCNT > endts);
        while(DWT_CYCCNT < endts);
    }
}

void delay_ms(__IO uint32_t nTime)
{
    delay_us(1000 * nTime);
}
