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

#include "ble_qiot_llsync_data.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ble_qiot_export.h"
#include "ble_qiot_import.h"
#include "ble_qiot_common.h"
#include "ble_qiot_llsync_event.h"
#include "ble_qiot_utils_base64.h"
#include "ble_qiot_log.h"
#include "ble_qiot_param_check.h"
#include "ble_qiot_service.h"
#include "ble_qiot_template.h"

#define BLE_LLDATA_MAX_INPUT_ID  8
#define BLE_LLDATA_MAX_OUTPUT_ID 8

static int ble_lldata_parse_tlv(const char *buf, int buf_len, e_ble_tlv *tlv)
{
    int      ret_len    = 0;
    uint16_t string_len = 0;

    tlv->type = BLE_QIOT_PARSE_TLV_HEAD_TYPE(buf[0]);
    if (tlv->type >= BLE_QIOT_DATA_TYPE_BUTT) {
        ble_qiot_log_e("invalid tlv type %d", tlv->type);
        return -1;
    }
    tlv->id = BLE_QIOT_PARSE_TLV_HEAD_ID(buf[0]);
    ret_len++;

    switch (tlv->type) {
        case BLE_QIOT_DATA_TYPE_BOOL:
            tlv->len = 1;
            tlv->val = (char *)buf + ret_len;
            ret_len += 1;
            break;
        case BLE_QIOT_DATA_TYPE_ENUM:
            tlv->len = 2;
            tlv->val = (char *)buf + ret_len;
            ret_len += 2;
            break;
        case BLE_QIOT_DATA_TYPE_INT:
        case BLE_QIOT_DATA_TYPE_FLOAT:
        case BLE_QIOT_DATA_TYPE_TIME:
            tlv->len = 4;
            tlv->val = (char *)buf + ret_len;
            ret_len += 4;
            break;
        case BLE_QIOT_DATA_TYPE_STRING:
            if (buf_len < 3) {
                ble_qiot_log_e("buf length invalid");
                return -1;
            }
            memcpy(&string_len, &buf[ret_len], sizeof(int16_t));
            tlv->len = NTOHS(string_len);
            ret_len += 2;
            tlv->val = (char *)buf + ret_len;
            ret_len += tlv->len;
            break;
        default:
            break;
    }
    ble_qiot_log_d("type %d, id %d, len %d", tlv->type, tlv->id, tlv->len);

    return ret_len;
}

static ble_qiot_ret_status_t ble_lldata_property_data_handle(bool is_request, const char *in_buf, int buf_len)
{
#ifdef BLE_QIOT_INCLUDE_PROPERTY
    uint16_t              parse_len = 0;
    uint16_t              ret_len   = 0;
    e_ble_tlv             tlv;
    int                   handle_ret = BLE_QIOT_REPLY_SUCCESS;
    ble_qiot_ret_status_t inform_ret = BLE_QIOT_RS_OK;

    while (parse_len < buf_len) {
        memset(&tlv, 0, sizeof(e_ble_tlv));
        ret_len = ble_lldata_parse_tlv(in_buf + parse_len, buf_len - parse_len, &tlv);
        parse_len += ret_len;
        if (parse_len > buf_len) {
            ble_qiot_log_e("parse tlv failed");
            handle_ret = BLE_QIOT_REPLY_DATA_ERR;
            break;
        }

        if (BLE_QIOT_RS_OK != ble_user_property_set_data(&tlv)) {
            ble_qiot_log_e("user handle property error, id %d, type %d, len %d", tlv.id, tlv.type, tlv.len);
            handle_ret = BLE_QIOT_REPLY_FAIL;
            break;
        }
    }

    if (is_request) {
        inform_ret =
            ble_event_notify(BLE_QIOT_EVENT_UP_CONTROL_REPLY, NULL, 0, (const char *)&handle_ret, sizeof(uint8_t));
        return (BLE_QIOT_REPLY_SUCCESS == handle_ret) ? inform_ret : BLE_QIOT_RS_ERR;
    } else {
        return (BLE_QIOT_REPLY_SUCCESS == handle_ret) ? BLE_QIOT_RS_OK : BLE_QIOT_RS_ERR;
    }
#else
    ble_qiot_log_e("llsync property not support");
    return BLE_QIOT_RS_OK;
#endif
}

