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
#ifndef H_SS_UART
#define H_SS_UART
/* the wrapper of subsystem uart driver */

#include "device/ip_hal/dev_uart.h"

#define SS_UART_FLAG_TX     (1 << 0)    /* interrupt tx */
#define SS_UART_FLAG_RX     (1 << 1)    /* interrupt rx */
#define SS_UART_FLAG_BUSY   (1 << 2)
#define SS_UART_FLAG_TX_RX  (1 << 3)    /* both tx and rx */
#define SS_UART_FLAG_ERROR  (1 << 4)

typedef struct ss_uart_dev_context {
	uint32_t reg_base;
	uint8_t dev_id;

	uint8_t intno;
	volatile uint16_t flags;

	uint32_t bus_freq;

	DEV_CALLBACK tx_cb;
	DEV_CALLBACK rx_cb;
	DEV_CALLBACK err_cb;

	DEV_UART_INFO *info;
} SS_UART_DEV_CONTEXT;

extern int32_t ss_uart_open(SS_UART_DEV_CONTEXT *ctx, uint32_t baud);
extern int32_t ss_uart_close(SS_UART_DEV_CONTEXT *ctx);
extern int32_t ss_uart_control(SS_UART_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param);
extern int32_t ss_uart_write(SS_UART_DEV_CONTEXT *ctx, const void *data, uint32_t len);
extern int32_t ss_uart_read(SS_UART_DEV_CONTEXT *ctx, void *data, uint32_t len);
extern void ss_uart_tx_cb(SS_UART_DEV_CONTEXT *ctx, void *param);
extern void ss_uart_rx_cb(SS_UART_DEV_CONTEXT *ctx, void *param);
extern void ss_uart_err_cb(SS_UART_DEV_CONTEXT *ctx, void *param);

#endif /* H_SS_UART */