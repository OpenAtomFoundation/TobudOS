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

#include "device/subsystem/i2c_master.h"
#include "device/subsystem/ss_i2c_master.h"
#include "device/subsystem/i2c_priv.h"

#define REG_READ(x)         arc_aux_read((ctx->reg_base + x))
#define REG_WRITE(x, y)     arc_aux_write((ctx->reg_base + x), y)

/**
 * @fn int32_t ss_iic_master_putready(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief Test whether iic is ready to write, 1 ready, 0 not ready
 */
Inline int32_t ss_iic_master_putready(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	uint32_t status = REG_READ(I2C_STATUS);

	return ((status & IC_STATUS_TFNF) != 0);
}

/**
 * @fn int32_t ss_iic_master_getready(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief test whether iic is ready to receive, 1 ready, 0 not ready
 */
Inline int32_t ss_iic_master_getready(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	uint32_t status = REG_READ(I2C_STATUS);

	return ((status & IC_STATUS_RFNE) != 0);
}

/**
 * @fn void ss_iic_mask_interrupt(SS_IIC_MASTER_DEV_CONTEXT *ctx, uint32_t mask)
 * @brief Mask iic interrupt
 */
Inline void ss_iic_mask_interrupt(SS_IIC_MASTER_DEV_CONTEXT *ctx, uint32_t mask)
{
	uint32_t intr_mask = REG_READ(I2C_INTR_MASK);

	REG_WRITE(I2C_INTR_MASK, intr_mask & (~mask));
}

static void ss_iic_master_enable_device(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	DEV_IIC_INFO *info = ctx->info;
	uint32_t enable = REG_READ(I2C_ENABLE);

	if (((enable & 0x1) & DEV_ENABLED) == 0) {
		REG_WRITE(I2C_ENABLE, (enable | 0x1));
		info->status |= DEV_ENABLED;
	}
}

static void ss_iic_master_disable_device(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	uint32_t enable = REG_READ(I2C_ENABLE);

	REG_WRITE(I2C_ENABLE, (enable & (~(0x1))));

	while ((0x1 & REG_READ(I2C_ENABLE_STATUS)) != 0) {
		;
	}
}

