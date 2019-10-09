#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "tos.h"

#define console_printf      tos_kprintf

// we need a console
#define console_init(...)       -1
#define console_write(...)
#define console_read(...)       -1

#endif

