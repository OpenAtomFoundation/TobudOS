/**
 * @file wm_gpio_afsel.h
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_IO_MUX_H
#define WM_IO_MUX_H

#ifdef __cplusplus
extern "C" {
#endif
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "tls_common.h"

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup IOMUX_Driver_APIs IOMUX Driver APIs
 * @brief IOMUX driver APIs
 */

/**
 * @addtogroup IOMUX_Driver_APIs
 * @{
 */

/**
 * @brief  config the pins used for highspeed spi
 * @param  numsel: config highspeed spi pins multiplex relation,valid para 0,1
 *			 0: hspi0
 *			    hspi_int PB14
 *				hspi_cs  PB15
 *			    hspi_ck  PB16
 *			    hspi_di  PB17
 *				hspi_do	 PB18
 *			 1: hspi1
 *			    hspi_int PB08
 *				hspi_cs  PB09
 *			    hspi_ck  PB10
 *			    hspi_di  PB11
 *				hspi_do	 PB12
 * @return None
 */
void wm_hspi_gpio_config(uint8_t numsel);

/**
 * @brief  config the pins used for spi ck
 * @param  io_name: config spi ck pins name
 *			WM_IO_PA_01			 
 *			WM_IO_PB_16
 *			WM_IO_PB_27
 *			WM_IO_PA_11		    
 *				
 * @return None
 */
void wm_spi_ck_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi cs
 * @param  io_name: config spi cs pins name
 *			WM_IO_PA_02			 
 *			WM_IO_PB_15
 *			WM_IO_PB_00
 *			WM_IO_PB_07
 *			WM_IO_PA_12
 *				
 * @return None
 */
void wm_spi_cs_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi di
 * @param  io_name: config spi di pins name
 *			WM_IO_PA_03			 
 *			WM_IO_PB_17
 *			WM_IO_PB_01
 *			WM_IO_PA_05
 *			WM_IO_PA_10
 *				
 * @return None
 */
void wm_spi_di_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi do
 * @param  io_name: config spi do pins name
 *			WM_IO_PB_18
 *			WM_IO_PB_02
 *			WM_IO_PB_04
 *			WM_IO_PA_09
 *			WM_IO_PA_10
 *				
 * @return None
 */
void wm_spi_do_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for sdio ck dat0 dat1 dat2 dat3
 * @param  numsel: config sdio ck dat0 dat1 dat2 dat3 pins multiplex relation,valid para 0
 *			0: 
 *			  sdio_ck   PB08
 *			  sdio_dat0 PB09
 *			  sdio_dat1 PB10
 *			  sdio_dat2 PB11
 *			  sdio_dat3 PB12
 *				
 * @return None
 */
void wm_sdio_config(uint8_t numsel);

/**
 * @brief  config the pins used for sdio cmd
 * @param  io_name: config sdio cmd pins name
 *			WM_IO_PA_06			 
 *			WM_IO_PB_07
 *			WM_IO_PB_13
 *				
 * @return None
 */
void wm_sdio_cmd_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart0 tx
 * @param  io_name: config uart0 tx pins name
 *			WM_IO_PA_04			 
 *			WM_IO_PA_08
 *			WM_IO_PB_07
 *			WM_IO_PB_26
 *				
 * @return None
 */
void wm_uart0_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart0 rx
 * @param  io_name: config uart0 rx pins name
 *			WM_IO_PA_05			 
 *			WM_IO_PA_09
 *			WM_IO_PB_06
 *			WM_IO_PB_25
 *				
 * @return None
 */
void wm_uart0_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart0 rts
 * @param  io_name: config uart0 rts pins name
 *			WM_IO_PA_13			 
 *			WM_IO_PB_04
 *			WM_IO_PB_24			
 *				
 * @return None
 */
void wm_uart0_rts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart0 cts
 * @param  io_name: config uart0 cts pins name
 *			WM_IO_PA_14			 
 *			WM_IO_PB_05
 *			WM_IO_PB_23			
 *				
 * @return None
 */
void wm_uart0_cts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 tx
 * @param  io_name: config uart1 tx pins name
 *			WM_IO_PB_12		 
 *			WM_IO_PB_18
 *				
 * @return None
 */
void wm_uart1_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 rx
 * @param  io_name: config uart1 rx pins name
 *			WM_IO_PB_11		 
 *			WM_IO_PB_17
 *				
 * @return None
 */
void wm_uart1_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 rts
 * @param  io_name: config uart1 rts pins name
 *			WM_IO_PB_10			
 *				
 * @return None
 */
void wm_uart1_rts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 cts
 * @param  io_name: config uart1 cts pins name
 *			WM_IO_PB_09			
 *				
 * @return None
 */
void wm_uart1_cts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 tx or 7816-io
 * @param  io_name: config uart2 tx or 7816-io pins name
 *			WM_IO_PA_01	 
 *			WM_IO_PA_11
 *			WM_IO_PB_02
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_uart2_tx_scio_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 rx
 * @param  io_name: config uart2 rx pins name
 *			WM_IO_PA_00		 
 *			WM_IO_PA_10
 *			WM_IO_PB_03
 *			WM_IO_PB_19
 *			WM_IO_PB_30
 *				
 * @return None
 */
