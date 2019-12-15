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

#include "ota_lib.h"

#include <string.h>
#include <stdio.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "ota_client.h"

#include "utils_md5.h"
#include "lite-utils.h"

/* Get the specific @key value, and copy to @dest */
/* 0, successful; -1, failed */
static int _qcloud_otalib_get_firmware_fixlen_para(const char *json_doc, const char *key,
                                        char *dest, size_t dest_len)
{
    IOT_FUNC_ENTRY;

    int ret = QCLOUD_RET_SUCCESS;

    char* key_bak = HAL_Malloc(strlen(key) + 1);
    if (key_bak == NULL) {
    	Log_e("not enough memory for malloc key");
    	ret = IOT_OTA_ERR_FAIL;
    	IOT_FUNC_EXIT_RC(ret);
    }

    char* json_doc_bak = HAL_Malloc(strlen(json_doc) + 1);
    if (json_doc_bak == NULL) {
    	Log_e("not enough memory for malloc json");
    	HAL_Free(key_bak);
    	ret = IOT_OTA_ERR_FAIL;
		IOT_FUNC_EXIT_RC(ret);
    }

    strcpy(key_bak, key);
    strcpy(json_doc_bak, json_doc);

    char* value = LITE_json_value_of(key_bak, json_doc_bak);
    if (value == NULL) {
    	Log_e("Not '%s' key in json doc of OTA", key);
    	ret = IOT_OTA_ERR_FAIL;
    }
    else {
    	uint32_t val_len = strlen(value);
		if (val_len > dest_len) {
			Log_e("value length of the key is too long");
			ret = IOT_OTA_ERR_FAIL;
		}
		else {
	    	memcpy(dest, value, val_len);
	    	ret = QCLOUD_RET_SUCCESS;
		}

    	HAL_Free(value);
    }

	if (key_bak != NULL) {
		HAL_Free(key_bak);
	}
	if (json_doc_bak != NULL) {
		HAL_Free(json_doc_bak);
	}

    IOT_FUNC_EXIT_RC(ret);
}


/* Get variant length parameter of firmware, and copy to @dest */
/* 0, successful; -1, failed */
static int _qcloud_otalib_get_firmware_varlen_para(const char *json_doc, const char *key, char **dest)
{
#define OTA_FIRMWARE_JSON_VALUE_MAX_LENGTH (64)

    IOT_FUNC_ENTRY;

    int ret = QCLOUD_RET_SUCCESS;

    char* key_bak = HAL_Malloc(strlen(key) + 1);
    if (key_bak == NULL) {
    	Log_e("not enough memory for malloc key");
    	ret = IOT_OTA_ERR_FAIL;
    	IOT_FUNC_EXIT_RC(ret);
    }

    char* json_doc_bak = HAL_Malloc(strlen(json_doc) + 1);
    if (json_doc_bak == NULL) {
    	Log_e("not enough memory for malloc json");
    	HAL_Free(key_bak);
    	ret = IOT_OTA_ERR_FAIL;
		IOT_FUNC_EXIT_RC(ret);
    }

    strcpy(key_bak, key);
    strcpy(json_doc_bak, json_doc);

    *dest = LITE_json_value_of(key_bak, json_doc_bak);
    if (*dest == NULL) {
    	Log_e("Not '%s' key in json '%s' doc of OTA", key_bak, json_doc_bak);
    	ret = IOT_OTA_ERR_FAIL;
    }

	if (key_bak != NULL) {
		HAL_Free(key_bak);
	}
	if (json_doc_bak != NULL) {
		HAL_Free(json_doc_bak);
	}

    IOT_FUNC_EXIT_RC(ret);

#undef OTA_FIRMWARE_JSON_VALUE_MAX_LENGTH
}

void *qcloud_otalib_md5_init(void)
{
    iot_md5_context *ctx = HAL_Malloc(sizeof(iot_md5_context));
    if (NULL == ctx) {
        return NULL;
    }

    utils_md5_init(ctx);
    utils_md5_starts(ctx);

    return ctx;
}

void qcloud_otalib_md5_update(void *md5, const char *buf, size_t buf_len)
{
    utils_md5_update(md5, (unsigned char *)buf, buf_len);
}

void qcloud_otalib_md5_finalize(void *md5, char *output_str)
{
    int i;
    unsigned char buf_out[16];
    utils_md5_finish(md5, buf_out);

    for (i = 0; i < 16; ++i) {
        output_str[i * 2] = utils_hb2hex(buf_out[i] >> 4);
        output_str[i * 2 + 1] = utils_hb2hex(buf_out[i]);
    }
    output_str[32] = '\0';
}

void qcloud_otalib_md5_deinit(void *md5)
{
    if (NULL != md5) {
        HAL_Free(md5);
    }
}

int qcloud_otalib_get_firmware_type(const char *json, char **type)
{
    return _qcloud_otalib_get_firmware_varlen_para(json, TYPE_FIELD, type);
}

