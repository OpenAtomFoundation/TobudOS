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
#include <stdbool.h>
#include <string.h>

#include "ble_qiot_export.h"
#include "ble_qiot_common.h"
#include "ble_qiot_utils_base64.h"
#include "ble_qiot_param_check.h"

// ignore property power switch,
static int ble_property_power_switch_set(const char *data, uint16_t len)
{
    return 0;
}

static int ble_property_power_switch_get(char *data, uint16_t buf_len)
{
    return 0;
}

// ignore heart rate set function, cause its read-only
static int ble_property_heart_rate_set(const char *data, uint16_t len)
{
    return 0;
}

static int32_t sg_heart_rate = 128;
static int ble_property_heart_rate_get(char *data, uint16_t buf_len)
{
    // get your heart rate and copy to the buf which data pointed
    // you must convert to network order if the data type is int or enum
    int32_t heart_rate = HTONL(sg_heart_rate);
    memcpy(data, &heart_rate, sizeof(int32_t));

    // if get data success, return the length of data. must 4 bytes for int type
    // if get data error, return -1
    // if the data no need report, just return 0
    return sizeof(uint32_t);
}

// ignore blood oxygen set function, cause its read-only
static int ble_property_blood_oxygen_set(const char *data, uint16_t len)
{
    return 0;
}

static int32_t sg_blood_oxygen = 96;
static int ble_property_blood_oxygen_get(char *data, uint16_t buf_len)
{
    // copy your blood_oxygen to data
    int32_t blood_oxygen = HTONL(sg_blood_oxygen);
    memcpy(data, &blood_oxygen, sizeof(int32_t));

    return sizeof(uint32_t);
}

static ble_property_t sg_ble_property_array[BLE_QIOT_PROPERTY_ID_BUTT] = {
        {ble_property_power_switch_set, ble_property_power_switch_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_BOOL},
        {ble_property_heart_rate_set,   ble_property_heart_rate_get,   BLE_QIOT_PROPERTY_AUTH_READ, BLE_QIOT_DATA_TYPE_INT},
        {ble_property_blood_oxygen_set, ble_property_blood_oxygen_get, BLE_QIOT_PROPERTY_AUTH_READ, BLE_QIOT_DATA_TYPE_INT},
};

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
        }else {
            return BLE_QIOT_RS_OK;
        }
    }
    ble_qiot_log_e("invalid set callback, id %d", tlv->id);

    return BLE_QIOT_RS_ERR;
}

int ble_user_property_get_data_by_id(uint8_t id, char *buf, uint16_t buf_len)
{
    POINTER_SANITY_CHECK(buf, BLE_QIOT_RS_ERR_PARA);

    if (NULL != sg_ble_property_array[id].get_cb) {
        return sg_ble_property_array[id].get_cb(buf, buf_len);
    }
    ble_qiot_log_e("invalid get callback, property id %d", id);

    return 0;
}

int ble_user_property_report_reply_handle(uint8_t result)
{
    ble_qiot_log_d("report reply result %d", result);

    return BLE_QIOT_RS_OK;
}

//of course you can get all param data, here is just a sample and i'm a bit lazy, so i set some event param return 0 means no data to report
static int ble_event_get_sleep_report_total_sleep_time(char *data, uint16_t buf_len)
{
    return 0;
}

static int32_t sg_falling_asleep_time = 1599557269;
static int ble_event_get_sleep_report_falling_asleep_time(char *data, uint16_t buf_len)
{
    int32_t time = 0;

    time = HTONL(sg_falling_asleep_time);
    ble_qiot_log_i("time %d", time);
    memcpy(data, &time, sizeof(int32_t));

    return sizeof(uint32_t);
}

static int ble_event_get_sleep_report_wakeup_time(char *data, uint16_t buf_len)
{
    return 0;
}

static int ble_event_get_sleep_report_deep_sleep_time(char *data, uint16_t buf_len)
{
    return 0;
}

static int ble_event_get_sleep_report_light_sleep_time(char *data, uint16_t buf_len)
{
    return 0;
}

static int32_t sg_sleep_score = 64;
static int ble_event_get_sleep_report_sleep_score(char *data, uint16_t buf_len)
{
    int32_t score = 0;

    score = HTONL(sg_sleep_score);
    ble_qiot_log_i("score %d", score);
    memcpy(data, &score, sizeof(int32_t));

    return sizeof(uint32_t);
}

