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
#include "string.h"

#include "device/designware/dw_nist_trng.h"

#ifdef TRNG_VERSION_NIST

#define DBG_LESS
#include "embARC_debug.h"

#define DW_TRNG_CHECK_EXP(EXPR, ERROR_CODE)     CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#ifndef DISABLE_DEVICE_OBJECT_VALID_CHECK
/** valid check of trng info object */
#define VALID_CHK_TRNG_INFO_OBJECT(trnginfo_obj_ptr)        {			   \
		DW_TRNG_CHECK_EXP((trnginfo_obj_ptr) != NULL, E_OBJ);		   \
		DW_TRNG_CHECK_EXP(((trnginfo_obj_ptr)->trng_ctrl) != NULL, E_OBJ); \
}
#endif

#define DW_TRNG_CMD_NOP             (0x0)
#define DW_TRNG_CMD_GEN_NOISE       (0x1)
#define DW_TRNG_CMD_GEN_NONCE       (0x2)
#define DW_TRNG_CMD_CREATE_STATE    (0x3)
#define DW_TRNG_CMD_RENEW_STATE     (0x4)
#define DW_TRNG_CMD_REFRESH_ADDIN   (0x5)
#define DW_TRNG_CMD_GEN_RANDOM      (0x6)
#define DW_TRNG_CMD_ADVANCE_STATE   (0x7)
#define DW_TRNG_CMD_RUN_KAT         (0x8)
#define DW_TRNG_CMD_ZEROIZE         (0xF)

/**
 * @fn dw_trng_unmask_reg_interrupt(DW_TRNG_REG_PTR trng_reg, uint32_t mask)
 * @brief Enable designware trng bit interrupt with mask
 */
Inline void dw_trng_unmask_reg_interrupt(DW_TRNG_REG_PTR trng_reg, uint32_t mask)
{
	trng_reg->IE |= mask;
}

/**
 * @fn void dw_trng_mask_reg_interrupt(DW_TRNG_REG_PTR trng_reg, uint32_t mask)
 * @brief Disable designware trng bit interrupt with mask
 */
Inline void dw_trng_mask_reg_interrupt(DW_TRNG_REG_PTR trng_reg, uint32_t mask)
{
	trng_reg->IE &= ~mask;
}

/**
 * @fn void dw_trng_wait_till_idle(DW_TRNG_REG_PTR trng_reg)
 */
Inline void dw_trng_wait_till_idle(DW_TRNG_REG_PTR trng_reg)
{
	while (trng_reg->stat.busy) {
		;
	}
}

static int32_t dw_trng_cmd(DEV_TRNG_INFO_PTR trng_info_ptr, uint32_t cmd)
{
	int32_t ercd = E_OK;
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;

	dw_trng_wait_till_idle(trng_reg_ptr);
	switch (cmd) {
	case DW_TRNG_CMD_NOP:
	case DW_TRNG_CMD_GEN_NOISE:
	case DW_TRNG_CMD_GEN_NONCE:
	case DW_TRNG_CMD_CREATE_STATE:
	case DW_TRNG_CMD_RENEW_STATE:
	case DW_TRNG_CMD_REFRESH_ADDIN:
	case DW_TRNG_CMD_GEN_RANDOM:
	case DW_TRNG_CMD_ADVANCE_STATE:
	case DW_TRNG_CMD_RUN_KAT:
	case DW_TRNG_CMD_ZEROIZE:
		trng_reg_ptr->CTRL |= cmd; break;
	default:
		ercd = E_PAR; break;
	}
	return ercd;
}

static void dw_trng_enable_sys_interrupt(DEV_TRNG_INFO_PTR trng_info_ptr)
{
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = (DW_TRNG_CTRL_PTR)(trng_info_ptr->trng_ctrl);

	trng_ctrl_ptr->int_status |= DW_TRNG_GINT_ENABLE;
	if (trng_ctrl_ptr->intno != DW_TRNG_INVALID_INTNO) {
		int_handler_install(trng_ctrl_ptr->intno, trng_ctrl_ptr->dw_trng_int_handler);
		int_enable(trng_ctrl_ptr->intno);
	}
}

static void dw_trng_disable_sys_interrupt(DEV_TRNG_INFO_PTR trng_info_ptr)
{
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = (DW_TRNG_CTRL_PTR)(trng_info_ptr->trng_ctrl);

	if (trng_ctrl_ptr->intno != DW_TRNG_INVALID_INTNO) {
		int_disable(trng_ctrl_ptr->intno);
	}
	trng_ctrl_ptr->int_status = DW_TRNG_GINT_DISABLE;
}

static int32_t dw_trng_zeroize(DEV_TRNG_INFO_PTR trng_info_ptr)
{
	int32_t ercd = E_OK;

	// DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	// DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;
	dw_trng_enable_sys_interrupt(trng_info_ptr);
	ercd = dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_ZEROIZE);
	return ercd;
}