#ifdef BLE_QIOT_INCLUDE_PROPERTY
ble_qiot_ret_status_t ble_user_property_get_report_data(void)
{
    uint8_t  property_id                       = 0;
    uint8_t  property_type                     = 0;
    int      property_len                      = 0;
    uint8_t  data_buf[BLE_QIOT_EVENT_MAX_SIZE] = {0};
    uint16_t data_len                          = 0;
    uint16_t string_len                        = 0;

    ble_qiot_log_d("property report");
    for (property_id = 0; property_id < BLE_QIOT_PROPERTY_ID_BUTT; property_id++) {
        property_type = ble_get_property_type_by_id(property_id);
        if (property_type == BLE_QIOT_DATA_TYPE_BUTT) {
            ble_qiot_log_e("property id %d type %d invalid", property_id, property_type);
            return BLE_QIOT_RS_ERR;
        }
        data_buf[data_len++] = BLE_QIOT_PACKAGE_TLV_HEAD(property_type, property_id);
        if (BLE_QIOT_DATA_TYPE_STRING == property_type) {
            // reserved 2 bytes for string length
            property_len = ble_user_property_get_data_by_id(property_id, (char *)data_buf + data_len + 2,
                                                            sizeof(data_buf) - data_len - 2);
        } else {
            property_len =
                ble_user_property_get_data_by_id(property_id, (char *)data_buf + data_len, sizeof(data_buf) - data_len);
        }
        if (property_len < 0) {
            ble_qiot_log_e("too long data, property id %d, data length %d", property_id, data_len);
            return BLE_QIOT_RS_ERR;
        } else if (property_len == 0) {
            // no data to post
            data_len--;
            data_buf[data_len] = '0';
            ble_qiot_log_d("property id %d no data to post", property_id);
        } else {
            if (BLE_QIOT_DATA_TYPE_STRING == property_type) {
                string_len = HTONS(property_len);
                memcpy(data_buf + data_len, &string_len, sizeof(uint16_t));
                data_len += sizeof(uint16_t);
            }
            data_len += property_len;
        }
    }
    // ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "user data", data_buf, data_len);

    return ble_event_notify(BLE_QIOT_EVENT_UP_PROPERTY_REPORT, NULL, 0, (const char *)data_buf, data_len);
}
#endif

// default method is control
ble_qiot_ret_status_t ble_lldata_property_request_handle(const char *in_buf, int buf_len)
{
    return ble_lldata_property_data_handle(true, in_buf, buf_len);
}

