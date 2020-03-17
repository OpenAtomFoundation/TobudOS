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
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc/arc_exception.h"
#include "board.h"
#include "device/designware/dw_sdio_hal.h"
#include "device/designware/dw_sdio.h"
#include "embARC_debug.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @fn void dw_sdio_enable_card_threshold(DW_SDIO_CTRL_PTR sdio, uint32_t threshold)
 */
Inline void dw_sdio_enable_card_threshold(DW_SDIO_CTRL_PTR sdio, uint32_t threshold)
{
	/* according to datasheet, write threshold is only applicable for HS400 mode */
	uint32_t reg;

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_CARD_THR_CTRL);
	reg |= 0x1;
	dw_sdio_reg_write(sdio, DWSDIO_REG_CARD_THR_CTRL, reg);
}

/**
 * @fn void dw_sdio_block_size_set(DW_SDIO_CTRL_PTR sdio, uint32_t bytes, uint32_t blksz)
 */
Inline void dw_sdio_block_size_set(DW_SDIO_CTRL_PTR sdio, uint32_t bytes, uint32_t blksz)
{
	dw_sdio_reg_write(sdio, DWSDIO_REG_BYTCNT, bytes);
	dw_sdio_reg_write(sdio, DWSDIO_REG_BLKSIZ, blksz);
}

/**
 * @fn void dw_sdio_fifo_read_poll(DW_SDIO_CTRL_PTR sdio, uint32_t *buf, uint32_t size)
 */
Inline void dw_sdio_fifo_read_poll(DW_SDIO_CTRL_PTR sdio, uint32_t *buf, uint32_t size)
{
	uint32_t len;
	uint32_t i;

	while (size) {
		do {
			len = dw_sdio_reg_read(sdio, DWSDIO_REG_STATUS);
		} while (len & DWSDIO_STATUS_FIFO_EMPTY);

		len = (len & DWSDIO_STATUS_MASK_FIFO) >> DWSDIO_STATUS_BIT_FIFO;

		len = min(size, len);

		for (i = 0; i < len; i++) {
			*buf++ = dw_sdio_reg_read(sdio, DWSDIO_REG_DATA);
		}
		size = size > len ? (size - len) : 0;
	}
}

/**
 * @fn void dw_sdio_clk_enable(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
 */
Inline void dw_sdio_clk_enable(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
{
	uint32_t reg;

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_CLKENA);
	reg |= (1 << card_number);
	dw_sdio_reg_write(sdio, DWSDIO_REG_CLKENA, reg);
}

/**
 * @fn dw_sdio_clk_disable(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
 */
Inline void dw_sdio_clk_disable(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
{
	uint32_t reg;

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_CLKENA);
	reg &= ~(1 << card_number);
	dw_sdio_reg_write(sdio, DWSDIO_REG_CLKENA, reg);
}

/**
 * @fn void dw_sdio_fifo_write_poll(DW_SDIO_CTRL_PTR sdio, uint32_t *buf, uint32_t size)
 */
Inline void dw_sdio_fifo_write_poll(DW_SDIO_CTRL_PTR sdio, uint32_t *buf, uint32_t size)
{
	uint32_t len;
	uint32_t i;
	uint32_t fifo_depth = sdio->fifo_depth;

	while (size) {
		do {
			len = dw_sdio_reg_read(sdio, DWSDIO_REG_STATUS);
		} while (len & DWSDIO_STATUS_FIFO_FULL);

		len = fifo_depth - ((len & DWSDIO_STATUS_MASK_FIFO) >> DWSDIO_STATUS_BIT_FIFO);
		len = min(size, len);

		for (i = 0; i < len; i++) {
			dw_sdio_reg_write(sdio, DWSDIO_REG_DATA, *buf++);
		}
		size = size > len ? (size - len) : 0;
	}
}

/**
 * @fn void dw_sdio_power_on(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
 */
