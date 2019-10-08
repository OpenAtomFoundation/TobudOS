/**
 * Copyright (c) 2016 - 2019 Nordic Semiconductor ASA and Luxoft Global Operations Gmbh.
 *
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
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
#include "config.h"

#include "fsm.h"
#include "uart.h"
#if (CONFIG_SECURE == 1)
#include "security.h"
#endif
#include "sys_fsm.h"
#include "sys_utils.h"
#include "sys_memory_manager.h"
#include "sys_debug.h"
#include "sys_task_scheduler.h"
#include "boards.h"
#include "nrf_gpio.h"
#include <stdint.h>
#include <stddef.h>

typedef struct
{
    mac_payload_descriptor_t   payload_descr;
    uint8_t                    length;
} fsm_frame_item_t;

typedef enum
{
    S_INITIAL,
    S_SETTING_CH,
    S_SETTING_SHORT_ADDR,
    S_SETTING_EXT_ADDR,
    S_SETTING_PAN_ID,
    S_SETTING_RX_ON_IDLE,
    S_SETTING_SECURITY,
    S_WORKING,
} fsm_states_t;

typedef enum
{
    GU_SET_SUCCESS,
    GU_SEND_SUCCESS,
    GU_UART_TX_DATA_AVAILABLE,
    GU_RADIO_TX_IDLE,
    GU_UART_TX_IDLE,
} fsm_guards_t;

typedef enum
{
    A_CHANNEL_SET,
    A_SHORT_ADDR_SET,
    A_EXT_ADDR_SET,
    A_PAN_ID_SET,
    A_RX_ON_IDLE_SET,
    A_CONFIG_COMPLETE,
    A_RADIO_TX_START,
    A_RADIO_TX_RESTART,
    A_UART_TX_START,
    A_UART_TX_IDLE_SET,
#if (CONFIG_SECURE == 1)
    A_SECURITY_SET
#endif
} fsm_actions_t;

typedef void (* fsm_action_t)(void *);
typedef bool (* fsm_guard_t)(void *);
typedef void (* app_action_t)(void);

typedef void (* store_data_handler_t)(fsm_event_data_t *);

// prototypes
static void fsm_action(sys_fsm_action_id_t action_id, void * p_data);
static bool fsm_guard(sys_fsm_guard_id_t guard_id, void * p_data);

static void a_channel_set(void * p_data);
static void a_short_addr_set(void * p_data);
static void a_ext_addr_set(void * p_data);
static void a_pan_id_set(void * p_data);
static void a_rx_on_idle_set(void * p_data);
static void a_config_complete(void * p_data);
static void a_radio_tx_start(void * p_data);
static void a_radio_tx_restart(void * p_data);
static void a_uart_tx_start(void * p_data);
static void a_uart_tx_idle_set(void * p_data);
#if (CONFIG_SECURE == 1)
static void a_security_set(void * p_data);
#endif

static bool gu_set_success(void * p_data);
static bool gu_send_success(void * p_data);
static bool gu_uart_tx_data_available(void * p_data);
static bool gu_radio_tx_idle(void * p_data);
static bool gu_uart_tx_idle(void * p_data);

static bool frames_available(void);

static void mlme_set_conf_store(fsm_event_data_t * p_data);
static void mcps_data_ind_store(fsm_event_data_t * p_data);
static void mcps_data_conf_store(fsm_event_data_t * p_data);

static void mcps_data_conf(mcps_data_conf_t * conf);
static void mlme_set_conf(mlme_set_conf_t * conf);

static const sys_fsm_transition_t m_transition_table[] =
{
    SYS_FSM_STATE     ( S_WORKING ),
    SYS_FSM_TRANSITION( E_SET_DONE,          SYS_FSM_ALWAYS,            A_CONFIG_COMPLETE,             SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_UART_TX_DONE,      GU_UART_TX_DATA_AVAILABLE, A_UART_TX_START,               SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_UART_TX_DONE,      SYS_FSM_OTHERWISE,         A_UART_TX_IDLE_SET,            SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_UART_RX_DONE,      GU_RADIO_TX_IDLE,          A_RADIO_TX_START,              SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_RADIO_RX_DONE,     GU_UART_TX_IDLE,           A_UART_TX_START,               SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_RADIO_TX_DONE,     GU_SEND_SUCCESS,           A_RADIO_TX_START,              SYS_FSM_SAME_STATE ),
    SYS_FSM_TRANSITION( E_RADIO_TX_DONE,     SYS_FSM_OTHERWISE,         A_RADIO_TX_RESTART,            SYS_FSM_SAME_STATE ),

    SYS_FSM_STATE     ( S_INITIAL ),
    SYS_FSM_TRANSITION( E_START_CONFIG,      SYS_FSM_ALWAYS,            A_CHANNEL_SET,                 S_SETTING_CH ),

    SYS_FSM_STATE     ( S_SETTING_CH ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_SHORT_ADDR_SET,              S_SETTING_SHORT_ADDR ),

    SYS_FSM_STATE     ( S_SETTING_SHORT_ADDR ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_EXT_ADDR_SET,                S_SETTING_EXT_ADDR ),

    SYS_FSM_STATE     ( S_SETTING_EXT_ADDR ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_PAN_ID_SET,                  S_SETTING_PAN_ID ),
#if (CONFIG_SECURE == 1)
    SYS_FSM_STATE     ( S_SETTING_PAN_ID ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_RX_ON_IDLE_SET,              S_SETTING_RX_ON_IDLE ),

    SYS_FSM_STATE     ( S_SETTING_RX_ON_IDLE ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_SECURITY_SET,                S_WORKING ),
#else
    SYS_FSM_STATE     ( S_SETTING_PAN_ID ),
    SYS_FSM_TRANSITION( E_SET_DONE,          GU_SET_SUCCESS,            A_RX_ON_IDLE_SET,              S_WORKING ),
#endif
};

static const sys_fsm_const_descriptor_t m_fsm_descriptor =
{
    FSM_DEBUG_NAME("wi-art fsm")

    /* Pointer to the transition table.*/
    .transition_table = m_transition_table,

    /* Number of transitions in the transition table. */
    .transitions_amount = ARRAY_SIZE(m_transition_table),

    /* Initial state ID. */
    .initial_state = S_INITIAL,

    /* Pointer to the guard condition function */
    .guard = fsm_guard,

    /* Pointer to the action function */
    .action = fsm_action
};

