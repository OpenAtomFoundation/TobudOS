/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */

#include <string.h>

#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "arc/arc_exception.h"

#include "device/designware/dw_i2s_hal.h"
#include "device/designware/dw_i2s.h"

/** check expressions used in DesignWare I2S driver implementation */
#define DW_I2S_CHECK_EXP(EXPR, ERROR_CODE)      CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#define DW_I2S_CHANNEL_CHECK(val, channels) (((val == DW_I2S_CHANNEL0) && (channels & DW_I2S_CHANNEL0_SUPPORTED)) || \
					     ((val == DW_I2S_CHANNEL1) && (channels & DW_I2S_CHANNEL1_SUPPORTED)) || \
					     ((val == DW_I2S_CHANNEL2) && (channels & DW_I2S_CHANNEL2_SUPPORTED)) || \
					     ((val == DW_I2S_CHANNEL3) && (channels & DW_I2S_CHANNEL3_SUPPORTED)))

#ifndef DISABLE_DEVICE_OBJECT_VALID_CHECK
/** valid check of I2S info object */
#define VALID_CHK_I2S_INFO_OBJECT(i2sinfo_obj_ptr)      {			\
		DW_I2S_CHECK_EXP((i2sinfo_obj_ptr) != NULL, E_OBJ);		\
		DW_I2S_CHECK_EXP(((i2sinfo_obj_ptr)->i2s_ctrl) != NULL, E_OBJ);	\
}
#endif

/**
 * @name	DesignWare I2S Interrupt Callback Routine Select Marcos
 * @brief	DesignWare I2S interrupt callback routines select macros definitions
 * @{
 */
#define DW_I2S_TX_RDY_SND           (0) /*!< TX ready to send callback */
#define DW_I2S_RX_RDY_RCV           (1) /*!< RX ready to receive callback */

/** @} */

/**
 * @defgroup	DEVICE_DW_I2S_STATIC	DesignWare I2S Driver Static Functions
 * @ingroup	DEVICE_DW_I2S
 * @brief	Static or inline functions, variables for DesignWare I2S handle I2S operations,
 * 	only used in this file.
 * @{
 */

/**
 * @fn void dw_i2s_rx_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Disable DesignWare I2S RX and flush all FIFOs
 */
Inline void dw_i2s_rx_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->IER &= ~DW_I2S_IER_IEN;
}

/**
 * @fn void dw_i2s_rx_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Enable DesignWare I2S RX IP
 */
Inline void dw_i2s_rx_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->IER |= DW_I2S_IER_IEN;
}

/**
 * @fn void dw_i2s_tx_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Disable DesignWare I2S TX and flush all FIFOs
 */
Inline void dw_i2s_tx_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->IER &= ~DW_I2S_IER_IEN;
}

/**
 * @fn void dw_i2s_tx_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Enable DesignWare I2S TX IP
 */
Inline void dw_i2s_tx_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->IER |= DW_I2S_IER_IEN;
}

/**
 * @fn void dw_i2s_rx_block_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Disable DesignWare I2S RX block
 */
Inline void dw_i2s_rx_block_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->IRER &= ~DW_I2S_IRER_RXEN;
}

/**
 * @fn void dw_i2s_rx_block_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Enable DesignWare I2S RX block
 */
Inline void dw_i2s_rx_block_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->IRER |= DW_I2S_IRER_RXEN;
}

/**
 * @fn void dw_i2s_tx_block_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Disable DesignWare I2S TX block
 */
Inline void dw_i2s_tx_block_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->ITER &= ~DW_I2S_ITER_TXEN;
}

/**
 * @fn void dw_i2s_tx_block_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Enable DesignWare I2S TX block
 */
Inline void dw_i2s_tx_block_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->ITER |= DW_I2S_ITER_TXEN;
}

/**
 * @fn void dw_i2s_rx_channel_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
 * @brief Disable DesignWare I2S RX channel
 */
Inline void dw_i2s_rx_channel_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_rx_reg_ptr->RER0 &= ~DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL1:
		i2s_rx_reg_ptr->RER1 &= ~DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL2:
		i2s_rx_reg_ptr->RER2 &= ~DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL3:
		i2s_rx_reg_ptr->RER3 &= ~DW_I2S_RERX_RXCHENX;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_rx_channel_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
 * @brief Enable DesignWare I2S RX channel
 */
Inline void dw_i2s_rx_channel_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_rx_reg_ptr->RER0 |= DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL1:
		i2s_rx_reg_ptr->RER1 |= DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL2:
		i2s_rx_reg_ptr->RER2 |= DW_I2S_RERX_RXCHENX;
		break;
	case DW_I2S_CHANNEL3:
		i2s_rx_reg_ptr->RER3 |= DW_I2S_RERX_RXCHENX;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_tx_channel_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
 * @brief Disable DesignWare I2S TX channel
 */
Inline void dw_i2s_tx_channel_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_reg_ptr->TER0 &= ~DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL1:
		i2s_tx_reg_ptr->TER1 &= ~DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL2:
		i2s_tx_reg_ptr->TER2 &= ~DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL3:
		i2s_tx_reg_ptr->TER3 &= ~DW_I2S_TERX_TXCHENX;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_tx_channel_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
 * @brief Enable DesignWare I2S TX channel
 */
Inline void dw_i2s_tx_channel_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
{
	uint32_t tar_channel = channel;

	switch (tar_channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_reg_ptr->TER0 |= DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL1:
		i2s_tx_reg_ptr->TER1 |= DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL2:
		i2s_tx_reg_ptr->TER2 |= DW_I2S_TERX_TXCHENX;
		break;
	case DW_I2S_CHANNEL3:
		i2s_tx_reg_ptr->TER3 |= DW_I2S_TERX_TXCHENX;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_rx_clear_overrun_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
 * @brief Clear DesignWare I2S RX overrun interrupt
 */
Inline void dw_i2s_rx_clear_overrun_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		(void)i2s_rx_reg_ptr->ROR0;
		break;
	case DW_I2S_CHANNEL1:
		(void)i2s_rx_reg_ptr->ROR1;
		break;
	case DW_I2S_CHANNEL2:
		(void)i2s_rx_reg_ptr->ROR2;
		break;
	case DW_I2S_CHANNEL3:
		(void)i2s_rx_reg_ptr->ROR3;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_tx_clear_overrun_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
 * @brief Clear DesignWare I2S TX overrun interrupt
 */
Inline void dw_i2s_tx_clear_overrun_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		(void)i2s_tx_reg_ptr->TOR0;
		break;
	case DW_I2S_CHANNEL1:
		(void)i2s_tx_reg_ptr->TOR1;
		break;
	case DW_I2S_CHANNEL2:
		(void)i2s_tx_reg_ptr->TOR2;
		break;
	case DW_I2S_CHANNEL3:
		(void)i2s_tx_reg_ptr->TOR3;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_rx_unmask_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t mask, uint32_t channel)
 * @brief Enable DesignWare I2S RX bit interrupt with mask
 */
Inline void dw_i2s_rx_unmask_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t mask, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_rx_reg_ptr->IMR0 &= ~mask;
		break;
	case DW_I2S_CHANNEL1:
		i2s_rx_reg_ptr->IMR1 &= ~mask;
		break;
	case DW_I2S_CHANNEL2:
		i2s_rx_reg_ptr->IMR2 &= ~mask;
		break;
	case DW_I2S_CHANNEL3:
		i2s_rx_reg_ptr->IMR3 &= ~mask;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_rx_mask_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t mask, uint32_t channel)
 * @brief Disable DesignWare I2S RX bit interrupt with mask
 */
Inline void dw_i2s_rx_mask_interrupt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t mask, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_rx_reg_ptr->IMR0 |= mask;
		break;
	case DW_I2S_CHANNEL1:
		i2s_rx_reg_ptr->IMR1 |= mask;
		break;
	case DW_I2S_CHANNEL2:
		i2s_rx_reg_ptr->IMR2 |= mask;
		break;
	case DW_I2S_CHANNEL3:
		i2s_rx_reg_ptr->IMR3 |= mask;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_tx_unmask_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t mask, uint32_t channel)
 * @brief Enable DesignWare I2S TX bit interrupt with mask
 */
Inline void dw_i2s_tx_unmask_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t mask, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_reg_ptr->IMR0 &= ~mask;
		break;
	case DW_I2S_CHANNEL1:
		i2s_tx_reg_ptr->IMR1 &= ~mask;
		break;
	case DW_I2S_CHANNEL2:
		i2s_tx_reg_ptr->IMR2 &= ~mask;
		break;
	case DW_I2S_CHANNEL3:
		i2s_tx_reg_ptr->IMR3 &= ~mask;
		break;
	default:
		break;
	}
}

/**
 * @fn void dw_i2s_tx_mask_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t mask, uint32_t channel)
 * @brief Disable DesignWare I2S TX bit interrupt with mask
 */
