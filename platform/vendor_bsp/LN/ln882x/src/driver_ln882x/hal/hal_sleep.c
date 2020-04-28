
#include <string.h>
#include "types.h"
#include "ln88xx.h"
#include "hal/hal_syscon.h"
#include "hal/hal_rtc.h"
#include "hal/hal_gpio.h"
#include "hal/cache.h"
#include "hal/flash.h"
#include "hal/hal_sleep.h"
#include "utils/debug/log.h"
#include "FreeRTOS.h"

#define SLEEP_TIMER_EXTRA_COMPENSATION_MSEC                             (0)
#define RTC_TIMER_EXTRA_COMPENSATION_MSEC                               (0)

hal_sleep_ctrl_t g_sleep_ctrl __attribute__((section("retention_data"))) = {0};

static hal_sleep_ctrl_t *hal_sleep_get_handle(void)
{
    return &g_sleep_ctrl;
}

void EXTERNAL_IRQHandler(void)
{
    HAL_SYSCON_EXT_INTR_Clear();
    hal_sleep_wakeup_source_set(EXT_INT_WAKEUP);
}
static void hal_sleep_external_irq_wakeup_init(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, SYSTEM_EXT_INT_Triggle_Type triggle)
{
    HAL_SYSCON_EXT_INTR_Set_Triggle_Condition(ext_int_idx, triggle);
    HAL_SYSCON_EXT_INTR_Enable(ext_int_idx, true);
    NVIC_EnableIRQ(EXTERNAL_IRQn);
}
void hal_sleep_rtc_wakeup_init(uint32_t counter_match)
{
    RTC_InitTypeDef rtcConfig;

    rtcConfig.counter_load = 0;
    rtcConfig.counter_match = counter_match;
    rtcConfig.int_en = RTC_INT_ENABLE;
    rtcConfig.int_mask = RTC_INT_UNMASK;
    rtcConfig.wrap_en = RTC_WRAP_DISABLE;//RTC_WRAP_ENABLE
    HAL_RTC_Init(rtcConfig);
    HAL_RTC_Enable(RTC_ENABLE);
    NVIC_EnableIRQ(RTC_IRQn);
}
void hal_sleep_rtc_wakeup_deinit(void)
{
    HAL_RTC_Enable(RTC_DISABLE);
    NVIC_DisableIRQ(RTC_IRQn);
}
void RTC_IRQHandler(void)
{
    RTC_IntStatus status = HAL_RTC_IntStatus();

    if (status) {
        // clear int
        HAL_RTC_ClearInt();

        //disable RTC
        hal_sleep_rtc_wakeup_deinit();

        //set wakeup src
        hal_sleep_wakeup_source_set(RTC_WAKEUP);
    }
}

void hal_sleep_init(hal_sleep_config_t *sleep_config)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();

    if(sleep_ctrl->inited){
        LOG(LOG_LVL_EMERG, "Function hal_sleep_init(*sleep_config) has been called, If you want to reconfigure, please call hal_sleep_deinit() and re-call hal_sleep_init(*sleep_config)\r\n");
        return;
    }
    memset(sleep_ctrl, 0, sizeof(hal_sleep_ctrl_t));
    sleep_ctrl->inited = true;
    memcpy(&(sleep_ctrl->sleep_config), sleep_config, sizeof(hal_sleep_config_t));
}
void hal_sleep_deinit(void)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    memset(sleep_ctrl, 0, sizeof(hal_sleep_ctrl_t));
    return;
}
static void hal_sleep_wakeup_source_clear_all(void)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);
    sleep_config->wakeup_src = 0;
}
static void hal_sleep_wakeup_source_update(void)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);
    sleep_mode_enum_t sleep_mode = sleep_config->sleep_mode;

    //clear wakeup source
    hal_sleep_wakeup_source_clear_all();
    if(sleep_mode >= LIGHT_SLEEP)
    {
        //保存中断使能寄存器的值
        sleep_ctrl->nvic_int_en[0] = NVIC->ISER[0];
        sleep_ctrl->nvic_int_en[1] = NVIC->ISER[1];

        //清除中断使能寄存器
        //disable all interrupts
        NVIC->ICER[0] = 0xFFFFFFFF;
        NVIC->ICER[1] = 0xFFFFFFFF;
    }

    //enable wakeup IRQ
    if(sleep_mode >= LIGHT_SLEEP){
        if(sleep_mode == LIGHT_SLEEP){
            NVIC_EnableIRQ(UART0_IRQn);
            NVIC_EnableIRQ(UART1_IRQn);
        }else{
            NVIC_EnableIRQ(SLEEP_IRQn);
        }

    #if WIFI_SWITCH==0
        if(sleep_mode <= DEEP_SLEEP){
            NVIC_EnableIRQ(MAC_IRQn);
        }
    #endif

        if(sleep_config->wakeup_src & EXT_INT_WAKEUP){
            //ext int wakeup init
            hal_sleep_external_irq_wakeup_init(HAL_GPIO_Mapping_To_Ext_Int(sleep_config->ext_irq_cfg.gpio), sleep_config->ext_irq_cfg.triggle_type);
        }
    }
}

void hal_sleep_set_config(hal_sleep_config_t *sleep_config)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();

    if(!sleep_ctrl->inited){
        LOG(LOG_LVL_EMERG, "Function hal_sleep_init(*sleep_config) has not been called before, so you can't set sleep_config!\r\n");
        return;
    }
    memcpy(&(sleep_ctrl->sleep_config), sleep_config, sizeof(hal_sleep_config_t));
}

