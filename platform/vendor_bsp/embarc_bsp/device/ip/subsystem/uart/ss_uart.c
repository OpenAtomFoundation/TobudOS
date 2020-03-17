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

#include "embARC_error.h"
#include "embARC_toolchain.h"
#include "arc/arc_exception.h"

#include "device/subsystem/uart.h"
#include "device/subsystem/ss_uart.h"
#include "io_config.h"

/* APEX UART device registers  */
#define UART_RBR    (0x00)
#define UART_THR    (0x00)
#define UART_DLL    (0x00)
#define UART_IER    (0x04)
#define UART_DLH    (0x04)
#define UART_IIR    (0x08)
#define UART_FCR    (0x08)
#define UART_LCR    (0x0c)
#define UART_MCR    (0x10)
#define UART_LSR    (0x14)      // unused
#define UART_MSR    (0x18)      // unused
#define UART_USR    (0x7c)      // unused
#define UART_CLKEN  (0xc0)

#define REG_READ(x)         arc_aux_read((ctx->reg_base + x))
#define REG_WRITE(x, y)     arc_aux_write((ctx->reg_base + x), y)

/** convert uart baudrate to subsystem divisor */
#define SS_UART_BAUD2DIV(perifreq, baud)        ((perifreq) / ((baud) * 16) + 1)

/**
 * @fn int32_t set_hwfc(uint32_t dev_id, uint32_t control)
 */
Inline int32_t set_hwfc(uint32_t dev_id, uint32_t control)
{
	uint32_t param = 0;

	if (control == UART_FC_NONE) {
		param = 0;
	} else {
		param = 1;
	}

	io_uart_ioctl(dev_id, IO_UART_SET_FLOW_CONTROL, &param);

	return E_OK;
}

/**
 * @fn int32_t set_baud(SS_UART_DEV_CONTEXT *ctx, uint32_t baud)
 */
Inline int32_t set_baud(SS_UART_DEV_CONTEXT *ctx, uint32_t baud)
{
	uint32_t param;
	DEV_UART_INFO *info = ctx->info;

	param = SS_UART_BAUD2DIV(ctx->bus_freq, baud);
	io_uart_ioctl(ctx->dev_id, IO_UART_SET_DLx, &param);
	info->baudrate = baud;

	return E_OK;
}

static int32_t set_dps(uint32_t dev_id, const UART_DPS_FORMAT *format)
{
	uint32_t param = 0;

	switch (format->databits) {
	case 6:
		param |= 0x1;
		break;
	case 7:
		param |= 0x2;
		break;
	case 8:
		param |= 0x3;
		break;
	default:
		return E_NOSPT;
	}

	if (format->stopbits == UART_STPBITS_ONE) {

	} else if (format->stopbits == UART_STPBITS_ONEHALF && format->databits == 5) {
		param |= 0x4;
	} else {
		return E_NOSPT;
	}

	if (format->parity != UART_PARITY_NONE) {
		param |= 0x8; // PEN

		if (format->parity == UART_PARITY_EVEN) {
			param |= 0x10; // EPS
		} else if (format->parity == UART_PARITY_MARK) {
			// PEN & Stick parity = logic 1
			param |= (0x20);
		} else if (format->parity == UART_PARITY_SPACE) {
			// PEN & EPS & Stick parity = logic 0
			param |= (0x10 | 0x20);
		}
	}

	io_uart_ioctl(dev_id, IO_UART_SET_LCR, &param);

	return E_OK;
}

static void io_uart_poll_write(SS_UART_DEV_CONTEXT *ctx, uint8_t *data, uint32_t *size)
{
	uint32_t old_val;
	uint32_t i = 0;
	uint32_t len;

	/* disbale uart interrupt */
	old_val = REG_READ(UART_IER);
	REG_WRITE(UART_IER, 0x80);

	len = *size;
	while (i < len) {
		while ((REG_READ(UART_LSR) & 0x20)) {
			;                                // wait THR empty
		}
		REG_WRITE(UART_THR, data[i++]);
	}

	REG_WRITE(UART_IER, old_val);
}