Inline void dw_i2s_tx_mask_interrupt(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t mask, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_reg_ptr->IMR0 |= mask;
		break;
	case DW_I2S_CHANNEL1:
		i2s_tx_reg_ptr->IMR1 |= mask;
		break;
	case DW_I2S_CHANNEL2:
		i2s_tx_reg_ptr->IMR2 |= mask;
		break;
	case DW_I2S_CHANNEL3:
		i2s_tx_reg_ptr->IMR3 |= mask;
		break;
	default:
		break;
	}
}

/**
 * @fn uint32_t dw_i2s_tx_get_txchet(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
 * @brief Get DesignWare I2S Transmit Channel Empty Trigger (TXCHET)
 */
Inline uint32_t dw_i2s_tx_get_txchet(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t channel)
{
	uint32_t len = 0;

	switch (channel) {
	case DW_I2S_CHANNEL0:
		len = i2s_tx_reg_ptr->TFCR0 & 0x0f;
		break;
	case DW_I2S_CHANNEL1:
		len = i2s_tx_reg_ptr->TFCR1 & 0x0f;
		break;
	case DW_I2S_CHANNEL2:
		len = i2s_tx_reg_ptr->TFCR2 & 0x0f;
		break;
	case DW_I2S_CHANNEL3:
		len = i2s_tx_reg_ptr->TFCR3 & 0x0f;
		break;
	default:
		break;
	}
	return len;
}

/**
 * @fn void dw_i2s_tx_set_txchet(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t level, uint32_t channel)
 * @brief Set DesignWare I2S Transmit Channel Empty Trigger (TXCHET)
 */
Inline void dw_i2s_tx_set_txchet(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t level, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_reg_ptr->TFCR0 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL1:
		i2s_tx_reg_ptr->TFCR1 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL2:
		i2s_tx_reg_ptr->TFCR2 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL3:
		i2s_tx_reg_ptr->TFCR3 = level & 0x0f;
		break;
	default:
		break;
	}
}

/**
 * @fn uint32_t dw_i2s_rx_get_rxchdt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
 * @brief Get DesignWare I2S Receive Channel Data Available Trigger (RXCHDT)
 */
Inline uint32_t dw_i2s_rx_get_rxchdt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t channel)
{
	uint32_t len = 0;

	switch (channel) {
	case DW_I2S_CHANNEL0:
		len = i2s_rx_reg_ptr->RFCR0 & 0x0f;
		break;
	case DW_I2S_CHANNEL1:
		len = i2s_rx_reg_ptr->RFCR1 & 0x0f;
		break;
	case DW_I2S_CHANNEL2:
		len = i2s_rx_reg_ptr->RFCR2 & 0x0f;
		break;
	case DW_I2S_CHANNEL3:
		len = i2s_rx_reg_ptr->RFCR3 & 0x0f;
		break;
	default:
		break;
	}
	return len;
}

/**
 * @fn void dw_i2s_rx_set_rxchdt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t level, uint32_t channel)
 * @brief Set DesignWare I2S Receiver Channel Data Available Trigger (RXCHDT)
 */
Inline void dw_i2s_rx_set_rxchdt(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t level, uint32_t channel)
{
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_rx_reg_ptr->RFCR0 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL1:
		i2s_rx_reg_ptr->RFCR1 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL2:
		i2s_rx_reg_ptr->RFCR2 = level & 0x0f;
		break;
	case DW_I2S_CHANNEL3:
		i2s_rx_reg_ptr->RFCR3 = level & 0x0f;
		break;
	}
}

/**
 * @fn void dw_i2s_tx_clock_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Enable DesignWare I2S TX clock generation
 */
Inline void dw_i2s_tx_clock_enable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->CER |= DW_I2S_CER_CLKEN;
}

/**
 * @fn void dw_i2s_tx_clock_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Disable DesignWare I2S TX clock generation
 */
Inline void dw_i2s_tx_clock_disable(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->CER &= ~DW_I2S_CER_CLKEN;
}

/**
 * @fn void dw_i2s_rx_clock_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Enable DesignWare I2S RX clock generation
 */
Inline void dw_i2s_rx_clock_enable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->CER |= DW_I2S_CER_CLKEN;
}

/**
 * @fn void dw_i2s_rx_clock_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Disable DesignWare I2S RX clock generation
 */
Inline void dw_i2s_rx_clock_disable(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->CER &= ~DW_I2S_CER_CLKEN;
}

/**
 * @fn void dw_i2s_tx_set_wss(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t word_select)
 * @brief Set DesignWare I2S TX clock configuration word select line (ws_out)
 */
Inline void dw_i2s_tx_set_wss(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t word_select)
{
	i2s_tx_reg_ptr->CCR &= DW_I2S_CCR_WSS(0) | 0x07;
	i2s_tx_reg_ptr->CCR |= DW_I2S_CCR_WSS(word_select);
}

/**
 * @fn void dw_i2s_rx_set_wss(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t word_select)
 * @brief Set DesignWare I2S RX clock configuration word select line (ws_out)
 */
Inline void dw_i2s_rx_set_wss(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t word_select)
{
	i2s_rx_reg_ptr->CCR &= DW_I2S_CCR_WSS(0) | 0x07;
	i2s_rx_reg_ptr->CCR |= DW_I2S_CCR_WSS(word_select);
}

/**
 * @fn void dw_i2s_rx_flush_fifo(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Reset DesignWare I2S RX FIFO, flush all the RX FIFOs (a self clearing bit)
 */
Inline void dw_i2s_rx_flush_fifo(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	i2s_rx_reg_ptr->RXFFR |= DW_I2S_RXFFR_RXFFR;
}

/**
 * @fn void dw_i2s_tx_flush_fifo(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Reset DesignWare I2S TX FIFO, flush all the TX FIFOs (a self clearing bit)
 */
Inline void dw_i2s_tx_flush_fifo(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	i2s_tx_reg_ptr->TXFFR |= DW_I2S_TXFFR_TXFFR;
}

/**
 * @fn uint32_t dw_i2s_rx_get_fifo_len(DW_I2S_RX_REG *i2s_rx_reg_ptr)
 * @brief Get RX FIFO length
 */
Inline uint32_t dw_i2s_rx_get_fifo_len(DW_I2S_RX_REG *i2s_rx_reg_ptr)
{
	uint32_t len = 0;

	switch ((((i2s_rx_reg_ptr->I2S_COMP_PARAM_1) >> 2) & 0x03)) {
	case 0:
		len = 2;
		break;
	case 1:
		len = 4;
		break;
	case 2:
		len = 8;
		break;
	case 3:
		len = 16;
		break;
	default:
		break;
	}
	return len;
}

/**
 * @fn uint32_t dw_i2s_tx_get_fifo_len(DW_I2S_TX_REG *i2s_tx_reg_ptr)
 * @brief Get TX FIFO length
 */
Inline uint32_t dw_i2s_tx_get_fifo_len(DW_I2S_TX_REG *i2s_tx_reg_ptr)
{
	uint32_t len = 0;

	switch ((((i2s_tx_reg_ptr->I2S_COMP_PARAM_1) >> 2) & 0x03)) {
	case 0:
		len = 2;
		break;
	case 1:
		len = 4;
		break;
	case 2:
		len = 8;
		break;
	case 3:
		len = 16;
		break;
	default:
		break;
	}
	return len;
}

/**
 * @fn void dw_i2s_tx_set_sclkg(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t clkg)
 * @brief Set DesignWare I2S TX clock configuration SCLKG (gating of sclk)
 */
Inline void dw_i2s_tx_set_sclkg(DW_I2S_TX_REG *i2s_tx_reg_ptr, uint32_t clkg)
{
	i2s_tx_reg_ptr->CCR &= DW_I2S_CCR_SCLKG(0) | 0x18;
	i2s_tx_reg_ptr->CCR |= DW_I2S_CCR_SCLKG(clkg);
}

/**
 * @fn void dw_i2s_rx_set_sclkg(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t clkg)
 * @brief Set DesignWare I2S RX clock configuration SCLKG (gating of sclk)
 */
Inline void dw_i2s_rx_set_sclkg(DW_I2S_RX_REG *i2s_rx_reg_ptr, uint32_t clkg)
{
	i2s_rx_reg_ptr->CCR &= DW_I2S_CCR_SCLKG(0) | 0x18;
	i2s_rx_reg_ptr->CCR |= DW_I2S_CCR_SCLKG(clkg);
}

/**
 * @fn uint32_t dw_i2s_get_wlen_bits(uint32_t resolution)
 * @brief Set RX/TX WLEN
 */
