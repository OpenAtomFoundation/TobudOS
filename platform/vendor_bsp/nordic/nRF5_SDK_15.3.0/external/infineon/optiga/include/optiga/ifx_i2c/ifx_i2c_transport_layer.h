/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file ifx_i2c_transport_layer.h
*
* \brief   This file defines the API prototype for transport layer of the Infineon I2C Protocol Stack library.
*
* \addtogroup  grIFXI2C
* @{
*/

#ifndef _IFX_I2C_TRANSPORT_LAYER_H_
#define _IFX_I2C_TRANSPORT_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
* HEADER FILES
**********************************************************************************************************************/
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "optiga/pal/pal_os_timer.h"

/***********************************************************************************************************************
* MACROS
**********************************************************************************************************************/
/***********************************************************************************************************************
* ENUMS
**********************************************************************************************************************/
/***********************************************************************************************************************
* DATA STRUCTURES
***********************************************************************************************************************/
/***********************************************************************************************************************
* API PROTOTYPES
**********************************************************************************************************************/
/**
 * @brief Function for initializing the module.
 *
 * Function initializes and enables the module and registers
 * an event handler to receive events from this module.
 * @attention This function must be called before using the module.
 *
 * @param[in,out] p_ctx     Pointer to ifx i2c context.
 * @param[in] handler     Function pointer to the event handler of the upper layer.
 *
 * @retval  IFX_I2C_STACK_SUCCESS If initialization was successful.
 * @retval  IFX_I2C_STACK_ERROR If the module is already initialized.
 */
host_lib_status_t ifx_i2c_tl_init(ifx_i2c_context_t *p_ctx,ifx_i2c_event_handler_t handler);

/**
 * @brief Function to transmit and receive a packet.
 *
 * Asynchronous function to send and receive a packet.
 * The function returns immediately. One of the following events is
 * propagated to the event handler registered with @ref ifx_i2c_tl_init
 *
 * @param[in,out] p_ctx     Pointer to ifx i2c context.
 * @param[in] p_packet             Buffer containing the packet header.
 * @param[in] packet_len           Packet header length.
 * @param[in] p_recv_packet        Buffer containing the packet payload.
 * @param[in] recv_packet_len      Packet payload length.
 *
 * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
 * @retval  IFX_I2C_STACK_ERROR If the module is busy.
 */
host_lib_status_t ifx_i2c_tl_transceive(ifx_i2c_context_t *p_ctx,uint8_t* p_packet, uint16_t packet_len,
                               uint8_t* p_recv_packet, uint16_t* recv_packet_len);

/**
 * @}
 **/
#ifdef __cplusplus
}
#endif
#endif /* IFX_I2C_TRANSPORT_LAYER_H__ */
