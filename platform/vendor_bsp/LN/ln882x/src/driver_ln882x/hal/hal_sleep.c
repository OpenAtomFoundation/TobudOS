
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

typedef struct
{
    int                   irq_num;
    sleep_condition_cb_t  sleep_condition;
    sleep_processing_cb_t pre_sleep;
    sleep_processing_cb_t post_sleep;
}hal_sleep_action_t ;

typedef struct {
    sleep_mode_enum_t       sleep_mode;
    uint32_t                peripheral_mod;
    hal_sleep_action_t      actions[29];
}hal_sleep_config_t;

typedef struct
{
    bool                    flag;
    hal_sleep_config_t      sleep_config;
    uint32_t                nvic_int_en[2];
    uint64_t                compensation;
    uint32_t                msec;
}hal_sleep_ctrl_t ;

hal_sleep_ctrl_t g_sleep_ctrl __attribute__((section("retention_data")));

static hal_sleep_ctrl_t *hal_sleep_get_handle(void)
{
    return &g_sleep_ctrl;
}

void EXTERNAL_IRQHandler(void)
{
    HAL_SYSCON_EXT_INTR_Clear();
}

static int hal_sleep_rtc_init(uint32_t counter_match)
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
    return 0;
}
static int hal_sleep_rtc_deinit(void)
{
    HAL_RTC_Enable(RTC_DISABLE);
    NVIC_DisableIRQ(RTC_IRQn);
    return 0;
}
void RTC_IRQHandler(void)
{
    RTC_IntStatus status = HAL_RTC_IntStatus();

    if (status) {
        // clear int
        HAL_RTC_ClearInt();

        //disable RTC
        hal_sleep_rtc_deinit();
    }
}
void SLEEP_IRQHandler(void)
{
    NVIC_DisableIRQ(SLEEP_IRQn);
    //do nothing
}

#if 0
#define RAM_RETENTION           (1 << 0)
#define RAM_MAC                 (1 << 1)
#define RAM_CPU                 (1 << 2)
#endif

