/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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

/* the wrapper of subsystem i2s master driver */
#include "embARC_error.h"
#include "embARC_toolchain.h"

#include "device/subsystem/i2s_rx_master.h"
#include "device/subsystem/i2s_tx_master.h"
#include "device/subsystem/ss_i2s_master.h"

/** check expressions used in DFSS I2S driver implementation */
#define SS_I2S_MASTER_CHECK_EXP(EXPR, ERROR_CODE)       CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

/* I2S Master device registers */
#define I2S_IER         0x000           /*!< (0x000) : Enable Register */
#define I2S_IRER        0x004           /*!< (0x004) : I2S Receiver Block Enable Register */
#define I2S_ITER        0x008
#define I2S_CER         0x00c           /*!< (0x00C) : Clock Enable Register */
#define I2S_CCR         0x010           /*!< (0x010) : Clock Configuration Register */
#define I2S_RXFFR       0x014           /*!< (0x014) : Receiver Block FIFO Register */
#define I2S_TXFFR       0x018
#define I2S_LTHR0       0x020           /*!< (0x020) : Left Transmit Holding Register 0 */
#define I2S_RTHR0       0x024           /*!< (0x024) : Right Transmit Holding Register 0 */
#define I2S_RER0        0x028           /*!< (0x028) : Receive Enable Register 0 */
#define I2S_TER0        0x02C
#define I2S_RCR0        0x030           /*!< (0x030) : Receive Configuration Register 0 */
#define I2S_TCR0        0x034
#define I2S_ISR0        0x038           /*!< (0x038) : Interrupt Status Register 0 */
#define I2S_IMR0        0x03c           /*!< (0x03C) : Interrupt Mask Register 0 */
#define I2S_ROR0        0x040           /*!< (0x040) : Receive Overrun Register 0 */
#define I2S_TOR0        0x044
#define I2S_RFCR0       0x048           /*!< (0x048) : Receive FIFO Configuration Register 0 */
#define I2S_RFF0        0x050           /*!< (0x050) : Receive FIFO Flush 0 */
#define I2S_TFF0        0x054
#define I2S_RXDMA       0x1c0           /*!< (0x1C0) : Receiver Block DMA Register */
#define I2S_TXDMA       0x1c8

#define REG_READ(x)         arc_aux_read((ctx->reg_base + x))
#define REG_WRITE(x, y)     arc_aux_write((ctx->reg_base + x), y)

static void ss_i2s_master_reset(SS_I2S_MST_DEV_CONTEXT *ctx)
{

}

static void ss_i2s_master_flush_tx(SS_I2S_MST_DEV_CONTEXT *ctx)
{

}

static void ss_i2s_master_flush_rx(SS_I2S_MST_DEV_CONTEXT *ctx)
{

}

static void ss_i2s_master_enable(SS_I2S_MST_DEV_CONTEXT *ctx)
{
	// enable device
	REG_WRITE(I2S_IER, 1);
}

static void ss_i2s_master_disable(SS_I2S_MST_DEV_CONTEXT *ctx)
{
	// disable device
	REG_WRITE(I2S_IER, 0);
}

int32_t ss_i2s_mst_open(SS_I2S_MST_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param)
{
	uint32_t dev_id = ctx->dev_id;
	uint8_t io_mode = ctx->io_mode;
	DEV_I2S_INFO_PTR info = ctx->info;
	io_cb_t callback;
	int32_t int_e = 0;

	if (mode != DEV_MASTER_MODE) {
		return E_PAR;
	}

	if (info->opn_cnt > 0) {
		info->opn_cnt++;
		return E_OPNED;
	}
	info->opn_cnt++;

	if (io_mode == SS_I2S_RX) {
		if (io_i2s_rx_master_open(dev_id)) {
			return E_SYS;
		}
		callback.cb = (IO_CB_FUNC)ctx->int_rx_cb;
		int_e = int_e | int_enable(ctx->int_rx_avil);
		int_e = int_e | int_enable(ctx->int_rx_err);
		io_i2s_rx_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
	} else if (io_mode == SS_I2S_TX) {
		if (io_i2s_tx_master_open(dev_id)) {
			return E_SYS;
		}
		callback.cb = (IO_CB_FUNC)ctx->int_tx_cb;
		io_i2s_tx_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
		int_e = int_e | int_enable(ctx->int_tx_req);
		int_e = int_e | int_enable(ctx->int_tx_err);
	} else {
		return E_SYS;
	}

	callback.cb = (IO_CB_FUNC)ctx->int_err_cb;
	io_i2s_tx_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);

	if (int_e != 0) {
		return E_PAR;
	}
	return E_OK;
}

int32_t ss_i2s_mst_close(SS_I2S_MST_DEV_CONTEXT *ctx)
{
	DEV_I2S_INFO_PTR info = ctx->info;
	uint8_t io_mode = ctx->io_mode;

	info->opn_cnt--;
	if (info->opn_cnt == 0) {
		if (io_mode == SS_I2S_RX) {
			int_disable(ctx->int_rx_avil);
			int_disable(ctx->int_rx_err);
			io_i2s_rx_master_close(ctx->dev_id);
		} else if (io_mode == SS_I2S_TX) {
			int_disable(ctx->int_tx_req);
			int_disable(ctx->int_tx_err);
			io_i2s_tx_master_close(ctx->dev_id);
		} else {
			return E_SYS;
		}
		info->opn_cnt = 0;
		info->status = DEV_DISABLED;
		ctx->flags = 0;
	} else {
		return E_OPNED;
	}
	return E_OK;
}