Inline uint32_t dw_i2s_get_wlen_bits(uint32_t resolution)
{
	uint32_t wlen_bits = 0;

	switch (resolution) {
	case I2S_AUD_DATA_NA:
		wlen_bits = 0;
		break;
	case I2S_AUD_DATA_12B:
		wlen_bits = 0x01;
		break;
	case I2S_AUD_DATA_16B:
		wlen_bits = 0x02;
		break;
	case I2S_AUD_DATA_20B:
		wlen_bits = 0x03;
		break;
	case I2S_AUD_DATA_24B:
		wlen_bits = 0x04;
		break;
	case I2S_AUD_DATA_32B:
		wlen_bits = 0x05;
		break;
	default:
		wlen_bits = 0;
		break;
	}
	return wlen_bits;
}
static void dw_i2s_rx_set_wlen(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint32_t wlen_bits;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr;
	DW_I2S_RX_REG *i2s_rx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_rx_reg_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[0]);
		i2s_rx_reg_ptr->RCR0 = DW_I2S_RCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL1:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[1]);
		i2s_rx_reg_ptr->RCR1 = DW_I2S_RCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL2:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[2]);
		i2s_rx_reg_ptr->RCR2 = DW_I2S_RCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL3:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[3]);
		i2s_rx_reg_ptr->RCR3 = DW_I2S_RCRX_WLEN(wlen_bits);
		break;
	default:
		break;
	}
}
static void dw_i2s_tx_set_wlen(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint32_t wlen_bits;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr;
	DW_I2S_TX_REG *i2s_tx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_tx_reg_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[0]);
		i2s_tx_reg_ptr->TCR0 = DW_I2S_TCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL1:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[1]);
		i2s_tx_reg_ptr->TCR1 = DW_I2S_TCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL2:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[2]);
		i2s_tx_reg_ptr->TCR2 = DW_I2S_TCRX_WLEN(wlen_bits);
		break;
	case DW_I2S_CHANNEL3:
		wlen_bits = dw_i2s_get_wlen_bits(i2s_config_ptr->data_res[3]);
		i2s_tx_reg_ptr->TCR3 = DW_I2S_TCRX_WLEN(wlen_bits);
		break;
	default:
		break;
	}
}

/* Disable I2S interrupt */
static void dw_i2s_rx_disable_interrupt(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint16_t da_intr = 0;
	uint16_t or_intr = 0;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr;
	DW_I2S_RX_REG *i2s_rx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_rx_reg_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	da_intr = i2s_config_ptr->intno[channel] & 0x0000ffff;
	or_intr = (i2s_config_ptr->intno[channel] & 0xffff0000) >> 16;
	/* disable I2S interrupt */
	dw_i2s_rx_mask_interrupt(i2s_rx_reg_ptr, DW_I2S_IMRX_RXFOM | DW_I2S_IMRX_RXDAM, channel);
	if ((i2s_rx_ctrl_ptr->int_status[channel] & DW_I2S_GINT_ENABLE) == 1) {
		i2s_rx_ctrl_ptr->int_status[channel] &= ~DW_I2S_GINT_ENABLE;
		int_disable(da_intr);
		int_disable(or_intr);
	}
}
static void dw_i2s_tx_disable_interrupt(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint16_t da_intr = 0;
	uint16_t or_intr = 0;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr;
	DW_I2S_TX_REG *i2s_tx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_tx_reg_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	da_intr = i2s_config_ptr->intno[channel] & 0x0000ffff;
	or_intr = (i2s_config_ptr->intno[channel] & 0xffff0000) >> 16;
	/* disable I2S interrupt */
	dw_i2s_tx_mask_interrupt(i2s_tx_reg_ptr, DW_I2S_IMRX_TXFOM | DW_I2S_IMRX_TXFEM, channel);
	if ((i2s_tx_ctrl_ptr->int_status[channel] & DW_I2S_GINT_ENABLE) == 1) {
		i2s_tx_ctrl_ptr->int_status[channel] &= ~DW_I2S_GINT_ENABLE;
		int_disable(da_intr);
		int_disable(or_intr);
	}
}

/* Disable RX/TX device interrupt */
static void dw_i2s_disable_dev_intr(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		dw_i2s_rx_disable_interrupt(i2s_info_ptr, channel);
	} else {
		dw_i2s_tx_disable_interrupt(i2s_info_ptr, channel);
	}

}

/* Enable I2S interrupt */
static void dw_i2s_rx_enable_interrupt(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint16_t da_intr = 0;
	uint16_t or_intr = 0;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr;
	DW_I2S_RX_REG *i2s_rx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_rx_reg_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	da_intr = i2s_config_ptr->intno[channel] & 0x0000ffff;
	or_intr = (i2s_config_ptr->intno[channel] & 0xffff0000) >> 16;
	/* Unmask I2S interrupt */
	dw_i2s_rx_unmask_interrupt(i2s_rx_reg_ptr, DW_I2S_IMRX_RXFOM | DW_I2S_IMRX_RXDAM, channel);
	if ((i2s_rx_ctrl_ptr->int_status[channel] & DW_I2S_GINT_ENABLE) == 0) {
		i2s_rx_ctrl_ptr->int_status[channel] |= DW_I2S_GINT_ENABLE;
		int_enable(da_intr);
		int_enable(or_intr);
	}
}
static void dw_i2s_tx_enable_interrupt(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	uint16_t da_intr = 0;
	uint16_t or_intr = 0;
	DW_I2S_CONFIG *i2s_config_ptr;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr;
	DW_I2S_TX_REG *i2s_tx_reg_ptr;

	i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_tx_reg_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	da_intr = i2s_config_ptr->intno[channel] & 0x0000ffff;
	or_intr = (i2s_config_ptr->intno[channel] & 0xffff0000) >> 16;
	/* Unmask I2S interrupt */
	dw_i2s_tx_unmask_interrupt(i2s_tx_reg_ptr, DW_I2S_IMRX_TXFOM | DW_I2S_IMRX_TXFEM, channel);
	if ((i2s_tx_ctrl_ptr->int_status[channel] & DW_I2S_GINT_ENABLE) == 0) {
		i2s_tx_ctrl_ptr->int_status[channel] |= DW_I2S_GINT_ENABLE;
		int_enable(da_intr);
		int_enable(or_intr);
	}
}

/* Enable RX/TX device interrupt */
static void dw_i2s_enable_dev_intr(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		dw_i2s_rx_enable_interrupt(i2s_info_ptr, channel);
	} else {
		dw_i2s_tx_enable_interrupt(i2s_info_ptr, channel);
	}
}

/* Enable I2S device and channel */
static void dw_i2s_enable_device(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;

	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		/* Enable I2S block */
		dw_i2s_rx_block_enable(i2s_rx_regs_ptr);
		/* Enable target channel */
		dw_i2s_rx_channel_enable(i2s_rx_regs_ptr, channel);
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		/* Enable I2S block */
		dw_i2s_tx_block_enable(i2s_tx_regs_ptr);
		/* Enable all available channels */
		dw_i2s_tx_channel_enable(i2s_tx_regs_ptr, channel);
	}
}

/* Disable I2S device and channel */
static void dw_i2s_disable_device(DEV_I2S_INFO *i2s_info_ptr, uint32_t channel)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;

	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		/* Disable target channel */
		dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, channel);
		/* Disable I2S block */
		dw_i2s_rx_block_disable(i2s_rx_regs_ptr);
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		/* Disable all available channels */
		dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, channel);
		/* Disable I2S block */
		dw_i2s_tx_block_disable(i2s_tx_regs_ptr);
	}
}

/* Disable I2S device all available channel */
static void dw_i2s_disable_device_all(DEV_I2S_INFO *i2s_info_ptr)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	uint32_t channels = i2s_config_ptr->channels;

	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		/* Disable all available channels */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, DW_I2S_CHANNEL0);
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, DW_I2S_CHANNEL1);
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, DW_I2S_CHANNEL2);
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, DW_I2S_CHANNEL3);
		}
		/* Disable I2S block */
		dw_i2s_rx_block_disable(i2s_rx_regs_ptr);
		/* Not Disable I2S IP */
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		/* Disable all available channels */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, DW_I2S_CHANNEL1);
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, DW_I2S_CHANNEL2);
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, DW_I2S_CHANNEL3);
		}
		/* Enable I2S block */
		dw_i2s_tx_block_disable(i2s_tx_regs_ptr);
		/* Not Disable I2S IP */
	}
}

/* Reset I2S device */
static void dw_i2s_reset_device(DEV_I2S_INFO *i2s_info_ptr)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	uint32_t channels = i2s_config_ptr->channels;

	dw_i2s_disable_device_all(i2s_info_ptr);
	i2s_info_ptr->cur_state = I2S_FREE;
	i2s_info_ptr->err_state = I2S_ERR_NONE;
	/* Clear FIFO overrun interrupt */
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		/* Disable all available channels */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL0);
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL1);
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL2);
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL3);
		}
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		/* Disable all available channels */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL1);
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL2);
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL3);
		}
	}
}

/* Flush FIFO */
static void dw_i2s_flush(DEV_I2S_INFO *i2s_info_ptr)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;

	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		dw_i2s_rx_block_disable(i2s_rx_regs_ptr);
		dw_i2s_rx_flush_fifo(i2s_rx_regs_ptr);
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		dw_i2s_tx_block_disable(i2s_tx_regs_ptr);
		dw_i2s_tx_flush_fifo(i2s_tx_regs_ptr);
	}
}

/* Enable interrupt for all available channels */
static void dw_i2s_enable_all_channel_intr(DEV_I2S_INFO *i2s_info_ptr)
{
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	uint32_t channels = i2s_config_ptr->channels;

	if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
		dw_i2s_enable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL0);
	}
	if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
		dw_i2s_enable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL1);
	}
	if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
		dw_i2s_enable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL2);
	}
	if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
		dw_i2s_enable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL3);
	}
}

