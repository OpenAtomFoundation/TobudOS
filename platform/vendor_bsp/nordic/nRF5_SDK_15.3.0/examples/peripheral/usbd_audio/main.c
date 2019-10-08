/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf.h"
#include "app_util.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"

#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_audio.h"
#include "app_error.h"
#include "boards.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/**@file
 * @defgroup usbd_aduio_example main.c
 * @{
 * @ingroup usbd_audio_example
 * @brief USBD Audio class example
 *
 */

#define LED_USB_RESUME (BSP_BOARD_LED_0)
#define LED_USB_START  (BSP_BOARD_LED_1)
#define LED_AUDIO_RX   (BSP_BOARD_LED_2)
#define LED_AUDIO_TX   (BSP_BOARD_LED_3)

/**
 * @brief USB audio samples size
 */
#define BUFFER_SIZE  (48)

/**
 * @brief Enable power USB detection
 *
 * Configure if example supports USB port connection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

/**
 * @brief Audio class user event handler
 */
static void hp_audio_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                     app_usbd_audio_user_event_t   event);
static void mic_audio_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_audio_user_event_t   event);

/* Channels and feature controls configuration */

/**
 * @brief   Input terminal channel configuration
 */
#define HP_TERMINAL_CH_CONFIG()                                                                       \
        (APP_USBD_AUDIO_IN_TERM_CH_CONFIG_LEFT_FRONT | APP_USBD_AUDIO_IN_TERM_CH_CONFIG_RIGHT_FRONT)

/**
 * @brief   Feature controls
 *
 *      general
 *      channel 0
 *      channel 1
 */
#define HP_FEATURE_CONTROLS()                                               \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE),  \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE),  \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE)



/**
 * @brief   Input terminal channel configuration
 */
#define MIC_TERMINAL_CH_CONFIG()                                                                       \
        (APP_USBD_AUDIO_IN_TERM_CH_CONFIG_LEFT_FRONT | APP_USBD_AUDIO_IN_TERM_CH_CONFIG_RIGHT_FRONT)

/**
 * @brief   Feature controls
 *
 *      general
 *      channel 0
 *      channel 1
 */
#define MIC_FEATURE_CONTROLS()                                                                     \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE),                         \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE),                         \
        APP_USBD_U16_TO_RAW_DSC(APP_USBD_AUDIO_FEATURE_UNIT_CONTROL_MUTE)


/* Microphone descriptors */

/**
 * @brief   Audio class specific format descriptor
 */
APP_USBD_AUDIO_FORMAT_DESCRIPTOR(m_mic_form_desc, 
                                 APP_USBD_AUDIO_AS_FORMAT_I_DSC(    /* Format type 1 descriptor */
                                    2,                              /* Number of channels */
                                    2,                              /* Subframe size */
                                    16,                             /* Bit resolution */
                                    1,                              /* Frequency type */
                                    APP_USBD_U24_TO_RAW_DSC(48000)) /* Frequency */
                                );

/**
 * @brief   Audio class input terminal descriptor
 */
APP_USBD_AUDIO_INPUT_DESCRIPTOR(m_mic_inp_desc, 
                                APP_USBD_AUDIO_INPUT_TERMINAL_DSC(
                                    1,                                     /* Terminal ID */
                                    APP_USBD_AUDIO_TERMINAL_IN_MICROPHONE, /* Terminal type */
                                    2,                                     /* Number of channels */
                                    MIC_TERMINAL_CH_CONFIG())              /* Channels config */
                                );

/**
 * @brief   Audio class output terminal descriptor
 */
APP_USBD_AUDIO_OUTPUT_DESCRIPTOR(m_mic_out_desc, 
                                 APP_USBD_AUDIO_OUTPUT_TERMINAL_DSC(
                                    3,                                     /* Terminal ID */
                                    APP_USBD_AUDIO_TERMINAL_USB_STREAMING, /* Terminal type */
                                    2)                                     /* Source ID */
                                );

/**
 * @brief   Audio class feature unit descriptor
 */
APP_USBD_AUDIO_FEATURE_DESCRIPTOR(m_mic_fea_desc, 
                                  APP_USBD_AUDIO_FEATURE_UNIT_DSC(
                                    2,                      /* Unit ID */
                                    1,                      /* Source ID */
                                    MIC_FEATURE_CONTROLS()) /* List of controls */
                                 );

/* Headphones descriptors */

/**
 * @brief   Audio class specific format III descriptor
 */
APP_USBD_AUDIO_FORMAT_DESCRIPTOR(m_hp_form_desc, 
                                 APP_USBD_AUDIO_AS_FORMAT_III_DSC(    /* Format type 3 descriptor */
                                    2,                                /* Number of channels */
                                    2,                                /* Subframe size */
                                    16,                               /* Bit resolution */
                                    1,                                /* Frequency type */
                                    APP_USBD_U24_TO_RAW_DSC(48000))   /* Frequency */
                                );

