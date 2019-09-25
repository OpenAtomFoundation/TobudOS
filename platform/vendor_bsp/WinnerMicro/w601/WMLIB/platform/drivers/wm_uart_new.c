/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

//#include "esp_common.h"
#include "wm_type_def.h"
#include "wm_uart_new.h"
#include "wm_uart.h"
#include "wm_regs.h"
#include "wm_cpu.h"
#include "wm_gpio_afsel.h"


enum {
    UART_EVENT_RX_CHAR,
    UART_EVENT_MAX
};

typedef struct _os_event_ {
    u32 event;
    u32 param;
} os_event_t;

typedef void(*uart_callback) (void);

uart_callback uart0_callback_func = NULL;
uart_callback uart1_callback_func = NULL;

static  u8
uart_tx_one_char(u8 uart, u8 TxChar)
{
	while (true) { 
		u32 fifo_cnt = (tls_reg_read32(HR_UART0_FIFO_STATUS + uart*0x200) & 0x3F);
		
		if (fifo_cnt < 60) {
            break;
        }
	}
	tls_reg_write32(HR_UART0_TX_WIN + uart*0x200, (char)TxChar);
	return 0;
}

void
uart1_write_char(char c)
{
    if (c == '\n') {
        uart_tx_one_char(UART1, '\r');
        uart_tx_one_char(UART1, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART1, c);
    }
}

void
uart0_write_char(char c)
{
    if (c == '\n') {
        uart_tx_one_char(UART0, '\r');
        uart_tx_one_char(UART0, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART0, c);
    }
}

void
UART_SetWordLength(UART_Port uart_no, UART_WordLength len)
{
	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 0, (len >> 0) & 0x01);
	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 1, (len >> 1) & 0x01);
}

void
UART_SetStopBits(UART_Port uart_no, UART_StopBits bit_num)
{
	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 2, bit_num);
}


void
UART_SetParity(UART_Port uart_no, UART_ParityMode Parity_mode)
{
	
	if (Parity_mode == USART_Parity_None) {
		tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 3, 0);
    } else {
    	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 3, 1);
		tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 4, Parity_mode);
    }
}

void
UART_SetBaudrate(UART_Port uart_no, u32 baud_rate)
{
	u32 value, apbclk;
	tls_sys_clk sysclk;
	
	tls_sys_clk_get(&sysclk);
	apbclk = sysclk.apbclk * 1000000;
    value = (apbclk / (16 * baud_rate) - 1) | (((apbclk % (baud_rate * 16)) * 16 / (baud_rate * 16)) << 16);
	tls_reg_write32(HR_UART0_BAUD_RATE_CTRL + uart_no*0x200, value);
}

//only when USART_HardwareFlowControl_RTS is set , will the rx_thresh value be set.
void
UART_SetFlowCtrl(UART_Port uart_no, UART_HwFlowCtrl flow_ctrl, u8 rx_thresh)
{
	rx_thresh = rx_thresh;
	switch (flow_ctrl)
    {
        case USART_HardwareFlowControl_None:
			tls_reg_write32(HR_UART0_FLOW_CTRL + uart_no*0x200, 0);
            break;
			
        case USART_HardwareFlowControl_RTS:
		case USART_HardwareFlowControl_CTS:
		case USART_HardwareFlowControl_CTS_RTS:
			tls_reg_write32(HR_UART0_FLOW_CTRL + uart_no*0x200, (1UL << 0) | ((rx_thresh/4-1) << 2));
            break;
			
        default:
            break;
    }
}

void
UART_WaitTxFifoEmpty(UART_Port uart_no) //do not use if tx flow control enabled
{
	while(tls_reg_read32(HR_UART0_FIFO_STATUS + uart_no*0x200) & 0x3F);
}

void
UART_ResetFifo(UART_Port uart_no)
{
	tls_bitband_write(HR_UART0_FIFO_CTRL + uart_no*0x200, 0, 1);
	tls_bitband_write(HR_UART0_FIFO_CTRL + uart_no*0x200, 1, 1);
}

void
UART_ClearIntrStatus(UART_Port uart_no, u32 clr_mask)
{
	tls_reg_write32(HR_UART0_INT_SRC + uart_no*0x200, clr_mask);
}

void
UART_SetIntrEna(UART_Port uart_no, u32 ena_mask)
{
	tls_reg_write32(HR_UART0_INT_MASK + uart_no*0x200, ena_mask);
}

void
UART_intr_handler_register(UART_Port uart_no, void *fn, void *arg)
{
    arg = arg;
	if( uart_no==UART0 )
	{
		uart0_callback_func = (uart_callback)fn;
	}
	else if( uart_no==UART1 )
	{
		uart1_callback_func = (uart_callback)fn;
	}
}

//void
//UART_SetPrintPort(UART_Port uart_no)
//{
//    if (uart_no == 1) {
//        os_install_putc1(uart1_write_char);
//    } else {
//        os_install_putc1(uart0_write_char);
//    }
//}

