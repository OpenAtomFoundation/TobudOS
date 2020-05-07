/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IOT_SHADOW_CLIENT_JSON_H_
#define IOT_SHADOW_CLIENT_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#define VERSION_FIELD      "version"
#define TYPE_FIELD         "type"
#define CLIENT_TOKEN_FIELD "clientToken"
#define RESULT_FIELD       "result"

#define OPERATION_DELTA  "delta"
#define OPERATION_GET    "get"
#define OPERATION_UPDATE "update"

#define PAYLOAD_STATE       "payload.state"
#define PAYLOAD_VERSION     "payload.version"
#define PAYLOAD_STATE_DELTA "payload.state.delta"

#define REPLY_CODE   "code"
#define REPLY_STATUS "status"

/**
 * add a JSON node to JSON string
 *
 * @param jsonBuffer    JSON string buffer
 * @param sizeOfBuffer  size of buffer
 * @param pKey          key of JSON node
 * @param pData         value of JSON node
 * @param type          value type of JSON node
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);

/**
 * add a event JSON node to JSON string
 *
 * @param jsonBuffer    JSON string buffer
 * @param sizeOfBuffer  size of buffer
 * @param pKey          key of JSON node
 * @param pData         value of JSON node
 * @param type          value type of JSON node
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int event_put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);

/**
 * @brief generate a ClientToken
 *
 * @param pStrBuffer    string buffer
 * @param sizeOfBuffer  size of buffer
 * @param tokenNumber   shadow token number, increment every time
 * @param product_id    device product ID
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int generate_client_token(char *pStrBuffer, size_t sizeOfBuffer, uint32_t *tokenNumber, char *product_id);

/**
 * @brief generate an empty JSON with only clientToken
 *
 * @param tokenNumber   shadow token number, increment every time
 * @param pJsonBuffer   JSON string buffer
 * @param product_id    device product ID
 */
void build_empty_json(uint32_t *tokenNumber, char *pJsonBuffer, char *product_id);

/**
 * @brief parse field of clientToken from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pClientToken   pointer to field of ClientToken
 * @return               true for success
 */
bool parse_client_token(char *pJsonDoc, char **pClientToken);

/**
 * @brief parse field of status from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pStatus        pointer to field of status
 * @return               true for success
 */
bool parse_status_return(char *pJsonDoc, char **pStatus);

/**
 * @brief parse field of code from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pCode          pointer to field of Code
 * @return               true for success
 */
bool parse_code_return(char *pJsonDoc, int32_t *pCode);

/**
 * @brief parse field of version from JSON string
 *
 * @param pJsonDoc       source JSON string

 * @param pVersionNumber pointer to version
 * @return               true for success
 */
bool parse_version_num(char *pJsonDoc, uint32_t *pVersionNumber);

/**
 * @brief parse field of state from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pState         pointer to field of state
 * @return               true for success
 */
bool parse_shadow_state(char *pJsonDoc, char **pState);

/**
 * @brief parse field of type from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pType          pointer to field of tyde
 * @return               true for success
 */
bool parse_shadow_operation_type(char *pJsonDoc, char **pType);

/**
 * @brief parse field of result from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pResultCode    pointer to result code
 * @return               true for success
 */
bool parse_shadow_operation_result_code(char *pJsonDoc, int16_t *pResultCode);

/**
 * @brief parse field of delta from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pDelta         pointer to field of delta
 * @return               true for success
 */
bool parse_shadow_operation_delta(char *pJsonDoc, char **pDelta);

/**
 * @brief parse field of get from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pDelta         pointer to field of delta
 * @return               true for success
 */
bool parse_shadow_operation_get(char *pJsonDoc, char **pDelta);

/**
 * @brief update value in JSON if key is matched, not for OBJECT type
 *
 * @param pJsonDoc       JSON string
 * @param pProperty      device property
 * @return               true for success
 */
bool update_value_if_key_match(char *pJsonDoc, DeviceProperty *pProperty);

#ifdef __cplusplus
}
#endif

#endif  // IOT_SHADOW_CLIENT_JSON_H_
