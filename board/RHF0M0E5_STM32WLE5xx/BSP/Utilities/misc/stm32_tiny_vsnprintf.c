/******************************************************************************
 * @file    stm32_tiny_vsnprintf.c
 * @author  MCD Application Team
 * @brief   Tiny implementation of vsnprintf like function
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the 
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/*
 * Following implementation is adapted from original one
 *   https://github.com/jpbonn/coremark_lm32/blob/master/ee_printf.c
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32_tiny_vsnprintf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define TINY_PRINTF

#define ZEROPAD    (1<<0)  /* Pad with zero */
#define SIGN      (1<<1)  /* Unsigned/signed long */
#define UPPERCASE   (1<<6)  /* 'ABCDEF' */
#ifdef TINY_PRINTF
#else
#define PLUS      (1<<2)  /* Show plus */
#define HEX_PREP   (1<<5)  /* 0x */
#define SPACE     (1<<3)  /* Spacer */
#define LEFT      (1<<4)  /* Left justified */
#endif

#define is_digit(c) ((c) >= '0' && (c) <= '9')
   
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static char *lower_digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* Functions Definition ------------------------------------------------------*/
#ifdef TINY_PRINTF
#else
static size_t strnlen(const char *s, size_t count);

static size_t strnlen(const char *s, size_t count)
{
  const char *sc;
  for (sc = s; *sc != '\0' && count--; ++sc);
  return sc - s;
}
#endif

static int ee_skip_atoi(const char **s)
{
  int i = 0;
  while (is_digit(**s)) i = i*10 + *((*s)++) - '0';
  return i;
}

#define ASSIGN_STR(_c)  do { *str++ = (_c); max_size--; if (max_size == 0) return str; } while (0)

static char *ee_number(char *str, int max_size, long num, int base, int size, int precision, int type)
{
  char c;
  char sign, tmp[66];
  char *dig = lower_digits;
  int i;

  if (type & UPPERCASE)  dig = upper_digits;
#ifdef TINY_PRINTF
#else
  if (type & LEFT) type &= ~ZEROPAD;
#endif
  if (base < 2 || base > 36) return 0;
  
  c = (type & ZEROPAD) ? '0' : ' ';
  sign = 0;
  if (type & SIGN)
  {
    if (num < 0)
    {
      sign = '-';
      num = -num;
      size--;
    }
#ifdef TINY_PRINTF
#else
    else if (type & PLUS)
    {
      sign = '+';
      size--;
    }
    else if (type & SPACE)
    {
      sign = ' ';
      size--;
    }
#endif
  }

#ifdef TINY_PRINTF
#else
  if (type & HEX_PREP)
  {
    if (base == 16)
      size -= 2;
    else if (base == 8)
      size--;
  }
#endif
  
  i = 0;

  if (num == 0)
    tmp[i++] = '0';
  else
  {
    while (num != 0)
    {
      tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
      num = ((unsigned long) num) / (unsigned) base;
    }
  }

  if (i > precision) precision = i;
  size -= precision;
  if (!(type & (ZEROPAD /* TINY option   | LEFT */))) while (size-- > 0) ASSIGN_STR(' ');
  if (sign) ASSIGN_STR(sign);
  
#ifdef TINY_PRINTF
#else
  if (type & HEX_PREP)
  {
    if (base == 8)
      ASSIGN_STR('0');
    else if (base == 16)
    {
      ASSIGN_STR('0');
      ASSIGN_STR(lower_digits[33]);
    }
  }
#endif
  
#ifdef TINY_PRINTF
  while (size-- > 0) ASSIGN_STR(c);
#else
  if (!(type & LEFT)) while (size-- > 0) ASSIGN_STR(c);
#endif
  while (i < precision--) ASSIGN_STR('0');
  while (i-- > 0) ASSIGN_STR(tmp[i]);
  while (size-- > 0) ASSIGN_STR(' ');

  return str;
}