static int32_t dw_trng_reseed(DEV_TRNG_INFO_PTR trng_info_ptr, uint32_t *nonce_buf)
{
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;

	if (nonce_buf != NULL) {// Nonce Reseed cmd, need to be run 2~3 time with new nonce depending on chosen security algorithm
		uint8_t i;
		volatile uint32_t *npa_data_ptr = &(trng_reg_ptr->NPA_DATA0);
		dw_trng_wait_till_idle(trng_reg_ptr);
		for (i = 0; i < 16; i++) {
			npa_data_ptr[i] = nonce_buf[i];
		}
		dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_GEN_NONCE);
	} else {// Random Reseed cmd
		dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_GEN_NOISE);
	}
	return E_OK;
}

static void dw_trng_get_rand(DW_TRNG_REG_PTR trng_reg_ptr, uint32_t *data) // read from rand0~3
{
	if (data != NULL) {
		data[0] = trng_reg_ptr->RAND0;
		data[1] = trng_reg_ptr->RAND1;
		data[2] = trng_reg_ptr->RAND2;
		data[3] = trng_reg_ptr->RAND3;
	} else {
		dbg_printf(DBG_LESS_INFO, "\r\n!DW_TRNG meet invalid memory error: LAST_CMD=0x%x\r\n", trng_reg_ptr->stat.last_cmd);
	}
}

static void dw_iic_mst_int_read(DEV_TRNG_PTR trng_obj)
{
	DEV_TRNG_INFO_PTR trng_info_ptr = &trng_obj->trng_info;
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;
	DEV_BUFFER *buf_ptr = &(trng_info_ptr->out_buf);
	uint32_t *p_buf = (uint32_t *)buf_ptr->buf;

	if (p_buf != NULL && buf_ptr->len >= buf_ptr->ofs + 4) {// check if buffer has enough space (needs 4 uint32_t per read)
		dw_trng_get_rand(trng_reg_ptr, p_buf);
		buf_ptr->ofs += 4;
	}
	if (trng_info_ptr->trng_cbs.out_cb) {
		trng_info_ptr->trng_cbs.out_cb(trng_info_ptr);
	}
}

int32_t dw_trng_open(DEV_TRNG_PTR trng_obj) // reseed with internal random seed generator
{
	int32_t ercd = E_OK;

	DW_TRNG_CHECK_EXP(trng_obj != NULL, E_OBJ);
	DEV_TRNG_INFO_PTR trng_info_ptr = &trng_obj->trng_info;
	VALID_CHK_TRNG_INFO_OBJECT(trng_info_ptr);
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;

	trng_reg_ptr->smode.secure_en = 1;// enable mission mode (secure)
	/**
	 * trng interrupt related init
	 */
	dw_trng_disable_sys_interrupt(trng_info_ptr);
	dw_trng_unmask_reg_interrupt(trng_reg_ptr, DW_TRNG_INT_ALL);
	dw_trng_zeroize(trng_info_ptr);
	// can do a KAT test here to ensure DRBGs are running correctly, need to zeroize afterwards
	dw_trng_reseed(trng_info_ptr, NULL);

	ercd = dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_CREATE_STATE);

error_exit:
	return ercd;
}

int32_t dw_trng_close(DEV_TRNG_PTR trng_obj)
{
	int32_t ercd = E_OK;

	DW_TRNG_CHECK_EXP(trng_obj != NULL, E_OBJ);
	DEV_TRNG_INFO_PTR trng_info_ptr = &trng_obj->trng_info;
	VALID_CHK_TRNG_INFO_OBJECT(trng_info_ptr);
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;

	dw_trng_mask_reg_interrupt(trng_reg_ptr, DW_TRNG_INT_ALL);
	dw_trng_disable_sys_interrupt(trng_info_ptr);
	// clean/release in buffer and out buffer
	memset(&(trng_info_ptr->in_buf), 0, sizeof(DEV_BUFFER));
	memset(&(trng_info_ptr->out_buf), 0, sizeof(DEV_BUFFER));
	memset(&(trng_info_ptr->trng_cbs), 0, sizeof(DEV_TRNG_CBS));

	// Do a zeroize for security
	dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_ZEROIZE);
	while (!trng_reg_ptr->istat.zeroized) {
		;                                // wait until secure data cleaned
	}
	trng_reg_ptr->istat.zeroized = 1;

error_exit:
	return E_OK;
}

