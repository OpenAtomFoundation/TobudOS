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
 * @defgroup ant_sdm_tx_example ANT SDM TX example
 * @{
 * @ingroup nrf_ant_sdm
 *
 * @brief Example of ANT SDM TX Profile.
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
#include "ant_sdm.h"
#include "ant_sdm_simulator.h"
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

/** @snippet [ANT SDM TX Instance] */
void ant_sdm_evt_handler(ant_sdm_profile_t * p_profile, ant_sdm_evt_t event);

SDM_SENS_CHANNEL_CONFIG_DEF(m_ant_sdm,
                            SDM_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM);
SDM_SENS_PROFILE_CONFIG_DEF(m_ant_sdm,
                            ANT_SDM_PAGE_2,
                            ant_sdm_evt_handler);

static ant_sdm_profile_t m_ant_sdm;
/** @snippet [ANT SDM TX Instance] */


NRF_SDH_ANT_OBSERVER(m_ant_observer, ANT_SDM_ANT_OBSERVER_PRIO, ant_sdm_sens_evt_handler, &m_ant_sdm);


static ant_sdm_simulator_t m_ant_sdm_simulator;    /**< Simulator used to simulate cadence. */

#if MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON
/**@brief Function for handling bsp events.
 */
/** @snippet [ANT SDM simulator button] */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            ant_sdm_simulator_increment(&m_ant_sdm_simulator);
            break;

        case BSP_EVENT_KEY_1:
            ant_sdm_simulator_decrement(&m_ant_sdm_simulator);
            break;

        default:
            return; // no implementation needed
    }
}
/** @snippet [ANT SDM simulator button] */
#endif // MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON

/**@brief Function for the timer and BSP initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

#if MODIFICATION_TYPE == MODIFICATION_TYPE_AUTO
    err_code = bsp_init(BSP_INIT_LEDS, NULL);
#else
    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                        bsp_evt_handler);
#endif

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_init(m_ant_sdm.channel_number, SDM_SENS_CHANNEL_TYPE);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the SDM simulator initialization.
 */
static void simulator_setup(void)
{
    /** @snippet [ANT SDM simulator init] */
    const ant_sdm_simulator_cfg_t simulator_cfg = DEFAULT_ANT_SDM_SIMULATOR_CFG(&m_ant_sdm,
                                                                                SIMULATOR_STRIDE_LEN,
                                                                                SIMULATOR_BURN_RATE,
                                                                                SIMULATOR_MIN,
                                                                                SIMULATOR_MAX,
                                                                                SIMULATOR_INCR);

    /** @snippet [ANT SDM simulator init] */

#if MODIFICATION_TYPE == MODIFICATION_TYPE_AUTO
    /** @snippet [ANT SDM simulator auto init] */
    ant_sdm_simulator_init(&m_ant_sdm_simulator, &simulator_cfg, true);
    /** @snippet [ANT SDM simulator auto init] */
#else
    /** @snippet [ANT SDM simulator button init] */
    ant_sdm_simulator_init(&m_ant_sdm_simulator, &simulator_cfg, false);
    /** @snippet [ANT SDM simulator button init] */
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

/**@brief Function for handling ANT SDM events.
 */
/** @snippet [ANT SDM simulator call] */
void ant_sdm_evt_handler(ant_sdm_profile_t * p_profile, ant_sdm_evt_t event)
{
    nrf_pwr_mgmt_feed();

    switch (event)
    {
        case ANT_SDM_PAGE_1_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_2_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_3_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_16_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_22_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_80_UPDATED:
        /* fall through */
        case ANT_SDM_PAGE_81_UPDATED:
            ant_sdm_simulator_one_iteration(&m_ant_sdm_simulator);
            break;

        default:
            break;
    }
}
/** @snippet [ANT SDM simulator call] */

/**@brief Function for SDM Profile initialization.
 *
 * @details Initializes the SDM Profile and opens the ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT SDM TX Profile Setup] */
    uint32_t err_code;

    err_code = ant_sdm_sens_init(&m_ant_sdm,
                                 SDM_SENS_CHANNEL_CONFIG(m_ant_sdm),
                                 SDM_SENS_PROFILE_CONFIG(m_ant_sdm));
    APP_ERROR_CHECK(err_code);

    // fill manufacturer's common data page.
    m_ant_sdm.page_80 = ANT_COMMON_page80(SDM_HW_REVISION,
                                          SDM_MANUFACTURER_ID,
                                          SDM_MODEL_NUMBER);
    // fill product's common data page.
    m_ant_sdm.page_81 = ANT_COMMON_page81(SDM_SW_REVISION_MAJOR,
                                          SDM_SW_REVISION_MINOR,
                                          SDM_SERIAL_NUMBER);

    // fill capabilities.
    m_ant_sdm.SDM_PROFILE_capabilities.cadency_is_valid  = true;
    m_ant_sdm.SDM_PROFILE_capabilities.speed_is_valid    = true;
    m_ant_sdm.SDM_PROFILE_capabilities.calorie_is_valid  = true;
    m_ant_sdm.SDM_PROFILE_capabilities.time_is_valid     = true;
    m_ant_sdm.SDM_PROFILE_capabilities.latency_is_valid  = true;
    m_ant_sdm.SDM_PROFILE_capabilities.distance_is_valid = true;

    // fill status.
    m_ant_sdm.SDM_PROFILE_status.state    = ANT_SDM_USE_STATE_ACTIVE;
    m_ant_sdm.SDM_PROFILE_status.health   = ANT_SDM_HEALTH_OK;
    m_ant_sdm.SDM_PROFILE_status.battery  = ANT_SDM_BATTERY_STATUS_GOOD;
    m_ant_sdm.SDM_PROFILE_status.location = ANT_SDM_LOCATION_ANKLE;

    err_code = ant_sdm_sens_open(&m_ant_sdm);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT SDM TX Profile Setup] */
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

    NRF_LOG_INFO("ANT+ Stride Based Speed and Distance TX example started.");

    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}
