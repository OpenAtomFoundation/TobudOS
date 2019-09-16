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

__QCLOUD_INTERNAL__ qcloud_err_t event_json_node_add(char *json_doc, size_t json_doc_size, const char *key, void *data, json_data_type_t type)
{
    qcloud_err_t rc;
    int32_t rc_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
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
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%u,",
                                              *(bool *) (data) ? 1 : 0);
            break;

        case JSON_DATA_TYPE_STRING:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"%s\",", (char *)(data));
            break;

        case JSON_DATA_TYPE_OBJECT:
            rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "%s,", (char *)(data));
            break;
    }

    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
}

__QCLOUD_INTERNAL__ int event_json_return_code_parse(char *json_doc, int32_t *return_code)
{
	int rc = QCLOUD_TRUE;
    char *return_code_str;
    
    return_code_str = LITE_json_value_of(EVENT_REPLY_FIELD_CODE, json_doc);
	if (!return_code_str) {
        return QCLOUD_FALSE;
    }

	if (sscanf(return_code_str, "%" SCNi32, return_code) != 1) {
		QCLOUD_LOG_E("parse code failed, rc: %d", QCLOUD_ERR_JSON_PARSE);
        rc = QCLOUD_FALSE;
	}

	osal_free(return_code_str);

	return rc;
}

__QCLOUD_INTERNAL__ int event_json_status_parse(char *json_doc, char **status)
{
	*status = LITE_json_value_of(EVENT_REPLY_FIELD_STATUS, json_doc);
	return *status != NULL;
}

#ifdef __cplusplus
}
#endif

