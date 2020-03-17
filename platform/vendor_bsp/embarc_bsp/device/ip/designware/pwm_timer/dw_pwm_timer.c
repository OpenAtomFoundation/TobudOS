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
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc/arc_exception.h"

#include "device/designware/dw_pwm_timer.h"

/** check expressions used in DesignWare PWM_TIMER driver implementation */
#define DW_PWM_TIMER_CHECK_EXP(EXPR, ERROR_CODE)        CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#ifndef DISABLE_DEVICE_OBJECT_VALID_CHECK
/** valid check of uart info object */
#define VALID_CHK_PWM_TIMER_INFO_OBJECT(pwm_timerinfo_obj_ptr)      {				  \
		DW_PWM_TIMER_CHECK_EXP((pwm_timerinfo_obj_ptr) != NULL, E_OBJ);			  \
		DW_PWM_TIMER_CHECK_EXP(((pwm_timerinfo_obj_ptr)->pwm_timer_ctrl) != NULL, E_OBJ); \
}
#endif

/**
 * @fn uint32_t dw_pwm_timer_int_read_status(DW_PWM_TIMER_CTRL_PTR port)
 */
Inline uint32_t dw_pwm_timer_int_read_status(DW_PWM_TIMER_CTRL_PTR port)
{
	return port->regs->PWM_TIMERS_INT_STATUS;
}

/**
 * @fn void dw_pwm_timer_timer_enable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
 */
Inline void dw_pwm_timer_timer_enable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
{
	port->regs->CTRL[ch].CONTROL_REG |= 0x03;
}

/**
 * @fn void dw_pwm_timer_timer_disable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
 */
Inline void dw_pwm_timer_timer_disable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
{
	port->regs->CTRL[ch].CONTROL_REG &= (~0x03);
}

/**
 * @fn void dw_pwm_timer_pwm_enable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
 */
Inline void dw_pwm_timer_pwm_enable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
{
	port->regs->CTRL[ch].CONTROL_REG |= 0x08;
}

/**
 * @fn void dw_pwm_timer_pwm_disable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
 */
Inline void dw_pwm_timer_pwm_disable(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
{
	port->regs->CTRL[ch].CONTROL_REG &= (~0x08);
}

/**
 * @fn void dw_pwm_timer_count_set(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch, uint16_t val)
 */
Inline void dw_pwm_timer_count_set(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch, uint16_t val)
{
	port->regs->CTRL[ch].LOAD_COUNT = val;
}

/**
 * @fn uint16_t dw_pwm_timer_count_get(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch)
 */
Inline uint16_t dw_pwm_timer_count_get(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch)
{
	return port->regs->CTRL[ch].LOAD_COUNT;
}

/**
 * @fn void dw_pwm_timer_count2_set(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch, uint16_t val)
 */
Inline void dw_pwm_timer_count2_set(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch, uint16_t val)
{
	port->regs->LOAD_COUNT2[ch] = val;
}

/**
 * @fn uint16_t dw_pwm_timer_count2_get(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch)
 */
Inline uint16_t dw_pwm_timer_count2_get(DW_PWM_TIMER_CTRL_PTR port, uint8_t ch)
{
	return port->regs->LOAD_COUNT2[ch];
}

static void dw_pwm_timer_int_clear_all(DW_PWM_TIMER_CTRL_PTR port)
{
	int32_t reg_val;

	reg_val = port->regs->PWM_TIMERS_EOI;
}

static void dw_pwm_timer_int_clear(DW_PWM_TIMER_CTRL_PTR port, uint32_t ch)
{
	int32_t reg_val;

	reg_val = port->regs->CTRL[ch].EOI;
}

/* interface for DEV_PWM_TIMER */
/** Open designware pwm_timer device */
int32_t dw_pwm_timer_open(DEV_PWM_TIMER *pwm_timer_obj)
{
	int32_t ercd = E_OK;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);

	port_info_ptr->opn_cnt++;
	if (port_info_ptr->opn_cnt > 1) { /* opened before */
		return E_OPNED;
	}

	if (port->intno != DW_PWM_TIMER_INVALID_INTNO) {
		for (uint8_t i = 0; i < port->ch_num; i++) {
			dw_pwm_timer_timer_disable(port, i);
			dw_pwm_timer_pwm_disable(port, i);
			int_disable(port->intno + i);
			int_handler_install(port->intno + i, port->int_handler);
		}
	}

error_exit:
	return ercd;
}

