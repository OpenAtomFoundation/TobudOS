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
/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/*------------------------------------------------------------------------*/

#ifndef H_XPRINTF
#define H_XPRINTF

#include <stdarg.h>
#include "embARC_toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XPRINTF_USE_XFUNC_OUT  1        /* 1: Use output functions */
#define XPRINTF_CR_CRLF        1        /* 1: Convert \n ==> \r\n in the output char */

#define XPRINTF_USE_XFUNC_IN   1        /* 1: Use input function */
#define XPRINTF_LINE_ECHO      1        /* 1: Echo back input chars in xgets function */

#if XPRINTF_USE_XFUNC_OUT
#define xdev_out(func) xfunc_out = (void (*)(uint8_t))(func)
extern void (*xfunc_out)(uint8_t);
extern void xputc(uint8_t c);
extern void xputs(const char *str);
extern void xfputs(void (*func)(uint8_t), const char *str);
extern void xvprintf(const char *fmt, va_list arp);
extern void xprintf(const char *fmt, ...);
extern void xsprintf(char *buff, const char *fmt, ...);
extern void xfprintf(void (*func)(uint8_t), const char *fmt, ...);
extern void put_dump(const void *buff, uint32_t addr, int32_t len, int32_t width);
#define DW_8BIT         sizeof(uint8_t)
#define DW_16BIT        sizeof(uint16_t)
#define DW_32BIT        sizeof(uint32_t)
#endif

#if XPRINTF_USE_XFUNC_IN
#define xdev_in(func) xfunc_in = (uint8_t (*)(void))(func)
extern uint8_t (*xfunc_in)(void);
extern int32_t xgets(char *buff, int32_t len);
extern int32_t xfgets(uint8_t (*func)(void), char *buff, int32_t len);
extern int32_t xatoi(char **str, int32_t *res);
#endif

#ifdef __cplusplus
}
#endif

#endif
