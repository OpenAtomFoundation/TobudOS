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

#include "ble_qiot_template.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ble_qiot_export.h"
#include "ble_qiot_common.h"
#include "ble_qiot_param_check.h"


extern void property_power_switch(const char *data, uint16_t len);
extern void action_led_blink(int ms);
extern void report_reply_blink(void);


static uint8_t sg_test_power_switch = false;

static int ble_property_power_switch_set(const char *data, uint16_t len)
{
    ble_qiot_log_d("set property power_switch %d", *(uint8_t *)data);
    property_power_switch(data, len);
    sg_test_power_switch = data[0];

    return 0;
}

static int ble_property_power_switch_get(char *data, uint16_t buf_len)
{
    ble_qiot_log_d("get property power_switch %d", sg_test_power_switch);
    data[0] = sg_test_power_switch;

    return sizeof(uint8_t);
}

static uint16_t sg_test_color = 0;

static int ble_property_color_set(const char *data, uint16_t len)
{
    uint16_t color_value = 0;

    memcpy(&color_value, data, sizeof(uint16_t));
    color_value = NTOHS(color_value);
    ble_qiot_log_d("set property color %d", color_value);
    sg_test_color = color_value;

    return 0;
}

static int ble_property_color_get(char *data, uint16_t buf_len)
{
    uint16_t color_value = 0;

    ble_qiot_log_d("get property color %d", color_value);
    color_value = HTONS(sg_test_color);
    memcpy(data, &color_value, sizeof(uint16_t));

    return sizeof(uint16_t);
}

static int sg_test_brightness = 0;

static int ble_property_brightness_set(const char *data, uint16_t len)
{
    int brightness_value = 0;

    memcpy(&brightness_value, data, sizeof(int));
    brightness_value = NTOHL(brightness_value);

    if ((brightness_value < BLE_QIOT_PROPERTY_BRIGHTNESS_MIN) ||
        (brightness_value > BLE_QIOT_PROPERTY_BRIGHTNESS_MAX)) {
        ble_qiot_log_e("invalid brightness value %d", brightness_value);
        return -1;
    }

    ble_qiot_log_d("set property brightness %d", brightness_value);
    sg_test_brightness = brightness_value;

    return 0;
}

static int ble_property_brightness_get(char *data, uint16_t buf_len)
{
    int brightness_value = 0;

    ble_qiot_log_d("get property brightness %d", sg_test_brightness);
    brightness_value = HTONL(sg_test_brightness);
    memcpy(data, &brightness_value, sizeof(int));

    return sizeof(uint32_t);
}

static char sg_test_name[100 + 1] = "default name";

static int ble_property_name_set(const char *data, uint16_t len)
{
    ble_qiot_log_d("set property name %.*s", len, data);
    if (len > sizeof(sg_test_name) - 1) {
        ble_qiot_log_d("too long name");
        return -1;
    }
    memset(sg_test_name, 0, sizeof(sg_test_name));
    memcpy(sg_test_name, data, len);

    return 0;
}

static int ble_property_name_get(char *data, uint16_t buf_len)
{
    int i = 0;

    ble_qiot_log_d("get property name %s", sg_test_name);
    if (0 == strncmp("default name", sg_test_name, sizeof("default name") - 1)) {
        for (i = 0; i < 26 * 3; i++) {
            data[i] = 'a' + (i % 26);
        }
        return i;
    } else {
        memcpy(data, sg_test_name, strlen(sg_test_name));
        return strlen(sg_test_name);
    }
}

static ble_property_t sg_ble_property_array[BLE_QIOT_PROPERTY_ID_BUTT] = {
    {ble_property_power_switch_set, ble_property_power_switch_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_BOOL},
    {ble_property_color_set, ble_property_color_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_ENUM},
    {ble_property_brightness_set, ble_property_brightness_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_INT},
    {ble_property_name_set, ble_property_name_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_STRING},
};

static bool ble_check_space_enough_by_type(uint8_t type, uint16_t left_size)
{
    switch (type) {
        case BLE_QIOT_DATA_TYPE_BOOL:
            return left_size >= sizeof(uint8_t);
        case BLE_QIOT_DATA_TYPE_INT:
        case BLE_QIOT_DATA_TYPE_FLOAT:
        case BLE_QIOT_DATA_TYPE_TIME:
            return left_size >= sizeof(uint32_t);
        case BLE_QIOT_DATA_TYPE_ENUM:
            return left_size >= sizeof(uint16_t);
        default:
            // string length is unknow, default true
            return true;
    }
}

