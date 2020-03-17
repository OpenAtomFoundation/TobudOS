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
#include "device/designware/dw_trng.h"

#ifndef TRNG_VERSION_NIST

#define DW_TRNG_CHECK_EXP(EXPR, ERROR_CODE)     CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#define DW_TRNG_MODE_R256           0x08
#define DW_TRNG_MODE_R256_OFFSET    3

#define DW_TRNG_ISTAT_RAND_RDY              0x01
#define DW_TRNG_ISTAT_RAND_RDY_OFFSET       0
#define DW_TRNG_ISTAT_SEED_DONE             0x02
#define DW_TRNG_ISTAT_SEED_DONE_OFFSET      1
#define DW_TRNG_ISTAT_AGE_ALARM             0x04
#define DW_TRNG_ISTAT_AGE_ALARM_OFFSET      2
#define DW_TRNG_ISTAT_RQST_ALARM            0x08
#define DW_TRNG_ISTAT_RQST_ALARM_OFFSET     3
#define DW_TRNG_ISTAT_LFSR_LOCKUP           0x10
#define DW_TRNG_ISTAT_LFSR_LOCKUP_OFFSET    4

#define DW_TRNG_CMD_GEN     1
#define DW_TRNG_CMD_RESEED  2
#define DW_TRNG_CMD_NONCE   3

static int32_t dw_trng_cmd(DW_TRNG_REG_PTR trng_reg, uint32_t cmd)
{
	int32_t ercd = E_OK;

	switch (cmd) {
	case DW_TRNG_CMD_GEN:
		trng_reg->CTRL |= cmd;
		break;

	case DW_TRNG_CMD_RESEED:
		trng_reg->CTRL |= cmd;
		break;

	case DW_TRNG_CMD_NONCE:
		trng_reg->CTRL |= cmd;
		break;

	default:
		ercd = E_PAR; break;
	}
	return ercd;
}

inline static int32_t dw_trng_get_mode(DW_TRNG_REG_PTR trng_reg)
{
	return (trng_reg->MODE & DW_TRNG_MODE_R256) >> 3;
}

inline static void dw_trng_set_mode(DW_TRNG_REG_PTR trng_reg, uint32_t r256)
{
	if (r256) {
		trng_reg->MODE |= DW_TRNG_MODE_R256;
	} else {
		trng_reg->MODE &= ~DW_TRNG_MODE_R256;
	}
}

inline static int32_t dw_trng_get_istat_rand_rdy(DW_TRNG_REG_PTR trng_reg)
{
	return (trng_reg->ISTAT & DW_TRNG_ISTAT_RAND_RDY) >> DW_TRNG_ISTAT_RAND_RDY_OFFSET;
}

inline static void dw_trng_clear_istat_rand_rdy(DW_TRNG_REG_PTR trng_reg)
{
	trng_reg->ISTAT = DW_TRNG_ISTAT_RAND_RDY;// write 1 to clear register bits
}

inline static int32_t dw_trng_get_istat_seed_done(DW_TRNG_REG_PTR trng_reg)
{
	return (trng_reg->ISTAT & DW_TRNG_ISTAT_SEED_DONE) >> DW_TRNG_ISTAT_SEED_DONE_OFFSET;
}

inline static void dw_trng_clear_istat_seed_done(DW_TRNG_REG_PTR trng_reg)
{
	trng_reg->ISTAT = DW_TRNG_ISTAT_SEED_DONE;// write 1 to clear register bits
}

static void dw_trng_get_rand(DW_TRNG_REG_PTR trng_reg, uint32_t *data) // read from rand0~3 or rand0~7
{
	data[0] = trng_reg->RAND0;
	data[1] = trng_reg->RAND1;
	data[2] = trng_reg->RAND2;
	data[3] = trng_reg->RAND3;
	// if(dw_trng_get_mode(trng_reg) == 1 )
	data[4] = trng_reg->RAND4;
	data[5] = trng_reg->RAND5;
	data[6] = trng_reg->RAND6;
	data[7] = trng_reg->RAND7;
	return;
}