sleep_mode_enum_t hal_sleep_get_mode(void)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    return sleep_ctrl->sleep_config.sleep_mode;
}
extern bool is_sta_sleeping(void);
extern void initialize_phy(void);
#define RAM_RETENTION           (1 << 0)
#define RAM_MAC                 (1 << 1)
#define RAM_CPU                 (1 << 2)
#if WIFI_SWITCH
extern uint8_t wifi_en;
#endif
void hal_pre_sleep_processing(uint32_t *ticks)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    sleep_mode_enum_t sleep_mode = hal_sleep_get_mode();
    uint64_t u64val = (uint64_t)(*ticks) * 1000 / configTICK_RATE_HZ;
    uint8_t sram_powerdown = 0x00;//RAM_RETENTION | RAM_MAC | RAM_CPU

#if WIFI_SWITCH
    if(sleep_mode < LIGHT_SLEEP || sleep_ctrl->flag || (wifi_en==1)){
        *ticks = 0;
        return;
    }
    //LOG(LOG_LVL_INFO, "\r\n%s() line:%d\r\n",__func__,__LINE__);
#else
    if(sleep_mode < LIGHT_SLEEP || sleep_ctrl->flag || (sleep_ctrl->wifi_is_slept_cb && !sleep_ctrl->wifi_is_slept_cb())){
        *ticks = 0;
        return;
    }
#endif

    if(u64val >> 32){
        LOG(LOG_LVL_EMERG, "[%s, %d]ticks(%d) expected overflow!\r\n", __func__, __LINE__, *ticks);
        u64val = u64val & 0xFFFFFFFFULL;
    }
    sleep_ctrl->msec = u64val;

    //Stop SysTick
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    sleep_ctrl->flag = true;

    //set wakeup source
    hal_sleep_wakeup_source_update();
    if(sleep_mode == LIGHT_SLEEP){
        // LIGHT_SLEEP睡眠模式时，使用RTC唤醒
        u64val = ((uint64_t)1000000 * (sleep_ctrl->msec - RTC_TIMER_EXTRA_COMPENSATION_MSEC))/HAL_SYSCON_Get32KPeriodNs();
        if(u64val >> 32){
            LOG(LOG_LVL_EMERG, "[%s, %d]RTC counter_match(%lld) overflow!\r\n", __func__, __LINE__, u64val);
            u64val = u64val & 0xFFFFFFFFULL;
        }
        hal_sleep_rtc_wakeup_init(u64val);
    }else{
        // 其他睡眠模式时，使用专门的sleep timer唤醒
        sleep_ctrl->compensation = HAL_SYSCON_CalculateCompensateNs() + (uint64_t)SLEEP_TIMER_EXTRA_COMPENSATION_MSEC*1000000;
        HAL_SYSCON_CPUSleepDurationEnable(((uint64_t)*ticks * 1000000000 / configTICK_RATE_HZ) - sleep_ctrl->compensation);
    }
    ll_pre_sleep_processing(sleep_mode, sram_powerdown);
}
uint32_t hal_post_sleep_processing(uint32_t ticks)
{
    uint64_t sleep_ns = 0, real0=0;

    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    sleep_mode_enum_t mode = hal_sleep_get_mode();

    if(mode < LIGHT_SLEEP || !sleep_ctrl->flag){
        return 0;
    }

    if(mode >= RETENTION_SLEEP){
        FLASH_Init();
        //Enable QSPI 4bit mode
        FLASH_QuadModeEnable(1);
        //Init Flash cache
        flash_cache_init(0);
        log_init();
    }

    sleep_ctrl->flag = false;
    ll_post_sleep_processing(mode);

    if(mode == LIGHT_SLEEP){
        ticks = (((uint64_t)HAL_RTC_GetCurValue() * HAL_SYSCON_Get32KPeriodNs() / 1000000 + RTC_TIMER_EXTRA_COMPENSATION_MSEC) * configTICK_RATE_HZ) / 1000;
    }else{
        real0 = HAL_SYSCON_RealSleepTime();
        sleep_ns = real0 + sleep_ctrl->compensation;
        ticks = (sleep_ns * configTICK_RATE_HZ) / 1000000000;
    }
    if(sleep_ctrl->msec < ticks){
        ticks = sleep_ctrl->msec;
    }

    if(mode >= LIGHT_SLEEP)
    {        
        NVIC->ISER[0] = sleep_ctrl->nvic_int_en[0];
        NVIC->ISER[1] = sleep_ctrl->nvic_int_en[1];
    }
    if(sleep_ctrl->reinit_phy_cb)
    {
        sleep_ctrl->reinit_phy_cb();
    }
    return ticks;
}
void hal_sleep_wakeup_source_set(hal_sleep_wakeup_src_enum_t wakeup_src)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);
    sleep_config->wakeup_src |= wakeup_src;
}
void hal_sleep_register_callback(reinitialize_phy_cb_t reinit_phy_cb, wifi_is_slept_cb_t wifi_is_slept_cb)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    sleep_ctrl->reinit_phy_cb = reinit_phy_cb;
    sleep_ctrl->wifi_is_slept_cb = wifi_is_slept_cb;
}

void SLEEP_IRQHandler(void)
{
    //do nothing
    hal_sleep_wakeup_source_set(SLEEP_TIMER_WAKEUP);
}

