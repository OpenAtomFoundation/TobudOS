/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

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

#include "qcloud.h"

#define PRIi32      "i"
#define PRIi16      "i"
#define PRIi8       "i"
#define PRIu32      "u"
#define PRIu16      "u"
#define PRIu8       "u"
#define SCNi8       "hhi"
#define SCNu8       "hhu"
#define SCNi16      "hi"
#define SCNu16      "hu"
#define SCNi32      "i"
#define SCNu32      "u"

__QCLOUD_STATIC__ qcloud_err_t shadow_json_value_do_update(char *value, shadow_dev_property_t *dev_property)
{
    switch (dev_property->type) {
        case JSON_DATA_TYPE_BOOL:
            return LITE_get_boolean(dev_property->data, value);

        case JSON_DATA_TYPE_INT32:
            return LITE_get_int32(dev_property->data, value);

        case JSON_DATA_TYPE_INT16:
            return LITE_get_int16(dev_property->data, value);

        case JSON_DATA_TYPE_INT8:
            return LITE_get_int8(dev_property->data, value);

        case JSON_DATA_TYPE_UINT32:
            return LITE_get_uint32(dev_property->data, value);

        case JSON_DATA_TYPE_UINT16:
            return LITE_get_uint16(dev_property->data, value);

        case JSON_DATA_TYPE_UINT8:
            return LITE_get_uint8(dev_property->data, value);

        case JSON_DATA_TYPE_FLOAT:
            return LITE_get_float(dev_property->data, value);

        case JSON_DATA_TYPE_DOUBLE:
            return LITE_get_double(dev_property->data, value);

        case JSON_DATA_TYPE_STRING:
        case JSON_DATA_TYPE_OBJECT:
            QCLOUD_LOG_D("string/object to be deal, %d %s", dev_property->type, value);
            return QCLOUD_ERR_SUCCESS;

        default:
            QCLOUD_LOG_E("unknow type, %d",dev_property->type);
            return QCLOUD_ERR_FAILURE;
    }
}

/**
 * @brief 检查函数snprintf的返回值
 *
 * @param returnCode       函数snprintf的返回值
 * @param maxSizeOfWrite   可写最大字节数
 * @return                 返回QCLOUD_ERR_JSON, 表示出错; 返回QCLOUD_ERR_JSON_BUFFER_TRUNCATED, 表示截断
 */
__QCLOUD_INTERNAL__ qcloud_err_t shadow_json_snprintf_rc2errno(int rc, size_t write_max)
{
    if (rc < 0) {
        return QCLOUD_ERR_JSON;
    }

    if (rc >= write_max) {
        return QCLOUD_ERR_JSON_BUFFER_TRUNCATED;
    }

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_json_client_token_generate(char *json_doc, size_t json_doc_size, uint32_t token_num, char *device_product_id)
{
    int rc_snprintf;

    rc_snprintf = osal_snprintf(json_doc, json_doc_size, "%s-%u", device_product_id, token_num);
    return shadow_json_snprintf_rc2errno(rc_snprintf, json_doc_size);
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_json_node_add(char *json_doc, size_t json_doc_size, const char *key, void *data, json_data_type_t type)
{
    qcloud_err_t rc;
    int32_t rc_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"%s\":", key);
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    if (!data) {
        rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "null,");
        QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
    }

    switch (type) {
        case JSON_DATA_TYPE_INT32:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIi32
                                            ",", *(int32_t *)(data));
            break;

        case JSON_DATA_TYPE_INT16:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIi16
                                            ",", *(int16_t *)(data));
            break;

        case JSON_DATA_TYPE_INT8:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIi8
                                            ",", *(int8_t *)(data));
            break;


        case JSON_DATA_TYPE_UINT32:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIu32
                                            ",", *(uint32_t *)(data));
            break;

        case JSON_DATA_TYPE_UINT16:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIu16
                                            ",", *(uint16_t *)(data));
            break;

        case JSON_DATA_TYPE_UINT8:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%"
                                            PRIu8
                                            ",", *(uint8_t *)(data));
            break;

        case JSON_DATA_TYPE_DOUBLE:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%f,", *(double *)(data));
            break;

        case JSON_DATA_TYPE_FLOAT:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%f,", *(float *)(data));
            break;

        case JSON_DATA_TYPE_BOOL:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%s,", *(bool *)(data) ? "true" : "false");
            break;

        case JSON_DATA_TYPE_STRING:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"%s\",", (char *)(data));
            break;

        case JSON_DATA_TYPE_OBJECT:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%s,", (char *)(data));
            break;
    }

    return shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_json_empty_doc_build(char *json_doc, uint32_t token_num, char *device_product_id)
{
    int rc_snprintf;

	rc_snprintf = osal_snprintf(json_doc, QCLOUD_SHADOW_JSON_WITH_CLIENT_TOKEN_MAX, "{\"clientToken\":\"%s-%u\"}", device_product_id, token_num);
    return shadow_json_snprintf_rc2errno(rc_snprintf, QCLOUD_SHADOW_JSON_WITH_CLIENT_TOKEN_MAX);
}

