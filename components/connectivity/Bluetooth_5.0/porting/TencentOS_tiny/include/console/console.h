#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#if 0

#include "tos.h"

#define console_printf      tos_kprintf

// we need a console
#define console_init(...)       -1
#define console_write(...)  printf(__VA_ARGS__);
#define console_read(...)       -1
#endif

#define console_init(...)       -1

int console_printf(const char *fmt, ...);

int console_read(char *str, int cnt, int *newline);

void console_write(const char *str, int cnt);

#endif

