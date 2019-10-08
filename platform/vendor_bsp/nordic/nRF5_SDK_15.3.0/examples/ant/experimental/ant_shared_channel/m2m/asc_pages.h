/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
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
 * @defgroup Two Byte Shared Address Page Manager
 * @{
 * @ingroup ASC Page Manager
 *
 * @brief Module to handle encoding/decoding of shared address pages using two byte addressing
 */

#ifndef ASC_PAGES_H__
#define ASC_PAGES_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup asc_pages/Pages
 *
 * @brief Describes each handshaking page involved in the auto shared channel
 *
 * @{
 */
#define SET_SLAVE_ADDRESS_PID               ((uint8_t) 0)       /**< */
#define COMMAND_PID                         ((uint8_t) 1)       /**< */
#define DEVICE_AVAILABILITY_PID             ((uint8_t) 2)       /**< */
#define UPDATE_DATA_PID                     ((uint8_t) 3)       /**< */
#define PHONE_COMMAND_PID                   ((uint8_t) 4)       /**< */
#define ADDRESS_AVAILABLE_PID               ((uint8_t) 32)      /**< */
#define REQUEST_ADDRESS_PID                 ((uint8_t) 33)      /**< */
#define BUSY_ACQUIRING_PID                  ((uint8_t) 34)      /**< */
#define CONFIRM_ACQUIRE_PID                 ((uint8_t) 35)      /**< */
#define REQUEST_DATA_PID                    ((uint8_t) 240)     /**< */

#define ADDRESS_AVAILABLE_MASK              ((uint8_t) 0x01)    /**< */
#define DATA_TIMEOUT_DISABLED               ((uint8_t) 0xFF)    /**< */
#define INVALID_BACKOFF_RANGE               ((uint8_t) 0xFF)    /**< */
#define INVALID_NEIGHBOUR_ID                ((uint16_t) 0x0000) /**< */
#define EVERY_MASTER_ID                     ((uint16_t) 0x0000) /**< */
#define RESERVED_ONES                       ((uint8_t) 0xFF)    /**< */
#define RESERVED_ZEROS                      ((uint8_t) 0x00)    /**< */

#define DECODE_PAGE_ID_BYTE_PHONE_FORMAT    ANT_MESSAGE_DATA0_OFFSET  /**< */
/** @} */


/**
 * @defgroup asc_pages/Handshaking Addresses
 *
 * @brief Describes the shared addresses used in the handshaking process
 *
 * @{
 */
#ifdef TWO_BYTE_SHARED_ADDRESS
    #define EVERY_DEVICE_ADDRESS                ((uint16_t) 0x0000)  /**< */
    #define SEARCH_ADDRESS                      ((uint16_t) 0xFFFF)  /**< */
    #define REGISTER_ADDRESS                    ((uint16_t) 0xFFFE)  /**< */
    #define MIN_SLAVE_ADDRESS                   ((uint16_t) 0x0001)  /**< */
    #define MAX_SLAVE_ADDRESS                   ((uint16_t) 0xFFFD)  /**< */
#else
    #define EVERY_DEVICE_ADDRESS                ((uint8_t) 0x00)  /**< */
    #define SEARCH_ADDRESS                      ((uint8_t) 0xFF)  /**< */
    #define REGISTER_ADDRESS                    ((uint8_t) 0xFE)  /**< */
    #define MIN_SLAVE_ADDRESS                   ((uint8_t) 0x01)  /**< */
    #define MAX_SLAVE_ADDRESS                   ((uint8_t) 0xFD)  /**< */
#endif
/** @} */


/**
 * @defgroup asc_pages Shared Page Format
 *
 * @brief Describes the location of the shared address and the page ID within an ASC ANT Message
 *
 * @{
 */

#define ENCODE_PHONE_PAGE_ID_BYTE                  ((uint8_t) 0x00)  /**< */
#define ENCODE_PHONE_SHARED_ADDRESS_BYTE1          ((uint8_t) 0x01)  /**< */
#define ENCODE_PHONE_SHARED_ADDRESS_BYTE2          ((uint8_t) 0x02)  /**< */
#define DECODE_PHONE_PAGE_ID_BYTE                  ((uint8_t) ANT_MESSAGE_DATA0_OFFSET)  /**< */
#define DECODE_PHONE_SHARED_ADDRESS_BYTE1          ((uint8_t) ANT_MESSAGE_DATA1_OFFSET)  /**< */
#define DECODE_PHONE_SHARED_ADDRESS_BYTE2          ((uint8_t) ANT_MESSAGE_DATA2_OFFSET)  /**< */

#ifdef TWO_BYTE_SHARED_ADDRESS
    #define ENCODE_SHARED_ADDRESS_BYTE1            ((uint8_t) 0x00)  /**< */
    #define ENCODE_SHARED_ADDRESS_BYTE2            ((uint8_t) 0x01)  /**< */
    #define ENCODE_PAGE_ID_BYTE                    ((uint8_t) 0x02)  /**< */
    #define DECODE_SHARED_ADDRESS_BYTE1            ANT_MESSAGE_DATA0_OFFSET  /**< */
    #define DECODE_SHARED_ADDRESS_BYTE2            ANT_MESSAGE_DATA1_OFFSET  /**< */
    #define DECODE_PAGE_ID_BYTE                    ANT_MESSAGE_DATA2_OFFSET  /**< */

    #define SHARED_FORMAT_DATA_PAYLOAD_SIZE        ((uint8_t) 0x06)  /**< */
#else
    #define ENCODE_SHARED_ADDRESS_BYTE             ((uint8_t) 0x00)  /**< */
    #define ENCODE_PAGE_ID_BYTE                    ((uint8_t) 0x01)  /**< */
    #define DECODE_SHARED_ADDRESS_BYTE             ANT_MESSAGE_DATA0_OFFSET  /**< */
    #define DECODE_PAGE_ID_BYTE                    ANT_MESSAGE_DATA1_OFFSET  /**< */

    #define SHARED_FORMAT_DATA_PAYLOAD_SIZE        ((uint8_t) 0x07)  /**< */
#endif
/** @} */


/**
 * @defgroup asc_pages Group Definitions
 *
 * @brief Describes special values to use for assigning/addressing groups
 *
 * @{
 */

#define GROUP_SET                                ((uint16_t) 1)
#define NO_GROUPS                                ((uint16_t) 0)
#define ALL_GROUPS                               ((uint16_t) 0x7FFF)

/** @} */


typedef enum
{
    MSG_PERIOD_0_5HZ = 0,
    MSG_PERIOD_2HZ,
    MSG_PERIOD_4HZ,
    MSG_PERIOD_8HZ,
    MSG_PERIOD_12HZ,
    MSG_PERIOD_16HZ,
    MSG_PERIOD_32HZ,
    MSG_PERIOD_64HZ
} asc_message_periods_t;


typedef enum
{
    TURN_OFF                    = 0,
    TURN_ON                     = 1,
    ASSIGN_TO_GROUP             = 2,
    ASSIGN_TO_ADDITIONAL_GROUP  = 3,
    ASSIGN_TO_ALL_GROUPS        = 4,
    CLEAR_FROM_GROUP            = 5,
    CLEAR_FROM_ALL_GROUPS       = 6,
    REPORTING_MODE_OFF          = 7,
    REPORTING_MODE_ON           = 8,
} asc_commands_t;


typedef struct
{
    asc_commands_t  command;
    #ifdef TWO_BYTE_SHARED_ADDRESS
    uint16_t        shared_address;
    #else
    uint8_t         shared_address;
    #endif
    uint16_t        master_id;
    uint8_t         group_number;
} asc_command_data_t;


/**@brief Enumeration defining the possible states of the ASC Slave's light.
 */
typedef enum
{
    SLAVE_STATE_OFF = 0,      /**< ASC Slave state: OFF. The ASC Slave's main functionality is off. */
    SLAVE_STATE_ON            /**< ASC Slave state: ON. The ASC Slave's main functionality is on. */
} asc_slave_states_t;


typedef struct
{
#ifdef TWO_BYTE_SHARED_ADDRESS
    uint16_t            shared_address;
#else
    uint8_t             shared_address;
#endif
    uint16_t            master_id;
    asc_slave_states_t  state;
} asc_update_data_t;


typedef struct
{
#ifdef TWO_BYTE_SHARED_ADDRESS
    uint16_t            shared_address;
#else
    uint8_t             shared_address;
#endif
    uint8_t     page_id_requested;
} asc_request_data_t;


typedef struct
{
    asc_message_periods_t   channel_period;
    uint8_t                 backoff_range;
    uint8_t                 data_timeout;
    bool                    is_address_available;
}asc_addr_available_paramters_t;


typedef struct
{
    uint32_t    serial_number;
    uint16_t    model_number;
    uint8_t     hw_revision;
    uint8_t     sw_revision;
}asc_confirm_acquire_paramters_t;


/**@brief Encodes and transmits the Set Shared Address page with specified shared address.
 *
 * @param[in]  shared_address       The shared address to configure the slave to use.
 *
 * @param[out] p_rx_buffer          The buffer where the Set Shared Address page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
 #ifdef TWO_BYTE_SHARED_ADDRESS
void asc_encode_set_shared_address(uint16_t shared_address, uint8_t * p_tx_buffer);
#else
void asc_encode_set_shared_address(uint8_t shared_address, uint8_t * p_tx_buffer);
#endif


/**@brief Encodes the Address Available page.
 *
 * @param[in]  params               The parameters to use when encoding the Address Available page.
 *
 * @param[out] p_rx_buffer          The buffer where the Address Available page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_address_available_page(asc_addr_available_paramters_t params, uint8_t * p_tx_buffer);


/**@brief Encodes the Request Address page as an acknowledged message.
 *
 * @param[in]  serial_number        The serial number of the slave device requesting an address.
 *
 * @param[out] p_rx_buffer          The buffer where the Request Address page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_request_address_page(uint32_t serial_num, uint8_t * p_tx_buffer);


/**@brief Encodes the Busy Acquiring page.
 *
 * @param[in]  next_address         The next address available to the requesting slave device.
 *
 * @param[in]  serial_number        The serial number of the slave device that requested an address.
 *
 * @param[out] p_rx_buffer          The buffer where the Busy Acquiring page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
 #ifdef TWO_BYTE_SHARED_ADDRESS
void asc_encode_busy_acquiring_page(uint16_t next_address, uint32_t serial_num, uint8_t * p_tx_buffer);
#else
void asc_encode_busy_acquiring_page(uint8_t next_address, uint32_t serial_num, uint8_t * p_tx_buffer);
#endif


/**@brief Encodes the Confirm Acquire page.
 *
 * @param[in]  params               The parameters to use when encoding the Confirm Acquire page.
 *
 * @param[out] p_rx_buffer          The buffer where the Command page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_confirm_acquire_page(asc_confirm_acquire_paramters_t params, uint8_t * p_tx_buffer);


/**@brief Encodes the Command page.
 *
 * @param[in] command_data          Contains the shared address and command that is to be loaded into the message buffer.
 *
 * @param[out] p_rx_buffer          The buffer where the Command page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_command_page(asc_command_data_t command_data, uint8_t * p_tx_buffer);


/**@brief Encodes the Phone Command page.
 *
 * @param[in] command_data          Contains the shared address and command that is to be loaded into the message buffer.
 *
 * @param[out] p_rx_buffer          The buffer where the Phone Command page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_phone_command_page(asc_command_data_t command_data, uint8_t * p_tx_buffer);


/**@brief Encodes the Update Data page.
 *
 * @param[in]  update_data          The data used to populate the transmission buffer.
 *
 * @param[out] p_rx_buffer          The buffer where the Update Data page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_update_data_page(asc_update_data_t update_data, uint8_t * p_tx_buffer);


/**@brief Encodes the Update Data page in its phone connection format.
 *
 * @param[in]  update_data          The data used to populate the transmission buffer.
 *
 * @param[out] p_rx_buffer          The buffer where the Update Data page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_phone_update_data_page(asc_update_data_t update_data, uint8_t * p_tx_buffer);


/**@brief Encodes the Request Data page.
 *
 * @param[in]  request_data         The data used to populate the transmission buffer.
 *
 * @param[out] p_rx_buffer          The buffer where the Request data page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_request_data_page(asc_request_data_t request_data,  uint8_t * p_tx_buffer);


/**@brief Encodes the Device Availability page.
 *
 * @param[in]  neighbor_id          The device number of the neighbor master that is currently connected.
 *
 * @param[out] p_rx_buffer          The buffer where the Device Availability page will be copied.
 *                                  The array size must be at least MESG_BUFFER_SIZE to
 *                                  accommadate the entire ANT_MESSAGE structure size.
 *                                  See ANT Message Structure in ant_parameters.h.
 */
void asc_encode_device_availability_page(uint16_t neighbor_id, uint8_t * p_tx_buffer);


/**@brief Decodes the Address Available page.
 *
 * @param[out] p_params             The parameters struct to populate from the rx buffer.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not an Address Available page.
 */
uint32_t asc_decode_address_available_page(asc_addr_available_paramters_t * p_params, uint8_t * p_rx_buffer);


/**@brief Decodes the Request Address page as an acknowledged message.
 *
 * @param[out] serial_number        The pointer to a uint where the received serial number from the ANT message will be copied to.
 *
 * @param[in] p_rx_buffer           The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Request Address page.
 */
uint32_t asc_decode_request_address_page(uint32_t * serial_number, uint8_t * p_rx_buffer);


/**@brief Decodes the Busy Acquiring page.
 *
 * @param[in]  serial_number        The serial number of the slave device requesting an address.
 *
 * @param[out] p_is_serial_match    The pointer to a bool where the result of the serial number comparison will be copied.
 *
 * @param[out] next_address         The pointer to a uchar where the decoded address will be copied to if is_serial_match is true.
 *                                  The value will not have been changed if p_is_serial_match is false.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Busy Acquiring page.
 */
#ifdef TWO_BYTE_SHARED_ADDRESS
uint32_t asc_decode_busy_acquiring_page(uint32_t serial_number,
                                        bool * p_is_serial_match,
                                        uint16_t * next_address,
                                        uint8_t * p_rx_buffer);
#else
uint32_t asc_decode_busy_acquiring_page(uint32_t serial_number,
                                        bool * p_is_serial_match,
                                        uint8_t * next_address,
                                        uint8_t * p_rx_buffer);
#endif


/**@brief Decodes the Confirm Acquire page.
 *
 * @param[in]  serial_number        The serial number of the slave device that send the confirmation.
 *
 * @param[out] p_is_serial_match    The pointer to a bool where the result of the serial number comparison will be copied.
 *
 * @param[out] p_params             The parameters struct to populate from the rx buffer.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Confirm Acquire page.
 */
uint32_t asc_decode_confirm_acquire_page(uint32_t serial_number,
                                         bool * p_is_serial_match,
                                         asc_confirm_acquire_paramters_t * p_params,
                                         uint8_t * p_rx_buffer);


/**@brief Decodes the Command page.
 *
 * @param[out] p_command_data       The pointer where the received ASC command data is to be copied.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Command page.
 */
uint32_t asc_decode_command_page(asc_command_data_t * p_command_data, uint8_t * p_rx_buffer);


/**@brief Decodes the Update Data page.
 *
 * @param[out] p_update_data        The pointer where the received ASC update data is to be copied.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not an Update page.
 */
uint32_t asc_decode_update_data_page(asc_update_data_t * p_update_data, uint8_t * p_rx_buffer);


/**@brief Decodes the Update Data page relay format.
 *
 * @param[out] p_update_data        The pointer where the received ASC update data is to be copied.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not an Update page.
 */
uint32_t asc_decode_phone_update_data_page(asc_update_data_t * p_update_data, uint8_t * p_rx_buffer);


/**@brief Decodes the Request Data page.
 *
 * @param[out] p_request_data       The pointer where the received ASC request data is to be copied.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Request page.
 */
uint32_t asc_decode_request_data_page(asc_request_data_t * p_request_data,  uint8_t * p_rx_buffer);


/**@brief Decodes the Phone Command page.
 *
 * @param[out] p_command_data       The pointer where the received ASC phone command data is to be copied.
 *
 * @param[in]  p_rx_buffer          The raw ANT message received.
 *
 * @retval NRF_SUCCESS              The page was successfully decoded.
 * @retval NRF_ERROR_INVALID_PARAM  The ant buffer was not a Request page.
 */
uint32_t asc_decode_phone_command_page(asc_command_data_t * p_command_data,  uint8_t * p_rx_buffer);


#ifdef __cplusplus
}
#endif

#endif /* ASC_PAGES_H__ */

/** @} */