/* Disable interrupt for all available channels */
static void dw_i2s_disable_all_channel_intr(DEV_I2S_INFO *i2s_info_ptr)
{
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	uint32_t channels = i2s_config_ptr->channels;

	if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
		dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL0);
	}
	if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
		dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL1);
	}
	if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
		dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL2);
	}
	if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
		dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL3);
	}
}

/* Disable I2S send or receive interrupt in interrupt routine */
static void dw_i2s_dis_cbr(DEV_I2S_INFO *i2s_info_ptr, uint32_t cbrtn, uint32_t channel)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;

	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		if (cbrtn == DW_I2S_RX_RDY_RCV) {
			dw_i2s_rx_disable_interrupt(i2s_info_ptr, channel);
			i2s_rx_ctrl_ptr->int_status[channel] &= ~DW_I2S_RXINT_ENABLE;
		}
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		if (cbrtn == DW_I2S_TX_RDY_SND) {
			dw_i2s_tx_disable_interrupt(i2s_info_ptr, channel);
			i2s_tx_ctrl_ptr->int_status[channel] &= ~DW_I2S_TXINT_ENABLE;
		}
	}
}

/* Write left holding register */
static void dw_i2s_tx_write_left_hold_reg(DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr, uint32_t data, uint32_t channel)
{
	DW_I2S_TX_REG *i2s_tx_regs_ptr;

	i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_regs_ptr->LTHR0 = DW_I2S_LTHR0_LTHR0(data);
		break;
	/* Todo: add more channel support */
	default:
		break;
	}
}

/* Write right holding register */
static void dw_i2s_tx_write_right_hold_reg(DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr, uint32_t data, uint32_t channel)
{
	DW_I2S_TX_REG *i2s_tx_regs_ptr;

	i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		i2s_tx_regs_ptr->RTHR0 = DW_I2S_RTHR0_RTHR0(data);
		break;
	/* Todo: add more channel support */
	default:
		break;
	}
}

/* Read left holding register */
static void dw_i2s_rx_read_left_hold_reg(DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr, uint32_t *data, uint32_t channel)
{
	DW_I2S_RX_REG *i2s_rx_regs_ptr;

	i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		*data = (uint32_t)DW_I2S_LRBR0_LRBR0(i2s_rx_regs_ptr->LRBR0);
	/* Todo: add more channel support */
	default:
		break;
	}
}

/* Read right holding register */
static void dw_i2s_rx_read_right_hold_reg(DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr, uint32_t *data, uint32_t channel)
{
	DW_I2S_RX_REG *i2s_rx_regs_ptr;

	i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	switch (channel) {
	case DW_I2S_CHANNEL0:
		*data = (uint32_t)DW_I2S_RRBR0_RRBR0(i2s_rx_regs_ptr->RRBR0);
	/* Todo: add more channel support */
	default:
		break;
	}
}

/*
 * @brief	I2S TX write samples into FIFO
 * @param[in]	i2s_info_ptr Pointer to I2S device information
 * @param[in]	count        Count of stereo data (two as left + right)
 * @param[in]	channel      Target channel of transmitter
 * @retval	Remaining sample count for the next transmitting in TX FIFO
 */
static uint32_t dw_i2s_tx_write_samples(DEV_I2S_INFO *i2s_info_ptr, uint32_t count, uint32_t channel)
{
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;

	i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;

	uint32_t empty_trig = dw_i2s_tx_get_txchet(i2s_tx_regs_ptr, channel);
	uint32_t remain_cnt = 0;
	uint32_t avail_cnt = i2s_tx_ctrl_ptr->dw_i2s_buf.len - i2s_tx_ctrl_ptr->dw_i2s_buf.ofs;
	int32_t i = 0;
	#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *buff = i2s_tx_ctrl_ptr->dw_i2s_buf.buf;
	#else
	uint32_t *buff = i2s_tx_ctrl_ptr->dw_i2s_buf.buf;
	#endif
	uint32_t count_pair = count >> 1;

	if (count_pair < avail_cnt) { /* enough space in TX FIFO */
		avail_cnt = count_pair;
		remain_cnt = 0;
	} else {   /* remaining data */
		remain_cnt = count - avail_cnt * 2;
	}
	/* fill I2S buffer with REG buffer */
	for (i = 0; i < avail_cnt; i++) {
		dw_i2s_tx_write_left_hold_reg(i2s_tx_ctrl_ptr, buff[2 * i], channel);
		dw_i2s_tx_write_right_hold_reg(i2s_tx_ctrl_ptr, buff[1 + 2 * i], channel);
	}
	/*the ofs is used to judge the next number of data to send to fifos*/
	if (remain_cnt == 0) {
		i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = 0;
	} else {
		i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = empty_trig;
	}

	return remain_cnt;

}

/*
 * @brief	I2S RX read samples from FIFO
 * @param[in]	i2s_info_ptr Pointer to I2S device information
 * @param[in]	count        Available count of stereo data (two as left + right)
 * @param[in]	channel      Target channel of transmitter
 * @retval	Remaining count in the buffer
 */
static uint32_t dw_i2s_rx_read_samples(DEV_I2S_INFO *i2s_info_ptr, uint32_t count, uint32_t channel)
{
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;

	i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;

	uint32_t avail_trig = dw_i2s_rx_get_rxchdt(i2s_rx_regs_ptr, channel);
	uint32_t remain_cnt = 0;
	uint32_t avail_cnt = avail_trig;
	int32_t i = 0;
	uint32_t buf = 0;
#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *buff = i2s_rx_ctrl_ptr->dw_i2s_buf.buf;
#else
	uint32_t *buff = i2s_rx_ctrl_ptr->dw_i2s_buf.buf;
#endif
	uint32_t count_pair = count >> 1;

	if (count_pair < avail_cnt) { /* not enough space in buffer */
		avail_cnt = count_pair;
		remain_cnt = 0;
	} else {   /* remaining data */
		remain_cnt = count - avail_cnt * 2;
	}
	/* Read I2S buffer and send it to buff */
	for (i = 0; i < avail_cnt; i++) {
#ifdef DW_I2S_DATA_LENGTH_16
		dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buf, channel);
		buff[2 * i] = (uint16_t)buf;
		dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buf, channel);
		buff[1 + 2 * i] = (uint16_t)buf;
#else
		dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buff[2 * i], channel);
		dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buff[1 + 2 * i], channel);
#endif
	}

	/* accumulation of sending data count */
	i2s_rx_ctrl_ptr->dw_i2s_buf.ofs += avail_cnt;

	return remain_cnt;

}

/** @} end of group DEVICE_DW_I2S_STATIC */

static void delay(uint32_t delay_cycle)
{
	uint32_t i, j;

	for (j = 0; j < delay_cycle; j++) {
		for (i = 0; i < 100; i++) {
			arc_nop();
		}
	}
}

/*
 * @brief	I2S write data in interrupt
 * @param[in]	i2s_obj Pointer to I2S device
 */
