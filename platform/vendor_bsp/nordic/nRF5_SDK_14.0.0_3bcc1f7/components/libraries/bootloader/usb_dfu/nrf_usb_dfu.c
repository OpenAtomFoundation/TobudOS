/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
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

#include "nrf_usb_dfu.h"

#include <string.h>
#include "boards.h"
#include "nrf_dfu_req_handler.h"
#include "nrf_dfu_transport.h"
#include "slip.h"

#include "nrf_gpio.h"
#include "nrf_drv_power.h"
#include "nrf_drv_usbd.h"

#include "app_scheduler.h"
#include "app_timer.h"
#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_core.h"
#include "app_usbd_string_desc.h"
#include "app_util_platform.h"

#include "nrf_log.h"


#define CREATE_OBJECT_REQUEST_LEN       (sizeof(uint8_t) + sizeof(uint32_t))
#define SET_RECEIPT_NOTIF_REQUEST_LEN   (sizeof(uint16_t))
#define CALCULATE_CRC_REQUEST_LEN       0
#define EXECUTE_OBJECT_REQUEST_LEN      0
#define SELECT_OBJECT_REQUEST_LEN       (sizeof(uint8_t))
#define GET_SERIAL_MTU_REQUEST_LEN      0
#define SERIAL_NUMBER_STRING_SIZE       (16)
#define MAX_RESPONSE_SIZE               (1 + 1 + 3 * 4)

#define CDC_ACM_COMM_INTERFACE          0
#define CDC_ACM_COMM_EPIN               NRF_DRV_USBD_EPIN2
#define CDC_ACM_DATA_INTERFACE          1
#define CDC_ACM_DATA_EPIN               NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT              NRF_DRV_USBD_EPOUT1

/**
 * @brief Enable power USB detection
 *
 * Configure if example supports USB port connection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

/**
 * @brief Interfaces list passed to @ref APP_USBD_CDC_ACM_GLOBAL_DEF
 * */
#define CDC_ACM_INTERFACES_CONFIG()                                                                \
    APP_USBD_CDC_ACM_CONFIG(CDC_ACM_COMM_INTERFACE,                                                \
                            CDC_ACM_COMM_EPIN,                                                     \
                            CDC_ACM_DATA_INTERFACE,                                                \
                            CDC_ACM_DATA_EPIN,                                                     \
                            CDC_ACM_DATA_EPOUT)


ANON_UNIONS_ENABLE
typedef struct
{
    uint8_t            op_code;
    nrf_dfu_res_code_t resp_val;
    union {
        struct
        {
            uint32_t offset;
            uint32_t crc;
        } crc_response;
        struct
        {
            uint32_t max_size;
            uint32_t offset;
            uint32_t crc;
        } select_response;
        struct
        {
            uint16_t mtu;
        } serial_mtu_response;
    };

} usb_dfu_response_t;
ANON_UNIONS_DISABLE


static usb_dfu_t m_dfu;
uint16_t g_extern_serial_number[SERIAL_NUMBER_STRING_SIZE + 1];

DFU_TRANSPORT_REGISTER(nrf_dfu_transport_t const dfu_trans) =
{
    .init_func = usb_dfu_transport_init,
    .close_func = usb_dfu_transport_close
};

/*lint -save -e26 -e64 -e505 -e651 */
static const uint8_t m_cdc_acm_class_descriptors[] =
{
    APP_USBD_CDC_ACM_DEFAULT_DESC(CDC_ACM_COMM_INTERFACE,
                                  CDC_ACM_COMM_EPIN,
                                  CDC_ACM_DATA_INTERFACE,
                                  CDC_ACM_DATA_EPIN,
                                  CDC_ACM_DATA_EPOUT)
};

static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);

/**@brief CDC_ACM class instance. */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            CDC_ACM_INTERFACES_CONFIG(),
                            cdc_acm_user_ev_handler,
                            m_cdc_acm_class_descriptors);
/*lint -restore */


