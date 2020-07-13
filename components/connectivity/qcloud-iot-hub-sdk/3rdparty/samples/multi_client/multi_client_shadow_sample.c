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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_getopt.h"

/*
 * This sample test multi shadow clients in multi-thread runtime.
 * 3 shadow clients run in each own thread
 * psk/cert_device_info1/2/3.json for each device info are required
 */

#define MAX_MQTT_THREAD_COUNT 3
// record the status of all the threadss
static unsigned int sg_thread_status[MAX_MQTT_THREAD_COUNT];

#ifdef WIN32
#ifdef AUTH_MODE_CERT
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {".\\cert_device_info1.json", ".\\cert_device_info2.json",
                                                 ".\\cert_device_info3.json"};
#else
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {".\\psk_device_info1.json", ".\\psk_device_info2.json",
                                                 ".\\psk_device_info3.json"};
#endif
#else
#ifdef AUTH_MODE_CERT
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {"./cert_device_info1.json", "./cert_device_info2.json",
                                                 "./cert_device_info3.json"};
#else
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {"./psk_device_info1.json", "./psk_device_info2.json",
                                                 "./psk_device_info3.json"};
#endif
#endif

// sample data structures
typedef struct SampleThreadData {
    int   thread_id;
    char *device_info_file;
    char *property_key;
} SampleThreadData;

static int sg_loop_cnt = 10;

void OnDeltaCallback(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty)
{
    Log_i("Thread recv delta str: %s", pJsonValueBuffer);
    pProperty->delta_arrived = true;
}

void OnShadowUpdateCallback(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument,
                            void *pUserdata)
{
    Log_i("Thread recv shadow update response ack: %d", requestAck);
}

static int _report_desire_null(void *handle, char *jsonBuffer, size_t sizeOfBuffer)
{
    /* device data updated, desire should be set null */
    int rc = IOT_Shadow_JSON_ConstructDesireAllNull(handle, jsonBuffer, sizeOfBuffer);
    if (rc == QCLOUD_RET_SUCCESS) {
        rc = IOT_Shadow_Update(handle, jsonBuffer, sizeOfBuffer, OnShadowUpdateCallback, "desire_null",
                               QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
        if (rc == QCLOUD_RET_SUCCESS) {
            Log_d("shadow update(desired) success");
        } else {
            Log_e("shadow update(desired) failed, err: %d", rc);
        }

    } else {
        Log_e("construct desire failed, err: %d", rc);
    }

    return rc;
}

static int _setup_connect_init_params(ShadowInitParams *initParams, DeviceInfo *device_info)
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
    initParams->auto_connect_enable    = 1;

    return QCLOUD_RET_SUCCESS;
}

static uint32_t _get_random_delay(void)
{
    srand((unsigned)HAL_GetTimeMs());
    /* range: 1000 - 5000 ms, in 10ms unit */
    return (rand() % 400 + 100) * 10;
}