static void dw_i2s_mst_int_write(DEV_I2S *i2s_obj)
{
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	DW_I2S_TX_REG *i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;

	uint32_t empty_trig = 16 - dw_i2s_tx_get_txchet(i2s_tx_regs_ptr, 0);
	int32_t i = 0, buf_ofs = i2s_info_ptr->tx_buf.ofs * empty_trig;
	int32_t count_pair;

#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *buff = (uint16_t *)i2s_info_ptr->tx_buf.buf;
#else
	uint32_t *buff = (uint32_t *)i2s_info_ptr->tx_buf.buf;
#endif

	dw_i2s_dis_cbr(i2s_info_ptr, DW_I2S_TX_RDY_SND, DW_I2S_CHANNEL0);
	uint16_t isr0 = i2s_tx_regs_ptr->ISR0;
	if (buff != NULL) {
		if ((isr0 & DW_I2S_ISRX_TXFO) != 0) {
			i2s_info_ptr->cur_state = I2S_FREE;
			if (i2s_info_ptr->i2s_cbs.err_cb != NULL) {
				i2s_info_ptr->i2s_cbs.err_cb(i2s_obj);
			}
		}
		if ((isr0 & DW_I2S_ISRX_TXFE) != 0) {
			if (i2s_info_ptr->tx_buf.len >= empty_trig * 2) {
				/* fill I2S buffer with REG buffer */
				for (i = 0; i < empty_trig; i++) {
					dw_i2s_tx_write_left_hold_reg(i2s_tx_ctrl_ptr, buff[2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
					dw_i2s_tx_write_right_hold_reg(i2s_tx_ctrl_ptr, buff[1 + 2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
				}
				i2s_info_ptr->tx_buf.ofs += 1;
				i2s_info_ptr->tx_buf.len -= empty_trig * 2;
				dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
				dw_i2s_tx_enable_interrupt(i2s_info_ptr, DW_I2S_CHANNEL0);
			} else {
				count_pair = i2s_info_ptr->tx_buf.len >> 1;
				for (i = 0; i < count_pair; i++) {
					dw_i2s_tx_write_left_hold_reg(i2s_tx_ctrl_ptr, buff[2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
					dw_i2s_tx_write_right_hold_reg(i2s_tx_ctrl_ptr, buff[1 + 2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
				}
				/* if the length of remainning data is less than empty_trig*2,fill 0 */
				for (i = count_pair; i < empty_trig; i++) {
					dw_i2s_tx_write_left_hold_reg(i2s_tx_ctrl_ptr, 0, DW_I2S_CHANNEL0);
					dw_i2s_tx_write_right_hold_reg(i2s_tx_ctrl_ptr, 0, DW_I2S_CHANNEL0);
				}
				i2s_info_ptr->tx_buf.len = 0;
				dw_i2s_tx_clear_overrun_interrupt(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
			}
			/*the ofs is used to judge the next number of data to send to fifos*/
			// empty_trig = dw_i2s_tx_get_txchet(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
			// i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = empty_trig;
			i2s_info_ptr->cur_state = I2S_FREE;
			if ((i2s_info_ptr->tx_buf.len == 0) && (i2s_info_ptr->i2s_cbs.tx_cb != NULL)) {
				i2s_info_ptr->i2s_cbs.tx_cb(i2s_obj);
			}
		}
	}
}

/*
 * @brief	I2S read data in interrupt
 * @param[in]	i2s_obj Pointer to I2S device
 */
static void dw_i2s_mst_int_rx(DEV_I2S *i2s_obj)
{
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	DW_I2S_RX_REG *i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;

	uint32_t empty_trig = dw_i2s_rx_get_rxchdt(i2s_rx_regs_ptr, 0), buf;
	int32_t i = 0, buf_ofs = i2s_info_ptr->rx_buf.ofs * empty_trig;
	int32_t count_pair;

#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *buff = (uint16_t *)i2s_info_ptr->rx_buf.buf;
#else
	uint32_t *buff = (uint32_t *)i2s_info_ptr->rx_buf.buf;
#endif
	dw_i2s_dis_cbr(i2s_info_ptr, DW_I2S_RX_RDY_RCV, DW_I2S_CHANNEL0);
	uint16_t isr0 = i2s_rx_regs_ptr->ISR0;
	if (buff != NULL) {
		/* over run interrupt,read all data in fifo */
		if ((isr0 & DW_I2S_ISRX_RXFO) != 0) {
			i2s_info_ptr->cur_state = I2S_FREE;
			for (i = 0; i < 16; i++) {
#ifdef DW_I2S_DATA_LENGTH_16
				dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
				buff[2 * (i + buf_ofs)] = (uint16_t)buf;
				dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
				buff[1 + 2 * (i + buf_ofs)] = (uint16_t)buf;
#else
				dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buff[2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
				dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buff[1 + 2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
#endif
			}
		}
		if ((isr0 & DW_I2S_ISRX_RXDA) != 0) {
			if (i2s_info_ptr->rx_buf.len >= empty_trig * 2) {
				for (i = 0; i < empty_trig; i++) {
#ifdef DW_I2S_DATA_LENGTH_16
					dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
					buff[2 * (i + buf_ofs)] = (uint16_t)buf;
					dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
					buff[1 + 2 * (i + buf_ofs)] = (uint16_t)buf;
#else
					dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buff[2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
					dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buff[1 + 2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
#endif
				}
				i2s_info_ptr->rx_buf.ofs += 1;
				i2s_info_ptr->rx_buf.len -= empty_trig * 2;
				dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL0);
				dw_i2s_rx_enable_interrupt(i2s_info_ptr, DW_I2S_CHANNEL0);
			} else {
				count_pair = i2s_info_ptr->rx_buf.len >> 1;
				for (i = 0; i < count_pair; i++) {
#ifdef DW_I2S_DATA_LENGTH_16
					dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
					buff[2 * (i + buf_ofs)] = (uint16_t)buf;
					dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
					buff[1 + 2 * (i + buf_ofs)] = (uint16_t)buf;
#else
					dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buff[2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
					dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buff[1 + 2 * (i + buf_ofs)], DW_I2S_CHANNEL0);
#endif
				}
				/* if the length of remainning data is less than empty_trig*2,fill 0 */
				for (i = count_pair; i < empty_trig; i++) {
					dw_i2s_rx_read_left_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
					dw_i2s_rx_read_right_hold_reg(i2s_rx_ctrl_ptr, &buf, DW_I2S_CHANNEL0);
				}
				i2s_info_ptr->rx_buf.len = 0;
				dw_i2s_rx_clear_overrun_interrupt(i2s_rx_regs_ptr, DW_I2S_CHANNEL0);
			}
			/*the ofs is used to judge the next number of data to send to fifos*/
			// empty_trig = dw_i2s_tx_get_txchet(i2s_tx_regs_ptr, DW_I2S_CHANNEL0);
			// i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = empty_trig;
			i2s_info_ptr->cur_state = I2S_FREE;
			if ((i2s_info_ptr->rx_buf.len == 0) && (i2s_info_ptr->i2s_cbs.rx_cb != NULL)) {
				i2s_info_ptr->i2s_cbs.rx_cb(i2s_obj);
			}
		}
	} else {
		printf("The buffer of receiver is NULL !\n");
	}
}
/* Clear interrupt */

/**
 * @defgroup	DEVICE_DW_I2S_IMPLEMENT	DesignWare I2S Driver Function API Implement
 * @ingroup	DEVICE_DW_I2S
 * @brief	HAL I2S implemention with DesignWare I2S
 * @{
 */

/**
 * @brief	Open a designware i2s device
 * @param[in]	i2s_obj	I2S device object pointer
 * @param[in]	mode	Working mode (@ref DEV_MASTER_MODE "master" or @ref DEV_SLAVE_MODE "slave")
 * @param[in]	param	Device type (@ref I2S_DEVICE_TRANSMITTER "transmitter" OR @ref I2S_DEVICE_RECEIVER "receiver")
 * @retval	E_OK	Open successfully without any issues
 * @retval	E_OPNED	Device was opened before
 * @retval	E_OBJ	Device object is not valid
 * @retval	E_SYS	Device initialized parameters are different with parameters in IP
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_NOSPT	Open settings are not supported
 */
int32_t dw_i2s_open(DEV_I2S *i2s_obj, uint32_t mode, uint32_t param)
{
	int32_t ercd = E_OK;
	uint32_t support_modes;
	uint32_t channels = 0;
	uint16_t da_intr = 0;
	uint16_t or_intr = 0;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);

	/* START ERROR CHECK */
	VALID_CHK_I2S_INFO_OBJECT(i2s_info_ptr);
	DW_I2S_CHECK_EXP((mode == DEV_MASTER_MODE) || (mode == DEV_SLAVE_MODE), E_PAR);
	DW_I2S_CHECK_EXP((param == I2S_DEVICE_TRANSMITTER) || (param == I2S_DEVICE_RECEIVER), E_PAR);
	DW_I2S_CHECK_EXP(i2s_info_ptr->device == param, E_PAR);
	/* END OF ERROR CHECK */

	/** Check FIFO length */
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
		if (i2s_config_ptr->fifo_len != dw_i2s_rx_get_fifo_len(i2s_rx_regs_ptr)) {
			return E_SYS;
		}
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
		if (i2s_config_ptr->fifo_len != dw_i2s_tx_get_fifo_len(i2s_tx_regs_ptr)) {
			return E_SYS;
		}
	}

	/* Check supported modes, master or slave */
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		support_modes = i2s_config_ptr->support_modes;
	} else {
		support_modes = i2s_config_ptr->support_modes;
	}
	DW_I2S_CHECK_EXP((((support_modes) & DW_I2S_MASTER_SUPPORTED) && (mode == DEV_MASTER_MODE)) || \
			 (((support_modes) & DW_I2S_SLAVE_SUPPORTED) && (mode == DEV_SLAVE_MODE)), E_NOSPT);

	/** Check opened before use case */
	if (i2s_info_ptr->opn_cnt > 0) {
		return E_OPNED;
	}

	/* auto increase open count */
	i2s_info_ptr->opn_cnt++;
	i2s_info_ptr->mode = mode;

	channels = i2s_config_ptr->channels;
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		/* Disable device before init it */
		dw_i2s_rx_block_disable(i2s_rx_regs_ptr);
		dw_i2s_rx_disable(i2s_rx_regs_ptr);
		/* Flush FIFO */
		dw_i2s_rx_flush_fifo(i2s_rx_regs_ptr);
		/* Set WSS and SCLKG */
		if (i2s_info_ptr->mode == DEV_MASTER_MODE) {
			dw_i2s_rx_clock_disable(i2s_rx_regs_ptr);
			dw_i2s_rx_set_wss(i2s_rx_regs_ptr, i2s_config_ptr->ws_length);
			dw_i2s_rx_set_sclkg(i2s_rx_regs_ptr, i2s_config_ptr->sclk_gate);
		}
		/* Set data resolution for channels and disable I2S device interrupt */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_rx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL0);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL0);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL0] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL0] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_rx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL1);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL1);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL1] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL1] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_rx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL2);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL2);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL2] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL2] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_rx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL3);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL3);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL3] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL3] & 0xffff0000) >> 16;
		}
		int_handler_install(da_intr, i2s_config_ptr->dw_i2s_int_handler);
		int_handler_install(or_intr, i2s_config_ptr->dw_i2s_int_handler);
		/* Clean I2S buffer in ctrl */
		memset(&(i2s_rx_ctrl_ptr->dw_i2s_buf), 0, sizeof(DW_I2S_BUFFER));
	} else {
		/* Disable device before init it */
		dw_i2s_tx_block_disable(i2s_tx_regs_ptr);
		dw_i2s_tx_disable(i2s_tx_regs_ptr);
		/* Flush FIFO */
		dw_i2s_tx_flush_fifo(i2s_tx_regs_ptr);
		/* Set WSS and SCLKG */
		if (i2s_info_ptr->mode == DEV_MASTER_MODE) {
			dw_i2s_tx_clock_disable(i2s_tx_regs_ptr);
			dw_i2s_tx_set_wss(i2s_tx_regs_ptr, i2s_config_ptr->ws_length);
			dw_i2s_tx_set_sclkg(i2s_tx_regs_ptr, i2s_config_ptr->sclk_gate);
		}
		/* Set data resolution for channels and disable I2S device interrupt */
		if (channels & DW_I2S_CHANNEL0_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_tx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL0);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL0);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL0] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL0] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL1_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_tx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL1);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL1);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL1] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL1] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL2_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_tx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL2);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL2);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL2] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL2] & 0xffff0000) >> 16;
		}
		if (channels & DW_I2S_CHANNEL3_SUPPORTED) {
			/* Set data resolution for channels */
			dw_i2s_tx_set_wlen(i2s_info_ptr, DW_I2S_CHANNEL3);
			/* Disable I2S device interrupt */
			dw_i2s_disable_dev_intr(i2s_info_ptr, DW_I2S_CHANNEL3);
			da_intr = i2s_config_ptr->intno[DW_I2S_CHANNEL3] & 0x0000ffff;
			or_intr = (i2s_config_ptr->intno[DW_I2S_CHANNEL3] & 0xffff0000) >> 16;
		}
		int_handler_install(da_intr, i2s_config_ptr->dw_i2s_int_handler);
		int_handler_install(or_intr, i2s_config_ptr->dw_i2s_int_handler);
		/* Clean I2S buffer in ctrl */
		memset(&(i2s_tx_ctrl_ptr->dw_i2s_buf), 0, sizeof(DW_I2S_BUFFER));
	}

	/* Need to check parameters of ws_length, sample_rate and data_res and
	   initialize sclk divider according to sample_rate in board layer */

	i2s_info_ptr->status = DEV_ENABLED;
	i2s_info_ptr->cur_state = I2S_FREE;
	i2s_info_ptr->err_state = I2S_ERR_NONE;
	i2s_info_ptr->extra = NULL;
	i2s_info_ptr->i2s_cbs.tx_cb = NULL;
	i2s_info_ptr->i2s_cbs.rx_cb = NULL;
	i2s_info_ptr->i2s_cbs.err_cb = NULL;

	/** Clear I2S TX/RX buffer and callback */
	memset(&(i2s_info_ptr->tx_buf), 0, sizeof(DEV_BUFFER));
	memset(&(i2s_info_ptr->rx_buf), 0, sizeof(DEV_BUFFER));

	/* Enable I2S TX/RX IP */
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		dw_i2s_rx_enable(i2s_rx_regs_ptr);
	} else {
		dw_i2s_tx_enable(i2s_tx_regs_ptr);
	}