static void serial_number_string_create(void)
{
    g_extern_serial_number[0] =
        (uint16_t)APP_USBD_DESCRIPTOR_STRING << 8 | sizeof(g_extern_serial_number);

    uint32_t dev_id_hi = NRF_FICR->DEVICEID[1];
    uint32_t dev_id_lo = NRF_FICR->DEVICEID[0];

    uint64_t device_id = (((uint64_t)dev_id_hi) << 32) | dev_id_lo;

    for (size_t i = 1; i < SERIAL_NUMBER_STRING_SIZE + 1; ++i)
    {
        char tmp[2];
        (void)snprintf(tmp, sizeof(tmp), "%x", (unsigned int)(device_id & 0xF));
        g_extern_serial_number[i] = tmp[0];
        device_id >>= 4;
    }
}


static void response_send(usb_dfu_t * p_dfu, usb_dfu_response_t * p_response)
{
    // Should be statically allocated since the response is sent asynchronously.
    static uint8_t encoded_response[MAX_RESPONSE_SIZE * 2 + 1] = {0};
    uint8_t        response_buffer[MAX_RESPONSE_SIZE]          = {0};
    uint32_t       encoded_response_length                     = 0;
    uint16_t       index                                       = 0;
    ret_code_t     ret_code;

    NRF_LOG_DEBUG("Sending Response: [0x%01x, 0x%01x]", p_response->op_code, p_response->resp_val);

    response_buffer[index++] = USB_DFU_OP_CODE_RESPONSE;

    // Encode the Request Op code
    response_buffer[index++] = p_response->op_code;

    // Encode the Response Value.
    response_buffer[index++] = (uint8_t)p_response->resp_val;

    if (p_response->resp_val == NRF_DFU_RES_CODE_SUCCESS)
    {
        switch (p_response->op_code)
        {
            case USB_DFU_OP_CODE_CALCULATE_CRC:
            {
                index += uint32_encode(p_response->crc_response.offset, &response_buffer[index]);
                index += uint32_encode(p_response->crc_response.crc, &response_buffer[index]);
            } break;

            case USB_DFU_OP_CODE_SELECT_OBJECT:
            {
                index +=
                    uint32_encode(p_response->select_response.max_size, &response_buffer[index]);
                index += uint32_encode(p_response->select_response.offset, &response_buffer[index]);
                index += uint32_encode(p_response->select_response.crc, &response_buffer[index]);
            } break;

            case USB_DFU_OP_CODE_GET_SERIAL_MTU:
            {
                index +=
                    uint16_encode(p_response->serial_mtu_response.mtu, &response_buffer[index]);
            } break;

            default:
                break;
        }
    }
    else if (p_response->resp_val == NRF_DFU_RES_CODE_EXT_ERROR)
    {
        response_buffer[index++] = ext_error_get();
        // Clear the last extended error code
        (void)ext_error_set(NRF_DFU_EXT_ERROR_NO_ERROR);
    }

    // Encode into slip
    ret_code = slip_encode(encoded_response, response_buffer, index, &encoded_response_length);
    if (ret_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed to slip encode response");
    }

    // Send
    ret_code = app_usbd_cdc_acm_write(&m_app_cdc_acm, encoded_response, encoded_response_length);
    if (ret_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed send response");
    }
}


