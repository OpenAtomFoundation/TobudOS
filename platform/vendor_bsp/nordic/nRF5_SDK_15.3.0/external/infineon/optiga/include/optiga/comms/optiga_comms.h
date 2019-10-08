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
* \file
*
* \brief This file implements optiga comms abstraction layer for IFX I2C Protocol.
*
* \addtogroup  grOptigaComms
* @{
*/

#ifndef _OPTIGA_COMMS_H_
#define _OPTIGA_COMMS_H_
/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/common/Datatypes.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/// Succesfull execution
#define OPTIGA_COMMS_SUCCESS        0x0000
/// Error in execution
#define OPTIGA_COMMS_ERROR          0x0001
/// Busy, doing operation
#define OPTIGA_COMMS_BUSY           0x0002

/**********************************************************************************************************************
 * DATA STRUCTURES
 *********************************************************************************************************************/

/** @brief optiga comms structure */
typedef struct optiga_comms
{
    /// Comms structure pointer
    void* comms_ctx;
    /// Upper layer contect
    void* upper_layer_ctx;
    /// Upper layer handler
    app_event_handler_t upper_layer_handler;
    /// Optiga comms state
    uint8_t state;
}optiga_comms_t;

extern optiga_comms_t optiga_comms;

/**********************************************************************************************************************
 * API Prototypes
 *********************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Opens the communication channel with OPTIGA.
 */
LIBRARY_EXPORTS host_lib_status_t optiga_comms_open(optiga_comms_t *p_ctx);

/**
 * \brief   Resets the OPTIGA.
 */
LIBRARY_EXPORTS host_lib_status_t optiga_comms_reset(optiga_comms_t *p_ctx,uint8_t reset_type);

/**
 * \brief   Sends and receives the APDU.
 */
LIBRARY_EXPORTS host_lib_status_t optiga_comms_transceive(optiga_comms_t *p_ctx,const uint8_t* p_data,
                                                          const uint16_t* p_data_length,
                                                          uint8_t* p_buffer, uint16_t* p_buffer_len);

/**
 * \brief   Closes the communication channel with OPTIGA.
 */
LIBRARY_EXPORTS host_lib_status_t optiga_comms_close(optiga_comms_t *p_ctx);

/**
* @}
*/

#endif /*_OPTIGA_COMMS_H_*/