static sys_fsm_t m_fsm;
static mlme_set_req_t * mp_set_req;
static mlme_set_conf_t m_set_conf;
static mcps_data_req_t m_data_req;
static mcps_data_conf_t m_data_conf;
static bool m_uart_tx_idle = true;
static bool m_radio_tx_idle = true;
static uint8_t m_radio_tx_buffer[PHY_MAX_PACKET_SIZE + MAC_MAX_MHR_SIZE];
static sequence_number_t tx_sequence_number = 0;
#if (CONFIG_SECURE == 1)
static uint8_t m_radio_tx_buffer_shadow[MAX_MSDU_SIZE + MAX_APP_SEQUENCE_NUMBER_SIZE];
#endif
static uint8_t m_events = 0;
static store_data_handler_t event_data_store[EVENTS_AMOUNT] =
{
    NULL,                 // E_START_CONFIG
    mlme_set_conf_store,  // E_SET_DONE
    mcps_data_ind_store,  // E_RADIO_RX_DONE
    mcps_data_conf_store, // E_RADIO_TX_DONE
    NULL,                 // E_UART_RX_DONE
    NULL,                 // E_UART_TX_DONE
    NULL,                 // E_SECURITY_SET
};
static fsm_frame_item_t m_radio_rx_array[CONFIG_POOL_SIZE / (PHY_MAX_PACKET_SIZE + PHY_MAX_HEADER_SIZE) + 1U];
static size_t m_radio_rx_array_read_index = 0;
static size_t m_radio_rx_array_write_index = 0;

static const fsm_guard_t m_fsm_guards[] =
{
    gu_set_success,
    gu_send_success,
    gu_uart_tx_data_available,
    gu_radio_tx_idle,
    gu_uart_tx_idle,
};

static const fsm_action_t m_fsm_actions[] =
{
    a_channel_set,
    a_short_addr_set,
    a_ext_addr_set,
    a_pan_id_set,
    a_rx_on_idle_set,
    a_config_complete,
    a_radio_tx_start,
    a_radio_tx_restart,
    a_uart_tx_start,
    a_uart_tx_idle_set,
#if (CONFIG_SECURE == 1)
    a_security_set,
#endif
};