static ble_qiot_ret_status_t ble_lldata_property_report_reply(const char *in_buf, int buf_len)
{
#ifdef BLE_QIOT_INCLUDE_PROPERTY
    int ret_code = 0;

    ret_code = ble_user_property_report_reply_handle(in_buf[0]);
    if (ret_code) {
        ble_qiot_log_e("ble_user_property_report_reply_handle error, ret %d", ret_code);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
#else
    ble_qiot_log_e("llsync property not support");
    return BLE_QIOT_RS_OK;
#endif
}

static ble_qiot_ret_status_t ble_lldata_property_get_status_reply(const char *in_buf, int buf_len)
{
#ifdef BLE_QIOT_INCLUDE_PROPERTY
    uint8_t  result    = 0;
    uint16_t reply_len = 0;

    result = in_buf[0];
    memcpy(&reply_len, &in_buf[1], sizeof(uint16_t));
    reply_len = NTOHS(reply_len);
    ble_qiot_log_d("get status result %d, data len %d", result, reply_len);
    if (BLE_QIOT_REPLY_SUCCESS == result) {
        return ble_lldata_property_data_handle(false, in_buf + 3, reply_len);
    } else {
        ble_qiot_log_e("get status failed, result %d", result);
        return BLE_QIOT_RS_ERR;
    }
#else
    ble_qiot_log_e("llsync property not support");
    return BLE_QIOT_RS_OK;
#endif
}

// handle reply from remote
ble_qiot_ret_status_t ble_lldata_property_reply_handle(uint8_t type, const char *in_buf, int buf_len)
{
    switch (type) {
        case BLE_QIOT_DATA_DOWN_REPORT_REPLY:
            return ble_lldata_property_report_reply(in_buf, buf_len);
        case BLE_QIOT_DATA_DOWN_GET_STATUS_REPLY:
            return ble_lldata_property_get_status_reply(in_buf, buf_len);
        default:
            ble_qiot_log_e("invalid property reply type");
            break;
    }

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_lldata_event_handle(uint8_t id, const char *in_buf, int len)
{
#ifdef BLE_QIOT_INCLUDE_EVENT
    int ret_code = 0;

    ret_code = ble_user_event_reply_handle(id, in_buf[0]);
    if (ret_code) {
        ble_qiot_log_e("ble_user_event_reply_handle error, ret %d", ret_code);
        return BLE_QIOT_RS_ERR;
    }
#else
    ble_qiot_log_e("llsync event not support");
#endif
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_lldata_action_handle(uint8_t action_id, const char *in_buf, int len)
{
#ifdef BLE_QIOT_INCLUDE_ACTION
    POINTER_SANITY_CHECK(in_buf, BLE_QIOT_RS_ERR_PARA);

    uint16_t  parse_len                                         = 0;
    uint8_t   tlv_index                                         = 0;
    int       ret_len                                           = 0;
    e_ble_tlv tlv[BLE_QIOT_ACTION_INPUT_ID_BUTT]                = {0};
    int       handle_ret                                        = BLE_QIOT_REPLY_SUCCESS;
    uint8_t   output_flag_array[BLE_QIOT_ACTION_OUTPUT_ID_BUTT] = {0};
    uint8_t   output_id                                         = 0;
    uint8_t   output_type                                       = 0;
    uint8_t   data_buf[BLE_QIOT_EVENT_MAX_SIZE]                 = {0};
    uint16_t  data_len                                          = 0;
    int       output_param_len                                  = 0;
    uint16_t  string_len                                        = 0;
    uint8_t   header_buf[2]                                     = {0};

    ble_qiot_log_d("action %d", action_id);
    while (parse_len < len) {
        if (tlv_index >= BLE_QIOT_ACTION_INPUT_ID_BUTT) {
            ble_qiot_log_e("invalid tlv index %d", tlv_index);
            handle_ret = BLE_QIOT_REPLY_DATA_ERR;
            goto end;
        }
        ret_len = ble_lldata_parse_tlv(in_buf + parse_len, len - parse_len, &tlv[tlv_index]);
        if (-1 == ret_len) {
            handle_ret = BLE_QIOT_REPLY_DATA_ERR;
            goto end;
        }
        parse_len += ret_len;
        tlv_index++;
        if (parse_len > len) {
            ble_qiot_log_e("parse tlv failed");
            handle_ret = BLE_QIOT_REPLY_DATA_ERR;
            goto end;
        }
    }
    if (0 != ble_action_user_handle_input_param(action_id, tlv, tlv_index, output_flag_array)) {
        ble_qiot_log_e("handle input failed, id %d", action_id);
        handle_ret = BLE_QIOT_REPLY_FAIL;
        goto end;
    }

    for (output_id = 0; output_id < BLE_QIOT_ACTION_OUTPUT_ID_BUTT; output_id++) {
        if (output_flag_array[output_id]) {
            output_type = ble_action_get_output_type_by_id(action_id, output_id);
            if (output_type == BLE_QIOT_DATA_TYPE_BUTT) {
                ble_qiot_log_e("invalid type, action id %d, output id %d", action_id, output_id);
                handle_ret = BLE_QIOT_REPLY_FAIL;
                goto end;
            }
            data_buf[data_len++] = BLE_QIOT_PACKAGE_TLV_HEAD(output_type, output_id);
            if (BLE_QIOT_DATA_TYPE_STRING == output_type) {
                output_param_len = ble_action_user_handle_output_param(
                    action_id, output_id, (char *)data_buf + data_len + 2, sizeof(data_buf) - data_len - 2);
            } else {
                output_param_len = ble_action_user_handle_output_param(
                    action_id, output_id, (char *)data_buf + data_len, sizeof(data_buf) - data_len);
            }
            if (output_param_len < 0) {
                ble_qiot_log_e("too long data, action id %d, data length %d", action_id, data_len);
                handle_ret = BLE_QIOT_REPLY_FAIL;
                goto end;
            } else if (output_param_len == 0) {
                // no data to post
                data_len--;
                data_buf[data_len] = '0';
                ble_qiot_log_d("output id %d no data to post", output_id);
            } else {
                if (BLE_QIOT_DATA_TYPE_STRING == output_type) {
                    string_len = HTONS(output_param_len);
                    memcpy(data_buf + data_len, &string_len, sizeof(uint16_t));
                    data_len += sizeof(uint16_t);
                }
                data_len += output_param_len;
            }
        }
    }
    // ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "user data", data_buf, data_len);

    header_buf[0] = BLE_QIOT_REPLY_SUCCESS;
    header_buf[1] = action_id;
    return ble_event_notify(BLE_QIOT_EVENT_UP_ACTION_REPLY, header_buf, sizeof(header_buf), (const char *)data_buf,
                            data_len);

end:
    ble_event_notify(BLE_QIOT_EVENT_UP_ACTION_REPLY, NULL, 0, (const char *)&handle_ret, sizeof(uint8_t));
    return BLE_QIOT_RS_ERR;
#else
    ble_qiot_log_e("llsync action not support");
    return BLE_QIOT_RS_ERR;
#endif
}

#ifdef __cplusplus
}
#endif