error_exit:
	return ercd;
}

/**
 * @brief	Close a designware i2s device
 * @param[in]	i2s_obj	I2S device object pointer
 * @retval	E_OK	Close successfully without any issues (including that device is already closed)
 * @retval	E_OPNED	Device is still opened, the device @ref DEV_I2S_INFO::opn_cnt "opn_cnt" decreased by 1
 * @retval	E_OBJ	Device object is not valid
 */
int32_t dw_i2s_close(DEV_I2S *i2s_obj)
{
	int32_t ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	uint32_t channels = 0;

	/* START ERROR CHECK */
	VALID_CHK_I2S_INFO_OBJECT(i2s_info_ptr);
	DW_I2S_CHECK_EXP(i2s_info_ptr->opn_cnt > 0, E_OK);
	/* END OF ERROR CHECK */

	i2s_info_ptr->opn_cnt--;
	if (i2s_info_ptr->opn_cnt == 0) {
		channels = i2s_config_ptr->channels;
		if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
			i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
			i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
			/* Disable device */
			dw_i2s_rx_block_disable(i2s_rx_regs_ptr);
			dw_i2s_rx_disable(i2s_rx_regs_ptr);
			/* Flush FIFO */
			dw_i2s_rx_flush_fifo(i2s_rx_regs_ptr);
		} else {
			i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
			i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
			/* Disable device */
			dw_i2s_tx_block_disable(i2s_tx_regs_ptr);
			dw_i2s_tx_disable(i2s_tx_regs_ptr);
			/* Flush FIFO */
			dw_i2s_tx_flush_fifo(i2s_tx_regs_ptr);
		}
		dw_i2s_disable_all_channel_intr(i2s_info_ptr);

		i2s_info_ptr->status = DEV_DISABLED;
		i2s_info_ptr->cur_state = I2S_FREE;
		i2s_info_ptr->err_state = I2S_ERR_NONE;
		i2s_info_ptr->extra = NULL;
		i2s_info_ptr->i2s_cbs.tx_cb = NULL;
		i2s_info_ptr->i2s_cbs.rx_cb = NULL;
		i2s_info_ptr->i2s_cbs.err_cb = NULL;

		/** Clear I2S TX/RX buffer and callback */
		memset(&(i2s_info_ptr->tx_buf), 0, sizeof(DEV_BUFFER));
		memset(&(i2s_info_ptr->rx_buf), 0, sizeof(DEV_BUFFER));
	} else {
		ercd = E_OPNED;
	}

error_exit:
	return ercd;
}

/**
 * @brief	Control i2s by ctrl command
 * @param[in]	i2s_obj	I2S device object pointer
 * @param[in]		ctrl_cmd	@ref DEVICE_HAL_I2S_CTRLCMD "control command", to change or get some thing related to i2s
 * @param[in,out]	param		Parameters that maybe argument of the command,
 * 					or return values of the command, must not be NULL
 * @retval	E_OK	Control device successfully
 * @retval	E_CLSED	Device is not opened
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid for current control command
 * @retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
 * @retval	E_CTX	Control device failed, due to different reasons like in transfer state
 * @retval	E_NOSPT	Control command is not supported or not valid
 */