static void io_uart_poll_read(SS_UART_DEV_CONTEXT *ctx, uint8_t *data, uint32_t *size)
{
	uint32_t i = 0;
	uint32_t len;
	uint32_t old_val;

	old_val = REG_READ(UART_IER);
	REG_WRITE(UART_IER, 0x80);

	len = *size;
	while (i < len) {
		while (!(REG_READ(UART_LSR) & 0x1)) {
			;                                // wait data ready
		}
		data[i++] = REG_READ(UART_RBR);
	}

	REG_WRITE(UART_IER, old_val);
}

static void io_uart_rx_int(SS_UART_DEV_CONTEXT *ctx, uint32_t enable)
{
	uint32_t val;

	if (enable) {
		/* enable ERBFI interrupt */
		val = REG_READ(UART_IER) | 0x1;
		REG_WRITE(UART_IER, val);
	} else {
		val = REG_READ(UART_IER) & ~0x1;
		REG_WRITE(UART_IER, val);
	}
}

static void io_uart_tx_int(SS_UART_DEV_CONTEXT *ctx, uint32_t enable)
{
	uint32_t val;

	if (enable) {
		/* enable ETBEI interrupt and enable use of interrupt for TX threshold */
		val = REG_READ(UART_IER) | 0x82;
		REG_WRITE(UART_IER, val);
	} else {
		val = REG_READ(UART_IER) & ~0x82;
		REG_WRITE(UART_IER, val);
	}
}

int32_t ss_uart_open(SS_UART_DEV_CONTEXT *ctx, uint32_t baud)
{
	int32_t ret = E_OK;
	DEV_UART_INFO *info = ctx->info;
	io_cb_t callback;
	int32_t dev_id = ctx->dev_id;

	info->opn_cnt++;

	if (info->opn_cnt > 1) {
		if (baud == info->baudrate) {
			return E_OK;
		} else {
			return E_OPNED;
		}
	}

	if (io_uart_open(dev_id)) {
		return E_SYS;
	}

	callback.cb = (IO_CB_FUNC)ctx->tx_cb;
	io_uart_ioctl(dev_id, IO_SET_CB_TX, &callback);

	callback.cb = (IO_CB_FUNC)ctx->rx_cb;
	io_uart_ioctl(dev_id, IO_SET_CB_RX, &callback);

	callback.cb = (IO_CB_FUNC)ctx->err_cb;
	io_uart_ioctl(dev_id, IO_SET_CB_ERR, &callback);

	UART_DPS_FORMAT_DEFAULT(info->dps_format);
	set_dps(dev_id, &info->dps_format);

	info->hwfc = UART_FC_DEFAULT;
	set_hwfc(dev_id, info->hwfc);

	set_baud(ctx, baud);

	ctx->flags = 0;

	info->baudrate = baud;

	int_enable(ctx->intno);

	return ret;
}

int32_t ss_uart_close(SS_UART_DEV_CONTEXT *ctx)
{
	DEV_UART_INFO *info = ctx->info;

	info->opn_cnt--;
	if (info->opn_cnt == 0) {
		info->baudrate = 0;
		io_uart_close(ctx->dev_id);
		int_disable(ctx->intno);
	}

	return E_OK;
}

