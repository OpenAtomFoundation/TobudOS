/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1) Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */

#include "embARC_error.h"
#include "embARC_toolchain.h"
#include "arc/arc_exception.h"

#include "device/subsystem/spi_master.h"
#include "device/subsystem/ss_spi_master.h"

int32_t ss_spi_master_open(SS_SPI_MASTER_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param)
{
	uint32_t dev_id = ctx->dev_id;
	DEV_SPI_INFO *info = ctx->info;
	io_cb_t callback;
	uint32_t val;

	if (mode != DEV_MASTER_MODE) {
		return E_PAR;
	}

	if (info->opn_cnt > 0) {
		if (param != info->freq) {
			return E_OPNED;
		} else {
			return E_OK;
		}
	}

	if (io_spi_master_open(dev_id)) {
		return E_SYS;
	}

	callback.cb = (IO_CB_FUNC)ctx->int_rx_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
	callback.cb = (IO_CB_FUNC)ctx->int_tx_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
	callback.cb = (IO_CB_FUNC)ctx->int_err_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_DFS, &info->dfs);
	val = 1;
	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_PACKING, &val);

	info->freq = param;
	param = ctx->bus_freq / param;

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_BAUD, &param);
	info->opn_cnt++;
	ctx->flags = 0;

/* cs pin will toggle when FIFO is empty, to avoid this, spi master operation
 * should not be interruptted by others. so set int pri to INT_PRI_MIN
 */
	int_enable(ctx->intno_rx);
	int_pri_set(ctx->intno_rx, INT_PRI_MIN);
	int_enable(ctx->intno_tx);
	int_pri_set(ctx->intno_tx, INT_PRI_MIN);
	int_enable(ctx->intno_idle);
	int_pri_set(ctx->intno_idle, INT_PRI_MIN);
	int_enable(ctx->intno_err);
	int_pri_set(ctx->intno_err, INT_PRI_MIN);

	return E_OK;
}

int32_t ss_spi_master_close(SS_SPI_MASTER_DEV_CONTEXT *ctx)
{
	DEV_SPI_INFO *info = ctx->info;

	info->opn_cnt--;
	if (info->opn_cnt == 0) {
		io_spi_master_close(ctx->dev_id);
		int_disable(ctx->intno_rx);
		int_disable(ctx->intno_tx);
		int_disable(ctx->intno_idle);
		int_disable(ctx->intno_err);
		info->status = DEV_DISABLED;
		ctx->flags = 0;
	} else {
		return E_OPNED;
	}
	return E_OK;
}