/* reset IIC master */
static void ss_iic_master_reset_device(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{

	DEV_IIC_INFO *info = ctx->info;

	io_i2c_master_close(ctx->dev_id);
	info->next_cond = IIC_MODE_STOP;
	info->cur_state = IIC_FREE;
	info->err_state = IIC_ERR_NONE;
	ctx->flags = 0;

	io_i2c_master_open(ctx->dev_id);
}

/** disable iic master interrupt for transmit or receive */
static void ss_iic_master_dis_cbr(SS_IIC_MASTER_DEV_CONTEXT *ctx, uint32_t cbrtn)
{
	switch (cbrtn) {
	case SS_IIC_MASTER_RDY_SND:
		ss_iic_mask_interrupt(ctx, R_TX_EMPTY);
		break;
	case SS_IIC_MASTER_RDY_RCV:
		ss_iic_mask_interrupt(ctx, R_TX_EMPTY | R_RX_FULL);
		break;
	default:
		break;
	}
}

/**
 * @fn void ss_iic_master_flush_tx(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief flush TX FIFO
 */
Inline void ss_iic_master_flush_tx(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{

}

/**
 * @fn int32_t ss_iic_master_flush_rx(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief Flush RX FIFO
 */
Inline void ss_iic_master_flush_rx(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{

}

/**
 * @fn uint32_t ss_iic_master_get_txavail(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief Get available transmit FIFO count
 */
Inline uint32_t ss_iic_master_get_txavail(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	uint32_t flr = REG_READ(I2C_TXFLR);

	return (int32_t)(IC_TX_RX_FIFO_SIZE - flr);
}

/**
 * @fn ss_iic_master_get_rxavail(SS_IIC_MASTER_DEV_CONTEXT *ctx)
 * @brief Get available receive FIFO count
 */
Inline uint32_t ss_iic_master_get_rxavail(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	uint32_t flr = REG_READ(I2C_RXFLR);

	return (int32_t)flr;
}

int32_t ss_iic_master_close(SS_IIC_MASTER_DEV_CONTEXT *ctx)
{
	int32_t ret = E_OK;

	DEV_IIC_INFO *info = ctx->info;

	if (info->opn_cnt > 0) {
		info->opn_cnt = 0;
		io_i2c_master_close(ctx->dev_id);
		int_disable(ctx->int_err);
		int_disable(ctx->int_rx_avail);
		int_disable(ctx->int_tx_req);
		int_disable(ctx->int_stop_det);
	} else {
		ret = E_CLSED;
	}

	return ret;
}

int32_t ss_iic_master_control(SS_IIC_MASTER_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	uint32_t val32;
	uint32_t arg;
	uint32_t dev_id = ctx->dev_id;
	DEV_IIC_INFO *iic_info_ptr = ctx->info;
	io_cb_t callback;

	switch (ctrl_cmd) {
	case IIC_CMD_GET_STATUS:
		SS_IIC_MASTER_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		*((int32_t *)param) = iic_info_ptr->status;
		break;
	case IIC_CMD_ENA_DEV:
		ss_iic_master_enable_device(ctx);
		break;
	case IIC_CMD_DIS_DEV:
		ss_iic_master_disable_device(ctx);
		break;
	case IIC_CMD_RESET:
		ss_iic_master_reset_device(ctx);
		break;
	case IIC_CMD_FLUSH_TX:
		ss_iic_master_flush_tx(ctx);
		break;
	case IIC_CMD_FLUSH_RX:
		ss_iic_master_flush_rx(ctx);
		break;
	case IIC_CMD_SET_ADDR_MODE:
		val32 = (uint32_t)param;
		SS_IIC_MASTER_CHECK_EXP((val32 == IIC_7BIT_ADDRESS) || (val32 == IIC_10BIT_ADDRESS), E_PAR);
		if (val32 == IIC_10BIT_ADDRESS) {
			arg = 1;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_10BIT_ADDR, &arg);
			iic_info_ptr->addr_mode = IIC_10BIT_ADDRESS;
		} else {
			arg = 0;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_10BIT_ADDR, &arg);
			iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
		}
		iic_info_ptr->addr_mode = val32;
		break;
	case IIC_CMD_GET_RXAVAIL:
		SS_IIC_MASTER_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		*((int32_t *)param) = ss_iic_master_get_rxavail(ctx);
		break;
	case IIC_CMD_GET_TXAVAIL:
		SS_IIC_MASTER_CHECK_EXP((param != NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
		*((int32_t *)param) = ss_iic_master_get_txavail(ctx);
		break;
	case IIC_CMD_SET_TXCB:
		SS_IIC_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		callback.cb = param;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
		break;
	case IIC_CMD_SET_RXCB:
		SS_IIC_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		callback.cb = param;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
		break;
	case IIC_CMD_SET_ERRCB:
		SS_IIC_MASTER_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		callback.cb = param;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);
		break;
	case IIC_CMD_ABORT_TX:
		ercd = E_NOSPT;
		break;
	case IIC_CMD_ABORT_RX:
		ercd = E_NOSPT;
		break;
	case IIC_CMD_SET_TXINT:
		val32 = (uint32_t)param;
		if (val32 == 0) {
			ss_iic_master_dis_cbr(ctx, SS_IIC_MASTER_RDY_SND);
		} else {
			ss_iic_master_dis_cbr(ctx, SS_IIC_MASTER_RDY_SND);
		}
		break;
	case IIC_CMD_SET_RXINT:
		val32 = (uint32_t)param;
		if (val32 == 0) {
			ss_iic_master_dis_cbr(ctx, SS_IIC_MASTER_RDY_RCV);
		} else {
			ss_iic_master_dis_cbr(ctx, SS_IIC_MASTER_RDY_RCV);
		}
		break;
	case IIC_CMD_SET_TXINT_BUF:
		ercd = E_NOSPT;
		break;
	case IIC_CMD_SET_RXINT_BUF:
		ercd = E_NOSPT;
		break;
	case IIC_CMD_MST_SET_SPEED_MODE:
		val32 = (uint32_t)param;
		SS_IIC_MASTER_CHECK_EXP((val32 >= IIC_SPEED_STANDARD) && (val32 <= IIC_SPEED_FAST), E_PAR);
		if (val32 == IIC_SPEED_STANDARD) {
			arg = 1;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			iic_info_ptr->speed_mode = IIC_SPEED_STANDARD;
		} else {
			arg = 2;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			iic_info_ptr->speed_mode = IIC_SPEED_FAST;
		}
		break;
	case IIC_CMD_MST_SET_TAR_ADDR:
		if (iic_info_ptr->addr_mode == IIC_7BIT_ADDRESS) {
			val32 = ((uint32_t)param) & IIC_7BIT_ADDRESS_MASK;
		} else {
			val32 = ((uint32_t)param) & IIC_10BIT_ADDRESS_MASK;
		}
		if (val32 != iic_info_ptr->tar_addr) {
			arg = val32;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_TARGET_ADDR, &arg);
			iic_info_ptr->tar_addr = val32;
		}
		break;
	case IIC_CMD_MST_SET_NEXT_COND:
		val32 = (uint32_t) param;
		if (val32 == IIC_MODE_STOP) {
			arg = I2C_STOP_CMD;
		} else if (val32 == IIC_MODE_RESTART) {
			arg = I2C_RESTART_CMD;
		} else {
			arg = 0;
		}
		io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_NEXT_COND, &arg);
		break;
	default:
		ercd = E_NOSPT;
		break;
	}

error_exit:
	return ercd;
}

