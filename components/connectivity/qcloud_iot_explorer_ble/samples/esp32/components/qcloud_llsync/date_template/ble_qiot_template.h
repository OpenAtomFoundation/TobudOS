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
extern "C"{
#endif

#include <stdint.h>


// data type in template, corresponding to type in json file
enum {
  BLE_QIOT_DATA_TYPE_BOOL = 0,
  BLE_QIOT_DATA_TYPE_INT,
  BLE_QIOT_DATA_TYPE_STRING,
  BLE_QIOT_DATA_TYPE_FLOAT,
  BLE_QIOT_DATA_TYPE_ENUM,
  BLE_QIOT_DATA_TYPE_TIME,
  BLE_QIOT_DATA_TYPE_BUTT,
};

// message type, reference data template 
enum {
  BLE_QIOT_MSG_TYPE_PROPERTY = 0,
  BLE_QIOT_MSG_TYPE_EVENT,
  BLE_QIOT_MSG_TYPE_ACTION,
  BLE_QIOT_MSG_TYPE_BUTT,
};

// define property authority, not used
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

// define message flow direction
enum {
  BLE_QIOT_EFFECT_REQUEST = 0,
  BLE_QIOT_EFFECT_REPLY,
  BLE_QIOT_EFFECT_BUTT,
};

// define message type that from server to device
enum {
  BLE_QIOT_DATA_DOWN_REPORT_REPLY = 0,
  BLE_QIOT_DATA_DOWN_CONTROL,
  BLE_QIOT_DATA_DOWN_GET_STATUS_REPLY,
  BLE_QIOT_DATA_DOWN_ACTION,
  BLE_QIOT_DATA_DOWN_EVENT_REPLY,
};

// define message type that from device to server
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

// msg header define, bit 7-6 is msg type, bit 5 means request or reply, bit 4 - 0 is id
#define	BLE_QIOT_PARSE_MSG_HEAD_TYPE(_C)        	(((_C) & 0XFF) >> 6)
#define	BLE_QIOT_PARSE_MSG_HEAD_EFFECT(_C)      	((((_C) & 0XFF) & 0X20) ? BLE_QIOT_EFFECT_REPLY : BLE_QIOT_EFFECT_REQUEST)
#define	BLE_QIOT_PARSE_MSG_HEAD_ID(_C)          	((_C) & 0X1F)
#define	BLE_QIOT_PACKAGE_MSG_HEAD(_TYPE, _REPLY, _ID)	(((_TYPE) << 6) | (((_REPLY) == BLE_QIOT_EFFECT_REPLY) << 5) | ((_ID) & 0X1F))

// tlv header define, bit 7 - 5 is type, bit 4 - 0 depends on type of data template
#define	BLE_QIOT_PARSE_TLV_HEAD_TYPE(_C)        	(((_C) & 0XFF) >> 5)
#define	BLE_QIOT_PARSE_TLV_HEAD_ID(_C)          	((_C) & 0X1F)
#define	BLE_QIOT_PACKAGE_TLV_HEAD(_TYPE, _ID)   	(((_TYPE) << 5) | ((_ID) & 0X1F))


// define tlv struct
typedef struct{
  uint8_t type;
  uint8_t id;
  uint16_t len;
  char *val;
}e_ble_tlv;
#define	BLE_QIOT_INCLUDE_PROPERTY

// define property id
enum {
  BLE_QIOT_PROPERTY_ID_CH20_PPM_VALUE = 0,
  BLE_QIOT_PROPERTY_ID_BUTT,
};

// define ch20_ppm_value attributes
#define	BLE_QIOT_PROPERTY_CH20_PPM_VALUE_MIN    	(0)
#define	BLE_QIOT_PROPERTY_CH20_PPM_VALUE_MAX    	(2)
#define	BLE_QIOT_PROPERTY_CH20_PPM_VALUE_START  	(0)
#define	BLE_QIOT_PROPERTY_CH20_PPM_VALUE_STEP   	(0.001)

// define property set handle return 0 if success, other is error
// sdk call the function that inform the server data to the device
typedef int (*property_set_cb)(const char *data, uint16_t len);

// define property get handle. return the data length obtained, -1 is error, 0 is no data
// sdk call the function fetch user data and send to the server, the data should wrapped by user adn skd just transmit
typedef int (*property_get_cb)(char *buf, uint16_t buf_len);

// each property have a struct ble_property_t, make up a array named sg_ble_property_array
typedef struct{
  property_set_cb set_cb;	//set callback
  property_get_cb get_cb;	//get callback
  uint8_t authority;	//property authority
  uint8_t type;	//data type
}ble_property_t;
// property module
#ifdef BLE_QIOT_INCLUDE_PROPERTY
uint8_t ble_get_property_type_by_id(uint8_t id);
int ble_user_property_set_data(const e_ble_tlv *tlv);
int ble_user_property_get_data_by_id(uint8_t id, char *buf, uint16_t buf_len);
int ble_user_property_report_reply_handle(uint8_t result);
#endif

// event module
#ifdef BLE_QIOT_INCLUDE_EVENT
int ble_event_get_id_array_size(uint8_t event_id);
uint8_t ble_event_get_param_id_type(uint8_t event_id, uint8_t param_id);
int ble_event_get_data_by_id(uint8_t event_id, uint8_t param_id, char *out_buf, uint16_t buf_len);
int ble_user_event_reply_handle(uint8_t event_id, uint8_t result);
#endif

// action module
#ifdef BLE_QIOT_INCLUDE_ACTION
uint8_t ble_action_get_intput_type_by_id(uint8_t action_id, uint8_t input_id);
uint8_t ble_action_get_output_type_by_id(uint8_t action_id, uint8_t output_id);
int ble_action_get_input_id_size(uint8_t action_id);
int ble_action_get_output_id_size(uint8_t action_id);
int ble_action_user_handle_input_param(uint8_t action_id, e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array);
int ble_action_user_handle_output_param(uint8_t action_id, uint8_t output_id, char *buf, uint16_t buf_len);
#endif


#ifdef __cplusplus
}
#endif
#endif //BLE_QIOT_TEMPLATE_H_