static void _shadow_client_thread_runner(void *ptr)
{
    int   rc            = QCLOUD_ERR_FAILURE;
    void *shadow_client = NULL;

    SampleThreadData *thread_data = (SampleThreadData *)ptr;
    int               thread_id   = thread_data->thread_id;

    DeviceInfo dev_info = {0};
    if (HAL_GetDevInfoFromFile(thread_data->device_info_file, (void *)&dev_info)) {
        Log_e("invalid dev info file: %s", thread_data->device_info_file);
        goto thread_exit;
    }

    Log_i("Thread running. ID: %d; device: %s", thread_id, thread_data->device_info_file);

    char           shadow_json_buffer[200];
    size_t         shadow_json_buf_size = sizeof(shadow_json_buffer) / sizeof(shadow_json_buffer[0]);
    DeviceProperty shadow_property;
    int            current_update_count = 0;

    // init connection
    ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;
    rc                           = _setup_connect_init_params(&init_params, &dev_info);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params err,rc=%d", rc);
        goto thread_exit;
    }

    shadow_client = IOT_Shadow_Construct(&init_params);
    if (shadow_client == NULL) {
        Log_e("shadow client constructed failed.");
        goto thread_exit;
    }

    // register delta property
    shadow_property.key  = thread_data->property_key;
    shadow_property.data = &current_update_count;
    shadow_property.type = JINT32;
    rc                   = IOT_Shadow_Register_Property(shadow_client, &shadow_property, OnDeltaCallback);
    if (rc != QCLOUD_RET_SUCCESS) {
        rc = IOT_Shadow_Destroy(shadow_client);
        Log_e("register device shadow property failed, err: %d", rc);
        goto thread_exit;
    }

    // do get and sync operation before update
    rc = IOT_Shadow_Get_Sync(shadow_client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("get device shadow failed, err: %d", rc);
        goto thread_exit;
    }

    int loop_cnt = 0;
    while (IOT_Shadow_IsConnected(shadow_client) || QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc ||
           QCLOUD_RET_MQTT_RECONNECTED == rc || QCLOUD_RET_SUCCESS == rc) {
        rc = IOT_Shadow_Yield(shadow_client, 500);

        if (QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }

        if (shadow_property.delta_arrived) {
            _report_desire_null(shadow_client, shadow_json_buffer, shadow_json_buf_size);

            shadow_property.delta_arrived = false;
        }

        IOT_Shadow_JSON_ConstructReport(shadow_client, shadow_json_buffer, shadow_json_buf_size, 1, &shadow_property);
        rc = IOT_Shadow_Update(shadow_client, shadow_json_buffer, shadow_json_buf_size, OnShadowUpdateCallback, NULL,
                               QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
        current_update_count++;

        if (loop_cnt++ >= sg_loop_cnt)
            break;

        // sleep for some time
        HAL_SleepMs(_get_random_delay());
    }

thread_exit:

    if (shadow_client != NULL) {
        IOT_Shadow_Destroy(shadow_client);
        shadow_client = NULL;
    }

    sg_thread_status[thread_id] = 1;
}

static int parse_arguments(int argc, char **argv)
{
    int c;
    while ((c = utils_getopt(argc, argv, "l:")) != EOF) switch (c) {
            case 'l':
                sg_loop_cnt = atoi(utils_optarg);
                break;

            default:
                HAL_Printf(
                    "usage: %s [options]\n"
                    "  [-l n] test loop count\n",
                    argv[0]);
                return -1;
        }
    return 0;
}

int main(int argc, char **argv)
{
    // init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    parse_arguments(argc, argv);

    SampleThreadData thread_data[MAX_MQTT_THREAD_COUNT];

    char *property_key[MAX_MQTT_THREAD_COUNT] = {"update_count_1", "update_count_2", "update_count_3"};

    int i;
    for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
        sg_thread_status[i]             = 0;  // init thread status flag
        thread_data[i].property_key     = property_key[i];
        thread_data[i].device_info_file = device_info_file[i];
        thread_data[i].thread_id        = i;
    }

    int created_thread_cnt = 0;
    /* create multi threads for multi mqtt client test */
    for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
        sg_thread_status[i] = 0;  // init thread status flag

        ThreadParams thread_params = {0};
        thread_params.thread_func  = _shadow_client_thread_runner;
        thread_params.user_arg     = &thread_data[i];

        int rc = HAL_ThreadCreate(&thread_params);
        if (rc) {
            Log_e("create mqtt thread fail: %d", rc);
        } else {
            created_thread_cnt++;
        }

        HAL_SleepMs(300);
    }

    Log_i("created %d shadow threads", created_thread_cnt);

    /* wait for all threads to finish their jobs */
    int finished_created_thread_cnt;
    do {
        finished_created_thread_cnt = 0;
        for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
            finished_created_thread_cnt += sg_thread_status[i];
        }
        Log_i(">>>>>>>>Finished thread count : %d", finished_created_thread_cnt);

        HAL_SleepMs(1000);
    } while (finished_created_thread_cnt < created_thread_cnt);

    return 0;
}