Inline void dw_sdio_power_on(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
{
	dw_sdio_reg_write(sdio, DWSDIO_REG_PWREN, (1 << card_number));
}

/**
 * @fn void dw_sdio_power_off(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
 */
Inline void dw_sdio_power_off(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
{
	uint32_t reg = dw_sdio_reg_read(sdio, DWSDIO_REG_PWREN);

	reg &= ~(1 << card_number);
	dw_sdio_reg_write(sdio, DWSDIO_REG_PWREN, reg);
}

/**
 * @fn void dw_sdio_reset(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
 */
Inline void dw_sdio_reset(DW_SDIO_CTRL_PTR sdio, uint32_t card_number)
{
	uint32_t reg;

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_RST_N);
	reg &= ~(1 << card_number);
	dw_sdio_reg_write(sdio, DWSDIO_REG_RST_N, reg);
	reg |= (1 << card_number);
	dw_sdio_reg_write(sdio, DWSDIO_REG_RST_N, reg);
}

/**
 * @fn void dw_sdio_bus_type_set(DW_SDIO_CTRL_PTR sdio, uint32_t card_number, uint32_t width)
 */
Inline void dw_sdio_bus_type_set(DW_SDIO_CTRL_PTR sdio, uint32_t card_number, uint32_t width)
{
	uint32_t reg;

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_CTYPE);

	reg &= ~((1 << card_number) | (1 << (card_number + 16)));

	switch (width) {
	case 1: break;
	case 4: reg |= (1 << card_number); break;
	case 8: reg |= (1 << (card_number + 16)); break;
	default: break;
	}

	dw_sdio_reg_write(sdio, DWSDIO_REG_CTYPE, reg);
}

/**
 * @fn void dw_sdio_ref_clk_set(DW_SDIO_CTRL_PTR sdio,  uint32_t ref_clk)
 */
Inline void dw_sdio_ref_clk_set(DW_SDIO_CTRL_PTR sdio,  uint32_t ref_clk)
{
	/* 0: 25 Mhz, 1: 50 Mhz, 2: 100 Mhz */
	uint32_t reg;

	if (ref_clk == 50000000) {
		ref_clk = 1;
	} else if (ref_clk == 100000000) {
		ref_clk = 2;
	} else {
		ref_clk = 0;
	}

	reg = dw_sdio_reg_read(sdio, DWSDIO_REG_UHS_REG_EXT);
	reg &= ~(DWSDIO_UHS_REG_EXT_MASK_REFCLK);
	reg |= (ref_clk << DWSDIO_UHS_REG_EXT_BIT_REFCLK);
	dw_sdio_reg_write(sdio, DWSDIO_REG_UHS_REG_EXT, reg);
}

static int32_t dw_sdio_bus_freq_set(DW_SDIO_CTRL_PTR sdio, uint32_t card_number, uint32_t freq)
{
	uint32_t clk;
	uint32_t div;
	uint32_t status;
	int32_t timeout = 10000;

	clk = sdio->ref_clk;

	if (freq == clk) {
		div = 0;
	} else {
		div = DIV_ROUND_UP(clk, 2 * freq);
	}

	dw_sdio_clk_disable(sdio, card_number);

	dw_sdio_reg_write(sdio, DWSDIO_REG_CLKSRC, 0);
	dw_sdio_reg_write(sdio, DWSDIO_REG_CLKDIV, div);

	dw_sdio_reg_write(sdio, DWSDIO_REG_CMD, DWSDIO_CMD_PRV_DAT_WAIT |
			  DWSDIO_CMD_UPD_CLK | DWSDIO_CMD_START |  DWSDIO_CMD_USE_HOLD_REG);

	do {
		status = dw_sdio_reg_read(sdio, DWSDIO_REG_CMD);
		if (timeout-- < 0) {
			DBG("%s: Timeout!\n", __func__);
			return E_TMOUT;
		}
	} while (status & DWSDIO_CMD_START);

	dw_sdio_clk_enable(sdio, card_number);

	dw_sdio_reg_write(sdio, DWSDIO_REG_CMD, DWSDIO_CMD_PRV_DAT_WAIT |
			  DWSDIO_CMD_UPD_CLK | DWSDIO_CMD_START |  DWSDIO_CMD_USE_HOLD_REG);

	timeout = 10000;
	do {
		status = dw_sdio_reg_read(sdio, DWSDIO_REG_CMD);
		if (timeout-- < 0) {
			DBG("%s: Timeout!\n", __func__);
			return E_TMOUT;
		}
	} while (status & DWSDIO_CMD_START);

	return E_OK;
}