#ifdef TINY_PRINTF
#else
static char *eaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
  char tmp[24];
  char *dig = lower_digits;
  int i, len;

  if (type & UPPERCASE)  dig = upper_digits;
  len = 0;
  for (i = 0; i < 6; i++)
  {
    if (i != 0) tmp[len++] = ':';
    tmp[len++] = dig[addr[i] >> 4];
    tmp[len++] = dig[addr[i] & 0x0F];
  }

  if (!(type & LEFT)) while (len < size--) *str++ = ' ';
  for (i = 0; i < len; ++i) *str++ = tmp[i];
  while (len < size--) *str++ = ' ';

  return str;
}

static char *iaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
  char tmp[24];
  int i, n, len;

  len = 0;
  for (i = 0; i < 4; i++)
  {
    if (i != 0) tmp[len++] = '.';
    n = addr[i];
    
    if (n == 0)
      tmp[len++] = lower_digits[0];
    else
    {
      if (n >= 100) 
      {
        tmp[len++] = lower_digits[n / 100];
        n = n % 100;
        tmp[len++] = lower_digits[n / 10];
        n = n % 10;
      }
      else if (n >= 10) 
      {
        tmp[len++] = lower_digits[n / 10];
        n = n % 10;
      }

      tmp[len++] = lower_digits[n];
    }
  }

  if (!(type & LEFT)) while (len < size--) *str++ = ' ';
  for (i = 0; i < len; ++i) *str++ = tmp[i];
  while (len < size--) *str++ = ' ';

  return str;
}
#endif

#ifdef HAS_FLOAT

char *ecvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
char *fcvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
static void ee_bufcpy(char *d, char *s, int count); 
 
void ee_bufcpy(char *pd, char *ps, int count) {
  char *pe=ps+count;
  while (ps!=pe)
    *pd++=*ps++;
}

static void parse_float(double value, char *buffer, char fmt, int precision)
{
  int decpt, sign, exp, pos;
  char *fdigits = NULL;
  char cvtbuf[80];
  int capexp = 0;
  int magnitude;

  if (fmt == 'G' || fmt == 'E')
  {
    capexp = 1;
    fmt += 'a' - 'A';
  }

  if (fmt == 'g')
  {
    fdigits = ecvtbuf(value, precision, &decpt, &sign, cvtbuf);
    magnitude = decpt - 1;
    if (magnitude < -4  ||  magnitude > precision - 1)
    {
      fmt = 'e';
      precision -= 1;
    }
    else
    {
      fmt = 'f';
      precision -= decpt;
    }
  }

  if (fmt == 'e')
  {
    fdigits = ecvtbuf(value, precision + 1, &decpt, &sign, cvtbuf);

    if (sign) *buffer++ = '-';
    *buffer++ = *fdigits;
    if (precision > 0) *buffer++ = '.';
    ee_bufcpy(buffer, fdigits + 1, precision);
    buffer += precision;
    *buffer++ = capexp ? 'E' : 'e';

    if (decpt == 0)
    {
      if (value == 0.0)
        exp = 0;
      else
        exp = -1;
    }
    else
      exp = decpt - 1;

    if (exp < 0)
    {
      *buffer++ = '-';
      exp = -exp;
    }
    else
      *buffer++ = '+';

    buffer[2] = (exp % 10) + '0';
    exp = exp / 10;
    buffer[1] = (exp % 10) + '0';
    exp = exp / 10;
    buffer[0] = (exp % 10) + '0';
    buffer += 3;
  }
  else if (fmt == 'f')
  {
    fdigits = fcvtbuf(value, precision, &decpt, &sign, cvtbuf);
    if (sign) *buffer++ = '-';
    if (*fdigits)
    {
      if (decpt <= 0)
      {
        *buffer++ = '0';
        *buffer++ = '.';
        for (pos = 0; pos < -decpt; pos++) *buffer++ = '0';
        while (*fdigits) *buffer++ = *fdigits++;
      }
      else
      {
        pos = 0;
        while (*fdigits)
        {
          if (pos++ == decpt) *buffer++ = '.';
          *buffer++ = *fdigits++;
        }
      }
    }
    else
    {
      *buffer++ = '0';
      if (precision > 0)
      {
        *buffer++ = '.';
        for (pos = 0; pos < precision; pos++) *buffer++ = '0';
      }
    }
  }

  *buffer = '\0';
}