void wm_uart2_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 rts or 7816-clk
 * @param  io_name: config uart2 rts or 7816-clk pins name
 *			WM_IO_PA_02	 
 *			WM_IO_PA_12
 *			WM_IO_PB_01
 *			WM_IO_PB_21
 *			WM_IO_PB_06
 *				
 * @return None
 */
void wm_uart2_rts_scclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 cts
 * @param  io_name: config uart2 cts pins name
 *			WM_IO_PA_03	 
 *			WM_IO_PB_28
 *			WM_IO_PB_00
 *				
 * @return None
 */
void wm_uart2_cts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s master ck
 * @param  io_name: config i2s master ck pins name
 *			WM_IO_PA_04	 
 *			WM_IO_PA_08
 *			WM_IO_PB_08
 *				
 * @return None
 */
void wm_i2s_m_ck_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s master ws
 * @param  io_name: config i2s master ws pins name
 *			WM_IO_PA_13	 
 *			WM_IO_PA_09
 *			WM_IO_PB_10
 *				
 * @return None
 */
void wm_i2s_m_ws_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s master do
 * @param  io_name: config i2s master do pins name
 *			WM_IO_PA_03	 
 *			WM_IO_PA_07
 *			WM_IO_PB_09
 *				
 * @return None
 */
void wm_i2s_m_do_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s slave ck
 * @param  io_name: config i2s slave ck pins name
 *			WM_IO_PA_15	 
 *			WM_IO_PA_11
 *			WM_IO_PB_15
 *			WM_IO_PB_04 
 *				
 * @return None
 */
void wm_i2s_s_ck_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s slave ws
 * @param  io_name: config i2s slave ws pins name
 *			WM_IO_PA_06	 
 *			WM_IO_PA_12
 *			WM_IO_PB_16
 *			WM_IO_PB_05
 *				
 * @return None
 */
void wm_i2s_s_ws_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s slave di
 * @param  io_name: config i2s slave di pins name
 *			WM_IO_PA_14	 
 *			WM_IO_PA_10
 *			WM_IO_PB_14
 *			WM_IO_PB_03
 *				
 * @return None
 */
void wm_i2s_s_di_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s mclk
 * @param  io_name: config i2s mclk pins name
 *			WM_IO_PA_02	 
 *			WM_IO_PB_28
 *			WM_IO_PB_31
 *				
 * @return None
 */
void wm_i2s_mclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s extclk
 * @param  io_name: config i2s extclk pins name
 *			WM_IO_PA_05
 *				
 * @return None
 */
void wm_i2s_extclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2c scl
 * @param  io_name: config i2c scl pins name
 *			WM_IO_PA_06
 *			WM_IO_PA_08
 *			WM_IO_PB_13
 *			WM_IO_PB_11
 *			WM_IO_PB_21
 *				
 * @return None
 */
void wm_i2c_scl_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2c sda
 * @param  io_name: config i2c sda pins name
 *			WM_IO_PA_15
 *			WM_IO_PA_07
 *			WM_IO_PB_14
 *			WM_IO_PB_12
 *			WM_IO_PB_22
 *				
 * @return None
 */
void wm_i2c_sda_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm1
 * @param  io_name: config pwm1 pins name
 *			WM_IO_PA_00
 *			WM_IO_PB_18
 *			WM_IO_PB_05
 *			WM_IO_PA_05
 *			WM_IO_PB_19
 *			WM_IO_PB_30
 *				
 * @return None
 */
void wm_pwm1_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm1
 * @param  io_name: config pwm1 pins name
 *			WM_IO_PA_01
 *			WM_IO_PB_11
 *			WM_IO_PB_04
 *			WM_IO_PA_07
 *			WM_IO_PB_13
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_pwm2_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm3
 * @param  io_name: config pwm3 pins name
 *			WM_IO_PA_02
 *			WM_IO_PB_16
 *			WM_IO_PB_03
 *			WM_IO_PA_08
 *			WM_IO_PB_21
 *				
 * @return None
 */
void wm_pwm3_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm4
 * @param  io_name: config pwm4 pins name
 *			WM_IO_PA_03
 *			WM_IO_PB_15
 *			WM_IO_PB_02
 *			WM_IO_PA_09
 *			WM_IO_PB_22
 *				
 * @return None
 */
void wm_pwm4_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm5
 * @param  io_name: config pwm5 pins name
 *			WM_IO_PA_04
 *			WM_IO_PB_14
 *			WM_IO_PB_01
 *			WM_IO_PA_10
 *			WM_IO_PB_23
 *				
 * @return None
 */
void wm_pwm5_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm break
 * @param  io_name: config pwm break pins name
 *			WM_IO_PB_24
 *			WM_IO_PB_29
 *			WM_IO_PB_00
 *			WM_IO_PA_11
 *				
 * @return None
 */
void wm_pwmbrk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for swd
 * @param  enable: enable or disable chip swd function
 *			1: enable
 *			0: disable
 *				
 * @return None
 */
void wm_swd_config(bool enable);

/**
 * @brief  config the pins used for adc
 * @param  Channel: the channel that shall be used
 *			0~07: single-ended input
 *			8~11: differential input
 *				
 * @return None
 */
void wm_adc_config(u8 Channel);

/**
 * @brief  disable all the gpio af
 *				
 * @return None
 *
 * @note  This function must call before anyothers for configure 
 * 		  gpio Alternate functions
 */
void wm_gpio_af_disable(void);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of WM_IO_MUX_H */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/

