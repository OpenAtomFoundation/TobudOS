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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"

#include "tos_kv.h"
#include "ota_download.h"

#ifdef AUTH_MODE_CERT
static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif

static DeviceInfo sg_devInfo;

static int sg_pub_ack = K_FALSE;
static int sg_packet_id = 0;

static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;

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
            if (sg_packet_id == packet_id)
                sg_pub_ack = K_TRUE;
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

/* demo of firmware info management in device side for resuming download from break point */
#define KEY_CURR_VERSION                "c_vs"
#define KEY_MD5                         "md5"
#define KEY_DOWNLOADED_SIZE            "dl_sz"
#define KEY_PREV_VERSION                "r_vs"

static void local_firmware_info_get(char **local_version, char **local_md5, char **local_fw_size)
{
    char *prev_version, *json_doc = NULL;

#define LOCAL_FW_INFO_MAX       128
    size_t info_len;
    static char local_fw_info[LOCAL_FW_INFO_MAX];

    if (tos_kv_get("local_fw", local_fw_info, sizeof(local_fw_info), &info_len) != KV_ERR_NONE) {
        *local_version  = NULL;
        *local_md5      = NULL;
        *local_fw_size  = NULL;
        return;
    }

    json_doc = local_fw_info;

    *local_version = LITE_json_value_of(KEY_CURR_VERSION, json_doc);
    *local_md5 = LITE_json_value_of(KEY_MD5, json_doc);
    *local_fw_size = LITE_json_value_of(KEY_DOWNLOADED_SIZE, json_doc);
    prev_version = LITE_json_value_of(KEY_PREV_VERSION, json_doc);
}

/* update local firmware info for resuming download from break point */
static int local_firmware_info_update(const char *curr_version, const char *prev_version, const char *md5, uint32_t downloaded_size)
{
#define JSON_DOC_LEN        256
    static char json_doc[JSON_DOC_LEN];

    memset(json_doc, 0, JSON_DOC_LEN);
    HAL_Snprintf(json_doc, JSON_DOC_LEN, "{\"%s\":\"%s\", \"%s\":\"%s\",\"%s\":%d,\"%s\":\"%s\"}", \
                 KEY_CURR_VERSION, curr_version,
                 KEY_MD5, md5,
                 KEY_DOWNLOADED_SIZE, downloaded_size, \
                 KEY_PREV_VERSION, prev_version);

    if (tos_kv_set("local_fw", json_doc, strlen(json_doc)) != KV_ERR_NONE) {
        Log_e("save version failed\n");
        return -1;
    }

    return 0;
}

/* get remote firmware information */
static int remote_firmware_info_get(void *ota_client, char remote_version[], char remote_md5[], uint32_t *remote_fw_size)
{
    if (IOT_OTA_Ioctl(ota_client, IOT_OTAG_VERSION, remote_version, 128) < 0) {
        return -1;
    }
    if (IOT_OTA_Ioctl(ota_client, IOT_OTAG_MD5SUM, remote_md5, 33) < 0) {
        return -1;
    }
    if (IOT_OTA_Ioctl(ota_client, IOT_OTAG_FILE_SIZE, remote_fw_size, sizeof(uint32_t)) < 0) {
        return -1;
    }

    Log_d("remote firmware: fw_size: %d  version: %s  md5: %s\n", *remote_fw_size, remote_version, remote_md5);

    return 0;
}

/* calculate left MD5 for resuming download from break point */
static int downloaded_firmware_md5(void *ota_client, size_t size)
{
#define BUF_SIZE        128
    static char buf[BUF_SIZE];
    size_t remain_len, read_len;

    uint32_t ota_flash_addr = ota_partition_start(OTA_PARTITION_OTA);

    remain_len = size;

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
    while (remain_len > 0) {
        read_len = MIN(sizeof(buf), remain_len);
        if (ota_flash_read(ota_flash_addr, buf, read_len) < 0) {
            return -1;
        }
        IOT_OTA_UpdateClientMd5(ota_client, buf, read_len);
        remain_len      -= read_len;
        ota_flash_addr  += read_len;
    }

    return 0;
}

static int connect_init_params_setup(MQTTInitParams* initParams)
{
    int ret;

    ret = HAL_GetDevInfo((void *)&sg_devInfo);
    if (QCLOUD_RET_SUCCESS != ret) {
        return ret;
    }

    initParams->device_name = sg_devInfo.device_name;
    initParams->product_id = sg_devInfo.product_id;

#ifdef AUTH_MODE_CERT
    char certs_dir[PATH_MAX + 1] = "certs";
    char current_path[PATH_MAX + 1];
    char *cwd = getcwd(current_path, sizeof(current_path));
    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }
    sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
    sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);

    initParams->cert_file = sg_cert_file;
    initParams->key_file = sg_key_file;
#else
    initParams->device_secret = sg_devInfo.device_secret;
#endif


    initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
    initParams->auto_connect_enable = 1;
    initParams->event_handle.h_fp = event_handler;

    return QCLOUD_RET_SUCCESS;
}

