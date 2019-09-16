/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
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

#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_CLI_RTT)
#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>
#include "nrf_cli_rtt.h"
#include "nrf_assert.h"

static void timer_handler(void * p_context)
{
    nrf_cli_rtt_internal_t * p_internal = (nrf_cli_rtt_internal_t *)p_context;
    if (SEGGER_RTT_HasData(0))
    {
        p_internal->p_cb->handler(NRF_CLI_TRANSPORT_EVT_RX_RDY, p_internal->p_cb->p_context);
    }
    p_internal->p_cb->handler(NRF_CLI_TRANSPORT_EVT_TX_RDY, p_internal->p_cb->p_context);

    ret_code_t err_code = app_timer_start(*p_internal->p_timer, APP_TIMER_TICKS(10), p_context);
    ASSERT(err_code == NRF_SUCCESS);
    UNUSED_VARIABLE(err_code);
}

static ret_code_t cli_rtt_init(nrf_cli_transport_t const * p_transport,
                               void const *                p_config,
                               nrf_cli_transport_handler_t evt_handler,
                               void *                      p_context)
{
    UNUSED_PARAMETER(p_config);

    nrf_cli_rtt_internal_t * p_internal =
                                   CONTAINER_OF(p_transport, nrf_cli_rtt_internal_t, transport);
    p_internal->p_cb->handler = evt_handler;
    p_internal->p_cb->p_context = p_context;
    p_internal->p_cb->timer_created = false;

    SEGGER_RTT_Init();

    return NRF_SUCCESS;
}

static ret_code_t cli_rtt_uninit(nrf_cli_transport_t const * p_transport)
{
    nrf_cli_rtt_internal_t * p_internal =
                                       CONTAINER_OF(p_transport, nrf_cli_rtt_internal_t, transport);

    return app_timer_stop(*p_internal->p_timer);
}

static ret_code_t cli_rtt_enable(nrf_cli_transport_t const * p_transport,
                                 bool                        blocking)
{
    nrf_cli_rtt_internal_t * p_internal =
                                       CONTAINER_OF(p_transport, nrf_cli_rtt_internal_t, transport);
    ret_code_t err_code = NRF_SUCCESS;

    if (p_internal->p_cb->timer_created)
    {
        err_code = app_timer_stop(*p_internal->p_timer); //Timer may be running or inactive
        if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE))
        {
            return err_code;
        }
        else
        {
            err_code = NRF_SUCCESS;
        }
    }
    if (!blocking)
    {
        if (!p_internal->p_cb->timer_created)
        {
            err_code = app_timer_create(p_internal->p_timer,
                                        APP_TIMER_MODE_SINGLE_SHOT,
                                        timer_handler);
            p_internal->p_cb->timer_created = true;
        }
        if (err_code == NRF_SUCCESS)
        {
            err_code = app_timer_start(*p_internal->p_timer, APP_TIMER_TICKS(10), p_internal);
            SEGGER_RTT_Init();
        }
    }
    return err_code;
}
static ret_code_t cli_rtt_read(nrf_cli_transport_t const * p_transport,
                               void *                      p_data,
                               size_t                      length,
                               size_t *                    p_cnt)
{
    ASSERT(p_cnt);
    UNUSED_PARAMETER(p_transport);

    size_t rcnt = SEGGER_RTT_Read(NRF_CLI_RTT_TERMINAL_ID, p_data, length);
    *p_cnt = rcnt;

    return NRF_SUCCESS;
}

static ret_code_t cli_rtt_write(nrf_cli_transport_t const * p_transport,
                                const void *                p_data,
                                size_t                      length,
                                size_t *                    p_cnt)
{
    ASSERT(p_cnt);
    UNUSED_PARAMETER(p_transport);

    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk))
    {
        /* If RTT session is not active but RTT console is processed program may stuck.
         * Workaround: If debugger is not connected always return NRF_SUCCESS
         */
         *p_cnt = length;
        return NRF_SUCCESS;
    }

    size_t wcnt;
    size_t acc = 0;
    do {
        wcnt = SEGGER_RTT_Write(NRF_CLI_RTT_TERMINAL_ID, p_data, length);

        acc += wcnt;
        length -= wcnt;
    } while (length);

    if (p_cnt)
    {
        *p_cnt = acc;
    }
    return NRF_SUCCESS;
}

const nrf_cli_transport_api_t nrf_cli_rtt_transport_api = {
        .init   = cli_rtt_init,
        .uninit = cli_rtt_uninit,
        .enable = cli_rtt_enable,
        .read   = cli_rtt_read,
        .write  = cli_rtt_write,
};

#endif
