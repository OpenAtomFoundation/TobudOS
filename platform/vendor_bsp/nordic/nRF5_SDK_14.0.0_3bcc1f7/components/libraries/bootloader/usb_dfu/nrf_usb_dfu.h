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
/**@file
 *
 * @defgroup nrf_usb_dfu USB DFU Transport layer
 * @{
 * @ingroup  sdk_nrf_bootloader
 * @brief    Device Firmware Update (DFU) transport layer using USB.
 *
 * @details  The transport layer can be used for performing firmware updates over USB.
 *           The implementation uses SLIP to encode packets.
 */

#ifndef NRF_USB_DFU_H__
#define NRF_USB_DFU_H__

#include <stdint.h>
#include "slip.h"
#include "nrf_drv_usbd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SECURE_DFU_USB_MTU  64

/**@brief   USB DFU opcodes.
 */
typedef enum
{
    USB_DFU_OP_CODE_CREATE_OBJECT     = 0x01,  /**< Value of the opcode field for a 'Create object' request. */
    USB_DFU_OP_CODE_SET_RECEIPT_NOTIF = 0x02,  /**< Value of the opcode field for a 'Set Packet Receipt Notification' request. */
    USB_DFU_OP_CODE_CALCULATE_CRC     = 0x03,  /**< Value of the opcode field for a 'Calculating checksum' request. */
    USB_DFU_OP_CODE_EXECUTE_OBJECT    = 0x04,  /**< Value of the opcode field for an 'Initialize DFU parameters' request. */
    USB_DFU_OP_CODE_SELECT_OBJECT     = 0x06,  /**< Value of the opcode field for a 'Select object' request. */
    USB_DFU_OP_CODE_GET_SERIAL_MTU    = 0x07,  /**< Value of the opcode field for a 'Get Serial MTU' request. */
    USB_DFU_OP_CODE_WRITE_OBJECT      = 0x08,  /**< Value of the opcode indicating a write to the current object. */
    USB_DFU_OP_CODE_RESPONSE          = 0x60   /**< Value of the opcode field for a response.*/
} usb_dfu_op_code_t;

/**@brief   DFU Service.
 *
 * @details This structure contains status information related to the service.
 */
typedef struct
{
    slip_t     slip;

    uint8_t    usb_buffer[NRF_DRV_USBD_EPSIZE];
    uint8_t    recv_buffer[2*SECURE_DFU_USB_MTU + 1];

    uint16_t   pkt_notif_target;
    uint16_t   pkt_notif_target_count;
} usb_dfu_t;


/**@brief      Function for initializing the transport layer.
 *
 * @retval     NRF_SUCCESS If the transport layer was successfully initialized. Otherwise, an error code is returned.
 */
uint32_t usb_dfu_transport_init(void);


/**@brief      Function for closing down the transport layer.
 *
 * @retval     NRF_SUCCESS If the transport layer was correctly closed down.
 */
uint32_t usb_dfu_transport_close(void);

#ifdef __cplusplus
}
#endif

#endif // NRF_USB_DFU_H__

/** @} */
