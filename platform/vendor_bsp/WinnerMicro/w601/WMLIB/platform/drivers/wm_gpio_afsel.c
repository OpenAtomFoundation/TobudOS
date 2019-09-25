/**
 * @file wm_gpio.c
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_irq.h"
//#include "wm_osal.h"
#include "tls_common.h"
#include "wm_gpio_afsel.h"
#include "wm_debug.h"


void wm_hspi_gpio_config(uint8_t numsel)
{
    switch(numsel)
    {
		case 0:
			tls_io_cfg_set(WM_IO_PB_14, WM_IO_OPTION1);
			tls_io_cfg_set(WM_IO_PB_15, WM_IO_OPTION1);
			tls_io_cfg_set(WM_IO_PB_16, WM_IO_OPTION1);
			tls_io_cfg_set(WM_IO_PB_17, WM_IO_OPTION1);
			tls_io_cfg_set(WM_IO_PB_18, WM_IO_OPTION1);
			break;
			
		case 1:
			tls_io_cfg_set(WM_IO_PB_08, WM_IO_OPTION2);
			tls_io_cfg_set(WM_IO_PB_09, WM_IO_OPTION2);
			tls_io_cfg_set(WM_IO_PB_10, WM_IO_OPTION2);
			tls_io_cfg_set(WM_IO_PB_11, WM_IO_OPTION2);
			tls_io_cfg_set(WM_IO_PB_12, WM_IO_OPTION2);
			break;
			
		default:
			TLS_DBGPRT_ERR("highspeed spi gpio config error!");
			break;
    }
}

void wm_spi_ck_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PA_01:
			tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_16:
			tls_io_cfg_set(WM_IO_PB_16, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_27:
			tls_io_cfg_set(WM_IO_PB_27, WM_IO_OPTION1);			
			break;	

		case WM_IO_PA_11:
			tls_io_cfg_set(WM_IO_PA_11, WM_IO_OPTION4);
			break;	
		
		default:
			TLS_DBGPRT_ERR("spi ck afsel config error!");
			break;
    }
}

void wm_spi_cs_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PA_02:
			tls_io_cfg_set(WM_IO_PA_02, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_15:
			tls_io_cfg_set(WM_IO_PB_15, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_00:
			tls_io_cfg_set(WM_IO_PB_00, WM_IO_OPTION1);			
			break;	

		case WM_IO_PB_07:
			tls_io_cfg_set(WM_IO_PB_07, WM_IO_OPTION4);
			break;

		case WM_IO_PA_12:
			tls_io_cfg_set(WM_IO_PA_12, WM_IO_OPTION4);
			break;
		
		default:
			TLS_DBGPRT_ERR("spi cs afsel config error!");
			break;
    }
}


void wm_spi_di_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PA_03:
			tls_io_cfg_set(WM_IO_PA_03, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_17:
			tls_io_cfg_set(WM_IO_PB_17, WM_IO_OPTION3);			
			break;	

		case WM_IO_PB_01:
			tls_io_cfg_set(WM_IO_PB_01, WM_IO_OPTION1);			
			break;	

		case WM_IO_PA_05:
			tls_io_cfg_set(WM_IO_PA_05, WM_IO_OPTION3);
			break;

		case WM_IO_PA_10:
			tls_io_cfg_set(WM_IO_PA_10, WM_IO_OPTION4);
			break;
		
		default:
			TLS_DBGPRT_ERR("spi di afsel config error!");
			break;
    }
}

void wm_spi_do_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PB_18:
			tls_io_cfg_set(WM_IO_PB_18, WM_IO_OPTION3);
			break;	

		case WM_IO_PB_02:
			tls_io_cfg_set(WM_IO_PB_02, WM_IO_OPTION1);			
			break;	

		case WM_IO_PA_04:
			tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION3);
			break;	

		case WM_IO_PA_09:
			tls_io_cfg_set(WM_IO_PA_09, WM_IO_OPTION4);
			break;

		case WM_IO_PA_10:
			tls_io_cfg_set(WM_IO_PA_10, WM_IO_OPTION4);
			break;
		
		default:
			TLS_DBGPRT_ERR("spi do afsel config error!");
			break;
    }
}

void wm_sdio_config(uint8_t numsel)
{
	switch(numsel)
	{
		case 0:
			tls_io_cfg_set(WM_IO_PB_08, WM_IO_OPTION3);
			tls_io_cfg_set(WM_IO_PB_09, WM_IO_OPTION3);
			tls_io_cfg_set(WM_IO_PB_10, WM_IO_OPTION3);
			tls_io_cfg_set(WM_IO_PB_11, WM_IO_OPTION3);
			tls_io_cfg_set(WM_IO_PB_12, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("sdio afsel config error!");
			break;
	}
}

void wm_sdio_cmd_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_06:
			tls_io_cfg_set(WM_IO_PA_06, WM_IO_OPTION3);
			break;
			
		case WM_IO_PB_07:
			tls_io_cfg_set(WM_IO_PB_07, WM_IO_OPTION3);
			break;
			
		case WM_IO_PB_13:
			tls_io_cfg_set(WM_IO_PB_13, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("sdio cmd afsel config error!");
			break;
	}
}

void wm_uart0_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION1);
			break;

		case WM_IO_PA_08:
			tls_io_cfg_set(WM_IO_PA_08, WM_IO_OPTION3);
			break;

		case WM_IO_PB_07:
			tls_io_cfg_set(WM_IO_PB_07, WM_IO_OPTION2);
			break;

		case WM_IO_PB_26:
			tls_io_cfg_set(WM_IO_PB_26, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 tx afsel config error!");
			break;
	}
}

void wm_uart0_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_05:
			tls_io_cfg_set(WM_IO_PA_05, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOA_DATA_PULLEN, 5, 0);
			break;

		case WM_IO_PA_09:
			tls_io_cfg_set(WM_IO_PA_09, WM_IO_OPTION3);
			tls_bitband_write(HR_GPIOA_DATA_PULLEN, 9, 0);
			break;

		case WM_IO_PB_06:
			tls_io_cfg_set(WM_IO_PB_06, WM_IO_OPTION2);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 6, 0);
			break;

		case WM_IO_PB_25:
			tls_io_cfg_set(WM_IO_PB_25, WM_IO_OPTION2);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 25, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 rx afsel config error!");
			break;
	}
}

void wm_uart0_rts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_13:
			tls_io_cfg_set(WM_IO_PA_13, WM_IO_OPTION1);
			break;

		case WM_IO_PB_04:
			tls_io_cfg_set(WM_IO_PB_04, WM_IO_OPTION2);
			break;

		case WM_IO_PB_24:
			tls_io_cfg_set(WM_IO_PB_24, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 rts afsel config error!");
			break;
	}
}

void wm_uart0_cts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_14:
			tls_io_cfg_set(WM_IO_PA_14, WM_IO_OPTION1);
			break;

		case WM_IO_PB_05:
			tls_io_cfg_set(WM_IO_PB_05, WM_IO_OPTION2);
			break;

		case WM_IO_PB_23:
			tls_io_cfg_set(WM_IO_PB_23, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 cts afsel config error!");
			break;
	}
}

void wm_uart1_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_12:
			tls_io_cfg_set(WM_IO_PB_12, WM_IO_OPTION1);
			break;

		case WM_IO_PB_18:
			tls_io_cfg_set(WM_IO_PB_18, WM_IO_OPTION4);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 tx afsel config error!");
			break;
	}
}

void wm_uart1_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_11:
			tls_io_cfg_set(WM_IO_PB_11, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 11, 0);
			break;

		case WM_IO_PB_17:
			tls_io_cfg_set(WM_IO_PB_17, WM_IO_OPTION4);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 17, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 rx afsel config error!");
			break;
	}
}

void wm_uart1_rts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_10:
			tls_io_cfg_set(WM_IO_PB_10, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 rts afsel config error!");
			break;
	}
}

void wm_uart1_cts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_09:
			tls_io_cfg_set(WM_IO_PB_09, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 cts afsel config error!");
			break;
	}
}

void wm_uart2_tx_scio_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_01:
			tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION1);
			break;

		case WM_IO_PA_11:
			tls_io_cfg_set(WM_IO_PA_11, WM_IO_OPTION3);
			break;

		case WM_IO_PB_02:
			tls_io_cfg_set(WM_IO_PB_02, WM_IO_OPTION2);
			break;

		case WM_IO_PB_20:
			tls_io_cfg_set(WM_IO_PB_20, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 tx afsel config error!");
			break;
	}
}

void wm_uart2_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_00:
			tls_io_cfg_set(WM_IO_PA_00, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOA_DATA_PULLEN, 0, 0);
			break;

		case WM_IO_PA_10:
			tls_io_cfg_set(WM_IO_PA_10, WM_IO_OPTION3);
			tls_bitband_write(HR_GPIOA_DATA_PULLEN, 10, 0);
			break;

		case WM_IO_PB_03:
			tls_io_cfg_set(WM_IO_PB_03, WM_IO_OPTION2);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 3, 0);
			break;

		case WM_IO_PB_19:
			tls_io_cfg_set(WM_IO_PB_19, WM_IO_OPTION2);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 19, 0);
			break;

		case WM_IO_PB_30:
			tls_io_cfg_set(WM_IO_PB_30, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 30, 0);
			break;			

		default:
			TLS_DBGPRT_ERR("uart2 rx afsel config error!");
			break;
	}
}

void wm_uart2_rts_scclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_02:
			tls_io_cfg_set(WM_IO_PA_02, WM_IO_OPTION1);
			break;

		case WM_IO_PA_12:
			tls_io_cfg_set(WM_IO_PA_12, WM_IO_OPTION3);
			break;

		case WM_IO_PB_01:
			tls_io_cfg_set(WM_IO_PB_01, WM_IO_OPTION2);
			break;

		case WM_IO_PB_21:
			tls_io_cfg_set(WM_IO_PB_21, WM_IO_OPTION2);
			break;

		case WM_IO_PB_06:
			tls_io_cfg_set(WM_IO_PB_06, WM_IO_OPTION4);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 rts afsel config error!");
			break;
	}
}

void wm_uart2_cts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_03:
			tls_io_cfg_set(WM_IO_PA_03, WM_IO_OPTION1);
			break;

		case WM_IO_PB_28:
			tls_io_cfg_set(WM_IO_PB_28, WM_IO_OPTION2);
			break;

		case WM_IO_PB_00:
			tls_io_cfg_set(WM_IO_PB_00, WM_IO_OPTION2);
			break;

		case WM_IO_PB_22:
			tls_io_cfg_set(WM_IO_PB_22, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 cts afsel config error!");
			break;
	}
}

void wm_i2s_m_ck_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION4);
			break;
			
		case WM_IO_PA_08:
			tls_io_cfg_set(WM_IO_PA_08, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_08:
			tls_io_cfg_set(WM_IO_PB_08, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s master ck afsel config error!");
			break;
		}
}

void wm_i2s_m_ws_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_13:
			tls_io_cfg_set(WM_IO_PA_13, WM_IO_OPTION2);
			break;
			
		case WM_IO_PA_09:
			tls_io_cfg_set(WM_IO_PA_09, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_10:
			tls_io_cfg_set(WM_IO_PB_10, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s master ws afsel config error!");
			break;
		}
}

void wm_i2s_m_do_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_03:
			tls_io_cfg_set(WM_IO_PA_03, WM_IO_OPTION4);
			break;
			
		case WM_IO_PA_07:
			tls_io_cfg_set(WM_IO_PA_07, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_09:
			tls_io_cfg_set(WM_IO_PB_09, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s master do afsel config error!");
			break;
		}
}

void wm_i2s_s_ck_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_15:
			tls_io_cfg_set(WM_IO_PA_15, WM_IO_OPTION2);
			break;
			
		case WM_IO_PA_11:
			tls_io_cfg_set(WM_IO_PA_11, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_15:
			tls_io_cfg_set(WM_IO_PB_15, WM_IO_OPTION4);
			break;

		case WM_IO_PB_04:
			tls_io_cfg_set(WM_IO_PB_04, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s slave ck afsel config error!");
			break;
		}
}

void wm_i2s_s_ws_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_06:
			tls_io_cfg_set(WM_IO_PA_06, WM_IO_OPTION2);
			break;
			
		case WM_IO_PA_12:
			tls_io_cfg_set(WM_IO_PA_12, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_16:
			tls_io_cfg_set(WM_IO_PB_16, WM_IO_OPTION4);
			break;

		case WM_IO_PB_05:
			tls_io_cfg_set(WM_IO_PB_05, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s slave ws afsel config error!");
			break;
		}
}

void wm_i2s_s_di_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_14:
			tls_io_cfg_set(WM_IO_PA_14, WM_IO_OPTION2);
			break;
			
		case WM_IO_PA_10:
			tls_io_cfg_set(WM_IO_PA_10, WM_IO_OPTION1);
			break;
			
		case WM_IO_PB_14:
			tls_io_cfg_set(WM_IO_PB_14, WM_IO_OPTION4);
			break;

		case WM_IO_PB_03:
			tls_io_cfg_set(WM_IO_PB_03, WM_IO_OPTION4);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s slave di afsel config error!");
			break;
		}
}

void wm_i2s_mclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_02:
			tls_io_cfg_set(WM_IO_PA_02, WM_IO_OPTION4);
			break;

		case WM_IO_PB_28:
			tls_io_cfg_set(WM_IO_PB_28, WM_IO_OPTION1);
			break;

		case WM_IO_PB_31:
			tls_io_cfg_set(WM_IO_PB_31, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("i2s mclk afsel config error!");
			break;		
	}
}

void wm_i2s_extclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_05:
			tls_io_cfg_set(WM_IO_PA_05, WM_IO_OPTION4);
			break;

		default:
			TLS_DBGPRT_ERR("i2s extclk afsel config error!");
			break;		
	}
}

void wm_i2c_scl_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_06:
			tls_io_cfg_set(WM_IO_PA_06, WM_IO_OPTION1);
			break;

		case WM_IO_PA_08:
			tls_io_cfg_set(WM_IO_PA_08, WM_IO_OPTION4);
			break;

		case WM_IO_PB_13:
			tls_io_cfg_set(WM_IO_PB_13, WM_IO_OPTION3);
			break;

		case WM_IO_PB_11:
			tls_io_cfg_set(WM_IO_PB_11, WM_IO_OPTION4);
			break;

		case WM_IO_PB_21:
			tls_io_cfg_set(WM_IO_PB_21, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("i2c scl afsel config error!");
			break;
	}
}

void wm_i2c_sda_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_15:
			tls_io_cfg_set(WM_IO_PA_15, WM_IO_OPTION1);
			break;

		case WM_IO_PA_07:
			tls_io_cfg_set(WM_IO_PA_07, WM_IO_OPTION4);
			break;

		case WM_IO_PB_14:
			tls_io_cfg_set(WM_IO_PB_14, WM_IO_OPTION3);
			break;

		case WM_IO_PB_12:
			tls_io_cfg_set(WM_IO_PB_12, WM_IO_OPTION4);
			break;

		case WM_IO_PB_22:
			tls_io_cfg_set(WM_IO_PB_22, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("i2c sda afsel config error!");
			break;
	}
}

void wm_pwm1_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_00:
			tls_io_cfg_set(WM_IO_PA_00, WM_IO_OPTION2);
			break;

		case WM_IO_PB_18:
			tls_io_cfg_set(WM_IO_PB_18, WM_IO_OPTION2);
			break;

		case WM_IO_PB_05:
			tls_io_cfg_set(WM_IO_PB_05, WM_IO_OPTION3);
			break;

		case WM_IO_PA_05:
			tls_io_cfg_set(WM_IO_PA_05, WM_IO_OPTION2);
			break;

		case WM_IO_PB_19:
			tls_io_cfg_set(WM_IO_PB_19, WM_IO_OPTION1);
			break;

		case WM_IO_PB_30:
			tls_io_cfg_set(WM_IO_PB_30, WM_IO_OPTION2);
			break;			

		default:
			TLS_DBGPRT_ERR("pwm1 afsel config error!");
			break;
	}
}


void wm_pwm2_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_01:
			tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION2);
			break;

		case WM_IO_PB_17:
			tls_io_cfg_set(WM_IO_PB_17, WM_IO_OPTION2);
			break;

		case WM_IO_PB_04:
			tls_io_cfg_set(WM_IO_PB_04, WM_IO_OPTION3);
			break;

		case WM_IO_PA_07:
			tls_io_cfg_set(WM_IO_PA_07, WM_IO_OPTION2);
			break;

		case WM_IO_PB_13:
			tls_io_cfg_set(WM_IO_PB_13, WM_IO_OPTION1);
			break;

		case WM_IO_PB_20:
			tls_io_cfg_set(WM_IO_PB_20, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm2 afsel config error!");
			break;
	}
}

void wm_pwm3_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_02:
			tls_io_cfg_set(WM_IO_PA_02, WM_IO_OPTION2);
			break;

		case WM_IO_PB_16:
			tls_io_cfg_set(WM_IO_PB_16, WM_IO_OPTION2);
			break;

		case WM_IO_PB_03:
			tls_io_cfg_set(WM_IO_PB_03, WM_IO_OPTION3);
			break;

		case WM_IO_PA_08:
			tls_io_cfg_set(WM_IO_PA_08, WM_IO_OPTION2);
			break;

		case WM_IO_PB_21:
			tls_io_cfg_set(WM_IO_PB_21, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm3 afsel config error!");
			break;
	}
}

void wm_pwm4_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_03:
			tls_io_cfg_set(WM_IO_PA_03, WM_IO_OPTION2);
			break;

		case WM_IO_PB_15:
			tls_io_cfg_set(WM_IO_PB_15, WM_IO_OPTION2);
			break;

		case WM_IO_PB_02:
			tls_io_cfg_set(WM_IO_PB_02, WM_IO_OPTION3);
			break;

		case WM_IO_PA_09:
			tls_io_cfg_set(WM_IO_PA_09, WM_IO_OPTION2);
			break;

		case WM_IO_PB_22:
			tls_io_cfg_set(WM_IO_PB_22, WM_IO_OPTION1);
			break;

		case WM_IO_PB_06:
			tls_io_cfg_set(WM_IO_PB_06, WM_IO_OPTION3);
			break;	

		default:
			TLS_DBGPRT_ERR("pwm4 afsel config error!");
			break;
	}
}

void wm_pwm5_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION2);
			break;

		case WM_IO_PB_14:
			tls_io_cfg_set(WM_IO_PB_14, WM_IO_OPTION2);
			break;

		case WM_IO_PB_01:
			tls_io_cfg_set(WM_IO_PB_01, WM_IO_OPTION3);
			break;

		case WM_IO_PA_10:
			tls_io_cfg_set(WM_IO_PA_10, WM_IO_OPTION2);
			break;

		case WM_IO_PB_23:
			tls_io_cfg_set(WM_IO_PB_23, WM_IO_OPTION1);
			break;

		case WM_IO_PB_08:
			tls_io_cfg_set(WM_IO_PB_08, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm5 afsel config error!");
			break;
	}
}

void wm_pwmbrk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_24:
			tls_io_cfg_set(WM_IO_PB_24, WM_IO_OPTION1);
			break;

		case WM_IO_PB_29:
			tls_io_cfg_set(WM_IO_PB_29, WM_IO_OPTION2);
			break;

		case WM_IO_PB_00:
			tls_io_cfg_set(WM_IO_PB_00, WM_IO_OPTION3);
			break;

		case WM_IO_PA_11:
			tls_io_cfg_set(WM_IO_PA_11, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("pwmbrk afsel config error!");
			break;
	}
}

void wm_swd_config(bool enable)
{
	if (enable)
	{
		tls_io_cfg_set(WM_IO_PB_06, WM_IO_OPTION1);
		tls_io_cfg_set(WM_IO_PB_07, WM_IO_OPTION1);
	}
	else
	{
		tls_io_cfg_set(WM_IO_PB_06, WM_IO_OPTION5);
		tls_io_cfg_set(WM_IO_PB_07, WM_IO_OPTION5);
	}
}

void wm_adc_config(u8 Channel)
{
        switch(Channel)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+Channel), WM_IO_OPTION6);
                break;
            case 8:
            case 9:
            case 10:
            case 11:
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+(Channel-8)*2), WM_IO_OPTION6);
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+(Channel-8)*2+1), WM_IO_OPTION6);
                break;

            default:
                break;                
        }
}

void wm_gpio_af_disable(void)
{
	tls_reg_write32(HR_GPIOA_AFSEL, 0x0);
	tls_reg_write32(HR_GPIOB_AFSEL, BIT(6) | BIT(7));//only enable swd
}

