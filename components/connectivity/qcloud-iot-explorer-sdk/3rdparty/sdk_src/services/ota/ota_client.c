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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ota_client.h"

#include "qcloud_iot_export.h"
#include "utils_param_check.h"

#include "ota_lib.h"
#include "ota_fetch.h"

#include "utils_timer.h"

#define OTA_VERSION_STR_LEN_MIN     (1)
#define OTA_VERSION_STR_LEN_MAX     (32)

typedef struct  {
    const char 				*product_id;     		/* point to product id */
    const char 				*device_name;    		/* point to device name */

    uint32_t 				id;                		/* message id */
    IOT_OTA_State_Code 		state;   				/* OTA state */
    uint32_t 				size_last_fetched; 		/* size of last downloaded */
    uint32_t 				size_fetched;      		/* size of already downloaded */
    uint32_t 				size_file;         		/* size of file */

    char 					*purl;                 	/* point to URL */
    char 					*version;              	/* point to string */
    char 					md5sum[33];            	/* MD5 string */

    void 					*md5;                  	/* MD5 handle */
    void 					*ch_signal;           	/* channel handle of signal exchanged with OTA server */
    void 					*ch_fetch;             	/* channel handle of download */

    int 					err;                    /* last error code */

    short					current_signal_type;

    Timer report_timer;

} OTA_Struct_t;


/* check ota progress */
/* return: true, valid progress state; false, invalid progress state. */
static int _ota_check_progress(IOT_OTA_Progress_Code progress)
{
    return ((progress >= IOT_OTAP_BURN_FAILED)
            && (progress <= IOT_OTAP_FETCH_PERCENTAGE_MAX));
}

/* callback when OTA topic msg is received */
static void _ota_callback(void *pcontext, const char *msg, uint32_t msg_len) {
#define OTA_JSON_TYPE_VALUE_LENGTH 64

    char *json_type = NULL;

    OTA_Struct_t *h_ota = (OTA_Struct_t *) pcontext;

    if (h_ota->state >= IOT_OTAS_FETCHING) {
        Log_i("In downloading or downloaded state");
        goto End;
    }

    if (msg == NULL || msg_len <= 0) {
    	Log_e("OTA response message is NULL");
    	return;
    }

    if (qcloud_otalib_get_firmware_type(msg, &json_type) != QCLOUD_RET_SUCCESS) {
        Log_e("Get firmware type failed!");
        goto End;
    }

    if (!strcmp(json_type, REPORT_VERSION_RSP)) {
    	if (qcloud_otalib_get_report_version_result(msg) < QCLOUD_RET_SUCCESS) {
            Log_e("Report version failed!");
            h_ota->err = IOT_OTA_ERR_REPORT_VERSION;
            h_ota->state = IOT_OTAS_FETCHED;
    	}
    	else {
    		Log_i("Report version success!");
    	}
    	goto End;
    }
    else {
        if (strcmp(json_type, UPDATE_FIRMWARE) != 0) {
        	Log_e("Netheir Report version result nor update firmware! type: %s", json_type);
            goto End;
        }

        if(NULL != json_type) {
            HAL_Free(json_type);
            json_type = NULL;
        }

        if (0 != qcloud_otalib_get_params(msg, &json_type, &h_ota->purl, &h_ota->version,
        		h_ota->md5sum, &h_ota->size_file)) {
            Log_e("Get firmware parameter failed");
            goto End;
        }

        h_ota->state = IOT_OTAS_FETCHING;
    }

End:
	if (json_type != NULL) HAL_Free(json_type);

#undef OTA_JSON_TYPE_VALUE_LENGTH
}

static void IOT_OTA_ResetStatus(void *handle)
{
   OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;
   h_ota->state = IOT_OTAS_INITED;
} 

