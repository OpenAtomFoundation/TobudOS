/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
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
#include "nfc_pair_m.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "bsp.h"
#include "sdk_macros.h"
#include "adafruit_pn532.h"
#include "nfc_t2t_parser.h"
#include "nfc_ndef_msg_parser.h"
#include "nfc_le_oob_rec_parser.h"
#include "ble_m.h"
#include "peer_manager.h"
#include "nrf_ble_lesc.h"
#include "nrf_sdh_ble.h"

#define NRF_LOG_MODULE_NAME NFC_PAIR_M
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define NFC_BLE_PAIR_OBSERVER_PRIO         1                                                /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define SEL_RES_CASCADE_BIT_NUM            3                                                /**< Number of Cascade bit within SEL_RES byte. */
#define SEL_RES_TAG_PLATFORM_MASK          0x60                                             /**< Mask of Tag Platform bit group within SEL_RES byte. */
#define SEL_RES_TAG_PLATFORM_BIT_OFFSET    5                                                /**< Offset of the Tag Platform bit group within SEL_RES byte. */

#define TAG_TYPE_2_UID_LENGTH              7                                                /**< Length of the Tag's UID. */
#define TAG_DATA_BUFFER_SIZE               1024                                             /**< Buffer size for data from a Tag. */
#define TAG_DETECT_TIMEOUT                 5000                                             /**< Timeout for function which searches for a tag. */
#define TAG_TYPE_2_DATA_AREA_SIZE_OFFSET   (T2T_CC_BLOCK_OFFSET + 2)                        /**< Offset of the byte with Tag's Data size. */
#define TAG_TYPE_2_DATA_AREA_MULTIPLICATOR 8                                                /**< Multiplicator for a value stored in the Tag's Data size byte. */
#define TAG_TYPE_2_FIRST_DATA_BLOCK_NUM    (T2T_FIRST_DATA_BLOCK_OFFSET / T2T_BLOCK_SIZE)   /**< First block number with Tag's Data. */
#define TAG_TYPE_2_BLOCKS_PER_EXCHANGE     (T2T_MAX_DATA_EXCHANGE / T2T_BLOCK_SIZE)         /**< Number of blocks fetched in single Tag's Read command. */

#define DEVICE_NAME_BUFF_SIZE              30                                               /**< Size of the buffer used to store BLE device name. */
#define BLE_GAP_ADDR_TYPE_INVALID          0x04                                             /**< Invalid address type. */

/**
 * @brief Possible Tag Types.
 */
typedef enum
{
    NFC_T2T = 0x00,      /**< Type 2 Tag Platform. */
    NFC_T4T = 0x01,      /**< Type 4A Tag Platform. */
    NFC_TT_NOT_SUPPORTED /**< Tag Type not supported. */
} nfc_tag_type_t;

static ble_gap_addr_t           m_device_addr;                                              /**< Value acquired by NFC. Holds BLE address of peer device. */
static ble_advdata_tk_value_t   m_device_tk;                                                /**< Value acquired by NFC. Holds Temporary Key of peer device. */

static volatile bool            m_tag_match             = false;                            /**< Flag indicating that the read tag has valid Connection Handover information. */
static bool                     m_read_tag              = false;                            /**< Flag indicating that NFC reader is turned on. */

static ble_gap_lesc_oob_data_t  m_ble_lesc_oob_peer_data;                                   /**< LESC OOB pairing data. */

static void ble_nfc_pair_handler(const ble_evt_t * const p_ble_evt, void * p_context);

void nfc_pair_start(void)
{
    m_read_tag = true;
}

void nfc_pair_stop(void)
{
    m_read_tag = false;
}

/**
 * @brief Function for getting peer OOB data that have been transmitted over NFC.
 */
static ble_gap_lesc_oob_data_t * nfc_peer_oob_data_get(uint16_t conn_handle)
{
    UNUSED_PARAMETER(conn_handle);

    return &m_ble_lesc_oob_peer_data;
}

void nfc_init(void)
{
    ret_code_t err_code = adafruit_pn532_init(false);
    APP_ERROR_CHECK(err_code);

    nrf_ble_lesc_peer_oob_data_handler_set(nfc_peer_oob_data_get);

    // Register handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, NFC_BLE_PAIR_OBSERVER_PRIO, ble_nfc_pair_handler, NULL);
}