/**
 * @brief   Audio class input terminal descriptor
 */
APP_USBD_AUDIO_INPUT_DESCRIPTOR(m_hp_inp_desc, 
                                APP_USBD_AUDIO_INPUT_TERMINAL_DSC(
                                    1,                                     /* Terminal ID */
                                    APP_USBD_AUDIO_TERMINAL_USB_STREAMING, /* Terminal type */
                                    2,                                     /* Number of channels */
                                    HP_TERMINAL_CH_CONFIG())               /* Channels config */
                               );

/**
 * @brief   Audio class output terminal descriptor
 */
APP_USBD_AUDIO_OUTPUT_DESCRIPTOR(m_hp_out_desc, 
                                 APP_USBD_AUDIO_OUTPUT_TERMINAL_DSC(
                                    3,                                      /* Terminal ID */
                                    APP_USBD_AUDIO_TERMINAL_OUT_HEADPHONES, /* Terminal type */
                                    2)                                      /* Source ID */
                                );

/**
 * @brief   Audio class feature unit descriptor
 */
APP_USBD_AUDIO_FEATURE_DESCRIPTOR(m_hp_fea_desc, 
                                  APP_USBD_AUDIO_FEATURE_UNIT_DSC(
                                    2,                     /* Unit ID */
                                    1,                     /* Source ID */
                                    HP_FEATURE_CONTROLS()) /* List of controls */
                                 );

/* Interfaces lists */

/**
 * @brief Interfaces list passed to @ref APP_USBD_AUDIO_GLOBAL_DEF
 */
#define HP_INTERFACES_CONFIG() APP_USBD_AUDIO_CONFIG_OUT(0, 1)

/**
 * @brief Interfaces list passed to @ref APP_USBD_AUDIO_GLOBAL_DEF
 */
#define MIC_INTERFACES_CONFIG() APP_USBD_AUDIO_CONFIG_IN(2, 3)

/*lint -save -e26 -e64 -e123 -e505 -e651*/


/**
 * @brief Headphone Audio class instance
 */
APP_USBD_AUDIO_GLOBAL_DEF(m_app_audio_headphone,
                          HP_INTERFACES_CONFIG(),
                          hp_audio_user_ev_handler,
                          &m_hp_form_desc,
                          &m_hp_inp_desc,
                          &m_hp_out_desc,
                          &m_hp_fea_desc,
                          0,
                          APP_USBD_AUDIO_AS_IFACE_FORMAT_PCM,
                          192,
                          APP_USBD_AUDIO_SUBCLASS_AUDIOSTREAMING
);



/**
 * @brief Microphone Audio class instance
 */
APP_USBD_AUDIO_GLOBAL_DEF(m_app_audio_microphone,
                          MIC_INTERFACES_CONFIG(),
                          mic_audio_user_ev_handler,
                          &m_mic_form_desc,
                          &m_mic_inp_desc,
                          &m_mic_out_desc,
                          &m_mic_fea_desc,
                          0,
                          APP_USBD_AUDIO_AS_IFACE_FORMAT_PCM,
                          192,
                          APP_USBD_AUDIO_SUBCLASS_AUDIOSTREAMING
);


/*lint -restore*/

/**
 * @brief Internal audio temporary buffer
 */
static int16_t  m_temp_buffer[2 * BUFFER_SIZE];



/**
 * @brief The size of last received block from the microphone
 */
static size_t m_temp_buffer_size;

/**
 * @brief Actual headphones mute
 */
static uint8_t  m_mute_hp;

/**
 * @brief Actual sampling frequency
 */
static uint32_t m_freq_hp;

/**
 * @brief Actual microphone mute state
 */
static uint8_t  m_mute_mic;

/**
 * @brief Actual microphone sampling frequency
 */
static uint32_t m_freq_mic;

/**
 * @brief Audio class specific request handle (headphones)
 */
static void hp_audio_user_class_req(app_usbd_class_inst_t const * p_inst)
{
    app_usbd_audio_t const * p_audio = app_usbd_audio_class_get(p_inst);
    app_usbd_audio_req_t * p_req = app_usbd_audio_class_request_get(p_audio);

    UNUSED_VARIABLE(m_mute_hp);
    UNUSED_VARIABLE(m_freq_hp);

    switch (p_req->req_target)
    {
        case APP_USBD_AUDIO_CLASS_REQ_IN:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only mute control is defined
                p_req->payload[0] = m_mute_hp;
            }

            break;
        case APP_USBD_AUDIO_CLASS_REQ_OUT:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only mute control is defined
                m_mute_hp = p_req->payload[0];
            }

            break;
        case APP_USBD_AUDIO_EP_REQ_IN:
            break;
        case APP_USBD_AUDIO_EP_REQ_OUT:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only set frequency is supported
                m_freq_hp = uint24_decode(p_req->payload);
            }

            break;
        default:
            break;
    }
}