/* param: speed mode, 1-standard mode, 2-fast mode, 2 as default in hardware */
int32_t ss_iic_master_open(SS_IIC_MASTER_DEV_CONTEXT *ctx, uint32_t param)
{
	int32_t ercd = E_OK;
	uint32_t dev_id = ctx->dev_id;
	DEV_IIC_INFO *info = ctx->info;
	io_cb_t callback;
	int32_t int_e = 0;
	uint32_t arg;

	SS_IIC_MASTER_CHECK_EXP((param >= IIC_SPEED_STANDARD) && (param <= IIC_SPEED_FAST), E_PAR);

	if (info->opn_cnt == 0) {
		SS_IIC_MASTER_CHECK_EXP(io_i2c_master_open(dev_id) == 0, E_SYS);

		if (param == IIC_SPEED_STANDARD) {
			arg = 1;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			info->speed_mode = IIC_SPEED_STANDARD;
		} else {
			arg = 2;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			info->speed_mode = IIC_SPEED_FAST;
		}

		callback.cb = (IO_CB_FUNC)ctx->int_rx_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
		callback.cb = (IO_CB_FUNC)ctx->int_tx_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
		callback.cb = (IO_CB_FUNC)ctx->int_err_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);

		info->opn_cnt++;
		info->addr_mode = IIC_7BIT_ADDRESS;
		info->mode = DEV_MASTER_MODE;
		info->tar_addr &= IIC_7BIT_ADDRESS_MASK;

		ctx->flags = 0;

		/* enable interrupt */
		int_e = int_e | int_enable(ctx->int_err);
		int_e = int_e | int_enable(ctx->int_rx_avail);
		int_e = int_e | int_enable(ctx->int_tx_req);
		int_e = int_e | int_enable(ctx->int_stop_det);

		if (int_e != 0) {
			ercd = E_PAR;
		}
	} else {
		ercd = E_OPNED;
	}

error_exit:
	return ercd;
}

int32_t ss_iic_master_write(SS_IIC_MASTER_DEV_CONTEXT *ctx, const void *data, uint32_t len)
{
	uint32_t xlen;
	uint32_t dev_id = ctx->dev_id;

	if (ctx->flags) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * because the bottom driver is simply interrupt driven
		 */
		return E_SYS;
	}

	xlen = len;

	ctx->flags = SS_IIC_MASTER_FLAG_TX;

	io_i2c_master_write(dev_id, (uint8_t *)data, &xlen);

	/* wait finished: i2c master int enable & no cpu lock */
	while (ctx->flags & SS_IIC_MASTER_FLAG_TX) {
		;
	}

	if (ctx->flags & SS_IIC_MASTER_FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}

	ctx->flags = 0;

	return len;
}

int32_t ss_iic_master_read(SS_IIC_MASTER_DEV_CONTEXT *ctx, const void *data, uint32_t len)
{
	uint32_t xlen;
	uint32_t dev_id = ctx->dev_id;

	if (ctx->flags) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * because the bottom driver is simply interrupt driven
		 */
		return E_SYS;
	}

	xlen = len;

	ctx->flags = SS_IIC_MASTER_FLAG_RX;

	io_i2c_master_read(dev_id, (uint8_t *)data, &xlen);

	/* wait finished: i2c master int enable & no cpu lock */
	while (ctx->flags & SS_IIC_MASTER_FLAG_RX) {
		;
	}

	if (ctx->flags & SS_IIC_MASTER_FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}

	ctx->flags = 0;

	return len;
}

void ss_iic_master_tx_cb(SS_IIC_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_IIC_INFO *info = ctx->info;

	if (ctx->flags & SS_IIC_MASTER_FLAG_TX) {
		ctx->flags &= ~SS_IIC_MASTER_FLAG_TX;
		if (info->iic_cbs.tx_cb) {
			info->iic_cbs.tx_cb(info);
		}
	}
}

void ss_iic_master_rx_cb(SS_IIC_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_IIC_INFO *info = ctx->info;

	if (ctx->flags & SS_IIC_MASTER_FLAG_RX) {
		ctx->flags &= ~SS_IIC_MASTER_FLAG_RX;
		if (info->iic_cbs.rx_cb) {
			info->iic_cbs.rx_cb(info);
		}
	}
}

void ss_iic_master_err_cb(SS_IIC_MASTER_DEV_CONTEXT *ctx, void *param)
{
	DEV_IIC_INFO *info = ctx->info;

	ctx->flags |= SS_IIC_MASTER_FLAG_ERROR;

	if (info->iic_cbs.err_cb) {
		info->iic_cbs.err_cb(info);
	}
}