static void fsm_action(sys_fsm_action_id_t action_id, void * p_data)
{
    m_fsm_actions[action_id](p_data);
}

static bool fsm_guard(sys_fsm_guard_id_t guard_id, void * p_data)
{
    return m_fsm_guards[guard_id](p_data);
}

/* guards */

static bool gu_set_success(void * p_data)
{
    if (m_set_conf.status != MAC_SUCCESS)
    {
        ASSERT_INFO(0, "Failed to set attribute %02x\n", m_set_conf.pib_attribute.mlme_id);
        return false;
    }

    return true;
}

static bool gu_send_success(void * p_data)
{
    return m_data_conf.status == MAC_SUCCESS;
}

static bool gu_uart_tx_data_available(void * p_data)
{
    return frames_available();
}

static bool gu_radio_tx_idle(void * p_data)
{
    return m_radio_tx_idle;
}

static bool gu_uart_tx_idle(void * p_data)
{
    return m_uart_tx_idle;
}


/* actions */

static void a_channel_set(void * p_data)
{
    const pib_id_t pib_id =
    {
        .plme_id = PHY_CURRENT_CHANNEL_ID,
    };
    mp_set_req = (mlme_set_req_t *)sys_mm_alloc(sizeof(mlme_set_req_t));
    ASSERT(mp_set_req != NULL);
    mp_set_req->value = (uint8_t *)sys_mm_alloc(mlme_pib_attr_size_calc(pib_id, 0));
    ASSERT(mp_set_req->value != NULL);

    uint8_t channel = CONFIG_TEST_CHANNEL;
    mp_set_req->pib_attribute.plme_id = PHY_CURRENT_CHANNEL_ID;
    mp_set_req->pib_attribute_idx = 0;
    memcpy(mp_set_req->value, &channel, sizeof(channel));
    mlme_set_req(mp_set_req, mlme_set_conf);
}

static void a_short_addr_set(void * p_data)
{
    const pib_id_t pib_id =
    {
        .mlme_id = MAC_SHORT_ADDRESS,
    };
    mp_set_req = (mlme_set_req_t *)sys_mm_alloc(sizeof(mlme_set_req_t));
    ASSERT(mp_set_req != NULL);
    mp_set_req->value = (uint8_t *)sys_mm_alloc(mlme_pib_attr_size_calc(pib_id, 0));
    ASSERT(mp_set_req->value != NULL);

    mp_set_req->pib_attribute.mlme_id = MAC_SHORT_ADDRESS;
    uint16_t address = CONFIG_DEVICE_SHORT_ADDRESS;
    memcpy(mp_set_req->value, &address, sizeof(uint16_t));
    mp_set_req->pib_attribute_idx = 0;
    mlme_set_req(mp_set_req, mlme_set_conf);
}

static void a_ext_addr_set(void * p_data)
{
    const pib_id_t pib_id =
    {
        .mlme_id = MAC_EXTENDED_ADDRESS,
    };
    mp_set_req = (mlme_set_req_t *)sys_mm_alloc(sizeof(mlme_set_req_t));
    ASSERT(mp_set_req != NULL);
    mp_set_req->value = (uint8_t *)sys_mm_alloc(mlme_pib_attr_size_calc(pib_id, 0));
    ASSERT(mp_set_req->value != NULL);

    mp_set_req->pib_attribute.mlme_id = MAC_EXTENDED_ADDRESS;

    // sum is for device address uniqueness
    uint64_t address = CONFIG_IEEE_ADDRESS + CONFIG_DEVICE_SHORT_ADDRESS;
    memcpy(mp_set_req->value, &address, sizeof(uint64_t));
    mp_set_req->pib_attribute_idx = 0;
    mlme_set_req(mp_set_req, mlme_set_conf);
}

