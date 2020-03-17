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
/*------------------------------------------------------------------------/
   /  Universal string handler for user console interface
   /-------------------------------------------------------------------------/
   /
   /  Copyright (C) 2011, ChaN, all right reserved.
   /
   / * This software is a free software and there is NO WARRANTY.
   / * No restriction on use. You can use, modify and redistribute it for
   /   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
   / * Redistributions of source code must retain the above copyright notice.
   /
   /-------------------------------------------------------------------------*/

#include "console/xprintf/xprintf.h"

#if XPRINTF_USE_XFUNC_OUT
#include <stdarg.h>
void (*xfunc_out)(uint8_t);     /* Pointer to the output stream */
static char *outptr;

/**
 * @brief Put a character
 *
 * @param c Character to put
 */
void xputc(uint8_t c)
{
	if (XPRINTF_CR_CRLF && c == '\n') {
		xputc('\r');    /* CR -> CRLF */

	}
	if (outptr) {
		*outptr++ = (uint8_t)c;
		return;
	}

	if (xfunc_out) {
		xfunc_out((uint8_t)c);
	}
}

/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

/**
 * @brief Put a string to the default device
 *
 * @param str Pointer to the string
 */
void xputs(const char *str)
{
	while (*str) {
		xputc(*str++);
	}
}

/**
 * @brief Put a string to the specified device
 *
 * @param func Pointer to the output function
 * @param str Pointer to the string
 */
void xfputs(void (*func)(uint8_t), const char *str)
{
	void (*pf)(uint8_t);

	pf = xfunc_out;         /* Save current output device */
	xfunc_out = func;       /* Switch output to specified device */
	while (*str) {          /* Put the string */
		xputc(*str++);
	}
	xfunc_out = pf;         /* Restore output device */
}

/**
 * @brief Formatted string output
 *  xprintf("%d", 1234);		"1234"
 *   xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
 *   xprintf("%-6u", 100);		"100   "
 *   xprintf("%ld", 12345678L);		"12345678"
 *  xprintf("%04x", 0xA3);		"00a3"
 *  xprintf("%08LX", 0x123ABC);		"00123ABC"
 *  xprintf("%016b", 0x550F);		"0101010100001111"
 *  xprintf("%s", "String");		"String"
 *  xprintf("%-4s", "abc");		"abc "
 *  xprintf("%4s", "abc");		 abc"
 *  xprintf("%c", 'a');			"a"
 *  xprintf("%f", 10.0);		"xprintf lacks floating point support"
 * @param fmt Pointer to the format string
 * @param arp  Pointer to arguments
 */
void xvprintf(const char *fmt, va_list arp)
{
	uint32_t r, i, j, w, f;
	uint32_t v;
	char s[16], c, d, *p;

	for (;;) {
		r = 0;
		c = *fmt++;                             /* Get a char */
		if (!c) {
			break;                          /* End of format? */
		}
		if (c != '%') {                         /* Pass through it if not a % sequence */
			xputc(c);
			continue;
		}
		f = 0;
		c = *fmt++;                             /* Get first char of the sequence */
		if (c == '0') {                         /* Flag: '0' padded */
			f = 1;
			c = *fmt++;
		} else {
			if (c == '-') {                 /* Flag: left justified */
				f = 2;
				c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++) { /* Minimum width */
			w = w * 10 + c - '0';
		}
		if (c == 'l' || c == 'L') {     /* Prefix: Size is long int */
			f |= 4;
			c = *fmt++;
		}
		if (!c) {
			break;                          /* End of format? */
		}
		d = c;
		if (d >= 'a') {
			d -= 0x20;
		}
		switch (d) {                            /* Type is... */
		case 'S':                               /* String */
			p = va_arg(arp, char *);
			for (j = 0; p[j]; j++) {
				;
			}
			while (!(f & 2) && j++ < w) {
				xputc(' ');
			}
			xputs(p);
			while (j++ < w) {
				xputc(' ');
			}
			break;
		case 'C':                                       /* Character */
			xputc((char)va_arg(arp, int32_t));
			break;
		case 'B':                                       /* Binary */
			r = 2;
			break;
		case 'O':                                       /* Octal */
			r = 8;
			break;
		case 'D':                                       /* Signed decimal */
		case 'U':                                       /* Unsigned decimal */
			r = 10;
			break;
		case 'X':                                       /* Hexdecimal */
			r = 16;
			break;
		default:                                        /* Unknown type (passthrough) */
			xputc(c);
			break;
		}

		if (r == 0) {
			continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? va_arg(arp, int32_t) : ((d == 'D') ? (int32_t)va_arg(arp, int32_t) : (int32_t)va_arg(arp, uint32_t));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (char)(v % r); v /= r;
			if (d > 9) {
				d += (c == 'x') ? 0x27 : 0x07;
			}
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if ((f & 8) && (i < sizeof(s))) {
			s[i++] = '-';
		}
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) {
			xputc(d);
		}
		do {
			xputc(s[--i]);
		} while (i);
		while (j++ < w) {
			xputc(' ');
		}
	}
}

/**
 * @brief Put a formatted string to the default device
 *
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xprintf(const char *fmt, ...)
{
	va_list arp;

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);
}

/**
 * @brief Put a formatted string to the memory
 *
 * @param buff Pointer to the output buffer
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xsprintf(char *buff, const char *fmt, ...)
{
	va_list arp;

	outptr = buff;          /* Switch destination for memory */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	*outptr = 0;            /* Terminate output string with a \0 */
	outptr = 0;             /* Switch destination for device */
}

