/*****************************************************************************
*
* File Name : wm_main.c
*
* Description: wm main
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*
* Author :
*
* Date : 2014-6-14
*****************************************************************************/
#include <string.h>
#include "wm_regs.h"
#include "wm_type_def.h"
#include "wm_cpu.h"
#include "wm_io.h"
#include "misc.h"
#include "wm_gpio_afsel.h"

#define FW_MAJOR_VER           0x03
#define FW_MINOR_VER           0x02
#define FW_PATCH_VER           0x00

//const char FirmWareVer[4] = {
//	'G',
//	FW_MAJOR_VER,  /* Main version */
//	FW_MINOR_VER, /* Subversion */
//	FW_PATCH_VER  /* Internal version */
//	};
//const char HwVer[6] = {
//	'H',
//	0x1,
//	0x0,
//	0x0,
//	0x0,
//	0x0
//};
extern const char WiFiVer[];
extern u8 tx_gain_group[];
extern void *tls_wl_init(u8 *tx_gain, u8* mac_addr, u8 *hwver);
extern int wpa_supplicant_init(u8 *mac_addr);
extern void tls_sys_auto_mode_run(void);
extern void tls_spi_slave_sel(u16 slave);
extern void UserMain(void);
extern void tls_fls_layout_init(void);


void Uart0Init(void)
{
//    u32 bd;

//    u32 apbclk;
//    tls_sys_clk sysclk;

//    tls_sys_clk_get(&sysclk);
//    apbclk = sysclk.apbclk * 1000000;

///* baud rate register value = apb_clk/(16*115200) */
///* 如果APB时钟是40MHz， */
///* 波特率寄存器的值设置为 115200 : 21 */
///* 9600bps : 260 */
//    {
//        bd = (apbclk / (16 * 115200) -
//              1) | (((apbclk % (115200 * 16)) * 16 / (115200 * 16)) << 16);
//        tls_reg_write32(HR_UART0_BAUD_RATE_CTRL, bd);
//    /* Line control register : Normal,No parity,1 stop,8 bits, only use tx */
//        tls_reg_write32(HR_UART0_LINE_CTRL, ULCON_WL8 | ULCON_TX_EN);

//    /* disable auto flow control */
//        tls_reg_write32(HR_UART0_FLOW_CTRL, 0);
//    /* disable dma */
//        tls_reg_write32(HR_UART0_DMA_CTRL, 0);
//    /* one byte tx */
//        tls_reg_write32(HR_UART0_FIFO_CTRL, 0);
//    /* disable interrupt */
//        tls_reg_write32(HR_UART0_INT_MASK, 0xFF);
//    }
}
/****************/
/* main program */
/****************/

//void vApplicationIdleHook( void )
//{
//    /* clear watch dog interrupt */

//#if !defined(__CC_ARM)
//        __asm volatile ("wfi");
//#else
//        __WFI();
//#endif

//    return;
//}


void wm_gpio_config()
{
	/* must call first */
	wm_gpio_af_disable();

//	/* UART0_TX-PA04 UART0_RX-PA05 */
//	wm_uart0_tx_config(WM_IO_PA_04);
//	wm_uart0_rx_config(WM_IO_PA_05);

	/* UART1_RX-PB11  UART1_TX-PB12 */	
	wm_uart1_rx_config(WM_IO_PB_11);
	wm_uart1_tx_config(WM_IO_PB_12);	

	/*MASTER SPI configuratioin*/
//	wm_spi_cs_config(WM_IO_PA_02);
//	wm_spi_ck_config(WM_IO_PA_11);
//	wm_spi_di_config(WM_IO_PA_03);
//	wm_spi_do_config(WM_IO_PA_09);
}

//int main(void)
//{
//	tls_sys_clk_set(CPU_CLK_80M);
//	
//	//tls_pmu_clk_select(0);
//	
//	//tls_sys_clk sysclk;
//	
//	//tls_sys_clk_get(&sysclk);
//	//SysTick_Config(sysclk.cpuclk*UNIT_MHZ/500);

//	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//	//NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP, ENABLE);
//	
//    wm_gpio_config();

//    UserMain();
//}