static int32_t dw_sdio_reset_wait(DW_SDIO_CTRL_PTR sdio, uint32_t reset_val)
{
	uint32_t try = 1000;
	uint32_t ctrl;

	dw_sdio_reg_write(sdio, DWSDIO_REG_CTRL, reset_val);

	while (try--) {
		ctrl = dw_sdio_reg_read(sdio, DWSDIO_REG_CTRL);
		if (!(ctrl & DWSDIO_CTRL_RESET_ALL)) {
			return E_SYS;
		}
	}

	return E_OK;
}

static int32_t dw_sdio_cmd_set(DW_SDIO_CTRL_PTR sdio, SDIO_CMD_PTR cmd, SDIO_DATA_PTR data)
{
	uint32_t reg = 0;

	dw_sdio_reg_write(sdio, DWSDIO_REG_CMDARG, cmd->arg);

	if (data) {
		reg |= DWSDIO_CMD_DATA_EXP;

		if (data->flags & SDIO_DATA_WRITE) {
			reg |= DWSDIO_CMD_RW;
		}
	}

	if ((cmd->resp_type & SDIO_RSP_136) && (cmd->resp_type & SDIO_RSP_BUSY)) {
		return -1;
	}

	/*  to fix */
	if (cmd->cmdidx == 12) {
		reg |= DWSDIO_CMD_ABORT_STOP;
	} else {
		reg |= DWSDIO_CMD_PRV_DAT_WAIT;
	}

	if (cmd->resp_type & SDIO_RSP_PRESENT) {
		reg |= DWSDIO_CMD_RESP_EXP;
		if (cmd->resp_type & SDIO_RSP_136) {
			reg |= DWSDIO_CMD_RESP_LENGTH;
		}
	}

	if (cmd->resp_type & SDIO_RSP_CRC) {
		reg |= DWSDIO_CMD_CHECK_CRC;
	}

	reg |= (cmd->cmdidx | (cmd->card << DWSDIO_CMD_BIT_CARD_NO) | DWSDIO_CMD_START
		| DWSDIO_CMD_USE_HOLD_REG);

	DBG("Sending CMD%d to card %d \r\n", cmd->cmdidx, cmd->card);

	dw_sdio_reg_write(sdio, DWSDIO_REG_CMD, reg);

	return 0;
}

static int32_t dw_sdio_data_transfer_poll(DW_SDIO_CTRL_PTR sdio, SDIO_DATA_PTR data)
{
	uint32_t size;
	uint32_t status;
	uint32_t timeout = 1000;
	uint32_t start = GET_CUR_MS();

	size = data->blksz * data->blkcnt / 4;

	for (;;) {
		status = dw_sdio_reg_read(sdio, DWSDIO_REG_RINTSTS);

		if (status & (DWSDIO_INT_DATA_ERR | DWSDIO_INT_DATA_TMO)) {
			DBG("%s:, data transfer error!\r\n", __func__);
			return E_SYS;
		}

		if (status & DWSDIO_INT_DTO) {
			return E_OK;
		}

		if (data->flags == SDIO_DATA_READ && (status & DWSDIO_INT_RXDR) && size) {
			dw_sdio_fifo_read_poll(sdio, (uint32_t *)data->in, size);
			size = 0;
			dw_sdio_reg_write(sdio, DWSDIO_REG_RINTSTS, DWSDIO_INT_RXDR);
		} else if (data->flags == SDIO_DATA_WRITE && (status & DWSDIO_INT_TXDR) && size) {
			dw_sdio_fifo_write_poll(sdio, (uint32_t *)data->out, size);
			size = 0;
			dw_sdio_reg_write(sdio, DWSDIO_REG_RINTSTS, DWSDIO_INT_TXDR);
		}

		if ((GET_CUR_MS() - start) > timeout) {
			DBG("%s: timeout on data transfer\r\n", __func__);
			return E_TMOUT;
		}
	}
}