/**
 * @brief Function for identifying Tag Platform Type.
 */
static nfc_tag_type_t tag_type_identify(uint8_t sel_res)
{
    uint8_t platform_config;

    // Check if Cascade bit in SEL_RES response is cleared. Cleared bit indicates that NFCID1 complete.
    if (!IS_SET(sel_res, SEL_RES_CASCADE_BIT_NUM))
    {
        // Extract platform configuration from SEL_RES response.
        platform_config = (sel_res & SEL_RES_TAG_PLATFORM_MASK) >> SEL_RES_TAG_PLATFORM_BIT_OFFSET;
        if (platform_config < NFC_TT_NOT_SUPPORTED)
        {
            return (nfc_tag_type_t) platform_config;
        }
    }

    return NFC_TT_NOT_SUPPORTED;
}

/**
 * @brief   Function for reading data from a Tag.
 *
 * @details This function waits for a Tag to appear in the field. When a Tag is detected, all of the pages
 *          within a Tag are read.
 */
ret_code_t tag_data_read(uint8_t * buffer, uint32_t buffer_size)
{
    ret_code_t      err_code;
    nfc_a_tag_info  tag_info;
    uint8_t         block_num = 0;

    // Not enough size in the buffer to read a tag header.
    if (buffer_size < T2T_FIRST_DATA_BLOCK_OFFSET)
    {
        return NRF_ERROR_NO_MEM;
    }

    // Detect a NFC-A Tag in the field and initiate a communication. This function activates
    // the NFC RF field. If a Tag is present, basic information about detected Tag is returned
    // in tag info structure.
    err_code = adafruit_pn532_nfc_a_target_init(&tag_info, TAG_DETECT_TIMEOUT);
    VERIFY_FALSE(err_code != NRF_SUCCESS, NRF_ERROR_NOT_FOUND);

    nfc_tag_type_t tag_type = tag_type_identify(tag_info.sel_res);
    VERIFY_FALSE(tag_type != NFC_T2T, NRF_ERROR_NOT_SUPPORTED);

    // Read pages 0 - 3 to get the header information.
    err_code = adafruit_pn532_tag2_read(block_num, buffer);
    if (err_code)
    {
        NRF_LOG_INFO("Failed to read blocks: %d-%d", block_num,
                     block_num + T2T_END_PAGE_OFFSET);
        return NRF_ERROR_INTERNAL;
    }

    uint16_t data_bytes_in_tag = TAG_TYPE_2_DATA_AREA_MULTIPLICATOR *
                                     buffer[TAG_TYPE_2_DATA_AREA_SIZE_OFFSET];

    if (data_bytes_in_tag + T2T_FIRST_DATA_BLOCK_OFFSET > buffer_size)
    {
        return NRF_ERROR_NO_MEM;
    }

    uint8_t blocks_to_read = data_bytes_in_tag / T2T_BLOCK_SIZE;

    for (block_num = TAG_TYPE_2_FIRST_DATA_BLOCK_NUM;
         block_num < blocks_to_read;
         block_num += TAG_TYPE_2_BLOCKS_PER_EXCHANGE)
    {
        uint16_t offset_for_block = T2T_BLOCK_SIZE * block_num;

        err_code = adafruit_pn532_tag2_read(block_num, buffer + offset_for_block);
        if (err_code)
        {
            NRF_LOG_INFO("Failed to read blocks: %d-%d",
                         block_num,
                         block_num + T2T_END_PAGE_OFFSET);
            return NRF_ERROR_INTERNAL;
        }
    }

    return NRF_SUCCESS;
}

/**
 * @brief Function used to indicate that the read Connection Handover NDEF Message is valid.
 */
__STATIC_INLINE void nfc_oob_pairing_tag_appoint(void)
{
    m_tag_match = true;
}

void nfc_oob_pairing_tag_invalidate(void)
{
    m_device_addr.addr_type = BLE_GAP_ADDR_TYPE_INVALID;
    m_tag_match             = false;
}

/**
 * @brief Function for storing BLE device address, Temporary Key and LESC OOB data in static memory.
 */