void
UART_ParamConfig(UART_Port uart_no,  UART_ConfigTypeDef *pUARTConfig)
{
    if (uart_no == UART1) {
        wm_uart1_tx_config(WM_IO_PB_12);
		wm_uart1_rx_config(WM_IO_PB_11);
    } else {	
		wm_uart0_tx_config(WM_IO_PA_04);
		wm_uart0_rx_config(WM_IO_PA_05);
    }

    UART_SetFlowCtrl(uart_no, pUARTConfig->flow_ctrl, pUARTConfig->UART_RxFlowThresh);
    UART_SetBaudrate(uart_no, pUARTConfig->baud_rate);
	UART_SetParity(uart_no, pUARTConfig->parity);
	UART_SetStopBits(uart_no, pUARTConfig->stop_bits);
	UART_SetWordLength(uart_no, pUARTConfig->data_bits);
	UART_SetFlowCtrl(uart_no, pUARTConfig->flow_ctrl, 28);
    UART_ResetFifo(uart_no);
}

void
UART_IntrConfig(UART_Port uart_no,  UART_IntrConfTypeDef *pUARTIntrConf)
{
	u32 reg1_val = 0, reg2_val = 0;
    UART_ClearIntrStatus(uart_no, UART_INTR_MASK);
	
	reg1_val = tls_reg_read32(HR_UART0_DMA_CTRL + uart_no*0x200);
	reg1_val |= (((pUARTIntrConf->UART_IntrEnMask & UIS_RX_FIFO_TIMEOUT) == 0) ? (pUARTIntrConf->UART_RX_TimeOutIntrThresh) : 0);
	reg1_val |= (((pUARTIntrConf->UART_IntrEnMask & UIS_RX_FIFO_TIMEOUT) == 0) ? (UDMA_RX_FIFO_TIMEOUT) : 0);
	tls_reg_write32(HR_UART0_DMA_CTRL + uart_no*0x200, reg1_val);
	
	reg2_val = tls_reg_read32(HR_UART0_FIFO_CTRL + uart_no*0x200);
	reg2_val |= (((pUARTIntrConf->UART_IntrEnMask & UIS_RX_FIFO) == 0) ? (pUARTIntrConf->UART_RX_FifoFullIntrThresh) : 0);
    reg2_val |= (((pUARTIntrConf->UART_IntrEnMask & UIS_TX_FIFO) == 0) ? (pUARTIntrConf->UART_TX_FifoEmptyIntrThresh) : 0);
	tls_reg_write32(HR_UART0_FIFO_CTRL + uart_no*0x200, reg2_val);
	
	tls_reg_write32(HR_UART0_INT_MASK + uart_no*0x200, 0);
	tls_reg_write32(HR_UART0_INT_MASK + uart_no*0x200, pUARTIntrConf->UART_IntrEnMask);
	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 6, 1); //tx en
	tls_bitband_write(HR_UART0_LINE_CTRL + uart_no*0x200, 7, 1); //rx en
}



//void UART0_IRQHandler(void)
//{
//	if( uart0_callback_func != NULL )
//	{
//		uart0_callback_func();
//	}
//}

void UART1_IRQHandler(void)
{
	if( uart1_callback_func != NULL )
	{
		uart1_callback_func();
	}
}


void
uart_init_new(void)
{
    UART_WaitTxFifoEmpty(UART0);
    UART_WaitTxFifoEmpty(UART1);

    UART_ConfigTypeDef uart_config;
    uart_config.baud_rate    = BIT_RATE_115200;
    uart_config.data_bits     = UART_WordLength_8b;
    uart_config.parity          = USART_Parity_None;
    uart_config.stop_bits     = USART_StopBits_1;
    uart_config.flow_ctrl      = USART_HardwareFlowControl_None;
    uart_config.UART_RxFlowThresh = 120;
//    uart_config.UART_InverseMask = UART_None_Inverse;
    UART_ParamConfig(UART0, &uart_config);

    UART_IntrConfTypeDef uart_intr;
    uart_intr.UART_IntrEnMask = ~(UIS_RX_FIFO_TIMEOUT | UIS_FRM_ERR | UIS_RX_FIFO | UIS_TX_FIFO_EMPTY);
    uart_intr.UART_RX_FifoFullIntrThresh = UFC_RX_FIFO_LVL_8_BYTE;
    uart_intr.UART_RX_TimeOutIntrThresh = (4 << UDMA_RX_FIFO_TIMEOUT_SHIFT);
    uart_intr.UART_TX_FifoEmptyIntrThresh = UFC_TX_FIFO_LVL_8_BYTE;
    UART_IntrConfig(UART0, &uart_intr);

    //UART_SetPrintPort(UART0);
    
    tls_irq_enable(UART0_INT);

}