void dw_sdio_isr(DEV_SDIO *sdio_obj, void *ptr)
{
	uint32_t int_status;

	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	int_status = dw_sdio_reg_read(sdio, DWSDIO_REG_MINTSTS);

	if (int_status & DWSDIO_INT_CAD) {
		DBG("SDIO card detected\r\n");
	}

	if (int_status & DWSDIO_INT_RE) {
		DBG("SDIO response error\r\n");
	}

	if (int_status & DWSDIO_INT_CD) {
		DBG("SDIO cmd done\r\n");
	}

	if (int_status & DWSDIO_INT_TXDR) {
		DBG("SDIO tx data request\r\n");
	}

	if (int_status & DWSDIO_INT_RXDR) {
		DBG("SDIO rx data request\r\n");
	}

	/* clear interrupt */
	dw_sdio_reg_write(sdio, DWSDIO_REG_RINTSTS, int_status);

	/* \todo dma based interrupt */
}

int32_t dw_sdio_cmd_poll(DEV_SDIO *sdio_obj, SDIO_CMD_PTR cmd, SDIO_DATA_PTR data)
{
	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	uint32_t start = GET_CUR_MS();
	uint32_t timeout = 500;
	uint32_t retry = 100000;
	uint32_t mask;
	uint32_t i;

	while (dw_sdio_reg_read(sdio, DWSDIO_REG_STATUS) & DWSDIO_STATUS_DATA_BUSY) {
		if ((GET_CUR_MS() - start) > timeout) {
			DBG("%s: timeout on data busy\r\n", __func__);
			return E_TMOUT;
		}
	}

	dw_sdio_reg_write(sdio, DWSDIO_REG_RINTSTS, DWSDIO_INT_ALL);

	if (data) {
		dw_sdio_enable_card_threshold(sdio, 1);
		dw_sdio_block_size_set(sdio, data->blksz * data->blkcnt, data->blksz);
		dw_sdio_reset_wait(sdio, DWSDIO_CTRL_RESET_FIFO);
	}

	if (dw_sdio_cmd_set(sdio, cmd, data) < 0) {
		return E_SYS;
	}

	for (i = 0; i < retry; i++) {
		mask = dw_sdio_reg_read(sdio, DWSDIO_REG_RINTSTS);
		if (mask & DWSDIO_INT_CD) {
			if (!data) {
				dw_sdio_reg_write(sdio, DWSDIO_REG_RINTSTS, mask);
			}
			break;
		}
	}

	if (i == retry) {
		DBG("%s: Timeout.\n", __func__);
		return E_TMOUT;
	}

	if (mask & DWSDIO_INT_RTO) {
		/*
		 * Timeout here is not necessarily fatal. (e)MMC cards
		 * will splat here when they receive CMD55 as they do
		 * not support this command and that is exactly the way
		 * to tell them apart from SD cards. Thus, this output
		 * below shall be debug(). eMMC cards also do not favor
		 * CMD8, please keep that in mind.
		 */
		DBG("%s: Response Timeout:%x.\r\n", __func__, mask);
		return E_TMOUT;
	} else if (mask & DWSDIO_INT_RE) {
		DBG("%s: Response Error:%x.\r\n", __func__, mask);
		return E_SYS;
	}

	if (cmd->resp_type & SDIO_RSP_PRESENT) {
		if (cmd->resp_type & SDIO_RSP_136) {
			cmd->resp[0] = dw_sdio_reg_read(sdio, DWSDIO_REG_RESP3);
			cmd->resp[1] = dw_sdio_reg_read(sdio, DWSDIO_REG_RESP2);
			cmd->resp[2] = dw_sdio_reg_read(sdio, DWSDIO_REG_RESP1);
			cmd->resp[3] = dw_sdio_reg_read(sdio, DWSDIO_REG_RESP0);
		} else {
			cmd->resp[0] = dw_sdio_reg_read(sdio, DWSDIO_REG_RESP0);
		}
	}

	if (data) {
		return dw_sdio_data_transfer_poll(sdio, data);
	}

	return E_OK;
}