static void a_pan_id_set(void * p_data)
{
    const pib_id_t pib_id =
    {
        .mlme_id = MAC_PAN_ID,
    };
    mp_set_req = (mlme_set_req_t *)sys_mm_alloc(sizeof(mlme_set_req_t));
    ASSERT(mp_set_req != NULL);
    mp_set_req->value = (uint8_t *)sys_mm_alloc(mlme_pib_attr_size_calc(pib_id, 0));
    ASSERT(mp_set_req->value != NULL);

    mp_set_req->pib_attribute.mlme_id = MAC_PAN_ID;

    uint16_t panid = CONFIG_PAN_ID;
    memcpy(mp_set_req->value, &panid, sizeof(uint16_t));
    mp_set_req->pib_attribute_idx = 0;
    mlme_set_req(mp_set_req, mlme_set_conf);
}

static void a_rx_on_idle_set(void * p_data)
{
    const pib_id_t pib_id =
    {
        .mlme_id = MAC_RX_ON_WHEN_IDLE,
    };
    mp_set_req = (mlme_set_req_t *)sys_mm_alloc(sizeof(mlme_set_req_t));
    ASSERT(mp_set_req != NULL);
    mp_set_req->value = (uint8_t *)sys_mm_alloc(mlme_pib_attr_size_calc(pib_id, 0));
    ASSERT(mp_set_req->value != NULL);

    mp_set_req->pib_attribute.mlme_id = MAC_RX_ON_WHEN_IDLE;
    uint8_t rx_on = 1;
    memcpy(mp_set_req->value, &rx_on, sizeof(uint8_t));
    mlme_set_req(mp_set_req, mlme_set_conf);
}

static void a_radio_tx_start(void * p_data)
{
    m_radio_tx_idle = false;

    const hal_uart_descriptor_t * p_descr = uart_descr_get();
    size_t sz = hal_uart_read_buffer_size_get(p_descr);

    LEDS_OFF(BIT(CONFIG_UPSTREAM_PIN));

    if (sz > MAX_MSDU_SIZE)
    {
        sz = MAX_MSDU_SIZE;
    }

    if (sz > 0)
    {
        hal_uart_read(p_descr, &m_radio_tx_buffer[PAYLOAD_START_POSITION], sz);
        memcpy(&m_radio_tx_buffer[COUNTER_POSITION], &tx_sequence_number, MAX_APP_SEQUENCE_NUMBER_SIZE);
#if (CONFIG_SECURE == 1)
        memcpy(m_radio_tx_buffer_shadow, &m_radio_tx_buffer[COUNTER_POSITION],
                                            sz + MAX_APP_SEQUENCE_NUMBER_SIZE);
#endif

        m_data_req.dst_addr_mode = MAC_ADDR_SHORT;
        m_data_req.dst_addr.short_address = CONFIG_OTHER_ADDRESS;
        m_data_req.dst_pan_id = CONFIG_PAN_ID;
        m_data_req.src_addr_mode = MAC_ADDR_SHORT;
        m_data_req.msdu = (uint8_t *)&m_radio_tx_buffer[MAC_MAX_MHR_SIZE];
        m_data_req.msdu_length = sz + MAX_APP_SEQUENCE_NUMBER_SIZE;
        m_data_req.msdu_handle++;
        m_data_req.tx_options.ack = true;
        m_data_req.tx_options.gts = false;
        m_data_req.tx_options.indirect = false;
#if (CONFIG_SECURE == 1)
        m_data_req.security_level = CONFIG_DATA_SECURITY_LEVEL;
        m_data_req.key_id_mode = 0;
#endif
        mcps_data_req(&m_data_req, mcps_data_conf);

        LEDS_ON(BIT(CONFIG_UPSTREAM_PIN));
    }
    else
    {
        m_radio_tx_idle = true;
    }
}

static void a_config_complete(void * p_data)
{
    LEDS_ON(BIT(CONFIG_INIT_DONE_PIN));
}

static void a_radio_tx_restart(void * p_data)
{
#if (CONFIG_SECURE == 1)
    /** This is necessary because data in m_radio_tx_buffer had been already ciphered. */
    memcpy(&m_radio_tx_buffer[COUNTER_POSITION], m_radio_tx_buffer_shadow, m_data_req.msdu_length);
#endif
    m_data_req.msdu_handle++;
    mcps_data_req(&m_data_req, mcps_data_conf);
    LEDS_ON(BIT(CONFIG_UPSTREAM_PIN));
}