static ble_event_param sg_ble_event_sleep_report_array[BLE_QIOT_EVENT_SLEEP_REPORT_PARAM_ID_BUTT] = {
        {ble_event_get_sleep_report_total_sleep_time,  BLE_QIOT_DATA_TYPE_INT},
        {ble_event_get_sleep_report_falling_asleep_time,  BLE_QIOT_DATA_TYPE_TIME},
        {ble_event_get_sleep_report_wakeup_time,  BLE_QIOT_DATA_TYPE_TIME},
        {ble_event_get_sleep_report_deep_sleep_time,  BLE_QIOT_DATA_TYPE_INT},
        {ble_event_get_sleep_report_light_sleep_time,  BLE_QIOT_DATA_TYPE_INT},
        {ble_event_get_sleep_report_sleep_score,  BLE_QIOT_DATA_TYPE_INT},
};

typedef struct sleep_data_{
    int time;
    uint8_t state;
    uint8_t rsv;
}__attribute__((packed))sleep_data;

static int ble_event_get_sleep_data_data(char *data, uint16_t buf_len)
{
    int i = 0;
    sleep_data user_data;
    int base_time = 1599555600;	// 2020/9/8 17:00:00
    int ret_len = 0;
    char base64_out_buf[16] = {0};
    size_t base64_out_len = 0;

    for (i = 0; i < 60; i++){
        user_data.time = HTONL(base_time + i * 60);	// 1 minute time step
        user_data.state = 1;	// light sleep
        user_data.rsv = 0;

#if 1 == BLE_QIOT_POST_BASE64
        memset(base64_out_buf, 0, sizeof(base64_out_buf));
		base64_out_len = 0;
		if (BLE_QIOT_RS_OK != qcloud_iot_utils_base64encode((uint8_t *)base64_out_buf, sizeof(base64_out_buf), &base64_out_len, (const uint8_t *)&user_data, sizeof(user_data))){
			ble_qiot_log_e("event base64 failed");
            return -1;
		}
		memcpy(data + ret_len, base64_out_buf, base64_out_len);
		ret_len += base64_out_len;
#else
        memcpy(data + ret_len, &user_data, sizeof(user_data));
        ret_len += sizeof(user_data);
#endif
    }
    ble_qiot_log_e("ret_len %d, base64_out_len %d", ret_len, base64_out_len);
    return ret_len;
}

static ble_event_param sg_ble_event_sleep_data_array[BLE_QIOT_EVENT_SLEEP_DATA_PARAM_ID_BUTT] = {
        {ble_event_get_sleep_data_data,   BLE_QIOT_DATA_TYPE_STRING},
};

static int ble_event_get_heart_rate_data_data(char *data, uint16_t buf_len)
{
    return 0;
}

static ble_event_param sg_ble_event_heart_rate_data_array[BLE_QIOT_EVENT_HEART_RATE_DATA_PARAM_ID_BUTT] = {
        {ble_event_get_heart_rate_data_data,  BLE_QIOT_DATA_TYPE_STRING},
};

static int ble_event_get_blood_oxygen_data_data(char *data, uint16_t buf_len)
{
    return 0;
}

static ble_event_param sg_ble_event_blood_oxygen_data_array[BLE_QIOT_EVENT_BLOOD_OXYGEN_DATA_PARAM_ID_BUTT] = {
        {ble_event_get_blood_oxygen_data_data,  BLE_QIOT_DATA_TYPE_STRING},
};

static int ble_event_get_today_activity_data(char *data, uint16_t buf_len)
{
    return 0;
}

static ble_event_param sg_ble_event_today_activity_array[BLE_QIOT_EVENT_TODAY_ACTIVITY_PARAM_ID_BUTT] = {
        {ble_event_get_today_activity_data,  BLE_QIOT_DATA_TYPE_STRING},
};

static ble_event_t sg_ble_event_array[BLE_QIOT_EVENT_ID_BUTT] = {
        {sg_ble_event_sleep_report_array, sizeof(sg_ble_event_sleep_report_array) / sizeof(ble_event_param)},
        {sg_ble_event_sleep_data_array,   sizeof(sg_ble_event_sleep_data_array) / sizeof(ble_event_param)},
        {sg_ble_event_heart_rate_data_array, sizeof(sg_ble_event_heart_rate_data_array) / sizeof(ble_event_param)},
        {sg_ble_event_blood_oxygen_data_array, sizeof(sg_ble_event_blood_oxygen_data_array) / sizeof(ble_event_param)},
        {sg_ble_event_today_activity_array, sizeof(sg_ble_event_today_activity_array) / sizeof(ble_event_param)},
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
        return -1;
    }

    return sg_ble_event_array[event_id].event_array[param_id].get_cb(out_buf, buf_len);
}

