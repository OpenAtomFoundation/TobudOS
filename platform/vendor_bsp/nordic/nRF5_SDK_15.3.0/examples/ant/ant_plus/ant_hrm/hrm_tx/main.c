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
 * @defgroup ant_hrm_tx_main ANT HRM TX example
 * @{
 * @ingroup nrf_ant_hrm
 *
 * @brief Example of ANT HRM TX profile.
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
#include "ant_hrm.h"
#include "ant_hrm_simulator.h"
#include "ant_state_indicator.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MODIFICATION_TYPE_BUTTON 0 /* predefined value, MUST REMAIN UNCHANGED */
#define MODIFICATION_TYPE_AUTO   1 /* predefined value, MUST REMAIN UNCHANGED */

#if (MODIFICATION_TYPE != MODIFICATION_TYPE_BUTTON) \
    && (MODIFICATION_TYPE != MODIFICATION_TYPE_AUTO)

    #error Unsupported value of MODIFICATION_TYPE.
#endif

#define APP_TICK_EVENT_INTERVAL  2000 /**< 2 second's tick event interval in timer tick units. */
#define HRM_CHANNEL_NUMBER       0x00 /**< Channel number assigned to HRM profile. */
#define ANTPLUS_NETWORK_NUMBER   0    /**< Network number. */

/** @snippet [ANT HRM TX Instance] */
void ant_hrm_evt_handler(ant_hrm_profile_t * p_profile, ant_hrm_evt_t event);

HRM_SENS_CHANNEL_CONFIG_DEF(m_ant_hrm,
                            HRM_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM);
HRM_SENS_PROFILE_CONFIG_DEF(m_ant_hrm,
                            true,
                            ANT_HRM_PAGE_0,
                            ant_hrm_evt_handler);

static ant_hrm_profile_t m_ant_hrm;
/** @snippet [ANT HRM TX Instance] */


NRF_SDH_ANT_OBSERVER(m_ant_observer, ANT_HRM_ANT_OBSERVER_PRIO,
                     ant_hrm_sens_evt_handler, &m_ant_hrm);


static ant_hrm_simulator_t  m_ant_hrm_simulator;    /**< Simulator used to simulate pulse. */
APP_TIMER_DEF(m_tick_timer);                        /**< Timer used to update cumulative operating time. */

#if MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON
/**@brief Function for handling bsp events.
 */
/** @snippet [ANT HRM simulator button] */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            ant_hrm_simulator_increment(&m_ant_hrm_simulator);
            break;

        case BSP_EVENT_KEY_1:
            ant_hrm_simulator_decrement(&m_ant_hrm_simulator);
            break;

        default:
            return; // no implementation needed
    }
}

/** @snippet [ANT HRM simulator button] */
#endif // MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON

/**
 * @brief 2 seconds tick handler for updataing cumulative operating time.
 */
static void app_tick_handler(void * p_context)
{
    // Only the first 3 bytes of this value are taken into account
    m_ant_hrm.HRM_PROFILE_operating_time++;
}

/**@brief Function for setup all things not directly associated with ANT stack/protocol.
 *
 * @desc Initialization of: @n
 *         - app_timer, presetup for bsp and ant pulse simulation.
 *         - bsp for signaling leds and user buttons (if use button is enabled in example).
 *         - ant pulse simulate for task of filling hrm profile data.
 */
static void utils_setup(void)
{
    // Initialize and start a single continuous mode timer, which is used to update the event time
    // on the main data page.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

#if (MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON)
    /** @snippet [ANT Pulse simulator button init] */
    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                        bsp_evt_handler);
    /** @snippet [ANT Pulse simulator button init] */
#else
    err_code = bsp_init(BSP_INIT_LEDS, NULL);
#endif
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_init(m_ant_hrm.channel_number, HRM_SENS_CHANNEL_TYPE);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_tick_timer,
                                APP_TIMER_MODE_REPEATED,
                                app_tick_handler);
    APP_ERROR_CHECK(err_code);

    // Schedule a timeout event every 2 seconds
    err_code = app_timer_start(m_tick_timer, APP_TICK_EVENT_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the HRM simulator initialization.
 */
static void simulator_setup(void)
{
    /** @snippet [ANT HRM simulator init] */
    const ant_hrm_simulator_cfg_t simulator_cfg =
        DEFAULT_ANT_HRM_SIMULATOR_CFG(&m_ant_hrm, SIMULATOR_MIN, SIMULATOR_MAX, SIMULATOR_INCR);

    /** @snippet [ANT HRM simulator init] */
#if MODIFICATION_TYPE == MODIFICATION_TYPE_AUTO
    /** @snippet [ANT HRM simulator auto init] */
    ant_hrm_simulator_init(&m_ant_hrm_simulator, &simulator_cfg, true);
    /** @snippet [ANT HRM simulator auto init] */
#else
    /** @snippet [ANT HRM simulator button init] */
    ant_hrm_simulator_init(&m_ant_hrm_simulator, &simulator_cfg, false);
    /** @snippet [ANT HRM simulator button init] */
#endif
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

/**@brief Function for handling ANT HRM events.
 */
/** @snippet [ANT HRM simulator call] */
void ant_hrm_evt_handler(ant_hrm_profile_t * p_profile, ant_hrm_evt_t event)
{
    nrf_pwr_mgmt_feed();

    switch (event)
    {
        case ANT_HRM_PAGE_0_UPDATED:
            /* fall through */
        case ANT_HRM_PAGE_1_UPDATED:
            /* fall through */
        case ANT_HRM_PAGE_2_UPDATED:
            /* fall through */
        case ANT_HRM_PAGE_3_UPDATED:
            /* fall through */
        case ANT_HRM_PAGE_4_UPDATED:
            ant_hrm_simulator_one_iteration(&m_ant_hrm_simulator);
            break;

        default:
            break;
    }
}
/** @snippet [ANT HRM simulator call] */

/**
 * @brief Function for HRM profile initialization.
 *
 * @details Initializes the HRM profile and open ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT HRM TX Profile Setup] */
    ret_code_t err_code;

    err_code = ant_hrm_sens_init(&m_ant_hrm,
                                 HRM_SENS_CHANNEL_CONFIG(m_ant_hrm),
                                 HRM_SENS_PROFILE_CONFIG(m_ant_hrm));
    APP_ERROR_CHECK(err_code);

    m_ant_hrm.HRM_PROFILE_manuf_id   = HRM_MFG_ID;
    m_ant_hrm.HRM_PROFILE_serial_num = HRM_SERIAL_NUMBER;
    m_ant_hrm.HRM_PROFILE_hw_version = HRM_HW_VERSION;
    m_ant_hrm.HRM_PROFILE_sw_version = HRM_SW_VERSION;
    m_ant_hrm.HRM_PROFILE_model_num  = HRM_MODEL_NUMBER;

    err_code = ant_hrm_sens_open(&m_ant_hrm);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT HRM TX Profile Setup] */
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
    simulator_setup();
    profile_setup();

    NRF_LOG_INFO("ANT+ Heart Rate TX example started.");

    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}


/**
 *@}
 **/