int32_t dw_i2s_control(DEV_I2S *i2s_obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = NULL;
	DW_I2S_TX_REG *i2s_tx_regs_ptr = NULL;
	uint32_t val32;
	uint32_t val16_0 = 0;
	uint32_t val16_1 = 0;
	uint32_t channels = 0;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);
	DEV_BUFFER *devbuf;

	/* START ERROR CHECK */
	VALID_CHK_I2S_INFO_OBJECT(i2s_info_ptr);
	DW_I2S_CHECK_EXP(i2s_info_ptr->opn_cnt > 0, E_CLSED);
	/* END OF ERROR CHECK */

	channels = i2s_config_ptr->channels;
	if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
		i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;
	} else {
		i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
		i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	}

	/* check whether current device is disabled */
	if ((i2s_info_ptr->status & DEV_ENABLED) == 0) {
		/** When device is disabled,
		 * only I2S_CMD_ENA_DEV, I2S_CMD_DIS_DEV, I2S_CMD_GET_STATUS, I2S_CMD_RESET
		 * are available, other commands will return E_SYS
		 */
		if ((ctrl_cmd != I2S_CMD_ENA_DEV) &&	\
		    (ctrl_cmd != I2S_CMD_DIS_DEV) &&	\
		    (ctrl_cmd != I2S_CMD_GET_STATUS) &&	\
		    (ctrl_cmd != I2S_CMD_RESET)) {
			return E_SYS;
		}
	}

	/*
	 * To add command check for transmitter and receiver seperately
	 */

	switch (ctrl_cmd) {
	/* Commmon commands for both TX and RX mode */
	case I2S_CMD_GET_STATUS:
		DW_I2S_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		*((int32_t *)param) = i2s_info_ptr->status;
		break;
	case I2S_CMD_ENA_DEV:
		val32 = (uint32_t)param;
		if (DW_I2S_CHANNEL_CHECK(val16_1, channels)) {
			dw_i2s_enable_device(i2s_info_ptr, val32);
			i2s_info_ptr->status = DEV_ENABLED;
		} else {
			ercd = E_PAR;
		}
		break;
	case I2S_CMD_DIS_DEV:
		val32 = (uint32_t)param;
		if (DW_I2S_CHANNEL_CHECK(val16_1, channels)) {
			dw_i2s_disable_device(i2s_info_ptr, val32);
			i2s_info_ptr->status = DEV_DISABLED;
		} else {
			ercd = E_PAR;
		}
		break;
	case I2S_CMD_RESET:
		dw_i2s_reset_device(i2s_info_ptr);
		break;
	case I2S_CMD_FLUSH_TX:
		dw_i2s_flush(i2s_info_ptr);
		break;
	case I2S_CMD_FLUSH_RX:
		dw_i2s_flush(i2s_info_ptr);
		break;
	case I2S_CMD_SET_TXCB:
		DW_I2S_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		i2s_info_ptr->i2s_cbs.tx_cb = param;
		break;
	case I2S_CMD_SET_RXCB:
		DW_I2S_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		i2s_info_ptr->i2s_cbs.rx_cb = param;
		break;
	case I2S_CMD_SET_ERRCB:
		DW_I2S_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		i2s_info_ptr->i2s_cbs.err_cb = param;
		break;
	case I2S_CMD_SET_TXINT:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_TRANSMITTER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state != I2S_IN_RX, E_CTX);
		val32 = (uint32_t)param;
		/* Set all available channels */
		if (val32 == 0) {
			dw_i2s_disable_all_channel_intr(i2s_info_ptr);
		} else {
			dw_i2s_enable_all_channel_intr(i2s_info_ptr);
		}
		break;
	case I2S_CMD_SET_RXINT:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_RECEIVER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state != I2S_IN_TX, E_CTX);
		val32 = (uint32_t)param;
		/* Set all available channels */
		if (val32 == 0) {
			dw_i2s_disable_all_channel_intr(i2s_info_ptr);
		} else {
			dw_i2s_enable_all_channel_intr(i2s_info_ptr);
		}
		break;
	case I2S_CMD_SET_TXINT_BUF:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_TRANSMITTER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
		DW_I2S_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		if (param != NULL) {
			devbuf = (DEV_BUFFER *)param;
			i2s_info_ptr->tx_buf = *devbuf;
			i2s_info_ptr->tx_buf.ofs = 0;
			i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = 0;
			i2s_tx_ctrl_ptr->dw_i2s_buf.len = devbuf->len;
		} else {
			i2s_info_ptr->tx_buf.buf = NULL;
			i2s_info_ptr->tx_buf.len = 0;
			i2s_info_ptr->tx_buf.ofs = 0;
			i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = 0;
			i2s_tx_ctrl_ptr->dw_i2s_buf.len = 0;
		}
		break;
	case I2S_CMD_SET_RXINT_BUF:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_RECEIVER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
		DW_I2S_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		if (param != NULL) {
			devbuf = (DEV_BUFFER *)param;
			i2s_info_ptr->rx_buf = *devbuf;
			i2s_info_ptr->rx_buf.ofs = 0;
			i2s_rx_ctrl_ptr->dw_i2s_buf.ofs = 0;
			i2s_rx_ctrl_ptr->dw_i2s_buf.len = devbuf->len;
		} else {
			i2s_info_ptr->rx_buf.buf = NULL;
			i2s_info_ptr->rx_buf.len = 0;
			i2s_info_ptr->rx_buf.ofs = 0;
			i2s_rx_ctrl_ptr->dw_i2s_buf.ofs = 0;
			i2s_rx_ctrl_ptr->dw_i2s_buf.len = 0;
		}
		break;
	case I2S_CMD_SET_TXCHET_BUF:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_TRANSMITTER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
		DW_I2S_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		val32 = (uint32_t)param;
		val16_0 = (uint16_t)((val32 & 0xffff0000) >> 16);       // trigger level
		val16_1 = (uint16_t)(val32 & 0x0000ffff);               // channel number
		DW_I2S_CHECK_EXP((val16_0 >= 0) && (val16_0 < 16), E_PAR);
		if (!DW_I2S_CHANNEL_CHECK(val16_1, channels)) {
			return E_PAR;
		}
		dw_i2s_tx_channel_disable(i2s_tx_regs_ptr, val16_1);
		dw_i2s_tx_set_txchet(i2s_tx_regs_ptr, val16_0, val16_1);
		dw_i2s_tx_channel_enable(i2s_tx_regs_ptr, val16_1);
		break;
	case I2S_CMD_SET_RXCHDT_BUF:
		DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_RECEIVER, E_SYS);
		DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
		DW_I2S_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		val32 = (uint32_t)param;
		val16_0 = (uint16_t)((val32 & 0xffff0000) >> 16);       // trigger level
		val16_1 = (uint16_t)(val32 & 0x0000ffff);               // channel number
		DW_I2S_CHECK_EXP((val16_0 >= 0) && (val16_0 < 16), E_PAR);
		if (!DW_I2S_CHANNEL_CHECK(val16_1, channels)) {
			return E_PAR;
		}
		dw_i2s_rx_channel_disable(i2s_rx_regs_ptr, val16_1);
		dw_i2s_rx_set_rxchdt(i2s_rx_regs_ptr, val16_0, val16_1);
		dw_i2s_rx_channel_enable(i2s_rx_regs_ptr, val16_1);
		break;
	/* Master mode only commands */
	case I2S_CMD_MST_SET_CLK:
		DW_I2S_CHECK_EXP(i2s_info_ptr->mode == DEV_MASTER_MODE, E_NOSPT);
		val32 = (uint32_t)param;
		if (val32 == 0) {
			if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
				dw_i2s_rx_clock_disable(i2s_rx_ctrl_ptr->dw_i2s_regs);
			} else {
				dw_i2s_tx_clock_disable(i2s_tx_ctrl_ptr->dw_i2s_regs);
			}
		} else {
			if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
				dw_i2s_rx_clock_enable(i2s_rx_ctrl_ptr->dw_i2s_regs);
			} else {
				dw_i2s_tx_clock_enable(i2s_tx_ctrl_ptr->dw_i2s_regs);
			}
		}
		break;
	case I2S_CMD_MST_SET_WSS:
		DW_I2S_CHECK_EXP(i2s_info_ptr->mode == DEV_MASTER_MODE, E_NOSPT);
		val32 = (uint32_t)param;
		DW_I2S_CHECK_EXP(((val32 == DW_I2S_WSS_16_CLK) || \
				  (val32 == DW_I2S_WSS_24_CLK) || \
				  (val32 == DW_I2S_WSS_32_CLK)), E_PAR);
		if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
			dw_i2s_rx_clock_disable(i2s_rx_ctrl_ptr->dw_i2s_regs);
			dw_i2s_rx_set_wss(i2s_rx_ctrl_ptr->dw_i2s_regs, val32);
			i2s_config_ptr->ws_length = val32;
		} else {
			dw_i2s_tx_clock_disable(i2s_tx_ctrl_ptr->dw_i2s_regs);
			dw_i2s_tx_set_wss(i2s_tx_ctrl_ptr->dw_i2s_regs, val32);
			i2s_config_ptr->ws_length = val32;
		}
		break;
	case I2S_CMD_MST_SET_SCLKG:
		DW_I2S_CHECK_EXP(i2s_info_ptr->mode == DEV_MASTER_MODE, E_NOSPT);
		val32 = (uint32_t)param;
		DW_I2S_CHECK_EXP(((val32 == DW_I2S_SCLKG_NO) ||	    \
				  (val32 == DW_I2S_SCLKG_12_CLK) || \
				  (val32 == DW_I2S_SCLKG_16_CLK) || \
				  (val32 == DW_I2S_SCLKG_20_CLK) || \
				  (val32 == DW_I2S_SCLKG_24_CLK)), E_PAR);
		if (i2s_info_ptr->device == I2S_DEVICE_RECEIVER) {
			dw_i2s_rx_clock_disable(i2s_rx_ctrl_ptr->dw_i2s_regs);
			dw_i2s_rx_set_sclkg(i2s_rx_ctrl_ptr->dw_i2s_regs, val32);
			i2s_config_ptr->sclk_gate = val32;
		} else {
			dw_i2s_tx_clock_disable(i2s_tx_ctrl_ptr->dw_i2s_regs);
			dw_i2s_tx_set_sclkg(i2s_tx_ctrl_ptr->dw_i2s_regs, val32);
			i2s_config_ptr->sclk_gate = val32;
		}
		break;
	default:
		ercd = E_NOSPT;
		break;
	}

error_exit:
	return ercd;
}

/**
 * @brief	Poll transmit data through designware i2s as master or slave
 * @param[in]	i2s_obj	I2S device object pointer
 * @param[in]	data Data to send
 * @param[in]	len	Data length to send, no timeout check
 * @param[in]	channel Send data via specified channel
 * @retval	>0	Byte count that was successfully sent for poll method,
 * 			it might can't send that much due to @ref DEV_I2S_INFO::err_state "I2S_ERROR_STATE".
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_CTX	Device is still in transfer state
 * @retval	E_SYS	Can't write data to hardware due to hardware issues, such as device is disabled
 */