static void on_packet_received(usb_dfu_t * p_dfu)
{
    nrf_dfu_req_t dfu_req;
    nrf_dfu_res_t dfu_res = {{{0}}};

    memset(&dfu_req, 0x00, sizeof(dfu_req));

    usb_dfu_response_t serial_response;

    const usb_dfu_op_code_t op_code            = (usb_dfu_op_code_t)p_dfu->recv_buffer[0];
    const uint16_t          packet_payload_len = p_dfu->slip.current_index - 1;
    uint8_t *               p_payload          = &p_dfu->recv_buffer[1];

    serial_response.op_code = op_code;

    switch (op_code)
    {
        case USB_DFU_OP_CODE_CREATE_OBJECT:
        {
            NRF_LOG_DEBUG("Received create object");
            if (packet_payload_len != CREATE_OBJECT_REQUEST_LEN)
            {
                serial_response.resp_val = NRF_DFU_RES_CODE_INVALID_PARAMETER;
                break;
            }

            // Reset the packet receipt notification on create object
            p_dfu->pkt_notif_target_count = p_dfu->pkt_notif_target;
            dfu_req.obj_type              = p_payload[0];
            dfu_req.object_size           = uint32_decode(&p_payload[1]);
            dfu_req.req_type              = NRF_DFU_OBJECT_OP_CREATE;

            serial_response.resp_val = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
        } break;

        case USB_DFU_OP_CODE_SET_RECEIPT_NOTIF:
        {
            NRF_LOG_DEBUG("Set receipt notif");
            if (packet_payload_len != SET_RECEIPT_NOTIF_REQUEST_LEN)
            {
                serial_response.resp_val = NRF_DFU_RES_CODE_INVALID_PARAMETER;
                break;
            }

            p_dfu->pkt_notif_target       = uint16_decode(&p_payload[0]);
            p_dfu->pkt_notif_target_count = p_dfu->pkt_notif_target;

            serial_response.resp_val = NRF_DFU_RES_CODE_SUCCESS;
        } break;

        case USB_DFU_OP_CODE_CALCULATE_CRC:
        {
            NRF_LOG_DEBUG("Received calculate CRC");
            dfu_req.req_type = NRF_DFU_OBJECT_OP_CRC;

            serial_response.resp_val            = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            serial_response.crc_response.offset = dfu_res.offset;
            serial_response.crc_response.crc    = dfu_res.crc;
        } break;

        case USB_DFU_OP_CODE_EXECUTE_OBJECT:
        {
            NRF_LOG_DEBUG("Received execute object");
            dfu_req.req_type = NRF_DFU_OBJECT_OP_EXECUTE;

            serial_response.resp_val = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
        } break;

        case USB_DFU_OP_CODE_SELECT_OBJECT:
        {
            NRF_LOG_DEBUG("Received select object");
            if (packet_payload_len != SELECT_OBJECT_REQUEST_LEN)
            {
                serial_response.resp_val = NRF_DFU_RES_CODE_INVALID_PARAMETER;
                break;
            }

            // Set object type to read info about
            dfu_req.obj_type = p_payload[0];
            dfu_req.req_type = NRF_DFU_OBJECT_OP_SELECT;

            serial_response.resp_val = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            serial_response.select_response.max_size = dfu_res.max_size;
            serial_response.select_response.offset   = dfu_res.offset;
            serial_response.select_response.crc      = dfu_res.crc;
        } break;

        case USB_DFU_OP_CODE_GET_SERIAL_MTU:
        {
            NRF_LOG_DEBUG("Received get serial mtu");
            serial_response.serial_mtu_response.mtu = sizeof(p_dfu->recv_buffer);
            serial_response.resp_val                = NRF_DFU_RES_CODE_SUCCESS;
        } break;

        case USB_DFU_OP_CODE_WRITE_OBJECT:
        {
            dfu_req.req_type = NRF_DFU_OBJECT_OP_WRITE;
            dfu_req.p_req    = &p_payload[0];
            dfu_req.req_len  = packet_payload_len;

            serial_response.resp_val = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            if (serial_response.resp_val != NRF_DFU_RES_CODE_SUCCESS)
            {
                NRF_LOG_WARNING("Failure to run packet write");
            }

            // Check if a packet receipt notification is needed to be sent.
            if (p_dfu->pkt_notif_target != 0 && --p_dfu->pkt_notif_target_count == 0)
            {
                serial_response.op_code             = USB_DFU_OP_CODE_CALCULATE_CRC;
                serial_response.crc_response.offset = dfu_res.offset;
                serial_response.crc_response.crc    = dfu_res.crc;

                // Reset the counter for the number of firmware packets.
                p_dfu->pkt_notif_target_count = p_dfu->pkt_notif_target;

                response_send(p_dfu, &serial_response);
            }
        } break;

        default:
        {
            // Unsupported op code.
            NRF_LOG_WARNING("Received unsupported OP code");
            serial_response.resp_val = NRF_DFU_RES_CODE_INVALID_PARAMETER;
        } break;
    }

    if (op_code != USB_DFU_OP_CODE_WRITE_OBJECT)
    {
        response_send(p_dfu, &serial_response);
    }
}


static void on_rx_complete(usb_dfu_t * p_dfu, uint8_t * p_data, uint8_t len)
{
    ret_code_t ret_code;
    for (uint32_t i = 0; i < len; i++)
    {
        ret_code = slip_decode_add_byte(&p_dfu->slip, p_data[i]);
        if (ret_code == NRF_SUCCESS)
        {
            on_packet_received(p_dfu);

            // reset the slip decoding
            p_dfu->slip.current_index = 0;
            p_dfu->slip.state         = SLIP_STATE_DECODING;
        }
    }
}


