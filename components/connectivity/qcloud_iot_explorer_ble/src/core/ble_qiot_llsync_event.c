/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_llsync_event.h"

#include <stdbool.h>
#include <string.h>

#include "ble_qiot_import.h"
#include "ble_qiot_common.h"
#include "ble_qiot_param_check.h"
#include "ble_qiot_service.h"
#include "ble_qiot_template.h"
#include "ble_qiot_llsync_device.h"

ble_qiot_ret_status_t ble_event_get_status(void)
{
#ifdef BLE_QIOT_INCLUDE_PROPERTY
    return ble_event_notify(BLE_QIOT_EVENT_UP_GET_STATUS, NULL, 0, NULL, 0);
#else
    ble_qiot_log_e("llsync property not support");
    return BLE_QIOT_RS_OK;
#endif
}

ble_qiot_ret_status_t ble_event_report_property(void)
{
#ifdef BLE_QIOT_INCLUDE_PROPERTY
    return ble_user_property_get_report_data();
#else
    ble_qiot_log_e("llsync property not support");
    return BLE_QIOT_RS_OK;
#endif
}

ble_qiot_ret_status_t ble_event_report_device_info(void)
{
    char     device_info[3] = {0};
    uint16_t mtu_size       = 0;

    mtu_size       = ble_get_user_data_mtu_size();
    mtu_size       = HTONS(mtu_size);
    device_info[0] = BLE_QIOT_LLSYNC_PROTOCOL_VERSION;
    memcpy(&device_info[1], &mtu_size, sizeof(mtu_size));

    return ble_event_notify(BLE_QIOT_EVENT_UP_REPORT_MTU, NULL, 0, (const char *)device_info, sizeof(device_info));
}

ble_qiot_ret_status_t ble_event_notify(uint8_t type, uint8_t *header, uint8_t header_len, const char *buf,
                                       uint16_t buf_len)
{
    char *   p                                 = NULL;
    uint16_t left_len                          = 0;
    uint16_t send_len                          = 0;
    uint16_t mtu_size                          = 0;
    uint8_t  slice_state                       = BLE_QIOT_EVENT_NO_SLICE;
    uint8_t  send_buf[BLE_QIOT_EVENT_BUF_SIZE] = {0};
    uint16_t send_buf_index                    = 0;
    uint16_t tmp_len                           = 0;

    if (!llsync_is_connected() && type != BLE_QIOT_EVENT_UP_BIND_SIGN_RET && type != BLE_QIOT_EVENT_UP_CONN_SIGN_RET &&
        type != BLE_QIOT_EVENT_UP_UNBIND_SIGN_RET) {
        ble_qiot_log_e("upload msg negate, device not connected");
        return BLE_QIOT_RS_ERR;
    }

    // reserve event header length, 3 bytes fixed length + n bytes header
    mtu_size = ble_get_user_data_mtu_size();
    mtu_size = mtu_size > sizeof(send_buf) ? sizeof(send_buf) : mtu_size;
    mtu_size -= (BLE_QIOT_EVENT_FIXED_HEADER_LEN + header_len);
    ble_qiot_log_d("mtu size %d", mtu_size);

    p        = (char *)buf;
    left_len = buf_len;
    do {
        memset(send_buf, 0, sizeof(send_buf));
        send_buf_index = 0;
        send_len       = left_len > mtu_size ? mtu_size : left_len;

        send_buf[send_buf_index++] = type;
        if (NULL != buf) {
            tmp_len = HTONS(send_len + header_len);
            memcpy(send_buf + send_buf_index, &tmp_len, sizeof(uint16_t));
            send_buf_index += sizeof(uint16_t);
            if (NULL != header) {
                memcpy(send_buf + send_buf_index, header, header_len);
                send_buf_index += header_len;
            }
            memcpy(send_buf + send_buf_index, p, send_len);
            send_buf_index += send_len;

            p += send_len;
            left_len -= send_len;
            // add event id
            send_len += (BLE_QIOT_EVENT_FIXED_HEADER_LEN + header_len);

            if (0 == left_len) {
                if (BLE_QIOT_EVENT_NO_SLICE == slice_state) {
                    slice_state = BLE_QIOT_EVENT_NO_SLICE;
                } else {
                    slice_state = BLE_QIOT_EVENT_SLICE_FOOT;
                }
            } else {
                if (BLE_QIOT_EVENT_NO_SLICE == slice_state) {
                    slice_state = BLE_QIOT_EVENT_SLICE_HEAD;
                } else {
                    slice_state = BLE_QIOT_EVENT_SLICE_BODY;
                }
            }
            // the high 2 bits means slice state, and the left 14 bits is data length
            send_buf[1] |= slice_state << 6;
        } else {
            send_len = send_buf_index;
        }

        ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "post data", (char *)send_buf, send_len);
        if (0 != ble_send_notify(send_buf, send_len)) {
            ble_qiot_log_e("event post failed, type %d", type);
            return BLE_QIOT_RS_ERR;
        }
    } while (left_len != 0);

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_event_post(uint8_t event_id)
{
#ifdef BLE_QIOT_INCLUDE_EVENT
    uint8_t  param_id                          = 0;
    uint8_t  param_id_size                     = 0;
    uint8_t  param_type                        = 0;
    int      param_len                         = 0;
    uint8_t  data_buf[BLE_QIOT_EVENT_MAX_SIZE] = {0};
    uint16_t data_len                          = 0;
    uint16_t string_len                        = 0;
    uint8_t  header_buf[1]                     = {0};

    ble_qiot_log_d("post event %d", event_id);
    param_id_size = ble_event_get_id_array_size(event_id);
    // get all the data
    for (param_id = 0; param_id < param_id_size; param_id++) {
        param_type = ble_event_get_param_id_type(event_id, param_id);
        if (param_type == BLE_QIOT_DATA_TYPE_BUTT) {
            ble_qiot_log_e("invalid type, event id %d, param id %d", event_id, param_id);
            return BLE_QIOT_RS_ERR;
        }

        data_buf[data_len++] = BLE_QIOT_PACKAGE_TLV_HEAD(param_type, param_id);
        if (BLE_QIOT_DATA_TYPE_STRING == param_type) {
            // reserved 2 bytes for string length, other type have fixed length
            param_len = ble_event_get_data_by_id(event_id, param_id, (char *)data_buf + data_len + 2,
                                                 sizeof(data_buf) - data_len - 2);
        } else {
            param_len =
                ble_event_get_data_by_id(event_id, param_id, (char *)data_buf + data_len, sizeof(data_buf) - data_len);
        }
        if (param_len < 0) {
            ble_qiot_log_e("too long data, event id %d, data length %d", event_id, data_len);
            return BLE_QIOT_RS_ERR;
        } else if (param_len == 0) {
            // no data to post
            data_len--;
            data_buf[data_len] = '0';
            ble_qiot_log_d("param id %d no data to post", param_id);
        } else {
            if (BLE_QIOT_DATA_TYPE_STRING == param_type) {
                string_len = HTONS(param_len);
                memcpy(data_buf + data_len, &string_len, sizeof(uint16_t));
                data_len += sizeof(uint16_t);
            }
            data_len += param_len;
        }
    }
    header_buf[0] = event_id;

    return ble_event_notify(BLE_QIOT_EVENT_UP_EVENT_POST, header_buf, sizeof(header_buf), (const char *)data_buf,
                            data_len);
#else
    ble_qiot_log_e("llsync event not support");
    return BLE_QIOT_RS_ERR;
#endif
}

#ifdef __cplusplus
}
#endif