static void decimal_point(char *buffer)
{
  while (*buffer)
  {
    if (*buffer == '.') return;
    if (*buffer == 'e' || *buffer == 'E') break;
    buffer++;
  }

  if (*buffer)
  {
    int n = strnlen(buffer,256);
    while (n > 0) 
    {
      buffer[n + 1] = buffer[n];
      n--;
    }

    *buffer = '.';
  }
  else
  {
    *buffer++ = '.';
    *buffer = '\0';
  }
}

static void cropzeros(char *buffer)
{
  char *stop;

  while (*buffer && *buffer != '.') buffer++;
  if (*buffer++)
  {
    while (*buffer && *buffer != 'e' && *buffer != 'E') buffer++;
    stop = buffer--;
    while (*buffer == '0') buffer--;
    if (*buffer == '.') buffer--;
    while (buffer!=stop)
    *++buffer=0;
  }
}

static char *flt(char *str, double num, int size, int precision, char fmt, int flags)
{
  char tmp[80];
  char c, sign;
  int n, i;

  // Left align means no zero padding
#ifdef TINY_PRINTF
#else
  if (flags & LEFT) flags &= ~ZEROPAD;
#endif
  
  // Determine padding and sign char
  c = (flags & ZEROPAD) ? '0' : ' ';
  sign = 0;
  if (flags & SIGN)
  {
    if (num < 0.0)
    {
      sign = '-';
      num = -num;
      size--;
    }
#ifdef TINY_PRINTF
#else
    else if (flags & PLUS)
    {
      sign = '+';
      size--;
    }
    else if (flags & SPACE)
    {
      sign = ' ';
      size--;
    }
#endif
  }

  // Compute the precision value
  if (precision < 0)
    precision = 6; // Default precision: 6

  // Convert floating point number to text
  parse_float(num, tmp, fmt, precision);

#ifdef TINY_PRINTF
#else
  if ((flags & HEX_PREP) && precision == 0) decimal_point(tmp);
#endif
  if (fmt == 'g' && !(flags & HEX_PREP)) cropzeros(tmp);

  n = strnlen(tmp,256);

  // Output number with alignment and padding
  size -= n;
  if (!(flags & (ZEROPAD | LEFT))) while (size-- > 0) *str++ = ' ';
  if (sign) *str++ = sign;
  if (!(flags & LEFT)) while (size-- > 0) *str++ = c;
  for (i = 0; i < n; i++) *str++ = tmp[i];
  while (size-- > 0) *str++ = ' ';

  return str;
}

#endif

#define CHECK_STR_SIZE(_buf, _str, _size) \
  if ((((_str) - (_buf)) >= ((_size)-1))) { break; }

