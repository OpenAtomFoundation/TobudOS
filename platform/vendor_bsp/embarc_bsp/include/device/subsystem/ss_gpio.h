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
#ifndef H_SS_GPIO
#define H_SS_GPIO
/* the wrapper of subsystem gpio driver */

#include "device/ip_hal/dev_gpio.h"

typedef struct ss_gpio_dev_context {
	uint32_t reg_base;

	uint8_t dev_id;
	uint8_t width;
	uint8_t intno;
	uint8_t reserved;

	DEV_CALLBACK int_cb;
	DEV_GPIO_HANDLER *handlers;
	DEV_GPIO_INFO   *info;
	uint32_t int_bit_type;
	uint32_t int_bit_polarity;
	uint32_t int_bit_debounce;
} SS_GPIO_DEV_CONTEXT;

extern int32_t ss_gpio_open(SS_GPIO_DEV_CONTEXT *ctx, uint32_t dir);
extern int32_t ss_gpio_close(SS_GPIO_DEV_CONTEXT *ctx);
extern int32_t ss_gpio_control(SS_GPIO_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param);
extern int32_t ss_gpio_write(SS_GPIO_DEV_CONTEXT *ctx, uint32_t val, uint32_t mask);
extern int32_t ss_gpio_read(SS_GPIO_DEV_CONTEXT *ctx, uint32_t *val, uint32_t mask);
extern void ss_gpio_int_cb(SS_GPIO_DEV_CONTEXT *ctx, void *param);

#endif /* H_SS_GPIO */