/**
 * @brief User event handler @ref app_usbd_cdc_acm_user_ev_handler_t (headphones)
 * */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    ret_code_t                 ret_code;
    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            // Setup first transfer
            ret_code =
                app_usbd_cdc_acm_read(&m_app_cdc_acm, m_dfu.usb_buffer, sizeof(m_dfu.usb_buffer));
            ASSERT(ret_code == NRF_SUCCESS);
        } break;

        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
        {
            nrf_dfu_req_handler_reset_if_dfu_complete();
        } break;

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            on_rx_complete(
                &m_dfu, (uint8_t *)m_dfu.usb_buffer, app_usbd_cdc_acm_rx_size(p_cdc_acm));

            // Setup next transfer
            ret_code =
                app_usbd_cdc_acm_read(&m_app_cdc_acm, m_dfu.usb_buffer, sizeof(m_dfu.usb_buffer));

            ASSERT(ret_code == NRF_SUCCESS);
        } break;

        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
        {
            nrf_dfu_req_handler_reset_if_dfu_complete();
        } break;

        default:
            break;
    }
}


static void power_usb_event_handler(nrf_drv_power_usb_evt_t event)
{
    switch (event)
    {
        case NRF_DRV_POWER_USB_EVT_DETECTED:
        {
            NRF_LOG_INFO("USB power detected");
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }

            nrf_gpio_pin_set(BSP_LED_0);
            nrf_gpio_pin_clear(BSP_LED_1);
        } break;

        case NRF_DRV_POWER_USB_EVT_REMOVED:
        {
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            nrf_gpio_pin_set(BSP_LED_1);
            nrf_gpio_pin_clear(BSP_LED_0);
        } break;

        case NRF_DRV_POWER_USB_EVT_READY:
        {
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
        } break;

        default:
            ASSERT(false);
    }
}


static ret_code_t usb_start(void)
{

    if (USBD_POWER_DETECTION)
    {
        static const nrf_drv_power_usbevt_config_t config = {.handler = power_usb_event_handler};

        return nrf_drv_power_usbevt_init(&config);
    }
    else
    {
        NRF_LOG_DEBUG("No USB power detection enabled\r\nStarting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    return NRF_SUCCESS;
}


/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(BSP_LED_0);
    nrf_gpio_cfg_output(BSP_LED_1);
    nrf_gpio_pin_clear(BSP_LED_0);
    nrf_gpio_pin_set(BSP_LED_1);
}


static void usbd_dfu_transport_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;

        default:
            break;
    }
}


static void usbd_sched_event_handler(void * p_event_data, uint16_t event_size)
{
    app_usbd_event_execute(p_event_data);
}


static void usbd_event_handler(app_usbd_internal_evt_t const * const p_event)
{
    uint32_t ret;
    ret = app_sched_event_put(p_event, sizeof(app_usbd_internal_evt_t), usbd_sched_event_handler);
    ASSERT(NRF_SUCCESS == ret);
}


uint32_t usb_dfu_transport_init(void)
{
    /* Execute event directly in interrupt handler */
    static const app_usbd_config_t usbd_config =
    {
        .ev_handler    = usbd_event_handler,
        .ev_state_proc = usbd_dfu_transport_ev_handler
    };

    uint32_t err_code;

    m_dfu.slip.p_buffer      = m_dfu.recv_buffer;
    m_dfu.slip.current_index = 0;
    m_dfu.slip.buffer_len    = sizeof(m_dfu.recv_buffer);
    m_dfu.slip.state         = SLIP_STATE_DECODING;

    leds_init();

    NRF_LOG_DEBUG("Initializing power driver.");

    err_code = nrf_drv_power_init(NULL);
    VERIFY_SUCCESS(err_code);

    serial_number_string_create();

    err_code = app_usbd_init(&usbd_config);
    VERIFY_SUCCESS(err_code);

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    err_code                                    = app_usbd_class_append(class_cdc_acm);
    VERIFY_SUCCESS(err_code);

    NRF_LOG_DEBUG("Starting USB");

    err_code = usb_start();
    VERIFY_SUCCESS(err_code);

    NRF_LOG_DEBUG("USB Transport initialized");

    return err_code;
}


uint32_t usb_dfu_transport_close(void)
{
    return NRF_SUCCESS;
}