__QCLOUD_INTERNAL__ int shadow_json_client_token_parse(char *json_doc, char **client_token)
{
	*client_token = LITE_json_value_of(SHADOW_FIELD_CLIENT_TOKEN, json_doc);
	return *client_token != NULL;
}

__QCLOUD_INTERNAL__ int shadow_json_version_parse(char *json_doc, uint32_t *version)
{
	int rc = QCLOUD_TRUE;
    char *version_str;

    version_str = LITE_json_value_of(SHADOW_PAYLOAD_VERSION, json_doc);
	if (!version_str) {
        return QCLOUD_FALSE;
    }

	if (sscanf(version_str, "%" SCNu32, version) != 1) {
		QCLOUD_LOG_E("parse shadow version failed, rc: %d", QCLOUD_ERR_JSON_PARSE);
        rc = QCLOUD_FALSE;
	}

	osal_free(version_str);

	return rc;
}

__QCLOUD_INTERNAL__ int shadow_json_operation_type_parse(char *json_doc, char **type)
{
	*type = LITE_json_value_of(SHADOW_FIELD_TYPE, json_doc);
	return *type != NULL;
}

__QCLOUD_INTERNAL__ int shadow_json_operation_result_code_parse(char *json_doc, int16_t *result_code)
{
	int rc = QCLOUD_TRUE;
    char *result_code_str;

    result_code_str = LITE_json_value_of(SHADOW_FIELD_RESULT, json_doc);
	if (!result_code_str) {
        return QCLOUD_FALSE;
    }

	if (sscanf(result_code_str, "%" SCNi16, result_code) != 1) {
		QCLOUD_LOG_E("parse result code failed, %d", QCLOUD_ERR_JSON_PARSE);
        rc = QCLOUD_FALSE;
	}

	osal_free(result_code_str);

	return rc;
}

__QCLOUD_INTERNAL__ int shadow_json_delta_parse(char *json_doc, char **delta)
{
    *delta = LITE_json_value_of(SHADOW_PAYLOAD_STATE, json_doc);
	return *delta != NULL;
}

__QCLOUD_INTERNAL__ int shadow_json_operation_delta_get(char *json_doc, char **delta)
{
    *delta = LITE_json_value_of(SHADOW_PAYLOAD_STATE_DELTA, json_doc);
	return *delta != NULL;
}

__QCLOUD_INTERNAL__ int shadow_state_parse(char *json_doc, char **state)
{
	*state = LITE_json_value_of(SHADOW_PAYLOAD_VERSION, json_doc);
	return *state != NULL;
}

__QCLOUD_INTERNAL__ int shadow_json_value_update(char *json, shadow_dev_property_t *dev_property)
{
    char *data;

    data = LITE_json_value_of((char *)dev_property->key, json);
	if (!data || strncmp(data, "null", 4) == 0 || strncmp(data, "NULL", 4) == 0) {
        return QCLOUD_FALSE;
	}

	shadow_json_value_do_update(data, dev_property);
	osal_free(data);

	return QCLOUD_TRUE;
}

#ifdef __cplusplus
}
#endif

