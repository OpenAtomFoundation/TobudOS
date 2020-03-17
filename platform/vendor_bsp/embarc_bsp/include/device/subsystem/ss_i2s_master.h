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
#ifndef H_SS_I2S_MASTER
#define H_SS_I2S_MASTER
/* the wrapper of subsystem i2s master driver */

#include "device/ip_hal/dev_i2s.h"

#define SS_I2S_TX       1
#define SS_I2S_RX       2

#define SS_I2S_MASTER_FLAG_TX       (1 << 0)    /* interrupt tx */
#define SS_I2S_MASTER_FLAG_RX       (1 << 1)    /* interrupt rx */
#define SS_I2S_MASTER_FLAG_BUSY     (1 << 2)

#define SS_I2S_MASTER_FLAG_TX_ERROR (1 << 5)
#define SS_I2S_MASTER_FLAG_RX_ERROR (1 << 6)

typedef struct ss_i2s_mst_dev_context {
	uint32_t reg_base;

	uint8_t dev_id;
	uint8_t io_mode;

	uint8_t int_tx_req;     /* Interrupt number of TX_REQ */
	uint8_t int_tx_err;     /* Interrupt number of TX_ERR */
	uint8_t int_rx_avil;    /* Interrupt number of RX_AVIL */
	uint8_t int_rx_err;     /* Interrupt number of RX_ERR */

	DEV_CALLBACK int_tx_cb;
	DEV_CALLBACK int_rx_cb;
	DEV_CALLBACK int_err_cb;

	volatile uint32_t flags;    /*  flag */
	DEV_I2S_INFO_PTR info;
} SS_I2S_MST_DEV_CONTEXT;

extern int32_t ss_i2s_mst_open(SS_I2S_MST_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param);
extern int32_t ss_i2s_mst_close(SS_I2S_MST_DEV_CONTEXT *ctx);
extern int32_t ss_i2s_mst_read(SS_I2S_MST_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern int32_t ss_i2s_mst_write(SS_I2S_MST_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern int32_t ss_i2s_mst_control(SS_I2S_MST_DEV_CONTEXT *ctx, uint32_t cmd, void *param);
extern void ss_i2s_mst_int_tx_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param);
extern void ss_i2s_mst_int_rx_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param);
extern void ss_i2s_mst_int_err_cb(SS_I2S_MST_DEV_CONTEXT *ctx, void *param);

#endif /* H_SS_I2S_MASTER*/
