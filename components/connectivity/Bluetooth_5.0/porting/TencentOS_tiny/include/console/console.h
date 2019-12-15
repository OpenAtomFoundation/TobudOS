#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define console_init(...)       -1

int console_printf(const char *fmt, ...);

int console_read(char *str, int cnt, int *newline);

void console_write(const char *str, int cnt);

#endif