static int IOT_OTA_ReportProgress(void *handle, IOT_OTA_Progress_Code progress, IOT_OTAReportType reportType)
{
#define MSG_REPORT_LEN  (256)

    int ret = QCLOUD_ERR_FAILURE;
    char *msg_reported;
    OTA_Struct_t * h_ota = (OTA_Struct_t *) handle;

    if (NULL == handle) {
        Log_e("handle is NULL");
        return IOT_OTA_ERR_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return QCLOUD_ERR_FAILURE;
    }

    if (!_ota_check_progress(progress)) {
        Log_e("progress is a invalid parameter");
        h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
        return QCLOUD_ERR_FAILURE;
    }

    if (NULL == (msg_reported = HAL_Malloc(MSG_REPORT_LEN))) {
        Log_e("allocate for msg_reported failed");
        h_ota->err = IOT_OTA_ERR_NOMEM;
        return QCLOUD_ERR_FAILURE;
    }

    ret = qcloud_otalib_gen_report_msg(msg_reported, MSG_REPORT_LEN, h_ota->id, h_ota->version, progress, reportType);
    if (0 != ret) {
        Log_e("generate reported message failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = qcloud_osc_report_progress(h_ota->ch_signal, msg_reported);
    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("Report progress failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = QCLOUD_RET_SUCCESS;

do_exit:
    if (NULL != msg_reported) {
        HAL_Free(msg_reported);
    }
    return ret;

#undef MSG_REPORT_LEN
}

static int IOT_OTA_ReportUpgradeResult(void *handle, const char *version, IOT_OTAReportType reportType)
{
#define MSG_UPGPGRADE_LEN  (256)

    POINTER_SANITY_CHECK(handle, IOT_OTA_ERR_INVALID_PARAM);
    POINTER_SANITY_CHECK(version, IOT_OTA_ERR_INVALID_PARAM);
    
    int ret, len;
    char *msg_upgrade;
    OTA_Struct_t *h_ota = (OTA_Struct_t *)handle;

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return QCLOUD_ERR_FAILURE;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        Log_e("version string is invalid: must be [1, 32] chars");
        h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
        return QCLOUD_ERR_FAILURE;
    }

    if (NULL == (msg_upgrade = HAL_Malloc(MSG_UPGPGRADE_LEN))) {
        Log_e("allocate for msg_informed failed");
        h_ota->err = IOT_OTA_ERR_NOMEM;
        return QCLOUD_ERR_FAILURE;
    }

    ret = qcloud_otalib_gen_report_msg(msg_upgrade, MSG_UPGPGRADE_LEN, 1, version, 1, reportType);
    if (ret != 0) {
        Log_e("generate inform message failed");
        h_ota->err = ret;
        ret = QCLOUD_ERR_FAILURE;
        goto do_exit;
    }

    ret = qcloud_osc_report_upgrade_result(h_ota->ch_signal, msg_upgrade);
    if (0 > ret) {
        Log_e("Report version failed");
        h_ota->err = ret;
        ret = QCLOUD_ERR_FAILURE;
        goto do_exit;
    }

    IOT_OTA_ResetStatus(h_ota);

do_exit:
    if (NULL != msg_upgrade) {
        HAL_Free(msg_upgrade);
    }
    return ret;

#undef MSG_UPGPGRADE_LEN
}

/* Init OTA handle */
void *IOT_OTA_Init(const char *product_id, const char *device_name, void *ch_signal)
{
    POINTER_SANITY_CHECK(product_id, NULL);
    POINTER_SANITY_CHECK(device_name, NULL);
    POINTER_SANITY_CHECK(ch_signal, NULL);
    
    OTA_Struct_t *h_ota = NULL;

    if (NULL == (h_ota = HAL_Malloc(sizeof(OTA_Struct_t)))) {
        Log_e("allocate failed");
        return NULL;
    }
    memset(h_ota, 0, sizeof(OTA_Struct_t));
    h_ota->state = IOT_OTAS_UNINITED;

    h_ota->ch_signal = qcloud_osc_init(product_id, device_name, ch_signal, _ota_callback, h_ota);
    if (NULL == h_ota->ch_signal) {
        Log_e("initialize signal channel failed");
        goto do_exit;
    }

    h_ota->md5 = qcloud_otalib_md5_init();
    if (NULL == h_ota->md5) {
        Log_e("initialize md5 failed");
        goto do_exit;
    }

    h_ota->product_id = product_id;
    h_ota->device_name = device_name;
    h_ota->state = IOT_OTAS_INITED;
#ifdef OTA_MQTT_CHANNEL
    h_ota->current_signal_type = MQTT_CHANNEL;
#else
    h_ota->current_signal_type = COAP_CHANNEL;
#endif
    return h_ota;

do_exit:

    if (NULL != h_ota->ch_signal) {
        qcloud_osc_deinit(h_ota->ch_signal);
    }

    if (NULL != h_ota->md5) {
        qcloud_otalib_md5_deinit(h_ota->md5);
    }

    if (NULL != h_ota) {
        HAL_Free(h_ota);
    }

    return NULL;

#undef AOM_INFO_MSG_LEN
}


/* Destroy OTA handle and resource */
int IOT_OTA_Destroy(void *handle)
{
    OTA_Struct_t *h_ota = (OTA_Struct_t*) handle;

    if (NULL == h_ota) {
        Log_e("handle is NULL");
        return IOT_OTA_ERR_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return QCLOUD_ERR_FAILURE;
    }

    qcloud_osc_deinit(h_ota->ch_signal);
    qcloud_ofc_deinit(h_ota->ch_fetch);
    qcloud_otalib_md5_deinit(h_ota->md5);

    if (NULL != h_ota->purl) {
        HAL_Free(h_ota->purl);
    }

    if (NULL != h_ota->version) {
        HAL_Free(h_ota->version);
    }

    HAL_Free(h_ota);
    return QCLOUD_RET_SUCCESS;
}

/*support continuous transmission of breakpoints*/
int IOT_OTA_StartDownload(void *handle, uint32_t offset, uint32_t size)
{
	OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;
	int Ret;

	h_ota->size_fetched += offset;
	h_ota->ch_fetch = ofc_Init(h_ota->purl, offset, size);
    if (NULL == h_ota->ch_fetch) {
        Log_e("Initialize fetch module failed");
		return QCLOUD_ERR_FAILURE;
    }

	Ret = qcloud_ofc_connect(h_ota->ch_fetch);
    if (QCLOUD_RET_SUCCESS != Ret) {
        Log_e("Connect fetch module failed");
        h_ota->state = IOT_OTAS_DISCONNECTED;	
    }
		
	return Ret;
}

/*support continuous transmission of breakpoints*/
void IOT_OTA_UpdateClientMd5(void *handle, char * buff, uint32_t size)
{
	OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;

	qcloud_otalib_md5_update(h_ota->md5, buff, size);	
}

int IOT_OTA_ReportVersion(void *handle, const char *version)
{
#define MSG_INFORM_LEN  (128)

    int ret, len;
    char *msg_informed;
    OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;

    POINTER_SANITY_CHECK(handle, IOT_OTA_ERR_INVALID_PARAM);
    POINTER_SANITY_CHECK(version, IOT_OTA_ERR_INVALID_PARAM);

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        Log_e("version string is invalid: must be [1, 32] chars");
        h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
        return QCLOUD_ERR_FAILURE;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return QCLOUD_ERR_FAILURE;
    }

    IOT_OTA_ResetStatus(h_ota);

    if (NULL == (msg_informed = HAL_Malloc(MSG_INFORM_LEN))) {
        Log_e("allocate for msg_informed failed");
        h_ota->err = IOT_OTA_ERR_NOMEM;
        return QCLOUD_ERR_FAILURE;
    }

    ret = qcloud_otalib_gen_info_msg(msg_informed, MSG_INFORM_LEN, h_ota->id, version);
    if (ret != 0) {
        Log_e("generate inform message failed");
        h_ota->err = ret;
        ret = QCLOUD_ERR_FAILURE;
        goto do_exit;
    }

    ret = qcloud_osc_report_version(h_ota->ch_signal, msg_informed);
    if (0 > ret) {
        Log_e("Report version failed");
        h_ota->err = ret;
        ret = QCLOUD_ERR_FAILURE;
        goto do_exit;
    }

do_exit:
    if (NULL != msg_informed) {
        HAL_Free(msg_informed);
    }
    return ret;

#undef MSG_INFORM_LEN
}

int IOT_OTA_ReportUpgradeBegin(void *handle)
{
    OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;
    return IOT_OTA_ReportUpgradeResult(handle, h_ota->version, IOT_OTAR_UPGRADE_BEGIN);
}

int IOT_OTA_ReportUpgradeSuccess(void *handle, const char *version)
{
	OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;
	int ret;

	if(NULL == version){
		ret = IOT_OTA_ReportUpgradeResult(handle, h_ota->version, IOT_OTAR_UPGRADE_SUCCESS);
	}else{
		ret = IOT_OTA_ReportUpgradeResult(handle, version, IOT_OTAR_UPGRADE_SUCCESS);
	}
	
	return ret;
}


int IOT_OTA_ReportUpgradeFail(void *handle, const char *version)
{
	OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;
	int ret;

	if(NULL == version){
		ret = IOT_OTA_ReportUpgradeResult(handle, h_ota->version, IOT_OTAR_UPGRADE_FAIL);
	}else{
		ret = IOT_OTA_ReportUpgradeResult(handle, version, IOT_OTAR_UPGRADE_FAIL);
	}
	
	return ret;
}


/* check whether is downloading */
int IOT_OTA_IsFetching(void *handle)
{
    OTA_Struct_t *h_ota = (OTA_Struct_t *)handle;

    if (NULL == handle) {
        Log_e("handle is NULL");
        return 0;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHING == h_ota->state);
}


/* check whether fetch over */
int IOT_OTA_IsFetchFinish(void *handle)
{
    OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;

    if (NULL == handle) {
        Log_e("handle is NULL");
        return 0;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        Log_e("handle is uninitialized");
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHED == h_ota->state);
}


int IOT_OTA_FetchYield(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_ms)
{
    int ret;
    OTA_Struct_t *h_ota = (OTA_Struct_t *) handle;

    POINTER_SANITY_CHECK(handle, IOT_OTA_ERR_INVALID_PARAM);
    POINTER_SANITY_CHECK(buf, IOT_OTA_ERR_INVALID_PARAM);
    NUMBERIC_SANITY_CHECK(buf_len, IOT_OTA_ERR_INVALID_PARAM);

    if (IOT_OTAS_FETCHING != h_ota->state) {
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return IOT_OTA_ERR_INVALID_STATE;
    }

    ret = qcloud_ofc_fetch(h_ota->ch_fetch, buf, buf_len, timeout_ms);
    if (ret < 0) {
        h_ota->state = IOT_OTAS_FETCHED;
        h_ota->err = IOT_OTA_ERR_FETCH_FAILED;

        if (ret == IOT_OTA_ERR_FETCH_AUTH_FAIL) { // OTA auth failed
            IOT_OTA_ReportUpgradeResult(h_ota, h_ota->version, IOT_OTAR_AUTH_FAIL);
            h_ota->err = ret;
        } else if (ret == IOT_OTA_ERR_FETCH_NOT_EXIST) { // fetch not existed
            IOT_OTA_ReportUpgradeResult(h_ota, h_ota->version, IOT_OTAR_FILE_NOT_EXIST);
            h_ota->err = ret;
        } else if (ret == IOT_OTA_ERR_FETCH_TIMEOUT) { // fetch timeout
            IOT_OTA_ReportUpgradeResult(h_ota, h_ota->version, IOT_OTAR_DOWNLOAD_TIMEOUT);
            h_ota->err = ret;
        } 
            
        return ret;
    } else if (0 == h_ota->size_fetched) {
        /* force report status in the first */
        IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_PERCENTAGE_MIN, IOT_OTAR_DOWNLOAD_BEGIN);

        InitTimer(&h_ota->report_timer);
        countdown(&h_ota->report_timer, 1);
    }

    h_ota->size_last_fetched = ret;
    h_ota->size_fetched += ret;

    /* report percent every second. */
    uint32_t percent = (h_ota->size_fetched * 100) / h_ota->size_file;
    if (percent == 100) {
        IOT_OTA_ReportProgress(h_ota, percent, IOT_OTAR_DOWNLOADING);
    } else if (h_ota->size_last_fetched > 0 && expired(&h_ota->report_timer)) {
        IOT_OTA_ReportProgress(h_ota, percent, IOT_OTAR_DOWNLOADING);
        countdown(&h_ota->report_timer, 1);
    }

    if (h_ota->size_fetched >= h_ota->size_file) {
        h_ota->state = IOT_OTAS_FETCHED;
    }

    qcloud_otalib_md5_update(h_ota->md5, buf, ret);

    return ret;
}