static uint16_t ble_check_ret_value_by_type(uint8_t type, uint16_t buf_len, uint16_t ret_val)
{
    switch (type) {
        case BLE_QIOT_DATA_TYPE_BOOL:
            return ret_val <= sizeof(uint8_t);
        case BLE_QIOT_DATA_TYPE_INT:
        case BLE_QIOT_DATA_TYPE_FLOAT:
        case BLE_QIOT_DATA_TYPE_TIME:
            return ret_val <= sizeof(uint32_t);
        case BLE_QIOT_DATA_TYPE_ENUM:
            return ret_val <= sizeof(uint16_t);
        default:
            // string length is unknow, default true
            return ret_val <= buf_len;
    }
}

uint8_t ble_get_property_type_by_id(uint8_t id)
{
    if (id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    return sg_ble_property_array[id].type;
}

int ble_user_property_set_data(const e_ble_tlv *tlv)
{
    POINTER_SANITY_CHECK(tlv, BLE_QIOT_RS_ERR_PARA);
    if (tlv->id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", tlv->id);
        return BLE_QIOT_RS_ERR;
    }

    if (NULL != sg_ble_property_array[tlv->id].set_cb) {
        if (0 != sg_ble_property_array[tlv->id].set_cb(tlv->val, tlv->len)) {
            ble_qiot_log_e("set property id %d failed", tlv->id);
            return BLE_QIOT_RS_ERR;
        } else {
            return BLE_QIOT_RS_OK;
        }
    }
    ble_qiot_log_e("invalid set callback, id %d", tlv->id);

    return BLE_QIOT_RS_ERR;
}

int ble_user_property_get_data_by_id(uint8_t id, char *buf, uint16_t buf_len)
{
    int ret_len = 0;

    POINTER_SANITY_CHECK(buf, BLE_QIOT_RS_ERR_PARA);
    if (id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", id);
        return -1;
    }

    if (NULL != sg_ble_property_array[id].get_cb) {
        if (!ble_check_space_enough_by_type(sg_ble_property_array[id].type, buf_len)) {
            ble_qiot_log_e("not enough space get property id %d data", id);
            return -1;
        }
        ret_len = sg_ble_property_array[id].get_cb(buf, buf_len);
        if (ret_len < 0) {
            ble_qiot_log_e("get property id %d data failed", id);
            return -1;
        } else {
            if (ble_check_ret_value_by_type(sg_ble_property_array[id].type, buf_len, ret_len)) {
                return ret_len;
            } else {
                ble_qiot_log_e("property id %d length invalid, type %d", id, sg_ble_property_array[id].type);
                return -1;
            }
        }
    }
    ble_qiot_log_e("invalid callback, property id %d", id);

    return 0;
}

int ble_user_property_report_reply_handle(uint8_t result)
{
    ble_qiot_log_d("report reply result %d", result);
    if (0 == result) {
        report_reply_blink();
    }

    return BLE_QIOT_RS_OK;
}

static int ble_event_get_status_report_status(char *buf, uint16_t buf_len)
{
    buf[0] = 1;

    return 1;
}

static int ble_event_get_status_report_message(char *buf, uint16_t buf_len)
{
    int i = 0;

    for (i = 0; i < 26 * 3; i++) {
        buf[i] = 'a' + (i % 26);
    }

    return i;
}

static ble_event_param sg_ble_event_status_report_array[BLE_QIOT_EVENT_STATUS_REPORT_PARAM_ID_BUTT] = {
    {ble_event_get_status_report_status, BLE_QIOT_DATA_TYPE_BOOL},
    {ble_event_get_status_report_message, BLE_QIOT_DATA_TYPE_STRING},
};

static int ble_event_get_low_voltage_voltage(char *data, uint16_t buf_len)
{
    float tmp = 1.0;

    memcpy(data, &tmp, sizeof(float));

    return sizeof(float);
}

static ble_event_param sg_ble_event_low_voltage_array[BLE_QIOT_EVENT_LOW_VOLTAGE_PARAM_ID_BUTT] = {
    {ble_event_get_low_voltage_voltage, BLE_QIOT_DATA_TYPE_FLOAT},
};

static int ble_event_get_hardware_fault_name(char *data, uint16_t buf_len)
{
    memcpy(data, "hardware_fault", sizeof("hardware_fault") - 1);

    return sizeof("hardware_fault") - 1;
}

static int ble_event_get_hardware_fault_error_code(char *data, uint16_t buf_len)
{
    int error_code = HTONL(1024);

    memcpy(data, &error_code, sizeof(int));

    return sizeof(int);
}

static ble_event_param sg_ble_event_hardware_fault_array[BLE_QIOT_EVENT_HARDWARE_FAULT_PARAM_ID_BUTT] = {
    {ble_event_get_hardware_fault_name, BLE_QIOT_DATA_TYPE_STRING},
    {ble_event_get_hardware_fault_error_code, BLE_QIOT_DATA_TYPE_INT},
};

static ble_event_t sg_ble_event_array[BLE_QIOT_EVENT_ID_BUTT] = {
    {sg_ble_event_status_report_array, sizeof(sg_ble_event_status_report_array) / sizeof(ble_event_param)},
    {sg_ble_event_low_voltage_array, sizeof(sg_ble_event_low_voltage_array) / sizeof(ble_event_param)},
    {sg_ble_event_hardware_fault_array, sizeof(sg_ble_event_hardware_fault_array) / sizeof(ble_event_param)},
};

int ble_event_get_id_array_size(uint8_t event_id)
{
    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return -1;
    }

    return sg_ble_event_array[event_id].array_size;
}