int ota_init(void **mqtt_client, void **ota_client)
{
    int rc = 0;
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;

    rc = connect_init_params_setup(&init_params);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params err,rc=%d", rc);
        return -1;
    }

    void *the_mqtt_client = IOT_MQTT_Construct(&init_params);
    if (!the_mqtt_client) {
        Log_e("Cloud Device Construct Failed");
        return -1;
    }

    Log_i("Cloud Device Construct Success");

    void *the_ota_client = IOT_OTA_Init(sg_devInfo.product_id, sg_devInfo.device_name, the_mqtt_client);
    if (!the_ota_client) {
        Log_e("initialize OTA failed");
        return -1;
    }

    *mqtt_client    = the_mqtt_client;
    *ota_client     = the_ota_client;

    return 0;
}

int remote_firmware_download(void *ota_client, void *mqtt_client, char *running_version, uint32_t offset, uint32_t fw_size, uint32_t *downloaded_size)
{
    int rc;
    uint32_t is_firmware_valid;
    uint32_t received_len;
    static char remote_version[128], remote_md5[33];

#define OTA_DOWNLOAD_BUF_LEN        128
    static char ota_download_buf[OTA_DOWNLOAD_BUF_LEN];

    uint32_t flash_addr = ota_partition_start(OTA_PARTITION_OTA);

    ////////////////////////////////////////////////
    /************* some sanity check  *************/

    /* is the file too large? */
    if (fw_size > ota_partition_size(OTA_PARTITION_OTA)) {
        Log_e("OTA partition too small, fw_size: %d, ota size: %d", fw_size, ota_partition_size(OTA_PARTITION_OTA));
        return -1;
    }

    /* we need the buf_len to be flash write-align aligned */
    if (!ota_flash_size_is_aligned(sizeof(ota_download_buf))) {
        return -1;
    }

    /* a total new firmware download, not start from a break point
       we need to erase the flash to make flash ready
     */
    if (offset == 0 &&
        ota_flash_erase_blocks(flash_addr, fw_size) < 0) {
        Log_e("flash erase failed\n");
        return -1;
    }

    ////////////////////////////////////////////////

    /* start http connect */
    rc = IOT_OTA_StartDownload(ota_client, offset, fw_size);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("OTA download start err, rc: %d", rc);
        return -1;
    }

    if (offset > 0) {
        Log_d("Get offset: %x", offset);
        if (downloaded_firmware_md5(ota_client, offset) < 0) {
            Log_e("cal download firmware md5 failed");
            return -1;
        }
    }

    do {
        received_len = IOT_OTA_FetchYield(ota_client, ota_download_buf, OTA_DOWNLOAD_BUF_LEN, 1);
        if (received_len <= 0) {
            Log_e("download fail rc = %d", received_len);
            return -1;
        }

        if (received_len < OTA_DOWNLOAD_BUF_LEN &&
            !ota_flash_size_is_aligned(received_len)) {
            Log_e("size not aligned %d", received_len);
            return -1;
        }

        if (ota_flash_write(flash_addr, ota_download_buf, ota_flash_write_size_aligned_get(received_len)) < 0) {
            return -1;
        }

        /* get OTA information and update local info */
        IOT_OTA_Ioctl(ota_client, IOT_OTAG_FETCHED_SIZE, downloaded_size, sizeof(uint32_t));
#if 0
        IOT_OTA_Ioctl(ota_client, IOT_OTAG_FILE_SIZE, &size_file, 4);
#endif
        IOT_OTA_Ioctl(ota_client, IOT_OTAG_MD5SUM, remote_md5, 33);
        IOT_OTA_Ioctl(ota_client, IOT_OTAG_VERSION, remote_version, 128);

        rc = local_firmware_info_update(remote_version, running_version, remote_md5, *downloaded_size);
        if (rc != QCLOUD_RET_SUCCESS) {
            Log_e("update local fw info err, rc: %d", rc);
            return -1;
        }

        IOT_MQTT_Yield(mqtt_client, 100);
    } while (!IOT_OTA_IsFetchFinish(ota_client));

    /* must check MD5 match or not */
    IOT_OTA_Ioctl(ota_client, IOT_OTAG_CHECK_FIRMWARE, &is_firmware_valid, sizeof(uint32_t));
    if (is_firmware_valid == 0) {
        Log_e("The firmware is invalid");
        rc = local_firmware_info_update(NULL, running_version, NULL, 0);
        if (rc != QCLOUD_RET_SUCCESS) {
            Log_e("update local fw info err, rc:%d", rc);
        }

        return -1;
    }

    return 0;
}