/**
 * @brief Put a formatted string to the specified device
 *
 * @param func Pointer to the output function
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xfprintf(void (*func)(uint8_t), const char *fmt, ...)
{
	va_list arp;

	void (*pf)(uint8_t);

	pf = xfunc_out;         /* Save current output device */
	xfunc_out = func;       /* Switch output to specified device */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	xfunc_out = pf;         /* Restore output device */
}

/**
 * @brief Dump a line of binary dump
 *
 * @param buff Pointer to the array to be dumped
 * @param addr Heading address value
 * @param len Number of items to be dumped
 * @param width Size of the items (DF_8BIT, DF_16BIT, DF_32BIT)
 */
void put_dump(const void *buff, uint32_t addr, int32_t len, int32_t width)
{
	int32_t i;
	const uint8_t *bp;
	const uint16_t *sp;
	const uint32_t *lp;

	/* address */
	xprintf("%08lX ", addr);

	switch (width) {
	case DW_8BIT:
		bp = buff;
		/* Hexdecimal dump */
		for (i = 0; i < len; i++) {
			xprintf(" %02X", bp[i]);
		}
		xputc(' ');
		/* ASCII dump */
		for (i = 0; i < len; i++) {
			xputc((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		}
		break;
	case DW_16BIT:
		sp = buff;
		/* Hexdecimal dump */
		do {
			xprintf(" %04X", *sp++);
		} while (--len);
		break;
	case DW_32BIT:
		lp = buff;
		/* Hexdecimal dump */
		do {
			xprintf(" %08LX", *lp++);
		} while (--len);
		break;
	default:
		break;
	}

	xputc('\n');
}

#endif /* XPRINTF_USE_XFUNC_OUT */

#if XPRINTF_USE_XFUNC_IN
uint8_t (*xfunc_in)(void);      /* Pointer to the input stream */

/**
 * @brief Get a line from the input
 *
 * @param buff Pointer to the buffer
 * @param len Buffer length
 * @return 0:End of stream, 1:A line arrived
 */
int32_t xgets(char *buff, int32_t len)
{
	int32_t c, i;

	/* No input function specified */
	if (!xfunc_in) {
		return 0;
	}

	i = 0;
	for (;;) {
		/* Get a char from the incoming stream */
		c = xfunc_in();
		/* End of stream? */
		if (!c) {
			return 0;
		}
		/* End of line? */
		if (c == '\r') {
			break;
		}
		/* Back space? */
		if (c == '\b' && i) {
			i--;
			if (XPRINTF_LINE_ECHO) {
				xputc(c);
			}
			continue;
		}
		/* Visible chars */
		if (c >= ' ' && i < len - 1) {
			buff[i++] = c;
			if (XPRINTF_LINE_ECHO) {
				xputc(c);
			}
		}
	}
	/* Terminate with a \0 */
	buff[i] = 0;
	if (XPRINTF_LINE_ECHO) {
		xputc('\n');
	}
	return 1;
}

/**
 * @brief Get a line from the input
 *
 * @param func Pointer to the input stream function
 * @param buff Pointer to the buffer
 * @param len  Buffer length
 * @return 0:End of stream, 1:A line arrived
 */
int32_t xfgets(uint8_t (*func)(void), char *buff, int32_t len)
{
	uint8_t (*pf)(void);
	int32_t n;

	pf = xfunc_in;          /* Save current input device */
	xfunc_in = func;        /* Switch input to specified device */
	n = xgets(buff, len);   /* Get a line */
	xfunc_in = pf;          /* Restore input device */

	return n;
}

/**
 * @brief Get a value of the string
 *      "123 -5   0x3ff 0b1111 0377  w "
 *          ^                           1st call returns 123 and next ptr
 *            ^                        2nd call returns -5 and next ptr
 *                   ^                3rd call returns 1023 and next ptr
 *                          ^         4th call returns 15 and next ptr
 *                               ^    5th call returns 255 and next ptr
 *                                  ^ 6th call fails and returns 0
 * @param str Pointer to pointer to the string
 * @param res Pointer to the variable to store the value
 * @return Failed, 1:Successful
 */
int32_t xatoi(char **str, int32_t *res)
{
	uint32_t val;
	uint8_t c, r, s = 0;

	*res = 0;

	/* Skip leading spaces */
	while ((c = **str) == ' ') {
		(*str)++;

	}
	/* negative? */
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		/* hexdecimal */
		case 'x':
			r = 16;
			c = *(++(*str));
			break;
		/* binary */
		case 'b':
			r = 2;
			c = *(++(*str));
			break;
		default:
			/* single zero */
			if (c <= ' ') {
				return 1;
			}
			/* invalid char */
			if (c < '0' || c > '9') {
				return 0;
			}
			/* octal */
			r = 8;
			break;
		}
	} else {
		/* EOL or invalid char */
		if (c < '0' || c > '9') {
			return 0;
		}
		/* decimal */
		r = 10;
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') {
			c -= 0x20;
		}
		c -= '0';
		if (c >= 17) {
			c -= 7;
		}
		/* invalid char for current radix */
		if (c >= r) {
			return 0;
		}
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) {
		/* apply sign if needed */
		val = 0 - val;

	}
	*res = val;
	return 1;
}

#endif /* XPRINTF_USE_XFUNC_IN */
