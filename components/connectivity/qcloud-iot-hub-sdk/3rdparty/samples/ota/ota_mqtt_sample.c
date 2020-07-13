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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lite-utils.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#define FW_RUNNING_VERSION "1.0.0"
#define KEY_VER            "version"
#define KEY_SIZE           "downloaded_size"

#define FW_VERSION_MAX_LEN    32
#define FW_FILE_PATH_MAX_LEN  128
#define OTA_BUF_LEN           5000
#define FW_INFO_FILE_DATA_LEN 128

typedef struct OTAContextData {
    void *ota_handle;
    void *mqtt_client;
    char  fw_file_path[FW_FILE_PATH_MAX_LEN];
    char  fw_info_file_path[FW_FILE_PATH_MAX_LEN];

    // remote_version means version for the FW in the cloud and to be downloaded
    char     remote_version[FW_VERSION_MAX_LEN];
    uint32_t fw_file_size;

    // for resuming download
    /* local_version means downloading but not running */
    char local_version[FW_VERSION_MAX_LEN];
    int  downloaded_size;

    // to make sure report is acked
    bool report_pub_ack;
    int  report_packet_id;

} OTAContextData;

static void _event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
    uintptr_t       packet_id = (uintptr_t)msg->msg;
    OTAContextData *ota_ctx   = (OTAContextData *)handle_context;

    switch (msg->event_type) {
        case MQTT_EVENT_UNDEF:
            Log_i("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            Log_i("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            Log_i("MQTT reconnect.");
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            if (ota_ctx->report_packet_id == packet_id)
                ota_ctx->report_pub_ack = true;
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;
        default:
            Log_i("Should NOT arrive here.");
            break;
    }
}

static int _setup_connect_init_params(MQTTInitParams *initParams, void *ota_ctx, DeviceInfo *device_info)
{
    initParams->product_id  = device_info->product_id;
    initParams->device_name = device_info->device_name;

#ifdef AUTH_MODE_CERT
    char  certs_dir[16] = "certs";
    char  current_path[128];
    char *cwd = getcwd(current_path, sizeof(current_path));

    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }

#ifdef WIN32
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#else
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#endif

#else
    initParams->device_secret = device_info->device_secret;
#endif

    initParams->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

    initParams->auto_connect_enable  = 1;
    initParams->event_handle.h_fp    = _event_handler;
    initParams->event_handle.context = ota_ctx;

    return QCLOUD_RET_SUCCESS;
}

static void _wait_for_pub_ack(OTAContextData *ota_ctx, int packet_id)
{
    int wait_cnt              = 10;
    ota_ctx->report_pub_ack   = false;
    ota_ctx->report_packet_id = packet_id;

    while (!ota_ctx->report_pub_ack) {
        HAL_SleepMs(500);
        IOT_MQTT_Yield(ota_ctx->mqtt_client, 500);
        if (wait_cnt-- == 0) {
            Log_e("wait report pub ack timeout!");
            break;
        }
    }
    ota_ctx->report_pub_ack = false;
    return;
}

/**********************************************************************************
 * OTA file operations START
 * these are platform-dependant functions
 * POSIX FILE is used in this sample code
 **********************************************************************************/
// calculate left MD5 for resuming download from break point
static int _cal_exist_fw_md5(OTAContextData *ota_ctx)
{
    char   buff[OTA_BUF_LEN];
    size_t rlen, total_read = 0;
    int    ret = QCLOUD_RET_SUCCESS;

    ret = IOT_OTA_ResetClientMD5(ota_ctx->ota_handle);
    if (ret) {
        Log_e("reset MD5 failed: %d", ret);
        return QCLOUD_ERR_FAILURE;
    }

    FILE *fp = fopen(ota_ctx->fw_file_path, "ab+");
    if (NULL == fp) {
        Log_e("open file %s failed", ota_ctx->fw_file_path);
        return QCLOUD_ERR_FAILURE;
    }

    // rewind(fp);
    size_t size = ota_ctx->downloaded_size;

    while ((size > 0) && (!feof(fp))) {
        rlen = (size > OTA_BUF_LEN) ? OTA_BUF_LEN : size;
        if (rlen != fread(buff, 1, rlen, fp)) {
            Log_e("read data len not expected");
            ret = QCLOUD_ERR_FAILURE;
            break;
        }
        IOT_OTA_UpdateClientMd5(ota_ctx->ota_handle, buff, rlen);
        size -= rlen;
        total_read += rlen;
    }

    fclose(fp);
    Log_d("total read: %d", total_read);
    return ret;
}