int32_t dw_i2s_write(DEV_I2S *i2s_obj, const void *data, uint32_t len, uint32_t channel)
{
	int32_t ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_TX_CTRL *i2s_tx_ctrl_ptr = (DW_I2S_TX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	DW_I2S_TX_REG *i2s_tx_regs_ptr = i2s_tx_ctrl_ptr->dw_i2s_regs;
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);

	uint32_t remain_cnt = 0;
	uint32_t remain_pro = 0;
	uint32_t trans_cnt = 0;
	int32_t error_state = I2S_ERR_NONE;

	#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *p_buf = (uint16_t *)data;
	#else
	uint32_t *p_buf = (uint32_t *)data;
	#endif

	/* START ERROR CHECK */
	VALID_CHK_I2S_INFO_OBJECT(i2s_info_ptr);
	DW_I2S_CHECK_EXP(i2s_info_ptr->opn_cnt > 0, E_CLSED);
	DW_I2S_CHECK_EXP(i2s_info_ptr->status & DEV_ENABLED, E_SYS);
	DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_TRANSMITTER, E_OBJ);
	DW_I2S_CHECK_EXP(data != NULL, E_PAR);

	DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
	/* END OF ERROR CHECK */

	remain_cnt = len;
	remain_pro = len;
	if (i2s_info_ptr->mode == DEV_MASTER_MODE) {                    /* Master mode transmit data */
		/* Start state Check */
		DW_I2S_CHECK_EXP(channel == DW_I2S_CHANNEL0, E_PAR);    // only support channel 0
		/* End state Check */
		/* Send data point to I2S control buffer */
		i2s_tx_ctrl_ptr->dw_i2s_buf.buf = p_buf;
		i2s_tx_ctrl_ptr->dw_i2s_buf.len = i2s_config_ptr->fifo_len;
		i2s_tx_ctrl_ptr->dw_i2s_buf.ofs = 0;
		i2s_info_ptr->cur_state = I2S_IN_TX;
		dw_i2s_enable_device(i2s_info_ptr, channel);
		dw_i2s_tx_clock_enable(i2s_tx_regs_ptr);
		while (remain_cnt != 0) {
			remain_cnt = dw_i2s_tx_write_samples(i2s_info_ptr, remain_pro, channel);
			// EMBARC_PRINTF("The while is ENTER!\n");
			while ((i2s_tx_regs_ptr->ISR0 & DW_I2S_ISRX_TXFE) == 0) {
				;
			}
			// EMBARC_PRINTF("The while is OK!\n");
			// delay(100);
			trans_cnt += (remain_pro - remain_cnt);
			if (remain_cnt == 0) {
				i2s_tx_ctrl_ptr->dw_i2s_buf.buf = &p_buf[trans_cnt - 1];
			} else {
				i2s_tx_ctrl_ptr->dw_i2s_buf.buf = &p_buf[trans_cnt];
			}
			remain_pro = remain_cnt;
		}
		// delay(20);
	} else {
		// Slave mode is not support in this version
		DW_I2S_CHECK_EXP(i2s_info_ptr->mode != DEV_SLAVE_MODE, E_SYS);
	}

	i2s_info_ptr->cur_state = I2S_FREE;
	i2s_info_ptr->err_state = error_state;

	ercd = trans_cnt;
error_exit:
	return ercd;
}

/**
 * @brief	Read data through designware i2s as master or slave
 * @param[in]	i2s_obj	I2S device object pointer
 * @param[out]	data	Data received (data must be uint32_t type), uint32_t(left) and uint32_t(right) alternating
 * @param[in]	len	I2S device channel to read
 * @param[in]	channel	Data length need to read
 * @retval	>0	Byte count that was successfully received for poll method,
 * 			it might can't send that much due to @ref DEV_I2S_INFO::err_state "I2S_ERROR_STATE".
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_CTX	Device is still in receive state
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_SYS	Can't read data from hardware due to hardware issues, such as device is disabled
 */
int32_t dw_i2s_read(DEV_I2S *i2s_obj, void *data, uint32_t len, uint32_t channel)
{
	int32_t ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_RX_CTRL *i2s_rx_ctrl_ptr = NULL;
	DW_I2S_RX_REG *i2s_rx_regs_ptr = NULL;

	uint32_t remain_cnt = 0;
	uint32_t remain_pro = 0;
	uint32_t trans_cnt = 0;
	int32_t error_state = I2S_ERR_NONE;

	#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *p_buf = (uint16_t *)data;
	#else
	uint32_t *p_buf = (uint32_t *)data;
	#endif
	i2s_rx_ctrl_ptr = (DW_I2S_RX_CTRL *)(i2s_info_ptr->i2s_ctrl);
	i2s_rx_regs_ptr = i2s_rx_ctrl_ptr->dw_i2s_regs;

	/* START ERROR CHECK */
	VALID_CHK_I2S_INFO_OBJECT(i2s_info_ptr);
	DW_I2S_CHECK_EXP(i2s_info_ptr->opn_cnt > 0, E_CLSED);
	DW_I2S_CHECK_EXP(i2s_info_ptr->status & DEV_ENABLED, E_SYS);
	DW_I2S_CHECK_EXP(i2s_info_ptr->device == I2S_DEVICE_RECEIVER, E_OBJ);
	DW_I2S_CHECK_EXP(data != NULL, E_PAR);
	DW_I2S_CHECK_EXP(i2s_info_ptr->cur_state == I2S_FREE, E_CTX);
	/* END OF ERROR CHECK */

	remain_cnt = len;
	remain_pro = len;
	if (i2s_info_ptr->mode == DEV_MASTER_MODE) {                    /* Master mode receive data */
		/* Start state Check */
		DW_I2S_CHECK_EXP(channel == DW_I2S_CHANNEL0, E_PAR);    // only support channel 0
		/* End state Check */
		/* Send data point to I2S control buffer */
		i2s_rx_ctrl_ptr->dw_i2s_buf.buf = p_buf;
		i2s_rx_ctrl_ptr->dw_i2s_buf.len = len;
		i2s_rx_ctrl_ptr->dw_i2s_buf.ofs = 0;
		i2s_info_ptr->cur_state = I2S_IN_RX;
		dw_i2s_enable_device(i2s_info_ptr, channel);
		dw_i2s_rx_clock_enable(i2s_rx_regs_ptr);
		while (remain_cnt != 0) {
			while ((i2s_rx_regs_ptr->ISR0 & DW_I2S_ISRX_RXDA) == 0) {
				;
			}
			remain_cnt = dw_i2s_rx_read_samples(i2s_info_ptr, remain_pro, channel);
			trans_cnt += remain_pro - remain_cnt;
			if (remain_cnt == 0) {
				i2s_rx_ctrl_ptr->dw_i2s_buf.buf = &p_buf[trans_cnt - 1];
			} else {
				i2s_rx_ctrl_ptr->dw_i2s_buf.buf = &p_buf[trans_cnt];
			}
			remain_pro = remain_cnt;
		}
	} else {
		// Slave mode is not support in this version
		DW_I2S_CHECK_EXP(i2s_info_ptr->mode != DEV_SLAVE_MODE, E_SYS);
	}

	i2s_info_ptr->cur_state = I2S_FREE;
	i2s_info_ptr->err_state = error_state;

	ercd = trans_cnt;
error_exit:
	return ercd;
}

/**
 * @brief	DesignWare i2s tx interrupt handler
 * @param[in]	i2s_obj	I2S device pointer
 * @param[in]	ptr		Extra information
 */
void dw_i2s_isr_tx(DEV_I2S *i2s_obj, void *ptr)
{
	int ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);

	uint32_t channels = i2s_config_ptr->channels;

	DW_I2S_CHECK_EXP((i2s_info_ptr->cur_state == I2S_FREE), E_CTX)
	/* END OF ERROR CHECK */
	DW_I2S_CHECK_EXP(((channels & DW_I2S_CHANNEL0_SUPPORTED) == 1) && \
			 ((channels & DW_I2S_CHANNEL1_SUPPORTED) == 0) && \
			 ((channels & DW_I2S_CHANNEL2_SUPPORTED) == 0) && \
			 ((channels & DW_I2S_CHANNEL3_SUPPORTED) == 0), E_PAR);
	/* End state Check */

	i2s_info_ptr->cur_state = I2S_IN_TX;
	dw_i2s_mst_int_write(i2s_obj);
error_exit:
	return;
}

/**
 * @brief	DesignWare i2s rx interrupt handler
 * @param[in]	i2s_obj	I2S device pointer
 * @param[in]	ptr		extra information
 */
void dw_i2s_isr_rx(DEV_I2S *i2s_obj, void *ptr)
{
	int ercd = E_OK;
	DEV_I2S_INFO *i2s_info_ptr = &(i2s_obj->i2s_info);
	DW_I2S_CONFIG *i2s_config_ptr = (DW_I2S_CONFIG *)(i2s_info_ptr->i2s_config);

	uint32_t channels = i2s_config_ptr->channels;

	DW_I2S_CHECK_EXP((i2s_info_ptr->cur_state == I2S_FREE), E_CTX)
	/* END OF ERROR CHECK */
	DW_I2S_CHECK_EXP(((channels & DW_I2S_CHANNEL0_SUPPORTED) == 1) && \
			 ((channels & DW_I2S_CHANNEL1_SUPPORTED) == 0) && \
			 ((channels & DW_I2S_CHANNEL2_SUPPORTED) == 0) && \
			 ((channels & DW_I2S_CHANNEL3_SUPPORTED) == 0), E_PAR);
	/* End state Check */

	i2s_info_ptr->cur_state = I2S_IN_RX;
	dw_i2s_mst_int_rx(i2s_obj);
error_exit:
	return;
}
/** @} */