static void dw_trng_get_stat(DW_TRNG_REG_PTR trng_reg, uint32_t *stat)
{
	*stat = trng_reg->STAT;
	return;
}

static int32_t dw_trng_reseed(DW_TRNG_REG_PTR trng_reg, uint32_t *nonce_buf)
{
	if (nonce_buf != NULL) {// Nonce Reseed cmd
		dw_trng_cmd(trng_reg, DW_TRNG_CMD_NONCE);
		// TODO
	} else {// Random Reseed cmd
		dw_trng_cmd(trng_reg, DW_TRNG_CMD_RESEED);
		while (!dw_trng_get_istat_seed_done(trng_reg)) {
			// do nothing, wait until reseed is ready
		}
		dw_trng_clear_istat_seed_done(trng_reg);// clear ISTAT.RAND_RDY bit to acknowledge
	}
	return E_OK;
}

int32_t dw_trng_open(DEV_TRNG_PTR trng_obj) // reseed with internal random seed generator
{
	int32_t ercd = E_OK;
	DEV_TRNG_INFO_PTR ptrng_info = &trng_obj->trng_info;
	DW_TRNG_CTRL_PTR dw_trng_ctrl_ptr = ptrng_info->trng_ctrl;
	DW_TRNG_REG_PTR ptrng = (DW_TRNG_REG_PTR) dw_trng_ctrl_ptr->dw_trng_regs;

	dw_trng_reseed(ptrng, NULL);

error_exit:
	return ercd;
}

int32_t dw_trng_close(DEV_TRNG_PTR trng_obj) // do nothing
{
	return E_OK;
}

int32_t dw_trng_control(DEV_TRNG_PTR trng_obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_TRNG_INFO_PTR ptrng_info = &trng_obj->trng_info;
	DW_TRNG_CTRL_PTR dw_trng_ctrl_ptr = ptrng_info->trng_ctrl;
	DW_TRNG_REG_PTR ptrng = (DW_TRNG_REG_PTR) dw_trng_ctrl_ptr->dw_trng_regs;

	// uint32_t val32;	/** to receive unsigned int value */

	switch (ctrl_cmd) {
	case TRNG_CMD_SET_RAND_128_BIT_OUTPUT:
		dw_trng_set_mode(ptrng, 0);
		break;
	case TRNG_CMD_SET_RAND_256_BIT_OUTPUT:
		dw_trng_set_mode(ptrng, 1);
		break;
	case TRNG_CMD_SET_RAND_RESEED_AUTO:
		dw_trng_reseed(ptrng, NULL);
		break;
	case TRNG_CMD_SET_RAND_RESEED_NONCE:
		if (param == NULL) {
			ercd = E_PAR;
			break;
		}
		dw_trng_reseed(ptrng, param);
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
	DEV_TRNG_INFO_PTR ptrng_info = &trng_obj->trng_info;
	DW_TRNG_CTRL_PTR dw_trng_ctrl_ptr = ptrng_info->trng_ctrl;
	DW_TRNG_REG_PTR ptrng = (DW_TRNG_REG_PTR) dw_trng_ctrl_ptr->dw_trng_regs;

	dw_trng_cmd(ptrng, DW_TRNG_CMD_GEN);
	while (!dw_trng_get_istat_rand_rdy(ptrng)) {
		// do nothing, wait until random numbers are ready
	}
	dw_trng_clear_istat_rand_rdy(ptrng);// clear ISTAT.RAND_RDY bit to acknowledge
	dw_trng_get_rand(ptrng, data_buf);

error_exit:
	return ercd;
}

void dw_trng_isr(DEV_TRNG_PTR trng_obj, void *ptr)
{
	// polling mode, isr not implemented
}

#endif /* TRNG_VERSION_NIST */