/* update local firmware info for resuming download from break point */
static int _update_local_fw_info(OTAContextData *ota_ctx)
{
    FILE *fp;
    int   wlen;
    int   ret = QCLOUD_RET_SUCCESS;
    char  data_buf[FW_INFO_FILE_DATA_LEN];

    memset(data_buf, 0, sizeof(data_buf));
    HAL_Snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%s\", \"%s\":%d}", KEY_VER, ota_ctx->remote_version, KEY_SIZE,
                 ota_ctx->downloaded_size);

    fp = fopen(ota_ctx->fw_info_file_path, "w");
    if (NULL == fp) {
        Log_e("open file %s failed", ota_ctx->fw_info_file_path);
        ret = QCLOUD_ERR_FAILURE;
        goto exit;
    }

    wlen = fwrite(data_buf, 1, strlen(data_buf), fp);
    if (wlen != strlen(data_buf)) {
        Log_e("save version to file err");
        ret = QCLOUD_ERR_FAILURE;
    }

exit:

    if (NULL != fp) {
        fclose(fp);
    }

    return ret;
}

static int _get_local_fw_info(char *file_name, char *local_version)
{
    int  len;
    int  rlen;
    char json_doc[FW_INFO_FILE_DATA_LEN] = {0};

    FILE *fp = fopen(file_name, "r");
    if (NULL == fp) {
        Log_e("open file %s failed", file_name);
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    if (len > FW_INFO_FILE_DATA_LEN) {
        Log_e("%s is too big, pls check", file_name);
        fclose(fp);
        return 0;
    }

    rewind(fp);
    rlen = fread(json_doc, 1, len, fp);
    if (len != rlen) {
        Log_e("read data len (%d) less than needed (%d), %s", rlen, len, json_doc);
        fclose(fp);
        return 0;
    }

    char *version = LITE_json_value_of(KEY_VER, json_doc);
    char *size    = LITE_json_value_of(KEY_SIZE, json_doc);

    if ((NULL == version) || (NULL == size)) {
        if (version)
            HAL_Free(version);
        if (size)
            HAL_Free(size);
        fclose(fp);
        return 0;
    }

    int local_size = atoi(size);
    HAL_Free(size);

    if (local_size <= 0) {
        Log_w("local info offset invalid: %d", local_size);
        HAL_Free(version);
        local_size = 0;
    }

    strncpy(local_version, version, FW_VERSION_MAX_LEN);
    HAL_Free(version);
    fclose(fp);
    return local_size;
}

/* get local firmware offset for resuming download from break point */
static int _update_fw_downloaded_size(OTAContextData *ota_ctx)
{
    int local_size = _get_local_fw_info(ota_ctx->fw_info_file_path, ota_ctx->local_version);
    if (local_size == 0) {
        ota_ctx->downloaded_size = 0;
        return 0;
    }

    if ((0 != strcmp(ota_ctx->local_version, ota_ctx->remote_version)) ||
        (ota_ctx->downloaded_size > ota_ctx->fw_file_size)) {
        ota_ctx->downloaded_size = 0;
        return 0;
    }

    ota_ctx->downloaded_size = local_size;
    Log_i("calc MD5 for resuming download from offset: %d", ota_ctx->downloaded_size);
    int ret = _cal_exist_fw_md5(ota_ctx);
    if (ret) {
        Log_e("regen OTA MD5 error: %d", ret);
        remove(ota_ctx->fw_info_file_path);
        ota_ctx->downloaded_size = 0;
        return 0;
    }
    Log_d("local MD5 update done!");
    return local_size;
}

static int _delete_fw_info_file(char *file_name)
{
    return remove(file_name);
}

static int _save_fw_data_to_file(char *file_name, uint32_t offset, char *buf, int len)
{
    FILE *fp;
    if (offset > 0) {
        if (NULL == (fp = fopen(file_name, "ab+"))) {
            Log_e("open file failed");
            return QCLOUD_ERR_FAILURE;
        }
    } else {
        if (NULL == (fp = fopen(file_name, "wb+"))) {
            Log_e("open file failed");
            return QCLOUD_ERR_FAILURE;
        }
    }

    fseek(fp, offset, SEEK_SET);

    if (1 != fwrite(buf, len, 1, fp)) {
        Log_e("write data to file failed");
        fclose(fp);
        return QCLOUD_ERR_FAILURE;
    }
    fflush(fp);
    fclose(fp);

    return 0;
}

static char *_get_local_fw_running_version()
{
    // asuming the version is inside the code and binary
    // you can also get from a meta file
    Log_i("FW running version: %s", FW_RUNNING_VERSION);
    return FW_RUNNING_VERSION;
}
/**********************************************************************************
 * OTA file operations END
 **********************************************************************************/

// main OTA cycle
bool process_ota(OTAContextData *ota_ctx)
{
    bool  download_finished     = false;
    bool  upgrade_fetch_success = true;
    char  buf_ota[OTA_BUF_LEN];
    int   rc;
    void *h_ota = ota_ctx->ota_handle;

    /* Must report version first */
    if (0 > IOT_OTA_ReportVersion(h_ota, _get_local_fw_running_version())) {
        Log_e("report OTA version failed");
        return false;
    }

    do {
        IOT_MQTT_Yield(ota_ctx->mqtt_client, 200);

        Log_i("wait for ota upgrade command...");

        // recv the upgrade cmd
        if (IOT_OTA_IsFetching(h_ota)) {
            IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &ota_ctx->fw_file_size, 4);
            IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, ota_ctx->remote_version, FW_VERSION_MAX_LEN);

            DeviceInfo *device_info = IOT_MQTT_GetDeviceInfo(ota_ctx->mqtt_client);
            HAL_Snprintf(ota_ctx->fw_file_path, FW_FILE_PATH_MAX_LEN, "./FW_%s_%s.bin", device_info->client_id,
                         ota_ctx->remote_version);
            HAL_Snprintf(ota_ctx->fw_info_file_path, FW_FILE_PATH_MAX_LEN, "./FW_%s.json", device_info->client_id);

            /* check if pre-downloading finished or not */
            /* if local FW downloaded size (ota_ctx->downloaded_size) is not zero, it will do resuming download */
            _update_fw_downloaded_size(ota_ctx);

            /*set offset and start http connect*/
            rc = IOT_OTA_StartDownload(h_ota, ota_ctx->downloaded_size, ota_ctx->fw_file_size);
            if (QCLOUD_RET_SUCCESS != rc) {
                Log_e("OTA download start err,rc:%d", rc);
                upgrade_fetch_success = false;
                break;
            }

            // download and save the fw
            do {
                int len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
                if (len > 0) {
                    rc = _save_fw_data_to_file(ota_ctx->fw_file_path, ota_ctx->downloaded_size, buf_ota, len);
                    if (rc) {
                        Log_e("write data to file failed");
                        upgrade_fetch_success = false;
                        break;
                    }
                } else if (len < 0) {
                    Log_e("download fail rc=%d", len);
                    upgrade_fetch_success = false;
                    break;
                }

                /* get OTA information and update local info */
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &ota_ctx->downloaded_size, 4);
                rc = _update_local_fw_info(ota_ctx);
                if (QCLOUD_RET_SUCCESS != rc) {
                    Log_e("update local fw info err,rc:%d", rc);
                }

                // quit ota process as something wrong with mqtt
                rc = IOT_MQTT_Yield(ota_ctx->mqtt_client, 100);
                if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
                    Log_e("MQTT error: %d", rc);
                    return false;
                }

            } while (!IOT_OTA_IsFetchFinish(h_ota));

            /* Must check MD5 match or not */
            if (upgrade_fetch_success) {
                // download is finished, delete the fw info file
                _delete_fw_info_file(ota_ctx->fw_info_file_path);

                uint32_t firmware_valid;
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
                if (0 == firmware_valid) {
                    Log_e("The firmware is invalid");
                    upgrade_fetch_success = false;
                } else {
                    Log_i("The firmware is valid");
                    upgrade_fetch_success = true;
                }
            }

            download_finished = true;
        }

        if (!download_finished)
            HAL_SleepMs(1000);

    } while (!download_finished);

    // do some post-download stuff for your need

    // report result
    int packet_id;
    if (upgrade_fetch_success)
        packet_id = IOT_OTA_ReportUpgradeSuccess(h_ota, NULL);
    else
        packet_id = IOT_OTA_ReportUpgradeFail(h_ota, NULL);
    _wait_for_pub_ack(ota_ctx, packet_id);

    return upgrade_fetch_success;
}

