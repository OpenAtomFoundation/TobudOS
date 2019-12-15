#include "console/console.h"

#include "stdint.h"
#include "stdarg.h"

int console_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

int console_read(char *str, int cnt, int *newline)
{
    return 0;
}

char convert_int2ascii(int8_t n)
{
    if (n < 10 && n >= 0) {
        return n - 0 + '0';
    } else if (n <= 15 && n >= 10) {
        return n - 10 + 'A';
    } else {
        return '$';
    }
}

void console_write(const char *str, int cnt)
{
    int i;

    if (cnt == 1 && str[0] == 10) {
        printf("\n");
        return;
    }

    printf("0x");
    for (i = 0; i < cnt; ++i) {
        printf("%c%c", convert_int2ascii(str[i] / 16), convert_int2ascii(str[i] % 16));
        if (i != cnt - 1) {
            printf("-");
        }
    }
}