int32_t ss_spi_master_control(SS_SPI_MASTER_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	uint32_t dev_id = ctx->dev_id;
	DEV_SPI_INFO *info = ctx->info;
	uint32_t val32 = (uint32_t)param;
	DEV_BUFFER *buf = (DEV_BUFFER *)param;
	DEV_SPI_TRANSFER *spi_xfer = &(info->xfer);

	switch (ctrl_cmd) {
	case SPI_CMD_SET_TXINT_BUF:
		if (buf && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			DEV_SPI_XFER_SET_TXBUF(spi_xfer, buf->buf, buf->ofs, buf->len);
			ctx->flags |= SS_SPI_MASTER_FLAG_TX_READY;
		} else {
			return E_NOSPT;
		}
		break;
	case SPI_CMD_SET_TXINT:
		if (val32 && (ctx->flags & SS_SPI_MASTER_FLAG_TX_READY) && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			ctx->flags |= SS_SPI_MASTER_FLAG_BUSY;
			val32 = SPI_TRANSMIT_ONLY_MODE;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
			io_spi_master_write(dev_id, spi_xfer->tx_buf + spi_xfer->tx_ofs,
					    &spi_xfer->tx_len);
		} else {
			return E_SYS;
		}
		break;

	case SPI_CMD_SET_RXINT_BUF:
		if (buf && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			DEV_SPI_XFER_SET_RXBUF(spi_xfer, buf->buf, buf->ofs, buf->len);
			ctx->flags |= SS_SPI_MASTER_FLAG_RX_READY;
		} else {
			return E_NOSPT;
		}
		break;

	case SPI_CMD_SET_RXINT:
		if (val32 && (ctx->flags & SS_SPI_MASTER_FLAG_RX_READY) && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			ctx->flags |= SS_SPI_MASTER_FLAG_BUSY;
			val32 = SPI_RECEIVE_ONLY_MODE;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
			io_spi_master_read(dev_id, spi_xfer->rx_buf + spi_xfer->rx_ofs,
					   &spi_xfer->rx_len);
		} else {
			return E_SYS;
		}
		break;

	case SPI_CMD_TRANSFER_INT:
		if (param != NULL && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			ctx->flags = SS_SPI_MASTER_FLAG_BUSY | SS_SPI_MASTER_FLAG_TX_RX;
			*spi_xfer = *((DEV_SPI_TRANSFER *)param);

			if (spi_xfer->rx_len == 0) {
				val32 = SPI_TRANSMIT_ONLY_MODE;
				ctx->flags |= SS_SPI_MASTER_FLAG_TX_READY;
				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);
			} else if (spi_xfer->tx_len == 0) {
				val32 = SPI_RECEIVE_ONLY_MODE;
				ctx->flags |= SS_SPI_MASTER_FLAG_RX_READY;
				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);
			} else if (spi_xfer->rx_ofs == spi_xfer->tx_len && spi_xfer->tx_ofs == 0) {
				ctx->flags |= (SS_SPI_MASTER_FLAG_RX_READY | SS_SPI_MASTER_FLAG_TX_READY);
				val32 = SPI_RECEIVE_AFTER_TRANSMIT_MODE;
				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);
				io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);
			} else {

				if (spi_xfer->tx_ofs != 0 || spi_xfer->rx_ofs != 0) {
					return E_NOSPT;
				}

				ctx->flags |= (SS_SPI_MASTER_FLAG_RX_READY | SS_SPI_MASTER_FLAG_TX_READY);
				DEV_SPI_XFER_INIT(spi_xfer);

				val32 = SPI_TRANSMIT_RECEIVE_MODE;

				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->tot_len);
				io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tot_len);
			}
		} else {
			return E_NOSPT;
		}
		break;

	case SPI_CMD_TRANSFER_POLLING:
		if (param != NULL && !(ctx->flags & SS_SPI_MASTER_FLAG_BUSY)) {
			spi_xfer = (DEV_SPI_TRANSFER *)param;
			while (spi_xfer != NULL) {
				ctx->flags = SS_SPI_MASTER_FLAG_BUSY;
				DEV_SPI_XFER_INIT(spi_xfer);
				io_spi_master_polling(dev_id, spi_xfer);
				ctx->flags &= ~SS_SPI_MASTER_FLAG_BUSY;
				spi_xfer = spi_xfer->next;
			}
		} else {
			return E_NOSPT;
		}
		break;

	case SPI_CMD_SET_TXCB:
		info->spi_cbs.tx_cb = param;
		break;

	case SPI_CMD_SET_RXCB:
		info->spi_cbs.rx_cb = param;
		break;

	case SPI_CMD_SET_XFERCB:
		info->spi_cbs.xfer_cb = param;
		break;

	case SPI_CMD_SET_ERRCB:
		info->spi_cbs.err_cb = param;
		break;

	case SPI_CMD_SET_CLK_MODE:
		switch (val32) {
		case SPI_CPOL_0_CPHA_0:
			val32 = 0;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
			break;
		case SPI_CPOL_0_CPHA_1:
			val32 = 0;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
			val32 = 1;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
			break;
		case SPI_CPOL_1_CPHA_0:
			val32 = 1;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
			val32 = 0;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
			break;
		case SPI_CPOL_1_CPHA_1:
			val32 = 1;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
			break;
		}
		break;

	case SPI_CMD_SET_DFS:
		io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_DFS, &val32);
		break;

	case SPI_CMD_MST_SET_FREQ:
		info->freq = val32;
		val32 = ctx->bus_freq / val32;
		io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_BAUD, &val32);
		break;

	case SPI_CMD_MST_SEL_DEV:
		info->slave = val32;
		val32 = (1 << val32);
		io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SE, &val32);
		break;

	case SPI_CMD_MST_DSEL_DEV:
		val32 = info->slave & (~(1 << val32));
		info->slave = SPI_SLAVE_NOT_SELECTED;
		io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SE, &val32);
		break;

	default:
		return E_NOSPT;
	}
	return E_OK;
}