#define PRE_SLEEP_LOOP_COUNT            10
static void pre_sleep_processing_hardware(sleep_mode_enum_t sleep_mode, uint8_t sram_powerdown)
{
    int i = 0;
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);

    if(sleep_mode < LIGHT_SLEEP || sleep_mode > FROZEN_SLEEP){
        return;
    }
    
    if(sleep_mode >= RETENTION_SLEEP){
        HAL_SYSCON_AWO_sram_pd_en(sram_powerdown);
        HAL_SYSCON_AWO_sram_po_en(0x07);
        HAL_SYSCON_AWO_pmu_ret_en(1);
        HAL_SYSCON_AWO_pmu_unret_en(1);
        HAL_SYSCON_AWO_pdcmp_po(0);
        if(sleep_mode == FROZEN_SLEEP){
            HAL_SYSCON_AWO_cpucore_retreg_po(0);//set 0 to enter frozon mode
            HAL_SYSCON_AWO_idle_reg_set(0x52);
        }else{
            HAL_SYSCON_AWO_cpucore_retreg_po(1);
            HAL_SYSCON_AWO_lp_mode_awo(1);
        }
    }else{//sleep_mode <= DEEP_SLEEP
        HAL_SYSCON_AWO_sram_pd_en(sram_powerdown);
        HAL_SYSCON_AWO_pmu_ret_en(0);
        HAL_SYSCON_AWO_pmu_unret_en(0);
        
        // if phy needs power down, then save phy reg to cmp
        HAL_SYSCON_CMP_rfreg_len(0x158);
        HAL_SYSCON_CMP_rfreg_base_addr(0x0);
        HAL_SYSCON_CMP_rfreg_action_set(0x1);//bit 0 is save rf register
        while(HAL_SYSCON_CMP_save_done_get() != 1);
    }

    if((sleep_mode >= DEEP_SLEEP) && (sleep_mode <= RETENTION_SLEEP)){
        ll_sleep_mac_tsf_clk_switch(1);//switch to 32k and power saving
    }

    //set CM4 sleep mode
    if(sleep_mode >= DEEP_SLEEP){
        SCB->SCR |= (1<<2);
    }else{
        SCB->SCR &= ~(1<<2);
    }
    if(sleep_mode == LIGHT_SLEEP){
        HAL_SYSCON_AWO_sw_pwr_ctrl_set(0x02);
        while(i++ < PRE_SLEEP_LOOP_COUNT){
            __NOP();
        }
        while(HAL_SYSCON_AWO_pmu_fsm_get() != 0);
        HAL_SYSCON_CMP_sw_clkg_set(sleep_config->peripheral_mod|(1<<MOD_QSPI|1<<MOD_CACHE|1<<MOD_RFREG));
    }
}
static void post_sleep_processing_hardware(sleep_mode_enum_t sleep_mode)
{
    if(sleep_mode < LIGHT_SLEEP || sleep_mode > FROZEN_SLEEP){
        return;
    }

    if(sleep_mode <= DEEP_SLEEP){
        if(sleep_mode == LIGHT_SLEEP){
            HAL_SYSCON_AWO_sw_pwr_ctrl_set(0x01);
            HAL_SYSCON_CMP_sw_clkg_set(0xFFFFFFFF);
            while(HAL_SYSCON_AWO_pmu_fsm_get() != 0);
        }
        HAL_SYSCON_AWO_pmu_ret_en(0);
        HAL_SYSCON_AWO_pmu_unret_en(0);
       //restore RF register firstly
        HAL_SYSCON_CMP_rfreg_len(0x158);
        HAL_SYSCON_CMP_rfreg_base_addr(0x0);
        HAL_SYSCON_CMP_rfreg_action_set(0x2);//bit 1 is restore rf register
        while(HAL_SYSCON_CMP_restore_done_get() != 1);
    }

    if((sleep_mode >= DEEP_SLEEP) && (sleep_mode <= RETENTION_SLEEP)){
        ll_sleep_mac_tsf_clk_switch(0);//switch to 40M
    }
}
static int hal_sleep_peripheral_module_to_IRQn(hal_peripheral_module_t peripheral_module)
{
    int ret = 0;

    switch(peripheral_module){
        case MOD_QSPI:
            ret = QSPI_IRQn;
            break;
        case MOD_ADDC:
            ret = ADC_IRQn;
            break;
        case MOD_I2S:
            ret = I2S_IRQn;
            break;
        case MOD_GPIO:
            ret = GPIO_IRQn;
            break;
        case MOD_SPIM:
            ret = SPI0_IRQn;
            break;
        case MOD_SPIS:
            ret = SPI2_IRQn;
            break;
        case MOD_I2C0:
            ret = I2C0_IRQn;
            break;
        case MOD_I2C1:
            ret = I2C1_IRQn;
            break;
        case MOD_UART0:
            ret = UART0_IRQn;
            break;
        case MOD_UART1:
            ret = UART1_IRQn;
            break;
        case MOD_SPIM2:
            ret = SPI1_IRQn;
            break;
        case MOD_WDT:
            ret = WDT_IRQn;
            break;
        case MOD_TIMER0:
            ret = TIMER_IRQn;
            break;
        case MOD_TIMER1:
            ret = TIMER_IRQn;
            break;
        case MOD_TIMER2:
            ret = TIMER_IRQn;
            break;
        case MOD_TIMER3:
            ret = TIMER_IRQn;
            break;
        case MOD_TIMER4:
            ret = TIMER_IRQn;
            break;
        case MOD_SDIO:
            ret = SDIO_FUN1_IRQn;
            break;
        case MOD_MAC:
            ret = MAC_IRQn;
            break;
        case MOD_DMA:
            ret = DMA_IRQn;
            break;
        case MOD_PWM:
            ret = PWM_IRQn;
            break;
        case MOD_TRNG:
            ret = TRNG_IRQn;
            break;
        case MOD_AES:
            ret = AES_IRQn;
            break;
        case MOD_EXT_INT:
            ret = EXTERNAL_IRQn;
            break;
        case MOD_DGBH:
        case MOD_CACHE:
        case MOD_RFREG:
        case MOD_EF:
        default:
            break;
    }
    return ret;
}

