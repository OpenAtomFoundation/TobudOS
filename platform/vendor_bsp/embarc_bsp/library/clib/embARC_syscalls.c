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
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#ifdef __GNU__
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/times.h>
#include <sys/stat.h>
#endif

#include "clib/embARC_syscalls.h"
#include "embARC_debug.h"
#include "embARC.h"

/**
 * @todo Consider hostlink and nsim support
 */

#ifndef _HOSTLINK_ /* Not using hostlink library */

#ifndef BOARD_CONSOLE_UART_ID
#define BOARD_CONSOLE_UART_ID       1
#endif

#ifndef BOARD_CONSOLE_UART_BAUD
#define BOARD_CONSOLE_UART_BAUD     UART_BAUDRATE_115200
#endif

#ifndef EMBARC_CFG_STDIO_CRLF
#define EMBARC_CFG_STDIO_CRLF       1
#endif

#define STDIO_FID_OFS           3

static int32_t stdio_uart_inited = 0;
static DEV_UART *stdio_uart_obj;

/**
 * @brief Initialize the uart used for stdio
 *
 */
static void init_stdio_serial(void)
{
	if (stdio_uart_inited) {
		return;
	}
	stdio_uart_inited = 1;
	stdio_uart_obj = uart_get_dev(BOARD_CONSOLE_UART_ID);
	if (stdio_uart_obj) {
		stdio_uart_obj->uart_open(BOARD_CONSOLE_UART_BAUD);
		stdio_uart_obj->uart_control(UART_CMD_SET_BAUD, CONV2VOID(BOARD_CONSOLE_UART_BAUD));
	} else {
		stdio_uart_inited = -1;
	}
}

/**
 * @brief Read stdio
 *
 * @param buffer Buffer to read
 * @param length Length to read
 * @return real Data's number
 */
static int32_t stdio_read(char *buffer, uint32_t length)
{
	if (!stdio_uart_obj) {
		return -1;
	}
	uint32_t avail_len;
	stdio_uart_obj->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&avail_len));
	length = (length < avail_len) ? length : avail_len;
	if (length == 0) {
		length = 1;
	}

	return stdio_uart_obj->uart_read((void *)buffer, (uint32_t)length);
}

/**
 * @brief Write char into stdio
 *
 * @param data Data to write;
 */
static void stdio_write_char(char data)
{
	char val = data;

	stdio_uart_obj->uart_write((const void *)(&val), 1);
}

/**
 * @brief Write data into stdio
 *
 * @param buffer Data to write
 * @param length Data length
 * @return Written data number
 */
static int32_t stdio_write(const char *buffer, uint32_t length)
{
	if (!stdio_uart_obj) {
		return -1;
	}

#if EMBARC_CFG_STDIO_CRLF
	int32_t i = 0;
	while (i < length) {
		if (arc_compiler_rarely(buffer[i] == '\n')) {
			stdio_write_char('\r');
		}
		stdio_write_char(buffer[i]);
		i++;
	}
	return i;
#else
	return stdio_uart_obj->uart_write((const void *)buffer, (uint32_t)length);
#endif
}

/**
 * @brief _read for C library
 *
 */
int32_t SYSCALL_PREFIX(_read)(int32_t handle, char *buffer, uint32_t length){
	if (handle < STDIO_FID_OFS) {
		init_stdio_serial();
		return stdio_read(buffer, length);
	}
	return -1;
}

/**
 * @brief _write for C library
 *
 */
int32_t SYSCALL_PREFIX(_write)(int32_t handle, const char *buffer, uint32_t length){
	if (handle < STDIO_FID_OFS) {
		init_stdio_serial();
		return stdio_write(buffer, length);
	}
	return -1;
}

#if __MW__
#pragma weak read = _read
#pragma weak write = _write
#endif

#endif /* !defined(_HOSTLINK_) */