int IOT_OTA_Ioctl(void *handle, IOT_OTA_CmdType type, void *buf, size_t buf_len)
{
    OTA_Struct_t * h_ota = (OTA_Struct_t *) handle;

    POINTER_SANITY_CHECK(handle, IOT_OTA_ERR_INVALID_PARAM);
    POINTER_SANITY_CHECK(buf, IOT_OTA_ERR_INVALID_PARAM);
    NUMBERIC_SANITY_CHECK(buf_len, IOT_OTA_ERR_INVALID_PARAM);

    if (h_ota->state < IOT_OTAS_FETCHING) {
        h_ota->err = IOT_OTA_ERR_INVALID_STATE;
        return IOT_OTA_ERR_INVALID_STATE;
    }

    switch (type) {
        case IOT_OTAG_FETCHED_SIZE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                Log_e("Invalid parameter");
                h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
                return QCLOUD_ERR_FAILURE;
            } else {
                *((uint32_t *)buf) = h_ota->size_fetched;
                return 0;
            }

        case IOT_OTAG_FILE_SIZE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                Log_e("Invalid parameter");
                h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
                return QCLOUD_ERR_FAILURE;
            } else {
                *((uint32_t *)buf) = h_ota->size_file;
                return 0;
            }

        case IOT_OTAG_VERSION:
            strncpy(buf, h_ota->version, buf_len);
            ((char *)buf)[buf_len - 1] = '\0';
            break;

        case IOT_OTAG_MD5SUM:
            strncpy(buf, h_ota->md5sum, buf_len);
            ((char *)buf)[buf_len - 1] = '\0';
            break;

        case IOT_OTAG_CHECK_FIRMWARE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                Log_e("Invalid parameter");
                h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
                return QCLOUD_ERR_FAILURE;
            } else if (h_ota->state != IOT_OTAS_FETCHED) {
                h_ota->err = IOT_OTA_ERR_INVALID_STATE;
                Log_e("Firmware can be checked in IOT_OTAS_FETCHED state only");
                return QCLOUD_ERR_FAILURE;
            } else {
                char md5_str[33];
                qcloud_otalib_md5_finalize(h_ota->md5, md5_str);
                Log_d("origin=%s, now=%s", h_ota->md5sum, md5_str);
                if (0 == strcmp(h_ota->md5sum, md5_str)) {
                    *((uint32_t *)buf) = 1;
                } else {
                    *((uint32_t *)buf) = 0;
                    // report MD5 inconsistent
                    IOT_OTA_ReportUpgradeResult(h_ota, h_ota->version, IOT_OTAR_MD5_NOT_MATCH);
                }
                return 0;
            }

        default:
            Log_e("invalid cmd type");
            h_ota->err = IOT_OTA_ERR_INVALID_PARAM;
            return QCLOUD_ERR_FAILURE;
    }

    return 0;
}


/* Get last error code */
int IOT_OTA_GetLastError(void *handle)
{
    OTA_Struct_t * h_ota = (OTA_Struct_t *) handle;

    if (NULL == handle) {
        Log_e("handle is NULL");
        return  IOT_OTA_ERR_INVALID_PARAM;
    }

    return h_ota->err;
}

#ifdef __cplusplus
}
#endif
