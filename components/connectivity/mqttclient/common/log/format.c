/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:54:19
 * @LastEditTime : 2019-12-28 01:53:41
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "format.h"


static int _get_atoi(const char **str)
{
    int n;
    for (n = 0; is_digit(**str); (*str)++)
        n = n * 10 + **str - '0';
    return n;
}

static void _buff_put_char(char *buf, unsigned int *pos, unsigned int max, char c)
{
    if (*pos < max)
        buf[(*pos)] = c;
    (*pos)++;
}


/**
 * Formats an integer number
 *  buf - buffer to print into
 *  len - current position in buffer
 *  maxlen - last valid position in buf
 *  num - number to print
 *  base - it's base
 *  width - how many spaces this should have; padding
 *  flags - above F flags
 */
static void _format_int(char *buf, unsigned int *len, unsigned int maxlen,
        signed long long num, int base, int width, int flags)
{
    char nbuf[64], sign = 0;
    char altb[8]; // small buf for sign and #
    int n = num;
    int npad;           // number of pads
    char pchar = ' ';   // padding character
    char *digits = "0123456789ABCDEF";
    char *ldigits = "0123456789abcdef";
    int i, j;

    if (base < 2 || base > 16)
        return;
    if (flags & F_SMALL) digits = ldigits;
    if (flags & F_LEFT) flags &= ~F_ZEROPAD;

    if ((flags & F_SIGNED) && num < 0) {
        n = -num;
        sign = '-';
    } else if (flags & F_PLUS) {
        sign = '+';
    } else if (flags & F_SPACE)
        sign = ' ';

    i = 0;
    do {
        nbuf[i++] = digits[n % base];
        n = n / base;
    } while (n > 0);
   
    j = 0;
    if (sign) altb[j++] = sign;
    if (flags & F_ALTERNATE) {
        if (base == 8 || base == 16) {
            altb[j++] = '0';
            if (base == 16)
                altb[j++] = (flags & F_SMALL) ? 'x' : 'X';
        }
    }
    altb[j] = 0;

    npad = width > i + j ? width - i - j : 0;

    if (width > i + j)
        npad = width - i - j;

    if (npad > 0 && ((flags & F_LEFT) == 0)) {
        if (flags & F_ZEROPAD) {
            for (j = 0; altb[j]; j++)
                _buff_put_char(buf, len, maxlen, altb[j]);
            altb[0] = 0;
        }
        while (npad-- > 0)
            _buff_put_char(buf, len, maxlen, (flags & F_ZEROPAD) ? '0' : ' ');
    }
    for (j = 0; altb[j]; j++)
        _buff_put_char(buf, len, maxlen, altb[j]);

    while (i-- > 0)
        _buff_put_char(buf, len, maxlen, nbuf[i]);

    if (npad > 0 && (flags & F_LEFT))
        while(npad-- > 0)
            _buff_put_char(buf, len, maxlen, pchar);
}

static void _format_char(char *buf, unsigned int *pos, unsigned int max, char c,
        int width, int flags)
{
    int npad = 0;
    if (width > 0) npad = width - 1;
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _buff_put_char(buf, pos, max, ' ');

    _buff_put_char(buf, pos, max, c);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _buff_put_char(buf, pos, max, ' ');
}

/**
 * strlen()
 */
static unsigned int _str_len(char *s)
{
    unsigned int i;
    for (i = 0; *s; i++, s++)
        ;
    return i;
}

static void _format_str(char *buf, unsigned int *pos, unsigned int max, char *s,
        int width, int flags)
{
    int npad = 0;
    if (width > 0) npad = width - _str_len(s);
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _buff_put_char(buf, pos, max, ' ');

    while (*s)
        _buff_put_char(buf, pos, max, *s++);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _buff_put_char(buf, pos, max, ' ');
}




/***********************************************************************************************************************/

/**
 * Shrinked down, vsnprintf implementation.
 *  This will not handle floating numbers (yet).
 */