static int log2(uint32_t val)
{
    int ret = 0;
    switch(val)
    {
        case 0x1: ret = 0; break;
        case 0x2: ret = 1; break;
        case 0x4: ret = 2; break;
        case 0x8: ret = 3; break;
        case 0x10: ret = 4; break;
        case 0x20: ret = 5; break;
        case 0x40: ret = 6; break;
        case 0x80: ret = 7; break;
        case 0x100: ret = 8; break;
        case 0x200: ret = 9; break;
        case 0x400: ret = 10; break;
        case 0x800: ret = 11; break;
        case 0x1000: ret = 12; break;
        case 0x2000: ret = 13; break;
        case 0x4000: ret = 14; break;
        case 0x8000: ret = 15; break;
        case 0x10000: ret = 16; break;
        case 0x20000: ret = 17; break;
        case 0x40000: ret = 18; break;
        case 0x80000: ret = 19; break;
        case 0x100000: ret = 20; break;
        case 0x200000: ret = 21; break;
        case 0x400000: ret = 22; break;
        case 0x800000: ret = 23; break;
        case 0x1000000: ret = 24; break;
        case 0x2000000: ret = 25; break;
        case 0x4000000: ret = 26; break;
        case 0x8000000: ret = 27; break;
        case 0x10000000: ret = 28; break;
        case 0x20000000: ret = 29; break;
        case 0x40000000: ret = 30; break;
        case 0x80000000: ret = 31; break;
        default: break;
    }
    return ret;
}

/**
 * @brief  Check whether can sleep
 *
 * @param  none
 *
 * @note   If can sleep, return true; else return flase. 
 */
static bool hal_sleep_condition_check(hal_sleep_config_t *sleep_config)
{
    bool ret = true;
    hal_sleep_action_t *action;
    uint32_t value, tmp, idx;

    if(!sleep_config){
        return false;
    }
    value = sleep_config->peripheral_mod;
    while(value){
        tmp = value & (value - 1);
        idx = log2(value - tmp);
        action = &(sleep_config->actions[idx]);
        if(action->sleep_condition){
            ret = ret && action->sleep_condition();
        }
        value = tmp;
    }
    return ret;
}

static void hal_pre_sleep_processing_registered_cb(hal_sleep_config_t *sleep_config)
{
    hal_sleep_action_t *action;
    uint32_t value, tmp, idx;

    if(!sleep_config){
        return;
    }
    value = sleep_config->peripheral_mod;
    while(value){
        tmp = value & (value - 1);
        idx = log2(value - tmp);
        action = &(sleep_config->actions[idx]);
        NVIC_EnableIRQ((IRQn_Type)action->irq_num);
        if(action->pre_sleep){
            action->pre_sleep();
        }
        value = tmp;
    }
}
static void hal_post_sleep_processing_registered_cb(hal_sleep_config_t *sleep_config)
{
    hal_sleep_action_t *action;
    uint32_t value, tmp, idx;

    if(!sleep_config){
        return;
    }
    value = sleep_config->peripheral_mod;
    while(value){
        tmp = value & (value - 1);
        idx = log2(value - tmp);
        action = &(sleep_config->actions[idx]);
        if(action->post_sleep){
            action->post_sleep();
        }
        value = tmp;
    }
}