int main(int argc, char **argv)
{
    int             rc;
    OTAContextData *ota_ctx     = NULL;
    void *          mqtt_client = NULL;
    void *          h_ota       = NULL;

    IOT_Log_Set_Level(eLOG_DEBUG);
    ota_ctx = (OTAContextData *)HAL_Malloc(sizeof(OTAContextData));
    if (ota_ctx == NULL) {
        Log_e("malloc failed");
        goto exit;
    }
    memset(ota_ctx, 0, sizeof(OTAContextData));

    DeviceInfo device_info = {0};
    rc                     = HAL_GetDevInfo((void *)&device_info);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("get device info failed: %d", rc);
        goto exit;
    }

    // setup MQTT init params
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc                         = _setup_connect_init_params(&init_params, ota_ctx, &device_info);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params err,rc=%d", rc);
        return rc;
    }

    // create MQTT mqtt_client and connect to server
    mqtt_client = IOT_MQTT_Construct(&init_params);
    if (mqtt_client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    // init OTA handle
    h_ota = IOT_OTA_Init(device_info.product_id, device_info.device_name, mqtt_client);
    if (NULL == h_ota) {
        Log_e("initialize OTA failed");
        goto exit;
    }

    ota_ctx->ota_handle  = h_ota;
    ota_ctx->mqtt_client = mqtt_client;

    bool ota_success;
    do {
        // mqtt should be ready first
        rc = IOT_MQTT_Yield(mqtt_client, 500);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }

        // OTA process
        ota_success = process_ota(ota_ctx);
        if (!ota_success) {
            Log_e("OTA failed! Do it again");
            HAL_SleepMs(2000);
        }
    } while (!ota_success);

exit:

    if (NULL != ota_ctx)
        HAL_Free(ota_ctx);

    if (NULL != h_ota)
        IOT_OTA_Destroy(h_ota);

    IOT_MQTT_Destroy(&mqtt_client);

    return 0;
}