uint8_t ble_event_get_param_id_type(uint8_t event_id, uint8_t param_id)
{
    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    if (param_id >= sg_ble_event_array[event_id].array_size) {
        ble_qiot_log_e("invalid param id %d", param_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }

    return sg_ble_event_array[event_id].event_array[param_id].type;
}

int ble_event_get_data_by_id(uint8_t event_id, uint8_t param_id, char *out_buf, uint16_t buf_len)
{
    int ret_len = 0;

    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return -1;
    }
    if (param_id >= sg_ble_event_array[event_id].array_size) {
        ble_qiot_log_e("invalid param id %d", param_id);
        return -1;
    }
    if (NULL == sg_ble_event_array[event_id].event_array[param_id].get_cb) {
        ble_qiot_log_e("invalid callback, event id %d, param id %d", event_id, param_id);
        return 0;
    }

    if (!ble_check_space_enough_by_type(sg_ble_event_array[event_id].event_array[param_id].type, buf_len)) {
        ble_qiot_log_e("not enough space get data, event id %d, param id %d", event_id, param_id);
        return -1;
    }
    ret_len = sg_ble_event_array[event_id].event_array[param_id].get_cb(out_buf, buf_len);
    if (ret_len < 0) {
        ble_qiot_log_e("get event data failed, event id %d, param id %d", event_id, param_id);
        return -1;
    } else {
        if (ble_check_ret_value_by_type(sg_ble_event_array[event_id].event_array[param_id].type, buf_len, ret_len)) {
            return ret_len;
        } else {
            ble_qiot_log_e("evnet data length invalid, event id %d, param id %d, type %d", event_id, param_id,
                           sg_ble_event_array[event_id].event_array[param_id].type);
            return -1;
        }
    }
}

int ble_user_event_reply_handle(uint8_t event_id, uint8_t result)
{
    ble_qiot_log_d("event id %d, reply result %d", event_id, result);

    return BLE_QIOT_RS_OK;
}

static int ble_action_handle_loop_input_cb(e_ble_tlv *input_param_array, uint8_t input_array_size,
                                           uint8_t *output_id_array)
{
    int result = 0;

    if (NULL == input_param_array || NULL == output_id_array) {
        ble_qiot_log_e("invalid param");
        return -1;
    }

    report_reply_blink();
    for (int i = 0; i < input_array_size; i++) {
        ble_qiot_log_d("id %d", input_param_array[i].id);
    }

    memcpy(&result, input_param_array[0].val, sizeof(int));
    result = NTOHL(result);
    ble_qiot_log_d("id %d, val %d", input_param_array[0].id, result);
    action_led_blink(result);

    output_id_array[BLE_QIOT_ACTION_LOOP_OUTPUT_ID_RESULT] = true;

    return 0;
}

