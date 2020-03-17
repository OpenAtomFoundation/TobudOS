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

/* the wrapper of subsystem gpio driver */

#include "embARC_error.h"
#include "embARC_toolchain.h"

#include "device/subsystem/gpio.h"
#include "device/subsystem/ss_gpio.h"

int32_t ss_gpio_open(SS_GPIO_DEV_CONTEXT *ctx, uint32_t dir)
{
	DEV_GPIO_INFO *info = ctx->info;
	int32_t ret = E_OK;
	io_cb_t cb;

	info->opn_cnt++;

	if (info->opn_cnt > 1) {                /* opened before */
		if (dir == info->direction) {   /* direction is the same */
			return E_OK;
		} else {                        /* open with different direction */
			return E_OPNED;
		}
	}

	if (io_gpio_open(ctx->dev_id)) {
		return E_SYS;
	}

	info->direction = dir;
	info->method = 0;
	ctx->int_bit_type = 0;
	ctx->int_bit_polarity = 0;
	ctx->int_bit_debounce = 0;
	cb.cb = (IO_CB_FUNC)ctx->int_cb;
	io_gpio_ioctl(ctx->dev_id, IO_SET_CB_RX, &cb);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_ENABLE, &info->method);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DIRECTION, &info->direction);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_POLARITY, &ctx->int_bit_polarity);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DEBOUNCE, &ctx->int_bit_debounce);

	int_enable(ctx->intno);

	return ret;
}

int32_t ss_gpio_close(SS_GPIO_DEV_CONTEXT *ctx)
{
	DEV_GPIO_INFO *info = ctx->info;

	info->opn_cnt--;

	if (info->opn_cnt == 0) {
		io_gpio_close(ctx->dev_id);
		int_disable(ctx->intno);
	} else {
		return E_OPNED;
	}

	return E_OK;
}

int32_t ss_gpio_control(SS_GPIO_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	int32_t ret = E_OK;
	DEV_GPIO_INFO *info = ctx->info;
	DEV_GPIO_INT_CFG *cfg = (DEV_GPIO_INT_CFG *)param;
	DEV_GPIO_BIT_ISR *isr = (DEV_GPIO_BIT_ISR *)param;
	uint32_t mask;

	switch (ctrl_cmd) {
	case GPIO_CMD_SET_BIT_DIR_INPUT:
	case GPIO_CMD_SET_BIT_DIR_OUTPUT:
		if (ctrl_cmd == GPIO_CMD_SET_BIT_DIR_OUTPUT) {
			info->direction |= ((uint32_t)param);
		} else {
			info->direction &= ~((uint32_t)param);
		}
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DIRECTION, &(info->direction));
		break;
	case GPIO_CMD_GET_BIT_DIR:
		*((uint32_t *) param) = info->direction;
		break;

	case GPIO_CMD_ENA_BIT_INT:
	case GPIO_CMD_DIS_BIT_INT:
		if (ctrl_cmd == GPIO_CMD_ENA_BIT_INT) {
			info->method |= ((uint32_t)param);
		} else {
			info->method &= ~((uint32_t)param);
		}
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_ENABLE, &(info->method));
		if (info->method) {
			int_enable(ctx->intno);
		} else {
			int_disable(ctx->intno);
		}
		break;

	case GPIO_CMD_GET_BIT_MTHD:
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_GET_INT_ENABLE, &(info->method));
		*((uint32_t *) param) = info->method;
		break;
	case GPIO_CMD_SET_BIT_INT_CFG:
		mask = cfg->int_bit_mask;
		ctx->int_bit_type &= ~(mask);
		ctx->int_bit_type |= cfg->int_bit_type;
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_TYPE, &ctx->int_bit_type);

		ctx->int_bit_polarity &= ~(mask);
		ctx->int_bit_polarity |= cfg->int_bit_polarity;
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_POLARITY, &ctx->int_bit_polarity);

		cfg->int_bit_debounce &= mask;
		ctx->int_bit_debounce &= ~(mask);
		ctx->int_bit_debounce |= cfg->int_bit_debounce;
		io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DEBOUNCE, &ctx->int_bit_debounce);
		break;

	case GPIO_CMD_GET_BIT_INT_CFG:
		cfg->int_bit_type = ctx->int_bit_type & cfg->int_bit_mask;
		cfg->int_bit_polarity = ctx->int_bit_polarity & cfg->int_bit_mask;
		cfg->int_bit_debounce = ctx->int_bit_debounce & cfg->int_bit_mask;
		break;

	case GPIO_CMD_SET_BIT_ISR:
		if (isr->int_bit_ofs < ctx->width) {
			ctx->handlers[isr->int_bit_ofs] = isr->int_bit_handler;
		} else {
			ret = E_PAR;
		}
		break;

	case GPIO_CMD_GET_BIT_ISR:
		if (isr->int_bit_ofs < ctx->width) {
			isr->int_bit_handler = ctx->handlers[isr->int_bit_ofs];
		} else {
			ret = E_PAR;
		}
		break;
	default:
		ret = E_NOSPT;
		break;
	}

	return ret;
}

int32_t ss_gpio_write(SS_GPIO_DEV_CONTEXT *ctx, uint32_t val, uint32_t mask)
{
	uint32_t temp_val;

	io_gpio_read(ctx->dev_id, &temp_val);
	temp_val &= (~mask);
	val &= mask;
	io_gpio_write(ctx->dev_id, temp_val | val);

	return 0;
}

int32_t ss_gpio_read(SS_GPIO_DEV_CONTEXT *ctx, uint32_t *val, uint32_t mask)
{
	uint32_t raw;

	io_gpio_read(ctx->dev_id, &raw);
	*val = raw & mask;

	return 0;
}

void ss_gpio_int_cb(SS_GPIO_DEV_CONTEXT *ctx, void *param)
{
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((uint32_t)param & (1 << i) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}