/** Close designware pwm_timer device */
int32_t dw_pwm_timer_close(DEV_PWM_TIMER *pwm_timer_obj)
{
	int32_t ercd = E_OK;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);

	DW_PWM_TIMER_CHECK_EXP(port_info_ptr->opn_cnt > 0, E_OK);

	port_info_ptr->opn_cnt--;
	if (port_info_ptr->opn_cnt == 0) {

		if (port->intno != DW_PWM_TIMER_INVALID_INTNO) {
			for (uint8_t i = 0; i < port->ch_num; i++) {
				dw_pwm_timer_timer_disable(port, i);
				dw_pwm_timer_pwm_disable(port, i);
				int_disable(port->intno + i);
			}
		}
	} else {
		ercd = E_OPNED;
	}

error_exit:
	return ercd;
}

/** Read designware pwm_timer device value */
int32_t dw_pwm_timer_read(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t *mode, uint32_t *freq, uint32_t *dc)
{
	int32_t ercd = E_OK;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);
	DW_PWM_TIMER_CHECK_EXP(port_info_ptr->opn_cnt > 0, E_CLSED);
	DW_PWM_TIMER_CHECK_EXP((ch >= 0) && (ch < port->ch_num), E_PAR);

	int32_t count_low = dw_pwm_timer_count_get(port, ch);
	int32_t count_high = dw_pwm_timer_count2_get(port, ch);

	*mode = port->mode[ch];
	if (*mode == DEV_PWM_TIMER_MODE_TIMER) {
		*dc = 100;
		if (count_low != 0) {
			*freq = port->clock / (count_low * 2);
		} else {
			*freq = 0;
		}
	} else if (*mode == DEV_PWM_TIMER_MODE_PWM) {
		if (count_low == 0 && count_high == 0) {
			*dc = 0;
			*freq = 0;
		} else {
			*dc = (count_high * 100) / (count_high + count_low);
			*freq = port->clock / (count_high + count_low);
		}
	} else {
		*dc = 0;
		*freq = 0;
	}

error_exit:
	return ercd;
}

/** Write designware pwm_timer device value */
int32_t dw_pwm_timer_write(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t mode, uint32_t freq, uint32_t dc)
{
	int32_t ercd = E_OK;
	int32_t count, count_high;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);
	DW_PWM_TIMER_CHECK_EXP(port_info_ptr->opn_cnt > 0, E_CLSED);
	DW_PWM_TIMER_CHECK_EXP((ch >= 0) && (ch < port->ch_num), E_PAR);
	DW_PWM_TIMER_CHECK_EXP((dc >= 0) && (dc <= 100), E_PAR);

	if (mode == DEV_PWM_TIMER_MODE_TIMER) {
		DW_PWM_TIMER_CHECK_EXP(freq > 0, E_PAR);
		port->mode[ch] = mode;

		count = port->clock / freq;
		dw_pwm_timer_count_set(port, ch, count / 2);
		dw_pwm_timer_count2_set(port, ch, 0);

		dw_pwm_timer_timer_enable(port, ch);
		dw_pwm_timer_pwm_disable(port, ch);
		int_enable(port->intno + ch);
	} else if (mode == DEV_PWM_TIMER_MODE_PWM) {
		DW_PWM_TIMER_CHECK_EXP(freq > 0, E_PAR);
		port->mode[ch] = mode;

		count = port->clock / freq;
		count_high = (count * dc) / 100;
		dw_pwm_timer_count_set(port, ch, count - count_high);
		dw_pwm_timer_count2_set(port, ch, count_high);
		dw_pwm_timer_timer_enable(port, ch);
		dw_pwm_timer_pwm_enable(port, ch);
		int_disable(port->intno + ch);
	} else if (mode == DEV_PWM_TIMER_MODE_CLOSE) {
		port->mode[ch] = mode;
		dw_pwm_timer_count_set(port, ch, 0);
		dw_pwm_timer_count2_set(port, ch, 0);
		dw_pwm_timer_timer_disable(port, ch);
		dw_pwm_timer_pwm_disable(port, ch);
		int_disable(port->intno + ch);
	}
error_exit:
	return ercd;
}