int ota_download_through_explorer(void)
{
    int rc;
    void *mqtt_client, *ota_client;

    uint32_t downloadeded_size;

    uint32_t offset;

    uint32_t local_fw_size;
    char *local_version = NULL, *local_md5 = NULL, *local_fw_size_str = NULL;

    uint32_t remote_fw_size;
    static char remote_version[128], remote_md5[33];

    ota_img_vs_t curr_version;
    char running_version[10] = { 0 };

    static char version[128], md5sum[33];

    IOT_Log_Set_Level(eLOG_DEBUG);

    if (ota_init(&mqtt_client, &ota_client) < 0) {
        Log_e("ota init Failed");
        return QCLOUD_ERR_FAILURE;
    }

    /* 1. make sure relative topic subscribe successfully */
    IOT_MQTT_Yield(mqtt_client, 1000);

    /* 2. get local firmware information */
    local_firmware_info_get(&local_version, &local_md5, &local_fw_size_str);

    if (local_version && local_md5 && local_fw_size_str) {
        Log_d("local_version: %s local_md5: %s, local_size: %s", local_version, local_md5, local_fw_size_str);
    }
    local_fw_size = local_fw_size_str ? atoi(local_fw_size_str) : 0;

    /* 3. get current running firmware version */
    curr_version = ota_info_curr_version();
    snprintf(running_version, sizeof(running_version), "%d.%d", curr_version.major, curr_version.minor);
    Log_d("running version: %s", running_version);

    /* 4. must report local version first */
    if (IOT_OTA_ReportVersion(ota_client, running_version) < 0) {
        Log_e("report OTA version failed");
        goto exit;
    }

    do {
        /* idle here until ota upgrate command received */
        Log_i("wait for ota upgrade command...");

        IOT_MQTT_Yield(mqtt_client, 200);

        HAL_SleepMs(2000);
    } while (!IOT_OTA_IsFetching(ota_client));

    /* 5. get remote firmware information */
    if (remote_firmware_info_get(ota_client, remote_version, remote_md5, &remote_fw_size) < 0) {
        Log_e("get remote firmware infomation failed\n");
        goto exit;
    }

    /* 6. compare remote and local firmware, check whether pre-download finished or not */
    if (!local_version || !local_md5 || !local_fw_size_str) {
        /* no previous downloaded firmware */
        offset = 0;
    } else if (strcmp(local_version, remote_version) != 0 ||
                strcmp(local_md5, remote_md5) != 0 ||
                local_fw_size > remote_fw_size) {
        /* there's a previous downloaded firmware, but not the same as current remote firmweare */
        offset = 0;
    } else {
        offset = local_fw_size;
    }

    /* if local_firmware offset does not equal to remote_fw_size, means we have not finish firmware download */
    if (offset != remote_fw_size &&
        remote_firmware_download(ota_client, mqtt_client, running_version, offset, remote_fw_size, &downloadeded_size) < 0) {
        goto exit;
    }

    printf("new firmware downloaded!\n");

#if 0
    /* begin execute OTA files, should report upgrade begin */
    sg_packet_id = IOT_OTA_ReportUpgradeBegin(ota_client);
    if (sg_packet_id < 0) {
        Log_e("report OTA begin failed error: %d", sg_packet_id);
        return QCLOUD_ERR_FAILURE;
    }

    while (!sg_pub_ack) {
        HAL_SleepMs(1000);
        IOT_MQTT_Yield(mqtt_client, 200);
    }
    sg_pub_ack = K_FALSE;

    /* add your own upgrade logic here*/
    // fw_upgrade.....

    if (QCLOUD_RET_SUCCESS == rc) {
        /* if upgrade success */
        /* after execute OTA files, should report upgrade result */
        sg_packet_id = IOT_OTA_ReportUpgradeSuccess(ota_client, NULL);
        if (sg_packet_id < 0) {
            Log_e("report OTA result failed error:%d", sg_packet_id);
            return QCLOUD_ERR_FAILURE;
        }
        while (!sg_pub_ack) {
            HAL_SleepMs(1000);
            IOT_MQTT_Yield(mqtt_client, 200);
        }
        rc = local_firmware_info_update(version, version, md5sum, size_downloaded); // just for example, add your own logic
        sg_pub_ack = K_FALSE;

    } else {
        /* if upgrade fail */
        sg_packet_id = IOT_OTA_ReportUpgradeFail(ota_client, NULL);
        if (0 > sg_packet_id) {
            Log_e("report OTA result failed error:%d", sg_packet_id);
            return QCLOUD_ERR_FAILURE;
        }
        while (!sg_pub_ack) {
            HAL_SleepMs(1000);
            IOT_MQTT_Yield(mqtt_client, 200);
        }
        rc = local_firmware_info_update(NULL, running_version, NULL, 0); // just for example, add your own logic
        sg_pub_ack = K_FALSE;
    }
#endif

exit:
    if (local_version) {
        HAL_Free(local_version);
        local_version = NULL;
    }

    if (local_md5) {
        HAL_Free(local_md5);
        local_md5 = NULL;
    }

    if (local_fw_size_str) {
        HAL_Free(local_fw_size_str);
        local_fw_size_str = NULL;
    }

    IOT_OTA_Destroy(ota_client);

    IOT_MQTT_Destroy(&mqtt_client);

    return 0;
}