static bool frames_available(void)
{
    return m_radio_rx_array_read_index != m_radio_rx_array_write_index;
}

static void frame_store(fsm_frame_item_t * p_frame)
{
    m_radio_rx_array[m_radio_rx_array_write_index++] = *p_frame;
    if (m_radio_rx_array_write_index == ARRAY_SIZE(m_radio_rx_array))
    {
        m_radio_rx_array_write_index = 0;
    }
    ASSERT(frames_available());
}

static fsm_frame_item_t * next_frame_get(void)
{
    ASSERT(frames_available());
    fsm_frame_item_t * result = &m_radio_rx_array[m_radio_rx_array_read_index++];
    if (m_radio_rx_array_read_index == ARRAY_SIZE(m_radio_rx_array))
    {
        m_radio_rx_array_read_index = 0;
    }
    return result;
}

static void a_uart_tx_start(void * p_data)
{
    ASSERT(frames_available());

    fsm_frame_item_t * p_fsm_frame = next_frame_get();
    const hal_uart_descriptor_t * p_uart_descr = uart_descr_get();

    hal_uart_write(p_uart_descr,
                   p_fsm_frame->payload_descr.p_payload + MAX_APP_SEQUENCE_NUMBER_SIZE,
                   p_fsm_frame->length - MAX_APP_SEQUENCE_NUMBER_SIZE);

    mac_mem_msdu_free(&p_fsm_frame->payload_descr);

    m_uart_tx_idle = false;

    LEDS_ON(BIT(CONFIG_DOWNSTREAM_PIN));

    (void)p_data;
}

static void a_uart_tx_idle_set(void * p_data)
{
    m_uart_tx_idle = true;
}

#if (CONFIG_SECURE == 1)
static void a_security_set(void * p_data)
{
    security_tables_init();
    fsm_event_post(E_SECURITY_SET, NULL);
}
#endif


/* callback functions */

static void mlme_set_conf(mlme_set_conf_t * conf)
{
    fsm_event_data_t data =
    {
        .mlme_set = conf,
    };
    fsm_event_post(E_SET_DONE, &data);

    sys_mm_free(mp_set_req->value);
    sys_mm_free(mp_set_req);
}

static void mcps_data_conf(mcps_data_conf_t * conf)
{
    fsm_event_data_t data =
    {
        .mcps_data_conf = conf
    };
    if (conf->status == MAC_SUCCESS)
    {
        tx_sequence_number++;
    }
    fsm_event_post(E_RADIO_TX_DONE, &data);
    LEDS_OFF(BIT(CONFIG_UPSTREAM_PIN));
}


/* storing functions */

static void mlme_set_conf_store(fsm_event_data_t * p_data)
{
    if (p_data != NULL)
    {
        m_set_conf = *p_data->mlme_set;
    }
}

static void mcps_data_ind_store(fsm_event_data_t * p_data)
{
    mcps_data_ind_t * p_frame = p_data->mcps_data_ind;
    fsm_frame_item_t fsm_frame;

    fsm_frame.payload_descr = p_frame->msdu;
    fsm_frame.length        = p_frame->msdu_length;
    frame_store(&fsm_frame);
}

static void mcps_data_conf_store(fsm_event_data_t * p_data)
{
    m_data_conf = *p_data->mcps_data_conf;
}

/* interface functions */

void fsm_init(void)
{
    sys_fsm_init(&m_fsm, &m_fsm_descriptor);
    m_radio_rx_array_read_index = 0;
    m_radio_rx_array_write_index = 0;
}

void fsm_event_post(fsm_events_t event, fsm_event_data_t * p_data)
{
    m_events |= BIT(event);
    if (event_data_store[event] != NULL)
    {
        event_data_store[event](p_data);
    }

    sys_task_post(APP_TASK_ID);
}

void fsm_event_scheduler_run(void)
{
    for (uint8_t event_id = 0; event_id < EVENTS_AMOUNT; event_id++)
    {
        uint8_t mask = BIT(event_id);
        if (m_events & mask)
        {
            m_events ^= mask;
            sys_fsm_event_post(&m_fsm, (fsm_events_t)event_id, NULL);
        }
    }

    if (m_events)
    {
        sys_task_post(APP_TASK_ID);
    }
}