int32_t ss_i2s_mst_read(SS_I2S_MST_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint8_t io_mode = ctx->io_mode;
	uint32_t rd_len = len;

	if (ctx->flags) {
		return E_NORES;
	}
	if (io_mode == SS_I2S_TX) {
		return E_OBJ;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * because the bottom driver is simply interrupt driven
		 */
		return E_SYS;
	}
	ctx->flags = SS_I2S_MASTER_FLAG_RX;
	io_i2s_rx_master_read(dev_id, data, &rd_len);
	/* wait finished: i2s master int enable & no cpu lock */
	while (ctx->flags & SS_I2S_MASTER_FLAG_RX) {
		;
	}

	if (ctx->flags & SS_I2S_MASTER_FLAG_RX_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}
	return rd_len;
}

int32_t ss_i2s_mst_write(SS_I2S_MST_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint8_t io_mode = ctx->io_mode;
	uint32_t wt_len = len;

	if (ctx->flags) {
		return E_NORES;
	}
	if (io_mode == SS_I2S_RX) {
		return E_OBJ;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * because the bottom driver is simply interrupt driven
		 */
		return E_SYS;
	}
	ctx->flags = SS_I2S_MASTER_FLAG_TX;
	io_i2s_tx_master_write(dev_id, data, &wt_len);
	/* wait finished: i2s master int enable & no cpu lock */
	while (ctx->flags & SS_I2S_MASTER_FLAG_TX) {
		;
	}

	if (ctx->flags & SS_I2S_MASTER_FLAG_TX_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}
	return wt_len;
}

int32_t ss_i2s_mst_control(SS_I2S_MST_DEV_CONTEXT *ctx, uint32_t cmd, void *param)
{
	uint32_t dev_id = ctx->dev_id;
	uint8_t io_mode = ctx->io_mode;
	DEV_I2S_INFO_PTR info = ctx->info;
	int32_t ercd = E_OK;

	io_i2s_rx_master_ioctl(dev_id, cmd, param);
	io_i2s_tx_master_ioctl(dev_id, cmd, param);

	switch (cmd) {
	case I2S_CMD_GET_STATUS:
		SS_I2S_MASTER_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		*((int32_t *)param) = info->status;
		break;
	case I2S_CMD_SET_TXCB:
		if (io_mode == SS_I2S_TX) {
			SS_I2S_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			io_i2s_tx_master_ioctl(dev_id, IO_SET_CB_TX, param);
		}
		break;
	case I2S_CMD_SET_RXCB:
		if (io_mode == SS_I2S_RX) {
			SS_I2S_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			io_i2s_rx_master_ioctl(dev_id, IO_SET_CB_RX, param);
		}
		break;
	case I2S_CMD_SET_ERRCB:
		if (io_mode == SS_I2S_RX) {
			SS_I2S_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			io_i2s_rx_master_ioctl(dev_id, IO_SET_CB_ERR, param);
		} else if (io_mode == SS_I2S_TX) {
			SS_I2S_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			io_i2s_tx_master_ioctl(dev_id, IO_SET_CB_ERR, param);
		}
		break;
	case I2S_CMD_SET_TXINT: break;
	case I2S_CMD_SET_RXINT: break;
	case I2S_CMD_RESET:
		ss_i2s_master_reset(ctx);
		break;
	case I2S_CMD_FLUSH_TX:
		ss_i2s_master_flush_tx(ctx);
		break;
	case I2S_CMD_FLUSH_RX:
		ss_i2s_master_flush_rx(ctx);
		break;
	case I2S_CMD_ENA_DEV:
		ss_i2s_master_enable(ctx);
		break;
	case I2S_CMD_DIS_DEV:
		ss_i2s_master_disable(ctx);
		break;
	case I2S_CMD_SET_TXINT_BUF: break;
	case I2S_CMD_SET_RXINT_BUF: break;
	case I2S_CMD_SET_TXCHET_BUF: break;
	case I2S_CMD_SET_RXCHDT_BUF: break;
	case I2S_CMD_MST_SET_CLK: break;
	case I2S_CMD_MST_SET_WSS: break;
	case I2S_CMD_MST_SET_SCLKG: break;
	default:
		ercd = E_NOSPT;
		break;
	}

error_exit:
	return ercd;
}

void ss_i2s_mst_int_tx_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;

	if (ctx->flags & SS_I2S_MASTER_FLAG_TX) {
		ctx->flags &= ~SS_I2S_MASTER_FLAG_TX;
		if (info->i2s_cbs.tx_cb) {
			info->i2s_cbs.tx_cb(info);
		}
	}
}

void ss_i2s_mst_int_rx_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;

	if (ctx->flags & SS_I2S_MASTER_FLAG_RX) {
		ctx->flags &= ~SS_I2S_MASTER_FLAG_RX;
		if (info->i2s_cbs.rx_cb) {
			info->i2s_cbs.rx_cb(info);
		}
	}
}

void ss_i2s_mst_int_err_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param)
{
	DEV_I2S_INFO_PTR info = ctx->info;
	uint8_t io_mode = ctx->io_mode;
	uint32_t flag;

	flag = (io_mode == SS_I2S_RX) ? SS_I2S_MASTER_FLAG_RX_ERROR : SS_I2S_MASTER_FLAG_TX_ERROR;

	if (ctx->flags & flag) {
		ctx->flags &= ~flag;
		if (info->i2s_cbs.err_cb) {
			info->i2s_cbs.err_cb(info);
		}
	}
}
