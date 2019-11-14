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

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include "shadow_client_json.h"

#include "qcloud_iot_device.h"
#include "shadow_client.h"
#include "lite-utils.h"

static int _direct_update_value(char *value, DeviceProperty *pProperty) {

    int rc = QCLOUD_RET_SUCCESS;

    if (pProperty->type == JBOOL) {
    	rc = LITE_get_boolean(pProperty->data, value);
    } else if (pProperty->type == JINT32) {
    	rc = LITE_get_int32(pProperty->data, value);
    } else if (pProperty->type == JINT16) {
    	rc = LITE_get_int16(pProperty->data, value);
    } else if (pProperty->type == JINT8) {
    	rc = LITE_get_int8(pProperty->data, value);
    } else if (pProperty->type == JUINT32) {
    	rc = LITE_get_uint32(pProperty->data, value);
    } else if (pProperty->type == JUINT16) {
    	rc = LITE_get_uint16(pProperty->data, value);
    } else if (pProperty->type == JUINT8) {
    	rc = LITE_get_uint8(pProperty->data, value);
    } else if (pProperty->type == JFLOAT) {
    	rc = LITE_get_float(pProperty->data, value);
    } else if (pProperty->type == JDOUBLE) {
    	rc = LITE_get_double(pProperty->data, value);
    }else if(pProperty->type == JSTRING){
		//Log_d("string type wait to be deal,%s",value);
	}else if(pProperty->type == JOBJECT){
		//Log_d("Json type wait to be deal,%s",value);
	}else{
		Log_e("pProperty type unknow,%d",pProperty->type);
	}

    return rc;
}

/**
 * add field of client token into JSON
 *
 * @param pJsonDocument             JSON string
 * @param maxSizeOfJsonDocument     max size of JSON string
 * @return                          length after adding
 */
static int32_t _add_client_token(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint32_t *tokenNumber) {

    int32_t rc_of_snprintf = HAL_Snprintf(pJsonDocument, maxSizeOfJsonDocument, "%s-%u", iot_device_info_get()->product_id, (*tokenNumber)++);

    return rc_of_snprintf;
}

/**
 * @brief check return value of snprintf
 *
 * @param returnCode       return value of snprintf
 * @param maxSizeOfWrite   max size of write buffer
 * @return                 QCLOUD_RET_SUCCESS for success, or err code for failure
 */
static inline int _check_snprintf_return(int32_t returnCode, size_t maxSizeOfWrite) {

    if (returnCode >= maxSizeOfWrite) {
        return QCLOUD_ERR_JSON_BUFFER_TRUNCATED;
    } else if (returnCode < 0) {
        return QCLOUD_ERR_JSON;
    }

    return QCLOUD_RET_SUCCESS;
}

int put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type) {

    int rc;
    int32_t rc_of_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\":", pKey);
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);
    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    if (pData == NULL) {
        rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "null,");
    } else {
        if (type == JINT32) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi32
                                      ",", *(int32_t *) (pData));
        } else if (type == JINT16) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi16
                                      ",", *(int16_t *) (pData));
        } else if (type == JINT8) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi8
                                      ",", *(int8_t *) (pData));
        } else if (type == JUINT32) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu32
                                      ",", *(uint32_t *) (pData));
        } else if (type == JUINT16) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu16
                                      ",", *(uint16_t *) (pData));
        } else if (type == JUINT8) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu8
                                      ",", *(uint8_t *) (pData));
        } else if (type == JDOUBLE) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f,", *(double *) (pData));
        } else if (type == JFLOAT) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f,", *(float *) (pData));
		} else if (type == JBOOL) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s,",
                                      *(bool *) (pData) ? "true" : "false");			
        } else if (type == JSTRING) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\",", (char *) (pData));
        } else if (type == JOBJECT) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s,", (char *) (pData));
        }
    }

    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    return rc;
}

