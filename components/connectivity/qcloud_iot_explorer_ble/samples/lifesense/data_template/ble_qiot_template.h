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
#ifndef BLE_QIOT_TEMPLATE_H_
#define BLE_QIOT_TEMPLATE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BLE_QIOT_SDK_VER "0.1.0"

// data type in template
enum {
    BLE_QIOT_DATA_TYPE_BOOL = 0,
    BLE_QIOT_DATA_TYPE_INT,
    BLE_QIOT_DATA_TYPE_STRING,
    BLE_QIOT_DATA_TYPE_FLOAT,
    BLE_QIOT_DATA_TYPE_ENUM,
    BLE_QIOT_DATA_TYPE_TIME,
    BLE_QIOT_DATA_TYPE_BUTT,
};

// message type
enum {
    BLE_QIOT_MSG_TYPE_PROPERTY = 0,
    BLE_QIOT_MSG_TYPE_EVENT,
    BLE_QIOT_MSG_TYPE_ACTION,
    BLE_QIOT_MSG_TYPE_BUTT,
};

// define property authority
enum {
    BLE_QIOT_PROPERTY_AUTH_RW = 0,
    BLE_QIOT_PROPERTY_AUTH_READ,
    BLE_QIOT_PROPERTY_AUTH_BUTT,
};

// define reply result
enum {
    BLE_QIOT_REPLY_SUCCESS = 0,
    BLE_QIOT_REPLY_FAIL,
    BLE_QIOT_REPLY_DATA_ERR,
    BLE_QIOT_REPLY_BUTT,
};

// define message request type
enum {
    BLE_QIOT_EFFECT_REQUEST = 0,
    BLE_QIOT_EFFECT_REPLY,
    BLE_QIOT_EFFECT_BUTT,
};

// define message type from remote
enum {
    BLE_QIOT_DATA_DOWN_REPORT_REPLY = 0,
    BLE_QIOT_DATA_DOWN_CONTROL,
    BLE_QIOT_DATA_DOWN_GET_STATUS_REPLY,
    BLE_QIOT_DATA_DOWN_ACTION,
    BLE_QIOT_DATA_DOWN_EVENT_REPLY,
};

// define msg type up to remote
enum {
    BLE_QIOT_EVENT_UP_PROPERTY_REPORT = 0,
    BLE_QIOT_EVENT_UP_CONTROL_REPLY,
    BLE_QIOT_EVENT_UP_GET_STATUS,
    BLE_QIOT_EVENT_UP_EVENT_POST,
    BLE_QIOT_EVENT_UP_ACTION_REPLY,
    BLE_QIOT_EVENT_UP_BIND_SIGN_RET,
    BLE_QIOT_EVENT_UP_CONN_SIGN_RET,
    BLE_QIOT_EVENT_UP_UNBIND_SIGN_RET,
    BLE_QIOT_EVENT_UP_REPORT_MTU,
    BLE_QIOT_EVENT_UP_BUTT,
};

// bit 7-6 is msg type, bit 5 means request or reply, bit 4 - 0 is id
#define BLE_QIOT_PARSE_MSG_HEAD_TYPE(_C)   (((_C)&0XFF) >> 6)
#define BLE_QIOT_PARSE_MSG_HEAD_EFFECT(_C) ((((_C)&0XFF) & 0X20) ? BLE_QIOT_EFFECT_REPLY : BLE_QIOT_EFFECT_REQUEST)
#define BLE_QIOT_PARSE_MSG_HEAD_ID(_C)     ((_C)&0X1F)
#define BLE_QIOT_PACKAGE_MSG_HEAD(_TYPE, _REPLY, _ID) \
    (((_TYPE) << 6) | (((_REPLY) == BLE_QIOT_EFFECT_REPLY) << 5) | ((_ID)&0X1F))

// parse tlv struct
#define BLE_QIOT_PARSE_TLV_HEAD_TYPE(_C)      (((_C)&0XFF) >> 5)
#define BLE_QIOT_PARSE_TLV_HEAD_ID(_C)        ((_C)&0X1F)
#define BLE_QIOT_PACKAGE_TLV_HEAD(_TYPE, _ID) (((_TYPE) << 5) | ((_ID)&0X1F))
typedef int (*get_string_cb)(char *buf, uint16_t buf_len, int *offset);

// define property id
enum {
    BLE_QIOT_PROPERTY_ID_POWER_SWITCH = 0,
    BLE_QIOT_PROPERTY_ID_HEART_RATE,
    BLE_QIOT_PROPERTY_ID_BLOOD_OXYGEN,
    BLE_QIOT_PROPERTY_ID_BUTT,
};