__STATIC_INLINE void nfc_essential_pairing_data_copy(nfc_ble_oob_pairing_data_t * p_pairing_data)
{
    if (p_pairing_data->p_device_addr != NULL)
    {
        memcpy(&m_device_addr, p_pairing_data->p_device_addr, sizeof(ble_gap_addr_t));
    }

    if (p_pairing_data->p_tk_value != NULL)
    {
        memcpy(&m_device_tk, p_pairing_data->p_tk_value, sizeof(ble_advdata_tk_value_t));
    }

    if (p_pairing_data->p_lesc_confirm_value != NULL)
    {
        memcpy(m_ble_lesc_oob_peer_data.c,
               p_pairing_data->p_lesc_confirm_value,
               sizeof(m_ble_lesc_oob_peer_data.c));
    }

    if (p_pairing_data->p_lesc_random_value != NULL)
    {
        memcpy(m_ble_lesc_oob_peer_data.r,
               p_pairing_data->p_lesc_random_value,
               sizeof(m_ble_lesc_oob_peer_data.r));
    }

    if (p_pairing_data->p_device_addr != NULL)
    {
        memcpy(&m_ble_lesc_oob_peer_data.addr,
               p_pairing_data->p_device_addr,
               sizeof(ble_gap_addr_t));
    }
}

/**
 * @brief Function for analyzing Connection Handover NDEF message.
 */
void ch_ndef_msg_handle(nfc_ndef_msg_desc_t * p_ch_msg_desc)
{
    nfc_ble_oob_pairing_data_t  le_oob_record_pairing_data;
    uint8_t                     device_name[DEVICE_NAME_BUFF_SIZE];
    ble_advdata_tk_value_t      device_tk;
    ble_gap_lesc_oob_data_t     device_lesc_data;

    for (uint8_t i = 0; i < p_ch_msg_desc->record_count; i++)
    {
        le_oob_record_pairing_data.device_name.p_name   = device_name;
        le_oob_record_pairing_data.device_name.len      = sizeof(device_name);
        le_oob_record_pairing_data.p_device_addr        = &device_lesc_data.addr;
        le_oob_record_pairing_data.p_tk_value           = &device_tk;
        le_oob_record_pairing_data.p_lesc_confirm_value = (uint8_t *)device_lesc_data.c;
        le_oob_record_pairing_data.p_lesc_random_value  = (uint8_t *)device_lesc_data.r;

        // Parse an NDEF message, assuming it is a Connection Handover message.
        ret_code_t err_code = nfc_le_oob_rec_parse(p_ch_msg_desc->pp_record[i],
                                                   &le_oob_record_pairing_data);
        if (err_code == NRF_SUCCESS)
        {
            nfc_oob_data_printout(&le_oob_record_pairing_data);

            int mem_diff = memcmp(&m_device_addr,
                                  le_oob_record_pairing_data.p_device_addr,
                                  sizeof(ble_gap_addr_t));

            if (mem_diff != 0) // NFC Tag of the currently disconnected peripheral has been read.
            { 
                if (m_device_addr.addr_type != BLE_GAP_ADDR_TYPE_INVALID)
                {
                     // Terminate the current connection, so the new peripheral will be able to connect.
                    ble_disconnect();
                    while (m_tag_match){};
                }
                nfc_essential_pairing_data_copy(&le_oob_record_pairing_data);
                nfc_oob_pairing_tag_appoint();
            }
            else // NFC Tag of the currently connected peripheral has been read.
            {
                ble_disconnect();
                while (m_tag_match){};
            }
            break;
        }
    }
}

/**
 * @brief   Function for analyzing NDEF data from a TLV block.
 *
 * @details This function checks if a TLV block is in NDEF format. If an NDEF block is detected,
 *          the NDEF data is parsed and printed.
 */
void ndef_data_analyze(tlv_block_t * p_tlv_block)
{
    uint8_t    desc_buf[NFC_NDEF_PARSER_REQIRED_MEMO_SIZE_CALC(10)];
    uint32_t   nfc_data_len;
    uint32_t   desc_buf_len = sizeof(desc_buf);
    ret_code_t ret_code;

    if (p_tlv_block->tag == TLV_NDEF_MESSAGE)
    {
        nfc_data_len = p_tlv_block->length;

        ret_code = ndef_msg_parser(desc_buf,
                                   &desc_buf_len,
                                   p_tlv_block->p_value,
                                   &nfc_data_len);
        ndef_msg_printout((nfc_ndef_msg_desc_t *) desc_buf);

        if (ret_code != NRF_SUCCESS)
        {
            NRF_LOG_INFO("Error during parsing a NDEF message.");
        }
        else
        {
            ch_ndef_msg_handle((nfc_ndef_msg_desc_t *) desc_buf);
        }
    }
}