int tiny_vsnprintf_like(char *buf, const int size, const char *fmt, va_list args)
{
  unsigned long num;
  int base;
  char *str;
  int len;
  int i;
  char *s;
  
  int flags;            // Flags to number()

  int field_width;      // Width of output field
  int precision;        // Min. # of digits for integers; max number of chars for from string
  int qualifier;        // 'h', 'l', or 'L' for integer fields

  if (size <= 0)
  {
    return 0;
  }
  
  for (str = buf; *fmt || ((str - buf) >= size-1); fmt++)
  {
    CHECK_STR_SIZE(buf, str, size);
    
    if (*fmt != '%')
    {
      *str++ = *fmt;
      continue;
    }
                  
    // Process flags
    flags = 0;
#ifdef TINY_PRINTF
    /* Support %0, but not %-, %+, %space and %# */
    fmt++;
    if (*fmt == '0')
    {
      flags |= ZEROPAD;
    }
#else
repeat:
    fmt++; // This also skips first '%'
    switch (*fmt)
    {
      case '-': flags |= LEFT; goto repeat;
      case '+': flags |= PLUS; goto repeat;
      case ' ': flags |= SPACE; goto repeat;
      case '#': flags |= HEX_PREP; goto repeat;
      case '0': flags |= ZEROPAD; goto repeat;
    }
#endif
    
    // Get field width
    field_width = -1;
    if (is_digit(*fmt))
      field_width = ee_skip_atoi(&fmt);
#ifdef TINY_PRINTF
    /* Does not support %* */
#else
    else if (*fmt == '*')
    {
      fmt++;
      field_width = va_arg(args, int);
      if (field_width < 0)
      {
        field_width = -field_width;
        flags |= LEFT;
      }
    }
#endif
    
    // Get the precision
    precision = -1;
#ifdef TINY_PRINTF
    /* Does not support %. */
#else    
    if (*fmt == '.')
    {
      ++fmt;    
      if (is_digit(*fmt))
        precision = ee_skip_atoi(&fmt);
      else if (*fmt == '*')
      {
        ++fmt;
        precision = va_arg(args, int);
      }
      if (precision < 0) precision = 0;
    }
#endif
    
    // Get the conversion qualifier
    qualifier = -1;
#ifdef TINY_PRINTF
      /* Does not support %l and %L */
#else
    if (*fmt == 'l' || *fmt == 'L')
    {
      qualifier = *fmt;
      fmt++;
    }
#endif
    
    // Default base
    base = 10;

    switch (*fmt)
    {
      case 'c':
#ifdef TINY_PRINTF
#else
        if (!(flags & LEFT))
#endif
          while (--field_width > 0) *str++ = ' ';
        *str++ = (unsigned char) va_arg(args, int);
#ifdef TINY_PRINTF
#else
        while (--field_width > 0) *str++ = ' ';
#endif
        continue;

      case 's':
        s = va_arg(args, char *);
        if (!s) s = "<NULL>";
#ifdef TINY_PRINTF
        len = strlen(s);
#else
        len = strnlen(s, precision);
        if (!(flags & LEFT))
#endif
          while (len < field_width--) *str++ = ' ';
        for (i = 0; i < len; ++i) *str++ = *s++;
#ifdef TINY_PRINTF
#else        
        while (len < field_width--) *str++ = ' ';
#endif
        continue;

#ifdef TINY_PRINTF
      /* Does not support %p, %A, %a, %o */
#else
      case 'p':
        if (field_width == -1)
        {
          field_width = 2 * sizeof(void *);
          flags |= ZEROPAD;
        }
        str = ee_number(str, (size - (str - buf)), (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
        continue;

      case 'A':
        flags |= UPPERCASE;

      case 'a':
        if (qualifier == 'l')
          str = eaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
        else
          str = iaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
        continue;

      // Integer number formats - set up the flags and "break"
      case 'o':
        base = 8;
        break;
#endif
      
      case 'X':
        flags |= UPPERCASE;

      case 'x':
        base = 16;
        break;

      case 'd':
      case 'i':
        flags |= SIGN;

      case 'u':
        break;

#ifdef HAS_FLOAT

      case 'f':
        str = flt(str, va_arg(args, double), field_width, precision, *fmt, flags | SIGN);
        continue;

#endif

      default:
        if (*fmt != '%') *str++ = '%';
        CHECK_STR_SIZE(buf, str, size);
        if (*fmt)
          *str++ = *fmt;
        else
          --fmt;
        CHECK_STR_SIZE(buf, str, size);
        continue;
    }

    if (qualifier == 'l')
      num = va_arg(args, unsigned long);
    else if (flags & SIGN)
      num = va_arg(args, int);
    else
      num = va_arg(args, unsigned int);

    str = ee_number(str, ((size - 1) - (str - buf)), num, base, field_width, precision, flags);
  }

  *str = '\0';
  return str - buf;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