// define heart_rate attributes
#define BLE_QIOT_PROPERTY_HEART_RATE_MIN   (0)
#define BLE_QIOT_PROPERTY_HEART_RATE_MAX   (255)
#define BLE_QIOT_PROPERTY_HEART_RATE_START (0)

// define blood_oxygen attributes
#define BLE_QIOT_PROPERTY_BLOOD_OXYGEN_MIN   (70)
#define BLE_QIOT_PROPERTY_BLOOD_OXYGEN_MAX   (100)
#define BLE_QIOT_PROPERTY_BLOOD_OXYGEN_START (70)

// define property set handle. return 0 if success, other is error
typedef int (*property_set_cb)(const char *data, uint16_t len);

// define property get handle. return the data length obtained, 0 is error
typedef int (*property_get_cb)(char *buf, uint16_t buf_len);

// define property struct
typedef struct {
    property_set_cb set_cb;     // set callback
    property_get_cb get_cb;     // get callback
    uint8_t         authority;  // property authority
    uint8_t         type;       // data type
} ble_property_t;

// define event id
enum {
    BLE_QIOT_EVENT_ID_SLEEP_REPORT = 0,
    BLE_QIOT_EVENT_ID_SLEEP_DATA,
    BLE_QIOT_EVENT_ID_HEART_RATE_DATA,
    BLE_QIOT_EVENT_ID_BLOOD_OXYGEN_DATA,
    BLE_QIOT_EVENT_ID_TODAY_ACTIVITY,
    BLE_QIOT_EVENT_ID_BUTT,
};

// define event param sleep_report
enum {
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_TOTAL_SLEEP_TIME = 0,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_FALLING_ASLEEP_TIME,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_WAKEUP_TIME,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_DEEP_SLEEP_TIME,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_LIGHT_SLEEP_TIME,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_SLEEP_SCORE,
    BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_BUTT,
};

// define total_sleep_time attributes
#define BLE_QIOT_EVENT_SLEEP_REPORT_TOTAL_SLEEP_TIME_MIN (0)
#define BLE_QIOT_EVENT_SLEEP_REPORT_TOTAL_SLEEP_TIME_MAX (86400)

// define deep_sleep_time attributes
#define BLE_QIOT_EVENT_SLEEP_REPORT_DEEP_SLEEP_TIME_MIN (0)
#define BLE_QIOT_EVENT_SLEEP_REPORT_DEEP_SLEEP_TIME_MAX (1440)

// define light_sleep_time attributes
#define BLE_QIOT_EVENT_SLEEP_REPORT_LIGHT_SLEEP_TIME_MIN (0)
#define BLE_QIOT_EVENT_SLEEP_REPORT_LIGHT_SLEEP_TIME_MAX (1440)

// define sleep_score attributes
#define BLE_QIOT_EVENT_SLEEP_REPORT_SLEEP_SCORE_MIN (-1)
#define BLE_QIOT_EVENT_SLEEP_REPORT_SLEEP_SCORE_MAX (100)

// define event param sleep_data
enum {
    BLE_QIOT_EVENT_SLEEP_DATA_PARAM_ID_DATA = 0,
    BLE_QIOT_EVENT_SLEEP_DATA_PARAM_ID_BUTT,
};

// define data attributes
#define BLE_QIOT_EVENT_SLEEP_DATA_DATA_LEN_MIN (1)
#define BLE_QIOT_EVENT_SLEEP_DATA_DATA_LEN_MAX (2048)

// define event param heart_rate_data
enum {
    BLE_QIOT_EVENT_HEART_RATE_DATA_PARAM_ID_DATA = 0,
    BLE_QIOT_EVENT_HEART_RATE_DATA_PARAM_ID_BUTT,
};

// define data attributes
#define BLE_QIOT_EVENT_HEART_RATE_DATA_DATA_LEN_MIN (1)
#define BLE_QIOT_EVENT_HEART_RATE_DATA_DATA_LEN_MAX (2048)

// define event param blood_oxygen_data
enum {
    BLE_QIOT_EVENT_BLOOD_OXYGEN_DATA_PARAM_ID_DATA = 0,
    BLE_QIOT_EVENT_BLOOD_OXYGEN_DATA_PARAM_ID_BUTT,
};

// define data attributes
#define BLE_QIOT_EVENT_BLOOD_OXYGEN_DATA_DATA_LEN_MIN (1)
#define BLE_QIOT_EVENT_BLOOD_OXYGEN_DATA_DATA_LEN_MAX (2048)

