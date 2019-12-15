/**************************************************************************//**
 * @file     wm_i2s.c
 * @author
 * @version
 * @date
 * @brief
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "wm_regs.h"
#include "wm_7816.h"
#include "wm_gpio.h"
#include "wm_uart.h"
#include "wm_cpu.h"

#include "wm_osal.h"

#define DEBUG_7816		1

#if DEBUG_7816
#define PRINT_DEBUG(fmt, args...)   do{(printf("[DEBUG] "), printf(fmt, ##args));}while(0)
#else
#define PRINT_DEBUG(fmt, args...)
#endif

#define SYS_CLK		(40000000)

sc_io_map sc_io;

/**
 * @brief
 *	This function is used to config the pin in gpio or 7816 mode for the 7816 power on timing
 * @param[in] mode : 1--gpio mode ; 0--7816 mode	
 * @retval
 */
void wm_sc_io_clk_config(uint8_t mode)
{	
	if (sc_io.initialed == 0)
	{
		printf("error : 7816 io map must init....\r\n");
		return ;
	}
	if (mode)  //gpio mode
	{
		tls_io_cfg_set(sc_io.clk_pin_num, WM_IO_OPTION5);
		tls_io_cfg_set(sc_io.io_pin_num, WM_IO_OPTION5);
		tls_gpio_cfg(sc_io.clk_pin_num, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
		tls_gpio_cfg(sc_io.io_pin_num, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
	}
	else  //7816 mode
	{	
		tls_io_cfg_set(sc_io.clk_pin_num, sc_io.clk_opt);
		tls_io_cfg_set(sc_io.io_pin_num, sc_io.io_opt);
	}
}

/**
 * @brief
 *	This function is used to config the block guard time param in 7816 mode
 * @param[in] bgt : the value of blcok guard time will be set	
 * @retval
 */
void wm_sc_set_bgt(uint8_t bgt)
{
	uint32_t reg;
	bgt = (bgt > 0x1f) ? 0x1F : bgt;
	reg = tls_reg_read32(HR_UART2_LINE_CTRL);
	reg &= ~(0x1f << 11);
	reg |= (bgt << 11);
	tls_reg_write32(HR_UART2_LINE_CTRL, reg);
}

/**
 * @brief
 *	This function is used to config the tx retry count when detect err signal
 * @param[in] count : the value of retry time will be set 7 for max
 * @retval
 */
void wm_sc_tx_retry_times(uint8_t count)
{
	uint32_t reg;
	reg = tls_reg_read32(HR_UART2_LINE_CTRL);
	reg &= ~(0x7 << 16);
	tls_reg_write32(HR_UART2_LINE_CTRL, reg|(count<<16));
	tls_bitband_write(HR_UART2_LINE_CTRL, 23, 1);
}

/**
 * @brief
 *	This function is used to config the rx retry count when detect parity error
 * @param[in] count : the value of retry time will be set 7 for max
 * @retval
 */
void wm_sc_rx_retry_times(uint8_t count)
{
	uint32_t reg;
	reg = tls_reg_read32(HR_UART2_LINE_CTRL);
	reg &= ~(0x7 << 20);
	tls_reg_write32(HR_UART2_LINE_CTRL, reg|(count<<20));
	tls_bitband_write(HR_UART2_LINE_CTRL, 19, 1);	
}

/**
 * @brief
 *	This function is used to config the etu param
 * @param[in] etu : the value of etu will be set
 * @retval
 */
void wm_sc_set_etu(uint16_t etu)
{
	uint32_t reg;
	
	reg = tls_reg_read32(HR_UART2_BAUD_RATE_CTRL);
	reg &= ~ 0xFFFF;
	reg |= etu;
	tls_reg_write32(HR_UART2_BAUD_RATE_CTRL, reg);	
}

/**
 * @brief
 *	This function config the module clock freq
 * @param[in] freq : the value of clock freq
 * @retval
 */
void wm_sc_set_frequency(uint32_t freq)
{
	uint32_t reg;
	uint8_t div;
	tls_sys_clk clk;

	tls_sys_clk_get(&clk);

	div = (clk.apbclk * 1000000 + freq)/(2 * freq) - 1;
	
	reg = tls_reg_read32(HR_UART2_BAUD_RATE_CTRL);
	reg &= ~ 0x3F0000;
	reg |= (div<<16);
	tls_reg_write32(HR_UART2_BAUD_RATE_CTRL, reg);
}

/**
 * @brief
 *	close af to use PB29 and PB30(uart2_rx as default if af is on) as gpio
 * @retval
 */
void wm_sc_powerInit(void)
{
#ifdef WM_SC_PWR_PIN	
	u32 regValue;
	
	// PB29 AF close, used as GPIO
	regValue = tls_reg_read32(HR_GPIOB_AFSEL);
	regValue &= ~0x20000000;
	tls_reg_write32(HR_GPIOB_AFSEL, regValue);

	// PB30 AF close, used as GPIO
	regValue = tls_reg_read32(HR_GPIOB_AFSEL);
	regValue &= ~0x40000000;
	tls_reg_write32(HR_GPIOB_AFSEL, regValue);
#endif	
}

/**
 * @brief
 *	power on the 7816 device if power is controled by GPIO 
 * @retval
 */
void wm_sc_poweron(void)
{
#ifdef WM_SC_PWR_PIN	
	tls_gpio_cfg(WM_SC_PWR_PIN, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
	tls_gpio_write(WM_SC_PWR_PIN, 1);
#endif	
}

/**
 * @brief
 *	power off the 7816 device if power is controled by GPIO 
 * @retval
 */
void wm_sc_poweroff(void)
{
#ifdef WM_SC_PWR_PIN
	tls_gpio_cfg(WM_SC_PWR_PIN, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
	tls_gpio_write(WM_SC_PWR_PIN, 0);
#endif	
}

/**
 * @brief
 *	driver the reset gpio in low level 
 * @retval
 */
void wm_sc_rst_low(void)
{
#ifdef WM_SC_RST_PIN	
	tls_gpio_cfg(WM_SC_RST_PIN, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
	tls_gpio_write(WM_SC_RST_PIN, 0);
#endif
}

/**
 * @brief
 *	driver the reset gpio in high level 
 * @retval
 */
void wm_sc_rst_high(void)
{
#ifdef WM_SC_RST_PIN	
	tls_gpio_cfg(WM_SC_RST_PIN, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
	tls_gpio_write(WM_SC_RST_PIN, 1);
#endif	
}

/**
 * @brief
 *	hotrest the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_hotreset(void)
{
	uint32_t delay = 0xffff;
	
	/* set the rst pin to low */
	wm_sc_rst_low();	
	/* delay */
	while(delay--);
	/* set f/d to default 372 */
	wm_sc_set_etu(WM_SC_DEFAULT_FD);
	/* set the rst pin to high */
	wm_sc_rst_high();
}

/**
 * @brief
 *	colreset the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_colreset(void)
{	
	/* power down */
	wm_sc_poweroff();
	/* select the clk io in gpio mode */
	wm_sc_io_clk_config(1);
	/* reset the clk pin */
	tls_gpio_write(sc_io.clk_pin_num, 0);
	/* reset the io pin */
	tls_gpio_write(sc_io.io_pin_num, 0);
	/* set the ret pin to low */
	wm_sc_rst_low();
	/* power on the card */
	wm_sc_poweron();
	/* config the model in 7816 mode */
	wm_sc_7816_mode(1);
	/* select the clk io pin in 7816 mode */
	wm_sc_io_clk_config(0);
	/* config the output clock freq*/
	wm_sc_set_frequency(5000000);
	/* set the F/D to default (372)*/
	wm_sc_set_etu(WM_SC_DEFAULT_FD);
	/* set the rst pin to high */
	wm_sc_rst_high();
}

/**
 * @brief
 *	deactive the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_deactive(void)
{	
	/* set the rst pin in low level*/
	wm_sc_rst_low();
	/* select the clk and io pin to 7816 mode */
	wm_sc_io_clk_config(0);
	/* disable the output clock */
	wm_sc_clk_enable(0);
	/* select the clk and io pin to gpio mode */
	wm_sc_io_clk_config(1);
	/* set the clk pin to low */
	tls_gpio_write(sc_io.clk_pin_num, 0);
	/* set the io pin to low */
	tls_gpio_write(sc_io.io_pin_num, 0);
	/* set the power pin to low */
	wm_sc_poweroff();
}