int ble_user_event_reply_handle(uint8_t event_id, uint8_t result)
{
    ble_qiot_log_d("event id %d, reply result %d", event_id, result);

    return BLE_QIOT_RS_OK;
}

bool ble_check_space_enough_by_type(uint8_t type, uint32_t left_size)
{
    switch(type)
    {
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

typedef struct read_property{
    uint16_t property_id;
    uint16_t val;
}__attribute__((packed))read_property;

static int sg_read_property_id = 0;
static int ble_action_handle_read_property_input_cb(e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array)
{
    int i = 0;
    int result = 0;

    for (i = 0; i < input_array_size; i++){
        ble_qiot_log_d("input id %d", input_param_array[i].id);
    }
    memcpy(&result, input_param_array[0].val, sizeof(int));
    sg_read_property_id = NTOHL(result);
    // if the input tirger
    output_id_array[BLE_QIOT_ACTION_READ_PROPERTY_OUTPUT_ID_VALUE] = true;

    return 0;
}

static int ble_action_handle_read_property_output_cb(uint8_t output_id, char *buf, uint16_t buf_len)
{
    read_property user_data;
    uint8_t base64_out_buf[16] = {0};
    size_t base64_out_len = 0;
    int ret_len = 0;

    if (BLE_QIOT_PROPERTY_ID_HEART_RATE == sg_read_property_id){
        uint16_t heart_rate = sg_heart_rate;
        user_data.property_id = HTONS(sg_read_property_id);
        user_data.val = HTONS(heart_rate);
    }else{
        uint16_t blood_oxygen = sg_blood_oxygen;
        user_data.property_id = HTONS(sg_read_property_id);
        user_data.val = HTONS(blood_oxygen);
    }

#if 1 == BLE_QIOT_POST_BASE64
    memset(base64_out_buf, 0, sizeof(base64_out_buf));
	if (BLE_QIOT_RS_OK != qcloud_iot_utils_base64encode((uint8_t *)base64_out_buf, sizeof(base64_out_buf), &base64_out_len, (const uint8_t *)&user_data, sizeof(user_data))){
		ble_qiot_log_e("action base64 failed");
		return -1;
	}
	memcpy(buf, base64_out_buf, base64_out_len);
	ret_len = base64_out_len;
#else
    memcpy(buf , &user_data, sizeof(user_data));
    ret_len = sizeof(user_data);
#endif

    return ret_len;
}

static uint8_t sg_ble_action_read_property_input_type_array[BLE_QIOT_ACTION_READ_PROPERTY_INPUT_ID_BUTT] = {
        BLE_QIOT_DATA_TYPE_INT,
};

static uint8_t sg_ble_action_read_property_output_type_array[BLE_QIOT_ACTION_READ_PROPERTY_OUTPUT_ID_BUTT] = {
        BLE_QIOT_DATA_TYPE_STRING,
};

static ble_action_t sg_ble_action_array[BLE_QIOT_ACTION_ID_BUTT] = {
        {ble_action_handle_read_property_input_cb, ble_action_handle_read_property_output_cb,
                sg_ble_action_read_property_input_type_array, sg_ble_action_read_property_output_type_array,
                sizeof(sg_ble_action_read_property_input_type_array) / sizeof(uint8_t),
                sizeof(sg_ble_action_read_property_output_type_array) / sizeof(uint8_t)},
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

int ble_action_user_handle_input_param(uint8_t action_id, e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array)
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
    if (action_id >= BLE_QIOT_ACTION_ID_BUTT) {
        ble_qiot_log_e("invalid action id %d", action_id);
        return -1;
    }

    int data_len = 0;

    if (NULL != sg_ble_action_array[action_id].output_cb) {
        data_len = sg_ble_action_array[action_id].output_cb(output_id, buf, buf_len);
        if (0 > data_len) {
            ble_qiot_log_e("output handle error");
            return -1;
        }
    }

    return data_len;
}


#ifdef __cplusplus
}
#endif