// define event param today_activity
enum {
    BLE_QIOT_EVENT_TODAY_ACTIVITY_PARAM_ID_DATA = 0,
    BLE_QIOT_EVENT_TODAY_ACTIVITY_PARAM_ID_BUTT,
};

// define data attributes
#define BLE_QIOT_EVENT_TODAY_ACTIVITY_DATA_LEN_MIN (1)
#define BLE_QIOT_EVENT_TODAY_ACTIVITY_DATA_LEN_MAX (2048)

// define event get handle. return 0 if success, other is error
typedef int (*event_get_cb)(char *buf, uint16_t buf_len);

// define param struct of event
typedef struct {
    event_get_cb get_cb;  // get param data callback
    uint8_t      type;    // param type
} ble_event_param;

// define event struct
typedef struct {
    ble_event_param *event_array;  // array of params data
    uint8_t          array_size;   // array size
} ble_event_t;

// define action id
enum {
    BLE_QIOT_ACTION_ID_READ_PROPERTY = 0,
    BLE_QIOT_ACTION_ID_BUTT,
};

// define action input read_property
enum {
    BLE_QIOT_ACTION_READ_PROPERTY_INPUT_ID_ID = 0,
    BLE_QIOT_ACTION_READ_PROPERTY_INPUT_ID_BUTT,
};

// define action output read_property
enum {
    BLE_QIOT_ACTION_READ_PROPERTY_OUTPUT_ID_VALUE = 0,
    BLE_QIOT_ACTION_READ_PROPERTY_OUTPUT_ID_BUTT,
};
#define BLE_QIOT_ACTION_INPUT_READ_PROPERTY_ID_MIN   (0)
#define BLE_QIOT_ACTION_INPUT_READ_PROPERTY_ID_MAX   (2)
#define BLE_QIOT_ACTION_INPUT_READ_PROPERTY_ID_START (0)
#define BLE_QIOT_ACTION_INPUT_READ_PROPERTY_ID_STEP  (1)

// define value attributes
#define BLE_QIOT_ACTION_OUTPUT_READ_PROPERTY_VALUE_MIN (0)
#define BLE_QIOT_ACTION_OUTPUT_READ_PROPERTY_VALUE_MAX (255)

// define max input id and output id
#define BLE_QIOT_ACTION_INPUT_ID_BUTT  1
#define BLE_QIOT_ACTION_OUTPUT_ID_BUTT 1

// define tlv struct
typedef struct {
    uint8_t  type;
    uint8_t  id;
    uint16_t len;
    char *   val;
} e_ble_tlv;

// define action input handle, return 0 is success, other is error. output_id_array filling with id number that need
// obtained
typedef int (*action_input_handle)(e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array);

// define action output handle, return length of the data
typedef int (*action_output_handle)(uint8_t output_id, char *buf, uint16_t buf_len);

// define action id struct
typedef struct {
    action_input_handle  input_cb;           // handle input data
    action_output_handle output_cb;          // get output data in the callback
    uint8_t *            input_type_array;   // type array for input id
    uint8_t *            output_type_array;  // type array for output id
    uint8_t              input_id_size;      // numbers of input id
    uint8_t              output_id_size;     // numbers of output id
} ble_action_t;
// property module
uint8_t ble_get_property_type_by_id(uint8_t id);
int     ble_user_property_set_data(const e_ble_tlv *tlv);
int     ble_user_property_get_data_by_id(uint8_t id, char *buf, uint16_t buf_len);
int     ble_user_property_report_reply_handle(uint8_t result);

// event module
int     ble_event_get_id_array_size(uint8_t event_id);
uint8_t ble_event_get_param_id_type(uint8_t event_id, uint8_t param_id);
int     ble_event_get_data_by_id(uint8_t event_id, uint8_t param_id, char *out_buf, uint16_t buf_len);
int     ble_user_event_reply_handle(uint8_t event_id, uint8_t result);

// action module
uint8_t ble_action_get_intput_type_by_id(uint8_t action_id, uint8_t input_id);
uint8_t ble_action_get_output_type_by_id(uint8_t action_id, uint8_t output_id);
int     ble_action_get_input_id_size(uint8_t action_id);
int     ble_action_get_output_id_size(uint8_t action_id);
int     ble_action_user_handle_input_param(uint8_t action_id, e_ble_tlv *input_param_array, uint8_t input_array_size,
                                           uint8_t *output_id_array);
int     ble_action_user_handle_output_param(uint8_t action_id, uint8_t output_id, char *buf, uint16_t buf_len);

#ifdef __cplusplus
}
#endif
#endif  // BLE_QIOT_TEMPLATE_H_