int qcloud_otalib_get_report_version_result(const char *json)
{
    IOT_FUNC_ENTRY;

    char *result_code = NULL;

    int rc = _qcloud_otalib_get_firmware_varlen_para(json, RESULT_FIELD, &result_code);
    if ( rc != QCLOUD_RET_SUCCESS || strcmp(result_code, "0") != 0) {
        if(NULL != result_code) HAL_Free(result_code); 
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    if(NULL != result_code) HAL_Free(result_code); 
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_otalib_get_params(const char *json, char **type, char **url, char **version, char *md5,
                     uint32_t *fileSize)
{
#define OTA_FILESIZE_STR_LEN    (16)

    IOT_FUNC_ENTRY;

    char file_size_str[OTA_FILESIZE_STR_LEN + 1] = {0};

    /* get type */
    if (0 != _qcloud_otalib_get_firmware_varlen_para(json, TYPE_FIELD, type)) {
        Log_e("get value of type key failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    /* get version */
    if (0 != _qcloud_otalib_get_firmware_varlen_para(json, VERSION_FIELD, version)) {
        Log_e("get value of version key failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    /* get URL */
    if (0 != _qcloud_otalib_get_firmware_varlen_para(json, URL_FIELD, url)) {
        Log_e("get value of url key failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    /* get md5 */
    if (0 != _qcloud_otalib_get_firmware_fixlen_para(json, MD5_FIELD, md5, 32)) {
        Log_e("get value of md5 key failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    /* get file size */
    if (0 != _qcloud_otalib_get_firmware_fixlen_para(json, FILESIZE_FIELD, file_size_str, OTA_FILESIZE_STR_LEN)) {
        Log_e("get value of size key failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    file_size_str[OTA_FILESIZE_STR_LEN] = '\0';
    *fileSize = atoi(file_size_str);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);

#undef OTA_FILESIZE_STR_LEN
}

int qcloud_otalib_gen_info_msg(char *buf, size_t bufLen, uint32_t id, const char *version)
{
    IOT_FUNC_ENTRY;

    int ret;
    ret = HAL_Snprintf(buf,
                       bufLen,
                       "{\"type\": \"report_version\", \"report\":{\"version\":\"%s\"}}",
                       version);

    if (ret < 0) {
        Log_e("HAL_Snprintf failed");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_otalib_gen_report_msg(char *buf, size_t bufLen, uint32_t id, const char *version, int progress, IOT_OTAReportType reportType)
{
    IOT_FUNC_ENTRY;

    int ret;

    switch (reportType) {
        /* report OTA download begin */
        case IOT_OTAR_DOWNLOAD_BEGIN:
        ret = HAL_Snprintf(buf,
                            bufLen,
                            "{\"type\": \"report_progress\", \"report\": {\"progress\": {\"state\":\"downloading\", \"percent\":\"0\", \"result_code\":\"0\", \"result_msg\":\"\"}, \"version\": \"%s\"}}", version);
        break;
        /* report OTA download progress */
        case IOT_OTAR_DOWNLOADING:
        ret = HAL_Snprintf(buf,
                            bufLen,
                            "{\"type\": \"report_progress\", \"report\": {\"progress\": {\"state\":\"downloading\", \"percent\":\"%d\", \"result_code\":\"0\", \"result_msg\":\"\"}, \"version\": \"%s\"}}",
                            progress, version);
        break;
        case IOT_OTAR_DOWNLOAD_TIMEOUT:
        case IOT_OTAR_FILE_NOT_EXIST:
        case IOT_OTAR_MD5_NOT_MATCH:
        case IOT_OTAR_AUTH_FAIL:
        case IOT_OTAR_UPGRADE_FAIL:
        ret = HAL_Snprintf(buf,
                            bufLen,
                            "{\"type\": \"report_progress\", \"report\": {\"progress\": {\"state\":\"fail\", \"result_code\":\"%d\", \"result_msg\":\"time_out\"}, \"version\": \"%s\"}}", reportType, version);
        break;
        /* report OTA upgrade begin */
        case IOT_OTAR_UPGRADE_BEGIN:
        ret = HAL_Snprintf(buf,
                       bufLen,
                       "{\"type\": \"report_progress\", \"report\":{\"progress\":{\"state\":\"burning\", \"result_code\":\"0\", \"result_msg\":\"\"}, \"version\":\"%s\"}}",
                       version);
        break;

        /* report OTA upgrade finish */
        case IOT_OTAR_UPGRADE_SUCCESS:
        ret = HAL_Snprintf(buf,
                       bufLen,
                       "{\"type\": \"report_progress\", \"report\":{\"progress\":{\"state\":\"done\", \"result_code\":\"0\", \"result_msg\":\"\"}, \"version\":\"%s\"}}",
                       version);
        break;
        
        default:
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
        break;
    }


    if (ret < 0) {
        Log_e("HAL_Snprintf failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    } else if (ret >= bufLen) {
        Log_e("msg is too long");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_STR_TOO_LONG);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

#ifdef __cplusplus
}
#endif
