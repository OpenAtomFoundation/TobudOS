#ifndef __HAL_SLEEP_H__
#define __HAL_SLEEP_H__

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "hal/hal_gpio.h"
#include "ll/ll_sleep.h"

typedef enum {
    SLEEP_TIMER_WAKEUP = (1 << 0),
    MAC_WAKEUP = (1 << 1),
    EXT_INT_WAKEUP = (1 << 2),
    RTC_WAKEUP = (1 << 3),
}hal_sleep_wakeup_src_enum_t;

typedef void (* reinitialize_phy_cb_t)(void);
typedef bool (* wifi_is_slept_cb_t)(void);
typedef struct
{
    GPIO_Num                    gpio;
    SYSTEM_EXT_INT_Triggle_Type triggle_type;
}ext_irq_cfg_t ;
typedef struct {
    sleep_mode_enum_t       sleep_mode;
    uint32_t                wakeup_src;//bit[0]--SLEEP_IRQn; bit[1]--MAC_IRQn;bit[2]--EXTERNAL_IRQn;bit[3]--RTC_IRQn
    ext_irq_cfg_t           ext_irq_cfg;
}hal_sleep_config_t;

typedef struct
{
    bool                    flag;
    hal_sleep_config_t      sleep_config;
    uint32_t                nvic_int_en[2];
    uint64_t                compensation;
    uint32_t                msec;
    wifi_is_slept_cb_t      wifi_is_slept_cb;
    reinitialize_phy_cb_t   reinit_phy_cb;
    bool                    inited;
}hal_sleep_ctrl_t ;

void hal_sleep_init(hal_sleep_config_t *sleep_config);
void hal_sleep_deinit(void);
void hal_sleep_set_config(hal_sleep_config_t *sleep_config);
sleep_mode_enum_t hal_sleep_get_mode(void);
void hal_pre_sleep_processing(uint32_t *ticks);
uint32_t hal_post_sleep_processing(uint32_t ticks);
void hal_sleep_register_callback(reinitialize_phy_cb_t reinit_phy_cb, wifi_is_slept_cb_t wifi_is_slept_cb);
void hal_sleep_wakeup_source_set(hal_sleep_wakeup_src_enum_t wakeup_src);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HAL_SLEEP_H__