int format_nstr(char *buf, unsigned int size, const char *fmt, va_list ap)
{
    unsigned int n = 0;
    char c, *s;
    char state = 0;
    signed long long num;
    int base;
    int flags, width, precision, lflags;

    if (!buf) size = 0;

    for (;;) {
        c = *fmt++;
        if (state == S_DEFAULT) {
            if (c == '%') {
                state = S_FLAGS;
                flags = 0;
            } else {
                _buff_put_char(buf, &n, size, c);
            }
        } else if (state == S_FLAGS) {
            switch (c) {
                case '#': flags |= F_ALTERNATE; break;
                case '0': flags |= F_ZEROPAD; break;
                case '-': flags |= F_LEFT; break;
                case ' ': flags |= F_SPACE; break;
                case '+': flags |= F_PLUS; break;
                case '\'':
                case 'I' : break; // not yet used
                default: fmt--; width = 0; state = S_WIDTH;
            }
        } else if (state == S_WIDTH) {
            if (c == '*') {
                width = va_arg(ap, int);
                if (width < 0) {
                    width = -width;
                    flags |= F_LEFT;
                }
            } else if (is_digit(c) && c > '0') {
                fmt--;
                width = _get_atoi(&fmt);
            } else {
                fmt--;
                precision = -1;
                state = S_PRECIS;
            }
        } else if (state == S_PRECIS) {
            // Ignored for now, but skip it
            if (c == '.') {
                if (is_digit(*fmt))
                    precision = _get_atoi(&fmt);
                else if (*fmt == '*')
                    precision = va_arg(ap, int);
                precision = precision < 0 ? 0 : precision;
            } else
                fmt--;
            lflags = 0;
            state = S_LENGTH;
        } else if (state == S_LENGTH) {
            switch(c) {
                case 'h': lflags = lflags == L_CHAR ? L_SHORT : L_CHAR; break;
                case 'l': lflags = lflags == L_LONG ? L_LLONG : L_LONG; break;
                case 'L': lflags = L_DOUBLE; break;
                default: fmt--; state = S_CONV;
            }
        } else if (state == S_CONV) {
            if (c == 'd' || c == 'i' || c == 'o' || c == 'b' || c == 'u' 
                    || c == 'x' || c == 'X') {
                if (lflags == L_LONG)
                    num = va_arg(ap, int);
                else if (lflags & (L_LLONG | L_DOUBLE))
                    num = va_arg(ap, signed long long);
                else if (c == 'd' || c == 'i')
                    num = va_arg(ap, int);
                else
                    num = (unsigned int) va_arg(ap, int);

                base = 10;
                if (c == 'd' || c == 'i') {
                    flags |= F_SIGNED;
                } else if (c == 'x' || c == 'X') {
                    flags |= c == 'x' ? F_SMALL : 0;
                    base = 16;
                } else if (c == 'o') {
                    base = 8;
                } else if (c == 'b') {
                    base = 2;
                }
                _format_int(buf, &n, size, num, base, width, flags);
            } else if (c == 'p') {
                num = (size_t) va_arg(ap, void *);
                base = 16;
                flags |= F_SMALL | F_ALTERNATE;
                _format_int(buf, &n, size, num, base, width, flags);
            } else if (c == 's') {
                s = va_arg(ap, char *);
                if (!s)
                    s = "(null)";
                _format_str(buf, &n, size, s, width, flags);
            } else if (c == 'c') {
                c = va_arg(ap, int);
                _format_char(buf, &n, size, c, width, flags);
            } else if (c == '%') {
                _buff_put_char(buf, &n, size, c);
            } else {
                _buff_put_char(buf, &n, size, '%');
                _buff_put_char(buf, &n, size, c);
            }
            state = S_DEFAULT;
        }
        if (c == 0)
            break;
    }
    n--;
    if (n < size)
        buf[n] = 0;
    else if (size > 0)
        buf[size - 1] = 0;

    return n;
}







