#include "tos_js_func.h"
#include "stdio.h"
#include "tos_k.h"

static jsval_t tov(double d) { union { double d; jsval_t v; } u = {d}; return u.v; }
static double tod(jsval_t v) { union { jsval_t v; double d; } u = {v}; return u.d; }
static jsval_t mkval(uint8_t type, uint64_t data) { return ((jsval_t) 0x7ff0U << 48U) | ((jsval_t) (type) << 48) | (data & 0xffffffffffffUL); }
jsval_t js_print(struct js *js, jsval_t *args, int nargs) {
    for (int i = 0; i < nargs; i++) {
        const char *space = i == 0 ? "" : " ";
        printf("%s%s", space, js_str(js, args[i]));
    }
    putchar('\n');
    return js_mkundef();
}
jsval_t js_sleep(struct js *js, jsval_t *args, int nargs) {
    tos_sleep_ms(tod(args[0]));
    return js_mkundef();
}
jsval_t js_fib(struct js *js, jsval_t *args, int nargs) {
    if (nargs != 1) return js_mkundef();
    double x = tod(args[0]);
    if (abs(x - (int64_t)x) > 0.001) return js_mkundef();
    int64_t ix = (int64_t)x;
    int64_t a = 1, b = 1, tmp;
    if (ix <= 1) return tov(1);
    for (int i = 1; i < ix; ++i) {
        tmp = a + b;
        a = b;
        b = tmp;
    }
    return tov(b);
}