int32_t ss_uart_control(SS_UART_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	DEV_BUFFER *devbuf;
	uint32_t int_val = (uint32_t)param;
	int32_t dev_id = ctx->dev_id;
	DEV_UART_INFO *info = ctx->info;

	switch (ctrl_cmd) {
	case UART_CMD_SET_BAUD:
		return set_baud(ctx, int_val);
	case UART_CMD_SET_HWFC:
		io_uart_ioctl(dev_id, IO_UART_SET_FLOW_CONTROL, param);
		return E_OK;
	case UART_CMD_SET_DPS_FORMAT:
		return set_dps(dev_id, (UART_DPS_FORMAT *)param);
	case UART_CMD_GET_RXAVAIL:
		if (REG_READ(UART_LSR) & 0x1) {
			/* at least one byte */
			*((uint32_t *)param) = 1;
		} else {
			*((uint32_t *)param) = 0;
		}
		return E_OK;
	case UART_CMD_GET_TXAVAIL:
		int_val = REG_READ(UART_LSR);
		if (int_val & 0x40) {
			*((uint32_t *)param) = IO_UART0_FS;
		} else {
			if (int_val & 0x20) {
				/* FIFO full */
				*((uint32_t *)param) = 0;
			} else {
				*((uint32_t *)param) = 1;
			}
		}
		return E_OK;
	case UART_CMD_SET_RXCB:
		info->uart_cbs.rx_cb = param;
		return E_OK;
	case UART_CMD_SET_TXCB:
		info->uart_cbs.tx_cb = param;
		return E_OK;
	case UART_CMD_SET_ERRCB:
		info->uart_cbs.err_cb = param;
		return E_OK;
	case UART_CMD_SET_TXINT:
		io_uart_tx_int(ctx, int_val);
		return E_OK;
	case UART_CMD_SET_RXINT:
		io_uart_rx_int(ctx, int_val);
		return E_OK;
	case UART_CMD_SET_TXINT_BUF:
		if (param != NULL) {
			devbuf = (DEV_BUFFER *)param;
			info->tx_buf = *devbuf;
			info->tx_buf.ofs = 0;
			io_uart_write(dev_id, (uint8_t *)(devbuf->buf),
				      &(devbuf->len));
		} else {
			info->tx_buf.buf = NULL;
			info->tx_buf.len = 0;
			info->tx_buf.ofs = 0;
			io_uart_write(dev_id, NULL, &(info->tx_buf.len));
		}
		break;
	case UART_CMD_SET_RXINT_BUF:
		if (param != NULL) {
			devbuf = (DEV_BUFFER *)param;
			info->rx_buf = *devbuf;
			info->rx_buf.ofs = 0;
			io_uart_read(dev_id, (uint8_t *)(devbuf->buf),
				     &(devbuf->len));
		} else {
			info->rx_buf.buf = NULL;
			info->rx_buf.len = 0;
			info->rx_buf.ofs = 0;
			io_uart_read(dev_id, NULL, &(info->rx_buf.len));
		}
		break;
	case UART_CMD_BREAK_SET:
	case UART_CMD_BREAK_CLR:
		return E_NOSPT;
	default:
		return E_NOSPT;
	}
	return E_OK;
}

int32_t ss_uart_write(SS_UART_DEV_CONTEXT *ctx, const void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;

	if (arc_locked() || arc_int_active()) {
		io_uart_poll_write(ctx, (uint8_t *) data, &len);
	} else {
		ctx->flags = SS_UART_FLAG_TX;
		io_uart_write(dev_id, (uint8_t *) data, &len);

		/* wait finished: uart int enable & no cpu lock */
		while (ctx->flags & SS_UART_FLAG_TX) {
			;
		}

		if (ctx->flags & SS_UART_FLAG_ERROR) {
			ctx->flags = 0;
			return E_SYS;
		}

		ctx->flags = 0;
	}

	return len;
}

int32_t ss_uart_read(SS_UART_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	int32_t dev_id = ctx->dev_id;

	if (arc_locked() || arc_int_active()) {
		io_uart_poll_read(ctx, (uint8_t *) data, &len);
	} else {
		ctx->flags = SS_UART_FLAG_RX;

		io_uart_read(dev_id, (uint8_t *) data, &len);

		/* wait finished: uart int enable & no cpu lock */
		while (ctx->flags & SS_UART_FLAG_RX) {
			;
		}

		if (ctx->flags & SS_UART_FLAG_ERROR) {
			ctx->flags = 0;
			return E_SYS;
		}

		ctx->flags = 0;
	}

	return len;
}

void ss_uart_tx_cb(SS_UART_DEV_CONTEXT *ctx, void *param)
{
	DEV_UART_INFO *info = ctx->info;

	if (ctx->flags & SS_UART_FLAG_TX) {
		ctx->flags &= ~SS_UART_FLAG_TX;
	} else if (info->uart_cbs.tx_cb) {
		info->uart_cbs.tx_cb(info);
	}
}

void ss_uart_rx_cb(SS_UART_DEV_CONTEXT *ctx, void *param)
{
	DEV_UART_INFO *info = ctx->info;

	if (ctx->flags & SS_UART_FLAG_RX) {
		ctx->flags &= ~SS_UART_FLAG_RX;
	} else if (info->uart_cbs.rx_cb) {
		info->uart_cbs.rx_cb(info);
	}
}

void ss_uart_err_cb(SS_UART_DEV_CONTEXT *ctx, void *param)
{
	DEV_UART_INFO *info = ctx->info;

	ctx->flags = SS_UART_FLAG_ERROR;

	if (info->uart_cbs.err_cb) {
		info->uart_cbs.err_cb(info);
	}
}