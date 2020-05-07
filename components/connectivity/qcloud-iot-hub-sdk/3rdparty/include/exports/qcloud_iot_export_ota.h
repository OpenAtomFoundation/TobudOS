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

#ifndef QCLOUD_IOT_EXPORT_OTA_H_
#define QCLOUD_IOT_EXPORT_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_import.h"

typedef enum {

    IOT_OTA_ERR_FAIL            = -1,
    IOT_OTA_ERR_INVALID_PARAM   = -2,
    IOT_OTA_ERR_INVALID_STATE   = -3,
    IOT_OTA_ERR_STR_TOO_LONG    = -4,
    IOT_OTA_ERR_FETCH_FAILED    = -5,
    IOT_OTA_ERR_FETCH_NOT_EXIST = -6,
    IOT_OTA_ERR_FETCH_AUTH_FAIL = -7,
    IOT_OTA_ERR_FETCH_TIMEOUT   = -8,
    IOT_OTA_ERR_NOMEM           = -9,
    IOT_OTA_ERR_OSC_FAILED      = -10,
    IOT_OTA_ERR_REPORT_VERSION  = -11,
    IOT_OTA_ERR_NONE            = 0

} IOT_OTA_Error_Code;

/* type of OTA state */
typedef enum {
    IOT_OTAS_UNINITED = 0, /* un-inited */
    IOT_OTAS_INITED,       /* inited */
    IOT_OTAS_FETCHING,     /* fetching */
    IOT_OTAS_FETCHED,      /* fetched */
    IOT_OTAS_DISCONNECTED  /* disconnected */
} IOT_OTA_State_Code;

/* type of OTA progress */
typedef enum {

    /* firmware upgrade failed */
    IOT_OTAP_BURN_FAILED = -4,

    /* firmware checksum failed */
    IOT_OTAP_CHECK_FALIED = -3,

    /* firmware download failed */
    IOT_OTAP_FETCH_FAILED = -2,

    /* init failed */
    IOT_OTAP_GENERAL_FAILED = -1,

    /* [0, 100], progress percentage */

    /* minimal of progress percentage */
    IOT_OTAP_FETCH_PERCENTAGE_MIN = 0,

    /* maximal of progress percentage */
    IOT_OTAP_FETCH_PERCENTAGE_MAX = 100

} IOT_OTA_Progress_Code;

typedef enum {

    IOT_OTAG_FETCHED_SIZE,  /* Size of firmware fetched */
    IOT_OTAG_FILE_SIZE,     /* Total size of firmware */
    IOT_OTAG_MD5SUM,        /* firmware md5 checksum (string) */
    IOT_OTAG_VERSION,       /* firmware version (string) */
    IOT_OTAG_CHECK_FIRMWARE /* check firmware */

} IOT_OTA_CmdType;

typedef enum {

    IOT_OTAR_DOWNLOAD_TIMEOUT = -1,
    IOT_OTAR_FILE_NOT_EXIST   = -2,
    IOT_OTAR_AUTH_FAIL        = -3,
    IOT_OTAR_MD5_NOT_MATCH    = -4,
    IOT_OTAR_UPGRADE_FAIL     = -5,
    IOT_OTAR_NONE             = 0,
    IOT_OTAR_DOWNLOAD_BEGIN   = 1,
    IOT_OTAR_DOWNLOADING      = 2,
    IOT_OTAR_UPGRADE_BEGIN    = 3,
    IOT_OTAR_UPGRADE_SUCCESS  = 4,

} IOT_OTAReportType;

/**
 * @brief Init OTA module and resources
 *        MQTT/COAP Client should be constructed beforehand
 *
 * @param product_id:   product Id
 * @param device_name:  device name
 * @param ch_signal:    channel: MQTT or COAP
 *
 * @return a valid OTA module handle when success, or NULL otherwise
 */
void *IOT_OTA_Init(const char *product_id, const char *device_name, void *ch_signal);

/**
 * @brief Destroy OTA module and resources
 *
 * @param handle: OTA module handle
 *
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_OTA_Destroy(void *handle);

/**
 * @brief Setup HTTP connection and prepare OTA download
 *
 * @param handle: OTA module handle
 * @param offset: offset of firmware downloaded
 * @param size: size of firmware
 *
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_OTA_StartDownload(void *handle, uint32_t offset, uint32_t size);

/**
 * @brief Update MD5 of local firmware
 *
 * @param handle: OTA module handle
 * @param buff: buffer to firmware
 * @param size: size of buffer
 *
 */
void IOT_OTA_UpdateClientMd5(void *handle, char *buff, uint32_t size);

int IOT_OTA_ResetClientMD5(void *handle);

/**
 * @brief Report local firmware version to server
 *        NOTE: do this report before real download
 *
 * @param handle: OTA module handle
 * @param version:  local firmware version string
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_OTA_ReportVersion(void *handle, const char *version);

/**
 * @brief Report upgrade begin to server
 *
 * @param handle: OTA module handle
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_OTA_ReportUpgradeBegin(void *handle);

/**
 * @brief Report upgrade success to server
 *
 * @param handle: OTA module handle
 * @param version:  version string of firmware to upgrade
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_OTA_ReportUpgradeSuccess(void *handle, const char *version);

/**
 * @brief Report upgrade fail to server
 *
 * @param handle: OTA module handle
 * @param version:  version string of firmware to upgrade
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_OTA_ReportUpgradeFail(void *handle, const char *version);

/**
 * @brief Check if firmware is fetching/downloading
 *
 * @param handle: OTA module handle
 *
 * @retval 1 : Yes.
 * @retval 0 : No.
 */
int IOT_OTA_IsFetching(void *handle);

/**
 * @brief Check if firmware fetching/downloading is finished
 *
 * @param handle: OTA module handle
 *
 * @retval 1 : Yes.
 * @retval 0 : No.
 */
int IOT_OTA_IsFetchFinish(void *handle);

/**
 * @brief Download firmware from HTTP server and save to buffer
 *
 * @param handle:       OTA module handle
 * @param buf:          buffer to store firmware
 * @param buf_len:      length of buffer
 * @param timeout_s:    timeout value in second
 *
 * @retval      < 0 : error code
 * @retval        0 : no data is downloaded in this period and timeout happen
 * @retval (0, len] : size of the downloaded data
 */
int IOT_OTA_FetchYield(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s);

/**
 * @brief Get OTA info (version, file_size, MD5, download state) from OTA module
 *
 * @param handle:   OTA module handle
 * @param type:     type of info to get, refer to IOT_OTA_CmdType
 * @param buf:      buffer for the data
 * @param buf_len:  length of buffer
 * @return
      NOTE:
      1) if type==IOT_OTAG_FETCHED_SIZE, 'buf' = uint32_t pointer, 'buf_len' = 4
      2) if type==IOT_OTAG_FILE_SIZE, 'buf' = uint32_t pointer, 'buf_len' = 4
      3) if type==IOT_OTAG_MD5SUM, 'buf' = char array buffer, 'buf_len = 33
      4) if type==IOT_OTAG_VERSION, 'buf'= char array buffer, 'buf_len = OTA_VERSION_LEN_MAX
      5) if type==IOT_OTAG_CHECK_FIRMWARE, 'buf' = uint32_t pointer, 'buf_len' = 4
 *
 * @retval   0 : success
 * @retval < 0 : error code for failure
 */
int IOT_OTA_Ioctl(void *handle, IOT_OTA_CmdType type, void *buf, size_t buf_len);

/**
 * @brief Get error code of last operation
 *
 * @param handle: OTA module handle
 *
 * @return error code
 */
int IOT_OTA_GetLastError(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_OTA_H_ */