/**
 * @brief Audio class specific request handle (microphone)
 */
static void mic_audio_user_class_req(app_usbd_class_inst_t const * p_inst)
{
    app_usbd_audio_t const * p_audio = app_usbd_audio_class_get(p_inst);
    app_usbd_audio_req_t * p_req = app_usbd_audio_class_request_get(p_audio);

    UNUSED_VARIABLE(m_mute_mic);
    UNUSED_VARIABLE(m_freq_mic);

    switch (p_req->req_target)
    {
        case APP_USBD_AUDIO_CLASS_REQ_IN:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only mute control is defined
                p_req->payload[0] = m_mute_mic;
            }

            break;
        case APP_USBD_AUDIO_CLASS_REQ_OUT:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only mute control is defined
                m_mute_mic = p_req->payload[0];
            }

            break;
        case APP_USBD_AUDIO_EP_REQ_IN:
            break;
        case APP_USBD_AUDIO_EP_REQ_OUT:

            if (p_req->req_type == APP_USBD_AUDIO_REQ_SET_CUR)
            {
                //Only set frequency is supported
                m_freq_mic = uint24_decode(p_req->payload);
            }

            break;
        default:
            break;
    }
}

/**
 * @brief User event handler @ref app_usbd_audio_user_ev_handler_t (headphones)
 */
static void hp_audio_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                     app_usbd_audio_user_event_t   event)
{
    app_usbd_audio_t const * p_audio = app_usbd_audio_class_get(p_inst);
    UNUSED_VARIABLE(p_audio);
    switch (event)
    {
        case APP_USBD_AUDIO_USER_EVT_CLASS_REQ:
            hp_audio_user_class_req(p_inst);
            break;
        case APP_USBD_AUDIO_USER_EVT_RX_DONE:
        {
            ret_code_t ret;
            /* Block from headphones copied into buffer, send it into microphone input */
            ret = app_usbd_audio_class_tx_start(&m_app_audio_microphone.base, m_temp_buffer, m_temp_buffer_size);
            if (NRF_SUCCESS == ret)
            {
                bsp_board_led_invert(LED_AUDIO_RX);
            }
            break;
        }
        default:
            break;
    }
}

/**
 * @brief User event handler @ref app_usbd_audio_user_ev_handler_t (microphone)
 */
static void mic_audio_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_audio_user_event_t   event)
{
    app_usbd_audio_t const * p_audio = app_usbd_audio_class_get(p_inst);
    UNUSED_VARIABLE(p_audio);

    switch (event)
    {
        case APP_USBD_AUDIO_USER_EVT_CLASS_REQ:
            mic_audio_user_class_req(p_inst);
            break;
        case APP_USBD_AUDIO_USER_EVT_TX_DONE:
        {
            bsp_board_led_invert(LED_AUDIO_TX);
            break;
        }
        default:
            break;
    }
}

static void hp_sof_ev_handler(uint16_t framecnt)
{
    UNUSED_VARIABLE(framecnt);
    if (APP_USBD_STATE_Configured != app_usbd_core_state_get())
    {
        return;
    }
    size_t rx_size = app_usbd_audio_class_rx_size_get(&m_app_audio_headphone.base);
    m_temp_buffer_size = rx_size;
    if (rx_size > 0)
    {
        ASSERT(rx_size <= sizeof(m_temp_buffer));
        ret_code_t ret;
        ret = app_usbd_audio_class_rx_start(&m_app_audio_headphone.base, m_temp_buffer, rx_size);
        if (NRF_SUCCESS != ret)
        {
            NRF_LOG_ERROR("Cannot start RX transfer from headphone\r\n");
        }
    }
}

/**
 * @brief USBD library specific event handler.
 *
 * @param event     USBD library event.
 */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_DRV_RESUME:
            bsp_board_led_on(LED_USB_RESUME);
            break;
        case APP_USBD_EVT_STARTED:
            bsp_board_led_on(LED_USB_START);
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;
        default:
            break;
    }
}

int main(void)
{
    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler,
        .enable_sof = true
    };

    ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);

    NRF_LOG_INFO("USBD audio example started.");

    // Initialize LEDs and buttons
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_hp =
        app_usbd_audio_class_inst_get(&m_app_audio_headphone);

    ret = app_usbd_audio_sof_interrupt_register(class_inst_hp, hp_sof_ev_handler);
    APP_ERROR_CHECK(ret);

    ret = app_usbd_class_append(class_inst_hp);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_mic =
        app_usbd_audio_class_inst_get(&m_app_audio_microphone);
    ret = app_usbd_class_append(class_inst_mic);
    APP_ERROR_CHECK(ret);

    if (USBD_POWER_DETECTION)
    {
        ret = app_usbd_power_events_enable();
        APP_ERROR_CHECK(ret);
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    while (true)
    {
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }

        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        /* Sleep CPU only if there was no interrupt since last loop processing */
        __WFE();
    }
}

/** @} */