/**
 * @brief   Function for analyzing data from a Tag.
 *
 * @details This function parses content of a Tag and prints it out.
 */
void tag_data_analyze(uint8_t * buffer)
{
    ret_code_t err_code;

    // Static declaration of Type 2 Tag structure. Maximum of 10 TLV blocks can be read.
    NFC_TYPE_2_TAG_DESC_DEF(test_1, 10);
    type_2_tag_t * test_type_2_tag = &NFC_TYPE_2_TAG_DESC(test_1);

    err_code = type_2_tag_parse(test_type_2_tag, buffer);
    if (err_code == NRF_ERROR_NO_MEM)
    {
        NRF_LOG_INFO("Not enough memory to read whole tag. Printing what've been read.");
    }
    else if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Error during parsing a tag. Printing what could've been read.");
    }

    NRF_LOG_RAW_INFO("\r\n");
    type_2_tag_printout(test_type_2_tag);
    NRF_LOG_RAW_INFO("\r\n");

    tlv_block_t * p_tlv_block = test_type_2_tag->p_tlv_block_array;
    uint32_t i;

    for (i = 0; i < test_type_2_tag->tlv_count; i++)
    {
        ndef_data_analyze(p_tlv_block);
        p_tlv_block++;
    }
}

void nfc_tag_process(void)
{
    ret_code_t err_code;

    if (m_read_tag)
    {
        // Buffer for tag data.
        static uint8_t tag_data[TAG_DATA_BUFFER_SIZE];

        err_code = tag_data_read(tag_data, TAG_DATA_BUFFER_SIZE);
        switch (err_code)
        {
            case NRF_SUCCESS:
                tag_data_analyze(tag_data);
                nfc_pair_stop();
                scan_start();
                break;

            case NRF_ERROR_NO_MEM:
                NRF_LOG_INFO("Declared buffer is to small to store tag data.");
                break;

            case NRF_ERROR_NOT_FOUND:
                NRF_LOG_INFO("No Tag found.");
                break;

            case NRF_ERROR_NOT_SUPPORTED:
                NRF_LOG_INFO("Tag not supported.");
                break;

            default:
                NRF_LOG_INFO("Error during tag read.");
                err_code = adafruit_pn532_field_off();
                APP_ERROR_CHECK(err_code);
                break;
        }
    }

    err_code = nrf_ble_lesc_request_handler();
    APP_ERROR_CHECK(err_code);
}

bool nfc_oob_pairing_tag_match(ble_gap_addr_t const * const p_peer_addr)
{
    if (m_tag_match)
    {
        if (memcmp(p_peer_addr->addr, m_device_addr.addr, BLE_GAP_ADDR_LEN) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Function used for acquiring Temporary Key value.
 *
 * @param[out] pp_tk_value   Pointer to pointer to the Temporary Key Value.
 *
 * @retval    NRF_SUCCESS         If the Temporary Key was found.
 * @retval    NRF_ERROR_NOT_FOUND Otherwise.
 */
ret_code_t nfc_tk_value_get(ble_advdata_tk_value_t ** pp_tk_value)
{
    if (m_tag_match)
    {
        *pp_tk_value = &m_device_tk;
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_NOT_FOUND;
    }
}

/**
 * @brief Function for handling NFC pairing BLE events.
 *
 * @details Handles authentication events, replying with OOB data.
 *
 * @param[in] p_ble_evt Bluetooth stack event.
 * @param[in] p_context Unused.
 */
static void ble_nfc_pair_handler(const ble_evt_t * const p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    const ble_gap_evt_t * const p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon authentication key request, reply with Temporary Key that was read from the NFC tag.
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
        {
            NRF_LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");

            ble_advdata_tk_value_t* oob_key;
            err_code = nfc_tk_value_get(&oob_key);
            APP_ERROR_CHECK(err_code);

            err_code = sd_ble_gap_auth_key_reply(p_gap_evt->conn_handle,
                                                 BLE_GAP_AUTH_KEY_TYPE_OOB,
                                                 oob_key->tk);
            APP_ERROR_CHECK(err_code);
        } break;
    }
}

