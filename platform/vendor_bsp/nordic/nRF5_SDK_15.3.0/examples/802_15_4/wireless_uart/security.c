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

#include <string.h>
#include "fsm.h"
#include "mac_security.h"
#include "mac_common.h"
#include "mac_mlme_pib.h"
#include "mac_mlme_comm_status.h"

#define GET_BYTE(x, n) (((x) >> (n * 8)) & 0xFF)

const uint8_t m_key_lookup_data[KEY_LOOKUP_SIZE_FIVE_VAL] = {GET_BYTE(CONFIG_PAN_ID, 0),
                                                             GET_BYTE(CONFIG_PAN_ID, 1),
                                                             GET_BYTE(CONFIG_OTHER_ADDRESS, 0),
                                                             GET_BYTE(CONFIG_OTHER_ADDRESS, 1),
                                                             0x00,
                                                            };

const uint8_t m_security_key[MAC_SECURITY_KEY_SIZE] = CONFIG_SECURITY_KEY;

void security_tables_init(void)
{
#if (CONFIG_SECURE == 1)
    pib_id_t                   id;
    bool                       security_enabled;
    mac_status_t               status;
    pib_id_t                   pib_id;
    mac_key_descr_t            key_descr;
    mac_key_id_lookup_descr_t  key_id_lookup_descr;
    mac_key_device_descr_t     key_device_descr;
    mac_key_usage_descr_t      key_usage_descr;
    mac_security_level_descr_t sec_level_descr;
    mac_device_descr_t         device_descr;

    id.mlme_id = MAC_SECURITY_ENABLED;
    security_enabled = true;
    mlme_set(id, 0, &security_enabled);

    mac_table_init(&key_descr.id_lookup_list);
    mac_table_init(&key_descr.key_device_list);
    mac_table_init(&key_descr.key_usage_list);

    key_id_lookup_descr.size = KEY_LOOKUP_SIZE_FIVE;
    memcpy(key_id_lookup_descr.data, m_key_lookup_data, KEY_LOOKUP_SIZE_FIVE_VAL);
    status = mac_table_item_set(&key_descr.id_lookup_list,
                                &key_id_lookup_descr.table_service,
                                MAC_KEY_ID_LOOKUP_LIST, 0);
    ASSERT(status == MAC_SUCCESS);

    key_device_descr.blacklisted = false;
    key_device_descr.unique_device = true;
    key_device_descr.device_handle = 0;
    key_device_descr.table_service.idx = 0;
    status = mac_table_item_set(&key_descr.key_device_list,
                                &key_device_descr.table_service,
                                MAC_KEY_DEVICE_LIST, 0);
    ASSERT(status == MAC_SUCCESS);

    key_usage_descr.frame_type = MAC_DATA;
    key_usage_descr.cmd_frame_id = 0;
    status = mac_table_item_set(&key_descr.key_usage_list,
                                &key_usage_descr.table_service,
                                MAC_KEY_USAGE_LIST, 0);
    ASSERT(status == MAC_SUCCESS);

    memcpy(key_descr.key, m_security_key, MAC_SECURITY_KEY_SIZE);

    pib_id.mlme_id = MAC_KEY_TABLE;
    status = mlme_set(pib_id, 0, &key_descr);
    ASSERT(status == MAC_SUCCESS);

    sec_level_descr.security_min = CONFIG_DATA_SECURITY_LEVEL;
    sec_level_descr.frame_type   = MAC_DATA;
    sec_level_descr.override_min = true;

    device_descr.extended_address = CONFIG_IEEE_ADDRESS + CONFIG_OTHER_ADDRESS;
    device_descr.pan_id           = CONFIG_PAN_ID;
    device_descr.short_address    = CONFIG_OTHER_ADDRESS;
    device_descr.frame_counter    = 0;
    device_descr.exempt           = 0;

    pib_id.mlme_id = MAC_DEVICE_TABLE;
    status = mlme_set(pib_id, 0, &device_descr);
    ASSERT(status == MAC_SUCCESS);

    pib_id.mlme_id = MAC_SECURITY_LEVEL_TABLE;
    status = mlme_set(pib_id, 0, &sec_level_descr);
    ASSERT(status == MAC_SUCCESS);

    fsm_event_post(E_SET_DONE, NULL);
    (void)status;
#endif
}

void mlme_comm_status_ind(mlme_comm_status_ind_t * ind)
{
    ASSERT_INFO(0, "security status: %d", ind->status);
}

