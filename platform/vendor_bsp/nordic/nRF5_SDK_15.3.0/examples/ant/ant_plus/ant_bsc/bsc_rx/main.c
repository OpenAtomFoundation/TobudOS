/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2012
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
/**@file
 * @defgroup ant_bsc_rx_example ANT BSC RX example
 * @{
 * @ingroup nrf_ant_bsc
 *
 * @brief Example of ANT BSC RX profile.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdio.h>
#include "nrf.h"
#include "bsp.h"
#include "hardfault.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "ant_key_manager.h"
#include "ant_bsc.h"
#include "bsp_btn_ant.h"
#include "ant_state_indicator.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define WHEEL_CIRCUMFERENCE         2070                                                            /**< Bike wheel circumference [mm] */
#define BSC_EVT_TIME_FACTOR         1024                                                            /**< Time unit factor for BSC events */
#define BSC_RPM_TIME_FACTOR         60                                                              /**< Time unit factor for RPM unit */
#define BSC_MS_TO_KPH_NUM           36                                                              /**< Numerator of [m/s] to [kph] ratio */
#define BSC_MS_TO_KPH_DEN           10                                                              /**< Denominator of [m/s] to [kph] ratio */
#define BSC_MM_TO_M_FACTOR          1000                                                            /**< Unit factor [m/s] to [mm/s] */
#define SPEED_COEFFICIENT           (WHEEL_CIRCUMFERENCE * BSC_EVT_TIME_FACTOR * BSC_MS_TO_KPH_NUM \
                                     / BSC_MS_TO_KPH_DEN / BSC_MM_TO_M_FACTOR)                      /**< Coefficient for speed value calculation */
#define CADENCE_COEFFICIENT         (BSC_EVT_TIME_FACTOR * BSC_RPM_TIME_FACTOR)                     /**< Coefficient for cadence value calculation */

#define APP_ANT_OBSERVER_PRIO       1                                                               /**< Application's ANT observer priority. You shouldn't need to modify this value. */

typedef struct
{
    int32_t acc_rev_cnt;
    int32_t prev_rev_cnt;
    int32_t prev_acc_rev_cnt;
    int32_t acc_evt_time;
    int32_t prev_evt_time;
    int32_t prev_acc_evt_time;
} bsc_disp_calc_data_t;

static bsc_disp_calc_data_t m_speed_calc_data   = {0};
static bsc_disp_calc_data_t m_cadence_calc_data = {0};

/** @snippet [ANT BSC RX Instance] */
void ant_bsc_evt_handler(ant_bsc_profile_t * p_profile, ant_bsc_evt_t event);

BSC_DISP_CHANNEL_CONFIG_DEF(m_ant_bsc,
                            BSC_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            DISPLAY_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM,
                            BSC_MSG_PERIOD_4Hz);
BSC_DISP_PROFILE_CONFIG_DEF(m_ant_bsc,
                            ant_bsc_evt_handler);

static ant_bsc_profile_t m_ant_bsc;

NRF_SDH_ANT_OBSERVER(m_bsc_ant_observer, ANT_BSC_ANT_OBSERVER_PRIO,
                     ant_bsc_disp_evt_handler, &m_ant_bsc);

/** @snippet [ANT BSC RX Instance] */

static uint32_t calculate_speed(int32_t rev_cnt, int32_t evt_time)
{
    static uint32_t computed_speed   = 0;

    if (rev_cnt != m_speed_calc_data.prev_rev_cnt)
    {
        m_speed_calc_data.acc_rev_cnt  += rev_cnt - m_speed_calc_data.prev_rev_cnt;
        m_speed_calc_data.acc_evt_time += evt_time - m_speed_calc_data.prev_evt_time;

        /* Process rollover */
        if (m_speed_calc_data.prev_rev_cnt > rev_cnt)
        {
            m_speed_calc_data.acc_rev_cnt += UINT16_MAX + 1;
        }
        if (m_speed_calc_data.prev_evt_time > evt_time)
        {
            m_speed_calc_data.acc_evt_time += UINT16_MAX + 1;
        }

        m_speed_calc_data.prev_rev_cnt  = rev_cnt;
        m_speed_calc_data.prev_evt_time = evt_time;

        computed_speed = SPEED_COEFFICIENT *
                         (m_speed_calc_data.acc_rev_cnt  - m_speed_calc_data.prev_acc_rev_cnt) /
                         (m_speed_calc_data.acc_evt_time - m_speed_calc_data.prev_acc_evt_time);

        m_speed_calc_data.prev_acc_rev_cnt  = m_speed_calc_data.acc_rev_cnt;
        m_speed_calc_data.prev_acc_evt_time = m_speed_calc_data.acc_evt_time;
    }

    return (uint32_t)computed_speed;
}