/** Control designware pwm_timer device */
int32_t dw_pwm_timer_control(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t ctrl_cmd, void *par)
{
	int32_t ercd = E_OK;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);
	DW_PWM_TIMER_CHECK_EXP(port_info_ptr->opn_cnt > 0, E_CLSED);
	DW_PWM_TIMER_CHECK_EXP((ch >= 0) && (ch < port->ch_num), E_PAR);

	DEV_PWM_TIMER_CFG *cfg_ptr;
	switch (ctrl_cmd) {
	case PWM_TIMER_CMD_SET_CFG:
		cfg_ptr = (DEV_PWM_TIMER_CFG *) par;

		dw_pwm_timer_count_set(port, ch, cfg_ptr->count_low);
		dw_pwm_timer_count2_set(port, ch, cfg_ptr->count_high);
		if (cfg_ptr->isr_hander != NULL) {
			port->ch_isr->int_ch_handler_ptr[ch] = cfg_ptr->isr_hander;
		}
		if (cfg_ptr->mode == DEV_PWM_TIMER_MODE_TIMER) {
			port->mode[ch] = cfg_ptr->mode;
			dw_pwm_timer_timer_enable(port, ch);
			int_enable(port->intno + ch);
		} else if (cfg_ptr->mode == DEV_PWM_TIMER_MODE_PWM) {
			port->mode[ch] = cfg_ptr->mode;
			dw_pwm_timer_timer_enable(port, ch);
			dw_pwm_timer_pwm_enable(port, ch);
		} else if (cfg_ptr->mode == DEV_PWM_TIMER_MODE_CLOSE) {
			port->mode[ch] = cfg_ptr->mode;
			dw_pwm_timer_timer_disable(port, ch);
			dw_pwm_timer_pwm_disable(port, ch);
		}
		break;
	case PWM_TIMER_CMD_GET_CFG:
		cfg_ptr = (DEV_PWM_TIMER_CFG *) par;
		cfg_ptr->count_low = dw_pwm_timer_count_get(port, ch);
		cfg_ptr->count_high = dw_pwm_timer_count2_get(port, ch);
		if (cfg_ptr->isr_hander != NULL) {
			cfg_ptr->isr_hander = port->ch_isr->int_ch_handler_ptr[ch];
		}
		cfg_ptr->mode = port->mode[ch];
		break;
	case PWM_TIMER_CMD_DIS_CH:
		dw_pwm_timer_timer_disable(port, ch);
		break;
	case PWM_TIMER_CMD_ENA_CH:
		dw_pwm_timer_timer_enable(port, ch);
		break;
	case PWM_TIMER_CMD_DIS_ISR:
		int_disable(port->intno + ch);
		break;
	case PWM_TIMER_CMD_ENA_ISR:
		int_enable(port->intno + ch);
		break;
	case PWM_TIMER_CMD_SET_ISR:
		DW_PWM_TIMER_CHECK_EXP((par != NULL) && CHECK_ALIGN_4BYTES(par), E_PAR);
		port->ch_isr->int_ch_handler_ptr[ch] = (DEV_PWM_TIMER_HANDLER)par;
		break;
	case PWM_TIMER_CMD_GET_ISR:
		DW_PWM_TIMER_CHECK_EXP((par != NULL) && CHECK_ALIGN_4BYTES(par), E_PAR);
		par = (void *)(port->ch_isr->int_ch_handler_ptr[ch]);
		break;
	default:
		ercd = E_NOSPT;
		break;
	}

error_exit:
	return ercd;
}

/** designware pwm_timer interrupt process */
int32_t dw_pwm_timer_isr_handler(DEV_PWM_TIMER *pwm_timer_obj, void *ptr)
{
	int32_t ercd = E_OK;
	DEV_PWM_TIMER_INFO_PTR port_info_ptr = &(pwm_timer_obj->pwm_timer_info);

	/* START ERROR CHECK */
	VALID_CHK_PWM_TIMER_INFO_OBJECT(port_info_ptr);
	/* END OF ERROR CHECK */

	DW_PWM_TIMER_CTRL_PTR port = (DW_PWM_TIMER_CTRL_PTR)(port_info_ptr->pwm_timer_ctrl);

	uint32_t i, pwm_timer_bit_isr_state;
	uint32_t max_int_bit_count = 0;

	/** read interrupt status */
	pwm_timer_bit_isr_state = dw_pwm_timer_int_read_status(port);

	if (port->ch_isr) {
		max_int_bit_count = (port->ch_isr->int_ch_max_cnt);
	} else {
		dw_pwm_timer_int_clear_all(port);
	}

	for (i = 0; i < max_int_bit_count; i++) {
		if (pwm_timer_bit_isr_state & (1 << i)) {
			/* this bit interrupt enabled */
			if (port->ch_isr->int_ch_handler_ptr[i]) {
				port->ch_isr->int_ch_handler_ptr[i](pwm_timer_obj);
			}
			dw_pwm_timer_int_clear(port, i); /** clear this bit interrupt */
		}
	}

error_exit:
	return ercd;
}
