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
#ifndef H_SS_SPI_MASTER
#define H_SS_SPI_MASTER

/* the wrapper of subsystem spi master driver */

#include "device/ip_hal/dev_spi.h"

#define SS_SPI_MASTER_FLAG_TX_READY     (1 << 0)        /* interrupt tx */
#define SS_SPI_MASTER_FLAG_RX_READY     (1 << 1)        /* interrupt rx */
#define SS_SPI_MASTER_FLAG_BUSY         (1 << 2)
#define SS_SPI_MASTER_FLAG_TX_RX        (1 << 3)        /* both tx and rx */
#define SS_SPI_MASTER_FLAG_ERROR        (1 << 4)

typedef struct ss_spi_master_dev_context {
	uint32_t reg_base;
	uint32_t dev_id;

	uint8_t intno_rx;
	uint8_t intno_tx;
	uint8_t intno_idle;
	uint8_t intno_err;

	uint32_t bus_freq;
	volatile uint32_t flags;

	DEV_CALLBACK int_rx_cb;
	DEV_CALLBACK int_tx_cb;
	DEV_CALLBACK int_err_cb;
	DEV_SPI_INFO *info;
} SS_SPI_MASTER_DEV_CONTEXT;

extern int32_t ss_spi_master_open(SS_SPI_MASTER_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param);
extern int32_t ss_spi_master_close(SS_SPI_MASTER_DEV_CONTEXT *ctx);
extern int32_t ss_spi_master_control(SS_SPI_MASTER_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param);
extern int32_t ss_spi_master_write(SS_SPI_MASTER_DEV_CONTEXT *ctx, const void *data, uint32_t len);
extern int32_t ss_spi_master_read(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern void ss_spi_master_tx_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param);
extern void ss_spi_master_rx_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param);
extern void ss_spi_master_err_cb(SS_SPI_MASTER_DEV_CONTEXT *ctx, void *param);

#endif /* H_SS_SPI_MASTER */