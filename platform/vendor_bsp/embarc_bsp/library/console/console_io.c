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

/**
 * @file
 * @brief implemantation of simple console
 */
#include <stdio.h>
#include "embARC.h"

static DEV_UART *console_uart;      /*!< console uart device pointer */

/** put one char */
static int32_t console_putchar(uint8_t chr)
{
	if (console_uart == NULL) {
		return -1;
	}

	console_uart->uart_write((const void *)(&chr), 1);

	return 0;
}

/** get one char*/
static int32_t console_getchar(void)
{
	uint8_t data;

	if (console_uart == NULL) {
		return -1;
	}

	while (!console_uart->uart_read((void *)(&data), 1)) {
		;
	}

	return (int32_t)data;
}

/** xprintf need functions api setup */
void xprintf_setup(void)
{
	console_uart = uart_get_dev(BOARD_CONSOLE_UART_ID);
	console_uart->uart_open(BOARD_CONSOLE_UART_BAUD);

	xdev_in(console_getchar);
	xdev_out(console_putchar);
}
