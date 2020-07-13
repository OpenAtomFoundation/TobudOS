#ifndef __HAL_SLEEP_H__
#define __HAL_SLEEP_H__

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "hal/syscon_types.h"
#include "ll/ll_sleep.h"

typedef enum
{
    ACTIVE = 0,
    LIGHT_SLEEP,
    DEEP_SLEEP,
    RETENTION_SLEEP,
    FROZEN_SLEEP
}sleep_mode_enum_t;

typedef enum {
    MOD_QSPI = SW_CLKGATE_QSPI,
    MOD_ADDC = SW_CLKGATE_ADCC,
    MOD_I2S = SW_CLKGATE_I2S,
    MOD_GPIO = SW_CLKGATE_GPIO,
    MOD_SPIM = SW_CLKGATE_SPIM,
    MOD_SPIS = SW_CLKGATE_SPIS,
    MOD_I2C0 = SW_CLKGATE_I2C0,
    MOD_I2C1 = SW_CLKGATE_I2C1,
    MOD_UART0 = SW_CLKGATE_UART0,
    MOD_UART1 = SW_CLKGATE_UART1,
    MOD_SPIM2 = SW_CLKGATE_SPIM2,
    MOD_WDT = SW_CLKGATE_WDT,
    MOD_TIMER0 = SW_CLKGATE_TIMER,
    MOD_TIMER1 = SW_CLKGATE_TIMER_1,
    MOD_TIMER2 = SW_CLKGATE_TIMER_2,
    MOD_TIMER3 = SW_CLKGATE_TIMER_3,
    MOD_TIMER4 = SW_CLKGATE_TIMER_4,
    MOD_DGBH = SW_CLKGATE_DBGH,
    MOD_SDIO = SW_CLKGATE_SDIO,
    MOD_MAC = SW_CLKGATE_MAC,
    MOD_CACHE = SW_CLKGATE_CACHE,
    MOD_DMA = SW_CLKGATE_DMA,
    MOD_RFREG = SW_CLKGATE_RFREG,
    MOD_PWM = SW_CLKGATE_PWM,
    MOD_EF = SW_CLKGATE_EF,
    MOD_TRNG = SW_CLKGATE_TRNG,
    MOD_AES = SW_CLKGATE_AES,
    MOD_EXT_INT,
    MOD_MAX,
}hal_peripheral_module_t;

typedef bool (*sleep_condition_cb_t)(void);
typedef void (*sleep_processing_cb_t)(void);

int hal_sleep_set_mode(sleep_mode_enum_t sleep_mode);
sleep_mode_enum_t hal_sleep_get_mode(void);
int hal_sleep_register(hal_peripheral_module_t peripheral_module, sleep_condition_cb_t sleep_condition, sleep_processing_cb_t pre_sleep_proc, sleep_processing_cb_t post_sleep_proc);
int hal_sleep_unregister(hal_peripheral_module_t peripheral_module);
void hal_pre_sleep_processing(uint32_t *ticks);
uint32_t hal_post_sleep_processing(uint32_t ticks);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HAL_SLEEP_H__

