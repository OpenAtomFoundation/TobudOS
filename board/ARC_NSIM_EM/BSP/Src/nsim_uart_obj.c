/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

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
#include <stdio.h>
#include "nsim.h"

/**
 * NSIM UART 0 Object Instantiation
 */
#if (USE_NSIM_UART_0)

static DEV_UART nsim_uart_0;                   /*!< nsim uart object */

/** nsim uart 0 open */
static int32_t nsim_uart_0_open(uint32_t baud)
{
	/* no need to open, stdio is used */
	return 0;
}
/** nsim uart 0 close */
static int32_t nsim_uart_0_close(void)
{
	return 0;
}
/** nsim uart 0 control */
static int32_t nsim_uart_0_control(uint32_t ctrl_cmd, void *param)
{
	return 0;
}
/** nsim uart 0 write */
static int32_t nsim_uart_0_write(const void *data, uint32_t len)
{
	return fwrite(data, len, sizeof(uint8_t), stdout);
}
/** nsim uart 0 close */
static int32_t nsim_uart_0_read(void *data, uint32_t len)
{
	uint32_t i;
	int32_t c;

	for (i = 0; i < len; i++) {
		c = getchar();
		if (c < 0) {
			break;
		}
		if (c == 10) {
			c = 13;
		}
		*((uint8_t *)data) = (uint8_t)c;
		data++;
	}

	return i;
}

/** install nsim uart 0 to system */
static void nsim_uart_0_install(void)
{
	DEV_UART *nsim_uart_ptr = &nsim_uart_0;

	/** uart dev init */
	nsim_uart_ptr->uart_open = nsim_uart_0_open;
	nsim_uart_ptr->uart_close = nsim_uart_0_close;
	nsim_uart_ptr->uart_control = nsim_uart_0_control;
	nsim_uart_ptr->uart_write = nsim_uart_0_write;
	nsim_uart_ptr->uart_read = nsim_uart_0_read;

}
#endif /* USE_DW_UART_0 */

/**
 * \brief	install all uart objects
 * \note	\b MUST be called during system init
 */
static void nsim_uart_all_install(void)
{
#if (USE_NSIM_UART_0)
	nsim_uart_0_install();
#endif
}

/** get one uart device structure */
DEV_UART_PTR uart_get_dev(int32_t uart_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		nsim_uart_all_install();
	}

	switch (uart_id) {
#if (USE_NSIM_UART_0)
	case NSIM_UART_0_ID:
		return &nsim_uart_0;
		break;
#endif
	default:
		break;
	}
	return NULL;
}
