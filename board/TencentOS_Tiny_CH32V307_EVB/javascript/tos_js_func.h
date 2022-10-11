#pragma once

#include "tos_js.h"
jsval_t js_print(struct js *js, jsval_t *args, int nargs);
jsval_t js_sleep(struct js *js, jsval_t *args, int nargs);
jsval_t js_fib(struct js *js, jsval_t *args, int nargs);
jsval_t js_LCD_ShowString(struct js *js, jsval_t *args, int nargs);