int event_put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type) {

    int rc;
    int32_t rc_of_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\":", pKey);
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);
    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    if (pData == NULL) {
        rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "null,");
    } else {
        if (type == JINT32) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi32
                                      ",", *(int32_t *) (pData));
        } else if (type == JINT16) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi16
                                      ",", *(int16_t *) (pData));
        } else if (type == JINT8) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi8
                                      ",", *(int8_t *) (pData));
        } else if (type == JUINT32) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu32
                                      ",", *(uint32_t *) (pData));
        } else if (type == JUINT16) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu16
                                      ",", *(uint16_t *) (pData));
        } else if (type == JUINT8) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu8
                                      ",", *(uint8_t *) (pData));
        } else if (type == JDOUBLE) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f,", *(double *) (pData));
        } else if (type == JFLOAT) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f,", *(float *) (pData));
		} else if (type == JBOOL) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%u,",
                                      *(bool *) (pData) ? 1 : 0);		
        } else if (type == JSTRING) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\",", (char *) (pData));
        } else if (type == JOBJECT) {
            rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s,", (char *) (pData));
        }
    }

    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    return rc;
}


int generate_client_token(char *pStrBuffer, size_t sizeOfBuffer, uint32_t *tokenNumber) {
    return _add_client_token(pStrBuffer, sizeOfBuffer, tokenNumber);
}

void build_empty_json(uint32_t *tokenNumber, char *pJsonBuffer) {
	HAL_Snprintf(pJsonBuffer, MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN, "{\"clientToken\":\"%s-%u\"}", iot_device_info_get()->product_id, (*tokenNumber)++);
}

bool parse_client_token(char *pJsonDoc, char **pClientToken) {
	*pClientToken = LITE_json_value_of(CLIENT_TOKEN_FIELD, pJsonDoc);
	return *pClientToken == NULL ? false : true;
}

bool parse_version_num(char *pJsonDoc, uint32_t *pVersionNumber) {

	bool ret = false;

	char *version_num = LITE_json_value_of(PAYLOAD_VERSION, pJsonDoc);
	if (version_num == NULL) return false;

	if (sscanf(version_num, "%" SCNu32, pVersionNumber) != 1) {
		Log_e("parse shadow version failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
	}
	else {
		ret = true;
	}

	HAL_Free(version_num);

	return ret;
}

bool parse_shadow_state(char *pJsonDoc, char **pState)
{
	*pState = LITE_json_value_of(PAYLOAD_VERSION, pJsonDoc);
	return *pState == NULL ? false : true;
}

bool parse_code_return(char *pJsonDoc, int32_t *pCode) {

	bool ret = false;

	char *code = LITE_json_value_of(REPLY_CODE, pJsonDoc);
	if (code == NULL) return false;

	if (sscanf(code, "%" SCNi32, pCode) != 1) {
		Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
	}
	else {
		ret = true;
	}

	HAL_Free(code);

	return ret;
}

bool parse_status_return(char *pJsonDoc, char **pStatus) {
	*pStatus = LITE_json_value_of(REPLY_STATUS, pJsonDoc);
	return *pStatus == NULL ? false : true;
}


bool parse_shadow_operation_type(char *pJsonDoc, char **pType)
{
	*pType = LITE_json_value_of(TYPE_FIELD, pJsonDoc);
	return *pType == NULL ? false : true;
}

bool parse_shadow_operation_result_code(char *pJsonDoc, int16_t *pResultCode)
{
	bool ret = false;

	char *result_code = LITE_json_value_of(RESULT_FIELD, pJsonDoc);
	if (result_code == NULL) return false;

	if (sscanf(result_code, "%" SCNi16, pResultCode) != 1) {
		Log_e("parse shadow result_code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
	}
	else {
		ret = true;
	}

	HAL_Free(result_code);

	return ret;
}

bool parse_shadow_operation_delta(char *pJsonDoc, char **pDelta)
{
    *pDelta = LITE_json_value_of(PAYLOAD_STATE, pJsonDoc);
	return *pDelta == NULL ? false : true;
}

bool parse_shadow_operation_get(char *pJsonDoc, char **pDelta)
{
    *pDelta = LITE_json_value_of(PAYLOAD_STATE_DELTA, pJsonDoc);
	return *pDelta == NULL ? false : true;
}


bool update_value_if_key_match(char *pJsonDoc, DeviceProperty *pProperty) {

	bool ret = false;

	char* property_data = LITE_json_value_of(pProperty->key, pJsonDoc);
	if ((property_data == NULL) || !(strncmp(property_data, "null", 4))
		 ||!(strncmp(property_data, "NULL", 4))) {
	}
	else {		
		_direct_update_value(property_data, pProperty);
		ret = true;
		HAL_Free(property_data);
	}

	return ret;
}

#ifdef __cplusplus
}
#endif