static uint32_t calculate_cadence(int32_t rev_cnt, int32_t evt_time)
{
    static uint32_t computed_cadence = 0;

    if (rev_cnt != m_cadence_calc_data.prev_rev_cnt)
    {
        m_cadence_calc_data.acc_rev_cnt  += rev_cnt - m_cadence_calc_data.prev_rev_cnt;
        m_cadence_calc_data.acc_evt_time += evt_time - m_cadence_calc_data.prev_evt_time;

        /* Process rollover */
        if (m_cadence_calc_data.prev_rev_cnt > rev_cnt)
        {
            m_cadence_calc_data.acc_rev_cnt += UINT16_MAX + 1;
        }
        if (m_cadence_calc_data.prev_evt_time > evt_time)
        {
            m_cadence_calc_data.acc_evt_time += UINT16_MAX + 1;
        }

        m_cadence_calc_data.prev_rev_cnt  = rev_cnt;
        m_cadence_calc_data.prev_evt_time = evt_time;

        computed_cadence = CADENCE_COEFFICIENT *
                        (m_cadence_calc_data.acc_rev_cnt  - m_cadence_calc_data.prev_acc_rev_cnt) /
                        (m_cadence_calc_data.acc_evt_time - m_cadence_calc_data.prev_acc_evt_time);

        m_cadence_calc_data.prev_acc_rev_cnt  = m_cadence_calc_data.acc_rev_cnt;
        m_cadence_calc_data.prev_acc_evt_time = m_cadence_calc_data.acc_evt_time;
    }

    return (uint32_t)computed_cadence;
}

/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
static void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    switch (p_ant_evt->event)
    {
        case EVENT_RX_FAIL_GO_TO_SEARCH:
            /* Reset speed and cadence values */
            memset(&m_speed_calc_data, 0, sizeof(m_speed_calc_data));
            memset(&m_cadence_calc_data, 0, sizeof(m_cadence_calc_data));
            break;

        default:
            // No implementation needed
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);

void ant_bsc_evt_handler(ant_bsc_profile_t * p_profile, ant_bsc_evt_t event)
{
    switch (event)
    {
        case ANT_BSC_PAGE_0_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_1_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_2_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_3_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_4_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_5_UPDATED:
            /* Log computed value */

            if (DISPLAY_TYPE == BSC_SPEED_DEVICE_TYPE)
            {
                NRF_LOG_INFO("Computed speed value:                 %u kph",
                              (unsigned int) calculate_speed(p_profile->BSC_PROFILE_rev_count,
                                                             p_profile->BSC_PROFILE_event_time));
            }
            else if (DISPLAY_TYPE == BSC_CADENCE_DEVICE_TYPE)
            {
                NRF_LOG_INFO("Computed cadence value:               %u rpm",
                              (unsigned int) calculate_cadence(p_profile->BSC_PROFILE_rev_count,
                                                               p_profile->BSC_PROFILE_event_time));
            }
            break;

        case ANT_BSC_COMB_PAGE_0_UPDATED:
            NRF_LOG_INFO("Computed speed value:                         %u kph",
                          (unsigned int) calculate_speed(p_profile->BSC_PROFILE_speed_rev_count,
                                                         p_profile->BSC_PROFILE_speed_event_time));
            NRF_LOG_INFO("Computed cadence value:                       %u rpms",
                          (unsigned int) calculate_cadence(p_profile->BSC_PROFILE_cadence_rev_count,
                                                           p_profile->BSC_PROFILE_cadence_event_time));
            break;

        default:
            break;
    }
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by BSP.
 */
void bsp_event_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
            break;

        default:
            break;
    }
}

/**
 * @brief Function for shutdown events.
 *
 * @param[in]   event       Shutdown type.
 */
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            err_code = bsp_btn_ant_sleep_mode_prepare();
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }

    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler,  APP_SHUTDOWN_HANDLER_PRIORITY);

/**@brief Function for the Timer and BSP initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                        bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ant_init(m_ant_bsc.channel_number, BSC_DISP_CHANNEL_TYPE);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_init(m_ant_bsc.channel_number, BSC_DISP_CHANNEL_TYPE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the ANT event interrupt.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUM);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for BSC profile initialization.
 *
 * @details Initializes the BSC profile and open ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT BSC RX Profile Setup] */
    uint32_t err_code;

    err_code = ant_bsc_disp_init(&m_ant_bsc,
                                 BSC_DISP_CHANNEL_CONFIG(m_ant_bsc),
                                 BSC_DISP_PROFILE_CONFIG(m_ant_bsc));
    APP_ERROR_CHECK(err_code);

    err_code = ant_bsc_disp_open(&m_ant_bsc);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT BSC RX Profile Setup] */
}

/**
 *@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for application main entry, does not return.
 */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();
    profile_setup();

    NRF_LOG_INFO("ANT+ Bicycle Speed and Cadence RX example started.");

    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}


/**
 *@}
 **/
