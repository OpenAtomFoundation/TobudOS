/**
 * @file    wm_cpu.c
 *
 * @brief   cpu driver module
 *
 * @author  kevin
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_cpu.h"
#include "wm_pwm.h"

/**
 * @brief          This function is used to set cpu clock
 *
 * @param[in]      	clk    select cpu clock
 *                        	clk == CPU_CLK_80M	80M
 *				clk == CPU_CLK_40M	40M
 *
 * @return         None
 *
 * @note           None
 */
void tls_sys_clk_set(u32 clk)
{
	u32 RegValue;
	u8 wlanDiv, cpuDiv;

	RegValue = tls_reg_read32(HR_CLK_DIV_CTL);
	wlanDiv = (RegValue>>4)&0x0F;
	RegValue &= 0xFFFFF000;
	RegValue |= 0x80000000;
	switch(clk)
	{
		case CPU_CLK_40M:
			cpuDiv = 4;
			break;

		default:
			cpuDiv = 2;
			break;
	}
	RegValue |= ((wlanDiv*4/cpuDiv)<<8) | (wlanDiv<<4) | cpuDiv;
	tls_reg_write32(HR_CLK_DIV_CTL, RegValue);

	return;
}


/**
 * @brief          	This function is used to get cpu clock
 *
 * @param[out]     *sysclk	point to the addr for system clk output
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_sys_clk_get(tls_sys_clk *sysclk)
{
	u32 value;
	value = tls_reg_read32(HR_CLK_DIV_CTL);
	sysclk->apbclk = W600_PLL_CLK_MHZ/4/((value>>4)&0x0F);
	sysclk->cpuclk = W600_PLL_CLK_MHZ/(value&0x0F);
	sysclk->wlanclk = W600_PLL_CLK_MHZ/((value>>4)&0x0F);
}