int32_t dw_trng_control(DEV_TRNG_PTR trng_obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;

	DW_TRNG_CHECK_EXP(trng_obj != NULL, E_OBJ);
	DEV_TRNG_INFO_PTR trng_info_ptr = &trng_obj->trng_info;
	VALID_CHK_TRNG_INFO_OBJECT(trng_info_ptr);

	// uint32_t val32;	/** to receive unsigned int value */

	switch (ctrl_cmd) {
	case TRNG_CMD_SET_RAND_RESEED_AUTO:
		ercd = dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_RENEW_STATE);
		break;
	case TRNG_CMD_SET_RAND_RESEED_NONCE:
		ercd = dw_trng_reseed(trng_info_ptr, param);
		break;
	case TRNG_CMD_SET_IN_CB:
		DW_TRNG_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		trng_info_ptr->trng_cbs.in_cb = param;
		break;
	case TRNG_CMD_SET_OUT_CB:
		DW_TRNG_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		trng_info_ptr->trng_cbs.out_cb = param;
		break;
	case TRNG_CMD_SET_ERR_CB:
		DW_TRNG_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
		trng_info_ptr->trng_cbs.err_cb = param;
		break;
	default:
		ercd = E_NOSPT;
		break;
	}

error_exit:
	return ercd;
}

int32_t dw_trng_read(DEV_TRNG_PTR trng_obj, uint32_t *data_buf)
{
	int32_t ercd = E_OK;

	DW_TRNG_CHECK_EXP(trng_obj != NULL, E_OBJ);
	DEV_TRNG_INFO_PTR trng_info_ptr = &trng_obj->trng_info;
	VALID_CHK_TRNG_INFO_OBJECT(trng_info_ptr);
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;

	dw_trng_cmd(trng_info_ptr, DW_TRNG_CMD_GEN_RANDOM);
	if (data_buf != NULL) {
		dw_trng_wait_till_idle(trng_reg_ptr);
		dw_trng_get_rand(trng_reg_ptr, data_buf);
	}

error_exit:
	return ercd;
}

void dw_trng_isr(DEV_TRNG_PTR trng_obj, void *ptr)
{
	int32_t ercd = E_OK;

	DW_TRNG_CHECK_EXP(trng_obj != NULL, E_OBJ);
	DEV_TRNG_INFO_PTR trng_info_ptr = &(trng_obj->trng_info);

	/* START ERROR CHECK */
	VALID_CHK_TRNG_INFO_OBJECT(trng_info_ptr);
	/* END OF ERROR CHECK */
	DW_TRNG_CTRL_PTR trng_ctrl_ptr = trng_info_ptr->trng_ctrl;
	DW_TRNG_REG_PTR trng_reg_ptr = (DW_TRNG_REG_PTR) trng_ctrl_ptr->dw_trng_regs;
	dbg_printf(DBG_MORE_INFO, "dw_trng_isr ");

	if (trng_reg_ptr->alarms.illegal_cmd_seq) {
		dbg_printf(DBG_LESS_INFO, "\r\n!DW_TRNG meet illegal_cmd_seq error: LAST_CMD=0x%x\r\n", trng_reg_ptr->stat.last_cmd);
		dw_trng_disable_sys_interrupt(trng_info_ptr);
		trng_reg_ptr->alarms.illegal_cmd_seq = 1;// Write 1 to clear bit
		if (trng_info_ptr->trng_cbs.err_cb) {
			trng_info_ptr->trng_cbs.err_cb(trng_info_ptr);
		}
	}

	if (trng_reg_ptr->istat.zeroized) {             // zeroize operations can interrupt other operations
		trng_reg_ptr->istat.zeroized = 1;       // Write 1 to clear bit
		dbg_printf(DBG_MORE_INFO, "zeroized");
	}
	if (trng_reg_ptr->istat.noise_rdy) {
		trng_reg_ptr->istat.noise_rdy = 1;// Write 1 to clear bit
		dbg_printf(DBG_MORE_INFO, "noise_rdy");
	}
	if (trng_reg_ptr->istat.done) {
		trng_reg_ptr->istat.done = 1;// Write 1 to clear bit
		dbg_printf(DBG_MORE_INFO, "done");
		if (trng_reg_ptr->stat.last_cmd == DW_TRNG_CMD_GEN_RANDOM) {
			dbg_printf(DBG_MORE_INFO, " --- GEN RANDOM done");
			trng_info_ptr->byte_generated += 4;// TODO: once too many bytes have been generated, prompt to reseed
			dw_iic_mst_int_read(trng_obj);
		}
	}
	if (trng_reg_ptr->istat.alarms) {
		trng_reg_ptr->istat.alarms = 1;// Write 1 to clear bit
		dbg_printf(DBG_MORE_INFO, "alarms");
	}
	if (trng_reg_ptr->istat.kat_completed) {
		trng_reg_ptr->istat.kat_completed = 1;// Write 1 to clear bit
		dbg_printf(DBG_MORE_INFO, "kat_completed");
	}

	dbg_printf(DBG_MORE_INFO, " #\r\n");

error_exit:
	return;
}

#endif /* TRNG_VERSION_NIST */