void hal_pre_sleep_processing(uint32_t *ticks)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    sleep_mode_enum_t sleep_mode = hal_sleep_get_mode();
    uint64_t u64val = (uint64_t)(*ticks) * 1000 / configTICK_RATE_HZ;
    uint8_t sram_powerdown = 0x00;//RAM_RETENTION | RAM_MAC | RAM_CPU
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);

    if(sleep_mode < LIGHT_SLEEP || sleep_ctrl->flag){
        *ticks = 0;
        return;
    }

    if(!hal_sleep_condition_check(sleep_config)){
        *ticks = 0;
        return;
    }

    if(u64val >> 32){
        LOG(LOG_LVL_EMERG, "[%s, %d]ticks(%d) expected overflow!\r\n", __func__, __LINE__, *ticks);
        u64val = u64val & 0xFFFFFFFFULL;
    }
    sleep_ctrl->msec = u64val;

    //Stop SysTick
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    sleep_ctrl->flag = true;

    hal_pre_sleep_processing_registered_cb(sleep_config);
    if(sleep_mode == LIGHT_SLEEP){
        // LIGHT_SLEEP睡眠模式时，使用RTC唤醒
        u64val = ((uint64_t)1000000 * (sleep_ctrl->msec - RTC_TIMER_EXTRA_COMPENSATION_MSEC))/HAL_SYSCON_Get32KPeriodNs();
        hal_sleep_rtc_init((uint32_t)u64val);
    }else{
        // 其他睡眠模式时，使用专门的sleep timer唤醒
        sleep_ctrl->compensation = HAL_SYSCON_CalculateCompensateNs() + (uint64_t)SLEEP_TIMER_EXTRA_COMPENSATION_MSEC*1000000;
        HAL_SYSCON_CPUSleepDurationEnable(((uint64_t)*ticks * 1000000000 / configTICK_RATE_HZ) - sleep_ctrl->compensation);
        NVIC_EnableIRQ(SLEEP_IRQn);
    }
    //ll_pre_sleep_processing(sleep_mode, sram_powerdown);
    pre_sleep_processing_hardware(sleep_mode, sram_powerdown);

    HAL_SYSCON_AWO_clk_sel_set(0); // switch to crystal clock
    
       

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
    post_sleep_processing_hardware(mode);

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
    hal_post_sleep_processing_registered_cb(&(sleep_ctrl->sleep_config));

    HAL_SYSCON_AWO_clk_sel_set(1);  // switch to system pll clock
    return ticks;
}

int hal_sleep_set_mode(sleep_mode_enum_t sleep_mode)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();

    sleep_ctrl->sleep_config.sleep_mode = sleep_mode;

    return 0;
}

sleep_mode_enum_t hal_sleep_get_mode(void)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    return sleep_ctrl->sleep_config.sleep_mode;
}

int hal_sleep_register(hal_peripheral_module_t peripheral_module, sleep_condition_cb_t sleep_condition, sleep_processing_cb_t pre_sleep, sleep_processing_cb_t post_sleep)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);
    hal_sleep_action_t *action;

    if(peripheral_module >= MOD_MAX || peripheral_module < MOD_QSPI){
        return -1;
    }

    action = &(sleep_config->actions[peripheral_module]);
    SET_BIT(sleep_config->peripheral_mod, peripheral_module);
    action->irq_num = hal_sleep_peripheral_module_to_IRQn(peripheral_module);
    if(sleep_condition){
        action->sleep_condition = sleep_condition;
    }
    if(pre_sleep){
        action->pre_sleep = pre_sleep;
    }
    if(post_sleep){
        action->post_sleep = post_sleep;
    }
    return 0;
}
int hal_sleep_unregister(hal_peripheral_module_t peripheral_module)
{
    hal_sleep_ctrl_t *sleep_ctrl = hal_sleep_get_handle();
    hal_sleep_config_t *sleep_config = &(sleep_ctrl->sleep_config);
    hal_sleep_action_t *action;

    if(peripheral_module >= MOD_MAX || peripheral_module < MOD_QSPI){
        return -1;
    }

    action = &(sleep_config->actions[peripheral_module]);
    CLR_BIT(sleep_config->peripheral_mod, peripheral_module);
    action->irq_num = 0;
    action->sleep_condition = NULL;
    action->pre_sleep = NULL;
    action->post_sleep = NULL;
    return 0;
}