int32_t dw_sdio_open(DEV_SDIO *sdio_obj, uint32_t card_number)
{
	uint32_t fifo_depth;
	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	if (sdio_info_ptr->opn_cnt > 0) {
		dw_sdio_power_on(sdio, card_number);
		dw_sdio_reset(sdio, card_number);
		dw_sdio_bus_freq_set(sdio, card_number, DWSDIO_ENUMERATION_FREQ);

		sdio_info_ptr->opn_cnt++;
		return E_OPNED;
	}

	sdio_info_ptr->opn_cnt++;

	dw_sdio_reset_wait(sdio, DWSDIO_CTRL_RESET_ALL);
	dw_sdio_ref_clk_set(sdio, sdio->ref_clk);

	dw_sdio_power_on(sdio, card_number);
	dw_sdio_reset(sdio, card_number);

	dw_sdio_bus_type_set(sdio, card_number, 1);
	dw_sdio_bus_freq_set(sdio, card_number, DWSDIO_ENUMERATION_FREQ);

	if (sdio->fifo_depth == 0) {
		fifo_depth = dw_sdio_reg_read(sdio, DWSDIO_REG_FIFOTH);
		fifo_depth = ((fifo_depth & DWSDIO_FIFOTH_MASK_RX_WMARK) >>
			      DWSDIO_FIFOTH_BIT_RX_WMARK) + 1;
		sdio->fifo_depth = fifo_depth;
	}

	fifo_depth = DWSDIO_FIFOTH_M_SIZE(0x2) |  DWSDIO_FIFOTH_RX_WMASK(sdio->fifo_depth / 2 - 1) |
		     DWSDIO_FIFOTH_TX_WMASK(sdio->fifo_depth / 2);
	dw_sdio_reg_write(sdio, DWSDIO_REG_FIFOTH, fifo_depth);

	return E_OK;
}

int32_t dw_sdio_close(DEV_SDIO *sdio_obj, uint32_t card_number)
{
	int32_t ret = E_OK;
	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	sdio_info_ptr->opn_cnt--;

	dw_sdio_power_off(sdio, card_number);
	if (sdio_info_ptr->opn_cnt == 0) {
		dw_sdio_reset_wait(sdio, DWSDIO_CTRL_RESET_ALL);
	} else {
		ret = E_OPNED;
	}

	return ret;
}

int32_t dw_sdio_cd(DEV_SDIO *sdio_obj, uint32_t card_number)
{
	uint32_t val;

	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	/* the card detect depends on the specific PCB lay out.
	 * Some use 1 (CD pin to VDD) to indicate card detect.
	 * Some use 0 (CD pin to GNU) to indicate card detect.
	 */
#ifdef DWSDIO_CARD_DETECT_HIGH_LEVEL
	val = dw_sdio_reg_read(sdio, DWSDIO_REG_CDETECT);
#else
	val = ~dw_sdio_reg_read(sdio, DWSDIO_REG_CDETECT);
#endif

	if ((val & (1 << card_number)) == (1 << card_number)) {
		return 1; /* the specific card detected */
	}

	return 0;
}

int32_t dw_sdio_wp(DEV_SDIO *sdio_obj, uint32_t card_number)
{
	uint32_t val;

	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	val = dw_sdio_reg_read(sdio, DWSDIO_REG_WRTPRT);

	if ((val & (1 << card_number)) == (1 << card_number)) {
		return 1; /* the specific card is write-protect */
	}

	return 0;
}

int32_t dw_sdio_control(DEV_SDIO *sdio_obj, SDIO_CTRL_CMD_PTR ctrl_cmd, void *param)
{
	int32_t ret = E_OK;
	uint32_t cmd, card;

	DEV_SDIO_INFO_PTR sdio_info_ptr = &(sdio_obj->sdio_info);
	DW_SDIO_CTRL_PTR sdio = (DW_SDIO_CTRL_PTR)sdio_info_ptr->sdio_ctrl;

	cmd = ctrl_cmd->cmd;
	card = ctrl_cmd->card;

	switch (cmd) {
	case SDIO_CMD_SET_BUS_WIDTH:
		dw_sdio_bus_type_set(sdio, card, (uint32_t)param);
		break;
	case SDIO_CMD_SET_BUS_FREQ:
		dw_sdio_bus_freq_set(sdio, card, (uint32_t)param);
		break;
	/* \todo add more cmds */
	default:
		ret = E_PAR;
	}
	return ret;
}