static int ble_action_handle_loop_output_cb(uint8_t output_id, char *buf, uint16_t buf_len)
{
    int data_len = 0;
    int i        = 0;

    switch (output_id) {
        case BLE_QIOT_ACTION_LOOP_OUTPUT_ID_RESULT:
            for (i = 0; i < 26 * 3; i++) {
                buf[i] = 'a' + (i % 26);
            }
            data_len = i;
            break;
        default:
            break;
    }
    return data_len;
}

static uint8_t sg_ble_action_loop_input_type_array[BLE_QIOT_ACTION_LOOP_INPUT_ID_BUTT] = {
    BLE_QIOT_DATA_TYPE_INT,
};

static uint8_t sg_ble_action_loop_output_type_array[BLE_QIOT_ACTION_LOOP_OUTPUT_ID_BUTT] = {
    BLE_QIOT_DATA_TYPE_STRING,
};

static ble_action_t sg_ble_action_array[BLE_QIOT_ACTION_ID_BUTT] = {
    {ble_action_handle_loop_input_cb, ble_action_handle_loop_output_cb, sg_ble_action_loop_input_type_array,
     sg_ble_action_loop_output_type_array, sizeof(sg_ble_action_loop_input_type_array) / sizeof(uint8_t),
     sizeof(sg_ble_action_loop_output_type_array) / sizeof(uint8_t)},
};

uint8_t ble_action_get_intput_type_by_id(uint8_t action_id, uint8_t input_id)
{
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    if (input_id >= sg_ble_event_array[action_id].array_size) {
        ble_qiot_log_e("invalid input id %d", input_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }

    return sg_ble_action_array[action_id].input_type_array[input_id];
}

uint8_t ble_action_get_output_type_by_id(uint8_t action_id, uint8_t output_id)
{
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    if (output_id >= sg_ble_event_array[action_id].array_size) {
        ble_qiot_log_e("invalid output id %d", output_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }

    return sg_ble_action_array[action_id].output_type_array[output_id];
}

int ble_action_get_input_id_size(uint8_t action_id)
{
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return -1;
    }

    return sg_ble_action_array[action_id].input_id_size;
}

int ble_action_get_output_id_size(uint8_t action_id)
{
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return -1;
    }

    return sg_ble_action_array[action_id].output_id_size;
}

int ble_action_user_handle_input_param(uint8_t action_id, e_ble_tlv *input_param_array, uint8_t input_array_size,
                                       uint8_t *output_id_array)
{
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return -1;
    }

    if (NULL != sg_ble_action_array[action_id].input_cb) {
        if (0 != sg_ble_action_array[action_id].input_cb(input_param_array, input_array_size, output_id_array)) {
            ble_qiot_log_e("input handle error");
            return -1;
        }
    }

    return 0;
}

int ble_action_user_handle_output_param(uint8_t action_id, uint8_t output_id, char *buf, uint16_t buf_len)
{
    int ret_len = 0;

    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return -1;
    }
    if (NULL == sg_ble_action_array[action_id].output_cb) {
        ble_qiot_log_e("invalid callback, action id %d", action_id);
        return 0;
    }

    if (!ble_check_space_enough_by_type(sg_ble_action_array[action_id].output_type_array[output_id], buf_len)) {
        ble_qiot_log_e("not enough space get data, action id %d, output id %d", action_id, output_id);
        return -1;
    }

    ret_len = sg_ble_action_array[action_id].output_cb(output_id, buf, buf_len);
    if (ret_len < 0) {
        ble_qiot_log_e("get action data failed, action id %d, output id %d", action_id, output_id);
        return -1;
    } else {
        if (ble_check_ret_value_by_type(sg_ble_action_array[action_id].output_type_array[output_id], buf_len,
                                        ret_len)) {
            return ret_len;
        } else {
            ble_qiot_log_e("action data length invalid, action id %d, output id %d", action_id, output_id);
            return -1;
        }
    }
}

#ifdef __cplusplus
}
#endif
