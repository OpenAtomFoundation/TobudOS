#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "tos_k.h"
#include "timing_alt.h"

unsigned long mbedtls_timing_get_timer(struct mbedtls_timing_hr_time *val, int reset)
{
    struct mbedtls_timing_hr_time now;

    now.timer_ms = tos_systick_get();

    if (reset) {
        val->timer_ms = now.timer_ms;
    }

    return (unsigned long)(now.timer_ms - val->timer_ms);
}

/*
 * Set delays to watch
 */
void mbedtls_timing_set_delay(void *data, uint32_t int_ms, uint32_t fin_ms)
{
    mbedtls_timing_delay_context *ctx;

    if (!data) {
        return;
    }

    ctx = (mbedtls_timing_delay_context*)data;

    ctx->int_ms = int_ms;
    ctx->fin_ms = fin_ms;

    if (fin_ms != 0) {
        (void)mbedtls_timing_get_timer(&ctx->timer, 1);
    }
}

/*
 * Get number of delays expired
 */
int mbedtls_timing_get_delay(void *data)
{
    unsigned long elapsed_ms;
    mbedtls_timing_delay_context *ctx;

    if (!data) {
        return -1;
    }

    ctx = (mbedtls_timing_delay_context*)data;

    if (ctx->fin_ms == 0) {
        return -1;
    }

    elapsed_ms = mbedtls_timing_get_timer(&ctx->timer, 0);

    if (elapsed_ms >= ctx->fin_ms) {
        return 2;
    }

    if (elapsed_ms >= ctx->int_ms) {
        return 1;
    }

    return 0;
}