int32_t ss_spi_master_write(SS_SPI_MASTER_DEV_CONTEXT *ctx, const void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint32_t len_ = len;
	uint32_t val32 = SPI_TRANSMIT_ONLY_MODE;

	/* spi busy */
	if (ctx->flags & SS_SPI_MASTER_FLAG_BUSY) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * Beacue the bottom drvier is simply interrupt driven
		 */
		return E_SYS;
	}

	ctx->flags = SS_SPI_MASTER_FLAG_BUSY;
	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
	io_spi_master_write(dev_id, (uint8_t *) data, &len_);

	/* wait finished: spi int enable & no cpu lock */
	while (ctx->flags & SS_SPI_MASTER_FLAG_BUSY) {
		;
	}

	if (ctx->flags & SS_SPI_MASTER_FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}

	ctx->flags = 0;
	return E_OK;
}

int32_t ss_spi_master_read(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint32_t len_ = len;
	uint32_t val32 = SPI_RECEIVE_ONLY_MODE;

	if (ctx->flags & SS_SPI_MASTER_FLAG_BUSY) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * Beacue the bottom drvier is simply interrupt driven
		 */
		return E_SYS;
	}

	ctx->flags = SS_SPI_MASTER_FLAG_BUSY;

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
	io_spi_master_read(dev_id, (uint8_t *)data, &len_);

	while (ctx->flags & SS_SPI_MASTER_FLAG_BUSY) {
		;
	}

	if (ctx->flags & SS_SPI_MASTER_FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}
	ctx->flags = 0;
	return 0;
}

void ss_spi_master_tx_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_SPI_INFO *info = ctx->info;
	DEV_SPI_TRANSFER *spi_xfer = &(info->xfer);

	if (ctx->flags & SS_SPI_MASTER_FLAG_TX_RX) {
		if (spi_xfer->rx_len == 0) {
			ctx->flags &= ~SS_SPI_MASTER_FLAG_TX_RX;

			if (ctx->flags & SS_SPI_MASTER_FLAG_TX_READY && info->spi_cbs.xfer_cb) {
				info->spi_cbs.xfer_cb(info);
			}
		}
	} else if (ctx->flags & SS_SPI_MASTER_FLAG_TX_READY && info->spi_cbs.tx_cb) {
		info->spi_cbs.tx_cb(info);
	}

	ctx->flags &= ~(SS_SPI_MASTER_FLAG_TX_READY | SS_SPI_MASTER_FLAG_BUSY);

}

void ss_spi_master_rx_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_SPI_INFO *info = ctx->info;

	if (ctx->flags & SS_SPI_MASTER_FLAG_TX_RX) {
		if (ctx->flags & SS_SPI_MASTER_FLAG_RX_READY && info->spi_cbs.xfer_cb) {
			info->spi_cbs.xfer_cb(info);
		}
		ctx->flags &= ~(SS_SPI_MASTER_FLAG_RX_READY | SS_SPI_MASTER_FLAG_TX_READY |
				SS_SPI_MASTER_FLAG_BUSY | SS_SPI_MASTER_FLAG_TX_RX);

	} else {
		if (ctx->flags & SS_SPI_MASTER_FLAG_RX_READY && info->spi_cbs.rx_cb) {
			info->spi_cbs.rx_cb(info);
		}

		ctx->flags &= ~(SS_SPI_MASTER_FLAG_RX_READY | SS_SPI_MASTER_FLAG_BUSY);
	}
}

void ss_spi_master_err_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_SPI_INFO *info = ctx->info;

	ctx->flags = SS_SPI_MASTER_FLAG_ERROR;

	if (info->spi_cbs.err_cb) {
		info->spi_cbs.err_cb(info);
	}
}