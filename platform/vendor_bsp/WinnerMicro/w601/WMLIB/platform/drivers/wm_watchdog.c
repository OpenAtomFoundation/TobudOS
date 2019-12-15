/**
 * @file    wm_watchdog.c
 *
 * @brief   watchdog Driver Module
 *
 * @author  kevin
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_cpu.h"
#include "wm_watchdog.h"


void WDG_IRQHandler(void)
{
	printf("WDG IRQ\n");
}

/**
 * @brief          This function is used to clear watchdog
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_watchdog_clr(void)
{
	tls_reg_write32(HR_WDG_INT_CLR, 0x01);
//	printf("FEED WDG\n");
}

/**
 * @brief          This function is used to init watchdog
 *
 * @param[in]      usec    microseconds
 *
 * @return         None
 *
 * @note           None
 */
void tls_watchdog_init(u32 usec)
{

	tls_sys_clk sysclk;

	tls_sys_clk_get(&sysclk);
	tls_irq_enable(WATCHDOG_INT);
	
	tls_reg_write32(HR_WDG_LOAD_VALUE, sysclk.apbclk * usec); 		/* 40M dominant frequency: 40 * 10^6 * (usec / 10^6) */
	tls_reg_write32(HR_WDG_CTRL, 0x3);             /* enable irq & reset */
}

/**
 * @brief          This function is used to reset system
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_sys_reset(void)
{
	tls_reg_write32(HR_WDG_LOCK, 0x1ACCE551);
	tls_reg_write32(HR_WDG_LOAD_VALUE, 0x100);
	tls_reg_write32(HR_WDG_CTRL, 0x3);
	tls_reg_write32(HR_WDG_LOCK, 1);
}

